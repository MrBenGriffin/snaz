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
		Timing(); //hide singleton from being publicly instantiable

		std::unordered_map<std::string,units> unitMap;
		timestamp nBasetime;    //used for timing nodes.
		timestamp bBasetime;    //used for timing builds.
		timestamp lBasetime;    //used for loop timing.
		std::unordered_map<std::string, timestamp> userTimes;
		std::unordered_map<std::string, timestamp> lapTimes;

	public:
		static Timing& t();
		bool showTiming;    		//for profiling.
		units unit(const string&,units=nat);
		timestamp timer_start();
		timecount timer_diff(const timestamp &);
		long long nanos(const timecount &);
		long double seconds(const timecount &);
		void setTiming(char,std::string= "unknown");
		void str(std::ostream&,timecount,units=nat);
		void getTiming(std::ostream&,char,std::string= "unknown",units=nat);
		void getTiming(Messages&,char,std::string= "unknown",units=nat);
		bool smaller(const timecount &, const timecount &);

	};
}


#endif //MACROTEXT_TIMING_H
