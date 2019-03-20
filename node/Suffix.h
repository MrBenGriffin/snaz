//
// Created by Ben on 2019-03-19.
//

#ifndef MACROTEXT_NODE_SUFFIX_H
#define MACROTEXT_NODE_SUFFIX_H

#include "node/Node.h"
#include "support/Message.h"
#include "support/db/Connection.h"

using namespace Support;
using namespace Db;

namespace node {
	class Suffix : public Node {
	public:
		flavour cultivar() const override { return suffix; }

		static constexpr auto finalSuffix 	= "A2";

		static void load(Messages&, Connection&);
		bool   get(Messages&,boolValue) const override;
		size_t get(Messages&,uintValue) const override;
		string get(Messages&,textValue) const override;
		Date   get(Messages&,dateValue) const override;

//		enum boolValue  { exec,batch};
//		enum uintValue  { team,layout,pages};
//		enum textValue  { title,shortTitle,tierRef,baseFilename,scope,classCode,synonyms,keywords,descr,fileSuffix,script};
//		enum dateValue  { birth,death};

		virtual ~Suffix();            //deleting a Node deletes all of it's children
	};
}
#endif //MACROTEXT_NODE_SUFFIX_H
