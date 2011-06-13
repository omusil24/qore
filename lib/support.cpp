/*
  support.cpp

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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

#include <qore/intern/qore_program_private.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

extern bool threads_initialized;

#define QORE_SERIALIZE_DEBUGGING_OUTPUT 1

#ifdef QORE_SERIALIZE_DEBUGGING_OUTPUT
static QoreThreadLock debug_output_lock;
#endif

int printe(const char *fmt, ...) {
   va_list args;
   QoreString buf;

   while (true) {
      va_start(args, fmt);
      int rc = buf.vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }
		    
   fputs(buf.getBuffer(), stderr);
   fflush(stderr);
   return 0;
}

static void get_timestamp(QoreString &str) {
   if (!threads_initialized)
      return;

   int us;
   int64 secs = q_epoch_us(us);
   DateTime now;
   now.setDate(currentTZ(), secs, us);
   now.format(str, "YYYY-MM-DD HH:mm:SS.xx");
}

int print_debug(int level, const char *fmt, ...) {
   if (level > debug)
      return 0;

   va_list args;
   QoreString buf;

   while (true) {
      va_start(args, fmt);
      int rc = buf.vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }

   QoreString ts;
   get_timestamp(ts);

#ifdef QORE_SERIALIZE_DEBUGGING_OUTPUT
   AutoLocker al(debug_output_lock);
#endif
   int tid = (threads_initialized && is_valid_qore_thread()) ? gettid() : -1;
   fprintf(stderr, "%s: TID %d: %s", ts.getBuffer(), tid, buf.getBuffer());
   fflush(stderr);
   return 0;
}

void trace_function(int code, const char *funcname) {
   if (!qore_trace)
      return;

   QoreString ts;
   get_timestamp(ts);
   if (code == TRACE_IN)
      printe("%s: TID %d: %s entered\n", ts.getBuffer(), threads_initialized ? gettid() : 0, funcname);
   else
      printe("%s: TID %d: %s exited\n", ts.getBuffer(), threads_initialized ? gettid() : 0, funcname);
}

char *remove_trailing_newlines(char *str) {
   int i = strlen(str);
   while (i && (str[i - 1] == '\n'))
      str[--i] = '\0';
   return str;
}

char *remove_trailing_blanks(char *str) {
   int i = strlen(str);
   while (i && (str[--i] == ' '))
      str[i] = '\0';
   return str;
}

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
void showCallStack() {
   QoreListNode *callStack = getCallStackList();
   int sl, el;
   const char *fn = get_pgm_counter(sl, el);
   if (sl == el)
      printf("terminated at %s:%d\n", fn, sl);
   else
      printf("terminated at %s:%d-%d\n", fn, sl, el);
   if (callStack && callStack->size()) {
      printe("call stack:\n");
      for (unsigned i = 0; i < callStack->size(); i++) {
         QoreHashNode *h = reinterpret_cast<QoreHashNode *>(callStack->retrieve_entry(i));
	 QoreStringNode *func = reinterpret_cast<QoreStringNode *>(h->getKeyValue("function"));
	 QoreStringNode *file = reinterpret_cast<QoreStringNode *>(h->getKeyValue("file"));
	 QoreStringNode *type = reinterpret_cast<QoreStringNode *>(h->getKeyValue("type"));
         printe(" %2d: %s() (%s line %d, %s)\n", i + 1, func->getBuffer(), file->getBuffer(), 
		(int)(reinterpret_cast<QoreBigIntNode *>(h->getKeyValue("line")))->val, type->getBuffer());
      }
   }
}
#endif

void parse_error(const char *fmt, ...) {
   printd(5, "parse_error(\"%s\", ...) called\n", fmt);

   QoreStringNode *desc = new QoreStringNode();
   while (true) {
      va_list args;
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }
   qore_program_private::makeParseException(getProgram(), desc);
}

void parse_error(const char *file, int sline, int eline, const char *fmt, ...) {
   printd(5, "parse_error(file=%s, sline=%d, eline=%d, \"%s\", ...) called\n", file, sline, eline, fmt);

   QoreStringNode *desc = new QoreStringNode;
   while (true) {
      va_list args;
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }
   qore_program_private::makeParseException(getProgram(), QoreProgramLocation(sline, eline, file), desc);
}

void parseException(const char *err, QoreStringNode *desc) {
   printd(5, "parseException(%s, %s) called\n", err, desc->getBuffer());
   qore_program_private::makeParseException(getProgram(), err, desc);
}

void parseException(const char *err, const char *fmt, ...) {
   QoreStringNode *desc = new QoreStringNode;
   while (true) {
      va_list args;
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }
   parseException(err, desc);
}

// returns 1 for success
static inline int tryIncludeDir(class QoreString *dir, const char *file) {
   // make fully-justified path
   if (dir->strlen() && dir->getBuffer()[dir->strlen() - 1] != '/')
      dir->concat('/');
   dir->concat(file);
   struct stat sb;
   //printd(5, "trying \"%s\"\n", dir->getBuffer());
   return !stat(dir->getBuffer(), &sb);
}

// FIXME: this could be a lot more efficient
QoreString *findFileInPath(const char *file, const char *path) {
   // if path is empty, return null
   if (!path || !path[0])
      return 0;

   // duplicate string for invasive searches
   QoreString plist(path);
   char *idir = (char *)plist.getBuffer();
   //printd(5, "findFileInEnvPath() %s=%s\n", varname, idir);

   // try each directory
   while (char *p = strchr(idir, ':')) {
      if (p != idir) {
	 *p = '\0';
	 TempString str(new QoreString(idir));
	 if (tryIncludeDir(*str, file))
	    return str.release();
      }
      idir = p + 1;
   }

   // try last directory
   if (idir[0]) {
      TempString str(new QoreString(idir));
      if (tryIncludeDir(*str, file))
	 return str.release();
   }

   return 0;
}

// FIXME: this could be a lot more efficient
QoreString *findFileInEnvPath(const char *file, const char *varname) {
   //printd(5, "findFileInEnvPath(file=%s var=%s)\n", file, varname);

   // if the file is an absolute path, then return it
   if (file[0] == '/')
      return new QoreString(file);

   // get path from environment
   QoreString str;
   if (SysEnv.get(varname, str))
      return 0;

   return findFileInPath(file, str.getBuffer());
}
