/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreNamespaceIntern.h

  Qore Programming Language

  Copyright (C) 2003 - 2012 David Nichols

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

#ifndef _QORE_QORENAMESPACEINTERN_H
#define _QORE_QORENAMESPACEINTERN_H

#include <qore/intern/QoreClassList.h>
#include <qore/intern/QoreNamespaceList.h>
#include <qore/intern/ConstantList.h>
#include <qore/intern/FunctionList.h>

#include <map>
#include <vector>

class qore_ns_private {
private:
   // not implemented
   DLLLOCAL qore_ns_private(const qore_ns_private&);
   // not implemented
   DLLLOCAL qore_ns_private& operator=(const qore_ns_private&);

public:
   std::string name;

   QoreClassList classList, pendClassList;
   ConstantList constant, pendConstant;
   QoreNamespaceList nsl, pendNSL;
   FunctionList func_list;

   // 0 = root namespace, ...
   unsigned depth;

   bool root;

   const qore_ns_private* parent;       // pointer to parent namespace (0 if this is the root namespace or an unattached namespace)
   q_ns_class_handler_t class_handler;   
   QoreNamespace *ns;

   DLLLOCAL qore_ns_private(QoreNamespace *n_ns, const char *n) : name(n), depth(0), root(false), parent(0), class_handler(0), ns(n_ns) {
   }

   DLLLOCAL qore_ns_private(QoreNamespace *n_ns) : depth(0), root(false), parent(0), class_handler(0), ns(n_ns) {      
   }

   // called when parsing
   DLLLOCAL qore_ns_private() : depth(0), root(false), parent(0), class_handler(0), ns(0) {
      // attaches to the ns attribute in the constructor
      new QoreNamespace(this);
      name = parse_pop_namespace_name();
   }

   DLLLOCAL qore_ns_private(const qore_ns_private &old, int64 po) 
      : name(old.name), 
        classList(old.classList, po, this), 
        constant(old.constant),        
        nsl(old.nsl, po, *this),
        func_list(old.func_list, po),
        depth(old.depth),
        root(old.root),
        parent(0), class_handler(old.class_handler), ns(0) {
   }

   DLLLOCAL ~qore_ns_private() {
      printd(5, "qore_ns_private::~qore_ns_private() this=%p '%s'\n", this, name.c_str());
   }

   DLLLOCAL void getPath(std::string& str) const {
      const qore_ns_private* w = parent;
      while (w && w->parent) {
         str.insert(0, "::");
         str.insert(0, w->name);
         w = w->parent;
      }

      // append this namespace's name
      str += name;
   }

   DLLLOCAL static QoreNamespace* newNamespace(const qore_ns_private& old, int64 po) {
      qore_ns_private* p = new qore_ns_private(old, po);
      return new QoreNamespace(p);
   }

   // destroys the object and frees all associated memory
   DLLLOCAL void purge() {
      constant.reset();
      pendConstant.reset();

      classList.reset();
      pendClassList.reset();

      nsl.reset();
      pendNSL.reset();
   }

   DLLLOCAL qore_root_ns_private* getRoot() {
      qore_ns_private* w = this;
      while (w->parent)
         w = (qore_ns_private*)w->parent;

      return w->root ? reinterpret_cast<qore_root_ns_private*>(w) : 0;
   }

   // finds a local class in the committed class list, if not found executes the class handler
   DLLLOCAL QoreClass *findLoadClass(const char *cname) {
      QoreClass *qc = classList.find(cname);
      if (!qc && class_handler)
	 qc = class_handler(ns, cname);
      return qc;
   }

   DLLLOCAL void assimilate(QoreNamespace* ns);

   DLLLOCAL void updateDepthRecursive(unsigned ndepth);

   DLLLOCAL int parseAddPendingClass(const NamedScope* n, QoreClass* oc);
   DLLLOCAL int parseAddPendingClass(QoreClass* oc);

   DLLLOCAL cnemap_t::iterator parseAddConstant(const char* name, AbstractQoreNode* value);

   DLLLOCAL void parseAddConstant(const NamedScope& name, AbstractQoreNode* value);

   DLLLOCAL int checkImportFunction(const char* name, ExceptionSink *xsink) {
      //printd(5, "qore_ns_private::checkImportFunction(%s) this: %p\n", name, this);

      if (func_list.findNode(name)) {
         xsink->raiseException("FUNCTION-IMPORT-ERROR", "function '%s' already exists in this namespace", name);
         return -1;
      }

      return 0;
   }

   DLLLOCAL FunctionEntry* importFunction(ExceptionSink* xsink, QoreFunction* u, const char* new_name = 0) {
      const char* fn = new_name ? new_name : u->getName();
      if (checkImportFunction(fn, xsink))
         return 0;

      return func_list.import(fn, u);
   }

   DLLLOCAL const QoreFunction* runtimeFindFunction(const char* name) {
      return func_list.find(name, true);
   }

   DLLLOCAL void runtimeFindCallFunction(const char* name, const QoreFunction*& ufc) {
      assert(!ufc);

      ufc = runtimeFindFunction(name);
      if (!ufc)
         ufc = builtinFunctions.find(name);
   }

   DLLLOCAL QoreNamespace* findCreateNamespacePath(const char* nspath);

