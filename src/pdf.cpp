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

#include "pdf.hpp"

#include <poppler/PDFDoc.h>
#include <poppler/Catalog.h>
#include <poppler/Annot.h>
#include <poppler/Page.h>
#include <poppler/XRef.h>
#include <poppler/Link.h>
#include <poppler/goo/GooString.h>

#include <QWebFrame>
#include <QWebPage>
#include <QWebElement>

#include <QScopedPointer>
#include <QString>
#include <QDebug>


namespace Objavi { namespace Pdf {

    typedef QScopedPointer<GooString> GooStringPtr;
    typedef QScopedPointer<PDFDoc> PDFDocPtr;


    class LinkAnnotation : public AnnotLink
    {
    public:
        LinkAnnotation(XRef * xref, PDFRectangle * rect, Catalog * catalog, char const * uri)
            : AnnotLink(xref, rect, catalog)
        {
            Object actionObject;
            actionObject.initDict(xref);

            GooString * uri_ = new GooString(uri);

            Object tmp;
            actionObject.dictSet((char*)"S", tmp.initName((char*)"URI"));
            actionObject.dictSet((char*)"URI", tmp.initString(uri_));

            action = LinkAction::parseAction(&actionObject, catalog->getBaseURI());

            annotObj.dictSet((char*)"A", &actionObject);
        }
    };

    AnnotLink * makeLinkAnnotation(PDFDoc & doc, QRect const & qrect, QString const & uri)
    {
        PDFRectangle * rect = new PDFRectangle(qrect.left(), qrect.top(), qrect.right(), qrect.bottom());
        return new LinkAnnotation(doc.getXRef(), rect, doc.getCatalog(), uri.toLatin1().data());
    }


    void printRect(PDFRectangle * rect)
    {
        qDebug() << rect->x1 << rect->y1 << rect->x2 << rect->y2;
    }


    void addAnnotation(QWebElement const & element, PDFDoc & doc)
    {
        QString urlText = element.attribute("href");
        QUrl url(urlText);

        if (url.isRelative() || url.isLocalFile())
        {
            return;
        }

        QRect rect = element.geometry();
        float pageSize = 796.0;

        int y = rect.y();
        int pageNum = y / pageSize;

        y = y % (int)pageSize;
        y = pageSize - y;
        rect.setY(y);

        qDebug() << rect << pageNum+1 << urlText;

        AnnotLink * annot = makeLinkAnnotation(doc, rect, urlText);

        Page * page = doc.getPage(pageNum+1);

        if (page != NULL)
        {
            page->addAnnot(annot, doc.getCatalog());
        }
    }

 
    void addAnnotations(QWebPage * page, QString const & pdfFilePath)
    {
        QWebFrame * frame = page->mainFrame();

        GooStringPtr inputPath(new GooString(pdfFilePath.toLatin1().data()));
        GooStringPtr outputPath(new GooString("annotated.pdf"));

        PDFDocPtr doc(new PDFDoc(inputPath.data()));

        QWebElementCollection links = frame->findAllElements("a");
        Q_FOREACH (QWebElement const & element, links)
        {
            addAnnotation(element, *doc);
        }

        AnnotLink * annot = makeLinkAnnotation(*doc, QRect(10, 10, 150, 150), "http://www.google.com");
        doc->getPage(1)->addAnnot(annot, doc->getCatalog());

        doc->saveAs(outputPath.data(), writeStandard);
    }

}}
