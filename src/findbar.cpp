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

#include "findbar.hpp"

#include <QWebPage>
#include <QLineEdit>
#include <QCheckBox>
#include <QToolButton>
#include <QSignalMapper>


namespace Objavi {

    FindBar::FindBar(QString const & title, QWidget * parent)
        : QToolBar(title, parent)
        , m_edit(NULL)
    {
        m_edit = new QLineEdit(this);
        m_edit->setMaximumWidth(200);

        QToolButton * done = new QToolButton(this);
        done->setText(tr("done"));
        QToolButton * prev = new QToolButton(this);
        prev->setArrowType(Qt::LeftArrow);
        QToolButton * next = new QToolButton(this);
        next->setArrowType(Qt::RightArrow);

        QCheckBox * matc = new QCheckBox(tr("Match Case"), this);
        QCheckBox * wrap = new QCheckBox(tr("Wrap Around"), this);
        QCheckBox * high = new QCheckBox(tr("HighLight All"), this);

        QSignalMapper * mapper = new QSignalMapper(this);
        mapper->setMapping(m_edit, 0);
        mapper->setMapping(next, 0);
        mapper->setMapping(prev, QWebPage::FindBackward);
        mapper->setMapping(matc, QWebPage::FindCaseSensitively);
        mapper->setMapping(wrap, QWebPage::FindWrapsAroundDocument);
        mapper->setMapping(high, QWebPage::HighlightAllOccurrences);

        connect(done,   SIGNAL(clicked()),   this, SLOT(hide()));
        connect(mapper, SIGNAL(mapped(int)), this, SLOT(find(int)));
        connect(m_edit, SIGNAL(textChanged(QString)), mapper, SLOT(map()));
        connect(prev,   SIGNAL(pressed()),         mapper, SLOT(map()));
        connect(next,   SIGNAL(pressed()),         mapper, SLOT(map()));
        connect(matc,   SIGNAL(stateChanged(int)), mapper, SLOT(map()));
        connect(wrap,   SIGNAL(stateChanged(int)), mapper, SLOT(map()));
        connect(high,   SIGNAL(stateChanged(int)), mapper, SLOT(map()));

        addWidget(done);
        addWidget(m_edit);
        addWidget(prev);
        addWidget(next);
        addWidget(matc);
        addWidget(wrap);
        addWidget(high);
    }


    void FindBar::show(QString const & text)
    {
        setVisible(true);
        m_edit->setText(text);
        m_edit->setFocus(Qt::PopupFocusReason);
    }


    void FindBar::setFound(bool found)
    {
        QPalette palette;

        if (found)
        {
            palette.setColor(m_edit->backgroundRole(), Qt::white);
        }
        else
        {
            palette.setColor(m_edit->backgroundRole(), QColor(255, 0, 0, 127));
        }

        m_edit->setPalette(palette);
    }


    void FindBar::find(int mode = 0)
    {
        Q_EMIT find(m_edit->text(), mode);
    }

}
