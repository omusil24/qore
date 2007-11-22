/*
 QC_QLocale.cc
 
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

#include "QC_QLocale.h"

int CID_QLOCALE;
class QoreClass *QC_QLocale = 0;

//QLocale ()
//QLocale ( const QString & name )
//QLocale ( Language language, Country country = AnyCountry )
//QLocale ( const QLocale & other )
static void QLOCALE_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QLOCALE, new QoreQLocale());
      return;
   }
   if (p && p->type == NT_STRING) {
      QString name;
      if (get_qstring(p, name, xsink))
	 return;
      self->setPrivate(CID_QLOCALE, new QoreQLocale(name));
      return;
   }
   QLocale::Language language = (QLocale::Language)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QLocale::Country country = (QLocale::Country)(p ? p->getAsInt() : 0);
   self->setPrivate(CID_QLOCALE, new QoreQLocale(language, country));
   return;
}

static void QLOCALE_copy(class Object *self, class Object *old, class QoreQLocale *ql, ExceptionSink *xsink)
{
   self->setPrivate(CID_QLOCALE, new QoreQLocale(*ql));
}

//Country country () const
static QoreNode *QLOCALE_country(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ql->country());
}

//QString dateFormat ( FormatType format = LongFormat ) const
static QoreNode *QLOCALE_dateFormat(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QLocale::FormatType format = (QLocale::FormatType)(p ? p->getAsInt() : 0);
   return new QoreNode(new QoreString(ql->dateFormat(format).toUtf8().data(), QCS_UTF8));
}

//QString dayName ( int day, FormatType type = LongFormat ) const
static QoreNode *QLOCALE_dayName(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int day = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QLocale::FormatType type = (QLocale::FormatType)(p ? p->getAsInt() : 0);
   return new QoreNode(new QoreString(ql->dayName(day, type).toUtf8().data(), QCS_UTF8));
}

//QChar decimalPoint () const
static QoreNode *QLOCALE_decimalPoint(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   QoreString *rv_str = new QoreString(QCS_UTF8);
   QChar rv_qc = ql->decimalPoint();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return new QoreNode(rv_str);
}

//QChar exponential () const
static QoreNode *QLOCALE_exponential(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   QoreString *rv_str = new QoreString(QCS_UTF8);
   QChar rv_qc = ql->exponential();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return new QoreNode(rv_str);
}

//QChar groupSeparator () const
static QoreNode *QLOCALE_groupSeparator(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   QoreString *rv_str = new QoreString(QCS_UTF8);
   QChar rv_qc = ql->groupSeparator();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return new QoreNode(rv_str);
}

//Language language () const
static QoreNode *QLOCALE_language(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ql->language());
}

//QString monthName ( int month, FormatType type = LongFormat ) const
static QoreNode *QLOCALE_monthName(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int month = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QLocale::FormatType type = (QLocale::FormatType)(!is_nothing(p) ? p->getAsInt() : QLocale::LongFormat);
   return new QoreNode(new QoreString(ql->monthName(month, type).toUtf8().data(), QCS_UTF8));
}

//QString name () const
static QoreNode *QLOCALE_name(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(ql->name().toUtf8().data(), QCS_UTF8));
}

//QChar negativeSign () const
static QoreNode *QLOCALE_negativeSign(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   QoreString *rv_str = new QoreString(QCS_UTF8);
   QChar rv_qc = ql->negativeSign();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return new QoreNode(rv_str);
}

//NumberOptions numberOptions () const
static QoreNode *QLOCALE_numberOptions(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ql->numberOptions());
}

//QChar percent () const
static QoreNode *QLOCALE_percent(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   QoreString *rv_str = new QoreString(QCS_UTF8);
   QChar rv_qc = ql->percent();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return new QoreNode(rv_str);
}

//void setNumberOptions ( NumberOptions options )
static QoreNode *QLOCALE_setNumberOptions(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QLocale::NumberOptions options = (QLocale::NumberOptions)(p ? p->getAsInt() : 0);
   ql->setNumberOptions(options);
   return 0;
}

//QString timeFormat ( FormatType format = LongFormat ) const
static QoreNode *QLOCALE_timeFormat(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QLocale::FormatType format = (QLocale::FormatType)(p ? p->getAsInt() : 0);
   return new QoreNode(new QoreString(ql->timeFormat(format).toUtf8().data(), QCS_UTF8));
}

//double toDouble ( const QString & s, bool * ok = 0 ) const
static QoreNode *QLOCALE_toDouble(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString s;
   if (get_qstring(p, s, xsink))
      return 0;
   bool ok;
   double rv = ql->toDouble(s, &ok);
   if (!ok) {
      xsink->raiseException("QLOCALE-TODOUBLE-ERROR", "error encountered in QLocale::toDouble()");
      return 0;
   }
      
   return new QoreNode(rv);
}

//qlonglong toLongLong ( const QString & s, bool * ok = 0, int base = 0 ) const
static QoreNode *QLOCALE_toLongLong(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString s;
   if (get_qstring(p, s, xsink))
      return 0;

   p = get_param(params, 1);
   int base = p ? p->getAsInt() : 0;

   bool ok;
   int64 rv = ql->toLongLong(s, &ok, base);
   if (!ok) {
      xsink->raiseException("QLOCALE-TOLONGLONG-ERROR", "error encountered in QLocale::toLongLong()");
      return 0;
   }
   return new QoreNode(rv);
}

////QString toString ( qlonglong i ) const
////QString toString ( const QDate & date, const QString & format ) const
////QString toString ( const QDate & date, FormatType format = LongFormat ) const
////QString toString ( const QTime & time, const QString & format ) const
////QString toString ( const QTime & time, FormatType format = LongFormat ) const
////QString toString ( qulonglong i ) const
////QString toString ( double i, char f = 'g', int prec = 6 ) const
////QString toString ( short i ) const
////QString toString ( ushort i ) const
////QString toString ( int i ) const
////QString toString ( uint i ) const
////QString toString ( float i, char f = 'g', int prec = 6 ) const
//static QoreNode *QLOCALE_toString(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   if (p && p->type == NT_???) {
//      QDate date;
//      if (get_qdate(p, date, xsink))
//         return 0;
//   p = get_param(params, 1);
//   QLocale::char f = (QLocale::char)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   int prec = !is_nothing(p) ? p->getAsInt() : 6;
//   return new QoreNode(new QoreString(ql->toString(date, f, prec).toUtf8().data(), QCS_UTF8));
//   }
//   if (p && p->type == NT_???) {
//      QDate date;
//      if (get_qdate(p, date, xsink))
//         return 0;
//   p = get_param(params, 1);
//   QLocale::char f = (QLocale::char)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   int prec = !is_nothing(p) ? p->getAsInt() : 6;
//   return new QoreNode(new QoreString(ql->toString(date, f, prec).toUtf8().data(), QCS_UTF8));
//   }
//   if (p && p->type == NT_???) {
//      QTime time;
//      if (get_qtime(p, time, xsink))
//         return 0;
//   p = get_param(params, 1);
//   QLocale::char f = (QLocale::char)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   int prec = !is_nothing(p) ? p->getAsInt() : 6;
//   return new QoreNode(new QoreString(ql->toString(time, f, prec).toUtf8().data(), QCS_UTF8));
//   }
//   if (p && p->type == NT_???) {
//      QTime time;
//      if (get_qtime(p, time, xsink))
//         return 0;
//   p = get_param(params, 1);
//   QLocale::char f = (QLocale::char)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   int prec = !is_nothing(p) ? p->getAsInt() : 6;
//   return new QoreNode(new QoreString(ql->toString(time, f, prec).toUtf8().data(), QCS_UTF8));
//   }
//   if (p && p->type == NT_INT) {
//      QLocale::qlonglong i = (QLocale::qlonglong)(p ? p->getAsInt() : 0);
//      return new QoreNode(new QoreString(ql->toString(i).toUtf8().data(), QCS_UTF8));
//   }
//   if (p && p->type == NT_INT) {
//      QLocale::qulonglong i = (QLocale::qulonglong)(p ? p->getAsInt() : 0);
//      return new QoreNode(new QoreString(ql->toString(i).toUtf8().data(), QCS_UTF8));
//   }
//   if (p && p->type == NT_INT) {
//      QLocale::short i = (QLocale::short)(p ? p->getAsInt() : 0);
//      return new QoreNode(new QoreString(ql->toString(i).toUtf8().data(), QCS_UTF8));
//   }
//   if (p && p->type == NT_INT) {
//      QLocale::ushort i = (QLocale::ushort)(p ? p->getAsInt() : 0);
//      return new QoreNode(new QoreString(ql->toString(i).toUtf8().data(), QCS_UTF8));
//   }
//   if (p && p->type == NT_INT) {
//      int i = p ? p->getAsInt() : 0;
//      return new QoreNode(new QoreString(ql->toString(i).toUtf8().data(), QCS_UTF8));
//   }
//   if (p && p->type == NT_INT) {
//      unsigned i = p ? p->getAsBigInt() : 0;
//      return new QoreNode(new QoreString(ql->toString(i).toUtf8().data(), QCS_UTF8));
//   }
//   double i = p ? p->getAsFloat() : 0.0;
//   p = get_param(params, 1);
//   QLocale::char f = (QLocale::char)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   int prec = !is_nothing(p) ? p->getAsInt() : 6;
//   return new QoreNode(new QoreString(ql->toString(i, f, prec).toUtf8().data(), QCS_UTF8));
//}

//QChar zeroDigit () const
static QoreNode *QLOCALE_zeroDigit(Object *self, QoreQLocale *ql, QoreNode *params, ExceptionSink *xsink)
{
   QoreString *rv_str = new QoreString(QCS_UTF8);
   QChar rv_qc = ql->zeroDigit();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return new QoreNode(rv_str);
}

static QoreNode *f_QLocale_countriesForLanguage(class QoreNode *params, class ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QLocale::Language language = (QLocale::Language)(p ? p->getAsInt() : 0);
   QList<QLocale::Country> ql = QLocale::countriesForLanguage(language);
   if (ql.empty())
      return 0;

   QoreList *l = new QoreList();
   for (int i = 0; i < ql.count(); ++i)
      l->push(new QoreNode((int64)ql.at(i)));
   return new QoreNode(l);
}

static QoreNode *f_QLocale_languageToString(class QoreNode *params, class ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QLocale::Language language = (QLocale::Language)(p ? p->getAsInt() : 0);
   return new QoreNode(new QoreString(QLocale::languageToString(language).toUtf8().data(), QCS_UTF8));
}

static QoreNode *f_QLocale_countryToString(class QoreNode *params, class ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QLocale::Country country = (QLocale::Country)(p ? p->getAsInt() : 0);
   return new QoreNode(new QoreString(QLocale::countryToString(country).toUtf8().data(), QCS_UTF8));
}

QoreClass *initQLocaleClass()
{
   QC_QLocale = new QoreClass("QLocale", QDOM_GUI);
   CID_QLOCALE = QC_QLocale->getID();

   QC_QLocale->setConstructor(QLOCALE_constructor);
   QC_QLocale->setCopy((q_copy_t)QLOCALE_copy);

   QC_QLocale->addMethod("country",                     (q_method_t)QLOCALE_country);
   QC_QLocale->addMethod("dateFormat",                  (q_method_t)QLOCALE_dateFormat);
   QC_QLocale->addMethod("dayName",                     (q_method_t)QLOCALE_dayName);
   QC_QLocale->addMethod("decimalPoint",                (q_method_t)QLOCALE_decimalPoint);
   QC_QLocale->addMethod("exponential",                 (q_method_t)QLOCALE_exponential);
   QC_QLocale->addMethod("groupSeparator",              (q_method_t)QLOCALE_groupSeparator);
   QC_QLocale->addMethod("language",                    (q_method_t)QLOCALE_language);
   QC_QLocale->addMethod("monthName",                   (q_method_t)QLOCALE_monthName);
   QC_QLocale->addMethod("name",                        (q_method_t)QLOCALE_name);
   QC_QLocale->addMethod("negativeSign",                (q_method_t)QLOCALE_negativeSign);
   QC_QLocale->addMethod("numberOptions",               (q_method_t)QLOCALE_numberOptions);
   QC_QLocale->addMethod("percent",                     (q_method_t)QLOCALE_percent);
   QC_QLocale->addMethod("setNumberOptions",            (q_method_t)QLOCALE_setNumberOptions);
   QC_QLocale->addMethod("timeFormat",                  (q_method_t)QLOCALE_timeFormat);
   QC_QLocale->addMethod("toDouble",                    (q_method_t)QLOCALE_toDouble);
   QC_QLocale->addMethod("toFloat",                     (q_method_t)QLOCALE_toDouble);
   QC_QLocale->addMethod("toInt",                       (q_method_t)QLOCALE_toLongLong);
   QC_QLocale->addMethod("toLongLong",                  (q_method_t)QLOCALE_toLongLong);
   QC_QLocale->addMethod("toShort",                     (q_method_t)QLOCALE_toLongLong);
   //QC_QLocale->addMethod("toString",                    (q_method_t)QLOCALE_toString);
   QC_QLocale->addMethod("toUInt",                      (q_method_t)QLOCALE_toLongLong);
   QC_QLocale->addMethod("toULongLong",                 (q_method_t)QLOCALE_toLongLong);
   QC_QLocale->addMethod("toUShort",                    (q_method_t)QLOCALE_toLongLong);
   QC_QLocale->addMethod("zeroDigit",                   (q_method_t)QLOCALE_zeroDigit);

   return QC_QLocale;
}

void initQLocaleStaticFunctions()
{
   // add builtin functions
   builtinFunctions.add("QLocale_countriesForLanguage",       f_QLocale_countriesForLanguage);
   builtinFunctions.add("QLocale_languageToString",           f_QLocale_languageToString);
   builtinFunctions.add("QLocale_countryToString",            f_QLocale_countryToString);
}
