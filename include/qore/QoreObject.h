/*
  QoreObject.h

  thread-safe object definition

  references: how many variables are pointing at this object?

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

#ifndef _QORE_QOREOBJECT_H

#define _QORE_QOREOBJECT_H

//! the implementation of Qore's object data type, reference counted, dynamically-allocated only
/** objects in Qore are unique unless copied explicitly (similar to Java)
    Builtin classes (those classes implemented in C++ as opposed to user classes implemented in the Qore language)
    can have "private data", which is data that maintains the state of the object per that class.  QoreObject
    objects store this data as well as any member data.
    
    objects have two levels of reference counts - one is for the existence of the c++ object (tRefs below)
    the other is for the scope of the object (the parent ReferenceObject) - when this reaches 0 the
    object will have its destructor run (if it hasn't already been deleted)
    only when tRefs reaches 0, meaning that no more pointers are pointing to this object will the object
    actually be deleted

    @see QoreClass
*/
class QoreObject : public AbstractQoreNode
{
   private:
      struct qore_object_private *priv;

      // must only be called when inside the gate
      DLLLOCAL inline void doDeleteIntern(class ExceptionSink *xsink);
      DLLLOCAL void cleanup(class ExceptionSink *xsink, class QoreHashNode *td);
      DLLLOCAL void addVirtualPrivateData(AbstractPrivateData *apd);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreObject(const QoreObject&);
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreObject& operator=(const QoreObject&);

   protected:
      DLLLOCAL virtual ~QoreObject();

   public:
      //! creates an object as belonging to the given class, the QoreProgram object is referenced for the life of the object as well
      /**
	 @param oc the class of the object being created
	 @param p the QoreProgram object where the object "lives", this QoreProgram object is referenced for the life of the object to ensure that it is not deleted while the object still exists (for example, if the object is exported to a parent QoreProgram object)
       */
      DLLEXPORT QoreObject(const QoreClass *oc, class QoreProgram *p);

      //! concatenate the verbose string representation of the list (including all contained values) to an existing QoreString
      /** used for %n and %N printf formatting
	  @param str the string representation of the type will be concatenated to this QoreString reference
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return -1 for exception raised, 0 = OK
      */
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      //! returns a QoreString giving the verbose string representation of the List (including all contained values)
      /** used for %n and %N printf formatting
	  @param del if this is true when the function returns, then the returned QoreString pointer should be deleted, if false, then it must not be
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  NOTE: Use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
	  @see QoreNodeAsStringHelper
      */
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      //! performs a deep copy of the list and returns the new list
      DLLEXPORT virtual class AbstractQoreNode *realCopy() const;

