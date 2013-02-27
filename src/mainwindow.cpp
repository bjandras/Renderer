/*
 * This file is part of the Objavi Renderer.
 *
 * Copyright (C) 2013 Borko Jandras <borko.jandras@sourcefabric.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.hpp"
#include "addressbar.hpp"
#include "findbar.hpp"
#include "webpage.hpp"
#include "webinspector.hpp"
#include "bookjs.hpp"

#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QWebSettings>

#include <QNetworkReply>
#include <QNetworkRequest>

#include <QAction>
#include <QPrinter>
#include <QMenuBar>
#include <QToolBar>
#include <QSplitter>
#include <QStatusBar>
#include <QToolTip>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QApplication>

#include <QUrl>
#include <QDebug>
#include <QSignalMapper>


namespace Objavi {

    MainWindow::MainWindow()
        : m_page(NULL)
        , m_view(NULL)
        , m_addressBar(NULL)
        , m_findBar(NULL)
        , m_findFlag(0)
    {
        m_page = new WebPage(this);

        m_zoomLevels
            << 30 << 50 << 67 << 80 << 90
            << 100
            << 110 << 120 << 133 << 150 << 170 << 200 << 240 << 300;
        m_currentZoom = 100;

        setupUi();

        connect(page(), SIGNAL(downloadRequested(QNetworkRequest const &)), this, SLOT(onDownloadRequested(QNetworkRequest const &)));
    }


    void MainWindow::setupUi()
    {
        QSplitter * splitter = new QSplitter(Qt::Vertical, this);
        setCentralWidget(splitter);
        resize(800, 600);

        m_view = new QWebView(splitter);
        m_view->setPage(page());
        m_view->installEventFilter(this);

        WebInspector * inspector = new WebInspector();
        connect(this, SIGNAL(destroyed()), inspector, SLOT(deleteLater()));
        splitter->addWidget(inspector);
        inspector->setPage(page());
        inspector->hide();

        QToolBar * toolBar = addToolBar("Navigation");

        m_addressBar = new AddressBar(toolBar);
        m_addressBar->setSizePolicy(QSizePolicy::Expanding, m_addressBar->sizePolicy().verticalPolicy());

        toolBar->addAction(page()->action(QWebPage::Back));
        toolBar->addAction(page()->action(QWebPage::Forward));
        toolBar->addAction(page()->action(QWebPage::Reload));
        toolBar->addAction(page()->action(QWebPage::Stop));
        toolBar->addWidget(m_addressBar);

        connect(page()->mainFrame(), SIGNAL(urlChanged(QUrl)), this, SLOT(setAddressUrl(QUrl)));
        connect(page()->mainFrame(), SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
        connect(page()->mainFrame(), SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
        connect(page()->mainFrame(), SIGNAL(iconChanged()), this, SLOT(onIconChanged()));
        connect(page()->mainFrame(), SIGNAL(titleChanged(QString)), this, SLOT(onTitleChanged(QString)));
        connect(page()->action(QWebPage::Reload), SIGNAL(triggered()), this, SLOT(loadAddressUrl()));
        connect(page(), SIGNAL(windowCloseRequested()), this, SLOT(close()));
        connect(page(), SIGNAL(loadProgress(int)), m_addressBar, SLOT(setProgress(int)));
        connect(page(), SIGNAL(linkHovered(QString, QString, QString)), this, SLOT(onLinkHovered(QString, QString, QString)));
        connect(m_addressBar, SIGNAL(returnPressed()), SLOT(loadAddressUrl()));

        page()->action(QWebPage::Back)->setShortcut(QKeySequence::Back);
        page()->action(QWebPage::Stop)->setShortcut(Qt::Key_Escape);
        page()->action(QWebPage::Forward)->setShortcut(QKeySequence::Forward);
        page()->action(QWebPage::Reload)->setShortcut(QKeySequence::Refresh);
        page()->action(QWebPage::Undo)->setShortcut(QKeySequence::Undo);
        page()->action(QWebPage::Redo)->setShortcut(QKeySequence::Redo);
        page()->action(QWebPage::Cut)->setShortcut(QKeySequence::Cut);
        page()->action(QWebPage::Copy)->setShortcut(QKeySequence::Copy);
        page()->action(QWebPage::Paste)->setShortcut(QKeySequence::Paste);
        page()->action(QWebPage::SelectAll)->setShortcut(QKeySequence::SelectAll);
        page()->action(QWebPage::ToggleBold)->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
        page()->action(QWebPage::ToggleItalic)->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
        page()->action(QWebPage::ToggleUnderline)->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_U));

        // file
        //
        QMenu * fileMenu = menuBar()->addMenu("&File");
        fileMenu->addAction("New Window", this, SLOT(newWindow()), QKeySequence::New);
        fileMenu->addAction(tr("Open File..."), this, SLOT(openFile()), QKeySequence::Open);
        fileMenu->addAction(tr("Open Location..."), this, SLOT(openLocation()), QKeySequence(Qt::CTRL | Qt::Key_L));
        fileMenu->addAction("Close Window", this, SLOT(close()), QKeySequence::Close);
        fileMenu->addSeparator();
        fileMenu->addAction(tr("Print"), this, SLOT(print()), QKeySequence::Print);
        fileMenu->addSeparator();
        fileMenu->addAction("Quit", QApplication::instance(), SLOT(closeAllWindows()), QKeySequence(Qt::CTRL | Qt::Key_Q));

        // edit
        //
        QMenu * editMenu = menuBar()->addMenu("&Edit");
        editMenu->addAction(page()->action(QWebPage::Undo));
        editMenu->addAction(page()->action(QWebPage::Redo));
        editMenu->addSeparator();
        editMenu->addAction(page()->action(QWebPage::Cut));
        editMenu->addAction(page()->action(QWebPage::Copy));
        editMenu->addAction(page()->action(QWebPage::Paste));
        editMenu->addSeparator();
        editMenu->addAction("&Find", this, SLOT(showFindBar()), QKeySequence(Qt::CTRL | Qt::Key_F));
        editMenu->addSeparator();
        QAction * setEditable = editMenu->addAction("Set Editable", this, SLOT(setEditable(bool)));
        setEditable->setCheckable(true);

        // format
        //
        QMenu * formatMenu = new QMenu("F&ormat", this);
        QAction * formatMenuAction = menuBar()->addMenu(formatMenu);
        formatMenuAction->setVisible(false);
        connect(setEditable, SIGNAL(triggered(bool)), formatMenuAction, SLOT(setVisible(bool)));
        formatMenu->addAction(page()->action(QWebPage::ToggleBold));
        formatMenu->addAction(page()->action(QWebPage::ToggleItalic));
        formatMenu->addAction(page()->action(QWebPage::ToggleUnderline));
        QMenu * writingMenu = formatMenu->addMenu(tr("Writing Direction"));
        writingMenu->addAction(page()->action(QWebPage::SetTextDirectionDefault));
        writingMenu->addAction(page()->action(QWebPage::SetTextDirectionLeftToRight));
        writingMenu->addAction(page()->action(QWebPage::SetTextDirectionRightToLeft));

        QMenu * viewMenu = menuBar()->addMenu("&View");
        viewMenu->addAction(page()->action(QWebPage::Stop));
        viewMenu->addAction(page()->action(QWebPage::Reload));
        viewMenu->addSeparator();

        QSignalMapper * zoomSignalMapper = new QSignalMapper(this);
        QAction * zoomIn = viewMenu->addAction("Zoom &In", zoomSignalMapper, SLOT(map()), QKeySequence(Qt::CTRL | Qt::Key_Plus));
        QAction * zoomOut = viewMenu->addAction("Zoom &Out", zoomSignalMapper, SLOT(map()), QKeySequence(Qt::CTRL | Qt::Key_Minus));
        QAction * zoomReset = viewMenu->addAction("Reset Zoom", zoomSignalMapper, SLOT(map()), QKeySequence(Qt::CTRL | Qt::Key_0));
        QAction * zoomTextOnly = viewMenu->addAction("Zoom Text Only", this, SLOT(toggleZoomTextOnly(bool)));
        zoomTextOnly->setCheckable(true);
        zoomTextOnly->setChecked(false);

        zoomSignalMapper->setMapping(zoomIn,    1);
        zoomSignalMapper->setMapping(zoomOut,  -1);
        zoomSignalMapper->setMapping(zoomReset, 0);
        connect(zoomSignalMapper, SIGNAL(mapped(int)), this, SLOT(zoom(int)));

        // tools
        //
        QMenu * toolsMenu = menuBar()->addMenu("&Tools");
        toolsMenu->addAction("Select Elements...", this, SLOT(selectElements()));
        QAction * showInspectorAction = toolsMenu->addAction("Show Web Inspector", inspector, SLOT(setVisible(bool)), QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_I));
        showInspectorAction->setCheckable(true);
        showInspectorAction->connect(inspector, SIGNAL(visibleChanged(bool)), SLOT(setChecked(bool)));
        toolsMenu->addSeparator();
        toolsMenu->addAction("Install BookJS", this, SLOT(installBookJS()));

        // "find text" toolbar
        //
        m_findBar = new FindBar("Find", this);
        addToolBar(Qt::BottomToolBarArea, m_findBar);
        m_findBar->setMovable(false);
        m_findBar->setVisible(false);
        connect(m_findBar, SIGNAL(find(const QString &, int)), this, SLOT(find(const QString &, int)));
    }


    void MainWindow::load(QString const & url)
    {
        QFileInfo fi(url);

        if (fi.exists() && fi.isRelative())
        {
            load(QUrl::fromUserInput(fi.absoluteFilePath()));
        }
        else
        {
            QUrl qurl = QUrl::fromUserInput(url);

            if (qurl.scheme().isEmpty())
            {
                qurl = QUrl("http://" + url + "/");
            }

            load(qurl);
        }
    }

    void MainWindow::load(QUrl const & url)
    {
        if (url.isValid())
        {
            setAddressUrl(url);
            page()->mainFrame()->load(url);
        }
    }


    void MainWindow::setAddressUrl(QString const & url)
    {
        m_addressBar->setText(url);
    }

    void MainWindow::setAddressUrl(QUrl const & url)
    {
        setAddressUrl(url.toString(QUrl::RemoveUserInfo));
    }


    void MainWindow::loadAddressUrl()
    {
        QString addressUrl = m_addressBar->text();
        QUrl pageUrl = page()->mainFrame()->url();

        if (pageUrl.isValid() && pageUrl.toString() == addressUrl)
        {
            page()->triggerAction(QWebPage::Reload);
        }
        else
        {
            load(addressUrl);
        }
    }


    void MainWindow::openFile()
    {
        static QString const caption = tr("Open");
        static QString const filter = "HTML Files (*.html *.htm);;All Files (*)";
        static QString const dir;

        QString file = QFileDialog::getOpenFileName(this, caption, dir, filter);

        if (! file.isEmpty())
        {
            load(QUrl::fromLocalFile(file));
        }
    }


    void MainWindow::openLocation()
    {
        m_addressBar->selectAll();
        m_addressBar->setFocus();
    }


    void MainWindow::onTitleChanged(QString const & title)
    {
        QString windowTitle = QCoreApplication::applicationName();

        if (! title.isEmpty())
        {
            windowTitle = title + " - " + windowTitle;
        }

        setWindowTitle(windowTitle);
    }


    void MainWindow::onIconChanged()
    {
        m_addressBar->setPageIcon(page()->mainFrame()->icon());
    }


    void MainWindow::onLoadStarted()
    {
        m_addressBar->setPageIcon(QIcon());
        m_view->setFocus(Qt::OtherFocusReason);
    }


    void MainWindow::onLoadFinished(bool ok)
    {
        QUrl url = page()->mainFrame()->url();
        setAddressUrl(url.toString(QUrl::RemoveUserInfo));
    }

    void MainWindow::onLinkHovered(QString const & link, QString const & title, QString const & textContent)
    {
        statusBar()->showMessage(link);

        if (! title.isEmpty())
        {
            QToolTip::showText(QCursor::pos(), title);
        }
    }


    void MainWindow::zoom(int d)
    {
        if (d == 0)
        {
            m_currentZoom = 100;
        }
        else
        {
            int curIdx = m_zoomLevels.indexOf(m_currentZoom);
            Q_ASSERT(curIdx >= 0);

            int newIdx = curIdx + d;

            if (newIdx > 0 && newIdx < m_zoomLevels.count() - 1)
            {
                m_currentZoom = m_zoomLevels[newIdx];
            }
        }

        page()->mainFrame()->setZoomFactor(qreal(m_currentZoom) / 100.0);
    }

    void MainWindow::toggleZoomTextOnly(bool b)
    {
        page()->settings()->setAttribute(QWebSettings::ZoomTextOnly, b);
    }


    void MainWindow::newWindow()
    {
        MainWindow * window = new MainWindow();
        window->show();
    }


    void MainWindow::print()
    {
        page()->mainFrame()->evaluateJavaScript("window.print();");
    }


    void MainWindow::setEditable(bool on)
    {
        page()->setContentEditable(on);
    }


    void MainWindow::showFindBar()
    {
        if (m_findBar->isVisible())
        {
            m_findBar->setVisible(false);
        }
        else
        {
            m_findBar->show(page()->selectedText());
        }
    }

    void MainWindow::find(QString const & text, int mode)
    {
        page()->findText("", QFlag(QWebPage::HighlightAllOccurrences));

        m_findFlag ^= mode;

        if (mode == 0 || mode == QWebPage::FindBackward)
        {
            bool found = page()->findText(text, QFlag(m_findFlag & ~QWebPage::HighlightAllOccurrences));

            m_findFlag ^= mode;

            m_findBar->setFound(found || text.isEmpty());
        }

        if ((m_findFlag & QWebPage::HighlightAllOccurrences) != 0)
        {
            page()->findText(text, QFlag(m_findFlag));
        }
    }


    void MainWindow::selectElements()
    {
        bool ok = false;
        QString elementName = QInputDialog::getText(this, "Select elements", "Choose elements", QLineEdit::Normal, "h1", &ok);

        if (ok && !elementName.isEmpty())
        {
            QWebElementCollection elements = page()->mainFrame()->findAllElements(elementName);

            Q_FOREACH (QWebElement element, elements)
            {
                element.setStyleProperty("background-color", "yellow");
            }

            statusBar()->showMessage(QString("%1 element(s) selected").arg(elements.count()), 3000);
        }
    }


    void MainWindow::onDownloadRequested(QNetworkRequest const & request)
    {
        QNetworkAccessManager * manager = new QNetworkAccessManager(this);
        QNetworkReply * reply = manager->get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(onDownloadFinished()));
    }

    void MainWindow::onDownloadFinished()
    {
        QNetworkReply * reply = static_cast<QNetworkReply*>(sender());

        QString requestFileName = QFileInfo(reply->request().url().toString()).fileName();
        QString saveFilePath = QFileDialog::getSaveFileName(this, "Save as...", requestFileName);

        if (saveFilePath.isEmpty())
        {
            return;
        }

        if (reply->error() == QNetworkReply::NoError)
        {
            QFile file(saveFilePath);
            file.open(QIODevice::WriteOnly);
            file.write(reply->readAll());

            QMessageBox::information(this, QString("Download"), QString("%1 downloaded successfully.").arg(requestFileName));
        }
        else
        {
            QMessageBox::critical(this, QString("Download"), QString("Download failed."));
        }
    }


    void MainWindow::installBookJS()
    {
        Objavi::BookJS::install(page());
        qDebug() << "BookJS page size: " << Objavi::BookJS::getPageSize(page());
    }

}