   DLLLOCAL AbstractQoreNode *getConstantValue(const char *name, const QoreTypeInfo *&typeInfo);
   DLLLOCAL QoreClass *parseFindLocalClass(const char *name);
   DLLLOCAL qore_ns_private* parseAddNamespace(QoreNamespace *nns);

   DLLLOCAL void addNamespace(qore_ns_private* nns);

   DLLLOCAL void parseInit();
   DLLLOCAL void parseInitConstants();
   DLLLOCAL void parseRollback();
   DLLLOCAL void parseCommit();

   DLLLOCAL const QoreFunction* parseMatchFunction(const NamedScope& nscope, unsigned& match) const;
   DLLLOCAL QoreNamespace *resolveNameScope(const NamedScope *name) const;
   DLLLOCAL QoreNamespace *parseMatchNamespace(const NamedScope& nscope, unsigned& matched);
   DLLLOCAL QoreClass *parseMatchScopedClass(const NamedScope* name, unsigned& matched);
   DLLLOCAL QoreClass *parseMatchScopedClassWithMethod(const NamedScope* nscope, unsigned& matched);
   DLLLOCAL AbstractQoreNode *parseCheckScopedReference(const NamedScope &ns, unsigned &m, const QoreTypeInfo *&typeInfo) const;

   DLLLOCAL AbstractQoreNode *parseResolveScopedReference(const NamedScope &ns, unsigned &m, const QoreTypeInfo *&typeInfo) const;

   DLLLOCAL AbstractQoreNode *parseFindLocalConstantValue(const char *cname, const QoreTypeInfo *&typeInfo);
   DLLLOCAL QoreNamespace *parseFindLocalNamespace(const char *nname);

   DLLLOCAL AbstractQoreNode *parseMatchScopedConstantValue(const NamedScope* name, unsigned& matched, const QoreTypeInfo*& typeInfo);

   DLLLOCAL FunctionEntry* addPendingVariantIntern(const char* name, AbstractQoreFunctionVariant* v, bool& new_func) {
      SimpleRefHolder<AbstractQoreFunctionVariant> vh(v);

      FunctionEntry* fe = func_list.findNode(name);

      if (!fe) {
         QoreFunction* u = new QoreFunction(name);
         u->addPendingVariant(vh.release());
         fe = func_list.add(u);
         new_func = true;
         return fe;
      }

      return fe->getFunction()->addPendingVariant(vh.release()) ? 0 : fe;
   }

   DLLLOCAL void addBuiltinVariant(const char* name, AbstractQoreFunctionVariant* v);

   static void getParams(unsigned num_params, type_vec_t &typeList, arg_vec_t &defaultArgList, name_vec_t& nameList, va_list args) {
      typeList.reserve(num_params);
      defaultArgList.reserve(num_params);
      nameList.reserve(num_params);
      for (unsigned i = 0; i < num_params; ++i) {
         typeList.push_back(va_arg(args, const QoreTypeInfo *));
         defaultArgList.push_back(va_arg(args, AbstractQoreNode *));
         nameList.push_back(va_arg(args, const char*));
         //printd(0, "qore_process_params() i=%d/%d typeInfo=%p (%s) defArg=%p\n", i, num_params, typeList[i], typeList[i]->getTypeName(), defaultArgList[i]);
      }
   }

   template <typename T, class B>
   DLLLOCAL void addBuiltinVariant(const char *name, T f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, va_list args) {
      //printd(0, "add2('%s', %p, flags=%lld) BEFORE\n", name, f, flags);
      type_vec_t typeList;
      arg_vec_t defaultArgList;
      name_vec_t nameList;
      if (num_params)
         getParams(num_params, typeList, defaultArgList, nameList, args);

      //printd(0, "add2('%s', %p, flags=%lld, domain=%lld, ret=%s, num_params=%d, ...)\n", name, f, flags, functional_domain, returnTypeInfo->getName(), num_params);
      addBuiltinVariant(name, new B(f, flags, functional_domain, returnTypeInfo, typeList, defaultArgList, nameList));
   }
   
   DLLLOCAL static AbstractQoreNode* parseResolveClassConstant(QoreClass* qc, const char* name, const QoreTypeInfo*& typeInfo);

   DLLLOCAL static ConstantList& getConstantList(const QoreNamespace *ns) {
      return ns->priv->constant;
   }

   DLLLOCAL static const QoreFunction* runtimeFindFunction(QoreNamespace& ns, const char *name) {
      return ns.priv->runtimeFindFunction(name);
   }

   DLLLOCAL static void runtimeFindCallFunction(QoreNamespace& ns, const char* name, const QoreFunction*& ufc) {
      return ns.priv->runtimeFindCallFunction(name, ufc);
   }

   DLLLOCAL static QoreListNode* getUserFunctionList(QoreNamespace& ns) {
      return ns.priv->func_list.getList(); 
   }

   DLLLOCAL static void parseAddPendingClass(QoreNamespace& ns, const NamedScope *n, QoreClass *oc) {
      ns.priv->parseAddPendingClass(n, oc);
   }

   DLLLOCAL static void parseAddNamespace(QoreNamespace& ns, QoreNamespace *nns) {
      ns.priv->parseAddNamespace(nns);
   }

   DLLLOCAL static void parseAddConstant(QoreNamespace& ns, const NamedScope& name, AbstractQoreNode* value) {
      ns.priv->parseAddConstant(name, value);
   }

   DLLLOCAL static void parseRollback(QoreNamespace& ns) {
      ns.priv->parseRollback();
   }

