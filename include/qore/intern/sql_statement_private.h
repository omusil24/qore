/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  sql_statement_private.h

  Qore Programming Language

  Copyright (C) 2006 - 2010 Qore Technologies, sro
  
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

#ifndef _QORE_SQL_STATEMENT_PRIVATE_H
#define _QORE_SQL_STATEMENT_PRIVATE_H

// private implementation of the SQLStatement class
struct sql_statement_private {
   // Datasource pointer
   Datasource *ds;
   // private data for statement handle/ptr
   void *data;
   // started transaction flag
   bool trans;

   DLLLOCAL sql_statement_private() : ds(0), data(0), trans(false) {
   }
}; 

#endif