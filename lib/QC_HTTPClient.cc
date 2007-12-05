/*
  QC_HTTPClient.cc

  Qore Programming Language

  Copyright (C) 2006, 2007 Qore Technologies

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
#include <qore/QC_HTTPClient.h>
#include <qore/QoreHTTPClient.h>
#include <qore/ssl_constants.h>
#include <qore/minitest.hpp>

#ifdef DEBUG_TESTS
#  include "tests/QC_HTTPClient_tests.cc"
#endif

int CID_HTTPCLIENT;

//-----------------------------------------------------------------------------
static void HC_constructor(class QoreObject *self, class QoreNode *params, ExceptionSink *xsink)
{
  QoreHTTPClient* client = new QoreHTTPClient();
  QoreNode* n = test_param(params, NT_HASH, 0);
  if (n) {
     if (client->setOptions(n->val.hash, xsink))
     {
	client->deref();
	return;
     }
  }

  self->setPrivate(CID_HTTPCLIENT, client);
}

//-----------------------------------------------------------------------------
static void HC_copy(QoreObject *self, QoreObject *old, QoreHTTPClient* client, ExceptionSink *xsink)
{
   xsink->raiseException("HTTPCLIENT-COPY-ERROR", "copying HTTPClient objects is not yet supported.");
}

static class QoreNode *HC_setHTTPVersion(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
   {
      xsink->raiseException("HTTP-CLIENT-SETHTTPVERSION-ERROR", "expecting string ('1.0' or '1.1') passed as only argument");
      return NULL;
   }
   client->setHTTPVersion(p->val.String->getBuffer(), xsink);
   return NULL;
}

static class QoreNode *HC_getHTTPVersion(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(client->getHTTPVersion());
}

static class QoreNode *HC_setSecure(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *n = get_param(params, 0);
   client->setSecure(n ? n->getAsBool() : false);
   return NULL;
}

static class QoreNode *HC_isSecure(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(client->isSecure());
}

static class QoreNode *HC_verifyPeerCertificate(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   char *c = getSSLCVCode(client->verifyPeerCertificate());
   return c ? new QoreNode(c) : NULL;
}

static class QoreNode *HC_getSSLCipherName(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   const char *str = client->getSSLCipherName();
   return str ? new QoreNode(str) : NULL;
}

static class QoreNode *HC_getSSLCipherVersion(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   const char *str = client->getSSLCipherVersion();
   return str ? new QoreNode(str) : NULL;
}

static class QoreNode *HC_connect(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   client->connect(xsink);
   return NULL;
}

static class QoreNode *HC_disconnect(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   client->disconnect();
   return NULL;
}

// send(data, method, path, headers, [getbody])
static class QoreNode *HC_send(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   const void *ptr = NULL;
   int size = 0;
   class QoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
   {
      if (p->type == NT_STRING)
      {
	 ptr = p->val.String->getBuffer();
	 size = p->val.String->strlen();
      }
      else if (p->type == NT_BINARY)
      {
         ptr = p->val.bin->getPtr();
         size = p->val.bin->size();
      }
      // ignore other types - no exception raised
   }
   
   p = test_param(params, NT_STRING, 1);
   if (!p)
   {
      xsink->raiseException("HTTP-CLIENT-SEND-ERROR", "expecting method name as second parameter");
      return NULL;
   }
   const char *meth = p->val.String->getBuffer();

   p = test_param(params, NT_STRING, 2);
   const char *path = p ? p->val.String->getBuffer() : NULL;

   p = test_param(params, NT_HASH, 3);
   class QoreHash *h = p ? p->val.hash : NULL;

   p = get_param(params, 4);
   bool getbody = p ? p->getAsBool() : false;

   return client->send(meth, path, h, ptr, size, getbody, xsink);
}

// get(path, headers)
static class QoreNode *HC_get(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
   {
      xsink->raiseException("HTTP-CLIENT-GET-ERROR", "expecting path as first parameter");
      return NULL;
   }
   const char *path = p->val.String->getBuffer();

   p = test_param(params, NT_HASH, 1);

   return client->get(path, p ? p->val.hash : NULL, xsink);
}

// head(path, headers)
static class QoreNode *HC_head(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
   {
      xsink->raiseException("HTTP-CLIENT-HEAD-ERROR", "expecting path as first parameter");
      return NULL;
   }
   const char *path = p->val.String->getBuffer();

   p = test_param(params, NT_HASH, 1);

   return client->head(path, p ? p->val.hash : NULL, xsink);
}

// post(path, data, headers)
static class QoreNode *HC_post(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
   {
      xsink->raiseException("HTTP-CLIENT-POST-ERROR", "expecting path as first parameter");
      return NULL;
   }
   const char *path = p->val.String->getBuffer();

   const void *ptr = NULL;
   int size = 0;
   p = get_param(params, 1);
   if (!is_nothing(p))
   {
      if (p->type == NT_STRING)
      {
	 ptr = p->val.String->getBuffer();
	 size = p->val.String->strlen();
      }
      else if (p->type == NT_BINARY)
      {
         ptr = p->val.bin->getPtr();
         size = p->val.bin->size();
      }
      else
      {
	 xsink->raiseException("HTTP-CLIENT-POST-ERROR", "expecting string or binary as second argument for message data");
	 return NULL;
      }
   }

   p = test_param(params, NT_HASH, 2);
   class QoreHash *h = p ? p->val.hash : NULL;

   return client->post(path, h, ptr, size, xsink);
}

static class QoreNode *HC_setTimeout(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   client->setTimeout(getMsZeroInt(get_param(params, 0)));
   return NULL;
}

static class QoreNode *HC_getTimeout(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)client->getTimeout());
}

static class QoreNode *HC_setEncoding(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("HTTP-CLIENT-SET-ENCODING-ERROR", "expecting charset name (string) as parameter of HTTPClient::setEncoding() call");
      return NULL;
   }

   client->setEncoding(QEM.findCreate(p0->val.String));
   return NULL; 
}

static class QoreNode *HC_getEncoding(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(client->getEncoding()->getCode());
}

class QoreNode *f_setURL(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *n = test_param(params, NT_STRING, 0);
   if (!n || !n->val.String->strlen())
      xsink->raiseException("HTTP-CLIENT-EMPTY-URL", "HTTPClient::setURL() called without a valid string argument");
   else
      client->setURL(n->val.String->getBuffer(), xsink);
   return NULL;
}

class QoreNode *f_getURL(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreString *str = client->getURL();
   return str ? new QoreNode(str) : NULL;
}

class QoreNode *f_setProxyURL(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *n = test_param(params, NT_STRING, 0);
   if (!n)
      client->clearProxyURL();
   else
      client->setProxyURL(n->val.String->getBuffer(), xsink);
   return NULL;
}

class QoreNode *f_getProxyURL(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreString *str = client->getProxyURL();
   return str ? new QoreNode(str) : NULL;
}

class QoreNode *f_clearProxyURL(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   client->clearProxyURL();
   return NULL;
}

class QoreNode *f_setProxySecure(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *n = get_param(params, 0);
   client->setProxySecure(n ? n->getAsBool() : false);
   return NULL;
}

class QoreNode *f_isProxySecure(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(client->isProxySecure());
}

class QoreNode *f_setMaxRedirects(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *n = get_param(params, 0);
   client->setMaxRedirects(n ? n->getAsInt() : 0);
   return NULL;
}

class QoreNode *f_getMaxRedirects(class QoreObject *self, class QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)client->getMaxRedirects());
}

//-----------------------------------------------------------------------------
QoreClass *initHTTPClientClass()
{
   tracein("initHTTPClientClass");

   // initialize static data structures in the QoreHTTPClient class
   QoreHTTPClient::static_init();

   QoreClass* client = new QoreClass("HTTPClient", QDOM_NETWORK);
   CID_HTTPCLIENT = client->getID();

   client->setConstructor(HC_constructor);
   client->setCopy((q_copy_t)HC_copy);
   client->addMethod("setHTTPVersion",         (q_method_t)HC_setHTTPVersion);
   client->addMethod("getHTTPVersion",         (q_method_t)HC_getHTTPVersion);
   client->addMethod("setSecure",              (q_method_t)HC_setSecure);
   client->addMethod("isSecure",               (q_method_t)HC_isSecure);
   client->addMethod("verifyPeerCertificate",  (q_method_t)HC_verifyPeerCertificate);
   client->addMethod("getSSLCipherName",       (q_method_t)HC_getSSLCipherName);
   client->addMethod("getSSLCipherVersion",    (q_method_t)HC_getSSLCipherVersion);
   client->addMethod("connect",                (q_method_t)HC_connect);
   client->addMethod("disconnect",             (q_method_t)HC_disconnect);
   client->addMethod("send",                   (q_method_t)HC_send);
   client->addMethod("get",                    (q_method_t)HC_get);
   client->addMethod("head",                   (q_method_t)HC_head);
   client->addMethod("post",                   (q_method_t)HC_post);
   client->addMethod("setTimeout",             (q_method_t)HC_setTimeout);
   client->addMethod("getTimeout",             (q_method_t)HC_getTimeout);
   client->addMethod("setEncoding",            (q_method_t)HC_setEncoding);
   client->addMethod("getEncoding",            (q_method_t)HC_getEncoding);
   client->addMethod("setURL",                 (q_method_t)f_setURL);
   client->addMethod("getURL",                 (q_method_t)f_getURL);
   client->addMethod("setProxyURL",            (q_method_t)f_setProxyURL);
   client->addMethod("getProxyURL",            (q_method_t)f_getProxyURL);
   client->addMethod("clearProxyURL",          (q_method_t)f_clearProxyURL);
   client->addMethod("setProxySecure",         (q_method_t)f_setProxySecure);
   client->addMethod("isProxySecure",          (q_method_t)f_isProxySecure);
   client->addMethod("setMaxRedirects",        (q_method_t)f_setMaxRedirects);
   client->addMethod("getMaxRedirects",        (q_method_t)f_getMaxRedirects);

   traceout("initHTTPClientClass");
   return client;
}

// EOF


