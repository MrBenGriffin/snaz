//
// Created by Ben on 2019-03-19.
//
#include <sstream>
#include <utility>
#include <unordered_map>

#include "node/Node.h"
#include "node/Tree.h"
#include "node/Locator.h"
#include "node/NodeFilename.h"

#include "node/Content.h"
#include "node/Taxon.h"
#include "node/Suffix.h"

#include "mt/Driver.h"
#include "support/File.h"
#include "support/Regex.h"
#include "support/Convert.h"
#include "support/Env.h"
#include "support/Timing.h"
#include "support/db/Query.h"

#include "content/Template.h"
#include "content/Layout.h"

#include "Build.h"

namespace node {
	using namespace std;

	Tree Content::editorial("Content");
	unordered_map<size_t,Content> Content::nodes;
	deque<Content *> 	Content::nodeStack;    // current node - used to pass to built-in functions

	Content::Content() : Node(editorial),layoutPtr(nullptr) {}

	void Content::loadTree(Messages& errs, Connection& sql, size_t language,buildKind build) {
//	This does languages, but not layouts / templates as they have not yet been done.
//  It also doesn't do segment content, which depend upon layouts!
//	We just get the layoutID for that.
		editorial.clear();
		nodes.clear();

		Timing &times = Timing::t();
		if (times.show()) { times.set("Content Tree"); }
		if (sql.dbselected() && sql.table_exists(errs, "bldnode") && sql.table_exists(errs, "bldnodelang")) {
			sql.lock(errs, "bldnode n read,bldnodelang l read");
			std::ostringstream str;
			str <<	"select n.id,ifnull(n.p,0) as parent,n.t as tier, n.lnkref as ref,"
		  			"n.layout,n.team,l.navtitle,l.title,l.incdate,l.outdate,"
					"l.editor from bldnode n,bldnodelang l where "
	   				"l.node=n.id and l.language=" << language << " and "
					"l.used " << (build==final ? "in('on','lk')" : "!=''") << " order by n.tw";
			Db::Query *q = nullptr;
			if (sql.query(errs, q, str.str()) && q->execute(errs)) {
				/**
				 *
				 * +----+------+------+--------+------+---------------+-----------+-----------+---------+---------+-----------+---------------------+
				 * | id | par. | tier | layout | team | ref           | navtitle  | title     | incdate | outdate | editor    | tstamp              |
				 * +----+------+------+--------+------+---------------+-----------+-----------+---------+---------+-----------+---------------------+
				 * |  2 |    0 |    1 |      1 |    4 | root          | Root      | Root      |    NULL |    NULL | ejohnson  | 2011-04-12 16:47:05 |
				 * |  3 |    4 |    3 |     25 |    4 | 404           | 404       | 404       |       0 |       0 | aoliver   | 2011-02-17 16:05:45 |
				 * |  4 |    2 |    2 |      1 |    4 | siteResources | Resources | Resources |       0 |       0 | jbuchanan | 0000-00-00 00:00:00 |
				 * |  5 |    2 |    2 |      1 |    6 | siteRoot      | Site Root | Site Root |       0 |       0 | jbuchanan | 0000-00-00 00:00:00 |
				 * +----+------+------+--------+------+---------------+-----------+-----------+---------+---------+-----------+---------------------+
				 **/
				size_t parent = 0;
				size_t active_tw = 1;  //we cannot use the native tw as we will be skipping used..
				auto par = nodes.end();
				while (q->nextrow()) {
					Content contents;
					//id, parent,tier,ref,tw
					contents.common(errs, q, parent, active_tw);
					if(!nodes.empty()) {
						par = nodes.find(parent);
					}
					if(nodes.empty() || par != nodes.end()) {
						::time_t birth,death;

						q->readfield(errs, "layout", contents._layout);
						q->readfield(errs, "team", contents._team);
						q->readfield(errs, "navtitle", contents._shortTitle);
						q->readfield(errs, "title", contents._title);
						q->readfield(errs, "editor", contents._editor);
						q->readTime(errs, "incdate", birth);
						q->readTime(errs, "outdate", death);
						contents._birth.set(birth);
						contents._death.set(death);

						auto ins = nodes.emplace(contents._id, std::move(contents));
						if (ins.second) {
							Content* node = &(ins.first->second);
							editorial.add(errs, node, parent);
						}
					} else {
						active_tw--; //don't want to include the fake tw.
					}
				}
			}
			sql.dispose(q);
			sql.unlock(errs);
			nodes.find(editorial.root()->id())->second.weigh();
		}
		if (times.show()) { times.use(errs, "Content Tree"); }
//		editorial.root()->str(cout);
	}

