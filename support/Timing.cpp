//
// Created by Ben on 2019-02-12.
//

#include "Timing.h"

#include <iomanip>
#include <iostream>
#include <ctime>

namespace Support {
	using namespace std;

	timestamp Timing::nBasetime;	//used for timing.
	timestamp Timing::bBasetime;	//used for timing.
	unordered_map<string,timestamp> Timing::userTimes;


	long long Timing::nanos(const timecount &ts) {
		return ts.count();
	}

	long double Timing::seconds(const timecount &ts) {
		return chrono::duration<long double, nano>(ts).count();
	}

	timestamp Timing::timer_start() {
		return chrono::high_resolution_clock::now();
	}

	timecount Timing::timer_diff(const timestamp &start_time) {
		return chrono::high_resolution_clock::now() - start_time;
	}

	bool Timing::smaller(const timecount &s, const timecount &l) {
		return s < l;
	}

	void Timing::getTiming(ostream& ostr,char timer_c,const string name) {
		timecount the_timer;
		timer_c = (char) tolower(timer_c);
		switch(timer_c) {
			case 'c': { //custom
				auto i = userTimes.find(name);
				if(i != userTimes.end()) {
					the_timer = timer_diff(i->second);
				} else {
					auto&& x = userTimes.insert({name,timer_start()});
					the_timer = timer_diff(x.first->second);
				}
			} break;
			case 'n':
				the_timer = timer_diff(nBasetime);
				break;
			case 'b':
				//build/process
				the_timer = timer_diff(bBasetime);
				break;
			default:
				return;
		}
		str(ostr,the_timer);
	}

	void Timing::getTiming(Messages& ostr,char timer_c,const string name) {
		ostringstream repo;
		getTiming(repo,timer_c,name);
		ostr << Message(timing,repo.str());
	}

	void Timing::setTiming(char kind,const string name) { //if bld, then build, else node.
		timestamp tmpbase = timer_start();
		kind = (char) tolower(kind);
		switch (kind) {
			case 'n': {
				nBasetime	=tmpbase;
			} break;
			case 'b': {
				bBasetime	=tmpbase;
			} break;
			case 'c': {
				auto i = userTimes.find(name);
				if(i != userTimes.end()) {
					i->second = tmpbase;
				} else {
					userTimes.insert({name,tmpbase});
				}
			} break;
		}
	}

	void Timing::startup() {
		bBasetime=timer_start();
		nBasetime=timer_start();
	}

	void Timing::str(ostream& ostr,timecount t) {
		ostr << setw(12) << fixed << setprecision(4L);
		if (t < 0ns) {
			t = 0ns - t;
			ostr << "-";
		}
		chrono::nanoseconds	ticks[5]={60s  ,1s   ,1ms  ,1us  ,1ns};
		string			  		times[5]={"m  ","s  ","ms ","µs ","ns "};
		size_t counter=0;
		for (counter=0; counter < 4 && t < ticks[counter]; counter++);
		long double b = chrono::duration<long double,nano>(t).count();
		long double d = chrono::duration<long double,nano>(ticks[counter]).count();
		long double x = b/d;
		ostr << x << times[counter];
	}


}