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

#include "renderer.hpp"
#include "bookjs.hpp"

#include <QWebFrame>
#include <QPrinter>
#include <QFileInfo>
#include <QDebug>


namespace Objavi {

    Renderer::Renderer(QUrl const & url, QString const & outputFilePath, QObject * parent)
        : QObject(parent)
    {
        m_options.outputFilePath = outputFilePath;
        start(url);
    }

    Renderer::Renderer(QUrl const & url, Options const & options, QObject * parent)
        : QObject(parent)
        , m_options(options)
    {
        start(url);
    }


    void Renderer::start(QUrl const & url)
    {
        m_webPage = new QWebPage(this);

        if (m_options.outputFilePath.isEmpty())
        {
            QFileInfo fileInfo(url.path());
            m_options.outputFilePath = fileInfo.baseName() + ".pdf";
        }
        
        connect(m_webPage, SIGNAL(loadFinished(bool)), this, SLOT(webPageLoadFinished(bool)), Qt::QueuedConnection);
        connect(m_webPage, SIGNAL(printRequested(QWebFrame *)), this, SLOT(webPagePrintRequested(QWebFrame *)), Qt::QueuedConnection);

        m_webPage->mainFrame()->load(url);
    }


    void Renderer::webPageLoadFinished(bool ok)
    {
        if (ok)
        {
            qDebug() << "web page loading finished";

            try
            {
                BookJS::install(m_webPage, m_options.bookjsPath, m_options.customCSS);
            }
            catch (std::exception & e)
            {
                qCritical() << "ERROR:" << e.what();
                Q_EMIT finished(false);
            }
        }
        else
        {
            qCritical() << "ERROR:" << "web page loading failed";
            Q_EMIT finished(false);
        }
    }


    void Renderer::webPagePrintRequested(QWebFrame *)
    {
        qDebug() << "web page print requested";
        printPage();
    }


    void Renderer::printPage()
    {
        QSizeF pageSize = BookJS::getPageSize(m_webPage);

        if (! pageSize.isValid())
        {
            qCritical() << "ERROR:" << "unable to obtain page size";
            Q_EMIT finished(false);
            return;
        }

        int const screenDpi = 96;
        int const paperDpi = 300;

        QSizeF paperSize = pageSize * 0.75 * paperDpi / screenDpi;

        qDebug() << "print starting";

        QPrinter printer;

        printer.setOutputFileName(m_options.outputFilePath);
        printer.setOutputFormat(QPrinter::PdfFormat);

        printer.setFullPage(true);
        printer.setResolution(paperDpi);
        printer.setPaperSize(paperSize, QPrinter::DevicePixel);

        m_webPage->mainFrame()->print(&printer);

        qDebug() << "print finished";
        Q_EMIT finished(true);
    }
    
} // namespace Objavi
