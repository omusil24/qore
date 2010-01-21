/*
  QC_File.cc

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
#include <qore/intern/QC_File.h>

qore_classid_t CID_FILE;
QoreClass *QC_File = 0;

static void FILE_system_constructor(QoreObject *self, int fd, va_list args) {
   //printd(5, "FILE_system_constructor() self=%08p, descriptor=%d\n", self, fd);
   File *f = new File(QCS_DEFAULT);
   f->makeSpecial(fd);
   self->setPrivate(CID_FILE, f);
}

static void FILE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   // get character set name if available
   const QoreEncoding *cs;
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (p0) {
      cs = QEM.findCreate(p0);
      //printd(0, "FILE_constructor() str=%s, cs=%08p\n", p0->getBuffer(), cs);
   }
   else
      cs = QCS_DEFAULT;

   self->setPrivate(CID_FILE, new File(cs));
}

static void FILE_copy(QoreObject *self, QoreObject *old, class File *f, ExceptionSink *xsink) {
   self->setPrivate(CID_FILE, new File(f->getEncoding()));
}

// open(filename, [flags, mode, charset])
static AbstractQoreNode *FILE_open(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   p0 = test_string_param(params, 0);
   if (!p0) {
      xsink->raiseException("FILE-OPEN-PARAMETER-ERROR", "expecting string filename as first argument of File::open()");
      return 0;
   }

   int flags, mode;
   const AbstractQoreNode *p = get_param(params, 1);
   if (!is_nothing(p))
      flags = p->getAsInt();
   else
      flags = O_RDONLY;

   p = get_param(params, 2);
   if (!is_nothing(p))
      mode = p->getAsInt();
   else
      mode = 0666;

   const QoreStringNode *pstr = test_string_param(params, 3);
   const QoreEncoding *charset;
   if (pstr)
      charset = QEM.findCreate(pstr);
   else
      charset = QCS_DEFAULT;

   return new QoreBigIntNode(f->open(p0->getBuffer(), flags, mode, charset));
}

// open2(filename, [flags, mode, charset])
// throws an exception if there is an error
static AbstractQoreNode *FILE_open2(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   int flags, mode;
   const QoreEncoding *charset;
   p0 = test_string_param(params, 0);
   if (!p0) {
      xsink->raiseException("FILE-OPEN2-PARAMETER-ERROR", "expecting string filename as first argument of File::open2()");
      return 0;
   }
   
   const AbstractQoreNode *p = get_param(params, 1);
   if (!is_nothing(p))
      flags = p->getAsInt();
   else
      flags = O_RDONLY;

   p = get_param(params, 2);
   if (!is_nothing(p))
      mode = p->getAsInt();
   else
      mode = 0666;

   const QoreStringNode *pstr = test_string_param(params, 3);
   if (pstr)
      charset = QEM.findCreate(pstr);
   else
      charset = QCS_DEFAULT;
   
   f->open2(xsink, p0->getBuffer(), flags, mode, charset);
   return 0;
}

static AbstractQoreNode *FILE_close(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(f->close());
}

static AbstractQoreNode *FILE_sync(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(f->sync());
}

static AbstractQoreNode *FILE_read(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;

   if (!size) {
      xsink->raiseException("FILE-READ-PARAMETER-ERROR", "expecting size as first parameter of File::read()");
      return 0;
   }

   // get timeout
   int timeout_ms = getMsMinusOneInt(get_param(params, 1));

   return f->read(size, timeout_ms, xsink);
}

static AbstractQoreNode *FILE_readu1(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   unsigned char c;
   if (f->readu1(&c, xsink))
      return 0;
   return new QoreBigIntNode(c);
}

static AbstractQoreNode *FILE_readu2(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   unsigned short s;
   if (f->readu2(&s, xsink))
      return 0;
   return new QoreBigIntNode(s);
}

static AbstractQoreNode *FILE_readu4(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   unsigned int i;
   if (f->readu4(&i, xsink))
      return 0;
   
   return new QoreBigIntNode(i);
}

static AbstractQoreNode *FILE_readu2LSB(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   unsigned short s;
   if (f->readu2LSB(&s, xsink))
      return 0;
   
   return new QoreBigIntNode(s);
}

static AbstractQoreNode *FILE_readu4LSB(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   unsigned int i;
   if (f->readu4LSB(&i, xsink))
      return 0;
   
   return new QoreBigIntNode(i);
}

static AbstractQoreNode *FILE_readi1(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   char c;
   if (f->readi1(&c, xsink))
      return 0;
   return new QoreBigIntNode(c);
}

static AbstractQoreNode *FILE_readi2(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   short s;
   if (f->readi2(&s, xsink))
      return 0;
   return new QoreBigIntNode(s);
}

static AbstractQoreNode *FILE_readi4(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   int i;
   if (f->readi4(&i, xsink))
      return 0;

   return new QoreBigIntNode(i);
}

static AbstractQoreNode *FILE_readi8(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   int64 i;
   if (f->readi8(&i, xsink))
      return 0;
   
   return new QoreBigIntNode(i);
}

static AbstractQoreNode *FILE_readi2LSB(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   short s;
   if (f->readi2LSB(&s, xsink))
      return 0;

   return new QoreBigIntNode(s);
}

static AbstractQoreNode *FILE_readi4LSB(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   int i;
   if (f->readi4LSB(&i, xsink))
      return 0;

   return new QoreBigIntNode(i);
}

static AbstractQoreNode *FILE_readi8LSB(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   int64 i;
   if (f->readi8LSB(&i, xsink))
      return 0;
   
   return new QoreBigIntNode(i);
}

static AbstractQoreNode *FILE_readBinary(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);

   int size = p ? p->getAsInt() : 0;
   if (!size) {
      xsink->raiseException("FILE-READ-BINARY-PARAMETER-ERROR", "expecting size as first parameter of File::readBinary()");
      return 0;
   }

   // get timeout
   int timeout_ms = getMsMinusOneInt(get_param(params, 1));

   return f->readBinary(size, timeout_ms, xsink);
}

static AbstractQoreNode *FILE_write(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->getType() != NT_STRING && p0->getType() != NT_BINARY)) {
      xsink->raiseException("FILE-WRITE-PARAMETER-ERROR", "expecting string or binary object to write as first parameter of File::write()");
      return 0;
   }

   int rc;
   if (p0->getType() == NT_STRING)
      rc = f->write(reinterpret_cast<const QoreStringNode *>(p0), xsink);
   else
      rc = f->write(reinterpret_cast<const BinaryNode *>(p0), xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_writei1(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   char c;
   if (!p0)
      c = 0;
   else
      c = p0->getAsInt();

   int rc = f->writei1(c, xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_writei2(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   short s;
   if (!p0)
      s = 0;
   else
      s = p0->getAsInt();

   int rc = f->writei2(s, xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_writei4(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   int i;
   if (!p0)
      i = 0;
   else
      i = p0->getAsInt();

   int rc = f->writei4(i, xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_writei8(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   int64 i;
   if (!p0)
      i = 0;
   else
      i = p0->getAsBigInt();
   
   int rc = f->writei8(i, xsink);
   if (xsink->isEvent())
      return 0;
   
   return new QoreBigIntNode(rc);
}
static AbstractQoreNode *FILE_writei2LSB(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   short s;
   if (!p0)
      s = 0;
   else
      s = p0->getAsInt();

   int rc = f->writei2LSB(s, xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_writei4LSB(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   int i;
   if (!p0)
      i = 0;
   else
      i = p0->getAsInt();

   int rc = f->writei4LSB(i, xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_writei8LSB(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   int64 i;
   if (!p0)
      i = 0;
   else
      i = p0->getAsBigInt();
   
   int rc = f->writei8LSB(i, xsink);
   if (xsink->isEvent())
      return 0;
   
   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_printf(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_sprintf(params, 0, 0, xsink));
   if (!str)
      return 0;

   int rc = f->write(*str, xsink);

   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_vprintf(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_vsprintf(params, 0, 0, xsink));
   if (!str)
      return 0;

   int rc = f->write(*str, xsink);

   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_f_printf(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_sprintf(params, 1, 0, xsink));
   if (!str)
      return 0;

   int rc = f->write(*str, xsink);

   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_f_vprintf(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_vsprintf(params, 1, 0, xsink));
   if (!str)
      return 0;

   int rc = f->write(*str, xsink);

   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_readLine(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   return f->readLine(xsink);
}

static AbstractQoreNode *FILE_setCharset(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreEncoding *charset;
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (p0)
      charset = QEM.findCreate(p0);
   else
      charset = QCS_DEFAULT;

   f->setEncoding(charset);
   return 0;
}

static AbstractQoreNode *FILE_getCharset(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(f->getEncoding()->getCode());
}

static AbstractQoreNode *FILE_setPos(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   int pos;
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (p0)
      pos = p0->getAsInt();
   else
      pos = 0;

   return new QoreBigIntNode(f->setPos(pos));
}

/*
static AbstractQoreNode *FILE_setPosFromEnd(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   //f->open();
   return 0;
}

static AbstractQoreNode *FILE_setPosFromCurrent(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   //f->open();
   return 0;
}
*/

