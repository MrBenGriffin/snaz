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
	enum	buildspace {Built,Temporary,Scripts,Media,Tests};	//Buildspace indicates what directory area is relevant
	enum 	buildArea  {Editorial,Final,Draft,Console,Release,Staging,Testing};

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

		Storage storage;

		string wd();

		Env(); // Disallow instantiation outside of the class.
		bool IsFinal; //is this draft or final.
//		bool MayBuild;
//		bool FullBuild;
//		bool AllTechs;
//		bool AllLangs;

//		deque<tech>   technologies; //queue holding all information about the build
//		deque<size_t> nodelist;     //things to build.

//		bool ParseOnly;

//		size_t LanguageID;
//		size_t TechnologyID;
//		size_t TechnologyCount;
//		size_t LanguageCount;

		bool ParseAdvanced;
		bool ParseLegacy;
		bool ForceDeleteLock;

		string SiteRootDir;
		string RemoteUser;
		string EditNodeUrl; //= "/mortar/oedit.obyx?node=0";


		void doArgs(Messages&,int,const char**);

	public:
		static Env& e();
		pair<Messages&,Db::Connection*> startup(int=0,const char** = nullptr);
		bool   get(string,string&,string="");
		buildArea area();

		Path basedir(buildspace);
		void basedir(string&,buildspace,bool,bool);
		std::string baseUrl(buildArea);
//		tech& technology()  { return technologies.front(); }  //returns currently built technology.
	};

}

#endif //MACROTEXT_ENVIRONMENT_H
