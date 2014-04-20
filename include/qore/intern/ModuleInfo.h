/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ModuleInfo.h

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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

#ifndef _QORE_MODULEINFO_H

#define _QORE_MODULEINFO_H

#ifdef NEED_DLFCN_WRAPPER
extern "C" {
#endif
#include <dlfcn.h>
#ifdef NEED_DLFCN_WRAPPER
}
#endif

#include <string>
#include <map>
#include <deque>

// user module parse options
#define USER_MOD_PO (PO_NO_TOP_LEVEL_STATEMENTS | PO_REQUIRE_PROTOTYPES | PO_REQUIRE_OUR | PO_IN_MODULE)

//! list of version numbers in order of importance (i.e. 1.2.3 = 1, 2, 3)
struct version_list_t : public std::vector<int> {
protected:
   QoreString ver;

public:
   DLLLOCAL version_list_t() {
   }

   DLLLOCAL version_list_t(const char* v) {
      set(v);
   }

   DLLLOCAL char set(const char* v);

   DLLLOCAL const char* operator*() const {
      return ver.getBuffer();
   }
};

class QoreAbstractModule {
private:
   // not implemented
   DLLLOCAL QoreAbstractModule(const QoreAbstractModule&);
   DLLLOCAL QoreAbstractModule& operator=(const QoreAbstractModule&);

protected:
   QoreString filename,
      name, 
      desc,
      author,
      url;

   //typedef std::map<std::string, std::string> strmap_t;
   //strmap_t strmap;

   DLLLOCAL QoreHashNode* getHashIntern(bool with_filename = true) const {
      QoreHashNode* h = new QoreHashNode;

      if (with_filename)
         h->setKeyValue("filename", new QoreStringNode(filename), 0);
      h->setKeyValue("name", new QoreStringNode(name), 0);
      h->setKeyValue("desc", new QoreStringNode(desc), 0);
      h->setKeyValue("version", new QoreStringNode(*version_list), 0);
      h->setKeyValue("author", new QoreStringNode(author), 0);
      if (!url.empty())
         h->setKeyValue("url", new QoreStringNode(url), 0);

      return h;
   }

public:
   version_list_t version_list;

   DLLLOCAL QoreAbstractModule(const char* fn, const char* n, const char* d, const char* v, const char* a, const char* u) : filename(fn), name(n), desc(d), author(a), url(u), version_list(v) {
   }

   // for "builtin" modules
   DLLLOCAL virtual ~QoreAbstractModule() {
   }

   DLLLOCAL const char* getName() const {
      return name.getBuffer();
   }

   DLLLOCAL const char* getFileName() const {
      return filename.getBuffer();
   }

   DLLLOCAL const char* getDesc() const {
      return desc.getBuffer();
   }

   DLLLOCAL const char* getVersion() const {
      return* version_list;
   }

   DLLLOCAL const char* getURL() const {
      return url.getBuffer();
   }

   DLLLOCAL virtual bool isBuiltin() const = 0;
   DLLLOCAL virtual bool isUser() const = 0;
   DLLLOCAL virtual QoreHashNode* getHash(bool with_filename = true) const = 0;
   DLLLOCAL virtual void addToProgram(QoreProgram* pgm, ExceptionSink& xsink) const = 0;
   DLLLOCAL virtual void issueParseCmd(QoreString &cmd) = 0;
};

// list/dequeue of strings
typedef std::deque<std::string> strdeque_t;

//! non-thread-safe unique list of strings of directory names
/** a deque should require fewer memory allocations compared to a linked list.
    the set is used for uniqueness
 */
class UniqueDirectoryList {
protected:
   typedef std::set<std::string> strset_t;

   strdeque_t dlist;
   strset_t dset;

public:
   DLLLOCAL void addDirList(const char* str);

   DLLLOCAL bool push_back(const char* str) {
      if (dset.find(str) != dset.end())
         return false;

      dlist.push_back(str);
      dset.insert(str);
      return true;
   }

   DLLLOCAL bool empty() const {
      return dlist.empty();
   }

   DLLLOCAL strdeque_t::const_iterator begin() const {
      return dlist.begin();
   }

   DLLLOCAL strdeque_t::const_iterator end() const {
      return dlist.end();
   }

