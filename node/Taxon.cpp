//
// Created by Ben on 2019-03-19.
//

#include "node/Tree.h"
#include "node/Taxon.h"
#include "node/Node.h"

#include <string>
#include <unordered_map>

#include "support/Timing.h"
#include "support/Message.h"
#include "support/db/Connection.h"
#include "mt/mt.h"

using namespace std;

namespace node {

	Tree Taxon::taxonomies("Taxonomies");
	unordered_map<size_t,Taxon> Taxon::nodes;
	Taxon::Taxon() : Node(taxonomies) {}

	const Node* Taxon::current() const { return taxonomies.root(); }

	void Taxon::loadTree(Messages& errs, Connection& sql,size_t language) {
		Timing& times = Timing::t();
		if (times.show()) { times.set("Load Taxonomies"); }
		if ( sql.dbselected() && sql.table_exists(errs,"bldtax") && sql.table_exists(errs,"bldtaxlang") ) {
			sql.lock(errs,"bldtax n read,bldtaxlang l read");
			std::ostringstream str;
			str <<	"select n.id,ifnull(n.p,0) as parent,n.tw,n.nc-n.tw as size,n.t as tier,n.team,n.classcode,"
		  			"n.scope,n.id as ref,l.title,l.navtitle as shorttitle,l.synonyms,l.keywords,l.descr,"
	   				"l.container='on' as container,l.moddate,"
	   				"l.editor,l.used='on' as active from bldtax n, bldtaxlang l where "
					"l.node=n.id and l.used='on' and l.language=" << language << " order by n.tw";
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
				size_t active_tw = 1;  //we cannot use the native tw as we will be skipping used..
				while(q->nextrow()) {
					Taxon taxon;
					//id, parent,tier,ref,tw
					taxon.common(errs,q,parent,active_tw);

					// Specific Suffix Values.
					::time_t _modified;

					q->readfield(errs,"team",taxon._team);
					q->readfield(errs,"classcode",taxon._classcode);
					q->readfield(errs,"scope",taxon._scope);
					q->readfield(errs,"title",taxon._title);
					q->readfield(errs,"shorttitle",taxon._shortTitle);
					q->readfield(errs,"synonyms",taxon._synonyms);
					q->readfield(errs,"keywords",taxon._keywords);
					q->readfield(errs,"descr",taxon._descr);
					q->readfield(errs,"editor",taxon._editor);
					q->readfield(errs,"container",taxon._container);
					q->readTime(errs,"modified",_modified);
					taxon._modified.set(_modified);

					auto ins = nodes.emplace(taxon._id,std::move(taxon));
					if(ins.second) {
						Taxon* node = &(ins.first->second);
						taxonomies.add(errs,node,parent);
					}
				}
			}
			sql.unlock(errs);
		}
		if (times.show()) { times.use(errs,"Load Taxonomies"); }
	}

	bool Taxon::get(Messages& errs,boolValue field) const {
		bool result=false;
		switch(field) {
			case container: result = _container; break;
			default: break;
		}
		return result;
	};
	size_t Taxon::get(Messages& errs,uintValue field) const {
		bool result=0;
		switch(field) {
			case team: result = _team; break;
			default: break;
		}
		return result;
	};

	string Taxon::get(Messages& errs,textValue field) const {
		string result;
		switch(field) {
			case classCode: result = _classcode; break;
			case scope: result = _scope; break;
			case title: result = _title; break;
			case shortTitle: result = _shortTitle; break;
			case synonyms: result = _synonyms; break;
			case keywords: result = _keywords; break;
			case description: result = _descr; break;
			case editor: result = _editor; break;
			default: break;
		}
		return result;
	};

	Date Taxon::get(Messages& errs,dateValue field) const {
		Date result;
		switch(field) {
			case modified: result = _modified; break;
			default: break;
		}
		return result;
	};

	const Node* Taxon::node(Messages& errs, size_t id, bool silent) const {
		const Node* result =  nullptr;
		auto found = nodes.find(id);
		if(found != nodes.end()) {
			result = &(found->second);
		} else {
			if(!silent) {
				ostringstream err;
				err << "There is no taxon with id " << id;
				errs << Message(range,err.str());
			}
		}
		return result;
	}

	Taxon::~Taxon() { }

}