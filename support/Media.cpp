//
// Created by Ben on 2019-04-30.
//

#include <sstream>

#include "Media.h"
#include "support/Timing.h"
#include "support/Env.h"
#include "support/Convert.h"
#include "support/Fandr.h"
#include "support/Encode.h"
#include "support/Crypto.h"
#include "support/db/Query.h"
#include "support/db/Connection.h"
#include "support/db/Service.h"
#include "node/Content.h"
#include "node/Metrics.h"
#include "mt/Driver.h"
#include "mt/Instance.h"
#include "mt/Definition.h"
#include "content/Segment.h"

namespace Support {

	Media::Media() {
		string magicPath;
		Env::e().get("IMAGEMAGICK",magicPath,"/usr/bin/convert -background none");
		imagick = File(magicPath);
	}

	MediaInfo Media::setfile(Messages& errs,const std::string &ref,size_t index) {

		string fpath,directory,imgbase,extension;
		::time_t modified;
		size_t version;
		bool use_id;

		//Set up a context for contextual macro expansion.
		node::Metrics metrics;
		metrics.push(node::Content::root(),nullptr);
		mt::Instance instance(&metrics);
		mt::mstack context;
		context.push_back({nullptr,instance}); //This is our context.

		media->setRow(errs,index);
		media->readfield(errs,"version", version);   // not sure why we do it this way...
		media->readfield(errs,"downloads", use_id);  // is this in a downloads-only category?
		media->readfield(errs,"directory",directory);
		media->readfield(errs,"ext",extension);
		media->readfield(errs,"udate",modified);

		if ((ref.size() > 3) && (ref[0]=='C') && (ref[1]=='M') && (ref[2]=='_')) {
			size_t endpos=ref.find('_',3);
			auto nodeId = natural(ref.substr(3,endpos-3));
			auto segId  = natural(ref.substr(endpos+1,string::npos));
			const node::Content* node = node::Content::root()->content(errs, nodeId, true); //Content by id. suppressed.
			if (node != nullptr) {
				imgbase = node->ref();
				auto* nodeLayout = node->layout();
				string segName = nodeLayout->segRef(errs,segId);
				auto scopedSegment = nodeLayout->segmentInScope(errs,segName);
				if(scopedSegment.first) {
					imgbase.push_back('_');
					fileEncode(segName);
					imgbase.append(segName);
					metrics.push(node,scopedSegment.second);
				} else {
					metrics.push(node,nullptr);
				}
				directory = mt::Driver::expand(errs,directory,context);
				metrics.pop();
			} else {
				directory = mt::Driver::expand(errs,directory,context);
				media->readfield(errs,"imgbase",imgbase);
			}
		} else {
			directory = mt::Driver::expand(errs,directory,context);
			media->readfield(errs,"imgbase",imgbase);
		}
		wsstrip(directory);
		wsstrip(extension);
		wsstrip(imgbase);
		tolower(imgbase);
		fandr(imgbase,".");
		fandr(imgbase,"_","-");  //This has to happen in order to catch linkrefs that have _ in them.
		if (use_id) { //used to uniquify media onsite.
			ostringstream iStr;
			iStr << imgbase << "_" << version;
			imgbase = iStr.str();
		}
		directory.insert(0,"/media/");
		directory.append("/");

		metrics.pop();
		return {version,modified,directory,imgbase,extension};
	}