static AbstractQoreNode *FILE_getPos(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(f->getPos());
}

static AbstractQoreNode *FILE_getchar(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return f->getchar();
}

static int lock_intern(struct flock &fl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   fl.l_type = p ? p->getAsInt() : 0;

   p = get_param(params, 1);
   fl.l_start = !is_nothing(p) ? p->getAsInt() : 0;

   p = get_param(params, 2);
   if (!is_nothing(p)) {
      fl.l_len = p->getAsInt();
      if (fl.l_len < 0) {
	 xsink->raiseException("FILE-LOCK-ERROR", "length of locked area cannot be negative (value passed=%d)", fl.l_len);
	 return -1;
      }
   }
   else
      fl.l_len = 0;
   
   p = get_param(params, 3);
   fl.l_whence = !is_nothing(p) ? p->getAsInt() : SEEK_SET;
   return 0;
}

static AbstractQoreNode *FILE_lock(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   struct flock fl;

   if (lock_intern(fl, params, xsink))
      return 0;

   int rc = f->lock(fl, xsink);
   if (*xsink)
      return 0;
   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_lockBlocking(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   struct flock fl;

   if (lock_intern(fl, params, xsink))
      return 0;

   f->lockBlocking(fl, xsink);
   return 0;
}

static AbstractQoreNode *FILE_getLockInfo(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink)
{
   struct flock fl;
   if (f->getLockInfo(fl, xsink))
      return 0;

   QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("start", new QoreBigIntNode(fl.l_start), xsink);
   h->setKeyValue("len", new QoreBigIntNode(fl.l_len), xsink);
   h->setKeyValue("pid", new QoreBigIntNode(fl.l_pid), xsink);
   h->setKeyValue("type", new QoreBigIntNode(fl.l_type), xsink);
   h->setKeyValue("whence", new QoreBigIntNode(fl.l_whence), xsink);

   return h;
}

static AbstractQoreNode *FILE_chown(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   uid_t owner = (uid_t)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   gid_t group = (gid_t)(p ? p->getAsInt() : 0);
   f->chown(owner, group, xsink);
   return 0;
}

static AbstractQoreNode *FILE_isDataAvailable(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   int timeout_ms = getMsZeroInt(p);
   bool rc = f->isDataAvailable(timeout_ms, xsink);
   return *xsink ? 0 : get_bool_node(rc);
}

static AbstractQoreNode *FILE_getTerminalAttributes(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   QoreObject *p = test_object_param(params, 0);
   QoreTermIOS *ios = p ? (QoreTermIOS *)p->getReferencedPrivateData(CID_TERMIOS, xsink) : 0;
   if (!ios) {
      if (!*xsink)
         xsink->raiseException("TERMIOS-GETTERMINALATTRIBUTES-ERROR", "expecting a TermIOS object as sole argument to File::getTerminalAttributes()");
      return 0;
   }
   ReferenceHolder<QoreTermIOS> holder(ios, xsink);
   f->getTerminalAttributes(ios, xsink);
   return 0;
}

static AbstractQoreNode *FILE_setTerminalAttributes(QoreObject *self, class File *f, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   int action = p0 ? p0->getAsInt() : 0;
   
   QoreObject *p1 = test_object_param(params, 1);
   QoreTermIOS *ios = p1 ? (QoreTermIOS *)p1->getReferencedPrivateData(CID_TERMIOS, xsink) : 0;
   if (!ios) {
      if (!*xsink)
         xsink->raiseException("TERMIOS-SETTERMINALATTRIBUTES-ERROR", "expecting a TermIOS object as second argument to File::setTerminalAttributes()");
      return 0;
   }
   ReferenceHolder<QoreTermIOS> holder(ios, xsink);
   f->setTerminalAttributes(action, ios, xsink);
   return 0;
}

static AbstractQoreNode *FILE_setEventQueue(QoreObject *self, File *f, const QoreListNode *params, ExceptionSink *xsink) {
    const QoreObject *o = test_object_param(params, 0);
    Queue *q = o ? (Queue *)o->getReferencedPrivateData(CID_QUEUE, xsink) : 0;
    if (*xsink)
        return 0;
    // pass reference from QoreObject::getReferencedPrivateData() to function
    f->setEventQueue(q, xsink);
    return 0;
}

QoreClass *initFileClass(QoreClass *QC_TermIOS) {
   QORE_TRACE("initFileClass()");

   QC_File = new QoreClass("File", QDOM_FILESYSTEM);
   CID_FILE = QC_File->getID();

   QC_File->setSystemConstructor(FILE_system_constructor);
   QC_File->setConstructor(FILE_constructor);
   QC_File->setCopy((q_copy_t)FILE_copy);

   QC_File->addMethodExtended("open",              (q_method_t)FILE_open, false, QDOM_DEFAULT, &bigIntTypeInfo, 1, &stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_File->addMethodExtended("open2",             (q_method_t)FILE_open2, false, QDOM_DEFAULT, &nothingTypeInfo, 1, &stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_File->addMethodExtended("close",             (q_method_t)FILE_close, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("sync",              (q_method_t)FILE_sync, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("read",              (q_method_t)FILE_read, false, QDOM_DEFAULT, &stringTypeInfo);
   QC_File->addMethodExtended("readu1",            (q_method_t)FILE_readu1, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("readu2",            (q_method_t)FILE_readu2, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("readu4",            (q_method_t)FILE_readu4, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("readu2LSB",         (q_method_t)FILE_readu2LSB, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("readu4LSB",         (q_method_t)FILE_readu4LSB, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("readi1",            (q_method_t)FILE_readi1, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("readi2",            (q_method_t)FILE_readi2, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("readi4",            (q_method_t)FILE_readi4, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("readi8",            (q_method_t)FILE_readi8, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("readi2LSB",         (q_method_t)FILE_readi2LSB, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("readi4LSB",         (q_method_t)FILE_readi4LSB, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("readi8LSB",         (q_method_t)FILE_readi8LSB, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("readBinary",        (q_method_t)FILE_readBinary, false, QDOM_DEFAULT, &stringTypeInfo);
   QC_File->addMethodExtended("write",             (q_method_t)FILE_write, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("writei1",           (q_method_t)FILE_writei1, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("writei2",           (q_method_t)FILE_writei2, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("writei4",           (q_method_t)FILE_writei4, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("writei8",           (q_method_t)FILE_writei8, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("writei2LSB",        (q_method_t)FILE_writei2LSB, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("writei4LSB",        (q_method_t)FILE_writei4LSB, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("writei8LSB",        (q_method_t)FILE_writei8LSB, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("readLine",          (q_method_t)FILE_readLine, false, QDOM_DEFAULT, &stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_File->addMethodExtended("setCharset",        (q_method_t)FILE_setCharset, false, QDOM_DEFAULT, &nothingTypeInfo);
   QC_File->addMethodExtended("getCharset",        (q_method_t)FILE_getCharset, false, QDOM_DEFAULT, &stringTypeInfo);
   QC_File->addMethodExtended("setPos",            (q_method_t)FILE_setPos, false, QDOM_DEFAULT, &bigIntTypeInfo);
   //QC_File->addMethod("setPosFromEnd",     (q_method_t)FILE_setPosFromEnd);
   //QC_File->addMethod("setPosFromCurrent", (q_method_t)FILE_setPosFromCurrent);
   QC_File->addMethodExtended("getPos",            (q_method_t)FILE_getPos, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("getchar",           (q_method_t)FILE_getchar, false, QDOM_DEFAULT, &stringTypeInfo);
   QC_File->addMethodExtended("printf",            (q_method_t)FILE_printf, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("vprintf",           (q_method_t)FILE_vprintf, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("f_printf",          (q_method_t)FILE_f_printf, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("f_vprintf",         (q_method_t)FILE_f_vprintf, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("lock",              (q_method_t)FILE_lock, false, QDOM_DEFAULT, &bigIntTypeInfo);
   QC_File->addMethodExtended("lockBlocking",      (q_method_t)FILE_lockBlocking, false, QDOM_DEFAULT, &nothingTypeInfo);
   QC_File->addMethodExtended("getLockInfo",       (q_method_t)FILE_getLockInfo, false, QDOM_DEFAULT, &hashTypeInfo);
   QC_File->addMethodExtended("chown",             (q_method_t)FILE_chown, false, QDOM_DEFAULT, &nothingTypeInfo);
   QC_File->addMethodExtended("isDataAvailable",        (q_method_t)FILE_isDataAvailable, false, QDOM_DEFAULT, &boolTypeInfo);
   QC_File->addMethodExtended("getTerminalAttributes",  (q_method_t)FILE_getTerminalAttributes, false, QDOM_DEFAULT, &nothingTypeInfo, 1, QC_TermIOS->getTypeInfo(), QORE_PARAM_NO_ARG);
   QC_File->addMethodExtended("setTerminalAttributes",  (q_method_t)FILE_setTerminalAttributes, false, QDOM_DEFAULT, &nothingTypeInfo, 2, QORE_PARAM_NO_ARG, QORE_PARAM_NO_ARG, QC_TermIOS->getTypeInfo(), QORE_PARAM_NO_ARG);
   QC_File->addMethodExtended("setEventQueue",          (q_method_t)FILE_setEventQueue, false, QDOM_DEFAULT, &nothingTypeInfo);

   return QC_File;
}