   DLLLOCAL static void parseCommit(QoreNamespace& ns) {
      ns.priv->parseCommit();
   }

   DLLLOCAL static void purge(QoreNamespace& ns) {
      ns.priv->purge();
   }

   DLLLOCAL static qore_ns_private* get(QoreNamespace& ns) {
      return ns.priv;
   }

   DLLLOCAL static const qore_ns_private* get(const QoreNamespace& ns) {
      return ns.priv;
   }
};

struct namespace_iterator_element {
   qore_ns_private* ns;
   nsmap_t::iterator i;
   bool committed;        // use committed or pending namespace list

   DLLLOCAL namespace_iterator_element(qore_ns_private* n_ns, bool n_committed) : 
      ns(n_ns), i(n_committed ? ns->nsl.nsmap.begin() : ns->pendNSL.nsmap.begin()), committed(n_committed) {
      assert(ns);
   }

   DLLLOCAL bool atEnd() const {
      return i == (committed ? ns->nsl.nsmap.end() : ns->pendNSL.nsmap.end());
   }

   DLLLOCAL QoreNamespace* next() {
      ++i;
      return atEnd() ? 0 : i->second;
   }
};

class QorePrivateNamespaceIterator {
protected:
   typedef std::vector<namespace_iterator_element> nsv_t;
   nsv_t nsv; // stack of namespaces
   qore_ns_private* root; // for starting over when done
   bool committed;        // use committed or pending namespace list

   DLLLOCAL void set(qore_ns_private* rns) {
      nsv.push_back(namespace_iterator_element(rns, committed));
      while (!(committed ? rns->nsl.empty() : rns->pendNSL.empty())) {
         rns = qore_ns_private::get(*((committed ? rns->nsl.nsmap.begin()->second : rns->pendNSL.nsmap.begin()->second)));
         nsv.push_back(namespace_iterator_element(rns, committed));
      }
   }

public:
   DLLLOCAL QorePrivateNamespaceIterator(qore_ns_private* rns, bool n_committed) : root(rns), committed(n_committed) {
      assert(rns);
   }

   DLLLOCAL bool next() {
      // reset when starting over
      if (nsv.empty()) {
         set(root);
         return true;
      }

      namespace_iterator_element* nie = &(nsv.back());

      // if the last element of the current namespace list has been iterated, take it off the stack
      if (nie->atEnd()) {
         nsv.pop_back();
         if (nsv.empty())
            return false;

         nie = &(nsv.back());
      }

      QoreNamespace* next = nie->next();
      if (next)
         set(qore_ns_private::get(*next));

      return true;
   }

   DLLLOCAL qore_ns_private* operator->() {
      return nsv.back().ns;
   }

   DLLLOCAL qore_ns_private* operator*() {
      return nsv.back().ns;
   }

   DLLLOCAL qore_ns_private* get() {
      return nsv.back().ns;
   }
};

struct NSOInfoBase {
   qore_ns_private* ns;

   DLLLOCAL NSOInfoBase(qore_ns_private* n_ns) : ns(n_ns) {
   }

   DLLLOCAL unsigned depth() const {
      return ns->depth;
   }
};

template <typename T>
struct NSOInfo : public NSOInfoBase {
   // object
   T* obj;

   DLLLOCAL NSOInfo(qore_ns_private* n_ns, T* n_obj) : NSOInfoBase(n_ns), obj(n_obj) {
   }

   DLLLOCAL void assign(qore_ns_private* n_ns, T* n_obj) {
      ns = n_ns;
      obj = n_obj;
   }
};

template <typename T>
class RootMap : public std::map<const char*, NSOInfo<T>, ltstr> {
private:
   // not implemented
   DLLLOCAL RootMap(const RootMap& old);
   // not implemented
   DLLLOCAL RootMap& operator=(const RootMap& m);

public:
   typedef NSOInfo<T> info_t;
   typedef std::map<const char*, NSOInfo<T>, ltstr> map_t;

   DLLLOCAL RootMap() {
   }

   DLLLOCAL void update(const char* name, qore_ns_private* ns, T* obj) {
      // get current lookup map entry for this object
      typename map_t::iterator i = this->find(name);
      if (i == this->end())
         this->insert(typename map_t::value_type(name, info_t(ns, obj)));
      else // if the old depth is > the new depth, then replace
         if (i->second.depth() > ns->depth)
            i->second.assign(ns, obj);
   }

   DLLLOCAL void update(typename map_t::const_iterator ni) {
      // get current lookup map entry for this object
      typename map_t::iterator i = this->find(ni->first);
      if (i == this->end()) {
         //printd(5, "RootMap::update(iterator) inserting '%s' new depth: %d\n", ni->first, ni->second.depth());
         this->insert(typename map_t::value_type(ni->first, ni->second));
      }
      else {
         // if the old depth is > the new depth, then replace
         if (i->second.depth() > ni->second.depth()) {
            //printd(5, "RootMap::update(iterator) replacing '%s' current depth: %d new depth: %d\n", ni->first, i->second.depth(), ni->second.depth());
            i->second = ni->second;      
         }
         //else
         //printd(5, "RootMap::update(iterator) ignoring '%s' current depth: %d new depth: %d\n", ni->first, i->second.depth(), ni->second.depth());
      }
   }

