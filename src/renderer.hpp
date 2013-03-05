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

#include "bookjs.hpp"

#include <QObject>
#include <QString>
#include <QTimer>
#include <QUrl>

#include <QWebPage>


namespace Objavi {
    
    class Renderer
        : public QObject
    {
        Q_OBJECT;

    public:
        struct Options
        {
            QString outputFilePath;
            QString bookjsPath;
            QString customCSS;
            BookJS::PaginationConfig paginationConfig;
            quint32 printTimeout;

            Options()
            {
                printTimeout = 10000;
            }
        };

    public:
        Renderer(QUrl const & url, Options const & options, QObject * parent = NULL);

        QString outputFilePath() const { return m_options.outputFilePath; }

    Q_SIGNALS:
        void finished(bool success);

    private:
        Options m_options;
        QTimer m_printTimer;
        QWebPage * m_webPage;

        void start(QUrl const & url);
        void printPage();

    private Q_SLOTS:
        void webPageLoadFinished(bool ok);
        void webPagePrintRequested(QWebFrame * frame);
        void printRequestTimeout();
    };

}
