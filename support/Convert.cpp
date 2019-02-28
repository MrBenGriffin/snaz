//
// Created by Ben Griffin on 2019-01-30.
//
#include <string>
#include <sstream>
#include <iomanip>

//for NAN
#include <cmath>
#include <sys/types.h>
#include <ctime>

#include "Convert.h"
namespace Support {
	//---------------------------------------------------------------------------
	void tabstrip(string& str) { //remove all tabs.
		string::size_type wsi = str.find_first_of('\t');
		while ( wsi != string::npos ) {
			string::size_type wso = str.find_first_not_of('\t',wsi);
			str.erase(wsi,wso-wsi);
			wsi = str.find_first_of('\t');
		}
	}
	//---------------------------------------------------------------------------
	void strip(string& str) { //remove all tabs, etc.
		char const* delims = "\t\r\n";
		string::size_type wsi = str.find_first_of(delims);
		while ( wsi != string::npos ) {
			string::size_type wso = str.find_first_not_of(delims,wsi);
			str.erase(wsi,wso-wsi);
			wsi = str.find_first_of(delims);
		}
	}
	//---------------------------------------------------------------------------
	void wsstrip(string& str) { //remove all spaces,tabs, etc.
		string delims = {' ',0x09,0x0d,0x0a,0};
		auto chars = delims.c_str();
		string::size_type wsi = str.find_first_of(chars);
		while ( wsi != string::npos ) {
			string::size_type wso = str.find_first_not_of(chars,wsi);
			str.erase(wsi,wso-wsi);
			wsi = str.find_first_of(chars);
		}
	}
	//---------------------------------------------------------------------------
	void trim(string& str) {
		char const* delims = " \t\r\n";
		string::size_type  notwhite = str.find_first_not_of(delims);
		str.erase(0,notwhite);
		notwhite = str.find_last_not_of(delims);
		str.erase(notwhite+1);
	}
	//---------------------------------------------------------------------------
	void wtrim(string& str) {
		char const* delims = "\t\r\n";
		string::size_type  notwhite = str.find_first_not_of(delims);
		str.erase(0,notwhite);
		notwhite = str.find_last_not_of(delims);
		str.erase(notwhite+1);
	}
	//---------------------------------------------------------------------------
	//this does fixedwidth ints, but overflowing ints will break the width.
	string tofixedstring(int w,int i) { // use a fixed width integer output - prefixed with 0 if needs be.
		ostringstream ost;
		ost.flags ( ios::right | ios::dec | ios::showbase | ios::fixed );
		ost.fill('0');
		ost << setw(w) << i;
		return ost.str();
	}
	//---------------------------------------------------------------------------
	string tostring(int i) {
		switch (i) {
			case 0: return "0";
			case 1: return "1";
			case 2: return "2";
			case 3: return "3";
			case 4: return "4";
			case 5: return "5";
			case 6: return "6";
			case 7: return "7";
			case 8: return "8";
			default: {
				ostringstream ost;
				ost << i;
				return ost.str();
			}
		}
	}

	string tostring(size_t i) {
		switch (i) {
			case 0: return "0";
			case 1: return "1";
			case 2: return "2";
			case 3: return "3";
			case 4: return "4";
			case 5: return "5";
			case 6: return "6";
			case 7: return "7";
			case 8: return "8";
			default: {
				ostringstream ost;
				ost << i;
				return ost.str();
			}
		}
	}

	//---------------------------------------------------------------------------
	string tostring(unsigned long long i) {
		switch (i) {
			case 0: return "0";
			case 1: return "1";
			case 2: return "2";
			case 3: return "3";
			case 4: return "4";
			case 5: return "5";
			case 6: return "6";
			case 7: return "7";
			case 8: return "8";
			default: {
				ostringstream ost;
				ost << i;
				return ost.str();
			}
		}
	}

	//---------------------------------------------------------------------------
	string tostring(long double answer,string extra) {
		char format = 'd';  //natural
		size_t dp = 12;		//default number of decimal points.
		if (!extra.empty() && !isdigit(extra[0])) {
			format = extra[0];
			extra.erase(0,1);
		}
		if (!extra.empty()) {
			dp = natural(extra);
		}
		if (isfinite(answer)) { //ie not a nan or infinite.
			ostringstream ans;
			switch(format) {
				case('i'): ans << static_cast<long long>(answer); break;
				case('f'): ans << fixed << setprecision(dp) << answer; break;
				case('x'): ans << std::hex << static_cast<long long>(answer); break;
				default: ans << std::fixed << setprecision(dp) <<  answer; break;
			}
			return ans.str();
		} else {
			if(isnan(answer)) {
				return "NAN";
			} else { // isinf()
				return "INF";
			}
		}
	}
	//---------------------------------------------------------------------------
	string tostring(double i,size_t precision) {
		ostringstream ost;
		ost << fixed << setprecision((int)precision) << i;
		string result = ost.str();
		if (result == "nan") result="NaN";
		return result;
	}
	//---------------------------------------------------------------------------
	void tostring(string& repository,int i) {
		ostringstream tmp;
		tmp << i;
		repository = tmp.str();
	}
	//---------------------------------------------------------------------------
	void tostring(string& repository,size_t i) {
		ostringstream tmp;
		tmp << i;
		repository = tmp.str();
	}

