/*
  QoreApp.h

  TIBCO integration to QORE

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

#ifndef _QORE_TIBAE_QOREAPP_H

#define _QORE_TIBAE_QOREAPP_H

#include <qore/Qore.h>

#include "tibae.h"

#include <Maverick.h>
#include <time.h>

#ifdef TIBCO_MDT_BUG
extern class LockedObject l_mdate_time;
#endif

class QoreApp : public AbstractPrivateData, public MApp
{
   private:
      char *session_name;
      char *service;
      char *network;
      char *daemon;
      char *subject;

      class MAppProperties *appProps;

      // for receive
      char *rcv_subject;
      class LockedObject rcv_lock;

      class QoreHash *classlist;

      class MPublisher *myPublisher;
      class MRvSession *mySession;
      class MSubscriber *mySubscriber;
      class QoreEventHandler *myEventHandler;
      class MClassRegistry *mcr;

      DLLLOCAL class MTree *make_MTree(const char *class_name, QoreNode *value, ExceptionSink *xsink);
      DLLLOCAL class MData *instantiate_class(QoreNode *v, const MBaseClassDescription *mbcd, ExceptionSink *xsink);
      DLLLOCAL class MData *instantiate_sequence(const MSequenceClassDescription *msd, QoreNode *v, ExceptionSink *xsink);
      DLLLOCAL class MData *instantiate_modeledclass(const MModeledClassDescription *mcd, QoreNode *v, ExceptionSink *xsink);
      DLLLOCAL class MData *instantiate_union(const MUnionDescription *mud, QoreNode *v, ExceptionSink *xsink);
      DLLLOCAL class MData *do_type(int type_code, QoreNode *v, ExceptionSink *xsink);
      DLLLOCAL class MData *do_primitive_type(const MPrimitiveClassDescription *pcd, QoreNode *v, ExceptionSink *xsink);
      DLLLOCAL const MBaseClassDescription *find_class(const char *cn, ExceptionSink *xsink);

      // helpers for operations 
      DLLLOCAL void setRequestParameters(MOperationRequest& req, QoreHash* params, ExceptionSink* xsink);

   protected:
      DLLLOCAL virtual void onInitialization() throw (MException);
      DLLLOCAL virtual void onTermination() throw (MException) {}
      DLLLOCAL virtual ~QoreApp();

   public:
      DLLLOCAL QoreApp(MAppProperties *pMAP, const char *name, class QoreHash *clh, 
		       const char *svc = NULL, const char *net = NULL, 
		       const char *dmn = NULL, const char *sbj = NULL);
      DLLLOCAL class QoreNode *sendWithSyncReply(const char *function_name, QoreNode *value, int timeout, ExceptionSink *xsink);
      DLLLOCAL class QoreNode *sendWithSyncReply(const char *subject, const char *function_name, QoreNode *value, int timeout, ExceptionSink *xsink);
      DLLLOCAL void send(const char *function_name, QoreNode *value, ExceptionSink *xsink);
      DLLLOCAL void send(const char *subject, const char *function_name, QoreNode *value, ExceptionSink *xsink);
      DLLLOCAL class QoreNode *receive(const char *subject, unsigned long timeout, ExceptionSink *xsink);
      DLLLOCAL void set_subject_name(const char *sub);
      DLLLOCAL const char *get_subject() { return subject; }
      DLLLOCAL class QoreNode *map_mtree_to_node(MTree *msg, ExceptionSink *xsink);
      DLLLOCAL virtual void deref(class ExceptionSink *xsink);

      // operations
      DLLLOCAL QoreNode* operationsCallWithSyncResult(const char *class_name, const char *method_name, QoreHash* parameters, unsigned timeout, const char *client_name, ExceptionSink* xsink);
      DLLLOCAL void operationsOneWayCall(const char *class_name, const char *method_name, QoreHash* parameters, const char *client_name, ExceptionSink* xsink);
      DLLLOCAL void operationsAsyncCall(const char *class_name, const char *method_name, QoreHash* parameters, unsigned timeout, const char *client_name, ExceptionSink* xsink);
      DLLLOCAL QoreNode* operationsGetAsyncCallResult(const char *class_name, const char *method_name, ExceptionSink* xsink);
};

class QoreEventHandler : public MEventListener
{
   private:
      QoreApp *myQoreApp; 
      int count;

      DLLLOCAL virtual void onEvent(const MEvent &refEvent);

   public:
      QoreNode *msgNode;
      MString replySubject;
      MString subject;
      ExceptionSink xsink;
      
      DLLLOCAL QoreEventHandler(QoreApp *pMApp);
      DLLLOCAL virtual ~QoreEventHandler();
      
}; // QoreEventHandler 

#endif // _QORE_TIBAE_QOREAPP_H