   T* findObj(const char* name) {
      typename map_t::iterator i = this->find(name);
      return i == this->end() ? 0 : i->second.obj;
   }
};

class NamespaceMap {
   friend class NamespaceMapIterator;

protected:
   // map from depth to namespace
   typedef std::multimap<unsigned, qore_ns_private*> nsdmap_t;
   // map from name to depth map
   typedef std::map<const char*, nsdmap_t, ltstr> nsmap_t;
   // map from namespace to depth for reindexing
   typedef std::map<qore_ns_private*, unsigned> nsrmap_t;

   nsmap_t nsmap;   // name to depth to namespace map
   nsrmap_t nsrmap; // namespace to depth map (for fast reindexing)

   // not implemented
   DLLLOCAL NamespaceMap(const NamespaceMap& old);
   // not implemented
   DLLLOCAL NamespaceMap& operator=(const NamespaceMap& m);

public:
   DLLLOCAL NamespaceMap() {
   }

   DLLLOCAL void update(qore_ns_private* ns) {
      // if this namespace is already indexes, then reindex
      nsrmap_t::iterator ri = nsrmap.find(ns);
      if (ri != nsrmap.end()) {
         // if the depth is the same, then do nothing
         if (ns->depth == ri->second)
            return;

         // otherwise get the depth -> namespace map under thisname
         nsmap_t::iterator i = nsmap.find(ns->name.c_str());
         assert(i != nsmap.end());

         // now get the namespace entry
         nsdmap_t::iterator di = i->second.find(ri->second);
         assert(di != i->second.end());

         // remove from depth -> namespace map
         i->second.erase(di);

         // remove from reverse map
         nsrmap.erase(ri);

         // add new entry to depth -> namespace map
         i->second.insert(nsdmap_t::value_type(ns->depth, ns));

         return;
      }
      else {
         // insert depth -> ns map entry
         nsmap_t::iterator i = nsmap.find(ns->name.c_str());
         if (i == nsmap.end())
            i = nsmap.insert(nsmap_t::value_type(ns->name.c_str(), nsdmap_t())).first;

         i->second.insert(nsdmap_t::value_type(ns->depth, ns));
      }

      // add new entry to reverse map
      nsrmap.insert(nsrmap_t::value_type(ns, ns->depth));
   }

   DLLLOCAL void commit(NamespaceMap& pend) {
      // commit entries
      for (nsrmap_t::iterator i = pend.nsrmap.begin(), e = pend.nsrmap.end(); i != e; ++i)
         update(i->first);
      pend.clear();
   }

   DLLLOCAL void clear() {
      nsmap.clear();
      nsrmap.clear();
   }
};

class NamespaceMapIterator {
protected:
   NamespaceMap::nsmap_t::iterator mi;
   NamespaceMap::nsdmap_t::iterator i;
   bool valid;

public:
   DLLLOCAL NamespaceMapIterator(NamespaceMap& nsm, const char* name) : mi(nsm.nsmap.find(name)), valid(mi != nsm.nsmap.end()) {
      if (valid)
         i = mi->second.end();

      /*
      //printd(5, "NamespaceMapIterator::NamespaceMapIterator(%s) valid: %d\n", name, valid);
      for (NamespaceMap::nsmap_t::iterator mmi = nsm.nsmap.begin(), mme = nsm.nsmap.end(); mmi != mme; ++mmi) {
         //printd(5, "NamespaceMapIterator::NamespaceMapIterator(%s) size: %d\n", name, mmi->second.size());
         for (NamespaceMap::nsdmap_t::iterator di = mmi->second.begin(), de = mmi->second.end(); di != de; ++di) {
            //printd(5, "NamespaceMapIterator::NamespaceMapIterator(%s) ns: %p (%s) depth: %d\n", name, di->second, di->second->name.c_str(), di->second->depth);            
         }
      }
      */
   }

   DLLLOCAL bool next() {
      if (!valid)
         return false;

      if (i == mi->second.end())
         i = mi->second.begin();
      else
         ++i;

      return i != mi->second.end();      
   }

   DLLLOCAL qore_ns_private* get() {
      return i->second;
   }
};

typedef RootMap<FunctionEntry> fmap_t;

typedef RootMap<ConstantEntry> cnmap_t;

typedef RootMap<QoreClass> clmap_t;

class qore_root_ns_private : public qore_ns_private {
   friend class qore_ns_private;

protected:
   DLLLOCAL int addPendingVariant(qore_ns_private& ns, const char* name, AbstractQoreFunctionVariant* v) {
      // try to add function variant to given namespace
      bool new_func = false;
      FunctionEntry* fe = ns.addPendingVariantIntern(name, v, new_func);
      if (!fe)
         return -1;

      if (new_func) {
         fmap_t::iterator i = fmap.find(fe->getName());
         // only add to pending map if either not in the committed map or the depth is higher in the committed map
         if (i == fmap.end() || i->second.depth() > ns.depth)
            pend_fmap.update(fe->getName(), &ns, fe);
      }

      return 0;      
   }

   DLLLOCAL int addPendingVariant(qore_ns_private& ns, const NamedScope& nscope, AbstractQoreFunctionVariant* v) {
      assert(nscope.size() > 1);
      SimpleRefHolder<AbstractQoreFunctionVariant> vh(v);

      QoreNamespace* fns = ns.ns;
      for (unsigned i = 0; i < nscope.size() - 1; ++i) {
         fns = fns->priv->parseFindLocalNamespace(nscope[i]);
         if (!fns) {
            parse_error("cannot find namespace '%s' in '%s()' as a child of namespace '%s'\n", nscope[i], nscope.ostr, ns.name.c_str());
            return -1;
         }
      }

      return addPendingVariant(*fns->priv, nscope.getIdentifier(), vh.release());
   }

