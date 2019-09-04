//
// Created by Ben on 2019-02-27.
//

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <deque>
#include <functional>

//#include "buildsettings.h"
#include "BadLocatorPath.h"
#include "Node.h"
#include "NodeFilename.h"
namespace node {

	using namespace std;

/////////////////////////////////////////////////////////////////////////////
// Filenaming strategies:
// Necessities:
// max. filename len, suffix len
// function to generate name given a Node

// reset the name map to nullptr
	void NodeFilename::resetNameMap() {
		usedNames.erase(usedNames.begin(), usedNames.end());
	}

	map<string, Node *, less<string> > NodeFilename::usedNames;
	string NodeFilename::suffix = "html";

/////////////////////////////////////////////////////
//////////////////////
// NodeFilename //
//////////////////////

// make a filename legal
	void NodeFilename::nameLegalise(string &s) {
		unsigned int i = 0;
		while (i < s.length()) {
			char c;
			c = s[i];
			if (!isalnum(c)) {
				if ((i == 0) || (c != '-')) {
					s.erase(i, 1);
				} else {
					i++;
				}
			} else {
				if (isupper(c)) {
					s[i] += 'a' - 'A';    // tolower()
				}
				i++;
			}
		}
	}


// create name according to given Node
	void NodeFilename::set(Node *nd) {
		ostringstream fname;
		if (nd->ref().length() > 0) {    // a link ref is present
			name = nd->ref();
			nameLegalise(name);
			if (name.empty()) { name = "node"; }
		} else {
			name = "node";
		}
		if (name.length() == 0 || usedNames.count(name) > 0) {    // already in the map
			fname << name << "_" << nd->id();
			name = fname.str();
		}
		usedNames[name] = nd;
	}

// get the full name for page 'pageNum'
	void NodeFilename::setPageName(string baseName, size_t num) {
		name = std::move(baseName);    // set the filename to be baseName
		if (num > 0) {
			name += '_';
			appendPageNumber(num);
		}
	}

// get the page number
	void NodeFilename::appendPageNumber(size_t num) {
		if (num > 0) {
			ostringstream ns;
			ns << num << flush;
			name += ns.str();
		}
	}
}
