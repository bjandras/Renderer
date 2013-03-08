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


    void populateLinkAnnotationDict(Dict * dict, XRef * xref, QRect const & rect)
    {
        Object tmp;

        Object typeObject;
        typeObject.initName("Annot");

        Object subtypeObject;
        subtypeObject.initName("Link");

        Object rectObject;
        rectObject.initArray(xref);
        rectObject.arrayAdd(tmp.initReal(rect.bottom()));
        rectObject.arrayAdd(tmp.initReal(rect.left()));
        rectObject.arrayAdd(tmp.initReal(rect.top()));
        rectObject.arrayAdd(tmp.initReal(rect.right()));
        tmp.free();
        
        dict->set("Type", &typeObject);
        dict->set("Sybtype", &subtypeObject);
        dict->set("Rect", &rectObject);
    }


    AnnotLink * makeLinkAnnotation(XRef * xref, Catalog * catalog, QRect const & rect)
    {
        Object * annotObject = new Object();
        annotObject->initDict(xref);
        xref->addIndirectObject(annotObject);

        Dict * dict = annotObject->getDict();
        populateLinkAnnotationDict(dict, xref, rect);
        
        AnnotLink * annot = new AnnotLink(xref, dict, catalog, annotObject);
        return annot;
    }


    void addAnnotation(QWebElement const & element, Page * pdfPage, XRef * xref, Catalog * catalog)
    {
        QRect geom = element.geometry();

        PDFRectangle * rect = new PDFRectangle(geom.left(), geom.top(), geom.right(), geom.bottom());

        Annot * annot = new AnnotLink(xref, rect, catalog);

        pdfPage->addAnnot(annot, catalog);
    }

 
    void addAnnotations(QWebPage * page, QString const & pdfFilePath)
    {
        QWebFrame * frame = page->mainFrame();

        GooStringPtr inputPath(new GooString(pdfFilePath.toLatin1().data()));
        GooStringPtr outputPath(new GooString("annotated.pdf"));

        PDFDocPtr doc(new PDFDoc(inputPath.data()));

        Catalog * catalog = doc->getCatalog();
        XRef * xref = doc->getXRef();

        qDebug() << doc->isOk() << doc->getNumPages();

        QWebElementCollection links = frame->findAllElements("a");

        Q_FOREACH (QWebElement const & element, links)
        {
            addAnnotation(element, doc->getPage(1), xref, catalog);
        }

        PDFRectangle * rect = new PDFRectangle(10, 10, 150, 150);
#if 0
        AnnotText * annot = new AnnotText(xref, rect, catalog);
        annot->setLabel(new GooString("ASDFG"));
#endif
        //AnnotLink * annot = new AnnotLink(xref, rect, catalog);
        AnnotLink * annot = makeLinkAnnotation(xref, catalog, QRect(10, 10, 150, 150));
        qDebug() << annot->getAction();
        doc->getPage(1)->addAnnot(annot, catalog);

        doc->saveAs(outputPath.data(), writeStandard);
    }

}}
