/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreTimeZoneManager.h

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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

#ifndef QORE_TIMEZONEMANAGER_H

#define QORE_TIMEZONEMANAGER_H

#include <string>
#include <vector>
#include <map>

#ifndef LOCALTIME_LOCATION
#define LOCALTIME_LOCATION "/etc/localtime"
#endif

#ifndef ZONEINFO_LOCATION
#define ZONEINFO_LOCATION "/usr/share/zoneinfo"
#endif

// transition info structure
struct QoreTransitionInfo {
   int         gmtoff;  // GMT offset in seconds east (negative for west)
   std::string abbr;    // time zone abbreviation (i.e. "EST")
   bool        isdst;   // is daylight standard time?
   bool        isstd;   // transition is standard time (true) or wall clock time (false)
   bool        isgmt;   // transition is UTC (true) or local time (false)
};

typedef std::vector<QoreTransitionInfo> trans_vec_t;

struct QoreLeapInfo {
   int ttime; // transition time
   int total; // total correction after transition time
};

struct QoreDSTTransition {
   int time;
   struct QoreTransitionInfo *trans;
};

class AbstractQoreTZInfo {
public:
   virtual DLLLOCAL ~AbstractQoreTZInfo() {
   }

   // returns general GMT offset for the time zone's standard time in minutes east
   DLLLOCAL virtual int getGMTOffset() const = 0;

   // returns the GMT offset for the given time given as seconds from the epoch (1970-01-01Z)
   DLLLOCAL virtual int getGMTOffset(int64 epoch_offset) = 0;

   // returns the GMT offset and local time zone name for the given time given as seconds from the epoch (1970-01-01Z)
   DLLLOCAL virtual int getGMTOffset(int64 epoch_offset, std::string &zone_name) = 0;

   // returns true if the zone has daylight savings time ever
   DLLLOCAL virtual bool hasDST() const = 0;
};

class QoreTZInfo : public AbstractQoreTZInfo {
protected:
   std::string name;
   // GMT offset in seconds east; -1 = unknown
   int gmtoff;

   bool valid;
   bool has_dst;

   // QoreDSTTransition times
   typedef std::vector<QoreDSTTransition> dst_transition_vec_t;
   dst_transition_vec_t QoreDSTTransitions;

   // QoreTransitionInfo array
   trans_vec_t tti;

   // leap info vector
   typedef std::vector<QoreLeapInfo> leap_vec_t;
   leap_vec_t leapinfo;

public:
   DLLLOCAL QoreTZInfo(QoreString &root, std::string &n_name, ExceptionSink *xsink);

   DLLLOCAL virtual ~QoreTZInfo() {
   }

   // returns GMT offset in seconds east
   DLLLOCAL virtual int getGMTOffset() const {
      return gmtoff == -1 ? 0 : gmtoff;
   }

   // returns the GMT offset for the given time given as seconds from the epoch (1970-01-01Z)
   DLLLOCAL virtual int getGMTOffset(int64 epoch_offset) {
      assert(false);
      return 0;
   }

   // returns the GMT offset and local time zone name for the given time given as seconds from the epoch (1970-01-01Z)
   DLLLOCAL virtual int getGMTOffset(int64 epoch_offset, std::string &zone_name) {
      assert(false);
      return 0;
   }

   // returns true if the zone has daylight savings time ever
   DLLLOCAL virtual bool hasDST() const {
      return has_dst;
   }

   DLLLOCAL operator bool() const {
      return valid;
   }

   DLLLOCAL const trans_vec_t &getTransitionList() const {
      return tti;
   }
};

class QoreTimeZoneManager {
protected:
   // time zone info map (ex: "Europe/Prague" -> QoreTZInfo*)
   typedef std::map<std::string, QoreTZInfo *> tzmap_t;

   unsigned tzsize;
   //unsigned tznearestsize;

   // our gmt offset in seconds east of GMT
   int our_gmtoffset;

   QoreString root;
   tzmap_t tzmap;

   // time zone code to gmtoffset - for nearest time zone code to our time zone as calculated by the GMT offset
   //typedef std::map<std::string, int> tz_nearest_map_t;
   //tz_nearest_map_t tznearest;

   // pointer to our regional time information
   QoreTZInfo *localtz;
   std::string localtzname;

   DLLLOCAL int process(const char *fn);

   DLLLOCAL int processDir(const char *d);

   // to set the local time zone information from a file 
   DLLLOCAL int setLocalTZ(std::string fname);

   DLLLOCAL void setFromLocalTimeFile();

   DLLLOCAL void init_intern(QoreString &TZ);

public:
   DLLLOCAL QoreTimeZoneManager();

   DLLLOCAL ~QoreTimeZoneManager() {
      for (tzmap_t::iterator i = tzmap.begin(), e = tzmap.end(); i != e; ++i)
	 delete i->second;
   }

   DLLLOCAL QoreTZInfo *getZone(const char *name) {
      tzmap_t::iterator i = tzmap.find(name);
      return i == tzmap.end() ? 0 : i->second;
   }

   DLLLOCAL int readAll(ExceptionSink *xsink);

   DLLLOCAL void init();

   DLLLOCAL const AbstractQoreTZInfo *getLocalTZInfo() const {
      return localtz;
   }

   DLLLOCAL const char *getLocalTZRegion() const {
      return localtzname.empty() ? 0 : localtzname.c_str();
   }
};

DLLLOCAL extern QoreTimeZoneManager QTZM;

#endif
