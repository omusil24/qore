/*
  lib/ql_xml.cc

  Qore JSON (JavaScript Object Notation) functions

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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
#include <qore/ql_json.h>

#include <ctype.h>
#include <stdlib.h>

// returns 0 for OK
int cmp_rest_token(char *&p, char *tok)
{
   p++;
   while (*tok)
      if ((*(p++)) != (*(tok++)))
	 return -1;
   if (!*p || *p == ',' || *p == ']' || *p == '}')
      return 0;
   if (isblank(*p) || (*p) == '\r' || (*p) == '\n')
   {
      p++;
      return 0;
   }
   return -1;
} 

static void skip_whitespace(char *&buf, int &line_number)
{
   while (*buf)
   {
      if (isblank(*buf) || (*buf) == '\r')
      {
	 buf++;
	 continue;
      }
      if ((*buf) == '\n')
      {
	 line_number++;
	 buf++;
	 continue;
      }
      break;
   }
}

// '"' has already been read and the buffer is set to this character
static class QoreString *getJSONStringToken(char *&buf, int &line_number, class QoreEncoding *enc, class ExceptionSink *xsink)
{
   // increment buffer to first character of string
   buf++;
   class QoreString *str = new QoreString(enc);
   while (*buf)
   {
      if (*buf == '"')
      {
	 buf++;
	 return str;
      }
      if (*buf == '\\')
      {
	 buf++;
	 if (*buf == '"' || *buf == '/' || *buf == '\\')
	 {
	    str->concat(*buf);
	    buf++;
	    continue;
	 }
	 // FIXME: implement unicode character parsing
	 if (*buf == '\b')
	    str->concat('\b');
	 else if (*buf == '\f')
	    str->concat('\f');
	 else if (*buf == '\n')
	    str->concat('\n');
	 else if (*buf == '\r')
	    str->concat('\r');
	 else if (*buf == '\t')
	    str->concat('\t');
	 else
	    break;
	 buf++;
	 continue;
      }
      if (*buf == '\n')
	 line_number++;
      str->concat(*buf);
      buf++;
   }
   delete str;
   xsink->raiseException("JSON_PARSE_ERROR", "premature end of input at line %d while parsing JSON string", line_number);
   return NULL;
}

static class QoreNode *getJSONValue(char *&buf, int &line_number, class QoreEncoding *enc, class ExceptionSink *xsink);

// '{' has already been read and the buffer is set to this character
static class QoreNode *getJSONObject(char *&buf, int &line_number, class QoreEncoding *enc, class ExceptionSink *xsink)
{
   // increment buffer to first character of object description
   buf++;
   class Hash *h = new Hash();

   // get either string or '}'
   skip_whitespace(buf, line_number);
      
   if (*buf == '}')
      return new QoreNode(h);

   while (*buf)
   {
      if (*buf != '"')
      {
	 //printd(5, "*buf='%c'\n", *buf);
	 if (h->size())
	    xsink->raiseException("JSON-PARSE-ERROR", "unexpected text encountered at line %d while parsing JSON object (expecting '\"' for key string)", line_number);
	 else
	    xsink->raiseException("JSON-PARSE-ERROR", "unexpected text encountered at line %d while parsing JSON object (expecting '\" or '}'')", line_number);
	 break;
      }

      // get key
      class QoreString *str = getJSONStringToken(buf, line_number, enc, xsink);
      if (!str)
	 break;

      //printd(5, "getJSONObject() key=%s\n", str->getBuffer());
      
      skip_whitespace(buf, line_number);
      if (*buf != ':')
      {
	 //printd(5, "*buf='%c'\n", *buf);
	 xsink->raiseException("JSON-PARSE-ERROR", "unexpected text encountered at line %d while parsing JSON object (expecting ':')", line_number);
	 break;
      }
      buf++;
      skip_whitespace(buf, line_number);

      // get value
      class QoreNode *val = getJSONValue(buf, line_number, enc, xsink);
      if (!val)
      {
	 if (!xsink->isException())
	    xsink->raiseException("JSON-PARSE-ERROR", "premature end of input at line %d while parsing JSON object (expecting JSON value for key '%s')", line_number, str->getBuffer());
	 delete str;
	 break;
      }
      h->setKeyValue(str, val, xsink);
      delete str;

      skip_whitespace(buf, line_number);
      if (*buf == '}')
      {
	 buf++;
	 return new QoreNode(h);
      }

      if (*buf != ',')
      {
	 xsink->raiseException("JSON-PARSE-ERROR", "unexpected text encountered at line %d while parsing JSON object (expecting ',' or '}')", line_number);
	 break;
      }
      buf++;
      
      skip_whitespace(buf, line_number);

   }
   h->derefAndDelete(NULL);
   return NULL;
}

// '[' has already been read and the buffer is set to this character
static class QoreNode *getJSONArray(char *&buf, int &line_number, class QoreEncoding *enc, class ExceptionSink *xsink)
{
   // increment buffer to first character of array description
   buf++;
   class List *l = new List();

   skip_whitespace(buf, line_number);
   if (*buf == ']')
      return new QoreNode(l);

   while (*buf)
   {
      //printd(5, "before getJSONValue() buf=%s\n", buf);
      class QoreNode *val = getJSONValue(buf, line_number, enc, xsink);
      if (!val)
      {
	 if (!xsink->isException())
	    xsink->raiseException("JSON-PARSE-ERROR", "premature end of input at line %d while parsing JSON array (expecting JSON value)", line_number);
	 break;
      }
      //printd(5, "after getJSONValue() buf=%s\n", buf);
      l->push(val);

      skip_whitespace(buf, line_number);
      if (*buf == ']')
      {
	 buf++;
	 return new QoreNode(l);
      }

      if (*buf != ',')
      {
	 //printd(5, "*buf='%c'\n", *buf);
	 xsink->raiseException("JSON-PARSE-ERROR", "unexpected text encountered at line %d while parsing JSON array (expecting ',' or ']')", line_number);
	 break;
      }
      buf++;
      
      skip_whitespace(buf, line_number);
   }
   l->derefAndDelete(NULL);
   return NULL;
}

static class QoreNode *getJSONValue(char *&buf, int &line_number, class QoreEncoding *enc, class ExceptionSink *xsink)
{
   // skip whitespace
   skip_whitespace(buf, line_number);
   if (!*buf)
      return NULL;

   // can expect: 't'rue, 'f'alse, '{', '[', '"'string...", integer, '.'digits
   if (*buf == '{')
      return getJSONObject(buf, line_number, enc, xsink);

   if (*buf == '[')
      return getJSONArray(buf, line_number, enc, xsink);

   if (*buf == '"')
   {
      class QoreString *str = getJSONStringToken(buf, line_number, enc, xsink);
      return str ? new QoreNode(str) : NULL;
   }

   // FIXME: implement parsing of JSON exponents
   if (isdigit(*buf) || (*buf) == '.')
   {
      // temporarily use a QoreString
      QoreString str;
      bool has_dot;
      if (*buf == '.')
      {
	 // add a leading zero
	 str.concat("0.");
	 has_dot = true;
      }
      else
      {
	 str.concat(*buf);
	 has_dot = false;
      }
      buf++;
      while (*buf)
      {
	 if (*buf == '.')
	 {
	    if (has_dot)
	    {
	       xsink->raiseException("JSON_PARSE_ERROR", "unexpected '.' in floating point number (too many '.' characters)");
	       return NULL;
	    }
	    has_dot = true;
	 }
	 // if another token follows then break but do not increment buffer position
	 else if (*buf == ',' || *buf == '}' || *buf == ']')
	    break;
	 // if whitespace follows then increment buffer position and break
	 else if (isblank(*buf) || (*buf) == '\r')
	 {
	    buf++;
	    break;
	 }
	 // if a newline follows then  increment buffer position and line number and break
	 else if ((*buf) == '\n')
	 {
	    buf++;
	    line_number++;
	    break;
	 }
	 else if (!isdigit(*buf))
	 {
	    xsink->raiseException("JSON_PARSE_ERROR", "unexpected character in number");
	    return NULL;
	 }
	 str.concat(*buf);
	 buf++;
      }
      if (has_dot)
	 return new QoreNode(atof(str.getBuffer()));
      return new QoreNode(strtoll(str.getBuffer(), NULL, 10));
   }
   
   if ((*buf) == 't')
   {
      if (!cmp_rest_token(buf, "rue"))
	 return boolean_true();
      goto error;
   }
   if ((*buf) == 'f')
   {
      if (!cmp_rest_token(buf, "alse"))
	 return boolean_false();
      goto error;
   }
   if ((*buf) == 'n')
   {
      if (!cmp_rest_token(buf, "ull"))
	 return nothing();
      goto error;
   }
   
  error:
   //printd(5, "buf=%s\n", buf);

   xsink->raiseException("JSON-PARSE-ERROR", "invalid input at line %d; unable to parse JSON value", line_number);
   return NULL;
}

#define JSF_THRESHOLD 20

static int doJSONValue(class QoreString *str, class QoreNode *v, int format, class ExceptionSink *xsink)
{
   if (is_nothing(v))
   {
      str->concat("null");
      return 0;
   }
   if (v->type == NT_LIST)
   {
      str->concat("[ ");
      ListIterator li(v->val.list);
      QoreString tmp(str->getEncoding());
      while (li.next())
      {
	 bool ind = tmp.strlen() > JSF_THRESHOLD;
	 tmp.terminate(0);
	 if (doJSONValue(&tmp, li.getValue(), format == -1 ? format : format + 2, xsink))
	    return -1;

	 if (format != -1 && (ind || tmp.strlen() > JSF_THRESHOLD))
	 {
	    str->concat('\n');
	    str->addch(' ', format + 2);
	 }
	 str->sprintf("%s", tmp.getBuffer());

	 if (!li.last())
	    str->concat(", ");
      }
      str->concat(" ]");
      return 0;
   }
   if (v->type == NT_HASH)
   {
      str->concat("{ ");
      HashIterator hi(v->val.hash);
      QoreString tmp(str->getEncoding());
      while (hi.next())
      {
	 bool ind = tmp.strlen() > JSF_THRESHOLD;
	 tmp.terminate(0);
	 if (doJSONValue(&tmp, hi.getValue(), format == -1 ? format : format + 2, xsink))
	    return -1;

	 if (format != -1 && (ind || tmp.strlen() > JSF_THRESHOLD))
	 {
	    str->concat('\n');
	    str->addch(' ', format + 2);
	 }
	 str->sprintf("\"%s\" : %s", hi.getKey(), tmp.getBuffer());
	 if (!hi.last())
	    str->concat(", ");
      }
      str->concat(" }");
      return 0;
   }
   if (v->type == NT_STRING)
   {
      // convert encoding if necessary
      QoreString *t;
      if (v->val.String->getEncoding() != str->getEncoding())
      {
	 t = v->val.String->convertEncoding(str->getEncoding(), xsink);
	 if (!t)
	 {
	    delete str;
	    return -1;
	 }
      }
      else
	 t = v->val.String;

      str->concat('"');
      str->concatEscape(t, '"');
      str->concat('"');
      if (t != v->val.String)
	 delete t;
      return 0;
   }
   if (v->type == NT_INT)
   {
      str->sprintf("%lld", v->val.intval);
      return 0;
   }
   if (v->type == NT_FLOAT)
   {
      str->sprintf("%.9g", v->val.floatval);
      return 0;
   }
   if (v->type == NT_BOOLEAN)
   {
      if (v->val.boolval)
	 str->concat("true");
      else
	 str->concat("false");
      return 0;
   }
   if (v->type == NT_DATE)  // this will be serialized as a string
   {
      str->concat('"');
      v->val.date_time->getString(str);
      str->concat('"');
      return 0;
   }
   
   delete str;
   xsink->raiseException("JSON-SERIALIZATION-ERROR", "don't know how to serialize type '%s'", v->type->getName());
   return -1;
}

static class QoreNode *f_makeJSONString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *val, *pcs;

   tracein("f_makeJSONString()");
   val = get_param(params, 0);

   class QoreEncoding *ccs;
   if ((pcs = test_param(params, NT_STRING, 1)))
      ccs = QEM.findCreate(pcs->val.String);
   else
      ccs = QCS_DEFAULT;

   class QoreString *str = new QoreString(ccs);
   return doJSONValue(str, val, -1, xsink) ? NULL : new QoreNode(str);
}

static class QoreNode *f_makeFormattedJSONString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *val, *pcs;

   tracein("f_makeFormattedJSONString()");
   val = get_param(params, 0);

   class QoreEncoding *ccs;
   if ((pcs = test_param(params, NT_STRING, 1)))
      ccs = QEM.findCreate(pcs->val.String);
   else
      ccs = QCS_DEFAULT;

   class QoreString *str = new QoreString(ccs);
   return doJSONValue(str, val, 0, xsink) ? NULL : new QoreNode(str);
}

static class QoreNode *parseJSONValue(class QoreString *str, class ExceptionSink *xsink)
{
   int line_number = 1;
   char *buf = str->getBuffer();
   class QoreNode *rv = getJSONValue(buf, line_number, str->getEncoding(), xsink);
   if (rv && *buf)
   {
      // check for excess text after JSON data
      skip_whitespace(buf, line_number);
      if (*buf)
      {
	 xsink->raiseException("JSON-PARSE-ERROR", "extra text after JSON data on line %d", line_number);
	 rv->deref(xsink);
	 rv = NULL;
      }
   }
   return rv;
}

static class QoreNode *f_parseJSON(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;

   if (!(p0 = test_param(params, NT_STRING, 0)))
       return NULL;
 
      class QoreEncoding *ccsid;
   if ((p1 = test_param(params, NT_STRING, 1)))
      ccsid = QEM.findCreate(p1->val.String);
   else
      ccsid = QCS_DEFAULT;

   return parseJSONValue(p0->val.String, xsink);
}

void init_json_functions()
{
   builtinFunctions.add("makeJSONString",          f_makeJSONString);
   builtinFunctions.add("makeFormattedJSONString", f_makeFormattedJSONString);
   builtinFunctions.add("parseJSON",               f_parseJSON);
}
