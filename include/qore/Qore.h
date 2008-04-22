/*
  Qore.h

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

#ifndef _QORE_QORE_H

#define _QORE_QORE_H

/** @file Qore.h
    the main header file for the Qore library.  All code using any part of the Qore library's functionality should include this file
*/

#include <qore/common.h>
#include <qore/QoreEncoding.h>
#include <qore/ReferenceHolder.h>
#include <qore/AbstractQoreNode.h>
#include <qore/QoreNodeEvalOptionalRefHolder.h>
#include <qore/QoreListNode.h>
#include <qore/QoreProgram.h>
#include <qore/ModuleManager.h>
#include <qore/QoreLib.h>
#include <qore/QoreStringNode.h>
#include <qore/DateTimeNode.h>
#include <qore/QoreHashNode.h>
#include <qore/QoreBigIntNode.h>
#include <qore/QoreBoolNode.h>
#include <qore/QoreFloatNode.h>
#include <qore/QoreNothingNode.h>
#include <qore/QoreNullNode.h>
#include <qore/QoreNet.h>
#include <qore/QoreURL.h>
#include <qore/QoreFile.h>
#include <qore/QoreSocket.h>
#include <qore/AbstractPrivateData.h>
#include <qore/QoreObject.h>
#include <qore/QoreNamespace.h>
#include <qore/ExceptionSink.h>
#include <qore/BinaryNode.h>
#include <qore/support.h>
#include <qore/QoreString.h>
#include <qore/DateTime.h>
#include <qore/QoreType.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/qore_thread.h>
#include <qore/QoreThreadLock.h>
#include <qore/QoreCondition.h>
#include <qore/DBI.h>
#include <qore/Datasource.h>
#include <qore/QoreClass.h>
#include <qore/ScopeGuard.h>
#include <qore/SystemEnvironment.h>
#include <qore/AutoVLock.h>
#include <qore/CallReferenceNode.h>
#include <qore/ReferenceNode.h>
#include <qore/params.h>
#include <qore/ReferenceHelper.h>

//! the complete version string of the qore library
DLLEXPORT extern char qore_version_string[];

//! the major version number of the qore library
DLLEXPORT extern int qore_version_major;

//! the minor version number of the qore library
DLLEXPORT extern int qore_version_minor;

//! the version number below the minor version number of the qore library
DLLEXPORT extern int qore_version_sub;

//! the build number of the qore library
DLLEXPORT extern int qore_build_number;

//! the build target machine word size in bits (32 or 64 normally)
DLLEXPORT extern int qore_target_bits;

//! the build target Operating System name
DLLEXPORT extern char qore_target_os[];

//! the build target machine architecture name
DLLEXPORT extern char qore_target_arch[];

#define QLO_NONE                    0       //!< no options (default)
#define QLO_DISABLE_SIGNAL_HANDLING 1 << 0  //!< disable qore signal handling entirely

//! initializes the Qore library
/** @param default_encoding the default character encoding for the library, if 0 then the environment variables QORE_CHARSET and LANG will be processed, in that order, to determine the default character encoding.  If no character encoding can be determined from either of these environment variables, UTF-8 will be used as the default.
    @param show_module_errors if true then any errors loading qore modules will be output to stdout
    @param init_options a binary "or" sum of the qore library options 
    @note This function can only be called once and must be called before any other qore facilities are used.
 */
DLLEXPORT void qore_init(char *default_encoding = 0, bool show_module_errors = false, int init_options = QLO_NONE);

//! frees all memory allocated by the library
/*
    @note This function can only be called once and should be called when a program using the Qore library terminates.
 */
DLLEXPORT void qore_cleanup();

// include private definitions if compiling the library
#ifdef _QORE_LIB_INTERN
#include <qore/intern/QoreLibIntern.h>
#endif

#endif  // _QORE_QORE_H
