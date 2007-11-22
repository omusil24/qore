
#if 0
class T {
#endif

   protected:
      virtual QWidget * createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
      {
	 if (!m_createEditor)
	    return QOREQTYPE::createEditor(parent, option, index);

	 QoreList *args = new QoreList();

	 args->push(return_qobject(parent));

	 Object *qw = new Object(QC_QStyleOptionViewItem, getProgram());
	 qw->setPrivate(CID_QSTYLEOPTIONVIEWITEM, new QoreQStyleOptionViewItem(option));
	 args->push(new QoreNode(qw));
	 
	 qw = new Object(QC_QModelIndex, getProgram());
	 qw->setPrivate(CID_QMODELINDEX, new QoreQModelIndex(index));
	 args->push(new QoreNode(qw));

	 ExceptionSink xsink;

	 QoreNode *rv = dispatch_event_intern(qore_obj, m_createEditor, args, &xsink);

	 if (xsink) {
            discard(rv, &xsink);
            return QOREQTYPE::createEditor(parent, option, index);
         }
	 
	 QoreQWidget *qqw = (rv && rv->type == NT_OBJECT) ? (QoreQWidget *)rv->val.object->getReferencedPrivateData(CID_QWIDGET, &xsink) : 0;
         discard(rv, &xsink);

         if (!qqw) {
            xsink.raiseException("CREATEEDITOR-ERROR", "the createEditor() method did not return a QWidget object");
            return QOREQTYPE::createEditor(parent, option, index);
         }
         ReferenceHolder<QoreQWidget> sizeHolder(qqw, &xsink);
	 return qqw->getQWidget();
      }

