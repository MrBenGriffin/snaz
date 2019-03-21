//
// Created by Ben on 2019-03-19.
//
#include <sstream>

#include "support/Timing.h"
#include "support/db/Query.h"

#include "mt/Definition.h"
#include "mt/Driver.h"

#include "node/Tree.h"
#include "node/Node.h"
#include "node/Suffix.h"

using namespace std;

namespace node {

	Tree Suffix::suffixes("Suffixes");
	unordered_map<size_t,Suffix> Suffix::nodes;

	const Node* Suffix::current() const {
		return suffixes.root();
	};

	Suffix::Suffix() : Node(suffixes) {}

	void Suffix::loadTree(Messages& errs, Connection& sql, size_t languageId) {
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
					Suffix suffix;
					suffix.common(errs,q,parent,baked_tw);
					// Specific Suffix Values.
					q->readfield(errs,"title",suffix._title);
					q->readfield(errs,"comment",suffix._comment);		// The comment as text
					q->readfield(errs,"macro",suffix._macro);			// is the comment a macrotext to generate the actual suffix? (always terminal).
					q->readfield(errs,"terminal",suffix._terminal);	// Is this a 'final' suffix?
					if(!suffix._terminal) {
						q->readfield(errs,"scriptpath",suffix._script);	// Script to handle process of suffix mapping.
						q->readfield(errs,"exec",suffix._exec);			// Run as executable?
						q->readfield(errs,"batch",suffix._batch);		// Batch process?
					}
					if(suffix._macro) {
						std::istringstream code(suffix._comment);
						suffix.code = mt::Driver(errs,code,mt::Definition::test_adv(suffix._comment)).parse(errs,true);
					}
					auto ins = nodes.emplace(suffix._id,std::move(suffix));
					if(ins.second) {
						Suffix* node = &(ins.first->second);
						suffixes.add(errs,node,parent);
					}
				}
			}
			sql.unlock(errs);
		}
		if (times.show()) { times.use(errs,"Load Suffixes"); }
	}

	const Node* Suffix::node(Messages& errs, size_t id, bool silent) const {
		const Node* result =  nullptr;
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

	bool Suffix::get(Messages& errs,boolValue field) const {
		bool result=false;
		switch(field) {
			case exec: result = _exec; break;
			case batch: result = _batch; break;
			case terminal: result = _terminal; break;
			case evaluate: result = _macro; break;
			default: break;
		}
		return result;
	};
	size_t Suffix::get(Messages& errs,uintValue field) const {
		return 0;
	};
	string Suffix::get(Messages& errs,textValue field) const {
		string result;
		switch(field) {
			case title: result = _title; break;
			case script: result = _script; break;
			case comment: result = _comment; break;
			default: break;
		}
		return result;
	};
	Date Suffix::get(Messages& errs,dateValue field) const {
		Date result;
		return result;
	};

}