#include "Internal.h"
#include "InternalInstance.h"
#include "support/Message.h"
#include "support/Encode.h"
#include "support/Crypto.h"

namespace mt {
	using namespace Support;

	void iBase64::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		// @iBase64({e[b]/d},{text})
		string way = my.parm(1);
		string basis = my.parm(2);
		size_t sz = way.size();
		if (sz > 0 && way[0] =='d') {
			base64decode(e,basis);
		} else {
			base64encode(basis,(sz > 1) && (way[1] == 'b'));
		}
		my.set(basis);
	}
	void iDecode::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		std::string basis =  my.parm(1);
		xmldecode(basis);
		my.logic(basis,2);
	}
	void iEncode::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		std::string basis =  my.parm(1);
		xmlencode(basis);
		my.logic(basis,2);
	}
	void iHex::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		std::string basis =  my.parm(1);
		tohex(basis);
		my.logic(basis,2);
	}
	void iUnHex::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		std::string basis =  my.parm(1);
		fromhex(e,basis);
		my.logic(basis,2);
	}
	void iUpper::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		std::string basis =  my.parm(1);
		toupper(basis);
		my.logic(basis,2);
	}
	void iLower::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		std::string basis =  my.parm(1);
		tolower(basis);
		my.logic(basis,2);
	}
	void iUrlEncode::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		std::string basis =  my.parm(1);
		urlencode(basis);
		my.logic(basis,2);
	}
	void iDigest::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
//		size_t my.count = calledParameters.size();
		std::string basis;
		if ( Digest::available(e) ) {
			Digest::digest format=Digest::md5;
			if ( my.count < 2) { //just the code, so we need to stick out the value.
				format=Digest::md5;
			} else {
				string fstr = my.parm(2);
				switch (fstr[0]) {
					case 'm': {
						if (fstr.length() > 2) {
							switch (fstr[2]) {
								case '2': format=Digest::md2; break;
								case '4': format=Digest::md4; break;
								case '5': format=Digest::md5; break;
								case 'c': format=Digest::mdc2; break;
							}
						}
					} break;
					case 's': {
						format= Digest::sha;
						if (fstr.length() > 3) {
							switch (fstr[3]) {
								case '1': format=Digest::sha1; break;
								case '2': format=Digest::sha256; break;
								case '3': format=Digest::sha384; break;
								case '5': format=Digest::sha512; break;
							}
						}
					}  break;
					case 'r': format=Digest::ripemd160; break;
				}
			}
			Digest::do_digest(format,basis);
			tohex(basis);
			my.set(basis);
		}

	}
}