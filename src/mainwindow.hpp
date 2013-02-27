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
#pragma once

#include "types.hpp"

#include <QMainWindow>
#include <QVector>


class QWebPage;
class QWebView;
class QWebFrame;
class QAction;
class QNetworkRequest;


namespace Objavi {

    class MainWindow
        : public QMainWindow
    {
        Q_OBJECT;

    public:
        MainWindow();

        void load(QString const & url);
        void load(QUrl const & url);

        QWebPage * page() const { return m_page; }
        QWebView * view() const { return m_view; }

    private:
        QWebPage * m_page;
        QWebView * m_view;
        AddressBar * m_addressBar;

        FindBar * m_findBar;
        int m_findFlag;

        QVector<int> m_zoomLevels;
        int m_currentZoom;

    private:
        void setupUi();

    private Q_SLOTS:
        void setAddressUrl(QString const & url);
        void setAddressUrl(QUrl const & url);
        void openFile();
        void openLocation();
        void loadAddressUrl();
        void onTitleChanged(QString const & title);
        void onIconChanged();
        void onLoadStarted();
        void onLoadFinished(bool);
        void onLinkHovered(QString const & link, QString const & title, QString const & textContent);

        void zoom(int d);
        void toggleZoomTextOnly(bool on);

        void newWindow();
        void print();

        void setEditable(bool on);

        void showFindBar();
        void find(QString const & text, int mode);

        void selectElements();

        void onDownloadRequested(QNetworkRequest const &);
        void onDownloadFinished();

        void installBookJS();
    };

}
