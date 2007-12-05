/*
 QC_QInputMethodEvent.cc
 
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

#include <qore/Qore.h>

#include "QC_QInputMethodEvent.h"

int CID_QINPUTMETHODEVENT;
class QoreClass *QC_QInputMethodEvent = 0;

//QInputMethodEvent ()
////QInputMethodEvent ( const QString & preeditText, const QList<Attribute> & attributes )
////QInputMethodEvent ( const QInputMethodEvent & other )
static void QINPUTMETHODEVENT_constructor(QoreObject *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QINPUTMETHODEVENT, new QoreQInputMethodEvent());
      return;
   }

   xsink->raiseException("QINPUTMETHODEVENT-CONSTRUCTOR-PARAM-ERROR", "not implemented");

//   if (!p || p->type != NT_STRING) {
//      xsink->raiseException("QINPUTMETHODEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a string as first argument to QInputMethodEvent::constructor()");
//      return;
//   }
//   const char *preeditText = p->val.String->getBuffer();
//   p = get_param(params, 1);
//   ??? QList<Attribute> attributes = p;
//   self->setPrivate(CID_QINPUTMETHODEVENT, new QoreQInputMethodEvent(preeditText, attributes));
//   return;
}

static void QINPUTMETHODEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQInputMethodEvent *qime, ExceptionSink *xsink)
{
   xsink->raiseException("QINPUTMETHODEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

////const QList<Attribute> & attributes () const
//static QoreNode *QINPUTMETHODEVENT_attributes(QoreObject *self, QoreQInputMethodEvent *qime, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qime->attributes());
//}

//const QString & commitString () const
static QoreNode *QINPUTMETHODEVENT_commitString(QoreObject *self, QoreQInputMethodEvent *qime, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qime->commitString().toUtf8().data(), QCS_UTF8));
}

//const QString & preeditString () const
static QoreNode *QINPUTMETHODEVENT_preeditString(QoreObject *self, QoreQInputMethodEvent *qime, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qime->preeditString().toUtf8().data(), QCS_UTF8));
}

//int replacementLength () const
static QoreNode *QINPUTMETHODEVENT_replacementLength(QoreObject *self, QoreQInputMethodEvent *qime, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qime->replacementLength());
}

//int replacementStart () const
static QoreNode *QINPUTMETHODEVENT_replacementStart(QoreObject *self, QoreQInputMethodEvent *qime, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qime->replacementStart());
}

//void setCommitString ( const QString & commitString, int replaceFrom = 0, int replaceLength = 0 )
static QoreNode *QINPUTMETHODEVENT_setCommitString(QoreObject *self, QoreQInputMethodEvent *qime, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QINPUTMETHODEVENT-SETCOMMITSTRING-PARAM-ERROR", "expecting a string as first argument to QInputMethodEvent::setCommitString()");
      return 0;
   }
   const char *commitString = p->val.String->getBuffer();
   p = get_param(params, 1);
   int replaceFrom = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int replaceLength = p ? p->getAsInt() : 0;
   qime->setCommitString(commitString, replaceFrom, replaceLength);
   return 0;
}

QoreClass *initQInputMethodEventClass(QoreClass *qevent)
{
   QC_QInputMethodEvent = new QoreClass("QInputMethodEvent", QDOM_GUI);
   CID_QINPUTMETHODEVENT = QC_QInputMethodEvent->getID();

   QC_QInputMethodEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QInputMethodEvent->setConstructor(QINPUTMETHODEVENT_constructor);
   QC_QInputMethodEvent->setCopy((q_copy_t)QINPUTMETHODEVENT_copy);

   //QC_QInputMethodEvent->addMethod("attributes",                  (q_method_t)QINPUTMETHODEVENT_attributes);
   QC_QInputMethodEvent->addMethod("commitString",                (q_method_t)QINPUTMETHODEVENT_commitString);
   QC_QInputMethodEvent->addMethod("preeditString",               (q_method_t)QINPUTMETHODEVENT_preeditString);
   QC_QInputMethodEvent->addMethod("replacementLength",           (q_method_t)QINPUTMETHODEVENT_replacementLength);
   QC_QInputMethodEvent->addMethod("replacementStart",            (q_method_t)QINPUTMETHODEVENT_replacementStart);
   QC_QInputMethodEvent->addMethod("setCommitString",             (q_method_t)QINPUTMETHODEVENT_setCommitString);

   return QC_QInputMethodEvent;
}
