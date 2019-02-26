//
// Created by Ben on 2019-02-14.
//

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <clocale>
#if __has_include(<filesystem>)
#include <filesystem>
#else
#include <unistd.h>
#endif
#include "Env.h"
#include "Fandr.h"
#include "Infix.h"
#include "Convert.h"
#include "Timing.h"
#include "Message.h"
#include "mt/Definition.h"

namespace Support {
	using namespace std;

	Env &Env::e() {
		static Env singleton;
		return singleton;
	}

	Env::Env() {
		MayBuild	= false;
		IsFinal 	= true;
		FullBuild 	= true;
		AllTechs 	= true;
		AllLangs 	= true;
		if (!get("RS_PATH",SiteRootDir))  {
			SiteRootDir=wd();
		}
		if (SiteRootDir.back() != '/') {
			SiteRootDir.push_back('/');
		}
		RemoteUser	= "AnonymousUser";
		NodeSet 	= Branch;

	}

	string Env::wd() {
		char buff[PATH_MAX];
		getcwd( buff, PATH_MAX );
		std::string cwd( buff );
		return cwd;
	}

	Path Env::basedir(buildspace space) {
		string directory;
		basedir(directory,space,true,true);
		return Path(directory);
	}

	void Env::basedir(string& base,buildspace space,bool addslash,bool full) {	//returns e.g.  /home/web/site/collins/buildlog/BUILDxxxx/ OR /home/web/site/collins/approve/
		string dirbit,extrabit;
		if(Testing) {
			dirbit = TestsDir;
		} else {
			if ( IsFinal ) {
				dirbit = FinalDir;
			} else {
				dirbit = DraftDir;
			}
		}
		switch (space) {
			case Build:		break;
			case Media:		dirbit.append("/media"); break;
			case Temporary:	{
//				dirbit = LogsDir + "_" + dirbit;
//				dirbit.append("/");
//				dirbit.append(tmpdirname);
			} break;
			case Scripts: {
				base= ScriptsDir;
				full = false;
			} break;
			case Tests:	{
				dirbit = "tests";
			} break;
		}
		if (full) {
			base = SiteRootDir + dirbit;
		}
		if (addslash) base.append("/");
	}

	buildarea Env::area() {
		buildarea retval = Final;
		string req_method;
		if ( get("REQUEST_METHOD",req_method) ) {
			if (req_method == "CONSOLE") {
				retval = Console;
			} else {
				string srvroot;
				if (! get("DOCUMENT_ROOT",srvroot)) {
					string fpath;
					if ( get("PATH_TRANSLATED",fpath)) {
						string fpathd(fpath,0,fpath.find_last_of('/') );
						if ( fpathd.find(FinalDir) != string::npos ) {
							retval = Final;
						} else {
							if ( fpathd.find(DraftDir) != string::npos ) {
								retval = Draft;
							} else {
								if ( fpathd.find(ReleaseDir) != string::npos ) {
									retval = Release;
								} else {
									retval = Staging;
								}
							}
						}
					}
				} else {
					string rootdir = srvroot.substr(1+srvroot.find_last_of('/'));
					if (rootdir == FinalDir) {
						retval = Final;
					} else {
						if (rootdir == DraftDir) {
							retval = Draft;
						} else {
							if (rootdir == ReleaseDir) {
								retval = Release;
							}
						}
					}
				}
			}
		}
		return retval;
	}


	bool Env::get(string name,string& value,string Default) {
		auto discovery = storage.find(name);
		if(discovery.found) {
			value = discovery.result;
			return discovery.found;
		} else {
			char* tmp = std::getenv(name.c_str());
			if(tmp != nullptr) {
				value = tmp;
				storage.set(name,value);
				return true;
			} else {
				value = std::move(Default);
				return false;
			}
		}
	}

