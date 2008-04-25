/*
 QC_QTextLine.h
 
  Qore Programming Language

 Copyright 2003 - 2008 David Nichols

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 */

#ifndef _QORE_QT_QC_QTEXTLINE_H

#define _QORE_QT_QC_QTEXTLINE_H

#include <QTextLine>

DLLLOCAL extern qore_classid_t CID_QTEXTLINE;
DLLLOCAL extern QoreClass *QC_QTextLine;
DLLLOCAL QoreNamespace *initQTextLineNS();

class QoreQTextLine : public AbstractPrivateData
{
   private:
      QTextLine line;
      
   public:
      DLLLOCAL QoreQTextLine()
      {
      }
      DLLLOCAL QoreQTextLine(const QTextLine &n_line) : line(n_line)
      {
      }
      DLLLOCAL QTextLine *getQTextLine()
      {
	 return &line;
      }

};

#endif // _QORE_QT_QC_QTEXTLINE_H
