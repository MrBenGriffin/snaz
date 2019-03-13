//
// Created by Ben on 2019-02-12.
//

#include "Timing.h"

#include <iomanip>
#include <iostream>
#include <ctime>
#include <cmath>
#include <float.h>


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

	void Timing::get(ostream& ostr,char timer_c,const string name,units style) {
		timecount the_timer;
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
			default:
				return;
		}
		str(ostr,uname,the_timer,style);
	}

	void Timing::get(Messages& ostr,char timer_c,const string name,units style) {
		ostringstream repo;
		switch(timer_c) {
			case 'b': repo << setw(18) << "Build:"; break;
			case 'n': repo << setw(18) << "Node:"; break;
			case 'l': repo << setw(11) << name << " (lap):"; break;
			case 'c': repo << setw(17) << name << ":"; break;
		}
		get(repo,timer_c,name,style);
		ostr << Message(timing,repo.str());
	}

	void Timing::get(Messages& ostr,const string name,units style) {
		get(ostr,'c',name,style);
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

	void Timing::str(ostream& ostr,string name,timecount t,units style) {
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