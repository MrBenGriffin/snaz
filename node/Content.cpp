//
// Created by Ben on 2019-03-19.
//
#include <fstream>
#include <sstream>
#include <limits>
#include <utility>
#include <unordered_map>

#include "node/Node.h"
#include "node/Tree.h"
#include "node/Locator.h"
#include "node/NodeFilename.h"

#include "node/Content.h"
#include "node/Taxon.h"
#include "node/Suffix.h"
#include "node/Metrics.h"

#include "mt/Driver.h"
#include "support/File.h"
#include "support/Regex.h"
#include "support/Convert.h"
#include "support/Encode.h"
#include "support/Env.h"
#include "support/Timing.h"
#include "support/db/Query.h"

#include "content/Template.h"
#include "content/Layout.h"
#include "content/Segment.h"

#include "BuildUser.h"
#include "Build.h"

namespace node {
	using namespace std;

	Tree Content::editorial("Content");
	unordered_map<size_t,Content> Content::nodes;
//	deque<const Content *> 	Content::nodeStack;    // current node - used to pass to built-in functions

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
						contents._death.set(death == 0 ? std::numeric_limits<::time_t>::max() : death);

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
			if(nodes.size() > 0) {
				nodes.find(editorial.root()->id())->second.weigh();
			}
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

	bool Content::get(Messages&,boolValue) const {
		return false;
	};
	size_t Content::get(Messages&,uintValue field) const {
		size_t result=0;
		switch(field) {
			case team: result = _team; break;
			case uintValue::layout: result = _layout; break;
			case templates: result= finalFilenames.size(); break;
		}
		return result;
	};
	string Content::get(Messages&,textValue field) const {
		string result;
		switch(field) {
			case title: 		result= _title; break;
			case shortTitle: 	result= _shortTitle; break;
			case comment: 		result= _comment; break;
			case baseFilename: 	result=baseFileName; break;
			case scope: break;
			case classCode: break;
			case synonyms: break;
			case keywords: break;
			case description: break;
			case fileSuffix: break;
			case script: break;
			case editor: result = _editor; break;
		}
		return result;
	};
	Date   Content::get(Messages&,dateValue field) const {
		switch(field) {
			case modified: return Date();
			case birth: return _birth;
			case death: return _death;
		}
	};

	void Content::setLayouts(Messages &errs) {
		Env& env = Env::e();
		Timing& times = Timing::t();
		if (times.show()) { times.set("setLayouts"); }

		Path unix = env.unixDir(Built);
		env.doLangTech(unix);
		unix.makeDir(errs,true);

		auto siteDir =  env.siteDir(Built);
		env.doLangTech(siteDir);

		for (auto i : editorial.twNodes) {
			Content& node = nodes[i.second->id()];
			node.layoutPtr = content::Layout::get(errs,node._layout);
			node.finalFilenames.clear();	//final filenames - in page order.
			if(node.layoutPtr != nullptr) {
				size_t fileNum = 0;
				for(auto *tmp : node.layoutPtr->templates) {
					auto* t = const_cast<content::Template*>(tmp);
					const node::Suffix* initialSuffix = t->suffix;
					if(initialSuffix) {
						const node::Suffix* finalSuffix = initialSuffix->last;
						ostringstream base;
						std::string refName = node._ref;
						Support::fileEncode(refName);
						base << refName;
						if(fileNum > 0) {
							base << "_" << fileNum;
						}
						Support::File file(siteDir,base.str());
						file.setExtension(finalSuffix->ref());
						node.finalFilenames.push_back(file);
					}
					fileNum++;
				}
			}
		}
		if (times.show()) { times.use(errs,"setLayouts"); }
	}

	void Content::reset() {
	}

