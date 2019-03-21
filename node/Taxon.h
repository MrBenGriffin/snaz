//
// Created by Ben on 2019-03-19.
//

#ifndef MACROTEXT_NODE_TAXON_H
#define MACROTEXT_NODE_TAXON_H

#include "node/Node.h"
#include "support/Message.h"
#include "support/db/Connection.h"

using namespace std;
using namespace Support;
using namespace Db;

namespace node {
	class Taxon : public Node {
	private:
		static unordered_map<size_t,Taxon> nodes;

		public:
		static Tree taxonomies;
		Taxon();

		flavour cultivar() const override { return taxon; }
		virtual ~Taxon();            //deleting a Node deletes all of it's children
		const Node* node(Messages&, size_t, bool= false) const override; //by id.
		bool   get(Messages&,boolValue) const override;
		size_t get(Messages&,uintValue) const override;
		string get(Messages&,textValue) const override;
		Date   get(Messages&,dateValue) const override;
		void 	loadTree(Messages&, Connection&, size_t) override; //depends upon Node flavour of
		const Node* current() const override;

		size_t _team;
		string _classcode;
		string _scope;
		string _title;
		string _shortTitle;
		string _synonyms;
		string _keywords;
		string _descr;
		string _editor;
		bool _container;
		Date _modified;

	};
}

#endif //MACROTEXT_NODE_TAXON_H
