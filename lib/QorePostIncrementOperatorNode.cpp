/*
 QorePostIncrementOperatorNode.cpp
 
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

QoreString QorePostIncrementOperatorNode::op_str("++ (post-increment) operator expression");

AbstractQoreNode *QorePostIncrementOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   // change to pre increment if return value is ignored because it's more efficient
   if (!ref_rv) {
      SimpleRefHolder<QorePostIncrementOperatorNode> del(this);

      QorePreIncrementOperatorNode *rv = new QorePreIncrementOperatorNode(exp);
      exp = 0;
      return rv->parseInitImpl(oflag, pflag, lvids, typeInfo);
   }

   parseInitIntern(op_str.getBuffer(), oflag, pflag, lvids, typeInfo);

   // version for local var
   return makeSpecialization<QoreIntPostIncrementOperatorNode>();
}

AbstractQoreNode *QorePostIncrementOperatorNode::evalImpl(ExceptionSink *xsink) const {
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper n(exp, xsink);
   if (!n)
      return 0;

   // reference for return value is reference for variable assignment (if not null)
   ReferenceHolder<AbstractQoreNode> rv(n.take_value(), xsink);

   // acquire new value
   QoreBigIntNode *b = new QoreBigIntNode(!is_nothing(*rv) ? rv->getAsBigInt() : 0);
   if (n.assign(b))
      return 0;

   // increment value
   ++b->val;

   // return original value (may be null or non-integer)
   return rv.release();
}

AbstractQoreNode *QorePostIncrementOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return QorePostIncrementOperatorNode::evalImpl(xsink);
}