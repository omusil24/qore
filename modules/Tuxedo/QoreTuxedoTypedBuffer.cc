/*
  modules/Tuxedo/QoreTuxedoTypedBuffer.cc

  Tuxedo integration to QORE

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

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

#include <qore/common.h>
#include <qore/support.h>
#include <qore/Exception.h>
#include <atmi.h>

#include "QoreTuxedoTypedBuffer.h"
#include "handle_error.h"

//------------------------------------------------------------------------------
QoreTuxedoTypedBuffer::QoreTuxedoTypedBuffer()
: buffer(0), size(0)
{
}

//------------------------------------------------------------------------------
QoreTuxedoTypedBuffer::~QoreTuxedoTypedBuffer()
{
  clear();
}

//-----------------------------------------------------------------------------
void QoreTuxedoTypedBuffer::clear()
{
  if (buffer) {
    tpfree(buffer);
    buffer = 0;
  }
  if (size) size = 0;
}

// EOF

