//
// Created by Ben on 2019-02-12.
//

#include "Timing.h"

#include <iomanip>
#include <iostream>
#include <ctime>
#include <cmath>
#include <float.h>
#include <thread>

namespace Support {
	using namespace std;


	Timing &Timing::t() {
		static Timing singleton;
		return singleton;
	}

	Timing::Timing() : _show(false) {
		unitMap = {
				{"µs",      us},
				{"ms",      ms},
				{"ns",      ns},
				{"s",       s},
				{"m",       m},
				{"h",       h},
				{"natural", nat},
				{"all",     all},
				{"none",    none}
		};
		bBasetime=timer_start();
		nBasetime=timer_start();
		lBasetime=bBasetime;
	}

	units Timing::unit(const string& base,units Default) {
		auto iUnit = Timing::unitMap.find(base);
		return iUnit == Timing::unitMap.end() ? Default : iUnit->second;
	}

	long long Timing::nanos(const timecount &ts) {
		return ts.count();
	}
	void Timing::wait(long double secs) {
		auto seconds = chrono::duration<long double>(secs);
		auto start = chrono::high_resolution_clock::now();
		std::this_thread::sleep_for(seconds);
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end-start;
		std::cout << "Waited " << elapsed.count() << " s\n";
	}

	long double Timing::seconds(const timecount &ts) {
		return chrono::duration<long double>(ts).count();
	}

	long double Timing::seconds(const timestamp &ts) {
		return chrono::duration<long double>(chrono::high_resolution_clock::now() - ts).count();
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

	timecount Timing::get(char timer_c,const string name) {
		timecount the_timer(0);
		string uname = name;
		timer_c = (char) tolower(timer_c);
		switch(timer_c) {
			case 'l': { //laptime
				auto timeNow = timer_start();
				auto i = lapTimes.find(name);
				if(i != lapTimes.end()) {
					the_timer = timer_diff(i->second);
					i->second = timeNow;
				} else {
					if(name.empty()) {
						the_timer = timer_diff(lBasetime);
						lBasetime = timeNow;
					} else {
						auto&& x = lapTimes.insert({name,timeNow});
						x = userTimes.insert({name,timeNow});
						the_timer = timer_diff(x.first->second);
					}
				}
			} break;
			case 'c': { //custom
				auto i = userTimes.find(name);
				if(i != userTimes.end()) {
					the_timer = timer_diff(i->second);
				} else {
					if(!name.empty()) {
						auto timeNow = timer_start();
						auto&& x = lapTimes.insert({name,timeNow});
						x = userTimes.insert({name,timeNow});
						the_timer = timer_diff(x.first->second);
					}
				}
			} break;
			case 'n':
				uname = "node";
				the_timer = timer_diff(nBasetime);
				break;
			case 'b':
				uname = "build";
				//build/process
				the_timer = timer_diff(bBasetime);
				break;
			default: break;
		}
		return the_timer;
	}

	void Timing::get(Messages& log,char timer_c,const string name,const Support::Purpose purpose) {
		ostringstream repo;
		Support::channel channel = custom;
		switch(timer_c) {
			case 'b': { channel = build; repo << "Build Time"; } break;
			case 'n': { channel = node; repo << "Node Time"; } break;
			case 'l': repo << name << " (lap)"; break;
			case 'c': repo << name; break;
		}
		log << Message(channel,purpose,repo.str(),Timing::seconds(get(timer_c,name)));
	}

	void Timing::get(Messages& log,const string name) {
		get(log,'c',name);
	}

	void Timing::use(Messages& log,const string name) {
		get(log,'c',name);
		userTimes.erase(name);
		lapTimes.erase(name);
	}

	void Timing::set(string name) {
		set('c',name);
	}

	void Timing::set(char kind,const string name) { //if bld, then build, else node.
		timestamp tmpbase = timer_start();
		kind = (char) tolower(kind);
		switch (kind) {
			case 'n': {
				nBasetime	=tmpbase;
			} break;
			case 'b': {
				bBasetime	=tmpbase;
			} break;
			case 'l': {
				lBasetime	=tmpbase;
			} break;
			case 'c': {
				auto i = userTimes.find(name);
				if(i != userTimes.end()) {
					i->second = tmpbase;
				} else {
					userTimes.insert({name,tmpbase});
				}
				i = lapTimes.find(name);
				if(i != lapTimes.end()) {
					i->second = tmpbase;
				} else {
					lapTimes.insert({name,tmpbase});
				}
			} break;
		}
	}

	void Timing::str(ostream& ostr,timecount t,units style) {
		ostr << fixed;
		chrono::nanoseconds	ticks[6]={3600s,   60s  ,1s   ,1ms  ,1us  ,1ns};
		string			  	times[6]={"h ", "m ","s ","ms","µs","ns"};
		size_t types=6;
		size_t counter=0;
//		enum units { µs, ms, ns, s, m, h, nat, all};
		long double b = chrono::duration<long double,nano>(t).count();
		if(style != all) {
			switch(style) {
				case nat: {
					for (counter=0; counter < types && t < ticks[counter]; counter++);
				} break;
				case ns: counter = 5; break;
				case us: counter = 4; break;
				case ms: counter = 3; break;
				case none:	//none uses seconds.
				case  s: counter = 2; break;
				case  m: counter = 1; break;
				case  h: counter = 0; break;
				case all: counter = 2; break;
			}
			long double d = chrono::duration<long double,nano>(ticks[counter]).count();
			long double x = b/d;
			if (t < 0ns) {
				t = 0ns - t;
				ostr << "-";
			}
			if(style == none) {
				ostr << setw(12) << setprecision(10L) << x;
			} else {
				ostr << setw(12) << setprecision(4L) << x << times[counter];
			}
		} else {
			long double fraction;
			long double integer;
//			for (counter=0; counter < types && t < ticks[counter]; counter++);
			for (counter=1; counter < types; counter++) { //skip hours
				long double d = chrono::duration<long double,nano>(ticks[counter]).count();
				fraction = modfl(b/d,&integer);
//				if(fraction == 0) break;
				ostr << setw(4) << setprecision(0L) << integer << times[counter];
				b = fraction * d;
			}
		}
	}


}