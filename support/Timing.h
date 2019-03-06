//
// Created by Ben on 2019-02-12.
//

#ifndef MACROTEXT_TIMING_H
#define MACROTEXT_TIMING_H


#include <ctime>
#include <string>
#include <iostream>
#include <unordered_map>
#include <chrono>

#include "Message.h"

namespace Support {
	using timestamp = std::chrono::high_resolution_clock::time_point;
	using timecount = std::chrono::nanoseconds;
	enum units { µs, ms, ns, s, m, h, nat, all, none};

	class Timing {
		static bool _show;    //show timing in logs..
		static timestamp nBasetime;    //used for timing nodes.
		static timestamp bBasetime;    //used for timing builds.
		static timestamp lBasetime;    //used for loop timing.
		static std::unordered_map<std::string, timestamp> userTimes;
		static std::unordered_map<std::string, timestamp> lapTimes;

	public:
		static const std::unordered_map<std::string,Support::units> unitMap;

		static units unit(const string&,units=nat);
		static timestamp timer_start();
		static timecount timer_diff(const timestamp &);
		static long long nanos(const timecount &);
		static long double seconds(const timecount &);
		static void startup();
		static void set(char,std::string= "unknown");
		static void setShow(bool Show = true) { _show = Show; }
		static bool show() { return _show; }
		static void str(std::ostream&,timecount,units=nat);
		static void get(std::ostream&,char,std::string= "unknown",units=nat);
		static void get(Messages&,char,std::string= "unknown",units=nat);
		static bool smaller(const timecount &, const timecount &);

	};
}


#endif //MACROTEXT_TIMING_H
