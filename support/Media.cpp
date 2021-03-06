//
// Created by Ben on 2019-04-30.
//

#include <sstream>
#include <cerrno>
#include <cstring>

#include "mt/Internal.h"
#include "mt/Driver.h"
#include "mt/Instance.h"
#include "mt/Definition.h"
#include "mt/MacroText.h"

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
#include "content/Segment.h"

namespace Support {

	using namespace mt;
	Media::Media() {
		string magicPath;
		Env::e().get("IMAGEMAGICK",magicPath,"/usr/bin/convert");
		imagick = File(magicPath);
		imagick.addArg("-background none");
	}

	MediaInfo Media::setfile(Messages& errs,const std::string &ref,size_t index) {
		auto& env=Env::e();
		string fpath,directory,imgbase,extension;
		::time_t modified;
		size_t version;
		bool use_id;

		//Set up a context for contextual macro expansion.
		node::Metrics metrics;
		metrics.push(node::Content::root(),nullptr);
		mstack context;
		auto instance =Instance(&metrics);
		context.push_back({nullptr,&instance}); //This is our context.
		auto mediaDir = env.dir(Built, Blobs);
		mediaDir.makeRelativeTo(env.dir(Built));

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
				directory = MacroText::expand(errs,directory,context,true);
				metrics.pop();
			} else {
				directory = MacroText::expand(errs,directory,context,true);
				media->readfield(errs,"imgbase",imgbase);
			}
		} else {
			directory = MacroText::expand(errs,directory,context,true);
			media->readfield(errs,"imgbase",imgbase);
		}
		wsstrip(directory);
		if(!directory.empty()) {
			tolower(directory);
			mediaDir.cd(directory);
		}
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
		metrics.pop();
		return {version,modified,mediaDir,imgbase,extension};
	}

	string Media::transFile(Messages& errs,const node::Metrics* metrics,pair<string,string>& ref,size_t index) {
		deque<string> parms;
		pair<string,string> trRef;
		string transName = ref.second;
		string parmList;
		// @tvImage(@tContent($NODE$,c_image_1):recolour[@kColorThree()]
		split('[',transName,trRef);
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
				plist parameters;
				for(auto& i: parms) {
					MacroText parm;
					parm.add(i);
					parameters.emplace_back(std::move(parm));
				}
				node::Metrics iMetrics(metrics);
				std::ostringstream result;
				MacroText resultTokens;
				mstack empty;
				Instance instance(&parameters,&iMetrics);
				transform->second.expand(errs,resultTokens,instance,empty);
				resultTokens.expand(errs,result,empty);
				transformCode = result.str();
			} else {
				ostringstream msg;
				msg << "Media transform " << transName << " is not known.";
				errs << Message(error,msg.str());
			}
		}

		MediaInfo& filebits = filenames[ref.first];
		string media_uniq= filebits.dir.output() + filebits.base + "." + filebits.ext;
		string extension=filebits.ext;
		auto tx = trExtensions.find(transName);
		if(tx != trExtensions.end()) {
			extension=tx->second;
		}
		//uniqueness requires version_id and the evaluation of this particular transform (including parameters).
		string tr_hash = Digest::hash(media_uniq + "@" + transName + "@" + transformCode,errs);
		string filename = filebits.dir.output() + filebits.base + "_" + transName + "_" + tr_hash + "." + extension;
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
		/**
		 * Returns the URL-component of a media file..
		 */
		pair<string,string> mtrans = getRef(ref);
		auto mi = mediamap.find(mtrans.first);
		if(mi == mediamap.end()) {
			mi = embedmap.find(mtrans.first);
			if(mi == embedmap.end()) {
				errs << Message(error,mtrans.first + " was not found in media.");
				return "";
			} else {
				size_t index = mi->second;
				string mime,text,result;
				emedia->setRow(errs,index);
				if(mtrans.second.empty()) {
					if ((ref.size() < 4) || (ref[0]!='C') || (ref[1]!='M') || (ref[2]!='_')) {
						ostringstream msg;
						msg << "Media referenced as " << ref << " appears to be embedded. "
							<< mtrans.first << " was found in embedded media. Use iEmbed for this.";
						errs << Message(warn, msg.str());
					}
					emedia->readfield(errs,"mime",mime);
					emedia->readfield(errs,"img",result);
					if(!left(mime,4,text) || text != "text") {
						base64encode(result);
					}
				} else {
					// This embed is now required as an output for conversion.
					string extension;
					media->readfield(errs, "ext", extension);     //not sure why we do it this way...
					MediaInfo &filebits = filenames[mtrans.first];
					File mediaFile(filebits.dir, filebits.base + "." + extension);
					result = mediaFile.url(errs);
					mediaUsed.emplace(mtrans.first);
				}
				return result;
			}
		} else {
			string filename;
			size_t index = mi->second;
			if (!mtrans.second.empty()) { //has a transform..
				filename = "/" + transFile(errs, metrics, mtrans, index);
			} else {
				string extension;
				media->setRow(errs, index);
				media->readfield(errs, "ext", extension);     //not sure why we do it this way...
				MediaInfo &filebits = filenames[mtrans.first];
				File mediaFile(filebits.dir, filebits.base + "." + extension);
				filename = mediaFile.url(errs);
				mediaUsed.emplace(mtrans.first);
			}
			return filename;
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
	string Media::attribute(Messages& errs,const string& ref,const string& attr) {
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
		} else {
			string fileError(strerror( errno ));
			errs << Message(error,fileError);

		}
	}

	void Media::save(Messages& errs,Db::Connection* c,bool reset) {
		Env& env = Env::e();
		if(!mediaUsed.empty()) {
			errs.push(Message(container,"Saving Media"));
			Timing& times = Timing::t();
			times.set("Media::save");
			env.dir(Temporary, Blobs).makeDir(errs);
			env.dir(Built, Blobs).makeDir(errs);
			if ( reset ) {
				errs << Message(debug,"Found media to save with reset");
				doSave(errs,c,env.dir(Temporary),env.dir(Built),reset);
			} else {
				errs << Message(debug,"Found media to save with no reset");
				doSave(errs,c,env.dir(Built),env.dir(Built),reset);
			}
			times.use(errs,"Media::save");
			errs.pop();
		} else {
			errs << Message(info,"No Media to generate.");
		}
	}