   // performed at runtime
   DLLLOCAL int importFunction(ExceptionSink *xsink, qore_ns_private& ns, QoreFunction *u, const char *new_name = 0) {
      FunctionEntry* fe = ns.importFunction(xsink, u, new_name);
      if (!fe)
         return -1;

      //printd(5, "qore_root_ns_private::importFunction() this: %p ns: %p '%s' (depth %d) func: %p %s\n", this, &ns, ns.name.c_str(), ns.depth, u, fe->getName());

      fmap.update(fe->getName(), &ns, fe);
      return 0;      
   }

   DLLLOCAL bool runtimeExistsFunctionIntern(const char* name) {
      return fmap.find(name) != fmap.end();
   }

   DLLLOCAL const QoreFunction* runtimeFindFunctionIntern(const char* name, const qore_ns_private*& ns) {
      fmap_t::iterator i = fmap.find(name);

      if (i != fmap.end()) {
         ns = i->second.ns;
         //printd(5, "qore_root_ns_private::runtimeFindFunctionIntern() this: %p %s found in ns: '%s' depth: %d\n", this, name, ns->name.c_str(), ns->depth);
         return i->second.obj->getFunction();
      }

      //printd(5, "qore_root_ns_private::runtimeFindFunctionIntern() this: %p %s not found i: %d\n", this, name, i != fmap.end());
      return 0;
   }

   DLLLOCAL FunctionEntry* parseFindFunctionEntryIntern(const char* name) {
      {
         // try to check in current namespace first
         qore_ns_private* nscx = parse_get_ns();
         if (nscx) {
            FunctionEntry* fe = nscx->func_list.findNode(name);
            if (fe)
               return fe;
         }
      }

      fmap_t::iterator i = fmap.find(name);
      fmap_t::iterator ip = pend_fmap.find(name);

      if (i != fmap.end()) {
         if (ip != pend_fmap.end()) {
            if (i->second.depth() < ip->second.depth())
               return i->second.obj;

            return ip->second.obj;
         }
 
         return i->second.obj;
      }

      if (ip != pend_fmap.end())
         return ip->second.obj;

      return 0;
   }

   DLLLOCAL QoreFunction* parseFindFunctionIntern(const char* name) {
      FunctionEntry* fe = parseFindFunctionEntryIntern(name);
      return !fe ? 0 : fe->getFunction();
   }

   DLLLOCAL void runtimeFindCallFunctionIntern(const char* name, const QoreFunction*& ufc) {
      assert(!ufc);

      fmap_t::iterator i = fmap.find(name);
      
      if (i != fmap.end()) {
         ufc = i->second.obj->getFunction();
         return;
      }

      ufc = builtinFunctions.find(name);
   }

   DLLLOCAL const QoreFunction* parseResolveFunctionIntern(const char* fname) {
      QORE_TRACE("qore_root_ns_private::parseResolveFunctionIntern()");

      const QoreFunction* f = parseFindFunctionIntern(fname);
      if (!f) {
         f = builtinFunctions.find(fname);

         // cannot find function, throw exception
         if (!f)
            parse_error("function '%s()' cannot be found", fname);
      }

      return f;
   }

   // called during parsing (plock already grabbed)
   DLLLOCAL AbstractCallReferenceNode* parseResolveCallReferenceIntern(UnresolvedProgramCallReferenceNode* fr) {
      std::auto_ptr<UnresolvedProgramCallReferenceNode> fr_holder(fr);
      char* fname = fr->str;

      FunctionEntry* fe = parseFindFunctionEntryIntern(fname);
      if (fe) {
         // check parse options to see if access is allowed
         /*
         if (fe->getFunction()->getUniqueFunctionality() & getProgram()->getParseOptions64()) 
            parse_error("parse options do not allow access to function '%s'", fname);
         else 
         */
            return fe->makeCallReference();
      }
      /*
      else
         // cannot find function, throw exception
         parse_error("reference to function '%s()' cannot be resolved", fname);

      return fr_holder.release();
      */

      const QoreFunction* bfc;
      if ((bfc = builtinFunctions.find(fname))) {
         printd(5, "qore_root_ns_private::parseResolveCallReference() resolved function reference to builtin function to %s\n", fname);
      
         // check parse options to see if access is allowed
         if (bfc->getUniqueFunctionality() & getProgram()->getParseOptions64())
            parse_error("parse options do not allow access to builtin function '%s()'", fname);
         else 
            return new LocalFunctionCallReferenceNode(bfc);
      }
      else
         // cannot find function, throw exception
         parse_error("reference to function '%s()' cannot be resolved", fname);

      return fr_holder.release();
   }

   DLLLOCAL void parseCommit() {
      // commit pending function lookup entries
      for (fmap_t::iterator i = pend_fmap.begin(), e = pend_fmap.end(); i != e; ++i)
         fmap.update(i);
      pend_fmap.clear();

      // commit pending constant lookup entries
      for (cnmap_t::iterator i = pend_cnmap.begin(), e = pend_cnmap.end(); i != e; ++i)
         cnmap.update(i);
      pend_cnmap.clear();
      
      // commit pending class lookup entries
      for (clmap_t::iterator i = pend_clmap.begin(), e = pend_clmap.end(); i != e; ++i)
         clmap.update(i);
      pend_clmap.clear();

      // commit pending namespace entries
      nsmap.commit(pend_nsmap);

      qore_ns_private::parseCommit();
   }

