/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreLogicalGreaterThanOrEqualsOperatorNode.h
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2015 David Nichols
 
  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_QORELOGICALGREATERTHANOREQUALSOPERATORNODE_H

#define _QORE_QORELOGICALGREATERTHANOREQUALSOPERATORNODE_H

class QoreLogicalGreaterThanOrEqualsOperatorNode : public QoreLogicalLessThanOperatorNode {
OP_COMMON
protected:
   DLLLOCAL virtual QoreValue evalValueImpl(bool &needs_deref, ExceptionSink *xsink) const {
      return !QoreLogicalLessThanOperatorNode::evalValueImpl(needs_deref, xsink).getAsBool();
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
