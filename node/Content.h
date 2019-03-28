//
// Created by Ben on 2019-03-19.
//

#ifndef MACROTEXT_NODE_CONTENT_H
#define MACROTEXT_NODE_CONTENT_H

#include "support/Definitions.h"
#include "support/Message.h"
#include "support/db/Connection.h"
#include "node/Node.h"
#include "node/Suffix.h"
#include "mt/mt.h"

using namespace Support;
using namespace Db;

namespace content {
	class Layout;
}

namespace node {

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

		string baseFileName;
		string scratchpad;    				// used for iForPeers <- maybe should go elsewhere..
//		vector<string> filenames;           // This is the initial page filename (includes . )
//		vector<string> suffixes;            // This is the initial page suffix.
//		vector<string> ffilename;           // This is the full, final filename (post-processed)
//		vector<size_t> tplates;             // list of template ID's to use

		void addpage(Messages &, NodeFilename *);
		void outputtofile(size_t page, string &out);
//Generation

		void gettextoutput(Messages &);
		void generateOutput(Messages &, int);

		void reset();
		void doTemplates(Messages &);

		// Single Node Build.
		void compose(Messages&,buildKind,size_t,size_t);

	public:
		static Tree editorial;
		static deque<Content *> nodeStack;    // current node - used to pass to built-in functions

		Content();

		const Node* node(Messages&, size_t, bool= false) const override; //Node by id.
		static const Content* content(Messages&, size_t, bool= false); //Content by id.
		static const Content* root();// { return dynamic_cast<const Content*>(editorial.root()); }

		void generate(Messages&,buildType,buildKind,size_t,size_t) const;

		bool   get(Messages&,boolValue) const override;
		size_t get(Messages&,uintValue) const override;
		string get(Messages&,textValue) const override;
		Date   get(Messages&,dateValue) const override;
		void   loadTree(Messages&, Connection&, size_t,buildKind) override; //depends upon Node flavour of

		void setLayouts(Messages &); 		//Set all the layouts for a nodetree
		flavour cultivar() const override { return flavour::content; }

/** Per language static functions **/

		static void updateBirthAndDeath(Messages&, Connection&, size_t,buildKind);
		static void updateContent(Messages&, Connection&, size_t,buildKind);

		const Node* current() const override;


	};
}

#endif //MACROTEXT_NODE_CONTENT_H


