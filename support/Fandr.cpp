//
// Created by Ben Griffin on 2019-01-30.
//

#include "Fandr.h"
#include "Convert.h"

namespace Support {

	//	multiple find/replace over basis.
	//• --------------------------------------------------------------------------
	bool fandr(string& basis, const vector<pair<string,string>>& delims,size_t level) {
		if(level < delims.size()) {
			auto& findStr=delims[level].first;
			auto& repStr =delims[level].second;
			size_t fSize = findStr.size();
			if ( fSize > 0 && (fSize <= basis.size())) {
				size_t start = 0;
				size_t spos = basis.find(findStr);
				while (spos < string::npos ) {
					string nBasis(basis,start,spos-start);
					fandr(nBasis,delims,level+1);
					nBasis.append(repStr);
					basis.replace(start,(spos+fSize)-start,nBasis);
					start+= nBasis.size();
					spos  = basis.find(findStr,start);
				}
				if(start < basis.size()) {
					string nBasis(basis,start,string::npos);
					if(fandr(nBasis,delims,level+1)) {
						basis.replace(start,string::npos,nBasis);
					}
				}
				return true;
			} else {
				fandr(basis,delims,level+1);
			}
		}
		return false;
	}

	//• --------------------------------------------------------------------------
	int fandr(string& data, const string toSearch, const string replaceStr) {
		int rep_count = 0;
		if(!data.empty()) {
			size_t dSize = data.size();
			size_t sSize = toSearch.size();
			size_t pos = data.find(toSearch);
			// Repeat till end is reached
			while (pos != std::string::npos) {
				rep_count++;
				pos = data.find(toSearch, pos + sSize);
			}
			if (rep_count > 0) {
				size_t rSize = replaceStr.size();
				data.reserve(dSize + (rep_count * (rSize - sSize)));
				pos = data.find(toSearch);
				while (pos != std::string::npos) {
					data.replace(pos, sSize, replaceStr);
					pos = data.find(toSearch, pos + rSize);
				}
			}
		}
//		int rep_count = 0;
//		size_t bsize = basis.size();
//		if ( ( source.size() >= bsize ) && ( bsize > 0 ) ) {
//			size_t rsize = replace.size();
//			size_t spos = source.find(basis);
//			while (  spos != string::npos ) {
//				source.replace(spos,bsize,replace); rep_count++;
//				spos = source.find(basis,spos+rsize);
//			}
//		}
		return rep_count;
	}
	//• --------------------------------------------------------------------------
	int fandr(string &t, const string s, size_t r) {
		string intval;
		tostring(intval,r);
		return fandr(t,s,intval);
	}
	//• --------------------------------------------------------------------------
	int fandr(string &t, const string s) {
		return fandr(t,s,"");
	}
	//• --------------------------------------------------------------------------
	bool position(string const searchtext,string const context,size_t& result) {
		bool retval = false;
		result=0;
		size_t i = context.find(searchtext);
		if ( i < string::npos ) {
			retval = true;
			string::const_iterator l1=context.begin()+i;
			string::const_iterator f1=context.begin();
			while(f1 < l1) {
				if ((*f1 & 0x80) == 0x00) { //1 byte code
					f1++; result++;
				} else if ((l1-f1 > 1) && (*f1 & 0xe0) == 0xc0 && (f1[1] & 0xc0) == 0x80) { // 2-byte
					f1 += 2; result++;
				} else if ((l1-f1 > 2) && (*f1 & 0xf0) == 0xe0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80) {
					f1 += 3; result++;
				} else if ((l1-f1 > 3) && (*f1 & 0xf8) == 0xf0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80 && (f1[3] & 0xc0) == 0x80) {
					f1 += 4; result++;
				} else {
					return false; // illegal utf-8
				}
			}
		}
		return retval;
	}
	//• --------------------------------------------------------------------------
	bool char_at(string const string_to_measure,size_t pos,string& result) {
		bool retval = true;
		result="";
		string::const_iterator f1=string_to_measure.begin();
		string::const_iterator l1=string_to_measure.end();
		while (f1 != l1 && pos > 0) {
			if ((*f1 & 0x80) == 0x00) {
				pos--;
				if (pos == 0) {
					result += *f1++;
				} else {
					f1++;
				}
			} else if ((l1-f1 > 1) && (*f1 & 0xe0) == 0xc0 && (f1[1] & 0xc0) == 0x80) {
				pos--;
				if (pos == 0) {
					result += *f1++;
					result += *f1++;
				} else {
					f1 += 2;
				}
			} else if ((l1-f1 > 2) && (*f1 & 0xf0) == 0xe0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80) {
				pos--;
				if (pos == 0) {
					result += *f1++;
					result += *f1++;
					result += *f1++;
				} else {
					f1 += 3;
				}
			} else if ((l1-f1 > 3) && (*f1 & 0xf8) == 0xf0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80 && (f1[3] & 0xc0) == 0x80) {
				pos--;
				if (pos == 0) {
					result += *f1++;
					result += *f1++;
					result += *f1++;
					result += *f1++;
				} else {
					f1 += 4;
				}
			} else {
				return false; // illegal utf-8
			}
		}
		return retval;
	}
	//• --------------------------------------------------------------------------
	bool length(string const string_to_measure,size_t& result) {
		bool retval = true;
		result = 0;
		string::const_iterator f1=string_to_measure.begin();
		string::const_iterator l1=string_to_measure.end();
		while (f1 != l1) {
			if ((*f1 & 0x80) == 0x00) {
				f1++;
				result++;
			} else if ((l1-f1 > 1) && (*f1 & 0xe0) == 0xc0 && (f1[1] & 0xc0) == 0x80) {
				f1 += 2;
				result++;
			} else if ((l1-f1 > 2) && (*f1 & 0xf0) == 0xe0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80) {
				f1 += 3;
				result++;
			} else if ((l1-f1 > 3) && (*f1 & 0xf8) == 0xf0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80 && (f1[3] & 0xc0) == 0x80) {
				f1 += 4;
				result++;
			} else {
				return false; // illegal utf-8
			}
		}
		return retval;
	}
	//• --------------------------------------------------------------------------
	bool left(string const string_to_cut,long numchars,string& result) {
		if (numchars < 0) { 			//We want the length of the string, minus p2 for our leftstring.
			long charcount = 0;
			string::const_iterator f1=string_to_cut.begin();
			string::const_iterator l1=string_to_cut.end();
			for (; f1 != l1; ) {
				if ((*f1 & 0x80) == 0x00) {
					f1++; charcount++;
				}
				else if ((l1-f1 > 1) && (*f1 & 0xe0) == 0xc0 && (f1[1] & 0xc0) == 0x80) {
					f1 += 2; charcount += 1;
				}
				else if ((l1-f1 > 2) && (*f1 & 0xf0) == 0xe0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80) {
					f1 += 3; charcount += 1;
				}
				else if ((l1-f1 > 3) && (*f1 & 0xf8) == 0xf0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80 && (f1[3] & 0xc0) == 0x80) {
					f1 += 4; charcount += 1;
				}
				else {
					return false;
				}
			}
			numchars = charcount + numchars;		//remember, numchars is a negative value here!
		}
		string::const_iterator f1=string_to_cut.begin();
		string::const_iterator l1=string_to_cut.end();
		while((f1 != l1) && numchars > 0) {
			if ((*f1 & 0x80) == 0x00) {
				f1++;
				numchars--;
			} else if ((l1-f1 > 1) &&(*f1 & 0xe0) == 0xc0 && (f1[1] & 0xc0) == 0x80) {
				f1 += 2; numchars--;
			} else if ((l1-f1 > 2) &&(*f1 & 0xf0) == 0xe0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80) {
				f1 += 3; numchars--;
			} else if ((l1-f1 > 3) &&(*f1 & 0xf8) == 0xf0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80 && (f1[3] & 0xc0) == 0x80) {
				f1 += 4; numchars--;
			} else {
				return false;
			}
		}
		result = std::string(string_to_cut.begin(),f1);
		return true;
	}
	//• --------------------------------------------------------------------------
	bool right(string const string_to_cut,long numchars,string& result) {
		if (numchars < 0) { 			//We want the length of the string, minus p2 for our leftstring.
			long charcount = 0;
			string::const_iterator f1=string_to_cut.begin();
			string::const_iterator l1=string_to_cut.end();
			for (; f1 != l1; ) {
				if ((*f1 & 0x80) == 0x00) {
					f1++; charcount++;
				}
				else if ((l1-f1 > 1) && (*f1 & 0xe0) == 0xc0 && (f1[1] & 0xc0) == 0x80) {
					f1 += 2; charcount += 1;
				}
				else if ((l1-f1 > 2) && (*f1 & 0xf0) == 0xe0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80) {
					f1 += 3; charcount += 1;
				}
				else if ((l1-f1 > 3) && (*f1 & 0xf8) == 0xf0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80 && (f1[3] & 0xc0) == 0x80) {
					f1 += 4; charcount += 1;
				}
				else {
					return false;
				}
			}
			numchars = charcount + numchars;		//remember, numchars is a negative value here!
		}
		long charcount = 0;
		long lcount = 0;
		string::const_iterator f1=string_to_cut.begin();
		string::const_iterator l1=string_to_cut.end();
		while ( f1 != l1 ) {
			if ((*f1 & 0x80) == 0x00) {
				f1++; charcount++;
			} else if ((l1-f1 > 1) &&(*f1 & 0xe0) == 0xc0 && (f1[1] & 0xc0) == 0x80) {
				f1 += 2; charcount++;
			} else if ((l1-f1 > 2) &&(*f1 & 0xf0) == 0xe0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80) {
				f1 += 3; charcount++;
			} else if ((l1-f1 > 3) &&(*f1 & 0xf8) == 0xf0 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80 && (f1[3] & 0xc0) == 0x80) {
				f1 += 4; charcount++;
			} else {
				return false;
			}
		}
		lcount = charcount-numchars;
		if (lcount < 0) lcount = 0;
		f1=string_to_cut.begin();
		while((f1 != l1) && lcount > 0) {
			if ((*f1 & 0x80) == 0x00) {
				f1++; lcount--;
			} else if ((l1-f1 > 1) && (*f1 & 0xe0) == 0xc0 && f1 + 1 != l1 && (f1[1] & 0xc0) == 0x80) {
				f1 += 2; lcount--;
			} else if ((l1-f1 > 2) && (*f1 & 0xf0) == 0xe0 && f1 + 1 != l1 && f1 + 2 != l1 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80) {
				f1 += 3; lcount--;
			} else if ((l1-f1 > 3) && (*f1 & 0xf8) == 0xf0 && f1 + 1 != l1 && f1 + 2 != l1 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80 && (f1[3] & 0xc0) == 0x80) {
				f1 += 4; lcount--;
			} else {
				return false;
			}
		}
		result = std::string(f1,l1);
		return true;
	}
}