//The following is run once per build.
	void Media::doSave(Messages& errs,Db::Connection* c,const Path& buildDir,const Path& finalDir,bool reset) {
		Timing& times = Timing::t();
		Query* query = c->query(errs);
		map<size_t,size_t> store = loadBinaries(errs,query); // Now we have a version -> binary map.
		long double adjust = mediamap.size() - mediaUsed.size();
		if(adjust > 0) {
			ostringstream str;
			str << (unsigned)adjust << " unused media skipped";
			errs << Message(channel::media,str.str(),adjust);
		}
		for (auto ref : mediaUsed) {
			MediaInfo& filebits = filenames[ref]; //Get file information from the filenames map.
			times.set(ref);

			ostringstream filename;
			filename << filebits.base << "." << filebits.ext;
			File outFile(filebits.dir,filename.str());
			outFile.makeAbsoluteFrom(buildDir);
			outFile.makeDir(errs);

			File orgFile(filebits.dir,filename.str());
			orgFile.makeAbsoluteFrom(finalDir);

			::time_t orgdate = orgFile.getModDate();				// the unix_time value of the modification date of the current published file (or zero).
			string outfp = outFile.output(true);
			string t_origin = outfp; //what to use as the basis of a transform.
			if(filebits.modified > orgdate) { // needs outputting.
				errs << Message(debug,outfp + " (write)");
				writeMedia(errs,query,store[filebits.version],outfp);
			} else {
				string orgfp = orgFile.output(true);
				t_origin = orgfp;
				errs << Message(debug,outfp + " (copy from) " + orgfp);
				if ( reset ) {
					orgFile.copyTo(outFile,errs,false);
				}
			}
			if(instances.find(ref) != instances.end()) {
				errs << Message(debug,ref + " needs transforms..");
				doTransforms(errs, ref, buildDir, finalDir, t_origin, orgdate, filebits, reset);
			} else {
				errs << Message(channel::media,ref,1.0L);
			}
			times.use(errs,ref);
		}
	}

	void Media::doTransforms(Messages& errs,string& ref,const Path& buildDir,const Path& finalDir,const string& t_origin,std::time_t orgdate,MediaInfo& filebits,bool reset) {
		if(instances.find(ref) != instances.end()) {
			//The stored transform has the full filename = eg /media/images/foo_bar..  so don't prefix directories!
			unordered_map<string,string>& mediaTransforms = instances[ref]; //And get any transforms.
			long double transformCount = mediaTransforms.size();
			if(transformCount > 0.0L) {
				long double progressValue(1.0L / transformCount);  //eg 1/2 for two transforms.
				for (auto trn : mediaTransforms) {
					File outFile(trn.first);
					outFile.makeAbsoluteFrom(buildDir);
					outFile.makeDir(errs);
					File orgFile(trn.first);
					orgFile.makeAbsoluteFrom(finalDir);
					::time_t original_date = orgFile.getModDate();				// the unix_time value of the modification date of the current published file (or zero).
					// Need to test against the master file AND the transformed file.
					if(filebits.modified > original_date || filebits.modified > orgdate ) { // needs outputting.
						errs << Message(debug,trn.first + " (transform)");
						File transform(imagick);
						transform.addArg(t_origin);
						transform.addArg(trn.second);
						transform.addArg(outFile.output(true));
						transform.exec(errs);
					} else {
						errs << Message(debug,trn.first + " (use original) ");
						if ( reset ) {
							orgFile.copyTo(outFile,errs,false);
						}
					}
					errs << Message(channel::transform,ref + ":" + trn.first,progressValue);
				}
			} else {
				errs << Message(channel::transform,ref,1.0L);
			}
		} //end of transforms
	}