   DLLLOCAL void appendPath(QoreString& str) const {
      if (dlist.empty()) {
         str.concat("<empty>");
         return;
      }
      for (strdeque_t::const_iterator i = dlist.begin(), e = dlist.end(); i != e; ++i) {
         str.concat((*i).c_str());
         str.concat(':');
      }
      str.terminate(str.size() - 1);
   }
};

class QoreModuleContextHelper : public QoreModuleContext {
public:
   DLLLOCAL QoreModuleContextHelper(const char* name, QoreProgram* pgm, ExceptionSink& xsink);
   DLLLOCAL ~QoreModuleContextHelper();
};

class QoreModuleDefContextHelper : public QoreModuleDefContext {
protected:
   QoreModuleDefContext* old;

public:
   DLLLOCAL QoreModuleDefContextHelper() : old(set_module_def_context(this)) {
   }

   DLLLOCAL ~QoreModuleDefContextHelper() {
      set_module_def_context(old);
   }
};

class QoreUserModuleDefContextHelper;

typedef std::set<std::string> strset_t;   
typedef std::multimap<std::string, strset_t> md_map_t;

class ModMap {
protected:
   md_map_t map;

public:
   DLLLOCAL bool addDep(const char* l, const char* r) {
      md_map_t::iterator i = map.find(l);      
      if (i == map.end())
         i = map.insert(md_map_t::value_type(l, strset_t()));
      else if (i->second.find(r) != i->second.end())
         return true;
      i->second.insert(r);
      return false;
   }

   DLLLOCAL md_map_t::iterator begin() {
      return map.begin();
   }

   DLLLOCAL md_map_t::iterator end() {
      return map.end();
   }

   DLLLOCAL md_map_t::iterator find(const char* n) {
      return map.find(n);
   }

   DLLLOCAL md_map_t::iterator find(const std::string& n) {
      return map.find(n);
   }

   DLLLOCAL void erase(md_map_t::iterator i) {
      map.erase(i);
   }

   DLLLOCAL void clear() {
      map.clear();
   }

   DLLLOCAL bool empty() const {
      return map.empty();
   }
};

class QoreModuleManager {
private:
   // not implemented
   DLLLOCAL QoreModuleManager(const QoreModuleManager&);
   // not implemented
   DLLLOCAL QoreModuleManager& operator=(const QoreModuleManager&);

protected:
   // recursive mutex; initialized in init()
   QoreThreadLock* mutex;

   // user module dependency map: module -> dependents
   ModMap md_map;
   // user module dependent map: dependent -> module
   ModMap rmd_map;

   // module blacklist
   typedef std::map<const char*, const char*, ltstr> bl_map_t;
   bl_map_t mod_blacklist;

   // module hash
   typedef std::map<const char*, QoreAbstractModule*, ltstr> module_map_t;
   module_map_t map;   

   // set of user modules with no dependencies
   strset_t umset;

   // list of module directories
   UniqueDirectoryList moduleDirList;

   DLLLOCAL QoreAbstractModule* findModuleUnlocked(const char* name) {
      module_map_t::iterator i = map.find(name);
      return i == map.end() ? 0 : i->second;
   }

   DLLLOCAL void loadModuleIntern(const char* name, QoreProgram* pgm, ExceptionSink& xsink) {
      AutoLocker sl(mutex); // make sure checking and loading are atomic

      loadModuleIntern(xsink, name, pgm);
   }

   DLLLOCAL void loadModuleIntern(ExceptionSink& xsink, const char* name, QoreProgram* pgm, mod_op_e op = MOD_OP_NONE, version_list_t* version = 0, const char* src = 0);

   DLLLOCAL QoreAbstractModule* loadBinaryModuleFromPath(ExceptionSink& xsink, const char* path, const char* feature = 0, QoreProgram* pgm = 0);
   DLLLOCAL QoreAbstractModule* loadUserModuleFromPath(ExceptionSink& xsink, const char* path, const char* feature = 0, QoreProgram* pgm = 0);
   DLLLOCAL QoreAbstractModule* loadUserModuleFromSource(ExceptionSink& xsink, const char* path, const char* feature, QoreProgram* tpgm, const char* src);
   DLLLOCAL QoreAbstractModule* setupUserModule(ExceptionSink& xsink, const char* path, const char* feature, QoreProgram* tpgm, ReferenceHolder<QoreProgram>& pgm, QoreUserModuleDefContextHelper& qmd);

public:
   DLLLOCAL QoreModuleManager() : mutex(0) {
   }

