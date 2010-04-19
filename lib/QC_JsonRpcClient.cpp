/*
  QC_JsonRpcClient.cpp

  Qore Programming Language

  Copyright (C) 2006 - 2010 Qore Technologies

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
#include <qore/intern/QC_JsonRpcClient.h>
#include <qore/intern/QC_HTTPClient.h>
#include <qore/QoreHTTPClient.h>
#include <qore/ReferenceHolder.h>
#include <qore/intern/ql_json.h>

#include <qore/intern/QC_Queue.h>

qore_classid_t CID_JSONRPCCLIENT;

static void set_jrc_defaults(QoreHTTPClient &client) {
   // set encoding to UTF-8
   client.setEncoding(QCS_UTF8);

   // set options for JSON-RPC communication
   client.setDefaultPath("JSON");
   client.setDefaultHeaderValue("Content-Type", "application/json");
   client.setDefaultHeaderValue("Accept", "application/json");
   client.setDefaultHeaderValue("User-Agent", "Qore JSON-RPC Client v" PACKAGE_VERSION);

   client.addProtocol("jsonrpc", 80, false);
   client.addProtocol("jsonrpcs", 443, true);
}

static void JRC_constructor_bool(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   // get HTTPClient object
   ReferenceHolder<QoreHTTPClient> client((QoreHTTPClient *)getStackObject()->getReferencedPrivateData(CID_HTTPCLIENT, xsink), xsink);
   if (!client)
      return;

   set_jrc_defaults(*(*client));

   if (!HARD_QORE_BOOL(params, 0))
      client->connect(xsink); 
}

static void JRC_constructor_hash_bool(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   // get HTTPClient object
   ReferenceHolder<QoreHTTPClient> client((QoreHTTPClient *)getStackObject()->getReferencedPrivateData(CID_HTTPCLIENT, xsink), xsink);
   if (!client)
      return;

   set_jrc_defaults(*(*client));

   const QoreHashNode* n = HARD_QORE_HASH(params, 0);
   if (client->setOptions(n, xsink))
      return;

   // do not connect immediately if the second argument is True
   if (!HARD_QORE_BOOL(params, 1))
      client->connect(xsink); 
}

static void JRC_copy(QoreObject *self, QoreObject *old, QoreHTTPClient* client, ExceptionSink *xsink) {
   xsink->raiseException("JSONRPCCLIENT-COPY-ERROR", "copying JsonRpcClient objects is not yet supported.");
}

static AbstractQoreNode *make_jsonrpc_call(QoreHTTPClient *client, QoreStringNode *msg, QoreHashNode *info, ExceptionSink *xsink) {
   ReferenceHolder<QoreHashNode> response(client->send("POST", 0, 0, msg->getBuffer(), msg->strlen(), true, info, xsink), xsink);
   if (!response)
      return 0;

   ReferenceHolder<AbstractQoreNode> ans(response->takeKeyValue("body"), xsink);

   if (!ans)
      return 0;

   AbstractQoreNode *ah = *ans;
   if (info) {
      info->setKeyValue("response", ans.release(), xsink);
      info->setKeyValue("response_headers", response.release(), xsink);
   }
   
   if (ah->getType() != NT_STRING) {
      xsink->raiseException("JSONRPCCLIENT-RESPONSE-ERROR", "undecoded binary response received from remote server");
      return 0;
   }

   // parse JSON-RPC response   
   return parseJSONValue(reinterpret_cast<QoreStringNode *>(ah), xsink);
}

static AbstractQoreNode *JRC_callArgs(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   // create the outgoing message in JSON-RPC call format
   SimpleRefHolder<QoreStringNode> msg(makeJSONRPC11RequestStringArgs(params, xsink));
   if (!msg)
      return 0;

   return make_jsonrpc_call(client, *msg, 0, xsink);
}

static AbstractQoreNode *JRC_call(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   // create the outgoing message in JSON-RPC call format
   SimpleRefHolder<QoreStringNode> msg(makeJSONRPC11RequestString(params, xsink));
   if (!msg)
      return 0;

   return make_jsonrpc_call(client, *msg, 0, xsink);
}

static AbstractQoreNode *JRC_callArgsWithInfo(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   // get info reference
   const ReferenceNode *ref = HARD_QORE_REF(params, 0);

   // get arguments
   ReferenceHolder<QoreListNode> args(params->copyListFrom(1), xsink);

   // create the outgoing message in JSON-RPC call format
   QoreStringNode *msg = makeJSONRPC11RequestStringArgs(*args, xsink);
   if (!msg)
      return 0;

   HTTPInfoRefHelper irh(ref, msg, xsink);

   // send the message to the server and get the response as a JSON string
   ReferenceHolder<AbstractQoreNode> rv(make_jsonrpc_call(client, msg, *irh, xsink), xsink);

   return *xsink ? 0 : rv.release();
}

static AbstractQoreNode *JRC_callWithInfo(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   // get info reference
   const ReferenceNode *ref = HARD_QORE_REF(params, 0);

   // get arguments
   ReferenceHolder<QoreListNode> args(params->copyListFrom(1), xsink);

   // create the outgoing message in JSON-RPC call format
   QoreStringNode *msg = makeJSONRPC11RequestString(*args, xsink);
   if (!msg)
      return 0;

   HTTPInfoRefHelper irh(ref, msg, xsink);

   // send the message to the server and get the response as a JSON string
   ReferenceHolder<AbstractQoreNode> rv(make_jsonrpc_call(client, msg, *irh, xsink), xsink);

   return *xsink ? 0 : rv.release();
}

static AbstractQoreNode *JRC_setEventQueue_nothing(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   client->setEventQueue(0, xsink);
   return 0;
}

static AbstractQoreNode *JRC_setEventQueue_queue(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(q, Queue, params, 0, CID_QUEUE, "Queue", "JsonRpcClient::setEventQueue", xsink);
   if (*xsink)
      return 0;
   // pass reference from QoreObject::getReferencedPrivateData() to function
   client->setEventQueue(q, xsink);
   return 0;
}

QoreClass *initJsonRpcClientClass(QoreClass *http_client) {
   assert(QC_QUEUE);

   QoreClass* client = new QoreClass("JsonRpcClient", QDOM_NETWORK);
   CID_JSONRPCCLIENT = client->getID();

   client->addDefaultBuiltinBaseClass(http_client);

   client->setConstructorExtended(JRC_constructor_bool, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, softBoolTypeInfo, &False);
   client->setConstructorExtended(JRC_constructor_hash_bool, false, QC_NO_FLAGS, QDOM_DEFAULT, 2, hashTypeInfo, QORE_PARAM_NO_ARG, softBoolTypeInfo, &False);

   client->setCopy((q_copy_t)JRC_copy);
   client->addMethodExtended("callArgs",         (q_method_t)JRC_callArgs, false, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, stringTypeInfo, QORE_PARAM_NO_ARG, anyTypeInfo, QORE_PARAM_NO_ARG);
   client->addMethodExtended("call",             (q_method_t)JRC_call, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, 0, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   client->addMethodExtended("callArgsWithInfo", (q_method_t)JRC_callArgsWithInfo, false, QC_NO_FLAGS, QDOM_DEFAULT, 0, 3, referenceTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, anyTypeInfo, QORE_PARAM_NO_ARG);
   client->addMethodExtended("callWithInfo",     (q_method_t)JRC_callWithInfo, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, 0, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // JsonRpcClient::setEventQueue() returns nothing
   client->addMethodExtended("setEventQueue",    (q_method_t)JRC_setEventQueue_nothing, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   // JsonRpcClient::setEventQueue(Queue $queue) returns nothing
   client->addMethodExtended("setEventQueue",    (q_method_t)JRC_setEventQueue_queue, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, QC_QUEUE->getTypeInfo(), QORE_PARAM_NO_ARG);

   return client;
} 
