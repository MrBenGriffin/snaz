//
// Created by Ben on 2019-02-14.
//

#ifndef MACROTEXT_ENVIRONMENT_H
#define MACROTEXT_ENVIRONMENT_H

#include <string>

#include "File.h"
#include "Storage.h"

namespace Support {
	using 	namespace std;
	enum	buildspace {Build,Temporary,Scripts,Media,Tests};	//Buildspace indicates what directory area is relevant
	enum 	buildarea  {Editorial,Final,Draft,Console,Release,Staging};
	enum    buildnodes {Branch,Descendants,Singles,None};

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
		bool Testing;
		bool IsFinal; //is this draft or final.
		bool MayBuild;
		bool FullBuild;
		bool AllTechs;
		bool AllLangs;

		bool ParseAdvanced;
		bool ParseLegacy;
		bool ParseOnly;
		bool ForceDeleteLock;

		size_t LanguageID;
		size_t TechnologyID;
		size_t TechnologyCount;
		size_t LanguageCount;

		string SiteRootDir;
		string RemoteUser;
		string EditNodeUrl; //= "/mortar/oedit.obyx?node=0";


		buildnodes NodeSet;

	public:
		static Env& e();
		void   startup(const int=0,const char** = nullptr);
		bool   get(string,string&,string="");
		buildarea area();

//		string logsDir()  	const 	{ return LogsDir; }
//		string finalDir() 	const 	{ return FinalDir; }
//		string draftDir() 	const 	{ return DraftDir; }
//		string scriptsDir() const 	{ return ScriptsDir; }
		Path basedir(buildspace);
		void basedir(string&,buildspace,bool,bool);
		void setTesting(bool flag) { Testing = flag; }
		size_t techID() { return TechnologyID; }
//		Path root(string append = "");
	};

}

#endif //MACROTEXT_ENVIRONMENT_H
