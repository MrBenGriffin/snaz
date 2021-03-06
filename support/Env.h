//
// Created by Ben on 2019-02-14.
//

#ifndef MACROTEXT_ENVIRONMENT_H
#define MACROTEXT_ENVIRONMENT_H

#include <string>
#include <set>
#include <memory>

#include "support/Definitions.h"
#include "support/File.h"
#include "support/Storage.h"
#include "db/Connection.h"

namespace Support {
	using 	namespace std;

	class Env {
	private:

		static constexpr auto TmpDir 		= "tmp";
		static constexpr auto BuildDir 		= "build";
		static constexpr auto LogsDir 		= "logs";
		static constexpr auto TestsDir 		= "tests";
		static constexpr auto FinalDir 		= "final";
		static constexpr auto DraftDir 		= "draft";
		static constexpr auto ScriptsDir 	= "scripts";
		static constexpr auto StagingDir 	= "staging";
		static constexpr auto ReleaseDir 	= "release";
		static constexpr auto IncludeDir 	= "include";
		static constexpr auto MediaDir 	    = "media";

		Storage storage;
		std::deque<size_t> askedTechs;
		std::deque<size_t> askedLangs;

		string wd();

		Env(); // Disallow instantiation outside of the class.

		bool ParseAdvanced;
		bool ParseLegacy;

		Path WorkingDir;
		string RemoteUser;
		string EditNodeUrl; //= "/mortar/oedit.obyx?node=0";

		void doArgs(Messages&,int,const char**);
		void doLangTech(Path&);

	public:
		static Env& e();
		pair< unique_ptr<Messages>,Db::Connection*> startup(int=0,const char** = nullptr);
		void shutdown(unique_ptr<Messages>&,Db::Connection*);

		string file_to_check;	// File to be used when checking syntax.

		bool get(string,string&,string="");
		bool getBool(const string&,const bool&);
		std::string get(const string&);
		buildArea area();

		Path dir(buildSpace, buildSub=Root);
		Path urlRoot(buildSpace=Built);
		void space(Path&,buildSpace,buildKind);

		std::string baseUrl(buildArea);
		void setWorkingDir(Messages&, bool);
		void releaseWorkingDir(Messages&, bool);

		const std::deque<size_t>& techs() const {return askedTechs;}
		const std::deque<size_t>& langs() const {return askedLangs;}
	};

}

#endif //MACROTEXT_ENVIRONMENT_H