   DLLLOCAL ~QoreModuleManager() {
      delete mutex;
   }

   DLLLOCAL void init(bool se);
   DLLLOCAL void delUser();
   DLLLOCAL void cleanup();
   DLLLOCAL void issueParseCmd(const char* mname, QoreProgram* pgm, QoreString &cmd);

   DLLLOCAL void addModule(QoreAbstractModule* m) {
      map.insert(std::make_pair(m->getName(), m));
   }

   DLLLOCAL QoreAbstractModule* findModule(const char* name) {
      AutoLocker al(mutex);
      return findModuleUnlocked(name);
   }

   DLLLOCAL void parseLoadModule(const char* name, QoreProgram* pgm, ExceptionSink& xsink);
   DLLLOCAL int runTimeLoadModule(const char* name, QoreProgram* pgm, ExceptionSink& xsink);

   DLLLOCAL QoreHashNode* getModuleHash();
   DLLLOCAL QoreListNode* getModuleList();

   DLLLOCAL void addModuleDir(const char* dir) {
      OptLocker al(mutex);
      moduleDirList.push_back(dir);
   }

   DLLLOCAL void addModuleDirList(const char* strlist) {
      OptLocker al(mutex);
      moduleDirList.addDirList(strlist);
   }

   DLLLOCAL void addStandardModulePaths();

   DLLLOCAL void registerUserModuleFromSource(const char* name, const char* src, QoreProgram *pgm, ExceptionSink& xsink);

   DLLLOCAL void trySetUserModuleDependency(const QoreAbstractModule* mi) {
      if (!mi->isUser())
         return;

      const char* old_name = get_user_module_context_name();
      if (old_name)
         setUserModuleDependency(mi->getName(), old_name);
      trySetUserModule(mi->getName());
   }

   DLLLOCAL void trySetUserModule(const char* name) {
      md_map_t::iterator i = md_map.find(name);
      if (i == md_map.end()) {
         umset.insert(name);
         //printd(5, "QoreModuleManager::trySetUserModule('%s') UMSET SET: rmd_map: empty\n", name);
      }
#ifdef DEBUG
      else {
	 QoreString str("[");
	 for (strset_t::iterator si = i->second.begin(), se = i->second.end(); si != se; ++si)
	    str.sprintf("'%s',", (*si).c_str());
	 str.concat("]");
         //printd(5, "QoreModuleManager::trySetUserModule('%s') UMSET NOT SET: md_map: %s\n", name, str.getBuffer());
      }
#endif
   }

   DLLLOCAL void setUserModuleDependency(const char* name, const char* dep) {
      //printd(5, "QoreModuleManager::setUserModuleDependency('%s' -> '%s')\n", name, dep);      
      if (md_map.addDep(name, dep))
         return;
      rmd_map.addDep(dep, name);

      strset_t::iterator ui = umset.find(name);
      if (ui != umset.end()) {
         umset.erase(ui);
         //printd(5, "QoreModuleManager::setUserModuleDependency('%s' -> '%s') REMOVED '%s' FROM UMMSET\n", name, dep, name);
      }
   }

   DLLLOCAL void removeUserModuleDependency(const char* name) {
      //printd(5, "QoreModuleManager::removeUserModuleDependency('%s')\n", name);
      md_map_t::iterator i = rmd_map.find(name);
      if (i == rmd_map.end()) {
         //printd(5, "QoreModuleManager::removeUserModuleDependency('%s') no deps\n", name);
         return;
      }

      // remove dependents
      for (strset_t::iterator si = i->second.begin(), se = i->second.end(); si != se; ++si) {
         md_map_t::iterator di = md_map.find(*si);
         assert(di != md_map.end());

         strset_t::iterator dsi = di->second.find(i->first);
         assert(dsi != di->second.end());
         di->second.erase(dsi);
         if (di->second.empty()) {
            //printd(5, "QoreModuleManager::removeUserModuleDependency('%s') '%s' now empty, ADDING TO UMMSET: '%s'\n", name, i->first.c_str(), (*si).c_str());
            md_map.erase(di);
            assert(umset.find(*si) == umset.end());
            umset.insert(*si);
         }
      }
      // remove from dep map
      rmd_map.erase(i);
   }
};

