//
// Created by Ben on 2019-02-14.
//

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <clocale>
//#include <sys/types.h>
#include <unistd.h>

#include "support/Env.h"
#include "support/Fandr.h"
#include "support/Infix.h"
#include "support/Convert.h"
#include "support/Timing.h"
#include "support/Message.h"
#include "support/db/ServiceFactory.h"
#include "support/File.h"

#include "mt/Definition.h"

#include "node/Locator.h"
#include "node/Content.h"

#include "content/Template.h"

#include "Build.h"

namespace Support {
	using namespace std;

	Env &Env::e() {
		static Env singleton;
		return singleton;
	}

	Env::Env() : ParseAdvanced(false),ParseLegacy(false) {
		string SiteRootDir;
		if (!get("RS_PATH",SiteRootDir))  {
			SiteRootDir=wd();
		}
		Path root(SiteRootDir);
		Path::setSiteRoot(root);
	}

	string Env::wd() {
		char buff[PATH_MAX];
		getcwd( buff, PATH_MAX ); // unistd.h
		std::string cwd( buff );
		return cwd;
	}

	Path Env::urlRoot(buildSpace bspace) {
		Path path = Path(); //rooted at siteroot.
		auto kind = Build::b().current();
		space(path,bspace,kind);
		return path;
	}

	//Path final(env.dir(Built, Support::Content));
	Path Env::dir(buildSpace bspace, buildSub sub) {
		Path path= urlRoot(bspace); //rooted at siteroot.
		if(bspace == Temporary || bspace == Built) {
			switch (sub) {
				case Blobs:
					path.cd(MediaDir);
					break;
				case Content:
					doLangTech(path);
					break;
				case Root:
					break;
			}
		}
		return path;
	}

	void Env::space(Path& path,buildSpace space, buildKind kind) {
		switch (space) {
			case Temporary:
				path = WorkingDir;
				break;
			case Built: {
				switch (kind) {
					case test:
						path.cd(TestsDir);
						break;
					case final:
						path.cd(FinalDir);
						break;
					case draft:
						path.cd(DraftDir);
						break;
					case parse:
					case define:
						path.cd(TmpDir);
						break;
				}
			} break;
			case Scripts: {
				path.cd(ScriptsDir);
			} break;
			case Tests:	{
				path.cd(TestsDir);
			} break;
		}
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

	void Env::doLangTech(Path& path) {
		static Build& build = Build::b();
		if (getBool("RS_USE_LANGUAGE",true))
			path.cd(build.language().ln);
		if (getBool("RS_USE_TECHNOLOGY",true))
			path.cd(build.technology().dir);
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
				auto base = dir(Built).output(true);
				response = "file:" + base + url ;
			}
		return response;
	}

	std::string Env::get(const string& name) {
		string value=name;
		get(name,value); //discarding bool.
		return value;
	}