      virtual bool editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index )
      {
	 if (!m_editorEvent)
	    return QOREQTYPE::editorEvent(event, model, option, index);

	 QoreList *args = new QoreList();
	 Object *o = new Object(QC_QEvent, getProgram());
	 o->setPrivate(CID_QEVENT, new QoreQEvent(*event));
	 args->push(new QoreNode(o));

	 // FIXME: find derived class from QAbstractItemModel and instantiate qore class
	 o = new Object(QC_QAbstractItemModel, getProgram());
	 o->setPrivate(CID_QABSTRACTITEMMODEL, new QoreQtQAbstractItemModel(o, model));
	 args->push(new QoreNode(o));

	 o = new Object(QC_QStyleOptionViewItem, getProgram());
	 o->setPrivate(CID_QSTYLEOPTIONVIEWITEM, new QoreQStyleOptionViewItem(option));
	 args->push(new QoreNode(o));

	 o = new Object(QC_QModelIndex, getProgram());
	 o->setPrivate(CID_QMODELINDEX, new QoreQModelIndex(index));
	 args->push(new QoreNode(o));

	 return dispatch_event_bool(qore_obj, m_editorEvent, args);
      }

      virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
      {
	 if (!m_paint) {
	    QOREQTYPE::paint(painter, option, index);
	    return;
	 }

	 QoreList *args = new QoreList();
	 Object *o = new Object(QC_QPainter, getProgram());
	 o->setPrivate(CID_QPAINTER, new QoreQPainter(painter));
	 args->push(new QoreNode(o));

	 o = new Object(QC_QStyleOptionViewItem, getProgram());
	 o->setPrivate(CID_QSTYLEOPTIONVIEWITEM, new QoreQStyleOptionViewItem(option));
	 args->push(new QoreNode(o));

	 o = new Object(QC_QModelIndex, getProgram());
	 o->setPrivate(CID_QMODELINDEX, new QoreQModelIndex(index));
	 args->push(new QoreNode(o));

	 dispatch_event(qore_obj, m_paint, args);
      }

      virtual void setEditorData ( QWidget * editor, const QModelIndex & index ) const
      {
	 if (!m_setEditorData)
	    return QOREQTYPE::setEditorData(editor, index);

	 QoreList *args = new QoreList();

	 args->push(return_qobject(editor));

	 Object *qw = new Object(QC_QModelIndex, getProgram());
	 qw->setPrivate(CID_QMODELINDEX, new QoreQModelIndex(index));
	 args->push(new QoreNode(qw));

	 dispatch_event(qore_obj, m_setEditorData, args);
      }

      virtual void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
      {
	 if (!m_setModelData) {
	    QOREQTYPE::setModelData(editor, model, index);
	    return;
	 }

	 QoreList *args = new QoreList();

	 Object *qw = 0;
	 if (editor) {
	    QVariant qv_ptr = editor->property("qobject");
	    qw = reinterpret_cast<Object *>(qv_ptr.toULongLong());
	    if (qw) 
	       qw->ref();
	 }
	 args->push(qw ? new QoreNode(qw) : nothing());

	 // FIXME: find derived class from QAbstractItemModel and instantiate qore class
	 Object *o = new Object(QC_QAbstractItemModel, getProgram());
	 o->setPrivate(CID_QABSTRACTITEMMODEL, new QoreQtQAbstractItemModel(o, model));
	 args->push(new QoreNode(o));

	 qw = new Object(QC_QModelIndex, getProgram());
	 qw->setPrivate(CID_QMODELINDEX, new QoreQModelIndex(index));
	 args->push(new QoreNode(qw));

	 dispatch_event(qore_obj, m_setModelData, args);
      }

      virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
      {
	 if (!m_sizeHint)
	    return QOREQTYPE::sizeHint(option, index);

	 QoreList *args = new QoreList();

	 Object *o = new Object(QC_QStyleOptionViewItem, getProgram());
	 o->setPrivate(CID_QSTYLEOPTIONVIEWITEM, new QoreQStyleOptionViewItem(option));
	 args->push(new QoreNode(o));
	 
	 o = new Object(QC_QModelIndex, getProgram());
	 o->setPrivate(CID_QMODELINDEX, new QoreQModelIndex(index));
	 args->push(new QoreNode(o));

	 ExceptionSink xsink;

	 QoreNode *rv = dispatch_event_intern(qore_obj, m_sizeHint, args, &xsink);
	 
	 if (xsink) {
            discard(rv, &xsink);
            return QOREQTYPE::sizeHint(option, index);
         }
	 
	 QoreQSize *qs = (rv && rv->type == NT_OBJECT) ? (QoreQSize *)rv->val.object->getReferencedPrivateData(CID_QSIZE, &xsink) : 0;
         discard(rv, &xsink);

         if (!qs) {
            xsink.raiseException("SIZEHINT-ERROR", "the sizeHint() method did not return a QSize object");
            return QOREQTYPE::sizeHint(option, index);
         }
         ReferenceHolder<QoreQSize> sizeHolder(qs, &xsink);
         QSize rv_qs = *(static_cast<QSize *>(qs));
         return rv_qs;
      }

      virtual void updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index ) const
      {
	 if (!m_updateEditorGeometry) {
	    QOREQTYPE::updateEditorGeometry(editor, option, index);
	    return;
	 }

	 QoreList *args = new QoreList();

	 Object *o = 0;
	 if (editor) {
	    QVariant qv_ptr = editor->property("qobject");
	    o = reinterpret_cast<Object *>(qv_ptr.toULongLong());
	    if (o) 
	       o->ref();
	 }
	 args->push(o ? new QoreNode(o) : nothing());

	 o = new Object(QC_QStyleOptionViewItem, getProgram());
	 o->setPrivate(CID_QSTYLEOPTIONVIEWITEM, new QoreQStyleOptionViewItem(option));
	 args->push(new QoreNode(o));

	 o = new Object(QC_QModelIndex, getProgram());
	 o->setPrivate(CID_QMODELINDEX, new QoreQModelIndex(index));
	 args->push(new QoreNode(o));

	 dispatch_event(qore_obj, m_updateEditorGeometry, args);
      }

   public:
      virtual QWidget * parent_createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
      {
	 return QOREQTYPE::createEditor(parent, option, index);
      }

      virtual bool parent_editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index )
      {
	 return QOREQTYPE::editorEvent(event, model, option, index);
      }

      virtual void parent_paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
      {
	 QOREQTYPE::paint(painter, option, index);
      }

      virtual void parent_setEditorData ( QWidget * editor, const QModelIndex & index ) const
      {
	 QOREQTYPE::setEditorData(editor, index);
      }

      virtual void parent_setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
      {
	 QOREQTYPE::setModelData(editor, model, index);
      }

      virtual QSize parent_sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
      {
	 return QOREQTYPE::sizeHint(option, index);
      }

      virtual void parent_updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index ) const
      {
	 QOREQTYPE::updateEditorGeometry(editor, option, index);
      }


#if 0
}
#endif

