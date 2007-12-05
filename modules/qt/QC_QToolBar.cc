/*
 QC_QToolBar.cc
 
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

#include "QC_QToolBar.h"

int CID_QTOOLBAR;
class QoreClass *QC_QToolBar = 0;

//QToolBar ( const QString & title, QWidget * parent = 0 )
//QToolBar ( QWidget * parent = 0 )
static void QTOOLBAR_constructor(QoreObject *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QTOOLBAR, new QoreQToolBar(self));
      return;
   }
   QString title;
   if (!get_qstring(p, title, xsink, true)) {
      if (*xsink)
	 return;
      
      QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QTOOLBAR, new QoreQToolBar(self, parent ? parent->getQWidget() : 0));
      return;
   }
   p = get_param(params, 1);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QTOOLBAR, new QoreQToolBar(self, title, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QTOOLBAR_copy(class QoreObject *self, class QoreObject *old, class QoreQToolBar *qtb, ExceptionSink *xsink)
{
   xsink->raiseException("QTOOLBAR-COPY-ERROR", "objects of this class cannot be copied");
}

//QAction * actionAt ( const QPoint & p ) const
//QAction * actionAt ( int x, int y ) const
static QoreNode *QTOOLBAR_actionAt(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         if (!xsink->isException())
            xsink->raiseException("QTOOLBAR-ACTIONAT-PARAM-ERROR", "QToolBar::actionAt() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(point), xsink);
      return return_qaction(qtb->qobj->actionAt(*(static_cast<QPoint *>(point))));
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   return return_qaction(qtb->qobj->actionAt(x, y));
}

//void addAction ( QAction * action )
//QAction * addAction ( const QString & text )
//QAction * addAction ( const QIcon & icon, const QString & text )
//QAction * addAction ( const QString & text, const QObject * receiver, const char * member )
//QAction * addAction ( const QIcon & icon, const QString & text, const QObject * receiver, const char * member )
static QoreNode *QTOOLBAR_addAction(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   QString text;
   if (!get_qstring(p, text, xsink, true)) {
      if (num_params(params) == 1)
	 return return_qaction(qtb->qobj->addAction(text));

      p = get_param(params, 1);
      QoreAbstractQObject *receiver = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
      if (!receiver) {
	 if (!xsink->isException())
	    xsink->raiseException("QTOOLBAR-ADDACTION-PARAM-ERROR", "this version of QToolBar::addAction() expects an object derived from QObject as the third argument");
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);
      p = get_param(params, 2);
      if (!p || p->type != NT_STRING) {
	 xsink->raiseException("QTOOLBAR-ADDACTION-PARAM-ERROR", "expecting a string as fourth argument to QToolBar::addAction()");
	 return 0;
      }
      const char *member = p->val.String->getBuffer();
      return return_qaction(qtb->qobj->addAction(text, receiver->getQObject(), member));
   }
   if (*xsink)
      return 0;
   if (!p | p->type != NT_OBJECT) {
      xsink->raiseException("QTOOLBAR-ADDACTION-PARAM-ERROR", "QToolBar::addAction() does not know how to handle arguments of type '%s' as the first argument", p ? p->type->getName() : "NOTHING");
      return 0;
   }

   QoreQIcon *icon = (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink);
   if (!icon) {
      QoreQAction *action = (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink);
      if (!action) {
	 if (!xsink->isException())
	    xsink->raiseException("QTOOLBAR-ADDACTION-PARAM-ERROR", "QToolBar::addAction() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> actionHolder(static_cast<AbstractPrivateData *>(action), xsink);
      qtb->qobj->addAction(static_cast<QAction *>(action->getQAction()));
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
   p = get_param(params, 1);
   if (get_qstring(p, text, xsink))
      return 0;
   p = get_param(params, 2);
   QoreAbstractQObject *receiver = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!receiver) {
      if (!xsink->isException())
	 xsink->raiseException("QTOOLBAR-ADDACTION-PARAM-ERROR", "this version of QToolBar::addAction() expects an object derived from QObject as the third argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);
   p = get_param(params, 3);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QTOOLBAR-ADDACTION-PARAM-ERROR", "expecting a string as fourth argument to QToolBar::addAction()");
      return 0;
   }
   const char *member = p->val.String->getBuffer();
   return return_qaction(qtb->qobj->addAction(*(static_cast<QIcon *>(icon)), text, receiver->getQObject(), member));
}

//QAction * addSeparator ()
static QoreNode *QTOOLBAR_addSeparator(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   return return_qaction(qtb->qobj->addSeparator());
}

//QAction * addWidget ( QWidget * widget )
static QoreNode *QTOOLBAR_addWidget(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBAR-ADDWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QToolBar::addWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return return_qaction(qtb->qobj->addWidget(static_cast<QWidget *>(widget->getQWidget())));
}

//Qt::ToolBarAreas allowedAreas () const
static QoreNode *QTOOLBAR_allowedAreas(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtb->qobj->allowedAreas());
}

//void clear ()
static QoreNode *QTOOLBAR_clear(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   qtb->qobj->clear();
   return 0;
}

//QSize iconSize () const
static QoreNode *QTOOLBAR_iconSize(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qtb->qobj->iconSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//QAction * insertSeparator ( QAction * before )
static QoreNode *QTOOLBAR_insertSeparator(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQAction *before = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!before) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBAR-INSERTSEPARATOR-PARAM-ERROR", "expecting a QAction object as first argument to QToolBar::insertSeparator()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> beforeHolder(static_cast<AbstractPrivateData *>(before), xsink);
   return return_qaction(qtb->qobj->insertSeparator(static_cast<QAction *>(before->getQAction())));
}

//QAction * insertWidget ( QAction * before, QWidget * widget )
static QoreNode *QTOOLBAR_insertWidget(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQAction *before = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!before) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBAR-INSERTWIDGET-PARAM-ERROR", "expecting a QAction object as first argument to QToolBar::insertWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> beforeHolder(static_cast<AbstractPrivateData *>(before), xsink);
   p = get_param(params, 1);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBAR-INSERTWIDGET-PARAM-ERROR", "expecting a QWidget object as second argument to QToolBar::insertWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return return_qaction(qtb->qobj->insertWidget(static_cast<QAction *>(before->getQAction()), static_cast<QWidget *>(widget->getQWidget())));
}

//bool isAreaAllowed ( Qt::ToolBarArea area ) const
static QoreNode *QTOOLBAR_isAreaAllowed(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ToolBarArea area = (Qt::ToolBarArea)(p ? p->getAsInt() : 0);
   return new QoreNode(qtb->qobj->isAreaAllowed(area));
}

//bool isFloatable () const
static QoreNode *QTOOLBAR_isFloatable(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qtb->qobj->isFloatable());
}

//bool isFloating () const
static QoreNode *QTOOLBAR_isFloating(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qtb->qobj->isFloating());
}

//bool isMovable () const
static QoreNode *QTOOLBAR_isMovable(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qtb->qobj->isMovable());
}

//Qt::Orientation orientation () const
static QoreNode *QTOOLBAR_orientation(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtb->qobj->orientation());
}

//void setAllowedAreas ( Qt::ToolBarAreas areas )
static QoreNode *QTOOLBAR_setAllowedAreas(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ToolBarAreas areas = (Qt::ToolBarAreas)(p ? p->getAsInt() : 0);
   qtb->qobj->setAllowedAreas(areas);
   return 0;
}

//void setFloatable ( bool floatable )
static QoreNode *QTOOLBAR_setFloatable(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool floatable = p ? p->getAsBool() : false;
   qtb->qobj->setFloatable(floatable);
   return 0;
}

//void setMovable ( bool movable )
static QoreNode *QTOOLBAR_setMovable(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool movable = p ? p->getAsBool() : false;
   qtb->qobj->setMovable(movable);
   return 0;
}

//void setOrientation ( Qt::Orientation orientation )
static QoreNode *QTOOLBAR_setOrientation(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   qtb->qobj->setOrientation(orientation);
   return 0;
}

//QAction * toggleViewAction () const
static QoreNode *QTOOLBAR_toggleViewAction(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   return return_qaction(qtb->qobj->toggleViewAction());
}

//Qt::ToolButtonStyle toolButtonStyle () const
static QoreNode *QTOOLBAR_toolButtonStyle(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtb->qobj->toolButtonStyle());
}

//QWidget * widgetForAction ( QAction * action ) const
static QoreNode *QTOOLBAR_widgetForAction(QoreObject *self, QoreQToolBar *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQAction *action = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!action) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBAR-WIDGETFORACTION-PARAM-ERROR", "expecting a QAction object as first argument to QToolBar::widgetForAction()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> actionHolder(static_cast<AbstractPrivateData *>(action), xsink);
   return return_qwidget(qtb->qobj->widgetForAction(static_cast<QAction *>(action->getQAction())));
}

static QoreClass *initQToolBarClass(QoreClass *qwidget)
{
   QC_QToolBar = new QoreClass("QToolBar", QDOM_GUI);
   CID_QTOOLBAR = QC_QToolBar->getID();

   QC_QToolBar->addBuiltinVirtualBaseClass(qwidget);

   QC_QToolBar->setConstructor(QTOOLBAR_constructor);
   QC_QToolBar->setCopy((q_copy_t)QTOOLBAR_copy);

   QC_QToolBar->addMethod("actionAt",                    (q_method_t)QTOOLBAR_actionAt);
   QC_QToolBar->addMethod("addAction",                   (q_method_t)QTOOLBAR_addAction);
   QC_QToolBar->addMethod("addSeparator",                (q_method_t)QTOOLBAR_addSeparator);
   QC_QToolBar->addMethod("addWidget",                   (q_method_t)QTOOLBAR_addWidget);
   QC_QToolBar->addMethod("allowedAreas",                (q_method_t)QTOOLBAR_allowedAreas);
   QC_QToolBar->addMethod("clear",                       (q_method_t)QTOOLBAR_clear);
   QC_QToolBar->addMethod("iconSize",                    (q_method_t)QTOOLBAR_iconSize);
   QC_QToolBar->addMethod("insertSeparator",             (q_method_t)QTOOLBAR_insertSeparator);
   QC_QToolBar->addMethod("insertWidget",                (q_method_t)QTOOLBAR_insertWidget);
   QC_QToolBar->addMethod("isAreaAllowed",               (q_method_t)QTOOLBAR_isAreaAllowed);
   QC_QToolBar->addMethod("isFloatable",                 (q_method_t)QTOOLBAR_isFloatable);
   QC_QToolBar->addMethod("isFloating",                  (q_method_t)QTOOLBAR_isFloating);
   QC_QToolBar->addMethod("isMovable",                   (q_method_t)QTOOLBAR_isMovable);
   QC_QToolBar->addMethod("orientation",                 (q_method_t)QTOOLBAR_orientation);
   QC_QToolBar->addMethod("setAllowedAreas",             (q_method_t)QTOOLBAR_setAllowedAreas);
   QC_QToolBar->addMethod("setFloatable",                (q_method_t)QTOOLBAR_setFloatable);
   QC_QToolBar->addMethod("setMovable",                  (q_method_t)QTOOLBAR_setMovable);
   QC_QToolBar->addMethod("setOrientation",              (q_method_t)QTOOLBAR_setOrientation);
   QC_QToolBar->addMethod("toggleViewAction",            (q_method_t)QTOOLBAR_toggleViewAction);
   QC_QToolBar->addMethod("toolButtonStyle",             (q_method_t)QTOOLBAR_toolButtonStyle);
   QC_QToolBar->addMethod("widgetForAction",             (q_method_t)QTOOLBAR_widgetForAction);

   return QC_QToolBar;
}

Namespace *initQToolBarNS(QoreClass *qwidget)
{
   Namespace *ns = new Namespace("QToolBar");
   ns->addSystemClass(initQToolBarClass(qwidget));

   return ns;
}