	//---------------------------------------------------------------------------
	pair<string,string> split(char splitter, const string& basis) {
		pair<string,string> result;
		split(splitter,basis,result);
		return result;
	}

	//---------------------------------------------------------------------------
	bool split(char splitter, const string& basis, pair<string,string>& result) {			   //Given a string, split it at the first given character (destroy the character)
		string::size_type pos = basis.find_first_of(splitter);
		if (pos == string::npos) {
			result.first = basis;
			return false;
		} else {
			result.first = basis.substr(0, pos);
			result.second = basis.substr(pos+1, string::npos);
			return true;
		}
	}
	//---------------------------------------------------------------------------
	bool rsplit(char splitter, const string& basis, pair<string,string>& result) {			   //Given a string, split it at the last given character (destroy the character)
		string::size_type pos = basis.find_last_of(splitter);
		if (pos == string::npos) {
			result.first = basis;
			return false;
		} else {
			result.first = basis.substr(0, pos);
			result.second = basis.substr(pos+1, string::npos);
			return true;
		}
	}
	// -------------------------------------------------------------------
	size_t natural(const string& s) {
		string::const_iterator in = s.begin();
		string::const_iterator out = s.end();
		size_t val = 0;
		while(in != out && isdigit(*in)) {
			val = val * 10 + *in - '0';
			in++;
		}
		return val;
	}
	// -------------------------------------------------------------------
	// this affects the iterator
	size_t natural(string::const_iterator& in) {
		size_t val = 0;
		while(isdigit(*in) ) {
			val = val * 10 + *in - '0';
			in++;
		}
		return val;
	}
	// -------------------------------------------------------------------
	pair<size_t,bool> znatural(const string& text,size_t& i) {
		bool isnumber = false;
		size_t val = 0;
		while(isdigit(text[i]) && (i < text.size()) ) {
			isnumber = true;
			val = val * 10 + text[i] - '0';
			i++;
		}
		return pair<size_t,bool>(val,isnumber);
	}

	// this affects the iterator
	pair<size_t,bool> znatural(string::const_iterator& in) {
		bool isnumber = false;
		size_t val = 0;
		while(isdigit(*in) ) {
			isnumber = true;
			val = val * 10 + *in - '0';
			in++;
		}
		return pair<size_t,bool>(val,isnumber);
	}

	pair<size_t,bool> znatural(string::const_iterator& in,string::const_iterator& end) {
		bool isnumber = false;
		size_t val = 0;
		while(isdigit(*in) && (in < end) ) {
			isnumber = true;
			val = val * 10 + *in - '0';
			in++;
		}
		return pair<size_t,bool>(val,isnumber);
	}

	pair<size_t,bool> znatural(const string& s) { //Given a string, returns a natural from any digits that it STARTS with.
		bool isnumber = false;
		string::const_iterator in = s.begin();
		string::const_iterator out = s.end();
		size_t val = 0;
		while(in != out && isdigit(*in)) {
			isnumber= true;
			val = val * 10 + *in - '0';
			in++;
		}
		return pair<size_t,bool>(val,isnumber);
	}


	pair<std::time_t,bool> time_t(const string& s) { //Given a string, returns a natural from any digits that it STARTS with.
		std::istringstream ss(s);
		std::time_t tt;
		ss >> tt;
		return pair<std::time_t,bool>(tt,ss.good());
	}
	//-----------------------------------------------------------------------------

