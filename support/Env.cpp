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
#include "support/Env.h"
#include "support/Fandr.h"
#include "support/Infix.h"
#include "support/Convert.h"
#include "support/Timing.h"
#include "support/Message.h"
#include "mt/Definition.h"
#include "support/db/ServiceFactory.h"
#include "node/NodeLocator.h"
#include "Build.h"

namespace Support {
	using namespace std;

	Env &Env::e() {
		static Env singleton;
		return singleton;
	}

	Env::Env() {
//		MayBuild	= false;
//		FullBuild 	= true;
//		AllTechs 	= true;
//		AllLangs 	= true;
		if (!get("RS_PATH",SiteRootDir))  {
			SiteRootDir=wd();
		}
		if (SiteRootDir.back() != '/') {
			SiteRootDir.push_back('/');
		}
//		RemoteUser	= "AnonymousUser";
//		NodeSet 	= Branch;

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
		Build& build = Build::b();
		string dirbit,extrabit;
		switch (build.current()) {
			case Editorial: dirbit = "/tmp"; break;
			case Testing: 	dirbit = TestsDir; break;
			case Final:   	dirbit = FinalDir; break;
			case Draft:   	dirbit = DraftDir; break;
			case Release: 	dirbit = ReleaseDir; break;
			case Staging: 	dirbit = StagingDir; break;
			case Console: 	dirbit = "/tmp"; break;
		}
		switch (space) {
			case Built:		break;
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

	buildArea Env::area() {
		buildArea retval = Final;
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

		std::string Env::baseUrl(buildArea area) {
			std::string response;
			std::string url;
//		enum 	buildArea  {Editorial,Final,Draft,Console,Release,Staging};
			bool found = false;
			switch (area) {
				case Editorial:	 	found = get("RS_URLE",url); break;
				case Draft:	 		found = get("RS_URLD",url); break;
				case Final:	 		found = get("RS_URLP",url); break;
				default:;
			}
			if(found) {
				response = "https://" + url ;
			} else {
				basedir(url,Built,false,true);
				response = "file:" + url ;
			}
		return response;
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

	void Env::doArgs(Messages& log,const int argc,const char **argv) {
		Timing& timer = Timing::t(); //auto startup at instantiation.
		Build& build = Build::b();

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
						build.setCurrent(Draft);
						break;
					case 'A':
						timer.setShow(true);
						break;
					case 'b':
						Node::setShowTemplates(true);
						break;
					case 'B': {
						deque<size_t> nodes;
						tolist(nodes, parameter.substr(2));
						build.setNodes(Build::Branch,nodes);
					} break;
					case 'c':   //forced advanced parse switch.
//--??					doParse = String::tostring(argi,' '); (text to parse?)
						ParseAdvanced = true;
						build.setCurrent(Draft);
					case 'C':
//--??					showProfile = true; //NOW used to show min/max macro parameter warnings.
						break;
					case 'D': {
//						Messages::setdebug(true);
//						showMediaReqs = true;
						NodeLocator::showPaths = true;
						Node::setShowTemplates(true);
//						showQueries = true;
//						showProfile = true; //NOW used to show min/max macro parameter warnings.
//						showFiling = true;
//						showNodes = true;
//						showTrace = true;
//						showGetSet = true;
						Messages::setVerbosity(9);
					}
						break;
					case 'd':
						build.setCurrent(Testing); // = true;
						Messages::setVerbosity(0);
						break;
					case 'f':
//						showFiling = true;
						break;
					case 'F':{
						deque<size_t> nodes;
						tolist(nodes, parameter.substr(2)); //setnodes
						build.setNodes(Build::Descendants,nodes);
				} break;
					case 'I':
//						showMediaReqs = true;
						break;
					case 'L': {
						deque<size_t> languages;
						tolist(languages, parameter.substr(2));
						build.setLangs(languages);
					}
						break;
					case 'M': {
						string cssurl;
						get("RS_CSSFILE", cssurl);         //rebuild.css   rebuild.css
						fandr(cssurl, "[S]", parameter.substr(
								2));  //allow for a css switch. eg rebuild[S].css env and -MAdmin -> rebuildA.css
						get("RS_BUILDNODEPATH", EditNodeUrl, "/mortar/oedit.obyx?node=0"); //if none, keep default.
					}
						break;
					case 'n':
//						showNodes = true;
						break;
					case 'O': {
						deque<size_t> nodes;
						tolist(nodes, parameter.substr(2));
						build.setNodes(Build::Singles,nodes);
					} break;
					case 'p':
						NodeLocator::showPaths = true;
						break;
					case 'P':
						build.setCurrent(Draft);
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
					case 'T': {
						deque<size_t> techs;
						tolist(techs,parameter.substr(2));
						build.setTechs(techs);
					} break;
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
						log << Message(warn,"Parameter (" + parameter + ") is unknown.");
						break;
				}
			} else {
				log << Message(warn,"Parameter (" + parameter + ") is unknown. Parameters should be prefixed with a '-'");
			}
		}
	}

	pair<Messages&,Db::Connection*> Env::startup(int argc,const char **argv) {
		setlocale(LC_ALL, "en_UK.UTF-8");
		Db::Connection *mysql = nullptr;
		mt::Definition::startup(); // Set the internals.
		Infix::Evaluate::startup();
		Messages::setMarkup(area() != Console);
		Messages log;
		doArgs(log,argc,argv);
		string config_path;
		if(get("SQL_CONFIG_FILE",config_path)) {
			mysql = Db::ServiceFactory::sf().getConnection(log,"mysql",config_path);
		} else {
			log << Message(fatal,"The environment variable `SQL_CONFIG_FILE` must be set.");
		}

//		bool ParseAdvanced;
//		bool ParseLegacy;
//		bool ParseOnly;
//		bool ForceDeleteLock;

		Build& build = Build::b();
		Messages::defer( build.mayDefer() && (Messages::verboseness() < 2) );
		return {log,mysql};
	}

}
