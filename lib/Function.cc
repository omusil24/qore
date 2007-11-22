/*
  Function.cc

  Qore Programming language

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
#include <qore/Function.h>
#include <qore/Operator.h>
#include <qore/StatementBlock.h>

#include <stdio.h>
#include <ctype.h>
#include <assert.h>

static inline void param_error()
{
   parse_error("parameter list contains non-variable reference expressions.");
}

SelfFunctionCall::SelfFunctionCall(char *n) 
{ 
   ns = NULL;
   name = n; 
   func = NULL; 
}

SelfFunctionCall::SelfFunctionCall(class NamedScope *n) 
{ 
   ns = n;
   name = NULL; 
   func = NULL; 
}

SelfFunctionCall::SelfFunctionCall(class Method *f) 
{ 
   ns = NULL;
   name = NULL;
   func = f; 
}

SelfFunctionCall::~SelfFunctionCall() 
{ 
   if (name) 
      free(name); 
   if (ns)
      delete ns;
}

char *SelfFunctionCall::takeName()
{
   char *n = name;
   name = 0;
   return n;
}

class NamedScope *SelfFunctionCall::takeNScope()
{
   NamedScope *rns = ns;
   ns = 0;
   return rns;
}

class QoreNode *SelfFunctionCall::eval(class QoreNode *args, class ExceptionSink *xsink)
{
   class Object *self = getStackObject();
   
   if (func)
      return func->eval(self, args, xsink);
   // otherwise exec copy method
   return self->getClass()->execCopy(self, xsink);
}

// called at parse time
void SelfFunctionCall::resolve()
{
#ifdef DEBUG
   if (ns)
      printd(5, "SelfFunctionCall:resolve() resolving base class call '%s'\n", ns->ostr);
   else 
      printd(5, "SelfFunctionCall:resolve() resolving '%s'\n", name ? name : "(null)");
   assert(!func);
#endif
   if (name)
   {
      // FIXME: warn if argument list passed (will be ignored)

      // copy method calls will be recognized by func = NULL
      if (!strcmp(name, "copy"))
      {
	 free(name);
	 name = NULL;
	 printd(5, "SelfFunctionCall:resolve() resolved to copy constructor\n");
	 return;
      }
      func = getParseClass()->resolveSelfMethod(name);
   }
   else
      func = getParseClass()->resolveSelfMethod(ns);
   if (func)
   {
      printd(5, "SelfFunctionCall:resolve() resolved '%s' to %08p\n", func->getName(), func);
      if (name)
      {
	 free(name);
	 name = NULL;
      }
      else if (ns)
      {
	 delete ns;
	 ns = NULL;
      }
   }
}

class QoreNode *ImportedFunctionCall::eval(class QoreNode *args, class ExceptionSink *xsink)
{
   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);
   
   class QoreNode *rv = pgm->callFunction(func, args, xsink);

   if (xsink->isException())
      xsink->addStackInfo(CT_USER, NULL, func->getName(), o_fn, o_ln, o_eln);
   
   return rv;
}

FunctionCall::FunctionCall(class UserFunction *u, class QoreNode *a)
{
   type = FC_USER;
   f.ufunc = u;
   args = a;
}

FunctionCall::FunctionCall(class BuiltinFunction *b, class QoreNode *a)
{
   type = FC_BUILTIN;
   f.bfunc = b;
   args = a;
}

FunctionCall::FunctionCall(class QoreNode *a, char *name)
{
   printd(5, "FunctionCall::FunctionCall(a=%08p, name=%s) FC_SELF this=%08p\n", a, name, this);
   type = FC_SELF;
   f.sfunc = new SelfFunctionCall(name);
   args = a;
}

FunctionCall::FunctionCall(class QoreNode *a, class NamedScope *n)
{
   printd(5, "FunctionCall::FunctionCall(a=%08p, n=%s) FC_SELF this=%08p\n", a, n->ostr, this);
   type = FC_SELF;
   f.sfunc = new SelfFunctionCall(n);
   args = a;
}

FunctionCall::FunctionCall(class Method *m, class QoreNode *a)
{
   printd(5, "FunctionCall::FunctionCall(a=%08p, method=%08p %s) FC_SELF this=%08p\n", a, m, m->getName(), this);
   type = FC_SELF;
   f.sfunc = new SelfFunctionCall(m);
   args = a;
}

FunctionCall::FunctionCall(char *name, class QoreNode *a)
{
   type = FC_UNRESOLVED;
   f.c_str = name;
   args = a;
}

FunctionCall::FunctionCall(class QoreProgram *p, class UserFunction *u, class QoreNode *a)
{
   type = FC_IMPORTED;
   f.ifunc = new ImportedFunctionCall(p, u);
   args = a;
}

FunctionCall::FunctionCall(char *n_c_str)
{
   type = FC_METHOD;
   f.c_str = n_c_str;
   args = NULL;
}

FunctionCall::~FunctionCall()
{
   printd(5, "FunctionCall::~FunctionCall(): type=%d args=%08p (%s)\n",
	  type, args, (type == FC_UNRESOLVED && f.c_str) ? f.c_str : "(null)");

   // there could be an object here in the case of a background expression
   if (args)
   {
      ExceptionSink xsink;
      args->deref(&xsink);
   }

   switch (type)
   {
      case FC_USER:
      case FC_BUILTIN:
	 break;
      case FC_SELF:
	 delete f.sfunc;
	 break;
      case FC_METHOD:
      case FC_UNRESOLVED:
	 if (f.c_str)
	    free(f.c_str);
	 break;
      case FC_IMPORTED:
	 delete f.ifunc;
	 break;
   }
}

char *FunctionCall::takeName()
{
   char *str = f.c_str;
   f.c_str = 0;
   return str;
}

void FunctionCall::parseMakeMethod()
{
   type = FC_METHOD;
}

// makes a "new" operator call from a function call
class QoreNode *FunctionCall::parseMakeNewObject()
{
   class QoreNode *rv = new QoreNode(new NamedScope(f.c_str), args);
   f.c_str = NULL;
   args = NULL;
   return rv;
}

class QoreNode *FunctionCall::eval(class ExceptionSink *xsink)
{
   switch (type)
   {
      case FC_USER:
	 return f.ufunc->eval(args, NULL, xsink);
      case FC_BUILTIN:
	 return f.bfunc->eval(args, xsink);
      case FC_SELF:
	 return f.sfunc->eval(args, xsink);
      case FC_IMPORTED:
	 return f.ifunc->eval(args, xsink);
   }

   assert(false);
   return NULL;
}

int FunctionCall::existsUserParam(int i) const
{
   if (type == FC_USER)
      return f.ufunc->params->num_params > i;
   if (type == FC_IMPORTED)
      return f.ifunc->func->params->num_params > i;
   return 1;
}

int FunctionCall::getType() const
{
   return type;
}

const char *FunctionCall::getName() const
{
   switch (type)
   {
      case FC_USER:
	 return f.ufunc->getName();
      case FC_BUILTIN:
	 return f.bfunc->getName();
      case FC_SELF:
	 return f.sfunc->name;
      case FC_IMPORTED:
	 return f.ifunc->func->getName();
      case FC_UNRESOLVED:
      case FC_METHOD:
	 return f.c_str ? f.c_str : (char *)"copy";
   }
   return NULL;   
}

Paramlist::Paramlist(class QoreNode *params)
{
   ids = NULL;
   if (!params)
   {
      num_params = 0;
      names = NULL;
   }
   else if (params->type == NT_VARREF)
   {
      num_params = 1;
      names = new char *[1];
      names[0] = strdup(params->val.vref->name);
      params->deref(NULL);
   }
   else if (params->type != NT_LIST)
   {
      param_error();
      num_params = 0;
      names = NULL;
   }
   else
   {
      num_params = params->val.list->size();
      names = new char *[params->val.list->size()];
      for (int i = 0; i < params->val.list->size(); i++)
      {
         if (params->val.list->retrieve_entry(i)->type != NT_VARREF)
         {
            param_error();
            num_params = 0;
            delete [] names;
            names = NULL;
            break;
         }
         else
            names[i] = strdup(params->val.list->retrieve_entry(i)->val.vref->name);
      }
      params->deref(NULL);
   }
}

Paramlist::~Paramlist()
{
   for (int i = 0; i < num_params; i++)
      free(names[i]);
   if (names)
      delete [] names;
   if (ids)
      delete [] ids;
}

UserFunction::UserFunction(char *nme, class Paramlist *parms, class StatementBlock *b, bool synced)
{
   synchronized = synced;
   if (synced)
      gate = new VRMutex();
# ifdef DEBUG
   else
      gate = NULL;
# endif
   name = nme;
   params = parms;
   statements = b;
}

UserFunction::~UserFunction()
{
   printd(5, "UserFunction::~UserFunction() deleting %s\n", name);
   if (synchronized)
      delete gate;
   delete params;
   if (statements)
      delete statements;
   free(name);
}

void UserFunction::deref()
{
   if (ROdereference())
      delete this;
}

BuiltinFunction::BuiltinFunction(const char *nme, q_func_t f, int typ)
{
   type = typ;
   name = nme;
   code.func = f;
   next = NULL;
}

BuiltinFunction::BuiltinFunction(const char *nme, q_method_t m, int typ)
{
   type = typ;
   name = nme;
   code.method = m;
   next = NULL;
}

BuiltinFunction::BuiltinFunction(q_constructor_t m, int typ)
{
   type = typ;
   name = "constructor";
   code.constructor = m;
   next = NULL;
}

BuiltinFunction::BuiltinFunction(q_destructor_t m, int typ)
{
   type = typ;
   name = "destructor";
   code.destructor = m;
   next = NULL;
}

BuiltinFunction::BuiltinFunction(q_copy_t m, int typ)
{
   type = typ;
   name = "copy";
   code.copy = m;
   next = NULL;
}

void BuiltinFunction::evalConstructor(class Object *self, class QoreNode *args, class BCList *bcl, class BCEAList *bceal, const char *class_name, class ExceptionSink *xsink)
{
   tracein("BuiltinFunction::evalConstructor()");

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   CodeContextHelper cch("constructor", self, xsink);
   // push call on call stack
   pushCall("constructor", CT_BUILTIN, self);

   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);
   
   if (!xsink->isEvent())
   {
      code.constructor(self, args, xsink);
      if (xsink->isException())
	 xsink->addStackInfo(CT_BUILTIN, class_name, "constructor", o_fn, o_ln, o_eln);
   }
   
   popCall(xsink);

   traceout("BuiltinFunction::evalWithArgs()");
}

void BuiltinFunction::evalSystemConstructor(class Object *self, class QoreNode *args, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink)
{
   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);
   
   code.constructor(self, args, xsink);
}

QoreNode *BuiltinFunction::evalWithArgs(class Object *self, class QoreNode *args, class ExceptionSink *xsink)
{
   tracein("BuiltinFunction::evalWithArgs()");
   printd(2, "BuiltinFunction::evalWithArgs() calling builtin function \"%s\"\n", name);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   QoreNode *rv;
   {
      CodeContextHelper cch(name, self, xsink);
      // push call on call stack
      pushCall(name, CT_BUILTIN, self);

      rv = code.func(args, xsink);

      popCall(xsink);
   }
   
   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, self ? self->getClass()->getName() : NULL, name, o_fn, o_ln, o_eln);

   traceout("BuiltinFunction::evalWithArgs()");
   return rv;
}

QoreNode *BuiltinFunction::evalMethod(class Object *self, void *private_data, class QoreNode *args, class ExceptionSink *xsink)
{
   tracein("BuiltinFunction::evalMethod()");
   printd(2, "BuiltinFunction::evalMethod() calling builtin function '%s' obj=%08p data=%08p\n", name, self, private_data);
   
   CodeContextHelper cch(name, self, xsink);
   // push call on call stack in debugging mode
   pushCall(name, CT_BUILTIN, self);

   // note: exception stack trace is added at the level above
   QoreNode *rv = code.method(self, private_data, args, xsink);

   popCall(xsink);

   traceout("BuiltinFunction::evalWithArgs()");
   return rv;
}

void BuiltinFunction::evalDestructor(class Object *self, void *private_data, const char *class_name, class ExceptionSink *xsink)
{
   tracein("BuiltinFunction::evalDestructor()");
   
   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   {
      CodeContextHelper cch("destructor", self, xsink);
      // push call on call stack
      pushCall("destructor", CT_BUILTIN, self);

      code.destructor(self, private_data, xsink);

      popCall(xsink);
   }
   
   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, class_name, "destructor", o_fn, o_ln, o_eln);
   
   traceout("BuiltinFunction::destructor()");
}

void BuiltinFunction::evalCopy(class Object *self, class Object *old, void *private_data, const char *class_name, class ExceptionSink *xsink)
{
   tracein("BuiltinFunction::evalCopy()");
   
   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);
   
   {
      CodeContextHelper cch("copy", self, xsink);
      // push call on call stack
      pushCall("copy", CT_BUILTIN, self);

      code.copy(self, old, private_data, xsink);

      popCall(xsink);
   }
   
   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, class_name, "copy", o_fn, o_ln, o_eln);
   
   traceout("BuiltinFunction::evalCopy()");
}

void BuiltinFunction::evalSystemDestructor(class Object *self, void *private_data, class ExceptionSink *xsink)
{
   code.destructor(self, private_data, xsink);
}

QoreNode *BuiltinFunction::eval(QoreNode *args, ExceptionSink *xsink)
{
   class QoreNode *tmp, *rv;
   ExceptionSink newsink;

   tracein("BuiltinFunction::eval(Node)");
   printd(3, "BuiltinFunction::eval(Node) calling builtin function \"%s\"\n", name);
   
   //printd(5, "BuiltinFunction::eval(Node) args=%08p %s\n", args, args ? args->type->getName() : "(null)");

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);
   
   if (args)
      tmp = args->eval(&newsink);
   else
      tmp = NULL;

   //printd(5, "BuiltinFunction::eval(Node) after eval tmp args=%08p %s\n", tmp, tmp ? tmp->type->getName() : "(null)");

   {
      CodeContextHelper cch(name, NULL, xsink);
      // push call on call stack
      pushCall(name, CT_BUILTIN);

      // execute the function if no new exception has happened
      // necessary only in the case of a builtin object destructor
      if (!newsink.isEvent())
	 rv = code.func(tmp, xsink);
      else
	 rv = NULL;

      xsink->assimilate(&newsink);

      // pop call off call stack
      popCall(xsink);
   }

   discard(tmp, xsink);

   if (xsink->isException())
      xsink->addStackInfo(CT_BUILTIN, NULL, name, o_fn, o_ln, o_eln);
   
   traceout("BuiltinFunction::eval(Node)");
   return rv;
}

// calls a user function
class QoreNode *UserFunction::eval(QoreNode *args, Object *self, class ExceptionSink *xsink, const char *class_name)
{
   tracein("UserFunction::eval()");
   printd(2, "UserFunction::eval(): function='%s' args=%08p (size=%d)\n", name, args, args ? args->val.list->size() : 0);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);
      
   int i = 0;
   class QoreNode *val = NULL;
   int num_args, num_params;

   if (args)
      num_args = args->val.list->size();
   else
      num_args = 0;

   // instantiate local vars from param list
   num_params = params->num_params;
   for (i = 0; i < num_params; i++)
   {
      QoreNode *n = args ? args->val.list->retrieve_entry(i) : NULL;
      printd(4, "UserFunction::eval() %d: instantiating param lvar %d (%08p %s)\n", i, params->ids[i], n, n ? n->type->getName() : "(null)");
      if (n)
      {
         if (n->type == NT_REFERENCE)
         {
	    bool is_self_ref = false;
            n = doPartialEval(n->val.lvexp, &is_self_ref, xsink);
	    //printd(5, "UserFunction::eval() ref self_ref=%d, self=%08p (%s) so=%08p (%s)\n", is_self_ref, self, self ? self->getClass()->name : "NULL", getStackObject(), getStackObject() ? getStackObject()->getClass()->name : "NULL");
            if (!xsink->isEvent())
	       instantiateLVar(params->ids[i], n, is_self_ref ? getStackObject() : NULL);
         }
         else
         {
            n = n->eval(xsink);
	    if (!xsink->isEvent())
	       instantiateLVar(params->ids[i], n);
         }
	 // the above if block will only instantiate the local variable if no
	 // exceptions have occurred. therefore here we do the cleanup the rest
	 // of any already instantiated local variables if an exception does occur
         if (xsink->isEvent())
         {
            if (n)
               n->deref(xsink);
            for (int j = i; j; j--)
               uninstantiateLVar(xsink);
            return NULL;
         }
      }
      else
         instantiateLVar(params->ids[i], NULL);
   }

   // if there are more arguments than parameters
   printd(5, "UserFunction::eval() params=%d arg=%d\n", num_params, num_args);
   class QoreNode *argv;
   
   if (num_params < num_args)
   {
      QoreList *l = new QoreList();
      
      for (i = 0; i < (num_args - num_params); i++)
         if (args->val.list->retrieve_entry(i + num_params))
         {
            QoreNode *v = args->val.list->eval_entry(i + num_params, xsink);
            if (xsink->isEvent())
            {
	       if (v)
		  v->deref(xsink);
               l->derefAndDelete(xsink);
               // uninstantiate local vars from param list
               for (int j = 0; j < num_params; j++)
                  uninstantiateLVar(xsink);
               return NULL;
            }
            l->push(v);
         }
         else
            l->push(NULL);
      argv = new QoreNode(l);
   }
   else
      argv = NULL;

   if (statements)
   {
      CodeContextHelper cch(name, self, xsink);

      pushCall(name, CT_USER, self);

      // push call on stack
      if (self)
         self->instantiateLVar(params->selfid);
   
      // instantiate argv and push id on stack
      instantiateLVar(params->argvid, argv);

      {
	 ArgvContextHelper(params->argvid);

	 // enter gate if necessary
	 if (!synchronized || (gate->enter(xsink) >= 0))
	 {
	    // execute function
	    val = statements->exec(xsink);
	    
	    // exit gate if necessary
	    if (synchronized)
	       gate->exit();
	 }	 
      }

      // uninstantiate argv
      uninstantiateLVar(xsink);

      // if self then uninstantiate
      if (self)
	 self->uninstantiateLVar(xsink);

      popCall(xsink);   
   }
   else
      discard(argv, xsink);

   if (num_params)
   {
      printd(5, "UserFunction::eval() about to uninstantiate %d vars\n", params->num_params);

      // uninstantiate local vars from param list
      for (i = 0; i < num_params; i++)
	 uninstantiateLVar(xsink);
   }
   if (xsink->isException())
      xsink->addStackInfo(CT_USER, self ? (class_name ? class_name : self->getClass()->getName()) : NULL, name, o_fn, o_ln, o_eln);
   
   traceout("UserFunction::eval()");
   return val;
}

// this function will set up user copy constructor calls
void UserFunction::evalCopy(Object *oold, Object *self, const char *class_name, ExceptionSink *xsink)
{
   tracein("UserFunction::evalCopy()");
   printd(2, "UserFunction::evalCopy(): function='%s', num_params=%d, oldobj=%08p\n", name, params->num_params, oold);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);

   // create QoreNode for "old" for either param or argv list
   oold->ref();
   QoreNode *old = new QoreNode(oold);

   // instantiate local vars from param list
   for (int i = 0; i < params->num_params; i++)
   {
      class QoreNode *n = (i ? NULL : old);
      printd(5, "UserFunction::evalCopy(): instantiating param lvar %d (%08p)\n", i, params->ids[i], n);
      instantiateLVar(params->ids[i], n);
   }

   class QoreNode *argv;

   if (!params->num_params)
   {
      QoreList *l = new QoreList();
      l->push(old);
      argv = new QoreNode(l);
   }
   else
      argv = NULL;

   if (statements)
   {
      CodeContextHelper cch(name, self, xsink);
      // push call on stack
      pushCall(name, CT_USER, self);

      // instantiate self
      self->instantiateLVar(params->selfid);
   
      // instantiate argv and push id on stack (for shift)
      instantiateLVar(params->argvid, argv);
      {
	 ArgvContextHelper(params->argvid);
	 // execute function
	 discard(statements->exec(xsink), xsink);
      }
      uninstantiateLVar(xsink);
      
      // uninstantiate self
      self->uninstantiateLVar(xsink);
      
      popCall(xsink);
   }
   else
      discard(argv, xsink);

   if (params->num_params)
   {
      printd(5, "UserFunction::evalCopy() about to uninstantiate %d vars\n", params->num_params);

      // uninstantiate local vars from param list
      for (int i = 0; i < params->num_params; i++)
	 uninstantiateLVar(xsink);
   }
   if (xsink->isException())
      xsink->addStackInfo(CT_USER, class_name, name, o_fn, o_ln, o_eln);
   
   traceout("UserFunction::evalCopy()");
}

// calls a user constructor method
class QoreNode *UserFunction::evalConstructor(QoreNode *args, Object *self, class BCList *bcl, class BCEAList *bceal, const char *class_name, class ExceptionSink *xsink)
{
   tracein("UserFunction::evalConstructor()");
   printd(2, "UserFunction::evalConstructor(): method='%s:%s' args=%08p (size=%d)\n", 
          class_name, name, args, args ? args->val.list->size() : 0);

   // save current program location in case there's an exception
   const char *o_fn = get_pgm_file();
   int o_ln, o_eln;
   get_pgm_counter(o_ln, o_eln);
   
   int i = 0;
   class QoreNode *val = NULL;
   int num_args, num_params;

   if (args)
      num_args = args->val.list->size();
   else
      num_args = 0;

   // instantiate local vars from param list
   num_params = params->num_params;
   for (i = 0; i < num_params; i++)
   {
      QoreNode *n = args ? args->val.list->retrieve_entry(i) : NULL;
      printd(4, "UserFunction::evalConstructor() %d: instantiating param lvar %d (%08p)\n", i, params->ids[i], n);
      if (n)
      {
         if (n->type == NT_REFERENCE)
         {
	    bool is_self_ref = false;
            n = doPartialEval(n->val.lvexp, &is_self_ref, xsink);
            if (!xsink->isEvent())
	       instantiateLVar(params->ids[i], n, is_self_ref ? getStackObject() : NULL);
         }
         else
         {
            n = n->eval(xsink);
	    if (!xsink->isEvent())
	       instantiateLVar(params->ids[i], n);
         }
	 // the above if block will only instantiate the local variable if no
	 // exceptions have occurred. therefore here we do the cleanup the rest
	 // of any already instantiated local variables if an exception does occur
         if (xsink->isEvent())
         {
            if (n)
               n->deref(xsink);
            for (int j = i; j; j--)
               uninstantiateLVar(xsink);
	    traceout("UserFunction::evalConstructor()");
            return NULL;
         }
      }
      else
         instantiateLVar(params->ids[i], NULL);
   }

   // if there are more arguments than parameters
   printd(5, "UserFunction::evalConstructor() params=%d arg=%d\n", num_params, num_args);
   class QoreNode *argv;
   
   if (num_params < num_args)
   {
      QoreList *l = new QoreList();
      
      for (i = 0; i < (num_args - num_params); i++)
         if (args->val.list->retrieve_entry(i + num_params))
         {
            QoreNode *v = args->val.list->eval_entry(i + num_params, xsink);
            if (xsink->isEvent())
            {
	       if (v)
		  v->deref(xsink);
               l->derefAndDelete(xsink);
               // uninstantiate local vars from param list
               for (int j = 0; j < num_params; j++)
                  uninstantiateLVar(xsink);
               return NULL;
            }
            l->push(v);
         }
         else
            l->push(NULL);
      argv = new QoreNode(l);
   }
   else
      argv = NULL;

   // evaluate base class constructors (if any)
   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);

   if (!xsink->isEvent())
   {
      // switch to new program for imported objects
      ProgramContextHelper pch(self->getProgram());
 
      // execute constructor
      if (statements)
      {
	 CodeContextHelper cch(name, self, xsink);
	 // push call on stack
	 pushCall(name, CT_USER, self);

	 // instantiate "$self" variable
	 self->instantiateLVar(params->selfid);
	 
	 // instantiate argv and push id on stack
	 instantiateLVar(params->argvid, argv);

	 {
	    ArgvContextHelper(params->argvid);
	    
	    // enter gate if necessary
	    if (!synchronized || (gate->enter(xsink) >= 0))
	    {
	       // execute function
	       val = statements->exec(xsink);
	       
	       // exit gate if necessary
	       if (synchronized)
		  gate->exit();
	    }
	 }
	 uninstantiateLVar(xsink);
	    
	 // uninstantiate "$self" variable
	 self->uninstantiateLVar(xsink);
	 
	 popCall(xsink);   
      }
      else
	 discard(argv, xsink);
   }

   if (num_params)
   {
      printd(5, "UserFunction::evalConstructor() about to uninstantiate %d vars\n", params->num_params);

      // uninstantiate local vars from param list
      for (i = 0; i < num_params; i++)
	 uninstantiateLVar(xsink);
   }
   if (xsink->isException())
      xsink->addStackInfo(CT_USER, class_name, name, o_fn, o_ln, o_eln);
   
   traceout("UserFunction::evalConstructor()");
   return val;
}

// this will only be called with lvalue expressions
class QoreNode *doPartialEval(class QoreNode *n, bool *is_self_ref, class ExceptionSink *xsink)
{
   QoreNode *rv = NULL;
   if (n->type == NT_TREE)
   {
      class QoreNode *nn = n->val.tree->right->eval(xsink);
      if (xsink->isEvent())
      {
	 discard(nn, xsink);
	 return NULL;
      }
      class Tree *t = new Tree(doPartialEval(n->val.tree->left, is_self_ref, xsink), n->val.tree->op, nn ? nn : nothing());
      if (!t->left)
	 delete t;
      else
	 rv = new QoreNode(t);
   }
   else
   {
      rv = n->RefSelf();
      if (n->type == NT_SELF_VARREF)
	 (*is_self_ref) = true;
   }
   return rv;
}
