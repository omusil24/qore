/*
 QC_QListView.cc
 
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

#include "QC_QListView.h"
#include "QC_QListWidget.h"

int CID_QLISTVIEW;
class QoreClass *QC_QListView = 0;

//QListView ( QWidget * parent = 0 )
static void QLISTVIEW_constructor(QoreObject *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QLISTVIEW, new QoreQListView(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QLISTVIEW_copy(class QoreObject *self, class QoreObject *old, class QoreQListView *qlv, ExceptionSink *xsink)
{
   xsink->raiseException("QLISTVIEW-COPY-ERROR", "objects of this class cannot be copied");
}

//int batchSize () const
static QoreNode *QLISTVIEW_batchSize(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlv->getQListView()->batchSize());
}

//void clearPropertyFlags ()
static QoreNode *QLISTVIEW_clearPropertyFlags(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   qlv->getQListView()->clearPropertyFlags();
   return 0;
}

//Flow flow () const
static QoreNode *QLISTVIEW_flow(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlv->getQListView()->flow());
}

//QSize gridSize () const
static QoreNode *QLISTVIEW_gridSize(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qlv->getQListView()->gridSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//bool isRowHidden ( int row ) const
static QoreNode *QLISTVIEW_isRowHidden(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   return new QoreNode(qlv->getQListView()->isRowHidden(row));
}

//bool isSelectionRectVisible () const
static QoreNode *QLISTVIEW_isSelectionRectVisible(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qlv->getQListView()->isSelectionRectVisible());
}

//bool isWrapping () const
static QoreNode *QLISTVIEW_isWrapping(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qlv->getQListView()->isWrapping());
}

//LayoutMode layoutMode () const
static QoreNode *QLISTVIEW_layoutMode(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlv->getQListView()->layoutMode());
}

//int modelColumn () const
static QoreNode *QLISTVIEW_modelColumn(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlv->getQListView()->modelColumn());
}

//Movement movement () const
static QoreNode *QLISTVIEW_movement(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlv->getQListView()->movement());
}

//ResizeMode resizeMode () const
static QoreNode *QLISTVIEW_resizeMode(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlv->getQListView()->resizeMode());
}

//void setBatchSize ( int batchSize )
static QoreNode *QLISTVIEW_setBatchSize(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int batchSize = p ? p->getAsInt() : 0;
   qlv->getQListView()->setBatchSize(batchSize);
   return 0;
}

//void setFlow ( Flow flow )
static QoreNode *QLISTVIEW_setFlow(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QListView::Flow flow = (QListView::Flow)(p ? p->getAsInt() : 0);
   qlv->getQListView()->setFlow(flow);
   return 0;
}

//void setGridSize ( const QSize & size )
static QoreNode *QLISTVIEW_setGridSize(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQSize *size = (p && p->type == NT_OBJECT) ? (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!size) {
      if (!xsink->isException())
         xsink->raiseException("QLISTVIEW-SETGRIDSIZE-PARAM-ERROR", "expecting a QSize object as first argument to QListView::setGridSize()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
   qlv->getQListView()->setGridSize(*(static_cast<QSize *>(size)));
   return 0;
}

//void setLayoutMode ( LayoutMode mode )
static QoreNode *QLISTVIEW_setLayoutMode(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QListView::LayoutMode mode = (QListView::LayoutMode)(p ? p->getAsInt() : 0);
   qlv->getQListView()->setLayoutMode(mode);
   return 0;
}

//void setModelColumn ( int column )
static QoreNode *QLISTVIEW_setModelColumn(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   qlv->getQListView()->setModelColumn(column);
   return 0;
}

//void setMovement ( Movement movement )
static QoreNode *QLISTVIEW_setMovement(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QListView::Movement movement = (QListView::Movement)(p ? p->getAsInt() : 0);
   qlv->getQListView()->setMovement(movement);
   return 0;
}

//void setResizeMode ( ResizeMode mode )
static QoreNode *QLISTVIEW_setResizeMode(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QListView::ResizeMode mode = (QListView::ResizeMode)(p ? p->getAsInt() : 0);
   qlv->getQListView()->setResizeMode(mode);
   return 0;
}

//void setRowHidden ( int row, bool hide )
static QoreNode *QLISTVIEW_setRowHidden(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   bool hide = p ? p->getAsBool() : false;
   qlv->getQListView()->setRowHidden(row, hide);
   return 0;
}

//void setSelectionRectVisible ( bool show )
static QoreNode *QLISTVIEW_setSelectionRectVisible(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool show = p ? p->getAsBool() : false;
   qlv->getQListView()->setSelectionRectVisible(show);
   return 0;
}

//void setSpacing ( int space )
static QoreNode *QLISTVIEW_setSpacing(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int space = p ? p->getAsInt() : 0;
   qlv->getQListView()->setSpacing(space);
   return 0;
}

//void setUniformItemSizes ( bool enable )
static QoreNode *QLISTVIEW_setUniformItemSizes(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qlv->getQListView()->setUniformItemSizes(enable);
   return 0;
}

//void setViewMode ( ViewMode mode )
static QoreNode *QLISTVIEW_setViewMode(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QListView::ViewMode mode = (QListView::ViewMode)(p ? p->getAsInt() : 0);
   qlv->getQListView()->setViewMode(mode);
   return 0;
}

//void setWordWrap ( bool on )
static QoreNode *QLISTVIEW_setWordWrap(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool on = p ? p->getAsBool() : false;
   qlv->getQListView()->setWordWrap(on);
   return 0;
}

//void setWrapping ( bool enable )
static QoreNode *QLISTVIEW_setWrapping(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qlv->getQListView()->setWrapping(enable);
   return 0;
}

//int spacing () const
static QoreNode *QLISTVIEW_spacing(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlv->getQListView()->spacing());
}

//bool uniformItemSizes () const
static QoreNode *QLISTVIEW_uniformItemSizes(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qlv->getQListView()->uniformItemSizes());
}

//ViewMode viewMode () const
static QoreNode *QLISTVIEW_viewMode(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlv->getQListView()->viewMode());
}

//bool wordWrap () const
static QoreNode *QLISTVIEW_wordWrap(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qlv->getQListView()->wordWrap());
}

//QRect rectForIndex ( const QModelIndex & index ) const
static QoreNode *QLISTVIEW_rectForIndex(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QLISTVIEW-RECTFORINDEX-PARAM-ERROR", "expecting a QModelIndex object as first argument to QListView::rectForIndex()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> indexHolder(static_cast<AbstractPrivateData *>(index), xsink);
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qlv->rectForIndex(*(static_cast<QModelIndex *>(index))));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//void setPositionForIndex ( const QPoint & position, const QModelIndex & index )
static QoreNode *QLISTVIEW_setPositionForIndex(QoreObject *self, QoreAbstractQListView *qlv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!position) {
      if (!xsink->isException())
         xsink->raiseException("QLISTVIEW-SETPOSITIONFORINDEX-PARAM-ERROR", "expecting a QPoint object as first argument to QListView::setPositionForIndex()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
   p = get_param(params, 1);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QLISTVIEW-SETPOSITIONFORINDEX-PARAM-ERROR", "expecting a QModelIndex object as second argument to QListView::setPositionForIndex()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> indexHolder(static_cast<AbstractPrivateData *>(index), xsink);
   qlv->setPositionForIndex(*(static_cast<QPoint *>(position)), *(static_cast<QModelIndex *>(index)));
   return 0;
}

static QoreClass *initQListViewClass(QoreClass *qabstractitemview)
{
   QC_QListView = new QoreClass("QListView", QDOM_GUI);
   CID_QLISTVIEW = QC_QListView->getID();

   QC_QListView->addBuiltinVirtualBaseClass(qabstractitemview);

   QC_QListView->setConstructor(QLISTVIEW_constructor);
   QC_QListView->setCopy((q_copy_t)QLISTVIEW_copy);

   QC_QListView->addMethod("batchSize",                   (q_method_t)QLISTVIEW_batchSize);
   QC_QListView->addMethod("clearPropertyFlags",          (q_method_t)QLISTVIEW_clearPropertyFlags);
   QC_QListView->addMethod("flow",                        (q_method_t)QLISTVIEW_flow);
   QC_QListView->addMethod("gridSize",                    (q_method_t)QLISTVIEW_gridSize);
   QC_QListView->addMethod("isRowHidden",                 (q_method_t)QLISTVIEW_isRowHidden);
   QC_QListView->addMethod("isSelectionRectVisible",      (q_method_t)QLISTVIEW_isSelectionRectVisible);
   QC_QListView->addMethod("isWrapping",                  (q_method_t)QLISTVIEW_isWrapping);
   QC_QListView->addMethod("layoutMode",                  (q_method_t)QLISTVIEW_layoutMode);
   QC_QListView->addMethod("modelColumn",                 (q_method_t)QLISTVIEW_modelColumn);
   QC_QListView->addMethod("movement",                    (q_method_t)QLISTVIEW_movement);
   QC_QListView->addMethod("resizeMode",                  (q_method_t)QLISTVIEW_resizeMode);
   QC_QListView->addMethod("setBatchSize",                (q_method_t)QLISTVIEW_setBatchSize);
   QC_QListView->addMethod("setFlow",                     (q_method_t)QLISTVIEW_setFlow);
   QC_QListView->addMethod("setGridSize",                 (q_method_t)QLISTVIEW_setGridSize);
   QC_QListView->addMethod("setLayoutMode",               (q_method_t)QLISTVIEW_setLayoutMode);
   QC_QListView->addMethod("setModelColumn",              (q_method_t)QLISTVIEW_setModelColumn);
   QC_QListView->addMethod("setMovement",                 (q_method_t)QLISTVIEW_setMovement);
   QC_QListView->addMethod("setResizeMode",               (q_method_t)QLISTVIEW_setResizeMode);
   QC_QListView->addMethod("setRowHidden",                (q_method_t)QLISTVIEW_setRowHidden);
   QC_QListView->addMethod("setSelectionRectVisible",     (q_method_t)QLISTVIEW_setSelectionRectVisible);
   QC_QListView->addMethod("setSpacing",                  (q_method_t)QLISTVIEW_setSpacing);
   QC_QListView->addMethod("setUniformItemSizes",         (q_method_t)QLISTVIEW_setUniformItemSizes);
   QC_QListView->addMethod("setViewMode",                 (q_method_t)QLISTVIEW_setViewMode);
   QC_QListView->addMethod("setWordWrap",                 (q_method_t)QLISTVIEW_setWordWrap);
   QC_QListView->addMethod("setWrapping",                 (q_method_t)QLISTVIEW_setWrapping);
   QC_QListView->addMethod("spacing",                     (q_method_t)QLISTVIEW_spacing);
   QC_QListView->addMethod("uniformItemSizes",            (q_method_t)QLISTVIEW_uniformItemSizes);
   QC_QListView->addMethod("viewMode",                    (q_method_t)QLISTVIEW_viewMode);
   QC_QListView->addMethod("wordWrap",                    (q_method_t)QLISTVIEW_wordWrap);

   // private methods
   QC_QListView->addMethod("rectForIndex",                (q_method_t)QLISTVIEW_rectForIndex, true);
   QC_QListView->addMethod("setPositionForIndex",         (q_method_t)QLISTVIEW_setPositionForIndex, true);
   
   return QC_QListView;
}

Namespace *initQListViewNS(QoreClass *qabstractitemview)
{
   Namespace *ns = new Namespace("QListView");
   ns->addSystemClass(initQListViewClass(qabstractitemview));

   ns->addSystemClass(initQListWidgetClass(QC_QListView));

   // Movement enum
   ns->addConstant("Static",                   new QoreNode((int64)QListView::Static));
   ns->addConstant("Free",                     new QoreNode((int64)QListView::Free));
   ns->addConstant("Snap",                     new QoreNode((int64)QListView::Snap));

   // Flow enum
   ns->addConstant("LeftToRight",              new QoreNode((int64)QListView::LeftToRight));
   ns->addConstant("TopToBottom",              new QoreNode((int64)QListView::TopToBottom));

   // ResizeMode enum
   ns->addConstant("Fixed",                    new QoreNode((int64)QListView::Fixed));
   ns->addConstant("Adjust",                   new QoreNode((int64)QListView::Adjust));

   // LayoutMode enum
   ns->addConstant("SinglePass",               new QoreNode((int64)QListView::SinglePass));
   ns->addConstant("Batched",                  new QoreNode((int64)QListView::Batched));

   // ViewMode enum
   ns->addConstant("ListMode",                 new QoreNode((int64)QListView::ListMode));
   ns->addConstant("IconMode",                 new QoreNode((int64)QListView::IconMode));

   return ns;
}
