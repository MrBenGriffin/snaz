//
// Created by Ben on 2019-03-19.
//

#ifndef MACROTEXT_NODE_SUFFIX_H
#define MACROTEXT_NODE_SUFFIX_H

#include <string>
#include <unordered_map>

#include "node/Node.h"
#include "support/Definitions.h"
#include "support/Message.h"
#include "support/db/Connection.h"
#include "mt/mt.h"

using namespace std;
using namespace Support;
using namespace Db;

namespace node {
	class Suffix : public Node {

	private:
		static unordered_map<size_t,Suffix> nodes;
		static unordered_map<string,Suffix*> refs; //reference pointers.

	public:
		static Tree suffixes;
		Suffix();
		flavour cultivar() const override { return flavour::suffix; }

		const Node* node(Messages&, size_t, bool= false) const override; //by id.
		static const Suffix* suffix(Messages&, size_t, bool= false); //Suffix by id.
		static const Suffix* byRef(Messages&, string, bool= false);  //Suffix by ref.


		bool   get(Messages&,boolValue) const override;
		size_t get(Messages&,uintValue) const override;
		string get(Messages&,textValue) const override;
		Date   get(Messages&,dateValue) const override;

		void loadTree(Messages&, Connection&, size_t,buildKind = Support::final) override;

		// data fields here.

		void weigh() override;

		const Suffix* last;
		string _title;
		string _script;
		string _comment;
		bool _output;
		bool _terminal;
		bool _exec;
		bool _batch;
		bool _macro;
		mt::mtext code;

		~Suffix() override;            //deleting a Node deletes all of it's children
	};
}
#endif //MACROTEXT_NODE_SUFFIX_H