	const Content* Content::content(Messages& errs, size_t id, bool silent) const {
		const Content* result =  nullptr;
		if(id == 0) {
			result = this;
		} else {
			auto found = nodes.find(id);
			if (found != nodes.end()) {
				result = &(found->second);
			} else {
				if (!silent) {
					ostringstream err;
					err << "There is no editorial node available with id " << id;
					errs << Message(range, err.str());
				}
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

	void Content::count(const BuildUser& user,buildType type,set<size_t>& counter) {
		switch (type) {
			case Single:
				if (user.check(_team, type)) {
					counter.insert(_id);
				}
			break;
			case Full:
				if (user.check(type) && this == root()) {
					count(user,Branch, counter);
				}
			break;
			case Branch:
				if (user.check(_team, type)) {
					counter.insert(_id);
					count(user,Descendants, counter);
				}
			break;
			case Descendants:
				if (user.check(_team, type)) {
					for (auto *node : children) {
						get(node->id()).count(user,Branch, counter);
					}
				}
			break;
		}
	}

	//-------------------------------------------------------------------
// All builds starts here, and then any single node build goes to compose.
	void Content::generate(Messages& errs,buildType build) {
		Build &core = Build::b();
		switch(build) {
			case Single:
				if(core.user.check(_team,build)) {
					compose(errs);
				} else {
					errs << Message(security,"Not allowed to build this node.");
				} break;
			case Full:
				if(core.user.check(build) && this == root()) {
					generate(errs,Branch);
				} else {
					errs << Message(security,"Not root or not allowed to full build.");
				} break;
			case Branch:
				if(core.user.check(_team,build)) {
					compose(errs);
					generate(errs,Descendants);
				} else {
					errs << Message(security,"Not allowed to branch build here.");
				} break;
			case Descendants:
				if(core.user.check(_team,build)) {
					for(auto* node : children) {
						auto& child = get(node->id());
						if(child.layoutPtr != nullptr && child.layoutPtr->buildPoint) {
							child.generate(errs,Branch);
						} else {
							errs << Message(channel::node,child.ids(),1.00L); //we included it, but will skip it here.
						}
					};
				} else {
					errs << Message(security,"Not allowed to descendant build here.");
				} break;
		}
	}

	const std::string Content::filename(Messages& errs,size_t page) const {
		string value = "";
		if(page < finalFilenames.size()) {
			auto& file = finalFilenames[page];
			value = file.getFileName(); // maybe if we had a FROM we could work out relative/absolute...
		} else {
			errs << Message(range,"Requested page number too high for this node");
		}
		return value;
	}

    void Content::compose(Messages& errs) {
		Env& env = Env::e();

		Timing& times = Timing::t();
		times.set('n'); // set node timer start.
		ostringstream msg; msg << "Node ID " << idStr << " `" << _ref << "` ";
		string name(msg.str());
		errs.push(Message(info,name));
        Metrics current;
        current.nodeStack.push_back(this);
        current.current = this;
        current.page = 0;
		mt::mstack context;
		mt::Instance instance(&current);
		Path destination = env.unixDir(Built);
//		destination.makeDir(errs,true);
		context.emplace_back(make_pair(nullptr,&instance)); //Make the carriage.
		long double fileCount = layoutPtr->templates.size();
		if(fileCount > 0.0L) {
			long double progressValue(1.0L / fileCount);  //eg 1/2 for two files.
			for (auto* t : layoutPtr->templates) {
				string filename= _ref + " not a file";
				std::ostringstream content;
				if(!t->code.empty() && !finalFilenames.empty()) {
					current.currentTemplate = t;
//					filename = finalFilenames[current.page];
					File file(finalFilenames[current.page]);
					file.makeAbsoluteFrom(destination);
//					outPath.head(filebits.dir,errs); //force append (even if dir starts with root)
					string filepath = file.output(true);
					// http://edit-preview.redsnapper.net/en/testl.php
					// errs << Message(info,file);
					errs.reset();
					mt::Wss::push(&(t->nl)); //!!! another global.. need to add to the metrics above.
					t->code.expand(errs, content, context); //no context here...
					mt::Wss::pop();
					try {
						std::ofstream outFile(filepath);
						outFile << content.str();
						outFile.close();
						chmod(filepath.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
					}    //create the file!
					catch (...) {
						errs << Message(error, "Failed to create file " + filepath);
					}
				}
				current.page++;
				errs << Message(channel::file,filename,progressValue); // done as a proportion.
			}
		} else {
			errs << Message(channel::node,"No files to output",1.0L);  // done as a proportion.
		}
 		context.pop_back();
        current.nodeStack.pop_back();
        mt::Internal::reset("*"); //reset all storage belonging to the node build.
		times.get(errs,'n',name);
		errs.pop();
    }
}
