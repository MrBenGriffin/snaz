//
// Created by Ben on 2019-02-14.
//

//#include <sys/times.h>
//#include <termios.h>
//#include <unistd.h>
#include <cmath>


#include "Date.h"
#include <cerrno>

namespace Support {

	bool Date::operator<(const Date& that) const {
		return tt < that.tt;
	}

	Date::Date() : local(nullptr),utc(nullptr),tt(0) {
		tt = time(&tt);
		local = localtime(&tt);
		utc = gmtime(&tt);
	}

	Date::Date(time_t ttime) : tt(ttime), local(nullptr),utc(nullptr) {
		utc = gmtime(&tt);
		local = localtime(&tt);
	}
	void Date::set(::time_t time) {
		tt = time;
		utc = gmtime(&tt);
		local = localtime(&tt);
	}

	void Date::getDateStr(struct tm* ttime, const std::string& format, std::string& cont)  {
		auto * buff = (char *)malloc (256);
		if (format.empty())
			strftime(buff, 255, "%Y-%m-%d %H:%M:%S", ttime);
		else
			strftime(buff, 255, format.c_str(), ttime);
		cont.append(buff);
		free(buff); // clear it out now
	}

	void Date::getDateStr(struct tm* ttime, std::string &cont)  {
		auto * buff = (char *)malloc (256);
		strftime(buff, 255, "%Y-%m-%d %H:%M:%S", ttime);
		cont.append(buff);
		free(buff); // clear it out now
	}

	void Date::getUTCDateStr(const std::string &format, std::string &cont) {
		getDateStr(utc, format, cont);
	}

	void Date::getLocalDateStr(const std::string &format, std::string &cont) {
		getDateStr(local, format, cont);
	}

	void Date::normalise(struct tm*& t) {
		//normalise seconds
		if ( t->tm_sec < 0 ) {
			t->tm_min = static_cast<int>(t->tm_min + ceil(t->tm_sec / 60.0) - 1);
			t->tm_sec = 60 + (static_cast<int>(t->tm_sec) % 60);
		} else if ( t->tm_sec > 59 ) {
			t->tm_min = static_cast<int>(t->tm_min + floor(t->tm_sec / 60.0));
			t->tm_sec = static_cast<int>(t->tm_sec) % 60;
		}
		//normalise minutes
		if ( t->tm_min < 0 ) {
			t->tm_hour = static_cast<int>(t->tm_hour + ceil(t->tm_min / 60.0) - 1);
			t->tm_min = 60 + (static_cast<int>(t->tm_min) % 60);
		} else if ( t->tm_min > 59 ) {
			t->tm_hour =static_cast<int>(t->tm_hour + floor(t->tm_min / 60.0));
			t->tm_min = static_cast<int>(t->tm_min) % 60;
		}
		//normalise hours
		if ( t->tm_hour < 0 ) {
			t->tm_mday = static_cast<int>(t->tm_mday + ceil(t->tm_hour / 24.0) - 1);
			t->tm_hour = 24 + (static_cast<int>(t->tm_hour) % 24);
		} else if ( t->tm_hour > 23 ) {
			t->tm_mday = static_cast<int>(t->tm_mday + floor(t->tm_hour / 24.0));
			t->tm_hour = static_cast<int>(t->tm_hour) % 24;
		}
		//now normalise months (as much as poss) // rmonth should be 0 .. 11
		if ( t->tm_mon < 0 ) {
			t->tm_year = static_cast<int>(t->tm_year + ceil(t->tm_mon / 12.0) - 1);
			t->tm_mon = 12 + (static_cast<int>(t->tm_mon) % 12);
		} else if ( t->tm_mon > 11 ) {
			t->tm_year = static_cast<int>(t->tm_year + floor(t->tm_mon / 12.0));
			t->tm_mon = static_cast<int>(t->tm_mon) % 12;
		}
		timegm (t);	//something should be here now. ( timegm is mktime without TZ conversion)
	}
	void Date::setYearOffset(const int yo) {
		local->tm_year += yo;
		utc->tm_year   += yo;
	}
	void Date::setSecOffset(const int so) {
		local->tm_sec += so;
		utc->tm_sec   += so;
		normalise(local); normalise(utc);
	}
	void Date::setMinOffset(const int mo) {
		local->tm_min += mo;
		utc->tm_min   += mo;
		normalise(local); normalise(utc);
	}
	void Date::setHourOffset(const int ho) {
		local->tm_hour += ho;
		utc->tm_hour   += ho;
		normalise(local); normalise(utc);
	}

	const time_t Date::getUTC() const {
		return tt;
	}
	void Date::getNow(std::string& cont) {
		tt = time(&tt);
		local = localtime(&tt);
		getDateStr(local,cont);
	}

	std::string Date::str() {
		std::string value;
		local = localtime(&tt);
		if(local == nullptr) {
			return "9999-12-31 01:00:00";
		} else {
			if (tt == 0) {
				local->tm_year = 0;
			}
		}
		getDateStr(local,value);
		return value;
	}

	void Date::getNowDateStr(const std::string &format, std::string &cont) {
		tt = time(&tt);
		local = localtime(&tt);
		getDateStr(local, format, cont);
	}

}