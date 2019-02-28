//
// Created by Ben on 2019-02-14.
//

#ifndef MACROTEXT_ENVIRONMENT_H
#define MACROTEXT_ENVIRONMENT_H

#include <string>

#include "File.h"
#include "Storage.h"
#include "db/Connection.h"

namespace Support {
	using 	namespace std;
	enum	buildspace {Build,Temporary,Scripts,Media,Tests};	//Buildspace indicates what directory area is relevant
	enum 	buildarea  {Editorial,Final,Draft,Console,Release,Staging};
	enum    buildnodes {Branch,Descendants,Singles,None};

	struct tech {
		size_t id;
		string name;
	};

	class Env {
	private:
		static constexpr auto LogsDir 		= "logs";
		static constexpr auto TestsDir 		= "tests";
		static constexpr auto FinalDir 		= "final";
		static constexpr auto DraftDir 		= "draft";
		static constexpr auto ScriptsDir 	= "scripts";
		static constexpr auto StagingDir 	= "staging";
		static constexpr auto ReleaseDir 	= "release";
		static constexpr auto IncludeDir 	= "include";

		Db::Connection*	mysql;

		Storage storage;

		string wd();

		Env(); // Disallow instantiation outside of the class.
		bool Testing;
		bool IsFinal; //is this draft or final.
		bool MayBuild;
		bool FullBuild;
		bool AllTechs;
//		bool AllLangs;

		deque<tech>   technologies; //queue holding all information about the build
		deque<size_t> nodelist;     //things to build.

		bool ParseAdvanced;
		bool ParseLegacy;
		bool ParseOnly;
		bool ForceDeleteLock;

//		size_t LanguageID;
		size_t TechnologyID;
		size_t TechnologyCount;
		size_t LanguageCount;

		string SiteRootDir;
		string RemoteUser;
		string EditNodeUrl; //= "/mortar/oedit.obyx?node=0";


		buildnodes NodeSet;
		void doArgs(Messages&,int,const char**);

	public:
		static Env& e();
		Messages startup(int=0,const char** = nullptr);
		bool   get(string,string&,string="");
		buildarea area();

//		string logsDir()  	const 	{ return LogsDir; }
//		string finalDir() 	const 	{ return FinalDir; }
//		string draftDir() 	const 	{ return DraftDir; }
//		string scriptsDir() const 	{ return ScriptsDir; }
		Path basedir(buildspace);
		void basedir(string&,buildspace,bool,bool);
		void setTesting(bool flag) { Testing = flag; }
		tech& technology()  { return technologies.front(); }  //returns currently built technology.
//		Path root(string append = "");
	};

}

#endif //MACROTEXT_ENVIRONMENT_H
