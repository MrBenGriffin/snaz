//
// Created by Ben on 2019-04-30.
//

#ifndef MACROTEXT_MEDIA_H
#define MACROTEXT_MEDIA_H

#include <unordered_set>
#include <map>
#include <variant>

#include "mt/declarations.h"
#include "support/File.h"
#include "support/Message.h"
#include "support/db/Query.h"

using namespace Support::Db;


namespace node {
	class Metrics;
}

namespace Support {
	class Date;
	class File;
	class Definitions;
	namespace Db {
		class Connection;
	}

	struct MediaInfo {
		size_t 	version;
//		string 	versionStr;
		::time_t modified;
		string 	dir;
		string 	base;
		string 	ext;
	};

	class Media {
		Query*							media;				// Media query. All media is kept in here, ready for use.
		Query*							emedia;				// Embedded media query.  All embedded media is kept in here, ready for use.

		unordered_map<string, mt::Definition> transforms;

		unordered_map<string,size_t>	mediamap;			// Reference -> resultSet index
		unordered_map<string,size_t>	embedmap;			// Reference -> resultSet line
		unordered_map<string,string>	trExtensions;
		unordered_map<string,MediaInfo> filenames;	// Reference -> <directory,baseFilename> (pre transform), excluding extension.
		unordered_map<string,unordered_map<string,string>> instances;	// Reference -> set of final {filename, transform.}
		unordered_set<string> 			mediaUsed;	// Reference = files to be generated.
		File 							imagick;

		pair<string,string> getRef(const string &) const;
		string transFile(Messages&,const node::Metrics*,pair<string,string>&,size_t);
//		string transMacroName(const string&);

		//Load / Save etc.
		string loaderSQL(bool);
		MediaInfo setfile(Messages&,const string&,size_t); 	// sets filenames
		void	loadMedia(Messages&,Db::Connection*,Db::Query*&,unordered_map<string,size_t>&,bool);
		void	loadTransforms(Messages&,Db::Connection*);
		void 	normalise(Messages&,Db::Connection*);
		void	writeMedia(Messages&,Db::Query*&,size_t,const string&);
		void  doSave(Messages&,Db::Connection*,const Path&,const Path&,bool);
		void	doTransforms(Messages&,string&,const Path&,const Path&,const string&,std::time_t,MediaInfo&,bool);
		map<size_t,size_t> loadBinaries(Messages&,Db::Query*&);


	public:
		Media();
		void	load(Messages&,Db::Connection*);
		void	setFilenames(Messages&);

		void	save(Messages&,Db::Connection*,bool);
		void	move(Messages&,bool);
		void	close();
		bool	exists(Messages&,const string&) const;
		string file(Messages&,const node::Metrics*,const string&);
		string embed(Messages&,const string&,bool) const;
		string attribute(Messages&,const string&,const string&,bool = false);
	};
}


#endif //MACROTEXT_MEDIA_H
