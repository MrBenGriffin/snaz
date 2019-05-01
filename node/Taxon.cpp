//
// Created by Ben on 2019-03-19.
//

#include "node/Tree.h"
#include "node/Taxon.h"
#include "node/Content.h"
#include "node/Node.h"

#include <string>
#include <unordered_map>

#include "support/Convert.h"
#include "support/Encode.h"
#include "support/Timing.h"
#include "support/Message.h"
#include "support/db/Connection.h"
#include "mt/mt.h"

using namespace std;

namespace node {

	Tree Taxon::taxonomies("Taxonomies");
	unordered_map<size_t, Taxon> Taxon::nodes;
	unordered_map<size_t, nodeScores > Taxon::similarScores;

	multimap<size_t, size_t, std::greater<unsigned>> Taxon::contentToTaxon;
	multimap<size_t, size_t, std::greater<unsigned>> Taxon::taxonToContent;

	Taxon::Taxon() : Node(taxonomies) {}


	void Taxon::loadTree(Messages &errs, Connection &sql, size_t language, buildKind kind) {
		Timing &times = Timing::t();
		if (times.show()) { times.set("Load Taxonomies"); }
		if (sql.dbselected() && sql.table_exists(errs, "bldtax") && sql.table_exists(errs, "bldtaxlang")) {
			sql.lock(errs, "bldtax n read,bldtaxlang l read,bldtax t read,bldtax g read");
			std::ostringstream str;
			//The following query is super inefficient.
			str << "select n.id,ifnull(n.p,0) as parent,n.tw,n.nc-n.tw as size,n.t as tier,n.team,n.classcode,n.scope,"
		  			"concat(n.id,':',l.title) as ref,"
		  			"l.title,l.navtitle as shorttitle,l.synonyms,l.keywords,l.descr,l.container='on' as container,"
	   				"l.moddate,l.editor,l.used='on' as active from bldtaxlang l,bldtax n "
					"where l.node=n.id and l.language=" << language << " and l.used='on' order by n.tw";
			Db::Query *q = nullptr;
			if (sql.query(errs, q, str.str()) && q->execute(errs)) {
				size_t parent = 0;
				size_t active_tw = 1;  //we cannot use the native tw as we will be skipping used..
				string taxonomy;
				while (q->nextrow()) {

	/**
	* +------+--------+------+------+------+------+-----------+---------+------------------------------------------------+------------------+----------+----------+--------------------------------------+-----------+------------+----------+--------+
	* | id   | parent | tw   | size | tier | team | classcode | scope   | title                                          | shorttitle       | synonyms | keywords | descr                                | container | moddate    | editor   | active |
	* +------+--------+------+------+------+------+-----------+---------+------------------------------------------------+------------------+----------+----------+--------------------------------------+-----------+------------+----------+--------+
	* | 4214 |      0 |    1 | 4636 |    1 |    1 |           |         | Subject Editing                                | Subject Editing  |          |          | All taxonomies hang from here.       |      NULL | 1047491436 | bgriffin |      1 |
	* | 4215 |   4214 |    2 | 4214 |    2 |    1 |           | LOCSH   | Library of Congress                            | LOCSH            |          |          | Library of Congress Subject Headings |      NULL | 1022001799 | bgriffin |      1 |
	* |    1 |   4215 |    3 |   31 |    3 |    1 | A         |         | general works                                  | general works    | NULL     | NULL     | NULL                                 |      NULL | 1021983832 | bgriffin |      1 |
	* |    2 |      1 |    4 |    5 |    4 |    1 | AC        | 1-999   | collections, series, collected works           | collections      | NULL     | NULL     | NULL                                 |      NULL | 1021983832 | bgriffin |      1 |
	* |    3 |      2 |    5 |    1 |    5 |    1 | AC        | 1-195   | collections of monographs, essays, etc.        | collections of m | NULL     | NULL     | NULL                                 |      NULL | 1021983832 | bgriffin |      1 |
	* |    4 |      2 |    6 |    1 |    5 |    1 | AC        | 801-895 | inaugural and program dissertations            | inaugural and pr | NULL     | NULL     | NULL                                 |      NULL | 1021983832 | bgriffin |      1 |
	* |    5 |      2 |    7 |    1 |    5 |    1 | AC        | 901-995 | pamphlet collections                           | pamphlet collect | NULL     | NULL     | NULL                                 |      NULL | 1021983832 | bgriffin |      1 |
	* |    6 |      2 |    8 |    1 |    5 |    1 | AC        | 999     | scrapbooks                                     | scrapbooks       | NULL     | NULL     | NULL                                 |      NULL | 1021983832 | bgriffin |      1 |
	* |    7 |      1 |    9 |    1 |    4 |    1 | AE        | 1-90    | encyclopedias                                  | encyclopedias    | NULL     | NULL     | general                              |      NULL | 1021983832 | bgriffin |      1 |
	* |    8 |      1 |   10 |    8 |    4 |    1 | AG        | 1-600   | dictionaries and other general reference works | dictionaries and | NULL     | NULL     | NULL                                 |      NULL | 1021983832 | bgriffin |      1 |
	* |    9 |      8 |   11 |    1 |    5 |    1 | AG        | 1-90    | dictionaries, minor encyclopedias              | dictionaries     | NULL     | NULL     | NULL                                 |      NULL | 1021983832 | bgriffin |      1 |
	* |   10 |      8 |   12 |    1 |    5 |    1 | AG        | 103-190 | general works, pocketbooks, receipts, etc.     | general works    | NULL     | NULL     | NULL                                 |      NULL | 1021983832 | bgriffin |      1 |
	* +------+--------+------+------+------+------+-----------+---------+------------------------------------------------+------------------+----------+----------+--------------------------------------+-----------+------------+----------+--------+
	**/

					Taxon taxon;
					//id, parent,tier,ref,tw
					taxon.common(errs, q, parent, active_tw);
					// Specific Suffix Values.
					::time_t _modified;
					q->readfield(errs, "team", taxon._team);
					q->readfield(errs, "classcode", taxon._classcode);
					q->readfield(errs, "scope", taxon._scope);
					q->readfield(errs, "title", taxon._title);
					q->readfield(errs, "shorttitle", taxon._shortTitle);
					q->readfield(errs, "synonyms", taxon._synonyms);
					q->readfield(errs, "keywords", taxon._keywords);
					q->readfield(errs, "descr", taxon._descr);
					q->readfield(errs, "editor", taxon._editor);
					q->readfield(errs, "container", taxon._container);
					q->readTime(errs, "modified", _modified);
					taxon._modified.set(_modified);
					if(taxon._tier == 2 ) {
						taxonomy = taxon._scope;
						if(!taxonomy.empty()) {
							taxonomy.push_back(':');
						}
					}
					taxon._ref=taxonomy;
					trim(taxon._title);
					trim(taxon._shortTitle);
					if(!taxon._title.empty()) {
						taxon._ref.append(taxon._title);
					} else {
						taxon._ref.append(taxon._shortTitle);
					}
					fileEncode(taxon._ref);
					auto ins = nodes.emplace(taxon._id, std::move(taxon));
					if (ins.second) {
						Taxon *node = &(ins.first->second);
						taxonomies.add(errs, node, parent);
					}
				}
			}
			sql.dispose(q);
			sql.unlock(errs);
			if(nodes.size() > 0) {
				nodes.find(taxonomies.root()->id())->second.weigh();
			}
		}
		loadSubs(errs, sql, language, kind);
		if (times.show()) { times.use(errs, "Load Taxonomies"); }
	}

