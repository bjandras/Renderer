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

#include "bookjs.hpp"
#include "jstext.hpp"

#include <QDir>
#include <QFile>
#include <QString>
#include <QVariant>
#include <QTextStream>
#include <QDebug>

#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>

#include <stdexcept>


namespace Objavi { namespace BookJS {

    PaginationConfig::PaginationConfig()
    {}

    PaginationConfig::PaginationConfig(QList<QPair<QString,QVariant> > const & items)
    {
        typedef QPair<QString,QVariant> ItemType;

        Q_FOREACH (ItemType const & item, items)
        {
            m_map[item.first] = item.second;
        }
    }


    PaginationConfig PaginationConfig::parse(QString const & text)
    {
        typedef QPair<QString,QVariant> ItemType;

        QList<ItemType> items;

        Q_FOREACH (QString item, text.split(','))
        {
            QStringList pair = item.split(':');

            if (pair.length() < 2) continue;

            QString key = pair[0].trimmed();
            QString val = pair[1].trimmed();

            QVariant value;

            if (key == "lengthUnit")
            {
                if (val == "'em'" || val == "\"em\"" ||
                    val == "'cm'" || val == "\"cm\"" ||
                    val == "'mm'" || val == "\"mm\"" ||
                    val == "'in'" || val == "\"in\"" ||
                    val == "'pt'" || val == "\"pt\"" ||
                    val == "'pc'" || val == "\"pc\"" ||
                    val == "'px'" || val == "\"px\"" ||
                    val == "'ex'" || val == "\"ex\"" ||
                    val == "'%'"  || val == "\"%\"")
                {
                    value = val;
                }
            }
            else
            {
                bool ok = false;
                double d = val.toDouble(&ok);

                if (ok)
                {
                    value = d;
                }
            }

            if (value.isValid())
            {
                items.append(ItemType(key, value));
            }
            else
            {
                QString msg = QString("invalid value for option %1: %2").arg(key).arg(val);
                throw std::runtime_error(msg.toLatin1().data());
            }
        }

        return PaginationConfig(items);
    }


    namespace {

        QSizeF makeSize(QString width, QString height)
        {
            if (width.endsWith("px") && height.endsWith("px"))
            {
                width  = width.left(width.size() - 2);
                height = height.left(height.size() - 2);
                    
                return QSizeF(width.toFloat(), height.toFloat());
            }
            else
            {
                return QSizeF();
            }
        }
        
    }

    
    QSizeF getPageSize(QWebPage const * page, QString className)
    {
        QWebElement document = page->mainFrame()->documentElement();
        
        QWebElement element = document.findFirst(QString(".%1").arg(className));
        if (! element.isNull())
        {
            QString width  = element.styleProperty("width",  QWebElement::ComputedStyle);
            QString height = element.styleProperty("height", QWebElement::ComputedStyle);

            return makeSize(width, height);
        }
        
        QString script_text = 
            "(function(className) {"
            "    var style = getComputedStyle(document.getElementsByClassName(className)[0], className);"
            "    return [style.width, style.height];"
            "})('%1');";

        script_text = script_text.arg(className);
        
        QVariant result = document.evaluateJavaScript(script_text);

        if (result.canConvert(QVariant::StringList))
        {
            QStringList list = result.toStringList();

            return makeSize(list[0], list[1]);
        }
        
        return QSizeF();
    }


    namespace {

        QString loadFile(QString path)
        {
            QFile file(path);

            if (! file.open(QFile::ReadOnly))
            {
                QString message = QString("could not open file %1").arg(path);

                throw std::runtime_error(message.toStdString());
            }

            return QTextStream(&file).readAll();
        }

        
        void appendCSS(QString text, QWebElement & element)
        {
            QString xml = element.toInnerXml();

            xml += "<style>";
            xml += text;
            xml += "</style>";

            element.setInnerXml(xml);
        }
        
        void appendJavascript(QString text, QWebElement & element)
        {
            QString xml = element.toInnerXml();

            xml += "<script type=\"text/javascript\">";
            xml += text;
            xml += "</script>";

            element.setInnerXml(xml);
        }


        QString makeFrontMatterContents(QWebElement const & head)
        {
            QString title;
            QString subtitle;
            QString editors;
            QString license;
            QString copyright;

            for (QWebElement node = head.firstChild(); !node.isNull(); node = node.nextSibling())
            {
                QString name = node.localName();

                if (name == "title")
                {
                    title = node.toPlainText();
                }
                else if (name == "meta")
                {
                    QString name = node.attribute("name");
                    QString value = node.attribute("content");

                    if (name == "license")
                    {
                        license = value;
                    }
                    else if (name == "copyright")
                    {
                        copyright = value;
                    }
                }
            }

            return QString(
                "<div id=\"booktitle\">%1</div>"
                "<div id=\"booksubtitle\">%2</div>"
                "<div id=\"bookeditors\">%3</div>"
                "<div class=\"pagebreak\"></div>"
                "<div id=\"copyrightpage\">Copyright: %4<br>License: %5</div>"
                "<div class=\"pagebreak\"></div>")
                .arg(title)
                .arg(subtitle)
                .arg(editors)
                .arg(copyright).arg(license);
        }
        
    }
        
    void install(QWebPage * page, QString bookjsPath, PaginationConfig const & paginationConfig, QString customCSS)
    {
        QWebElement document = page->mainFrame()->documentElement();
        QWebElement head = document.findFirst("head");

        if (head.isNull())
        {
            throw std::runtime_error("no document head");
        }

        QDir bookjsDir = QDir::current();
        if (! bookjsPath.isEmpty())
        {
            bookjsDir = QDir(bookjsPath);
        }

        if (bookjsDir.exists() == false)
        {
            throw std::runtime_error("invalid BookJS path");
        }

        if (! customCSS.isEmpty())
        {
            appendCSS(customCSS, head);
        }

        QString script;

        // preset pagination configuration
        script += loadFile(bookjsDir.filePath("book-config.js"));

        // user-specified pagination configuration
        //
        for (PaginationConfig::MapType::ConstIterator it = paginationConfig.items().begin();
             it != paginationConfig.items().end(); ++it)
        {
            QString optionKey   = it.key();
            QString optionValue = it.value().toString();

            script += QString("\npaginationConfig.%1 = %2;").arg(optionKey).arg(optionValue);
        }

        // front matter configuration
        //
        QString frontmatterContents = makeFrontMatterContents(head);
        script += QString("\npaginationConfig.frontmatterContents = %1;\n").arg(JsText::format(frontmatterContents));

        // main BookJS script text
        script += loadFile(bookjsDir.filePath("book.js"));

        // apply
        script += "\nPagination.applyBookLayout();";

        page->mainFrame()->evaluateJavaScript(script);
    }

} }  // namespace Objavi::BookJS