   DLLLOCAL void parseRollback() {
      // roll back pending lookup entries
      pend_fmap.clear();
      pend_cnmap.clear();
      pend_clmap.clear();
      pend_nsmap.clear();

      qore_ns_private::parseRollback();
   }

   ConstantEntry* parseFindOnlyConstantEntryIntern(const char* cname, qore_ns_private*& ns) {
      {
         // first try to look in current namespace context
         qore_ns_private* nscx = parse_get_ns();
         if (nscx) {
            ConstantEntry* ce = nscx->constant.findEntry(cname);
            if (!ce)
               ce = nscx->pendConstant.findEntry(cname);
            if (ce) {
               ns = nscx;
               return ce;
            }
         }
      }

      // look up in global constant map
      cnmap_t::iterator i = cnmap.find(cname);
      cnmap_t::iterator ip = pend_cnmap.find(cname);

      if (i != cnmap.end()) {
         if (ip != pend_cnmap.end()) {
            if (i->second.depth() < ip->second.depth()) {
               ns = i->second.ns;
               return i->second.obj;
            }

            ns = ip->second.ns;
            return ip->second.obj;;
         }

         ns = i->second.ns;
         return i->second.obj;;
      }

      if (ip != pend_cnmap.end()) {
         ns = ip->second.ns;
         return ip->second.obj;;
      }

      return 0;
   }

   AbstractQoreNode* parseFindOnlyConstantValueIntern(const char* cname, const QoreTypeInfo*& typeInfo) {
      qore_ns_private* ns;
      ConstantEntry *ce = parseFindOnlyConstantEntryIntern(cname, ns);
      if (!ce)
         return 0;

      //printd(5, "qore_root_ns_private::parseFindOnlyConstantValueIntern() const: %s ns: %p %s\n", cname, ns, ns->name.c_str());

      NamespaceParseContextHelper nspch(ns);
      return ce->get(typeInfo);
   }

   AbstractQoreNode* parseFindConstantValueIntern(const char* cname, const QoreTypeInfo*& typeInfo, bool error) {
      // look up class constants first
      QoreClass* pc = getParseClass();
      if (pc) {
         AbstractQoreNode* rv = qore_class_private::parseFindConstantValue(pc, cname, typeInfo);
         if (rv)
            return rv;
      }

      AbstractQoreNode* rv = parseFindOnlyConstantValueIntern(cname, typeInfo);

      if (rv)
         return rv;

      if (error)
         parse_error("constant '%s' cannot be resolved in any namespace", cname);      

      return 0;
   }

   DLLLOCAL ResolvedCallReferenceNode* runtimeGetCallReference(const char *fname, ExceptionSink* xsink) {
      fmap_t::iterator i = fmap.find(fname);
      if (i == fmap.end()) {
         xsink->raiseException("NO-SUCH-FUNCTION", "callback function '%s()' does not exist", fname);
         return 0;
      }

      return i->second.obj->makeCallReference();
   }

   DLLLOCAL QoreClass *parseFindScopedClassIntern(const NamedScope* name);
   DLLLOCAL QoreClass *parseFindScopedClassIntern(const NamedScope* name, unsigned& matched);
   DLLLOCAL QoreClass *parseFindScopedClassWithMethodInternError(const NamedScope *name, bool error);
   DLLLOCAL QoreClass *parseFindScopedClassWithMethodIntern(const NamedScope *name, unsigned& matched);

   DLLLOCAL QoreClass* parseFindClassIntern(const char *cname, bool error) {
      {
         // try to check in current namespace first
         qore_ns_private* nscx = parse_get_ns();
         if (nscx) {
            QoreClass* qc = nscx->parseFindLocalClass(cname);
            if (qc)
               return qc;
         }
      }

      clmap_t::iterator i = clmap.find(cname);
      clmap_t::iterator ip = pend_clmap.find(cname);

      if (i != clmap.end()) {
         if (ip != pend_clmap.end()) {
            if (i->second.depth() < ip->second.depth())
               return i->second.obj;
            return ip->second.obj;
         }

         return i->second.obj;
      }

      if (ip != pend_clmap.end())
         return ip->second.obj;

      if (error)
         parse_error("reference to undefined class '%s'", cname);

      return 0;
   }

   DLLLOCAL QoreClass* runtimeFindClass(const char *name) {
      clmap_t::iterator i = clmap.find(name);
      return i != clmap.end() ? i->second.obj : 0;
   }

   DLLLOCAL void addConstant(qore_ns_private& ns, const char* cname, AbstractQoreNode *value, const QoreTypeInfo* typeInfo);

   DLLLOCAL AbstractQoreNode *parseFindConstantValueIntern(const NamedScope *name, const QoreTypeInfo*& typeInfo, bool error);

   DLLLOCAL AbstractQoreNode* parseResolveBarewordIntern(const char* bword, const QoreTypeInfo*& typeInfo);

   DLLLOCAL AbstractQoreNode *parseResolveScopedReferenceIntern(const NamedScope& name, const QoreTypeInfo *&typeInfo);

