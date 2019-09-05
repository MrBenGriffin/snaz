//
// Created by Ben on 2019-03-19.
//
#include <sstream>

#include "support/db/Query.h"
#include "support/Timing.h"
#include "support/Env.h"
#include "support/File.h"
#include "support/Definitions.h"
#include "support/Message.h"
#include "support/Sass.h"
#include "support/Fandr.h"

#include "mt/Definition.h"
#include "mt/Driver.h"

#include "node/Tree.h"
#include "node/Node.h"
#include "node/Suffix.h"

#include "Build.h"

using namespace std;

namespace node {

	Tree Suffix::suffixes("Suffixes");
	unordered_map<size_t,Suffix> Suffix::nodes;
	unordered_map<string,const Suffix*> Suffix::refs;
	unordered_set<Batch, hashBatch> Suffix::batches;	// Batch files.
	unordered_set<File, hashFile> Suffix::scssFiles;	 			// SCSS files.

	Suffix::Suffix() : Node(suffixes),last(nullptr),_output(true) {}
	
	Suffix::Suffix(Suffix && rhs) noexcept : Node(suffixes) {
		_tree = rhs._tree;
		_parent = rhs._parent;
		_id = rhs._id;
		_tw = rhs._tw;
		_tier = rhs._tier;
		_sibling = rhs._sibling;
		_weight = rhs._weight;
		_ref=std::move(rhs._ref);
		idStr=std::move(rhs.idStr);
		_comment=std::move(rhs._comment);
		children=std::move(rhs.children);

//		(Node(&rhs));
		last = rhs.last;
		_title=std::move(rhs._title);
		_script=std::move(rhs._script);
		_output = rhs._output;
		_terminal = rhs._terminal;
		_exec = rhs._exec;
		_batch = rhs._batch;
		_macro = rhs._macro;
		code.adopt(rhs.code); 	//Parsed newline.
	}

