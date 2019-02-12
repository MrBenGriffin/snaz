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

	class Timing {

		static timestamp nBasetime;    //used for timing nodes.
		static timestamp bBasetime;    //used for timing builds.
		static std::unordered_map<std::string, timestamp> userTimes;

	public:
		static timestamp timer_start();
		static timecount timer_diff(const timestamp &);
		static long long nanos(const timecount &);
		static long double seconds(const timecount &);
		static void startup();
		static void setTiming(char,std::string= "unknown");
		static void str(std::ostream&,timecount);
		static void getTiming(std::ostream&,char,std::string= "unknown");
		static void getTiming(Messages&,char,std::string= "unknown");
		static bool smaller(const timecount &, const timecount &);

	};
}


#endif //MACROTEXT_TIMING_H