	void Content::updateBirthAndDeath(Messages& errs, Connection& sql, size_t langId,buildKind) {
		Timing &times = Timing::t();
		if (times.show()) { times.set("Birth/Death Dates"); }
		if (sql.dbselected() && sql.table_exists(errs,"bldnode") && sql.table_exists(errs,"bldnodelang") ) {
			Query* q1 = nullptr;
			Query* q2 = nullptr;
			std::ostringstream death,birth;
			death << "update bldnodelang set used='' where "
				   "(( unix_timestamp() >= outdate) AND (outdate <> 0)) and language=" << langId;
			birth << "update bldnodelang set used='on' where "
		  			"(( unix_timestamp() >= incdate) AND (incdate <> 0)) and language=" << langId;
			sql.lock(errs,"bldnodelang write");
			if(sql.query(errs,q1,death.str())) { q1->execute(errs); }
			if(sql.query(errs,q2,birth.str())) { q2->execute(errs); }
			sql.dispose(q1);
			sql.dispose(q2);
			sql.unlock(errs);
		}
		if (times.show()) { times.use(errs,"Birth/Death Dates"); }
	}

	void Content::updateContent(Messages& errs, Connection& sql, size_t langId,buildKind build) {
		if (build == final) {
			Timing &times = Timing::t();
			if (times.show()) { times.set("Content Versions"); }
			if (sql.dbselected() && sql.table_exists(errs,"bldcontent") && sql.table_exists(errs,"bldcontentv") && sql.table_exists(errs,"bldnodelang") ) {
				Query* q = nullptr;
				std::ostringstream query;
				query << "update bldcontent c,bldcontentv v,bldnodelang l set c.pcontent=v.content,c.editor=v.editor,c.moddate=v.tstamp "
						 "where l.used='on' and v.active='on' and l.node=c.node and l.language=c.language and v.node=c.node and v.language=c.language "
						 "and v.segment=c.segment and v.id=c.version and l.language=" << langId;
				sql.lock(errs,"bldcontent c write,bldcontentv v read,bldnodelang l read");
				if(sql.query(errs,q,query.str())) { q->execute(errs); sql.dispose(q); }
				sql.unlock(errs);
			}
			if (times.show()) { times.use(errs,"Content Versions"); }
		}
	}

	const Node* Content::current() const {
		if (!nodeStack.empty()) {
			return nodeStack.back();
		} else {
			return editorial.root();
		}
	}
	bool   Content::get(Messages& errs,boolValue field) const {
		return false;
	};
	size_t Content::get(Messages& errs,uintValue field) const {
		size_t result=0;
		switch(field) {
			case team: break;
			case layout: break;
			case page: break;
			case templates: result=0; break;
		}
		return result;
	};
	string Content::get(Messages& errs,textValue field) const {
		string result;
		switch(field) {
			case title: break;
			case shortTitle: break;
			case comment: break;
			case baseFilename: result=baseFileName; break;
			case scope: break;
			case classCode: break;
			case synonyms: break;
			case keywords: break;
			case description: break;
			case fileSuffix: break;
			case script: break;
			case editor: break;
		}
		return result;
	};
	Date   Content::get(Messages& errs,dateValue field) const {
		Date result;
		switch(field) {
			case modified: break;
			case birth: break;
			case death: break;
		}
		return result;
	};

	void Content::setLayouts(Messages &errs) {
		Timing& times = Timing::t();
		if (times.show()) { times.set("setLayouts"); }
		for (auto i : editorial.twNodes) {
			Content& node = nodes[i.second->id()];
			node.layoutPtr = content::Layout::get(errs,node._layout);
		}
		if (times.show()) { times.use(errs,"setLayouts"); }
	}

	void Content::reset() {
	}

	const Content* Content::content(Messages& errs, size_t id, bool silent) {
		const Content* result =  nullptr;
		auto found = nodes.find(id);
		if(found != nodes.end()) {
			result = &(found->second);
		} else {
			if(!silent) {
				ostringstream err;
				err << "There is no editorial node available with id " << id;
				errs << Message(range,err.str());
			}
		}
		return result;
	}

