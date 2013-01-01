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

#include "application.hpp"
#include "renderer.hpp"
#include "mainwindow.hpp"

#include <QUrl>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMetaEnum>
#include <QTextStream>
#include <QDebug>


namespace {
    
    QString takeOptionValue(QStringList * arguments, int index)
    {
        QString result;

        if (++index < arguments->count() && !arguments->at(index).startsWith("-"))
        {
            result = arguments->takeAt(index);
        }

        return result;
    }

    
    QUrl urlFromUserInput(QString input)
    {
        QFileInfo fi(input);
        
        if (fi.exists() && fi.isRelative())
        {
            input = fi.absoluteFilePath();
        }

        return QUrl::fromUserInput(input);
    }


    void initWebSettings()
    {
        QWebSettings::setMaximumPagesInCache(4);

        QWebSettings::setObjectCacheCapacities((16*1024*1024) / 8, (16*1024*1024) / 8, 16*1024*1024);

        QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
        QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
        QWebSettings::globalSettings()->setAttribute(QWebSettings::CSSRegionsEnabled, true);

        QWebSettings::enablePersistentStorage();
    }


    QString makeDefaultUrl()
    {
        QString default_file = QString("%1/%2").arg(QDir::homePath()).arg(QLatin1String("index.html"));

        if (QFile(default_file).exists())
        {
            return QString("file://%1").arg(default_file);
        }
        else
        {
            return QString();
        }
    }

} // namespace



namespace Objavi {

    Application::Application(int argc, char ** argv)
        : QApplication(argc, argv, QApplication::GuiServer)
        , m_gui(false)
    {
        setOrganizationName("Sourcefabric");
        setApplicationName("Objavi Renderer");
        setApplicationVersion("0.1");

        initWebSettings();
        parseArguments(arguments());

        QStringList urls = m_urls;

        if (urls.isEmpty())
        {
            urls.append(makeDefaultUrl());
        }

        if (m_gui)
        {
            m_window.reset(new MainWindow());
            m_window->load(urls[0]);
            m_window->show();
        }
        else
        {
            QString url = urls[0];

            QUrl qurl = urlFromUserInput(url);
            if (qurl.scheme().isEmpty())
            {
                qurl = QUrl("http://" + url + "/");
            }

            m_renderer.reset(new Objavi::Renderer(qurl, m_rendererOptions));
            QObject::connect(m_renderer.data(), SIGNAL(finished(bool)), this, SLOT(onRendererFinished(bool)));

            qDebug() << "rendering to: " << m_renderer->outputFilePath();
        }
    }


    void Application::onRendererFinished(bool success)
    {
        QCoreApplication::exit(success ? 0 : -1);
    }


    void Application::parseArguments(QStringList args)
    {
        QFileInfo program(args.at(0));
        QString programName("Objavi PDF Renderer");

        if (program.exists())
            programName = program.baseName();

        if (args.contains("-h") || args.contains("-help") || args.contains("--help"))
        {
            qDebug() << "Usage:" << programName.toLatin1().data()
                     << "[-gui]"
                     << "[-custom-css path]"
                     << "[-output path]"
                     << "URL";

            appQuit(0);
        }

        m_gui = args.contains("-gui");

        int cssPathIndex = args.indexOf("-custom-css");
        if (cssPathIndex != -1)
        {
            QString path = takeOptionValue(&args, cssPathIndex);

            if (! QFileInfo(path).isReadable())
            {
                appQuit(1, QString("file does not exist or is not readable: %1").arg(path));
            }

            QFile file(path);

            if (! file.open(QFile::ReadOnly))
            {
                appQuit(1, QString("cout not open file: %1").arg(path));
            }

            m_rendererOptions.customCSS = QTextStream(&file).readAll();
        }

        int outputPathIndex = args.indexOf("-output");
        if (outputPathIndex != -1)
            m_rendererOptions.outputFilePath = takeOptionValue(&args, outputPathIndex);

        int lastArg = args.lastIndexOf(QRegExp("^-.*"));
        m_urls = (lastArg != -1) ? args.mid(++lastArg) : args.mid(1);
    }


    void Application::appQuit(int exitCode, QString const & msg)
    {
        if (! msg.isEmpty())
        {
            if (exitCode > 0)
            {
                qCritical("ERROR: %s", msg.toLatin1().data());
            }
            else
            {
                qDebug("%s", msg.toLatin1().data());
            }
        }

        disconnect(this);
        
        ::exit(exitCode);
    }

} // namespace Objavi



int main(int argc, char ** argv)
{
    Objavi::Application app(argc, argv);

    return app.exec();
}