//-----------------------------------------------------------------------------
// Happens once per build... or not at all..
// TODO:: This should really be refactored into a Path function
	void Media::move(Messages& errs,bool reset) {
		auto& env = Env::e();
		Path finalDir = env.dir(Built, Blobs);
		Path buildDir = env.dir(Temporary, Blobs);
		File removeDir(Path("/bin"),"rm"); removeDir.addArg("-rf");
		if ( reset ) {
			errs << Message(info, " Moving Media");
			Path finalDirOld = finalDir; finalDirOld.pop(); finalDirOld.cd("old");
			File moveOrgToOld(Path("/bin"),"mv"); moveOrgToOld.addArg("-f");
			File moveOutToOrg(moveOrgToOld);

			//move org to old.
			moveOrgToOld.addArg(finalDir.output(false));
			moveOrgToOld.addArg(finalDirOld.output(false));
			moveOrgToOld.exec(errs);
			//move out to org.
			moveOutToOrg.addArg(buildDir.output(false));
			moveOutToOrg.addArg(finalDir.output(false));
			moveOutToOrg.exec(errs);
			//remove old..
			removeDir.addArg(finalDirOld.output(false));
			removeDir.exec(errs);
		} else {
			if(! (finalDir == buildDir)) {
				//the built media have been moved/copied just delete temp.
				errs << Message(info, "Removing Temporary Media Directory");
				removeDir.addArg(buildDir.output(false));
				removeDir.exec(errs);
			}
		}
	}

//-----------------------------------------------------------------------------
	void Media::load(Messages& errs,Db::Connection* c) {
		Timing& times = Timing::t();
		times.set("Media load");
		normalise(errs,c);
		loadTransforms(errs,c);
		loadMedia(errs,c,media,mediamap,false);
		loadMedia(errs,c,emedia,embedmap,true);
		size_t mediaCount = mediamap.size();
		size_t eMediaCount = embedmap.size();
		errs << Message(info,to_string(eMediaCount) + " embedded media imported.");
		errs << Message(info,to_string(mediaCount) + " media imported.");
		errs.setProgressSize(Message::media,mediaCount);
		times.use(errs,"Media load");
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
			errs.setProgressSize(Message::transform,transforms.size());
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