	void tolist(vector<string>& list,string basis,const string& cutter) {
		if (!cutter.empty()) {
			while (!basis.empty() ) {
				string::size_type pos = basis.find(cutter);
				if (pos != string::npos) {
					list.push_back(basis.substr(0, pos)); //pos says 2
					basis = basis.substr(pos+cutter.size(),string::npos);
				} else {
					list.push_back(basis);
					basis.clear();
				}
			}
		}
	}
	//-----------------------------------------------------------------------------
	void tolist(vector<size_t>& ilist,const string &s) {
		ilist.clear();
		size_t i;
		char comma;
		istringstream ist(s);
		while (ist >> i) {
			ilist.push_back(i);
			ist >> comma;
		}
	}
	//-----------------------------------------------------------------------------
	void tolist(deque<size_t>& ilist,const string &s) {
		ilist.clear();
		size_t i;
		char comma;
		istringstream ist(s);
		while (ist >> i) {
			ilist.push_back(i);
			ist >> comma;
		}
	}
	//---------------------------------------------------------------------------
	string tostring(string::const_iterator& i,const char x) {
		string result;
		while (true) {
			if( *i != x && *i != 0 ) {
				result.push_back(*i);
			} else {
				break;
			}
			i++;
		}
		return result;
	}
	//---------------------------------------------------------------------------
	long integer(string::const_iterator& i) {
		bool minus = false;
		if (*i == '-') {
			minus = true; i++;
		} else if (*i == '+') {
			i++;
		}
		int val = 0;
		while (true) {
			if( *i >= '0' && *i <= '9' ) {
				val = val * 10 + ( *i - '0' );
			} else {
				break;
			}
			i++;
		}
		if (minus)
			return -val;
		return val;
	}
	// -------------------------------------------------------------------
	pair<long,bool> integer(const string& s) { //Given a string, returns a natural from any digits that it STARTS with.
		bool isnumber = false;
		long val = 0;
		if (!s.empty()) {
			istringstream ist(s);
			ist >> val;
			streampos c_counted =  ist.tellg();
			if (c_counted == -1 || ((size_t)c_counted == s.length())) {
				isnumber = true;
			}
		}
		return pair<long,bool>(val,isnumber);
	}
	//---------------------------------------------------------------------------
	double real(string::const_iterator& x) {
		string::const_iterator y(x); //Start position as copy constructor.
		string valids("0123456789-+.");
		while( valids.find(*x) != string::npos ) x++;
		if ( x == y ) return NAN; //sqrt(static_cast<double>(-1.0));
		double i;
		istringstream ist(string(y,x));
		ist >> i;
		return i;
	}
	//---------------------------------------------------------------------------
	double real(const string& s) {
		if(s == "INF") return +INFINITY;
		if(s == "NAN") return NAN;
		string::const_iterator i = s.begin();
		return real( i );
	}
	//---------------------------------------------------------------------------
	size_t hex(string::const_iterator& i,double& val) {
		string::const_iterator j(i); //Start position as copy constructor.
		val = 0;
		bool ended = false;
		while (!ended) {
			switch (*i++) {
				case '0': val = val * 16; break;
				case '1': val = val * 16 + 1; break;
				case '2': val = val * 16 + 2; break;
				case '3': val = val * 16 + 3; break;
				case '4': val = val * 16 + 4; break;
				case '5': val = val * 16 + 5; break;
				case '6': val = val * 16 + 6; break;
				case '7': val = val * 16 + 7; break;
				case '8': val = val * 16 + 8; break;
				case '9': val = val * 16 + 9; break;
				case 'a': val = val * 16 + 10; break;
				case 'b': val = val * 16 + 11; break;
				case 'c': val = val * 16 + 12; break;
				case 'd': val = val * 16 + 13; break;
				case 'e': val = val * 16 + 14; break;
				case 'f': val = val * 16 + 15; break;
				case 'A': val = val * 16 + 10; break;
				case 'B': val = val * 16 + 11; break;
				case 'C': val = val * 16 + 12; break;
				case 'D': val = val * 16 + 13; break;
				case 'E': val = val * 16 + 14; break;
				case 'F': val = val * 16 + 15; break;
				default: ended=true; i--; break;
			}
		}
		if ( i == j ) val = NAN; //sqrt(static_cast<double>(-1.0)); //Illegal - not even 1 hexdigit
		return (int)(i - j);
	}
	// -------------------------------------------------------------------
	void todigits(string& s) {
		if (!s.empty()) {
			for (string::iterator i = s.begin(); i < s.end(); i++) {
				while (i != s.end() && ! isdigit(*i) ) {
					s.erase(i);
				}
			}
		}
	}
	//---------------------------------------------------------------------------
	void tolower(string& s) {
		for (char &i : s)
			i = std::tolower(i);
	}

	void toupper(string& s) {
		for (char &i : s)
			i = std::toupper(i);
	}

	//---------------------------------------------------------------------------
	bool isint(const string& s) {
		if (!s.empty()) { if (s.find_first_of("+-0123465789") == 0) return true; } return false;
	}

	//---------------------------------------------------------------------------
	bool isfloat(const string &s) {
		float i;
		istringstream ist(s);
		ist >> i;
		return(!ist.fail());
	}

	//---------------------------------------------------------------------------
	bool isdouble(const string &s) {
		double i;
		istringstream ist(s);
		ist >> i;
		return(!ist.fail());
	}
	//---------------------------------------------------------------------------
	std::string fixUriSpaces(const std::string& uriString) {
		std::string s(uriString);
		if(s.empty()) return s;
		size_t pos = 0;
		while(pos < s.size() && isspace(s[pos])) ++pos;
		s.erase(0, pos);
		if(s.empty()) return s;
		pos = s.size();
		while(isspace(s[pos-1]) && pos > 0) --pos;
		if(pos != s.size()) s.erase(pos);
		pos = 0;
		while((pos = s.find(' ', pos)) != std::string::npos) {
			s.replace(pos, 1, "%20");
			pos += 2;
		}
		return s;
	}

//---------------------------------------------------------------------------
}