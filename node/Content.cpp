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

#include "Build.h"
#include "Content.h"

namespace node {
	using namespace std;

	Tree Content::editorial("Content");
	unordered_map<size_t,Content> Content::nodes;
	segmentIdMap		Content::segmentIDs;			//A quick reference to the segment IDs/types by name
	invRefMap			Content::segmentNames;		//A quick reference to the segment names by ID..
	idTemplateMap    	Content::templateList;
	idIdListMap        	Content::layoutList;               //Stores a layout-id -> template-list structure
	refMap            	Content::layoutRefs;               //Layout names -> ids
	invRefMap        	Content::layoutNames;              //Layout ids->names
	bool            	Content::_showTemplates = false;   //show templates in logs..
	deque<Content *> 	Content::nodeStack;    // current node - used to pass to built-in functions

	Content::Content() : Node(editorial) {}

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

	void Content::loadGlobal(Messages &errs, Connection &sql) {
		//Set Segment Maps.
		/*
		 Segment types (as declared in blddefs) and what happens to them.
		 name	XML encoded, expanded
		 SEG**	√		√
		 SEGXM	X		√
		 SEGRE	√		X
		 SEGRW	X		X
		 */
		Timing &times = Timing::t();
		if (times.show()) { times.set("Content Init"); }

		if (sql.dbselected() && sql.table_exists(errs,"bldsegment") && sql.table_exists(errs,"blddefs") ) {
			string select = "select s.name,s.id,d.type from bldsegment s,blddefs d where left(d.type,3)='SEG' and d.code=s.type order by s.id";
			Query* q = nullptr;
			if (sql.query(errs,q,select) && q->execute(errs)) {
				while(q->nextrow()) {
					size_t f_uid(0);
					string f_name,f_type;
					q->readfield(errs,"id",f_uid);		//discarding bool.
					q->readfield(errs,"name",f_name);	//discarding bool.
					q->readfield(errs,"type",f_type);	//discarding bool.
					segmentNames.emplace(f_uid,f_name);
					segmentIDs.emplace(f_name,pair<size_t,string>(f_uid,f_type));
				}
			}
			sql.dispose(q);
		}
		if (times.show()) { times.use(errs,"Content Init"); }
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
		auto i = _tree->twNodes.begin();
		size_t twms = _tree->twNodes.size();
		if (errs.verbosity() > 6) {
			ostringstream message;
			message << "Layouts iterator set for " << twms << " Nodes.";
			errs << Message(info, message.str());
		}
		while (i != _tree->twNodes.end()) {
			i->second->get(errs,layout);
			i++;
		}
		if (errs.verbosity() > 5) {
			errs << Message(info, "Layouts are set");
		}
	}

	void Content::reset() {
		//reset templates etc. after each technology.
	}

	const Content* Content::content(Messages& errs, size_t id, bool silent) const {
		const Content* result =  nullptr;
		auto found = nodes.find(id);
		if(found != nodes.end()) {
			result = &(found->second);
		} else {
			if(!silent) {
				ostringstream err;
				err << "There is no editorial node with id " << id;
				errs << Message(range,err.str());
			}
		}
		return result;
	}