	bool Env::getBool(const string& name,const bool& Default=false) {
		auto discovery = storage.find(name);
		if(discovery.found) {
			return discovery.result == "true";
		} else {
			char* tmp = std::getenv(name.c_str());
			if(tmp != nullptr) {
				string value = tmp;
				storage.set(name,value);
				return value == "true";
			} else {
				return Default;
			}
		}
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

	void Env::setWorkingDir(Messages& log, bool fullBuild) {
		if(fullBuild) {
			WorkingDir = Path();
			WorkingDir.cd(LogsDir);
			WorkingDir.cd(BuildDir);
			WorkingDir.makeTempDir(log,string(Build::b().current()));
		} else {
			WorkingDir = dir(Built, Root);
		}
		log << Message(debug,WorkingDir.output() + " (Working Dir)");
	}

	void Env::releaseWorkingDir(Messages& log, bool fullBuild) {
		if(fullBuild) {
			WorkingDir.removeDir(log);
		}
		log << Message(debug,"Released (Working Dir)");
	}


	void Env::doArgs(Messages& log,const int argc,const char **argv) {
		Timing& timer = Timing::t(); //auto startup at instantiation.
		Build& build = Build::b();

		for(int i = 1; i < argc; i++) {
			string parameter(argv[i]);
			string::const_iterator argi = parameter.begin();
			if(*argi++ == '-') { // expect a '-'
				switch(*argi++) {
					case 'A':
						timer.setShow(true);
						break;
					case 'b':
						content::Template::show(true);
						break;
					case 'B': {
						deque<size_t> nodes;
						tolist(nodes, parameter.substr(2));
						build.setNodes(Branch,nodes);
					} break;
					case 'c':
						if(parameter == "-check") {
							build.setCurrent(parse); // = true;
							if (i == argc) {
								log << Message(error," -check needs a path to the file to test");
							} else {
								file_to_check = string(argv[++i]);
								log.reset();
								log.set_no_store();
								log.set_debug(false);
							}
						} else {
							log << Message(error,"-c is old. Please use -check {path_to_file}");
						}
					case 'D': {
						node::Locator::showPaths = true;
						content::Template::show(true);
					} break;
					case 'd':
						if(parameter == "-define") {
							build.setCurrent(define); // = true;
							if (i == argc) {
								log << Message(error," -define needs a path to the file to test");
							} else {
								file_to_check = string(argv[++i]);
								log.reset();
								log.set_no_store();
								log.set_debug(false);
							}
						} else {
							build.setCurrent(test); // = true;
						}
						break;
					case 'F':{
						deque<size_t> nodes;
						tolist(nodes, parameter.substr(2)); //setnodes
						build.setNodes(Descendants,nodes);
					} break;
					case 'L': {
						tolist(askedLangs, parameter.substr(2));
					} break;
					case 'M': {
						string cssurl;
						get("RS_CSSFILE", cssurl);         //rebuild.css   rebuild.css
						fandr(cssurl, "[S]", parameter.substr(
								2));  //allow for a css switch. eg rebuild[S].css env and -MAdmin -> rebuildA.css
						get("RS_BUILDNODEPATH", EditNodeUrl, "/mortar/oedit.obyx?node=0"); //if none, keep default.
					} break;
						case 'm': {
							if(parameter == "-macro") {
								if (i == argc) {
									log << Message(error," -macro needs the name of the macro being defined.");
								} else {
									build.setMacro(string(argv[++i]));
								}
							}
						} break;
					case 'O': {
						deque<size_t> nodes;
						tolist(nodes, parameter.substr(2));
						build.setNodes(Single,nodes);
					} break;
					case 'p':
						node::Locator::showPaths = true;
						break;
					case 'P':
						build.setCurrent(draft);
						break;
					case 'Q':
//						showQueries = true;
						break;
					case 's':
//						showGetSet = true;
						break;
					case 't':
						if(parameter == "-tests") {
							build.setCurrent(test); // = true;
							log.set_no_store();
						} else {
//							Messages::setVerbosity(4);
							log << Message(error,"-t (macro tracing / parse visits) is not yet implemented.");
						}
						break;
					case 'T': {
						tolist(askedTechs, parameter.substr(2));
					} break;
					case 'X':
						build.breakLock();
						break;
					case 'x': //comment until next -
						break;
					default:
						log << Message(warn,"Parameter (" + parameter + ") is unknown or no longer used.");
						break;
				}
			} else {
				log << Message(warn,"Parameter (" + parameter + ") is unknown. Parameters should be prefixed with a '-'");
			}
		}
	}

	void Env::shutdown(std::unique_ptr<Support::Messages>& log, Support::Db::Connection *sql) {
		if(log) {
			if(!sql) {
				log->str(std::cerr,true);
			}
			log->shutdown();
		}
		if(sql) {
			sql->close();
		}
	}

	pair< unique_ptr<Messages>,Db::Connection*> Env::startup(int argc,const char **argv) {
		setlocale(LC_ALL, "en_UK.UTF-8");
		Db::Connection *mysql = nullptr;
		Infix::Evaluate::startup();
		string config_path;
		unique_ptr<Messages> log = make_unique<Messages>(mysql);
//		Messages log(mysql);
		if(get("SQL_CONFIG_FILE",config_path)) {
			mysql = Db::ServiceFactory::sf().getConnection(*log,"mysql",config_path);
		} else {
			*log << Message(fatal,"The environment variable `SQL_CONFIG_FILE` must be set.");
		}
		log->setConnection(mysql);
		doArgs(*log,argc,argv);

		//Initialise RNG.
		::time_t tt; time(&tt);
		clock_t clk = clock();
		srand48( static_cast<long>(clk+tt) );	//need to set up the randomizer once

		*log << Message(debug,"Environment is initialised.");

//		Build& build = Build::b();
//		Messages::defer( build.mayDefer() && (Messages::verboseness() < 2) );
		return {std::move(log),mysql};
	}

}
