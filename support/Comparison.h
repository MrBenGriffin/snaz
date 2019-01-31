//
// Created by Ben Griffin on 2019-01-31.
//

#ifndef MACROTEXT_COMPARISON_H
#define MACROTEXT_COMPARISON_H


#include <string>
#include <cctype>

using namespace std;

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


}


#endif //MACROTEXT_COMPARISON_H
