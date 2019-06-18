//
// Created by Ben on 2019-02-14.
//

#ifndef MACROTEXT_ENVIRONMENT_H
#define MACROTEXT_ENVIRONMENT_H

#include <string>
#include <set>

#include "Definitions.h"
#include "File.h"
#include "Storage.h"
#include "db/Connection.h"

namespace Support {
	using 	namespace std;

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
		std::deque<size_t> askedTechs;
		std::deque<size_t> askedLangs;

		string wd();

		Env(); // Disallow instantiation outside of the class.

		bool ParseAdvanced;
		bool ParseLegacy;

		string SiteRootDir;
		string RemoteUser;
		string EditNodeUrl; //= "/mortar/oedit.obyx?node=0";

		void doArgs(Messages&,int,const char**);

	public:
		static Env& e();
		pair< unique_ptr<Messages>,Db::Connection*> startup(int=0,const char** = nullptr);
		bool   get(string,string&,string="");
		std::string get(const string&);
		buildArea area();

		Path basedir(buildSpace);
		void basedir(string&,buildSpace,bool,bool);
		std::string baseUrl(buildArea);
		const std::deque<size_t>& techs() const {return askedTechs;}
		const std::deque<size_t>& langs() const {return askedLangs;}
//		tech& technology()  { return technologies.front(); }  //returns currently built technology.
	};

}

#endif //MACROTEXT_ENVIRONMENT_H
