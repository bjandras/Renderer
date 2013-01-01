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

#include <QLineEdit>
#include <QStyleOptionFrameV3>
#include <QTimer>


class QIcon;
class QLabel;


namespace Objavi {

    class AddressBar
        : public QLineEdit
    {
        Q_OBJECT;

    public:
        AddressBar(QWidget * parent = NULL);

        void setPageIcon(QIcon const &);

    public Q_SLOTS:
        void setProgress(int progress);

    private Q_SLOTS:
        void reset();

    protected:
        virtual void paintEvent(QPaintEvent*);
        virtual void resizeEvent(QResizeEvent*);

    private:
        void updateInternalGeometry();

        int m_progress;
        QTimer m_clearTimer;
        QLabel * m_pageIconLabel;
    };

}