   DLLLOCAL void parseAddConstantIntern(QoreNamespace& ns, const NamedScope& name, AbstractQoreNode* value);

   DLLLOCAL void parseAddClassIntern(const NamedScope *name, QoreClass *oc);

   DLLLOCAL QoreNamespace *parseResolveNamespace(const NamedScope *nscope);

   DLLLOCAL const QoreFunction* parseResolveFunctionIntern(const NamedScope& nscope);

   // returns 0 for success, non-zero for error
   DLLLOCAL int parseAddMethodToClassIntern(const NamedScope *name, MethodVariantBase *qcmethod, bool static_flag);

   DLLLOCAL static void rebuildConstantIndexes(cnmap_t& cnmap, ConstantList& cl, qore_ns_private* ns) {
      ConstantListIterator cli(cl);
      while (cli.next())
         cnmap.update(cli.getName().c_str(), ns, cli.getEntry());
   }

   DLLLOCAL static void rebuildClassIndexes(clmap_t& clmap, QoreClassList& cl, qore_ns_private* ns) {
      ClassListIterator cli(cl);
      while (cli.next())
         clmap.update(cli.getName(), ns, cli.get());
   }

   DLLLOCAL void rebuildIndexes(qore_ns_private* ns) {
      // process function indexes
      for (fl_map_t::iterator i = ns->func_list.begin(), e = ns->func_list.end(); i != e; ++i) {
         fmap.update(i->first, ns, i->second);
         //printd(5, "qore_root_ns_private::rebuildIndexes() this: %p ns: %p func %s\n", this, ns, i->first);
      }

      // process constant indexes
      rebuildConstantIndexes(cnmap, ns->constant, ns);

      // process class indexes
      rebuildClassIndexes(clmap, ns->classList, ns);

      // reindex namespace
      nsmap.update(ns);
   }

   DLLLOCAL void parseRebuildIndexes(qore_ns_private* ns) {
      //printd(5, "qore_root_ns_private::parseRebuildIndexes() this: %p ns: %p (%s) depth %d\n", this, ns, ns->name.c_str(), ns->depth);
      
      // process function indexes
      for (fl_map_t::iterator i = ns->func_list.begin(), e = ns->func_list.end(); i != e; ++i)
         pend_fmap.update(i->first, ns, i->second);

      // process pending constant indexes
      rebuildConstantIndexes(pend_cnmap, ns->pendConstant, ns);

      // process constant indexes
      rebuildConstantIndexes(cnmap, ns->constant, ns);

      // process pending class indexes
      rebuildClassIndexes(pend_clmap, ns->pendClassList, ns);

      // process class indexes
      rebuildClassIndexes(clmap, ns->classList, ns);

      // reindex namespace
      pend_nsmap.update(ns);
   }

   DLLLOCAL void parseAddNamespaceIntern(QoreNamespace *nns) {
      qore_ns_private *ns = qore_ns_private::parseAddNamespace(nns);

      //printd(5, "qore_root_ns_private::parseAddNamespaceIntern() this: %p ns: %p\n", this, ns);

      // add all objects to the new (or assimilated) namespace
      if (ns) {
         QorePrivateNamespaceIterator qpni(ns, false);
         while (qpni.next())
            parseRebuildIndexes(qpni.get());
      }
   }

public:
   RootQoreNamespace* rns;
   QoreNamespace* qoreNS;

   fmap_t fmap,    // root function map
      pend_fmap;   // root pending function map (only used during parsing)

   cnmap_t cnmap,  // root constant map
      pend_cnmap;  // root pending constant map (used only during parsing)
   
   clmap_t clmap,  // root class map
      pend_clmap;  // root pending class map (used only during parsing)

   NamespaceMap nsmap,  // root namespace map
      pend_nsmap;       // root pending namespace map (used only during parsing)

   DLLLOCAL qore_root_ns_private(RootQoreNamespace* n_rns) : qore_ns_private(n_rns), rns(n_rns), qoreNS(0) {
      root = true;
   }

   DLLLOCAL qore_root_ns_private(const qore_root_ns_private& old, int64 po) : qore_ns_private(old, po) {
      qoreNS = nsl.find("Qore");
      assert(qoreNS);

      // rebuild root indexes - only for committed objects
      QorePrivateNamespaceIterator qpni(this, true);
      while (qpni.next())
         rebuildIndexes(qpni.get());
   }

   DLLLOCAL ~qore_root_ns_private() {
   }

   DLLLOCAL RootQoreNamespace* copy(int64 po) {
      qore_root_ns_private* p = new qore_root_ns_private(*this, po);
      RootQoreNamespace* rv = new RootQoreNamespace(p);
      return rv;
   }

   DLLLOCAL static RootQoreNamespace* copy(const RootQoreNamespace& rns, int64 po) {
      return rns.rpriv->copy(po);
   }

   DLLLOCAL static int addPendingVariant(QoreNamespace& ns, const char* name, AbstractQoreFunctionVariant* v) {
      return getRootNS()->rpriv->addPendingVariant(*ns.priv, name, v);
   }

   DLLLOCAL static int addPendingVariant(QoreNamespace& ns, const NamedScope& name, AbstractQoreFunctionVariant* v) {
      return getRootNS()->rpriv->addPendingVariant(*ns.priv, name, v);
   }