	bool Taxon::get(Messages &errs, boolValue field) const {
		bool result = false;
		switch (field) {
			case container:
				result = _container;
				break;
			default:
				break;
		}
		return result;
	};

	size_t Taxon::get(Messages &errs, uintValue field) const {
		bool result = 0;
		switch (field) {
			case team:
				result = _team;
				break;
			default:
				break;
		}
		return result;
	};

	string Taxon::get(Messages &errs, textValue field) const {
		string result;
		switch (field) {
			case classCode:
				result = _classcode;
				break;
			case scope:
				result = _scope;
				break;
			case title:
				result = _title;
				break;
			case shortTitle:
				result = _shortTitle;
				break;
			case synonyms:
				result = _synonyms;
				break;
			case keywords:
				result = _keywords;
				break;
			case description:
				result = _descr;
				break;
			case editor:
				result = _editor;
				break;
			default:
				break;
		}
		return result;
	};

	Date Taxon::get(Messages &errs, dateValue field) const {
		Date result;
		switch (field) {
			case modified:
				result = _modified;
				break;
			default:
				break;
		}
		return result;
	};

	const Taxon* Taxon::taxon(Messages &errs, size_t id, bool silent) {
		const Taxon *result = nullptr;
		auto found = nodes.find(id);
		if (found != nodes.end()) {
			result = &(found->second);
		} else {
			if (!silent) {
				ostringstream err;
				err << "There is no taxon with id " << id;
				errs << Message(range, err.str());
			}
		}
		return result;
	}