	const Content* Content::root() {
		return dynamic_cast<const Content*>(editorial.root());
	}

	const Node* Content::node(Messages& errs, size_t id, bool silent) const {
		return content(errs,id,silent);
	}

//-------------------------------------------------------------------
// All builds starts here, and then any single node build goes to compose.
	void Content::generate(Messages& errs,buildType build,buildKind kind,size_t langID,size_t techID) const {
		Build &core = Build::b();
//		ostringstream msg;
//		msg << string(build) << ": " << _id << ";" << _tw << ";" << _ref;
//		errs << Message(info,msg.str());
//		static unordered_map<size_t,Content>  nodes;
		switch(build) {
			case Full:
				if(core.user.check(build) && this == root()) {
					generate(errs,Branch,kind,langID,techID);
				} else {
					errs << Message(security,"Not root or not allowed to full build.");
				} break;
			case Branch:
				if(core.user.check(_team,build)) {
					nodes[_id].compose(errs,kind,langID,techID);
					generate(errs,Descendants,kind,langID,techID);
				} else {
					errs << Message(security,"Not allowed to branch build here.");
				} break;
			case Descendants:
				if(core.user.check(_team,build)) {
					for(auto* node : children) {
						const Content* child = dynamic_cast<const Content *>(node);
						if(child != nullptr && child->layoutPtr != nullptr && child->layoutPtr->buildPoint) {
							child->generate(errs,Branch,kind,langID,techID);
						}
					}
				} else {
					errs << Message(security,"Not allowed to descendant build here.");
				} break;
			case Single:
				if(core.user.check(_team,build)) {
					nodes[_id].compose(errs,kind,langID,techID);
				} else {
					errs << Message(security,"Not allowed to build this node.");
				} break;
		}
	}

