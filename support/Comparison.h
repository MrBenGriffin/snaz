//
// Created by Ben Griffin on 2019-01-31.
//

#ifndef MACROTEXT_COMPARISON_H
#define MACROTEXT_COMPARISON_H


#include <string>
#include <cctype>
#include "node/Node.h"
#include "support/Message.h"

using namespace std;
using namespace node;

namespace Support {

	struct Sort {
		static size_t offset;
		static string regex;
	};

	bool asc_caseinsensitive(const string&, const string&);
	bool asc_casesensitive(const string&, const string&);
	bool asc_numeric(const string&, const string&);
	bool asc_date(const string&, const string&);

	bool desc_caseinsensitive(const string&, const string&);
	bool desc_casesensitive(const string&, const string&);
	bool desc_numeric(const string&, const string&);
	bool desc_date(const string&, const string&);

	bool sortscratch(const Node *,const Node *);
	bool sorttw(const Node *,const Node *);
	bool sorttier(const Node *,const Node *);
	bool sortfname(const Node *,const Node *);
	bool sortbirth(const Node *,const Node *);
	bool sortdeath(const Node *,const Node *);
	bool sorttitle(const Node *,const Node *);
	bool sorttitlei(const Node *,const Node *);
	bool sortlinkref(const Node *,const Node *);
	bool sortlinkrefi(const Node *,const Node *);
	bool sortshorttitle(const Node *,const Node *);
	bool sortshorttitlei(const Node *,const Node *);
}


#endif //MACROTEXT_COMPARISON_H
