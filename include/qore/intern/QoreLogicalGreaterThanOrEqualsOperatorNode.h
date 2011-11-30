/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreLogicalGreaterThanOrEqualsOperatorNode.h
 
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

#ifndef _QORE_QORELOGICALGREATERTHANOREQUALSOPERATORNODE_H

#define _QORE_QORELOGICALGREATERTHANOREQUALSOPERATORNODE_H

class QoreLogicalGreaterThanOrEqualsOperatorNode : public QoreLogicalLessThanOperatorNode {
OP_COMMON
protected:
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const {
      bool rc = QoreLogicalGreaterThanOrEqualsOperatorNode::boolEvalImpl(xsink);
      return *xsink ? 0 : get_bool_node(rc);
   }

   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
      needs_deref = false;
      return QoreLogicalGreaterThanOrEqualsOperatorNode::evalImpl(xsink);
   }

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const {
      return QoreLogicalGreaterThanOrEqualsOperatorNode::boolEvalImpl(xsink);
   }
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const {
      return QoreLogicalGreaterThanOrEqualsOperatorNode::boolEvalImpl(xsink);
   }
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const {
      return QoreLogicalGreaterThanOrEqualsOperatorNode::boolEvalImpl(xsink);
   }

   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const {
      return !QoreLogicalLessThanOperatorNode::boolEvalImpl(xsink);
   }

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      AbstractQoreNode *rv = QoreLogicalLessThanOperatorNode::parseInitIntern(op_str.getBuffer(), oflag, pflag, lvids, typeInfo);
      // make sure to reverse sense of comparison if this expression was resolved to a constant boolean value
      if (rv != this)
         return rv->getAsBool() ? (AbstractQoreNode*)&False : (AbstractQoreNode*)&True;
      return rv;
   }

public:
   DLLLOCAL QoreLogicalGreaterThanOrEqualsOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreLogicalLessThanOperatorNode(n_left, n_right) {
   }
};

#endif