//
// Created by Ben Griffin on 2019-01-30.
//

#include <sstream>
#include "support/Encode.h"
#include "support/Fandr.h"
#include "support/Message.h"

namespace Support {
	using namespace std;

	//rfc2045 -- here we include a CRLF after every 72 characters.
//---------------------------------------------------------------------------
	//rfc2045 -- here we include a CRLF after every 72 characters.
	//"All line breaks or other characters not found in Table 1 must be ignored by decoding software"
	//---------------------------------------------------------------------------
	bool base64decode(Messages& errs,string& basis) {
		// A correct string has a length that is multiple of four.
		// Every 4 encoded bytes corresponds to 3 decoded bytes, (or null terminated).
		string b64ch="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		bool warned = false;
		size_t len = basis.size();
		size_t pos = 0;
		int i = 0,j = 0;
		unsigned char encbuf[4], decbuf[3];
		std::string ret;
		while (len-- && basis[pos] != '=') {
			size_t ch = b64ch.find(basis[pos]);
			if(ch == string::npos) {
				if(!warned) {
					errs << Message(warn,"base64 decoding found illegal character.");
					warned = true;
				}
			} else {
				encbuf[i++] = basis[pos];
				pos++;
				if (i == 4) {
					for (i = 0; i < 4; i++) {
						encbuf[i] = b64ch.find(encbuf[i]);
					}
					decbuf[0] = (encbuf[0] << 2) + ((encbuf[1] & 0x30) >> 4);
					decbuf[1] = ((encbuf[1] & 0xf) << 4) + ((encbuf[2] & 0x3c) >> 2);
					decbuf[2] = ((encbuf[2] & 0x3) << 6) + encbuf[3];
					for (i = 0; (i < 3); i++) {
						ret.push_back(decbuf[i]);
					}
					i = 0;
				}
			}
		}
		if (i) {
			for (j = i; j <4; j++) {
				encbuf[j] = 0;
			}
			for (j = 0; j <4; j++) {
				encbuf[j] = b64ch.find(encbuf[j]);
			}
			decbuf[0] = (encbuf[0] << 2) + ((encbuf[1] & 0x30) >> 4);
			decbuf[1] = ((encbuf[1] & 0xf) << 4) + ((encbuf[2] & 0x3c) >> 2);
			decbuf[2] = ((encbuf[2] & 0x3) << 6) + encbuf[3];
			for (j = 0; (j < i - 1); j++) {
				ret += decbuf[j];
			}
		}
		basis=ret;
		return true;
	}

	//---------------------------------------------------------------------------
	bool base64encode(string& s,bool withlf) {
		const char x[65]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		unsigned char i[3];
		size_t line_time = 0;
		std::string source(s);
		s.clear();
		s.reserve( source.length() + (source.length() >> 1) );
		string::const_iterator c = source.begin();
		string::const_iterator n = source.end();
		while( c < n ) {
			size_t chars=3;
			i[0] = *c++;
			if ( c < n ) {
				i[1] = *c++;
				if ( c < n ) {
					i[2] = *c++;
				} else {
					i[2] = 0; chars=2;
				}
			} else {
				i[1] = 0; i[2] = 0; chars=1;
			}
			s.push_back( x[ i[0] >> 2 ] );
			s.push_back( x[ ( (i[0] & 0x03) << 4) | ( (i[1] & 0xf0) >> 4) ] );
			s.push_back( chars > 1 ? x[ ( (i[1] & 0x0f) << 2) | ( (i[2] & 0xc0) >> 6) ] : '=' );
			s.push_back( chars > 2 ? x[ i[2] & 0x3f ] : '=' );
			if (withlf) {
				line_time += 4;
				if ( line_time >= 72) {
					line_time = 0;
					s.push_back('\n');
				}
			}
		}
		return true;
	}

	//---------------------------------------------------------------------------
	void xmlencode(string& basis) {  //XML escape
		static const vector<pair<string,string>> conversions = {
				{"&" ,"&amp;" },
				{"<" ,"&lt;"  },
				{">" ,"&gt;"  },
				{"'" ,"&#39;" }, // &apos; MSIE doesn't know apos
				{"\"","&quot;"}
		};
		if (! basis.empty() ) {
			fandr(basis,conversions);
		}
	}

	//---------------------------------------------------------------------------
	void xmldecode(string& basis) {  //XML de-escape
		static const vector<pair<string,string>> conversions = {
				{"&amp;" ,"&" },
				{"&lt;"  ,"<" },
				{"&gt;"  ,">" },
				{"&#39;" ,"'" },
				{"&apos;","'" },
				{"&quot;","\""}
		};
		if (! basis.empty() ) {
			fandr(basis,conversions);
		}
	}

	//---------------------------------------------------------------------------
	void tolower(string& s) {
		for (char &i : s)
			i = std::tolower(i);
	}

