//
// Created by Ben on 2019-02-27.
//

#ifndef MACROTEXT_NODEFILENAME_H
#define MACROTEXT_NODEFILENAME_H


#include <iostream>
#include <string>
#include <map>
#include <vector>

class Node;
using namespace std;

// generic filename
class NodeFilename {
protected:
	static map< string, Node*, less<string> > usedNames;
	static string suffix;
	int maxlen{};
	string name;

public:
	NodeFilename() = default;

	explicit NodeFilename(char *nname) : name(nname){}

	explicit NodeFilename(string& nname) : name(nname){}

	static void resetNameMap();
	static string getSuffix(){return suffix;}
	static NodeFilename *createInstance() { return (new NodeFilename); }

	void getBaseName(string& out) {out = name;}
	virtual void getFullName(string& out, string& nsuffix) {out = name + "." + nsuffix;}
	void setPageName(string baseName, size_t num);
	void appendPageNumber(size_t num);
	void nameLegalise(string& s);

	void set(Node *nd);

	virtual ~NodeFilename() = default;;

};



#endif //MACROTEXT_NODEFILENAME_H