	const Node* Content::node(Messages& errs, size_t id, bool silent) const {
		return content(errs,id,silent);
	}


//-------------------------------------------------------------------
// load template list for the (content) node
// It should probably be part of nodeval
	void Content::doTemplates(Messages &errs) {
		Build build = Build::b();
		if (errs.verbosity() > 6) errs << Message(info, "Loading template list for node " + idStr);
		Env &env = Env::e();
		if (_id != 0) {  //don't do anything for root
			vector<size_t> templateids;
			if (errs.verbosity() > 6) {
				ostringstream message;
				message << "Node " << idStr << " uses layout " << get(errs,layout);
				errs << Message(info, message.str());
			}
			auto nmi = layoutList.find(get(errs,layout));
			if (nmi != layoutList.end()) {
				templateids = nmi->second; // My order
			} else {
				ostringstream text;
				text << "Found a Layout " << get(errs,layout) << " at Node " + idStr + " for Technology " << build.tech()
					 << " with a null templatelist.";
				errs << Message(error, text.str());
				return;
			}
			if (errs.verbosity() > 6) {
				size_t tids = templateids.size();
				ostringstream text;
				text << "Node " << idStr << ", Layout " << get(errs,layout) << " has a template list with " << (size_t) tids
					 << " template(s).";
				errs << Message(info, text.str());
			}
			if (!templateids.empty()) {    // template list not defined //
				// get template list and count number of templates in list //
				tplates = templateids;
				filenames.resize(templateids.size(), "");     //Source filename (no suffix anymore)
				suffixes.resize(templateids.size(), "");     //
				ffilename.resize(templateids.size(), "");    //Makes space for the filenames & ffilenames.
				if (errs.verbosity() > 6) {
					ostringstream text;
					text << "Node " + idStr + " record has been resized for " << templateids.size() << " templates.";
					errs << Message(info, text.str());
				}
				NodeFilename *nfilename = NodeFilename::createInstance();
				if (errs.verbosity() > 6) {
					errs << Message(info, "Node " + idStr + " had it's filenaming instance created.");
					if (this == _tree->root()) {
						errs << Message(info, "Node " + idStr + " is the root content node.");
					} else {
						ostringstream text;
						text << "Node " << idStr << " is NOT the root content node. The root content node id is:"
							 << _tree->root()->id();
						errs << Message(info, text.str());
					}
				}
				if (this == _tree->root()) {
					ostringstream text;
					text << "Node id: " + idStr + ", basefilename: " << baseFileName << ", rootfilename: " << rootFilename;
					errs << Message(info, text.str());
					baseFileName = rootFilename; //Set basefilename
					errs << Message(info, "basefilename: " + baseFileName);
				}
				nfilename->set(this);
				string thisbasename;
				nfilename->getBaseName(thisbasename);
				if (errs.verbosity() > 6)
					errs << Message(info, "Node " + idStr + " Basename is set to:" + thisbasename);
				baseFileName = thisbasename;
				if (errs.verbosity() > 6) errs << Message(info, "Node " + idStr + " About to add the pages. ");
				for (size_t i = 0; i < templateids.size(); i++) {
					addpage(errs, nfilename);        //<--- We are adding the pages here..
				}
				if (errs.verbosity() > 6) errs << Message(info, "Node " + idStr + " Finished adding pages. ");
				delete nfilename;
			}
		}
	}

	//select layout,technology,templatelist from bldlayouttechs;
	void Content::loadLayouts(Messages &errs, Connection &dbc, size_t techID) {
		//loads layouts and templates.
		ostringstream text;
		Env &env = Env::e();
		Timing &times = Timing::t();
		string techName = Build::b().techName();
		if (times.show()) { times.set("Load " + techName); }
		layoutList.clear();
		layoutRefs.clear();
		layoutNames.clear();
		templateList.clear();
		if (dbc.dbselected() && dbc.table_exists(errs, "bldlayout") && dbc.table_exists(errs, "bldlayouttechs") &&
			dbc.table_exists(errs, "bldtemplate")) {
			ostringstream qstr;
			dbc.lock(errs, "bldlayout l read,bldlayouttechs x read,bldtemplate t read"); //everyone can read only..
			qstr << "select layout,templatelist,name from bldlayout l, bldlayouttechs x where x.layout=l.id"
		   			" and technology=" << techID;
			auto *q = dbc.query(errs, qstr.str());
			if (q->execute(errs)) {
				string f_layout_id, f_templatelist, f_name;
				while (q->nextrow()) {
					q->readfield(errs, "layout", f_layout_id);             //discarding bool.
					q->readfield(errs, "templatelist", f_templatelist);    //discarding bool.
					q->readfield(errs, "name", f_name);                    //discarding bool.
					vector<size_t> templateIDlist;
					size_t id = natural(f_layout_id);
					tolist(templateIDlist, f_templatelist);
					layoutList.insert({id, templateIDlist});
					layoutRefs.insert({f_name, id});
					layoutNames.insert({id, f_name});
				}
				qstr.str("");
				qstr << "select distinct(t.id) as id,t.templatemacro,t.suffix,t.break,t.comment from bldtemplate t,bldlayouttechs x where ";
				qstr << "find_in_set(t.id,x.templatelist) != 0 and x.technology=" << techID;
				q->setquery(qstr.str());
				if (q->execute(errs)) {
					string f_id, f_comment;
					while (q->nextrow()) {
						FileTemplate n;
						q->readfield(errs, "id", f_id);                //discarding bool.
						q->readfield(errs, "templatemacro", n.str);    //discarding bool.
						q->readfield(errs, "suffix", n.suffix);        //discarding bool.
						q->readfield(errs, "break", n.br);             //discarding bool.
						q->readfield(errs, "comment", f_comment);      //discarding bool.
						size_t id = natural(f_id);
						templateList.insert({id, pair<string, FileTemplate>(f_comment, n)});
					}
				}
			}
			dbc.unlock(errs);
		}
		if (times.show()) { times.get(errs,"Load " + techName); }
	}