	//---------------------------------------------------------------------------
	void toupper(string& s) {
		for (char &i : s)
			i = std::toupper(i);
	}

	//---------------------------------------------------------------------------
	void fileEncode(string& str) { //remove all tabs.
		tolower(str);
		vector<pair<string,string>> subs = {{"-",""},{" ","-"},{"_",""}};
		fandr(str,subs);
		string fileCh=":-abcdefghijklmnopqrstuvwxyz0123456789";
		auto chars = fileCh.c_str();
		string::size_type wsi = str.find_first_not_of(chars);
		while ( wsi != string::npos ) {
			string::size_type wso = str.find_first_of(chars,wsi);
			str.erase(wsi,wso-wsi);
			wsi = str.find_first_not_of(chars);
		}
	}
	//• --------------------------------------------------------------------------
	bool normalise(string& container) {
		bool retval = true;
		ostringstream result;
		string::const_iterator f1=container.begin();
		string::const_iterator l1=container.end();
		while (f1 != l1) {
			if ((*f1 & 0x80) == 0x00) {
				result.put(*f1++);
			} else if ((*f1 & 0xe0) == 0xc0 && f1 + 1 != l1 && (f1[1] & 0xc0) == 0x80) {
				result.put(*f1++);
				result.put(*f1++);
			} else if ((*f1 & 0xf0) == 0xe0 && f1 + 1 != l1 && f1 + 2 != l1 && (f1[1] & 0xc0) == 0x80 && (f1[2] & 0xc0) == 0x80) {
				result.put(*f1++);
				result.put(*f1++);
				result.put(*f1++);
			} else {
				f1++;
				retval = false; // illegal utf-8 found
			}
		}
		container = result.str();
		return retval;
	}

