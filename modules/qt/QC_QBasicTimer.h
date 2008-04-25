/*
 QC_QBasicTimer.h
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols
 
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

#ifndef _QORE_QT_QC_QBASICTIMER_H

#define _QORE_QT_QC_QBASICTIMER_H

#include <QBasicTimer>

DLLLOCAL extern qore_classid_t CID_QBASICTIMER;
DLLLOCAL extern class QoreClass *QC_QBasicTimer;

DLLLOCAL class QoreClass *initQBasicTimerClass();

class QoreQBasicTimer : public AbstractPrivateData, public QBasicTimer
{
   public:
      DLLLOCAL QoreQBasicTimer() : QBasicTimer()
      {
      }
};

#endif // _QORE_QT_QC_QBASICTIMER_H