	void Content::compose(Messages& errs,buildKind kind,size_t langID,size_t techID) {
		// To get here, this node generates files
		ostringstream msg; msg << "Composing " << _ref;
		errs << Message(info,msg.str());
		content::Layout::layouts[layoutPtr->id].compose(errs,*this,kind,langID,techID);
	}
	/**
			auto nmi = templateList.find(tid);
			if (nmi != templateList.end()) {
				curtemplate = nmi->second.second; // My order
				nodeStack.push_back(this); //tid
				ostringstream tname;
				tname << "Suffix " << tid;
				ostringstream result;
				string suff = mt::Driver::expand(errs, curtemplate.suffix, tname.str());
				nodeStack.pop_back();
				suffixes[current_page]=suff;
				if (errs.verbosity() > 6) {
					text << "Node " << idStr << " page " << current_page << " has suffix " << suff;
					errs << Message(info, text.str());
					text.str("");
				}
				//We need to find the ancestor suffix so that any links we create can deal with the post-processed versions!
				const Node* x = Suffix::suffixes.byRef(errs, suff);
				if (x == nullptr) {
					string fname = filenames[current_page];
					filename->getFullName(fname, suff);
					ffilename[current_page] = fname;
				} else {
//						const Node *byPath(Messages &,Locator&, string::const_iterator, string::const_iterator) const;
//						Node *vx = x->nodebypath(errs, Suffix::finalSuffix.begin(), Suffix::finalSuffix.end());
					Locator suffixLocator(x); string suffixTop(Suffix::finalSuffix);
					const Node *vx = Suffix::suffixes.byPath(errs,suffixLocator,suffixTop.begin(), suffixTop.end());
					if (vx == nullptr) {
						errs << Message(error, "Error of some sort in suffix table");
					} else {
						suff = vx->get(errs,fileSuffix);
					}
					string fname = filenames[current_page];
					filename->getFullName(fname, suff);    //This now works!  Amazing.
					if (errs.verbosity() > 6) {
						text << "Node " + idStr + " page " << current_page << " is set with filename " << fname;
						errs << Message(info, text.str());
						text.str("");
					}
					ffilename[current_page] = fname;
				}
				current_page++;
			} else {
				Env env = Env::e();
				text << "Template " << tid << " at Node " << id() << ",Technology " << build.tech()
					 << " referenced but does not exist.";
				errs << Message(error, text.str());
				text.str("");
			}

***/
//-------------------------------------------------------------------
//This is the beginning of the node build.
/**
	void Content::gettextoutput(Messages &errs) {
		Env env = Env::e();
		Build build = Build::b();
		Timing timing = Timing::t();
		string in_s, baseurl_a, baseurl_s, langtech_s;
		timing.set('N');
		env.get("LTPATH", langtech_s);
		bool inTeam = build.user.mayTeamEdit(get(errs,team));
		if (errs.verbosity() > 0) {
			ostringstream line_oss;
			if (inTeam) {
				baseurl_a = env.baseUrl(Editorial);
				if (Regex::available(errs)) {
					string editor;
					const string pattern = "^([^0]*)0([^0]*)0([^0]*)$";
					env.get("RS_BUILDNODEPATH", editor, "/en/nnedit.mxs?0&d&0"); //if none, keep default.
					std::ostringstream sub;
					sub << "\\1" << this->id() << "\\2" << build.lang() << "\\3";
					Regex::replace(errs, pattern, sub.str(), editor);
					line_oss << editor;
				} else {
					line_oss << "/en/nnedit.mxs?" << this->id() << "&d&" << build.lang();
				}
			}
			string Title = get(errs,title);
			if (Title.empty()) Title = ref();
			baseurl_s = env.baseUrl(buildArea(build.current())); //current()
			if (Title.length() > 60) {
				Title = Title.substr(0, 60);
				Title = Title.append("...");
			}
			bool did_url = false;
			for (size_t i = 0; i < get(errs,templates) && !did_url; i++) {
//				if (suffixes[i] != "XXX") {
//					ostringstream line;
//					if (inTeam) {
//						line << "<a href='" << baseurl_a << line_oss.str() << "'>" << "*" << "</a>";
//					} else {
//						line << "+ ";
//					}
//					line << this->id() << " - <a href='" << baseurl_s << "/" << langtech_s << ffilename[0] << "'>"
//						 << Title << "</a>";
//					errs << Message(even, line.str());
//					did_url = true;
//				}
			}
			if (!did_url) {
				ostringstream line;
				if (inTeam) {
					line << "<a href='" << baseurl_a << line_oss.str() << "'>" << "*" << "</a>";
				} else {
					line << "+ ";
				}
				line << this->id() << " - " << Title;
				errs << Message(struc, line.str());
			}
		}
		//--
		string bk;
		for (size_t i = 0; i < get(errs,templates); i++) {
			build.setPage(i);
			if (content::Template::_show && get(errs,templates) > 1) {
				ostringstream text;
				text << " Template (" << i + 1 << ")";
				errs << Message(info, text.str());
			}

			string templatename("Undefined Template");
//			size_t templateId = tplates[i];
//			content::Template curtemplate;
//			auto nmi = templateList.find(templateId);
//			if (nmi != templateList.end()) {
//				curtemplate = nmi->second.second;  // My order
//				templatename = "[" + nmi->second.first + "]";  // The template's name
//				build.setSuffix(curtemplate.suffix);
//				in_s = curtemplate.str;  //Starter for template
//				bk = curtemplate.br; //Template's own break.
//			} else {
//				ostringstream line;
//				line << "Template " << templateId << " at Node " << this->id() << ",Technology " << build.tech()
//					 << " referenced but does not exist.";
//				errs << Message(warn, line.str());
//				build.setSuffix("");
//				in_s = "";
//				bk = "";
//			}

			//TODO:: Finish template generation!!

		Macro::environ TemplateEnv = macro::topEnv();
		if ( bk.length() > 0 ) {
			macro::pushEnv({false,"\n","\t"}); //do not rembr, \n  = newline, tabs are tabs.
			nodeStack.push_back(this);
			string crlfName = "CR:" + to_string(String::Digest::hash(bk));
			TemplateEnv.crlf = macro::process(Logger::log,crlfName,bk);
			nodeStack.pop_back();
			macro::popEnv();
		}
		macro::pushEnv(TemplateEnv);
		{
			if (in_s.length() > 0) {
				string outval;
				if (in_s.length() > 0) {						//fandr may remove the last char!
					nodeStack.push_back(this);
					string tnumStr = to_string(tnum);
					outval = macro::process(Logger::log,"Template " + tnumStr,in_s);
					nodeStack.pop_back();
				}
				outputtofile(i,outval);
			}
			if (showTemplate && get(errs,templates) > 1 && errs.verbosity()  > 0) {
				*Logger::log << O; //end of show template.
			}
			in_s.clear();
			bk.clear();
		}
		macro::popEnv();

		}
		//TODO:: And the rest.

	//-- This used to be in genfiles, but then happened once for each file, rather than one for each Node.
	if (dbc->dbselected() && dbc->table_exists("bldnode") ) {
		ostringstream qstr;
		qstr << "UPDATE LOW_PRIORITY bldnode SET filename='" << this->Ffilename(0) << "' WHERE id=" << this->id();
		Query* q = dbc->query(qstr.str());
		if (! q->execute() ) {
			errs << Message(error << "Node::gettextoutput(): DB Error" );
		}
		dbc.dispose(q);
	}
	resetNodePersistance();	//Delete *vars.
	if (showTiming && errs.verbosity()  > 3) {
		errs << Message(timing << Log::Ntime << " build time for node: " << linkref() );
	}
	if (errs.verbosity()  > 0) *Logger::log << O; //end of node.
	}
**/
//-----------------------------------------------------------------
// We shall generate each file here, and then post-process it until its suffix is at A2 of the processor tree.
// We use A2, because we are allowed multiple suffixes!
//-----------------------------------------------------------------
/**
	void Content::outputtofile(size_t page, string &out) {
		//TODO:: Finish outputtofile.
		File tmpScrp(scrDir);
		File tmpFil1(tmpDir);
		File tmpFil2(tmpDir);
		string tfilename;
		string suff;
		bool postprocessed=false;
		suff = Suffix(page);									// Starter position.
		if (suff != "XXX") {
			tfilename =  filenames[page];
			if (suff == "scssi") {					// Sass internal pre-process
				bld->scssifiles.push_back(tfilename);
				suff = "scss";									// flipped.
			}
			tfilename.append(suff);
			tmpFil1.setFileName(tfilename);					// Start off with the source..
			if (bld->showFiling) {
				size_t tflen =  out.length();
				errs << Message(info << "About to output file " << tfilename << " having " << (size_t)tflen << " bytes" ) ;
			}
			try {
				ofstream outFile(tmpFil1.output().c_str());
				outFile << out;
				outFile.close();
				chmod(tmpFil1.output().c_str(),S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
			}	//create the file!
			catch(...) {
				errs << Message(error << "Failed to create file (" << tmpFil1.output() << ")" ) ;
			}
			out.clear();
			Node *x = roots->nodebylinkref(suff);		// Find the post-processor NodeSuffix.  We have the file for this now.
			if (x != nullptr) {								// We do have a post-processing entry, so lets use it.
				Node* y = x->nodebypath(finalsuffix.begin(),finalsuffix.end());
				while ( x != y && x != nullptr) {					// While x is not y, and x is not null, use the scriptpath for x
					if (!postprocessed && showFiling) {
						errs << Message(info << "for post-processing using: " << suff << " ";
						postprocessed = true;
					}
					tmpScrp.setFileName(x->script());		// Get the script name for x
					Node *z = x->parent();
					if (  x->batch() == 1  ) {							// this is to be batch-processed
						if (bld->showFiling) {
							*Logger::log << " as one of a batch." );
						}
						bld->batchscripts.insert(storage_map_type::value_type(tmpScrp.output(), bld->tmpDir.output() ));
					} else {
						string sfilename =  filenames[page] + x->suffix();
						tmpFil1.setFileName(sfilename);
						string dfilename =  filenames[page] + z->suffix();
						tmpFil2.setFileName(dfilename);
						if (bld->showFiling) *Logger::log << " -> "<< z->suffix();
						ostringstream command;
						string build=bld->dev ? "dev " : "live "; // + Environment::Path();
						command << tmpScrp.output() << " " << tmpFil1.output() << " " << bld->pagePath.output(true) << " " << build << " > " << tmpFil2.output();
						if (bld->showFiling) errs << Message(info << " About to run command:" << command.str() );
						MLog::stream errs("exec");
						FileUtils::exec(command.str(),errs);
						errs.str(Logger::log);
						if ( z->exec() == 1 ) {
							if (bld->showFiling) *Logger::log << " (Adding exec to:" << tmpFil2.output() << ")";
							if (bld->showFiling) errs << Message(info << " About to adjust permissions for execute." );
							chmod(tmpFil2.output().c_str(),S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH );
						}
						tmpFil1.removeFile();
					}
					x = z;
				}
			}
			if (bld->showFiling && !postprocessed)  errs << Message(info << " with no post-processing";
			if (bld->showFiling) errs << Message(end;
		}
	}
**/

}
