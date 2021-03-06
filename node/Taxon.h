//
// Created by Ben on 2019-03-19.
//

#ifndef MACROTEXT_NODE_TAXON_H
#define MACROTEXT_NODE_TAXON_H

#include "node/Node.h"
#include "mt/using.h"

#include "support/Message.h"
#include "support/Definitions.h"

#include "support/db/Connection.h"

#include <unordered_map>
#include <set>
#include <map>
#include <deque>

using namespace std;
using namespace Support;
using namespace Db;

namespace node {
	/**
	 * A taxon is a node in a taxonomic tree.
	 * The taxonomic tree is a tree of taxonomies.
	 * Each taxonomy is rooted at tier=2 (the taxonomic tree is a forest).
	 * So, eg, a taxonomy 'Canon' is a set of restricted headings concerning 'Canon'.
	 * Which may include, eg.. 'Colours', 'Models', etc.
	 * For namespace purposes, each taxonomy uses a unique classcode (for the tier), as does each heading  (at T=3) within it.
	 * So, if Canon uses 'C',
	 * 'C:C:Red', 'C:N:DX360' etc.
	 * Namespaces are optional. Leaving them blank means that they will not be used.
	 * There's still no guarantee of uniqueness however.
	 */
	class Content;
	using nodeScore  = pair<long double,const Content*>;
	using nodeScores = deque< nodeScore >;

	class Taxon : public Node {
	private:
		static unordered_map<size_t,Taxon> nodes;
		static unordered_map<size_t, nodeScores > similarScores;
		static multimap<size_t,size_t,std::greater<unsigned>> contentToTaxon;
		static multimap<size_t,size_t,std::greater<unsigned>> taxonToContent;
		void loadSubs(Messages&,Connection&,size_t,buildKind);
		static bool scoreGreater(nodeScore& a, nodeScore& b)  { return a.first > b.first; }

	public:

		static Tree taxonomies;
		Taxon();
		Taxon(Taxon &&) = default; //ensure that we do not use a copy constructor on move..
		Taxon(const Taxon&) = delete;
		static const Taxon* root();// { return dynamic_cast<const Content*>(editorial.root()); }

		flavour cultivar() const override { return flavour::taxon; }
		virtual ~Taxon();            //deleting a Node deletes all of it's children
		const Node* node(Messages&, size_t, bool= false) const override; //by id.
		static const Taxon* taxon(Messages&, size_t, bool= false); //by id.
		void contentNodes(Messages&, mt::nlist&, const Node* ) const;
		void nodeContent(Messages&,mt::nlist&) const;

		bool   bGet(Messages&,valueField) const override;
		size_t iGet(Messages&,valueField) const override;
		string sGet(Messages&,valueField) const override;
		Date   dGet(Messages&,valueField) const override;
		void 	loadTree(Messages&, Connection&, size_t, buildKind) override; //depends upon Node flavour of

		static bool hasSimilar(Messages&, Connection&,const Content*);
		static const nodeScores* getSimilar(Messages&, Connection&,const Content*);
		static const nodeScores* loadSimilar(Messages&, Connection&,const Content*);

		size_t _team;
		string _classcode;
		string _scope;
		string _title;
		string _shortTitle;
		string _synonyms;
		string _keywords;
		string _article;
		string _editor;
		bool _container;
		Date _modified;

	};
}

#endif //MACROTEXT_NODE_TAXON_H
