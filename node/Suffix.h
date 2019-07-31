//
// Created by Ben on 2019-03-19.
//

#ifndef MACROTEXT_NODE_SUFFIX_H
#define MACROTEXT_NODE_SUFFIX_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "support/db/Connection.h"
#include "support/Definitions.h"
#include "support/Message.h"
#include "support/File.h"

#include "node/Node.h"
#include "mt/mt.h"

using namespace std;
using namespace Support;
using namespace Db;

namespace node {
	struct Batch {
		File script;
		Path dir;
		std::string extension;
		bool operator==(const Batch& rhs) const {
			return script == rhs.script && dir == rhs.dir  && extension == rhs.extension;
		}
	};

	struct hashBatch {
		std::hash<std::string> hash_fn;
		size_t operator()(const Batch& b) const noexcept {
			string s = b.script.output(true);
			string d = b.dir.output(true);
			string c = b.extension;
			uintmax_t h1 = hash_fn(s);
			uintmax_t h2 = hash_fn(d);
			uintmax_t h3 = hash_fn(c);
			uintmax_t hx =  h1 ^ (h2 + 0x9e3779b97f4a7c17 + (h1 << 6) + (h1 >> 2));
			uintmax_t hy =  hx ^ (h3 + 0x9e3779b97f4a7c17 + (hx << 6) + (hx >> 2));
			return hy;
		}
	};

	class Suffix : public Node {

	private:
		static unordered_map<size_t,Suffix> nodes;
		static unordered_map<string,const Suffix*> refs; // Reference pointers.
		static unordered_set<Batch, hashBatch> 	batches;	 	// Batchfiles.
		static unordered_set<string> scsses;	 	// Batchfiles.

	public:
		static Tree suffixes;
		Suffix();
		Suffix(Suffix &&) noexcept; //ensure that we do not use a copy constructor on move..
		Suffix(const Suffix&) = delete;

		flavour cultivar() const override { return flavour::suffix; }

		const Node* node(Messages&, size_t, bool= false) const override; //by id.
		static const Suffix* suffix(Messages&, size_t, bool= false); //Suffix by id.
		static const Suffix* byRef(Messages&, string, bool= false);  //Suffix by ref.

		void process(Messages&,const Content*,File&) const;

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
		bool _output;
		bool _terminal;
		bool _exec;
		bool _batch;
		bool _macro;
		mt::MacroText code;

		~Suffix() override;            //deleting a Node deletes all of it's children
	};
}
#endif //MACROTEXT_NODE_SUFFIX_H
