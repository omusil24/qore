/*
  modules/TIBCO/qore-rv.h

  TIBCO Rendezvous integration to QORE

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

#ifndef _QORE_TIBCO_QORE_RV_H

#define _QORE_TIBCO_QORE_RV_H

#include <qore/common.h>
#include <qore/support.h>
#include <qore/Exception.h>

#include <tibrv/tibrvcpp.h>

class Hash *tibrvmsg_to_hash(TibrvMsg *msg, class ExceptionSink *xsink);
void hash_to_tibrvmsg(TibrvMsg *msg, class Hash *hash, class ExceptionSink *xsink);

#endif