DLLLOCAL extern QoreModuleManager QMM;

class QoreBuiltinModule : public QoreAbstractModule {
protected:
   unsigned api_major, api_minor;
   qore_module_init_t module_init;
   qore_module_ns_init_t module_ns_init;
   qore_module_delete_t module_delete;
   qore_module_parse_cmd_t module_parse_cmd;
   const void* dlptr;

public:
   DLLLOCAL QoreBuiltinModule(const char* fn, const char* n, const char* d, const char* v, const char* a, const char* u, unsigned major, unsigned minor, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del, qore_module_parse_cmd_t pcmd, const void* p) : QoreAbstractModule(fn, n, d, v, a, u), api_major(major), api_minor(minor), module_init(init), module_ns_init(ns_init), module_delete(del), module_parse_cmd(pcmd), dlptr(p) {
   }

   DLLLOCAL virtual ~QoreBuiltinModule() {
      printd(5, "ModuleInfo::~ModuleInfo() '%s': %s calling module_delete: %p\n", name.getBuffer(), filename.getBuffer(), module_delete);
      module_delete();
      if (dlptr) {
         printd(5, "calling dlclose(%p)\n", dlptr);
#ifndef DEBUG
         // do not close modules when debugging
         dlclose((void* )dlptr);
#endif
      }
   }

   DLLLOCAL unsigned getAPIMajor() const {
      return api_major;
   }

   DLLLOCAL unsigned getAPIMinor() const {
      return api_minor;
   }

   DLLLOCAL virtual bool isBuiltin() const {
      return true;
   }

   DLLLOCAL virtual bool isUser() const {
      return false;
   }

   DLLLOCAL QoreHashNode* getHash(bool with_filename = true) const {
      QoreHashNode* h = getHashIntern(with_filename);
   
      h->setKeyValue("user", &False, 0);
      h->setKeyValue("api_major", new QoreBigIntNode(api_major), 0);
      h->setKeyValue("api_minor", new QoreBigIntNode(api_minor), 0);
   
      return h;
   }

   DLLLOCAL const void* getPtr() const {
      return dlptr;
   }

   DLLLOCAL virtual void addToProgram(QoreProgram* pgm, ExceptionSink& xsink) const;
   DLLLOCAL virtual void issueParseCmd(QoreString &cmd);
};

class QoreUserModule : public QoreAbstractModule {
protected:
   QoreProgram* pgm;
   QoreClosureParseNode* del; // deletion closure

public:
   DLLLOCAL QoreUserModule(const char* fn, const char* n, const char* d, const char* v, const char* a, const char* u, QoreProgram* p, QoreClosureParseNode* dl) : QoreAbstractModule(fn, n, d, v, a, u), pgm(p), del(dl) {
   }

   DLLLOCAL virtual ~QoreUserModule();

   DLLLOCAL virtual bool isBuiltin() const {
      return false;
   }

   DLLLOCAL virtual bool isUser() const {
      return true;
   }

   DLLLOCAL virtual QoreHashNode* getHash(bool with_filename = true) const {
      return getHashIntern(with_filename);
   }

   DLLLOCAL virtual void addToProgram(QoreProgram* pgm, ExceptionSink& xsink) const;

   DLLLOCAL virtual void issueParseCmd(QoreString &cmd) {
      parseException("PARSE-COMMAND-ERROR", "module '%s' loaded from '%s' is a user module; only builtin modules can support parse commands", name.getBuffer(), filename.getBuffer());
   }
};

class QoreUserModuleDefContextHelper : public QoreModuleDefContextHelper {
protected:
   const char* old_name;

public:
   DLLLOCAL QoreUserModuleDefContextHelper(const char* name) : old_name(set_user_module_context_name(name)) {
   }

   DLLLOCAL ~QoreUserModuleDefContextHelper() {
      set_user_module_context_name(old_name);
   }
};

#endif
