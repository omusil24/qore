/*
 QC_QFrame.h
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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

#ifndef _QORE_QC_QFRAME_H

#define _QORE_QC_QFRAME_H

#include "QoreAbstractQWidget.h"

#include <QFrame>

DLLEXPORT extern int CID_QFRAME;

DLLLOCAL class QoreClass *initQFrameClass(class QoreClass *parent);

class QoreQFrame : public QoreAbstractQWidget
{
   public:
      QPointer<QFrame>qobj;

      DLLLOCAL QoreQFrame(QWidget *parent = 0, Qt::WindowFlags window_flags = 0) : qobj(new QFrame(parent, window_flags))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
	 return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual QWidget *getQWidget() const
      {
	 return static_cast<QWidget *>(&(*qobj));
      }
};

/*
template<typename T>
static QoreNode *QW_setFont(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQFont *qf = p ? (QoreQFont *)p->val.object->getReferencedPrivateData(CID_QFONT, xsink) : NULL;
   if (!p || !qf)
   {
      if (!xsink->isException())
         xsink->raiseException("QFRAME-SETFONT-PARAM-EXCEPTION", "expecting a QFont object as parameter to QFrame::setFont()");
      return NULL;
   }
   ReferenceHolder<QoreQFont> holder(qf, xsink);

   qw->qobj->setFont(*((QFont *)qf));
   return 0;
}
*/

#endif
