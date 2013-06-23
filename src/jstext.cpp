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

#include "jstext.hpp"

#include <QString>


namespace Objavi { namespace JsText {

   QString format(QString const & string)
   {
       QString result;

       for (int i = 0; i < string.length(); ++i)
       {
           QChar c = string[i];

           switch (c.toLatin1())
           {
           case '\'':
               result.append("\\'");
               break;

           case '\\':
               result.append("\\\\");
               break;

           default:
               result.append(c);
           }
       }

       return QString("'%1'").arg(result);
   }


   QString format(double value)
   {
       return QString::number(value);
   }

} }