	void Env::startup(const int argc,const char **argv) {
		setlocale(LC_ALL, "en_UK.UTF-8");
		mt::Definition::startup(); // Set the internals.
		Messages::startup(area() != Console);
		Infix::Evaluate::startup();
		Timing::startup();
		vector<size_t>	nodelist;
		if (get("REMOTE_USER",RemoteUser)) {
			if (RemoteUser != "AnonymousUser" || area() == Console) MayBuild = true;
		}

//		bool ParseAdvanced;
//		bool ParseLegacy;
//		bool ParseOnly;
//		bool ForceDeleteLock;

		for(int i = 1; i < argc; i++) {
			string parameter(argv[i]);
			string::const_iterator argi = parameter.begin();
			if(*argi++ == '-') { // expect a '-'
				switch(*argi++) {
					case 'a':   //dynamically assessed classic/advanced parse
//--??				doParse = tostring(argi,' ');
						ParseAdvanced = false;
						IsFinal = false;
						break;
					case 'A':
//--??						showTiming = true;
						break;
					case 'b':
//--??							showTemplate = true;
						break;
					case 'B':
						NodeSet = Branch;
						tolist(nodelist,parameter.substr(2));
						break;
					case 'c':   //forced advanced parse switch.
//--??					doParse = String::tostring(argi,' '); (text to parse?)
						ParseAdvanced = true;
						IsFinal = false;
					case 'C':
//--??					showProfile = true; //NOW used to show min/max macro parameter warnings.
						break;
					case 'D': {
//						Messages::setdebug(true);
//						showMediaReqs = true;
//						showPaths = true;
//						showTemplate = true;
//						showQueries = true;
//						showProfile = true; //NOW used to show min/max macro parameter warnings.
//						showFiling = true;
//						showNodes = true;
//						showTrace = true;
//						showGetSet = true;
						Messages::setVerbosity(9);
					} break;
					case 'd':
						Testing = true;
						Messages::setVerbosity(0);
						break;
					case 'f':
//						showFiling = true;
						break;
					case 'F':
						NodeSet = Descendants;
						tolist(nodelist,parameter.substr(2));
						break;
					case 'I':
//						showMediaReqs = true;
						break;
					case 'L':
						LanguageID = natural(argi);
						LanguageCount = 1;
						break;
					case 'M': {
						string cssurl;
						get("RS_CSSFILE",cssurl);         //rebuild.css   rebuild.css
						fandr(cssurl,"[S]",parameter.substr(2));  //allow for a css switch. eg rebuild[S].css env and -MAdmin -> rebuildA.css
						get("RS_BUILDNODEPATH",EditNodeUrl,"/mortar/oedit.obyx?node=0"); //if none, keep default.
					} break;
					case 'n':
//						showNodes = true;
						break;
					case 'O':
						NodeSet = Singles;
						tolist(nodelist,parameter.substr(2));
						break;
					case 'p':
//						showPaths = true;
						break;
					case 'P':
						IsFinal = false;
						break;
					case 'Q':
//						showQueries = true;
						break;
					case 's':
//						showGetSet = true;
						break;
					case 't':
//						showTrace = true;
						Messages::setVerbosity(4);
						break;
					case 'T':
						TechnologyID = natural(argi);
						TechnologyCount = 1;
						break;
					case 'V':
						Messages::setVerbosity(natural(argi));
						break;
					case 'X':
						ForceDeleteLock = true;
						break;
					case 'x': //comment until next -
						break;
					case 'z':
//						keepTmp = true;
						break;
					default:
						break;
				}
			} else {
				//report arg didn't start with a -
				//			*Logger::log << warn << "Parameter (" << parameter << ") is unknown. Parameters should be prefixed with a '-'" << Log::end; //cannot use Logger here
			}
		}
//		TODO:: get the from id stuff..
//		for (unsigned long &i : nodelist) {
//			if (i != 0) { fromID.insert(fromID.begin(), i); }
//		}
		Messages::defer( NodeSet == Singles && (Messages::verbosity() < 2) );
	}

}