      //! tests for equality ("deep compare" including all contained values) with possible type conversion (soft compare)
      /**
	 @param v the value to compare
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! tests for equality ("deep compare" including all contained values) without type conversions (hard compare)
      /**
	 @param v the value to compare
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! returns the data type
      DLLEXPORT virtual const QoreType *getType() const;

      //! returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;

      // decrements the reference count
      /** deletes the object when the reference count = 0.  The ExceptionSink 
	  argument is needed for those types that could throw an exception when 
	  they are deleted (ex: QoreObject) - which could be contained in this object as well
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT virtual void deref(class ExceptionSink *xsink);

      //! returns true if the list does not contain any parse expressions, otherwise returns false
      DLLEXPORT virtual bool is_value() const;

      DLLEXPORT bool validInstanceOf(int cid) const;

      //! sets the value of the given member to the given value
      /** the value must be already referenced for the assignment to the object
	  @param key the name of the member
	  @param val the value to set for the member (must be already referenced for the assignment, 0 is OK too)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void setValue(const char *key, class AbstractQoreNode *val, class ExceptionSink *xsink);

      //! returns the list of members, caller owns the list returned
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT class QoreListNode *getMemberList(class ExceptionSink *xsink) const;

      //! removes a member from the object, if the member's value is an object it is deleted as well (destructor is called)
      /**
	  @param key the name of the member to delete
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void deleteMemberValue(const class QoreString *key, class ExceptionSink *xsink);

      //! removes a member from the object, if the member's value is an object it is deleted as well (destructor is called)
      /**
	  @param key the name of the member to delete, assumed to be in the default encoding (QCS_DEFAULT)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void deleteMemberValue(const char *key, class ExceptionSink *xsink);

      //! returns the number of members of the object
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT int size(class ExceptionSink *xsink) const;

      //! tests for equality ("deep compare" including all contained values) with possible type conversion of contained elements (soft compare)
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT bool compareSoft(const class QoreObject *obj, class ExceptionSink *xsink) const;

      //! tests for equality ("deep compare" including all contained values) with possible type conversion of contained elements (hard compare)
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT bool compareHard(const class QoreObject *obj, class ExceptionSink *xsink) const;

      //! returns the value of the given member with the reference count incremented, the caller owns the AbstractQoreNode (reference) returned
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT AbstractQoreNode *getReferencedMemberNoMethod(const char *mem, class ExceptionSink *xsink) const;

      //! retuns all member data of the object (or 0 if there's an exception), caller owns the QoreHashNode reference returned
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT class QoreHashNode *copyData(class ExceptionSink *xsink) const;

      //! copies all member data of the current object to the passed QoreHashNode
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void mergeDataToHash(class QoreHashNode *hash, class ExceptionSink *xsink);

      //! sets private data to the passed key, used in Qore-language constructors
      DLLEXPORT void setPrivate(int key, AbstractPrivateData *pd);

      //! returns the private data corresponding to the key passed with an incremented reference count, caller owns the reference
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT AbstractPrivateData *getReferencedPrivateData(int key, class ExceptionSink *xsink) const;

      //! evaluates the given method with the arguments passed and returns the return value, caller owns the AbstractQoreNode (reference) returned
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT AbstractQoreNode *evalMethod(const class QoreString *name, const class QoreListNode *args, class ExceptionSink *xsink);

      DLLEXPORT const QoreClass *getClass(int cid) const;
      DLLEXPORT const QoreClass *getClass() const;
      DLLEXPORT const char *getClassName() const;
      DLLEXPORT int getStatus() const;
      DLLEXPORT int isValid() const;
      DLLEXPORT class QoreProgram *getProgram() const;
      DLLEXPORT bool isSystemObject() const;
      DLLEXPORT void tRef() const;
      DLLEXPORT void tDeref();
      DLLEXPORT void ref() const;

      //! returns the value of the member with an incremented reference count, or executes the memberGate() method and returns the value
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode *evalMember(const QoreString *member, class ExceptionSink *xsink);

      DLLLOCAL void instantiateLVar(lvh_t id);

      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL void uninstantiateLVar(class ExceptionSink *xsink);

      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL void merge(const class QoreHashNode *h, class ExceptionSink *xsink);
      DLLLOCAL class KeyNode *getReferencedPrivateDataNode(int key);

      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractPrivateData *getAndClearPrivateData(int key, class ExceptionSink *xsink);

      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode *evalBuiltinMethodWithPrivateData(class BuiltinMethod *meth, const class QoreListNode *args, class ExceptionSink *xsink);
      // called on old to acquire private data, copy method called on self (new copy)
      DLLLOCAL void evalCopyMethodWithPrivateData(class BuiltinMethod *meth, class QoreObject *self, const char *class_name, class ExceptionSink *xsink);
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL void addPrivateDataToString(class QoreString *str, class ExceptionSink *xsink) const;

      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL void obliterate(class ExceptionSink *xsink);

      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL void doDelete(class ExceptionSink *xsink);

      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL void defaultSystemDestructor(int classID, class ExceptionSink *xsink);

      //! returns the pointer to the value of the member
      /** if the member exists, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  an exception will be thrown if the character encoding conversion fails
	  also if the object has a deleted status an exception will be thrown
	  NOTE: the value returned is not referenced by this function, but rather the object is locked
	  @param key the name of the member
	  @param vl the AutoVLock container for nested locking
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode *getMemberValueNoMethod(const class QoreString *key, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns the pointer to the value of the member
      /**
	  NOTE: the value returned is not referenced by this function, but rather the object is locked
	  @param key the name of the member, assumed to be in the default encoding (QCS_DEFAULT)
	  @param vl the AutoVLock container for nested locking
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode *getMemberValueNoMethod(const char *key, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns a pointer to a pointer to the value of the member, so it can be set externally
      /** if no exception occurs, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  an exception will be thrown if the character encoding conversion fails
	  also if the object has a deleted status an exception will be thrown
	  @param key the name of the member
	  @param vl the AutoVLock container for nested locking
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode **getMemberValuePtr(const class QoreString *key, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns a pointer to a pointer to the value of the member, so it can be set or changed externally
      /** if no exception occurs, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  if the object has a deleted status an exception will be thrown
	  @param key the name of the member, assumed to be in the default encoding (QCS_DEFAULT)
	  @param vl the AutoVLock container for nested locking
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode **getMemberValuePtr(const char *key, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns a pointer to a pointer to the value of the member only if it already exists, so it can be set externally
      /** if no exception occurs, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  an exception will be thrown if the character encoding conversion fails
	  also if the object has a deleted status an exception will be thrown
	  @param mem the name of the member
	  @param vl the AutoVLock container for nested locking
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode **getExistingValuePtr(const class QoreString *mem, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns a pointer to a pointer to the value of the member only if it already exists
      /** if no exception occurs, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  if the object has a deleted status an exception will be thrown
	  @param mem the name of the member, assumed to be in the default encoding (QCS_DEFAULT)
	  @param vl the AutoVLock container for nested locking
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode **getExistingValuePtr(const char *mem, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! creates the object with the initial data passed as "d", used by the copy constructor
      DLLLOCAL QoreObject(const QoreClass *oc, class QoreProgram *p, class QoreHashNode *d);
};

#endif