	void Suffix::loadTree(Messages& errs, Connection& sql,size_t,buildKind) {
		size_t baked_tw = 0;  //we won't be incrementing this as we can use the native tw. But we need to declare it.
		Timing& times = Timing::t();
		if (times.show()) { times.set("Load Suffixes"); }
		if ( sql.dbselected() && sql.table_exists(errs,"bldscript") ) {
			sql.lock(errs,"bldscript read");
			std::ostringstream str;
			str << "select id,ifnull(p,0) as parent,tw,nc-tw as size,t as tier,"
				   "suffix as ref,name as title,comment,ex='on' and t=2 as macro,t=2 as terminal,"
				   "scriptpath as script,ex='on' as exec,pp='on' as batch from bldscript order by tw";
			Db::Query* q = nullptr;
			if (sql.query(errs,q,str.str()) && q->execute(errs)) {
				/**
				 *
				 * +----+--------+----+------+------+------------+---------------------------+---------+-------+----------+-------------+------+-------+
				 * | id | parent | tw | size | tier | ref        | title                     | comment | macro | terminal | script      | exec | batch |
				 * +----+--------+----+------+------+------------+---------------------------+---------+-------+----------+-------------+------+-------+
				 * |  1 |      0 |  1 |   30 |    1 | ---        | Root                      | xxx     |     0 |        0 | none        |    0 |     0 |
				 * |  2 |      1 |  2 |    3 |    2 | html       | XHTML 1.1                 | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * |  3 |      2 |  3 |    1 |    3 | mxhtml     | mxs-xhtml                 | xxx     |     0 |        0 | mxs2html    |    0 |     0 |
				 * | 23 |      2 |  4 |    1 |    3 | html.dirty | dirty html                | xxx     |     0 |        0 | cleanHTML   |    0 |     0 |
				 * |  4 |      1 |  5 |    1 |    2 | mxs        | Medusa Parsed             | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * |  5 |      1 |  6 |    2 |    2 | css        | Stylesheet                | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 29 |      5 |  7 |    1 |    3 | scssi      | scss->css                 | xxx     |     0 |        0 |             |    0 |     0 |
				 * |  6 |      1 |  8 |    1 |    2 | smxs       | Secure Medusa Executable  | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * |  7 |      1 |  9 |    1 |    2 | sxhtml     | Secure XHTML              | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * |  8 |      1 | 10 |    1 |    2 | php        | PHP                       | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * |  9 |      1 | 11 |    1 |    2 | phtml      | Parsed HTML               | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 10 |      1 | 12 |    1 |    2 | dmxs       | FAQ mxs page              | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 11 |      1 | 13 |    3 |    2 | jpg        | JPEG Image                | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 12 |     11 | 14 |    1 |    3 | svgj       | SVG for JPG conv          | xxx     |     0 |        0 | svg2jpg     |    0 |     0 |
				 * | 14 |     11 | 15 |    1 |    3 | svg        | Batch SVG conversion      | xxx     |     0 |        0 | batchsvgjpg |    0 |     1 |
				 * | 13 |      1 | 16 |    1 |    2 | svgc       | SVG file                  | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 15 |      1 | 17 |    1 |    2 | txt        | Raw Text                  | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 16 |      1 | 18 |    1 |    2 | htc        | htc for windows png       | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 17 |      1 | 19 |    2 |    2 | js         | Javascript                | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 30 |     17 | 20 |    1 |    3 | jsx        | expanded js for squishing | xxx     |     0 |        0 | jssquish    |    0 |     1 |
				 * | 18 |      1 | 21 |    1 |    2 | qxml       | Context                   | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 19 |      1 | 22 |    1 |    2 | xml        | Generic xml file          | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 20 |      1 | 23 |    2 |    2 | ixml       | included context          | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 25 |     20 | 24 |    1 |    3 | ixml.dirty | Dirty IXML                | xxx     |     0 |        0 | cleanHTML   |    0 |     0 |
				 * | 21 |      1 | 25 |    1 |    2 | XXX        | removed file              | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 22 |      1 | 26 |    1 |    2 | obyx       | obyx files                | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 26 |      1 | 27 |    2 |    2 | acl        | [ACL] htaccess            | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 27 |     26 | 28 |    1 |    3 | aclm       | [ACL] local htaccess      | xxx     |     0 |        0 | mvhtaccess  |    0 |     1 |
				 * | 28 |      1 | 29 |    1 |    2 | scss       | scss source               | xxx     |     0 |        1 | NULL        |    0 |     0 |
				 * | 28 |      1 | 30 |    1 |    2 | arb        | scss source               | @arb()  |     1 |        1 | NULL        |    1 |     0 |
				 * +----+--------+----+------+------+------------+---------------------------+---------+-------+----------+-------------+------+-------+
				 */
				size_t parent = 0;
				while(q->nextrow()) {
//					auto suffix = make_unique<Suffix>();
					Suffix _suffix;
					Suffix* suffix = &_suffix;
					suffix->common(errs,q,parent,baked_tw);
					// Specific Suffix Values.
					suffix->_output = (suffix->_ref != "XXX");
					q->readfield(errs,"title",suffix->_title);
					q->readfield(errs,"comment",suffix->_comment);		// The comment as text
					q->readfield(errs,"macro",suffix->_macro);			// is the comment a macrotext to generate the actual suffix? (always terminal).
					q->readfield(errs,"terminal",suffix->_terminal);		// Is this a 'final' suffix?
					if(!suffix->_terminal) {
						q->readfield(errs,"script",suffix->_script);	// Script to handle process of suffix mapping.
						q->readfield(errs,"exec",suffix->_exec);			// Run as executable?
						q->readfield(errs,"batch",suffix->_batch);		// Batch process?
					}
					if(suffix->_macro) {
						std::istringstream program(suffix->_comment);
						mt::Driver(errs,program,mt::Definition::test_adv(suffix->_comment)).parse(errs,suffix->code,true);
					}
					//unordered_map<size_t,Suffix>
					auto ins = nodes.emplace(suffix->_id,std::move(_suffix));
					if(ins.second) {
						Suffix* node = &(ins.first->second);
						suffixes.add(errs,node,parent);
						refs.emplace(node->ref(),std::move(node));
					}
			
				}
			}
			sql.dispose(q);
			sql.unlock(errs);
			if(nodes.size() > 0) {
				nodes.find(suffixes.root()->id())->second.weigh();
			}
		}
		if (times.show()) { times.use(errs,"Load Suffixes"); }
	}

	void Suffix::weigh() {
		_weight = 1;
		if(_tier > 1 && _parent) {
			const Node* suff = this;
			while (suff->tier() > 2 ) {
				suff = suff->parent();
			}
			last = suff->suffix();
		}
		for (auto& i : children) {
			const_cast<Node*>(i)->weigh();
			_weight += i->size();
		}
	}

	const Node* Suffix::node(Messages& errs, size_t id, bool silent) const {
		return suffix(errs,id,silent);
	}

	const Suffix* Suffix::byRef(Messages& errs, string name, bool silent) {
		const Suffix* result =  nullptr;
		auto found = refs.find(name);
		if(found != refs.end()) {
			result = found->second;
		} else {
			if(!silent) {
				ostringstream err;
				err << "There is no suffix with reference " << name;
				errs << Message(range,err.str());
			}
		}
		return result;
	}

	const Suffix* Suffix::suffix(Messages& errs, size_t id, bool silent) {
		const Suffix* result =  nullptr;
		auto found = nodes.find(id);
		if(found != nodes.end()) {
			result = &(found->second);
		} else {
			if(!silent) {
				ostringstream err;
				err << "There is no suffix with id " << id;
				errs << Message(range,err.str());
			}
		}
		return result;
	}

	Suffix::~Suffix() {}

	bool Suffix::bGet(Messages& e,valueField field) const {
		bool result=false;
		switch(field) {
			case exec: result = _exec; break;
			case batch: result = _batch; break;
			case terminal: result = _terminal; break;
			case evaluate: result = _macro; break;
			default:
				result = Node::bGet(e, field);

		}
		return result;
	}

	size_t Suffix::iGet(Messages&,valueField) const {
		return 0;
	}

	string Suffix::sGet(Messages& e,valueField field) const {
		string result;
		switch(field) {
			case title: result = _title; break;
			case script: result = _script; break;
			default:
				result = Node::sGet(e, field);
		}
		return result;
	}