   DLLLOCAL static int importFunction(RootQoreNamespace& rns, ExceptionSink *xsink, QoreNamespace& ns, QoreFunction *u, const char *new_name = 0) {
      return rns.rpriv->importFunction(xsink, *ns.priv, u, new_name);
   }

   DLLLOCAL static const QoreFunction* runtimeFindFunction(RootQoreNamespace& rns, const char *name, const qore_ns_private*& ns) {
      return rns.rpriv->runtimeFindFunctionIntern(name, ns);
   }

   DLLLOCAL static bool runtimeExistsFunction(RootQoreNamespace& rns, const char *name) {
      return rns.rpriv->runtimeExistsFunctionIntern(name);
   }

   DLLLOCAL static void runtimeFindCallFunction(RootQoreNamespace& rns, const char* name, const QoreFunction*& ufc) {
      return rns.rpriv->runtimeFindCallFunctionIntern(name, ufc);
   }

   DLLLOCAL static void addConstant(qore_root_ns_private& rns, qore_ns_private& ns, const char* cname, AbstractQoreNode *value, const QoreTypeInfo* typeInfo) {
      rns.addConstant(ns, cname, value, typeInfo);
   }

   DLLLOCAL static const QoreFunction* parseResolveFunction(const char* fname) {
      return getRootNS()->rpriv->parseResolveFunctionIntern(fname);
   }

   // called during parsing (plock already grabbed)
   DLLLOCAL static AbstractCallReferenceNode* parseResolveCallReference(UnresolvedProgramCallReferenceNode* fr) {
      return getRootNS()->rpriv->parseResolveCallReferenceIntern(fr);
   }

   DLLLOCAL static void parseInit() {
      qore_ns_private* p = getRootNS()->priv;
      p->parseInitConstants();
      p->parseInit();
   }

   DLLLOCAL static void parseCommit(RootQoreNamespace& rns) {
      rns.rpriv->parseCommit();
   }

   DLLLOCAL static void parseRollback(RootQoreNamespace& rns) {
      rns.rpriv->parseRollback();
   }

   DLLLOCAL static AbstractQoreNode *parseFindConstantValue(const char *name, const QoreTypeInfo *&typeInfo, bool error) {
      return getRootNS()->rpriv->parseFindConstantValueIntern(name, typeInfo, error);
   }

   DLLLOCAL static AbstractQoreNode *parseFindConstantValue(const NamedScope *name, const QoreTypeInfo *&typeInfo, bool error) {
      return getRootNS()->rpriv->parseFindConstantValueIntern(name, typeInfo, error);
   }

   DLLLOCAL static AbstractQoreNode* parseResolveBareword(const char* bword, const QoreTypeInfo *&typeInfo) {
      return getRootNS()->rpriv->parseResolveBarewordIntern(bword, typeInfo);
   }

   DLLLOCAL static AbstractQoreNode *parseResolveScopedReference(const NamedScope& name, const QoreTypeInfo*& typeInfo) {
      return getRootNS()->rpriv->parseResolveScopedReferenceIntern(name, typeInfo);
   }

   DLLLOCAL static QoreClass *parseFindClass(const char *name, bool error) {
      return getRootNS()->rpriv->parseFindClassIntern(name, error);
   }

   DLLLOCAL static QoreClass *parseFindScopedClass(const NamedScope *name) {
      return getRootNS()->rpriv->parseFindScopedClassIntern(name);
   }

   DLLLOCAL static QoreClass *parseFindScopedClassWithMethod(const NamedScope *name, bool error) {
      return getRootNS()->rpriv->parseFindScopedClassWithMethodInternError(name, error);
   }

   DLLLOCAL static void parseAddConstant(QoreNamespace& ns, const NamedScope &name, AbstractQoreNode *value) {
      getRootNS()->rpriv->parseAddConstantIntern(ns, name, value);
   }

   // returns 0 for success, non-zero for error
   DLLLOCAL static int parseAddMethodToClass(const NamedScope *name, MethodVariantBase *qcmethod, bool static_flag) {
      return getRootNS()->rpriv->parseAddMethodToClassIntern(name, qcmethod, static_flag);
   }

   DLLLOCAL static void parseAddClass(const NamedScope *name, QoreClass *oc) {
      getRootNS()->rpriv->parseAddClassIntern(name, oc);
   }

   DLLLOCAL static void parseAddNamespace(QoreNamespace *nns) {
      getRootNS()->rpriv->parseAddNamespaceIntern(nns);
   }

   DLLLOCAL static const QoreFunction* parseResolveFunction(const NamedScope& nscope) {
      return getRootNS()->rpriv->parseResolveFunctionIntern(nscope);
   }

   DLLLOCAL static ResolvedCallReferenceNode* runtimeGetCallReference(RootQoreNamespace& rns, const char *name, ExceptionSink* xsink) {
      return rns.rpriv->runtimeGetCallReference(name, xsink);
   }

   DLLLOCAL static void runtimeModuleRebuildIndexes(RootQoreNamespace& rns) {
      // rebuild root indexes
      QorePrivateNamespaceIterator qpni(rns.priv, true);
      while (qpni.next())
         rns.rpriv->rebuildIndexes(qpni.get());
   }

   DLLLOCAL static QoreClass *runtimeFindClass(RootQoreNamespace& rns, const char *name) {
      return rns.rpriv->runtimeFindClass(name);
   }
};

#endif
