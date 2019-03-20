//
// Created by Ben on 2019-03-19.
//

#ifndef MACROTEXT_NODE_TAXON_H
#define MACROTEXT_NODE_TAXON_H

#include "node/Node.h"
#include "support/Message.h"
#include "support/db/Connection.h"

using namespace Support;
using namespace Db;

namespace node {
	class Taxon : public Node {
		public:
		flavour cultivar() const override { return taxon; }
		static void load(Messages&, Connection&);
		virtual ~Taxon();            //deleting a Node deletes all of it's children
		bool   get(Messages&,boolValue) const override;
		size_t get(Messages&,uintValue) const override;
		string get(Messages&,textValue) const override;
		Date   get(Messages&,dateValue) const override;


//		enum boolValue  { exec,batch};
//		enum uintValue  { team,layout,pages};
//		enum textValue  { title,shortTitle,tierRef,baseFilename,scope,classCode,synonyms,keywords,descr,fileSuffix,script};
//		enum dateValue  { birth,death};

	};
}

#endif //MACROTEXT_NODE_TAXON_H