	void Content::addpage(Messages &errs, NodeFilename *filename) {
		ostringstream text;
		Build &build = Build::b();
		if (this == _tree->root()) {
			return;
		} else {
			size_t current_page = get(errs,page); //current page we are creating..
			if (errs.verbosity() > 6) {
				text << "Node " << idStr << " currently on page " << current_page << " page of " << get(errs,templates);
				errs << Message(info, text.str());
				text.str("");
			}
			string emptyname;

			if (current_page >= get(errs,templates)) {
				text << " Attempt to create pages for too few templates; " << current_page << " requested; "
					 << (size_t) get(errs,templates) << " defined. Will leave it empty";
				errs << Message(warn, text.str());
				return;
			}
			filename->setPageName(baseFileName, current_page);
			filename->getFullName(filenames[current_page], emptyname);
			FileTemplate curtemplate;
			size_t tid = tplates[current_page];
			if (errs.verbosity() > 6) {
				ostringstream text;
				text << "Node " << idStr << " page " << current_page << " has template ID " << tid;
				errs << Message(info, text.str());
				text.str("");
			}

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
		}
		if (errs.verbosity() > 6) errs << Message(info, "Node " + idStr + " Finished adding page ");
	}

//-------------------------------------------------------------------
//This is the beginning of the node build.
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
				if (suffixes[i] != "XXX") {
					ostringstream line;
					if (inTeam) {
						line << "<a href='" << baseurl_a << line_oss.str() << "'>" << "*" << "</a>";
					} else {
						line << "+ ";
					}
					line << this->id() << " - <a href='" << baseurl_s << "/" << langtech_s << ffilename[0] << "'>"
						 << Title << "</a>";
					errs << Message(even, line.str());
					did_url = true;
				}
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
			if (_showTemplates && get(errs,templates) > 1) {
				ostringstream text;
				text << " Template (" << i + 1 << ")";
				errs << Message(info, text.str());
			}

			string templatename("Undefined Template");
			size_t templateId = tplates[i];
			FileTemplate curtemplate;
			auto nmi = templateList.find(templateId);
			if (nmi != templateList.end()) {
				curtemplate = nmi->second.second;  // My order
				templatename = "[" + nmi->second.first + "]";  // The template's name
				build.setSuffix(curtemplate.suffix);
				in_s = curtemplate.str;  //Starter for template
				bk = curtemplate.br; //Template's own break.
			} else {
				ostringstream line;
				line << "Template " << templateId << " at Node " << this->id() << ",Technology " << build.tech()
					 << " referenced but does not exist.";
				errs << Message(warn, line.str());
				build.setSuffix("");
				in_s = "";
				bk = "";
			}

			//TODO:: Finish template generation!!
/*
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
*/
		}
		//TODO:: And the rest.
/*
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
	*/
	}

//-----------------------------------------------------------------
// We shall generate each file here, and then post-process it until its suffix is at A2 of the processor tree.
// We use A2, because we are allowed multiple suffixes!
//-----------------------------------------------------------------
	void Content::outputtofile(size_t page, string &out) {
		//TODO:: Finish outputtofile.

		/**
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
		 **/
	}

// -----------------------------------------------------------------------------------
// generate basic text output for all nodes below Node n in the Node tree */
// and 'n's direct ancestors for link generation */
	void Content::generateOutput(Messages &errs, int level) {
		if (level >= 0 && _parent != nullptr) {// generate output for all direct ancestors
			Content* par = const_cast<Content*>(dynamic_cast<const Content*>(_parent));
			par->generateOutput(errs, level + 1);
		}
		if (this != _tree->root()) {
			gettextoutput(errs);
		}
		if (level <= 0) {    // generate output for all descendents
			for (auto* childNode : children) {
				Content *aChild = const_cast<Content*>(dynamic_cast<const Content *>(childNode));
				aChild->generateOutput(errs, level - 1);
			}
		}
	}

// -----------------------------------------------------------------------------------
// generate basic text output for all nodes below Node n in the Node tree */
	void Content::generateBranch(Messages &errs, int level) {
		if (this != _tree->root()) gettextoutput(errs);
		if (level <= 0) {    // generate output for all descendents
			for (auto* childNode : children) {
				Content *aChild = const_cast<Content*>(dynamic_cast<const Content *>(childNode));
				aChild->generateBranch(errs, level - 1);
			}
		}
	}


}
