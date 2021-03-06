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
	enum units { us, ms, ns, s, m, h, nat, all, none}; //µ not allowed.

	class Timing {
		Timing(); //hide singleton from being publicly instantiable

		std::unordered_map<std::string,units> unitMap;
		timestamp nBasetime;    //used for timing nodes.
		timestamp bBasetime;    //used for timing builds.
		timestamp lBasetime;    //used for loop timing.
		std::unordered_map<std::string, timestamp> userTimes;
		std::unordered_map<std::string, timestamp> lapTimes;
		bool _show;    //show timing in logs..

	public:
		static Timing& t();
		units unit(const string&,units=nat);
		timestamp timer_start();
		timecount timer_diff(const timestamp &);
		long long nanos(const timecount &);
		long double seconds(const timecount &);
		long double seconds(const timestamp &);
		void str(std::ostream&,timecount,units=nat);
		bool smaller(const timecount &, const timecount &);

		void set(char,std::string= "unknown");
		void set(std::string= "custom");
		void setShow(bool Show = true) { _show = Show; }
		bool show() { return _show; }
		timecount get(char,std::string= "unknown");
		void get(Messages&,std::string= "unknown");
		void use(Messages&,std::string= "unknown");
		void get(Messages&,char,std::string= "unknown",const Support::Purpose = timer);
		void wait(long double);

	};
}


#endif //MACROTEXT_TIMING_H
