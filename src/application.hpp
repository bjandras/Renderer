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

#include "mainwindow.hpp"
#include "renderer.hpp"
#include "types.hpp"

#include <QApplication>
#include <QScopedPointer>
#include <QStringList>
#include <QString>


namespace Objavi {

    class Application
        : public QApplication
    {
        Q_OBJECT;

    public:
        Application(int argc, char ** argv);

    private:
        QScopedPointer<MainWindow> m_window;
        QScopedPointer<Renderer> m_renderer;

        Renderer::Options m_rendererOptions;
        QStringList m_args;
        QStringList m_urls;
        bool m_gui;

        void parseArguments(QStringList args);

        void appQuit(int exitCode, QString const & msg = QString()) __attribute__((noreturn));

    private Q_SLOTS:
        void start();
        void onRendererFinished(bool success);
    };
    
} // namespace Objavi
