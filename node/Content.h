//
// Created by Ben on 2019-03-19.
//

#ifndef MACROTEXT_NODE_CONTENT_H
#define MACROTEXT_NODE_CONTENT_H

#include "support/Definitions.h"
#include "support/Message.h"
#include "support/File.h"
#include "support/db/Connection.h"
#include "node/Tree.h"
#include "node/Node.h"
#include "node/Suffix.h"
#include "mt/mt.h"

#include <deque>
#include <stack>


using namespace Support;
using namespace Db;

class BuildUser;
namespace content {
	class Layout;
	class Segment;
	class Template;
}

namespace node {

	class Metrics;
	class Content : public Node {
	private:
		static unordered_map<size_t,Content>  nodes;
		static constexpr auto rootFilename 	= "index";

	//** Language Content (per node)
		size_t _team;
		size_t _layout;
		string _shortTitle;
		string _title;
		string _editor;
		Date _birth;
		Date _death;
		const content::Layout* layoutPtr;
		deque<File>	finalFilenames;	//final filenames - in page order.
		string baseFileName;

		void reset();

		// Single Node Build.
		void compose(Messages&, Path&);

	public:
		static Tree editorial;
		static Content& get(size_t i) { return nodes[i]; }


		Content();
		const Node* node(Messages&, size_t, bool) const override; //Node by id.
		const Content* content(Messages&, size_t= 0, bool= false) const; //Content by id.
		static const Content* root();// { return dynamic_cast<const Content*>(editorial.root()); }

		const content::Layout* layout() const { return layoutPtr; }
		const std::string filename(Messages&,size_t) const;

		void generate(Messages&,buildType);
		void count(const BuildUser&,buildType,set<size_t>&);

		bool   bGet(Messages&,valueField) const override;
		size_t iGet(Messages&,valueField) const override;
		string sGet(Messages&,valueField) const override;
		Date   dGet(Messages&,valueField) const override;
		void   loadTree(Messages&, Connection&, size_t,buildKind) override; //depends upon Node flavour of
		void 	setLayouts(Messages &); 		//Set all the layouts for a nodetree
		flavour cultivar() const override { return flavour::content; }

		/** Per language static functions **/

		static void updateBirthAndDeath(Messages&, Connection&, size_t,buildKind);
		static void updateContent(Messages&, Connection&, size_t,buildKind);

	};
}

#endif //MACROTEXT_NODE_CONTENT_H


