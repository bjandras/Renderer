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

#include "addressbar.hpp"

#include <QApplication>
#include <QLabel>
#include <QIcon>


namespace Objavi {

    namespace {
    
        static const QSize gPageIconSize(16, 16);

        QPixmap defaultPageIcon()
        {
            static QPixmap icon;

            if (icon.isNull())
            {
                icon.load(":/favicon.ico");
            }

            return icon;
        }

    }


    AddressBar::AddressBar(QWidget * parent)
        : QLineEdit(parent)
        , m_progress(0)
    {
        m_clearTimer.setSingleShot(true);
        connect(&m_clearTimer, SIGNAL(timeout()), this, SLOT(reset()));

        m_pageIconLabel = new QLabel(this);
        m_pageIconLabel->setFixedSize(gPageIconSize);
        m_pageIconLabel->setPixmap(defaultPageIcon());
    }

    void AddressBar::setPageIcon(QIcon const & icon)
    {
        if (icon.isNull())
            m_pageIconLabel->setPixmap(defaultPageIcon());
        else
            m_pageIconLabel->setPixmap(icon.pixmap(gPageIconSize));
    }

    void AddressBar::setProgress(int progress)
    {
        m_clearTimer.stop();
        m_progress = progress;
        update();
    }

    void AddressBar::reset()
    {
        setProgress(0);
    }

    void AddressBar::resizeEvent(QResizeEvent*)
    {
        updateInternalGeometry();
    }

    void AddressBar::updateInternalGeometry()
    {
        QStyleOptionFrameV3 styleOption;
        initStyleOption(&styleOption);

        QRect textRect = style()->subElementRect(QStyle::SE_LineEditContents, &styleOption, this);

        const int spacing = 2;

        int x = textRect.x() + spacing;
        int y = (textRect.center().y() + 1) - gPageIconSize.height() / 2;

        m_pageIconLabel->move(x, y);

        QMargins margins = textMargins();
        margins.setLeft(m_pageIconLabel->sizeHint().width() + spacing);
        setTextMargins(margins);
    }

    void AddressBar::paintEvent(QPaintEvent* ev)
    {
        QColor backgroundColor = QApplication::palette().color(QPalette::Base);
        QColor progressColor = QColor(120, 180, 240);
        QPalette p = palette();

        if (!m_progress)
            p.setBrush(QPalette::Base, backgroundColor);
        else {
            QLinearGradient gradient(0, 0, width(), 0);
            gradient.setColorAt(0, progressColor);
            gradient.setColorAt(((double) m_progress) / 100, progressColor);
            if (m_progress != 100)
                gradient.setColorAt((double) m_progress / 100 + 0.001, backgroundColor);
            p.setBrush(QPalette::Base, gradient);
        }
        setPalette(p);

        QLineEdit::paintEvent(ev);

        if (m_progress == 100)
            m_clearTimer.start(100);
    }

}