	string Media::transFile(Messages& errs,const node::Metrics* metrics,pair<string,string>& ref,size_t index) {
		vector<string> parms;
		pair<string,string> trRef;
		string transName = ref.second;
		string parmList;
		split('(',transName,trRef);
		string transformCode;

		if(!trRef.second.empty()) {
			trRef.second.pop_back(); 	//remove close bracket. This is now a ;-delimited list of parameters
			transName = trRef.first;	//transform is now the macro reference.
			parmList = trRef.second;
		}

		if(!transName.empty()) {
			auto transform = transforms.find(transName);
			if (transform != transforms.end()) {
				tolist(parms,parmList,",");
				mt::plist parameters;
				for(auto& i: parms) {
					mt::mtext parm;
					parm.emplace_back(new mt::Text(i));
					parameters.emplace_back(std::move(parm));
				}
				node::Metrics iMetrics(metrics);
				mt::iteration it = {0,parameters.size()};
				mt::Instance instance(parameters,it,&iMetrics); // we don't want to generate here.
				std::ostringstream result;
				mt::mtext resultTokens;
				mt::mstack empty;
				transform->second.expand(errs,resultTokens,instance,empty);
				for (auto &i : resultTokens) {
					i->final(result);
				}
				transformCode = result.str();
			} else {
				ostringstream msg;
				msg << "Media transform " << transName << " is not known.";
				errs << Message(error,msg.str());
			}
		}

		MediaInfo& filebits = filenames[ref.first];
		string media_uniq= filebits.base;
		string extension=filebits.ext;
		auto tx = trExtensions.find(transName);
		if(tx != trExtensions.end()) {
			extension=tx->second;
		}
		//uniqueness requires version_id and the evaluation of this particular transform (including parameters).
		string tr_hash = Digest::hash(media_uniq + "@" + transformCode,errs);
		string filename = filebits.dir + filebits.base + "_" + transName + "_" + tr_hash + "." + extension;
		mediaUsed.emplace(ref.first);
		auto it = instances.find(ref.first);
		if(it == instances.end()) {
			instances.insert({ref.first,{{filename,transformCode}}});
		} else {
			it->second.insert({filename,transformCode});
		}
		return filename;
	}

//-----------------------------------------------------------------------------
	string Media::embed(Messages& errs,const string& ref,bool encode) const {
		string result;
		pair<string,string> mtrans = getRef(ref);
		auto mi = embedmap.find(mtrans.first);
		if(mi != embedmap.end()) {
			emedia->readfield(errs, mi->second,"img",result);
			if(encode) {
				base64encode(result);
			}
		} else {
			errs << Message(error,mtrans.first + " was not found as an embedded media.");
		}
		return result;
	}

//-----------------------------------------------------------------------------
	std::string Media::file(Messages& errs,const node::Metrics* metrics,const string& ref) {
		pair<string,string> mtrans = getRef(ref);
		auto mi = mediamap.find(mtrans.first);
		if(mi == mediamap.end()) {
			mi = embedmap.find(mtrans.first);
			if(mi == embedmap.end()) {
				errs << Message(error,mtrans.first + " was not found in as media.");
				return "";
			} else {
				size_t index = mi->second;
				string mime,text,result;
				if ((ref.size() < 4) || (ref[0]!='C') || (ref[1]!='M') || (ref[2]!='_')) {
					errs << Message(warn, mtrans.first + " is embedded. Use iEmbedMedia.");
				}
				emedia->setRow(errs,index);
				emedia->readfield(errs,"mime",mime);
				emedia->readfield(errs,"img",result);
				if(!left(mime,4,text) || text != "text") {
					base64encode(result);
				}
				return result;
			}
		} else {
			size_t index = mi->second;
			if (!mtrans.second.empty()) { //has a transform..
				return transFile(errs, metrics, mtrans, index);
			} else {
				string filename, extension;
				media->setRow(errs, index);
				media->readfield(errs, "ext", extension);     //not sure why we do it this way...
				MediaInfo &filebits = filenames[mtrans.first];
				filename = filebits.dir + filebits.base + "." + extension;
				mediaUsed.emplace(mtrans.first);
				return filename;
			}
		}
	}

//-----------------------------------------------------------------------------
	bool Media::exists(Messages&,const string& ref) const {
		pair<string,string> mtrans = getRef(ref); // media / trans
		//not M.mt or not E.mt = found in M or found in E.
		if (mediamap.find(mtrans.first) != mediamap.end()) return true;
		return embedmap.find(mtrans.first) != embedmap.end();
	}

//-----------------------------------------------------------------------------
	string Media::attribute(Messages& errs,const string& ref,const string& attr,bool embedded) {
		string result;
		pair<string,string> mtrans = getRef(ref);
		auto mi = mediamap.find(mtrans.first);
		if(mi != mediamap.end()) {
			media->readfield(errs, mi->second,attr,result);
		} else {
			mi = embedmap.find(mtrans.first);
			if(mi != embedmap.end()) {
				emedia->readfield(errs, mi->second,attr,result);
			} else {
				errs << Message(error,mtrans.first + " was not found.");
			}
		}
		return result;
	}

