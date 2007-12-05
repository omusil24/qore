/*
 QC_QMoveEvent.cc
 
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

#include "QC_QMoveEvent.h"
#include "QC_QRegion.h"
#include "QC_QRect.h"

int CID_QMOVEEVENT;

class QoreClass *QC_QMoveEvent = 0;

static void QMOVEEVENT_constructor(class QoreObject *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *pos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QMOVEEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QPoint object as first argument to QMoveEvent::constructor()");
      return;
   }
   ReferenceHolder<QoreQPoint> holder(pos, xsink);
   p = get_param(params, 1);
   QoreQPoint *oldPos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!oldPos) {
      if (!xsink->isException())
         xsink->raiseException("QMOVEEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QPoint object as second argument to QMoveEvent::constructor()");
      return;
   }
   ReferenceHolder<QoreQPoint> holder2(oldPos, xsink);

   QoreQMoveEvent *qme = new QoreQMoveEvent(*(static_cast<QPoint *>(pos)), *(static_cast<QPoint *>(oldPos)));
   self->setPrivate(CID_QMOVEEVENT, qme);
}

static void QMOVEEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQMoveEvent *qr, ExceptionSink *xsink)
{
   xsink->raiseException("QMOVEEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//const QPoint & oldPos () const
static QoreNode *QMOVEEVENT_oldPos(QoreObject *self, QoreQMoveEvent *qme, QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qme->oldPos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//const QPoint & pos () const
static QoreNode *QMOVEEVENT_pos(QoreObject *self, QoreQMoveEvent *qme, QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qme->pos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

class QoreClass *initQMoveEventClass(class QoreClass *qevent)
{
   tracein("initQMoveEventClass()");
   
   QC_QMoveEvent = new QoreClass("QMoveEvent", QDOM_GUI);
   CID_QMOVEEVENT = QC_QMoveEvent->getID();

   QC_QMoveEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QMoveEvent->setConstructor(QMOVEEVENT_constructor);
   QC_QMoveEvent->setCopy((q_copy_t)QMOVEEVENT_copy);

   QC_QMoveEvent->addMethod("oldPos",                      (q_method_t)QMOVEEVENT_oldPos);
   QC_QMoveEvent->addMethod("pos",                         (q_method_t)QMOVEEVENT_pos);

   traceout("initQMoveEventClass()");
   return QC_QMoveEvent;
}