	Date Suffix::dGet(Messages& e,valueField field) const {
		return Node::dGet(e, field);
	}

	void Suffix::processScss(Messages& log) {
		if(!scssFiles.empty()) {
			log.push(Message(info, "Processing Scss Files"));
			Env &env = Env::e();
			Path final(env.dir(Built, Support::Content));
			if (Sass::available(log)) {
				Sass::resetpath();
				Sass::addpath(log, Path("/websites/shared/live/"));
				for (auto &scss : scssFiles) {
					Sass::addpath(log, Path(scss));
				}
				for (auto &scss : scssFiles) {
					string file = scss.output(true);
					ostringstream scss_oss;
					string sass_result;
					scss_oss << "@import '" << file << "';";
					File mapFile(scss);
					mapFile.setExtension("map");
					string map_result;
					string source = scss_oss.str(); //We need this to be a non const.
					string mapFilename = mapFile.output(true); //We need this to be a non const.
					bool compress = Build::b().current() == Support::final;
					if (Sass::expand(log, source, sass_result, map_result, mapFilename, compress)) {
						if(!sass_result.empty()) {
							File resultFile(scss);
							resultFile.setExtension("css");
							fandr(sass_result, "/websites/shared/live/", "/c/");
							resultFile.write(log, sass_result);
							if (!map_result.empty()) {
								mapFile.write(log, map_result);
							}
							string resultName = resultFile.output(true);
							log << Message(debug, "scss file " + resultName + " saved.");
						} else {
							log << Message(warn, "scss file " + file + " failed to compile.");
						}
					}
				}
			} else {
				log << Message(error, "Sass is not available despite there being scss files to process.");
			}
			log.pop();
		} else {
			log << Message(debug, "No SCSS found for processing.");
		}
	}

	void Suffix::processBatches(Messages& log) {
		/**
		 * /websites/site/edit_f/scripts/set /websites/site/edit_f/draft/en/ /websites/site/edit_f/draft/en draft txti txt 2>&1
		 * Batch scripts are passed the following parameters:
		 * 1: the directory that it is to work from (and to!).
		 * 2: the directory that it is ending up in
		 * 3: the build kind (draft/final).
		 * 4: the source extension - the suffix to consume
		 * 5: the destination extension - the suffix to produce
		 **/
		if(!batches.empty()) {
			log.push(Message(info, "Processing Batch Files"));
			log << Message(custom, "Batch Files");
			Env &env = Env::e();
			Path final(env.dir(Built, Support::Content));
			auto kind = (std::string) (Build::b().current());
			for (auto &batch : batches) {
				auto *current = batch.suffix;
				File batchFile(batch.script);
				while (current && !current->_terminal) {
					auto *parent = dynamic_cast<const Suffix *>(current->_parent);
					batchFile.addArg(batch.dir.output(false));
					batchFile.addArg(parent->_terminal ? final.output(false) : batch.script.output(false));
					batchFile.addArg(kind);
					batchFile.addArg(current->_ref);
					batchFile.addArg(parent->_ref);
					batchFile.exec(log);
					if (!parent->_terminal) {
						if (parent->_batch && !parent->_script.empty()) {
							File script(env.dir(Scripts), parent->_script);
							if (script.exists()) {
								batchFile = script;
							} else {
								log << Message(error, "Script " + script.output(true) + " does not exist");
							}
						} else {
							log << Message(error, "Nested batch suffices must be contained by batch suffixes. " +
												  parent->_ref +
												  " is either marked as not a batch file or is missing a script name.");
						}
					}
					current = parent;
				}
			}
			log << Message(custom, "Batch Files");
			log.pop();
		}
	}


	void Suffix::process(Messages& log,const Content* content,File& file) const {
		// file contains the current suffix, I think.
		if(	! _terminal ) {
			if ( _ref == "scssi" ) {
				scssFiles.insert(file);
			} else {
				File script(Env::e().dir(Scripts));
				auto *parent = dynamic_cast<const Suffix *>(_parent);
				auto next = file;
				next.setExtension(parent->_ref);
				if (_batch) {
					if (!_script.empty()) {
						script.setFileName(_script, true);
						if (script.exists()) {
							Batch batch({this, script, file});
							batches.insert(batch);
						} else {
							log << Message(error, "Script " + script.output(true) + " does not exist");
						}
					} else {
						log << Message(error, "Batch file marked with an empty script");
					}
				} else {
					if (!_script.empty()) {
						script.setFileName(_script, true);
						if (script.exists()) {
							auto kind = (std::string) (Build::b().current());
							ostringstream run;
							run << script.output(true) << " ";
							run << file.output(true) << " " << kind << " > " << next.output(true);
							log << Message(debug, "About to run script " + run.str());
							system(run.str().c_str());                                // Call system to run the script
							file.removeFile();
						} else {
							log << Message(error, "Script " + script.output(true) + " does not exist");
						}
					} else {
						file.moveTo(log, next);
					}
					parent->process(log, content, next);
				}
			}
		}
	}


}