	void Media::close() {
		if (emedia != nullptr)  delete emedia;
		if (media != nullptr)   delete media;
	}

	map<size_t,size_t> Media::loadBinaries(Messages& errs,Db::Query*& query) {
		map<size_t,size_t> store;			//  index to binaries.. maybe we should do this later when we have ignored unnecessaries..
		if(query && !mediaUsed.empty()) {
			ostringstream binsql;
			binsql << "select id,img from bldmediav where id in (";
			for (auto ref : mediaUsed) {
				binsql << filenames[ref].version << ",";
			}
			binsql << "0)"; //deal with trailing comma.
			query->setquery(binsql.str());
			if(query->execute(errs)) { //keep this open until we have exported all we need.
				size_t idx = 1, id = 0;
				while(query->nextrow()) {
					query->readfield(errs, "id",id);	//discarding bool.
					store.insert({id,idx});
					idx++;
				}
			}
		}
		return store;
	}

	void Media::writeMedia(Messages& errs,Db::Query*& query,size_t index,const string& filename) {
		FILE *out = fopen(filename.c_str(),"wb+");
		if (out != nullptr) {
			string mfile;
			query->readfield(errs, index,"img",mfile);
			fwrite(mfile.c_str(), 1, mfile.length(), out);
			fclose(out);
			chmod(filename.c_str(),S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		}
	}

	void Media::save(Messages& errs,Db::Connection* c,bool reset) {
		if(!mediaUsed.empty()) {
			errs << Message(warn,"Media::save needs to be implemented.");
			//TODO:: set this up in the proper place.
			if ( reset ) {
				//Built, Temporary, Scripts, Blobs, Tests
				doSave(errs,c,Env::e().basedir(Temporary),Env::e().basedir(Blobs),reset);
			} else {
				doSave(errs,c,Env::e().basedir(Blobs),Env::e().basedir(Blobs),reset);
			}
		} else {
			errs << Message(info,"No Media to generate.");
		}
	}

//The following is run once per language per build.
	void Media::doSave(Messages& errs,Db::Connection* c,const Path& outDir,const Path& orgDir,bool reset) {
		Query* query = c->query(errs);
		map<size_t,size_t> store = loadBinaries(errs,query); // Now we have a version -> binary map.
		for (auto ref : mediaUsed) {
			MediaInfo& filebits = filenames[ref]; //Get file information from the filenames map.

			//Identify and construct directory for media...
			Path outPath(outDir);
			Path orgPath(orgDir);
			outPath.cd(filebits.dir,true); //force append (even if dir starts with root)
			orgPath.cd(filebits.dir,true);
			outPath.makeDir(errs,true); // This now makes the directories if they are not present.
			orgPath.makeDir(errs,true);

			//set up the basis for the file.
			File outFile(outPath);
			outFile.setBase(filebits.base);
			outFile.setExtension(filebits.ext);
			File orgFile(orgPath);
			orgFile.setBase(filebits.base);
			orgFile.setExtension(filebits.ext);

			::time_t orgdate = orgFile.getModDate();				// the unix_time value of the modification date of the current published file (or zero).
			string outfp = outFile.output(true);
			string t_origin = outfp; //what to use as the basis of a transform.
			if(filebits.modified > orgdate) { // needs outputting.
				writeMedia(errs,query,store[filebits.version],outfp);
			} else {
				string orgfp = orgFile.output(true);
				t_origin = orgfp;
				if ( reset ) {
					orgFile.copyTo(outFile,errs,false);
				}
			}
			doTransforms(errs,ref,outDir,orgDir,t_origin,orgdate,filebits,reset);
		}
	}

	void Media::doTransforms(Messages& errs,string& ref,const Path& outPath,const Path& orgPath,const string& t_origin,std::time_t orgdate,MediaInfo& filebits,bool reset) {
		if(instances.find(ref) != instances.end()) {
			unordered_map<string,string>& mediaTransforms = instances[ref]; //And get any transforms.
			for (auto trn : mediaTransforms) {
				File trOrgFile(orgPath);
				File trOutFile(outPath);
				trOrgFile.setFileName(trn.first,true); //ignore leading slash.
				trOutFile.setFileName(trn.first,true);

				bool needGen = true;
				if (filebits.modified <= orgdate) { //the media source is old.
					std::time_t trdate = trOrgFile.getModDate();	//if the transform changed we will have a different md5.
					if (trdate > 0) {
						needGen = false;
						if(reset) {
							trOrgFile.copyTo(trOutFile,errs,false);
						}
					}
				}
				if(needGen) {
					ostringstream tos;
					tos << t_origin << " " << trn.second << " " << trOutFile.output(true);
					imagick.exec(errs,tos.str());
				}
			}
		} //end of transforms
	}


//-----------------------------------------------------------------------------
//Happens once per build... or not at all..
	void Media::move(Messages& errs,bool reset) {
		ostringstream mvos,mvts,rmos;
		string outDir;
		string retval;
		File move("/bin/mv");
		File del("/bin/rm");
		errs << Message(warn,"Media::move is not yet properly implemented.");
//TODO:: sort all the below this out.
		Env::e().basedir(outDir,Temporary,true,true);
		if ( reset ) {
			string orgDir;
			Env::e().basedir(orgDir,Blobs,true,true);
			mvos << "-f  " << orgDir << "media " << orgDir << "media_old";
			mvts << "-f  " << outDir << "media " << orgDir << "media";
			rmos << "-rf " << orgDir << "media_old";

			move.exec(errs,rmos.str()); //remove media_old if it's there.
			move.exec(errs,mvos.str());
			move.exec(errs,mvts.str());
			del.exec(errs,rmos.str());
		} else {		//the built media have been moved/copied just delete temp.
			rmos << " -rf " << outDir;
			del.exec(errs,rmos.str());
		}
	}

//-----------------------------------------------------------------------------
	void Media::load(Messages& errs,Db::Connection* c) {
		Timing& times = Timing::t();
		if (times.show()) { times.set("Media load"); }
		normalise(errs,c);
		loadTransforms(errs,c);
		loadMedia(errs,c,media,mediamap,false);
		loadMedia(errs,c,emedia,embedmap,true);
		if (times.show()) { times.use(errs,"Media load"); }

	}
//-----------------------------------------------------------------------------
//Normalise Media.
	void Media::normalise(Messages& errs,Db::Connection* c) {
		if (c->table_exists(errs,"bldmediav")  ) {
			c->exec(errs,"lock tables bldmediav write");
			Query* q = c->query(errs,"select id,imgbase from bldmediav where not (imgbase regexp '^[a-zA-Z0-9-]+$')");
			if ( q->execute(errs) ) {
				while(q->nextrow()) {
					size_t f_id;
					string f_base;
					q->readfield(errs,"id",f_id);
					q->readfield(errs,"imgbase",f_base);
					ostringstream update_str;
					fandr(f_base,"_","-");
					fandr(f_base,".");
					fileencode(f_base);
					update_str << "update bldmediav set imgbase=\"" << f_base << "\" where id=" << f_id ;
					Query* i = c->query(errs,update_str.str()); i->execute(errs); delete i;
				}
			}
			delete q;
			c->exec(errs,"unlock tables");
		}
	}

//-----------------------------------------------------------------------------
	string Media::loaderSQL(bool embedded) {
		ostringstream sqlstr;
		sqlstr << "select m.id, m.ref, v.imgbase, v.imgext as ext, v.altname as alt,";
		if(embedded) {
			sqlstr << "v.img,";
		}
		sqlstr << "v.id as version,v.imgwidth as width,v.imgheight as height,v.imglength as size,"
				  "v.imgtype as mime,v.description,v.editor, m.lng, v.directory,"
				  "DATE_FORMAT(v.tstamp,'\%Y \%m \%d \%H \%i \%S') as cdate,UNIX_TIMESTAMP(v.tstamp) as udate,"
				  "v.imgname,v.mapped as map, m.loband as lo,v.comments"
				  " as longdesc,c.downloads from bldmedian m,bldmediav v,bldmediacat c"
				  " where m.used='on' and c.id=m.cat and m.version=v.id and m.id=v.mediaid";
		if(embedded) {
			sqlstr << " and m.embedded = 'on'";
		} else {
			sqlstr << " and m.embedded != 'on'";
		}
		return sqlstr.str();
	}

//-----------------------------------------------------------------------------
	void Media::loadTransforms(Messages& errs,Db::Connection* c) {
		trExtensions.clear();
		if (c->table_exists(errs,"bldtransform")  ) {
			c->exec(errs,"lock tables bldtransform read");
			Query* q = c->query(errs, "select name,ext,transform from bldtransform");
			if ( q->execute(errs) ) {
				while(q->nextrow()) {
					string f_name,f_ext,f_transform;
					q->readfield(errs,"name",f_name);
					q->readfield(errs,"ext",f_ext);
					q->readfield(errs,"transform",f_transform);
					mt::Definition macro(errs,f_name,f_transform,0,INT_MAX,true,true,false);
					transforms.emplace(f_name,std::move(macro));
					if(!f_ext.empty()) {
						trExtensions.insert({f_name,f_ext});
					}
				}
			}
			delete q;
			c->exec(errs,"unlock tables");
		}
	}

//-----------------------------------------------------------------------------
	void Media::setFilenames(Messages& errs) {
		/**
		 * Must happen after layouts are assigned to content.
		 * This means that maybe we need to generate media (or some media) after each technology.
		 */
		filenames.clear();
		for(auto& i : mediamap) {
			filenames.insert({i.first,setfile(errs,i.first,i.second)});
		}
	}
//-----------------------------------------------------------------------------
	void Media::loadMedia(Messages& errs,Db::Connection* c,Query*& query,unordered_map<string,size_t>& store,bool embedded) {
		store.clear();
		if (c->table_exists(errs,"bldmedian") && c->table_exists(errs,"bldmediav") && c->table_exists(errs,"bldmediacat")) {
			c->exec(errs,"lock tables bldmedian m read,bldmediav v read,bldmediacat c read");
			query = c->query(errs, loaderSQL(embedded));
			if ( query->execute(errs) ) {
				size_t idx = 1;
				string f_ref;
				while(query->nextrow()) {
					query->readfield(errs, "ref",f_ref);	//discarding bool.
					store.insert({f_ref,idx});
					idx++;
				}
			} else {
				errs << Message(error,"SQL Media Loading Error ");
			}
			errs << Message(info,to_string(media->getnumrows()) + " media imported.");
		}
		c->exec(errs,"unlock tables");
	}
//-----------------------------------------------------------------------------
	pair<string,string> Media::getRef(const string& ref) const {
		string mediaref = ref;
		pair<string,string> mtrans;
		if ( split(':',mediaref, mtrans) ) {
			mediaref=mtrans.first;
		}
		return mtrans;
	}
//-----------------------------------------------------------------------------

}