	//---------------------------------------------------------------------------
	//file
	//	[4]   	FileChar   ::=   	 Letter | '_' | Digit | '.' | '-' | CombiningChar | Extender
	//	[5]   	File	   ::=   	 (FileChar)*
	//---------------------------------------------------------------------------------
	bool fileencode(string& s) {  //xml name encoding returns false if it can't return a valid name.
		bool retval=true;
		std::string result;
		unsigned char c;
		string::size_type q=s.size();
		if ( q > 0) {
			for (string::size_type p = 0; p < q; p++) {  //carry p on from where it left off.
				c=s[p];
				if (
						(c >= 'A' && c <= 'Z') ||
						(c >= 'a' && c <= 'z') ||
						(c == '_' ) ||
						(c >= '0' && c <= '9') ||
						(c == '.' ) ||
						(c == '-' )
						) {
					result += c;
				}
			}
		} else {
			retval=false;
		}
		s = result;
		return retval;
	}
	//---------------------------------------------------------------------------
	string hexencode(const unsigned char c) {
		std::string result("%");
		unsigned char v[2];
		v[0] = c >> 4;
		v[1] = c & 0x0F;
		if (v[0] <= 9) result.push_back(v[0] + '0');
		else result.push_back((v[0] - 10) + 'A');
		if (v[1] <= 9) result.push_back(v[1] + '0');
		else result.push_back((v[1] - 10) + 'A');
		return result;
	}
	//---------------------------------------------------------------------------
	bool tohex(string& s) {
		const unsigned char Abase = 'A' - 10;
		bool success=true;
		std::string orig;
		orig.swap(s);
		string::size_type sz = orig.size();
		s.reserve(sz >> 1);
		for (string::size_type p= 0; p < sz; ++p) {
			unsigned char v[2];
			v[0] = orig[p] & 0x00F0;
			v[1] = orig[p] & 0x000F;
			if (v[0] <= 0x0090 ) {
				s.push_back( (v[0] >> 4) + '0');
			} else {
				s.push_back( (v[0] >> 4) + Abase );
			}
			if (v[1] <= 0x0009 ) {
				s.push_back(v[1] + '0');
			} else {
				s.push_back(v[1] + Abase );
			}
		}
		return success;
	}
	//---------------------------------------------------------------------------
	bool fromhex(Messages& errs,string& s) {
		const unsigned char Aubase = 'A' - 10;
		const unsigned char Albase = 'a' - 10;
		bool success=true;
		std::string orig;
		orig.swap(s);
		string::size_type sz = orig.size();
		s.reserve(sz << 1);
		for (string::size_type p= 0; p < sz; p += 2) {
			unsigned char hi = orig[p];
			unsigned char lo = p+1 < sz ? orig[p+1] : '0';
			if ( hi >= '0' && hi <= '9' ) {
				hi -= '0';
			} else {
				if ( hi >= 'A' && hi <= 'F' ) {
					hi -= Aubase;
				} else {
					if ( hi >= 'a' && hi <= 'f' ) {
						hi -= Albase;
					} else {
						hi = 0;
						success = false;
						errs << Message(warn,"hex decoding found illegal character.");
					}
				}
			}
			if ( lo >= '0' && lo <= '9' ) {
				lo -= '0';
			} else {
				if ( lo >= 'A' && lo <= 'F' ) {
					lo -= Aubase;
				} else {
					if ( lo >= 'a' && lo <= 'f' ) {
						lo -= Albase;
					} else {
						lo = 0;
						success = false;
						errs << Message(warn,"hex decoding found illegal character.");
					}
				}
			}
			unsigned char ch = hi << 4 | lo;
			s.push_back(ch);
		}
		return success;
	}
	//---------------------------------------------------------------------------
/*
 void urldecode(std::string& s) {
		std::string result;
		unsigned char c;
		for (size_t p= 0; p < s.size(); ++p) {
			c= s[p];
			if (c == '+') c= ' ';  // translate '+' to ' ':
								   // translate %XX:
			if ((c == '%') && (p + 2 < s.size())) {  // check length
				unsigned char v[2];
				for (size_t i= p + 1; i < p + 3; ++i)
					// check and translate syntax
					if ((s[i] >= '0') && (s[i] <= '9'))
						v[i - p - 1]= s[i] - '0';
					else if ((s[i] >= 'A') && (s[i] <= 'F'))
						v[i - p - 1]= 10 + s[i] - 'A';
					else if ((s[i] >= 'a') && (s[i] <= 'f'))
						v[i - p - 1]= 10 + s[i] - 'a';
					else v[i - p - 1]= 16;  // error
				if ((v[0] < 16) && (v[1] < 16)) {  // check ok
					c= (unsigned char) 16*v[0] + v[1];
					p+= 2;
				}
			}
			result+= c;
		}
		s = result;
	}
*/
//---------------------------------------------------------------------------
// as defined in RFC2068
/*
		void urlencode(std::string& s) {
			std::string result;
			unsigned char c;
			for (size_t p= 0; p < s.size(); ++p) {
				c= s[p];
				if ((c <= 31) || (c >= 127))  // CTL, >127
					result+= hexencode(c);
				else switch (c) {
					case ';':
					case '/':
					case '?':
					case ':':
					case '@':
					case '&':
					case '=':
					case '+':  // until here: reserved
					case '"':
					case '#':
					case '%':
					case '<':
					case '>':  // until here: unsafe
						result+= hexencode(c);
						break;
					case ' ':  // SP
						result+= '+';
						break;
					default:  // no need to encode
						result+= c;
						break;
				}
			}
		s = result;
	}
*/
	//---------------------------------------------------------------------------
	void urldecode(Messages&,std::string& s) {
		std::string result;
		unsigned char c;
		for (size_t p= 0; p < s.size(); ++p) {
			c= s[p];
			if (c == '+') c= ' ';  // translate '+' to ' ':
			// translate %XX:
			if ((c == '%') && (p + 2 < s.size())) {  // check length
				unsigned char v[2];
				for (size_t i= p + 1; i < p + 3; ++i)
					// check and translate syntax
					if ((s[i] >= '0') && (s[i] <= '9'))
						v[i - p - 1]= s[i] - '0';
					else if ((s[i] >= 'A') && (s[i] <= 'F'))
						v[i - p - 1]= 10 + s[i] - 'A';
					else if ((s[i] >= 'a') && (s[i] <= 'f'))
						v[i - p - 1]= 10 + s[i] - 'a';
					else v[i - p - 1]= 16;  // error
				if ((v[0] < 16) && (v[1] < 16)) {  // check ok
					c= (unsigned char) 16*v[0] + v[1];
					p+= 2;
				}
			}
			result+= c;
		}
		s = result;
	}

	//---------------------------------------------------------------------------
	//	5.1.  Parameter Encoding
	//	All parameter names and values are escaped using the [RFC3986] percent-encoding (%xx) mechanism.
	//	Characters not in the unreserved character set ([RFC3986] section 2.3) MUST be encoded.
	//  Characters in the unreserved character set MUST NOT be encoded.
	//	Hexadecimal characters in encodings MUST be upper case.
	//	Text names and values MUST be encoded as UTF-8 octets before percent-encoding them per [RFC3629].
	//	unreserved = ALPHA, DIGIT, '-', '.', '_', '~'
	//---------------------------------------------------------------------------
	// as defined in RFCs 1738 2396
	void urlencode(std::string& s) {
		std::string result;
		unsigned char c;
		for (unsigned char p : s) {
			c= p;
			if (
					((c >= 'a') && (c <= 'z')) ||
					((c >= '0') && (c <= '9')) ||
					((c >= 'A') && (c <= 'Z')) ||
					( c == '-' || c == '.' || c == '_' || c == '~' )
					) {  //reserved.
				result.push_back(c);
			} else { //unreserved.
				result.append(hexencode(c));
			}
		}
		s = result;
	}

}