	const Node *Taxon::node(Messages &errs, size_t id, bool silent) const {
		return taxon(errs,id,silent);
	}



	Taxon::~Taxon() {}

	void Taxon::loadSubs(Messages &errs, Connection &sql, size_t language,
						 buildKind kind) {  //load relations and reset scores.
		contentToTaxon.clear();
		taxonToContent.clear(); //This is called per language..
		similarScores.clear();  //Also, because we use Content pointers..
		Timing &times = Timing::t();
		if (times.show()) { times.set("Subscriptions"); }
		if (sql.dbselected() && sql.table_exists(errs, "bldnode") && sql.table_exists(errs, "bldnodelang") &&
			sql.table_exists(errs, "bldtaxsubs")) {
			sql.lock(errs, "bldtaxsubs s read,bldnode n read,bldnodelang l read");
			std::ostringstream str;
			str << "select s.nodeid,s.taxid from bldtaxsubs s,bldnode n,bldnodelang l "
				   "where n.id=s.nodeid and l.node=n.id and"
				   " l.language=" << language << " and l.used" << (kind == final ? "='on'" : "!=''");
			Db::Query *q = nullptr;
			if (sql.query(errs, q, str.str()) && q->execute(errs)) {
				while (q->nextrow()) {
					size_t tax, content;
					q->readfield(errs, "taxid", tax);
					q->readfield(errs, "nodeid", content);
					contentToTaxon.insert({content, tax});
					taxonToContent.insert({tax, content}); //store nodes against taxids.
				}
			}
			sql.dispose(q);
			sql.unlock(errs);
		}
		if (times.show()) { times.use(errs, "Subscriptions"); }
	}

	bool Taxon::hasSimilar(Messages &errs, Connection &sql,const Content* node) {
		if (similarScores.find(node->id()) == similarScores.end()) {
			return loadSimilar(errs, sql, node) != nullptr;
		}
		return true;
	}

	const nodeScores *Taxon::getSimilar(Messages &errs, Connection &sql,const Content* node) {
		auto found = similarScores.find(node->id());
		if (found == similarScores.end()) {
			return loadSimilar(errs, sql, node);
		} else {
			return &(found->second);
		}
	}

	const nodeScores * Taxon::loadSimilar(Messages &errs, Connection &sql,const Content* node) {
		const nodeScores * result = nullptr;
		if (sql.dbselected() && sql.table_exists(errs, "bldtaxscore")) {
			sql.lock(errs, "bldtaxscore read");
			std::ostringstream str;
			str << "select j as other,score from bldtaxscore where i=" << node;
			Db::Query *q = nullptr;
			nodeScores scores;
			if (sql.query(errs, q, str.str()) && q->execute(errs)) {
				while (q->nextrow()) {
					size_t id;
					nodeScore score;
					q->readfield(errs, "score", score.first);
					q->readfield(errs, "other", id);
					score.second= node->content(errs,id,true);
					if(score.second != nullptr) {
						score.first = 100.0 * score.first;
						scores.push_back(score);
					}
				}
			}
			if(!scores.empty()) {
				std::sort(scores.begin(),scores.end(),scoreGreater);
				auto iter = similarScores.emplace(node->id(),std::move(scores));
				result = &(iter.first->second);
			}
			sql.dispose(q);
			sql.unlock(errs);
		}
		return result;
	}
}
