/*
  ql_pwd.cpp
  
  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
#include <qore/intern/ql_env.h>

#include <stdio.h>
#include <pwd.h>
#include <errno.h>

static AbstractQoreNode *f_getpwuid(const QoreListNode *params, ExceptionSink *xsink) {
   int uid = HARD_QORE_INT(params, 0);
   return q_getpwuid(uid);
}

static AbstractQoreNode *f_getpwuid2(const QoreListNode *params, ExceptionSink *xsink) {
   int uid = HARD_QORE_INT(params, 0);
   errno = 0;
   QoreHashNode *h = q_getpwuid(uid);
   if (!h) {
      if (!errno)
         xsink->raiseException("GETPPWUID2-ERROR", "uid %d not found", uid);
      else
         xsink->raiseException("GETPPWUID2-ERROR", q_strerror(errno));
   }
   return h;
}

static AbstractQoreNode *f_getpwnam(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *name = HARD_QORE_STRING(params, 0);
   return q_getpwnam(name->getBuffer());
}

static AbstractQoreNode *f_getpwnam2(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *name = HARD_QORE_STRING(params, 0);
   errno = 0;
   QoreHashNode *h = q_getpwnam(name->getBuffer());
   if (!h) {
      if (!errno)
         xsink->raiseException("GETPPWNAM2-ERROR", "user '%s' not found", name->getBuffer());
      else
         xsink->raiseException("GETPPWNAM2-ERROR", q_strerror(errno));
   }
   return h;
}

static AbstractQoreNode *f_getgrgid(const QoreListNode *params, ExceptionSink *xsink) {
   int gid = HARD_QORE_INT(params, 0);
   return q_getgrgid(gid);
}

static AbstractQoreNode *f_getgrgid2(const QoreListNode *params, ExceptionSink *xsink) {
   int gid = HARD_QORE_INT(params, 0);
   errno = 0;
   QoreHashNode *h = q_getgrgid(gid);
   if (!h) {
      if (!errno)
         xsink->raiseException("GETPGRGID2-ERROR", "gid %d not found", gid);
      else
         xsink->raiseException("GETPGRGID2-ERROR", q_strerror(errno));
   }
   return h;
}

static AbstractQoreNode *f_getgrnam(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *name = HARD_QORE_STRING(params, 0);
   return q_getgrnam(name->getBuffer());
}

static AbstractQoreNode *f_getgrnam2(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *name = HARD_QORE_STRING(params, 0);
   errno = 0;
   QoreHashNode *h = q_getgrnam(name->getBuffer());
   if (!h) {
      if (!errno)
         xsink->raiseException("GETPGRNAM2-ERROR", "group '%s' not found", name->getBuffer());
      else
         xsink->raiseException("GETPGRNAM2-ERROR", q_strerror(errno));
   }
   return h;
}

void init_pwd_functions() {
   builtinFunctions.add2("getpwuid", f_noop, QC_RUNTIME_NOOP, QDOM_EXTERNAL_INFO, nothingTypeInfo);

   // getpwuid(softint $uid) returns *hash
   builtinFunctions.add2("getpwuid", f_getpwuid, QC_CONSTANT, QDOM_EXTERNAL_INFO, hashOrNothingTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // getpwuid2(softint $uid) returns hash
   builtinFunctions.add2("getpwuid2", f_getpwuid2, QC_NO_FLAGS, QDOM_EXTERNAL_INFO, hashTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   
   // getpwnam(string $group) returns *hash
   builtinFunctions.add2("getpwnam", f_getpwnam, QC_CONSTANT, QDOM_EXTERNAL_INFO, hashOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // getpwnam2(string $group) returns hash
   builtinFunctions.add2("getpwnam2", f_getpwnam2, QC_NO_FLAGS, QDOM_EXTERNAL_INFO, hashTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // getgrgid(softint $gid) returns *hash
   builtinFunctions.add2("getgrgid", f_getgrgid, QC_CONSTANT, QDOM_EXTERNAL_INFO, hashOrNothingTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // getgrgid2(softint $gid) returns hash
   builtinFunctions.add2("getgrgid2", f_getgrgid2, QC_NO_FLAGS, QDOM_EXTERNAL_INFO, hashTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   
   // getgrnam(string $group) returns *hash
   builtinFunctions.add2("getgrnam", f_getgrnam, QC_CONSTANT, QDOM_EXTERNAL_INFO, hashOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // getgrnam2(string $group) returns hash
   builtinFunctions.add2("getgrnam2", f_getgrnam2, QC_NO_FLAGS, QDOM_EXTERNAL_INFO, hashTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
}
