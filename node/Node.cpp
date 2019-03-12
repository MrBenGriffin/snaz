
#include <sstream>
#include <utility>
#include "node/Node.h"
#include "node/NodeLocator.h"
#include "node/NodeVal.h"

#include "mt/Driver.h"
#include "support/File.h"
#include "support/Regex.h"
#include "support/Convert.h"
#include "support/Env.h"
#include "support/Timing.h"
#include "support/db/Query.h"
#include "Build.h"

using namespace Support;
using namespace Support::Db;
using namespace mt;

refMap 			Node::tax_fields;				//A quick reference to the fields by name (as in iTax)
idTemplateMap	Node::templateList;
idIdListMap		Node::layoutList;				//Stores a layout-id -> template-list structure
refMap			Node::layoutRefs;				//Layout names -> ids
invRefMap		Node::layoutNames;				//Layout ids->names
deque< Node* > 	Node::nodeStack;				//current node - used to pass to built-in functions
Node* 			Node::roott = nullptr;
Node* 			Node::rootc = nullptr;
Node* 			Node::roots = nullptr;
std::string 	Node::kRootFilename="index";
std::string		Node::finalSuffix="A2"; 		//final suffix. All suffixes end up with an A2 suffix
bool 			Node::_showTemplates=false;   //show templates in logs..


//-------------------------------------------------------------------
//return current node (I0)
Node* Node::current(kind tree) {
	if (tree == page && !nodeStack.empty()) {
		return nodeStack.back();
	} else {
		switch(tree) {
			case page:	return rootc;
			case tax: 	return roott;
			case file:	return roots;
		}
	}
}
Node* Node::node(size_t id,kind tree) {
	Node* n = current(tree);
	return n->nodebyid(id);
}

//-----------------------------------------------------------------
void Node::inittaxfields() {
	tax_fields.insert({"id",0});
	tax_fields.insert({"scope",1});
	tax_fields.insert({"linkref",2});
	tax_fields.insert({"tier",3});
	tax_fields.insert({"team",4});
	tax_fields.insert({"title",5});
	tax_fields.insert({"shorttitle",6});
	tax_fields.insert({"classcode",7});
	tax_fields.insert({"synonyms",8});
	tax_fields.insert({"article",9}); //descr
}

//-------------------------------------------------------------------
Node::Node() {
	locator			= nullptr;
	nodeparent		= nullptr; // filled during addchild
	v				= nullptr; // empty the NodeVal
	nodeid 			= 0;
	nodetw			= 0;
	nodetier 	    = 0;
	nodesiblingnum	= 0;
	children.clear();	// added in when necessary
}

//-------------------------------------------------------------------
Node::Node(NodeLocator* loc, NodeVal* x,string ids,size_t newid,size_t newtw,size_t newtier) {
	nodeid 			= newid;
	nodetw			= newtw;
	idstr			= ids;
	nodetier 	  	= newtier;
	nodeparent		= nullptr; // filled during addchild
	nodesiblingnum	= 0;
	v				= x;    // assign NodeVal
	locator			= loc;
	children.clear();		// added in when necessary
}

//-------------------------------------------------------------------
Node::~Node() {
	while (!children.empty()) {
		delete children.back();
		children.pop_back();
	}
	v			    = nullptr; // a reference value, so do not delete here!
	locator			= nullptr; // a reference value, so do not delete here!
	nodeparent		= nullptr;	// a reference value - so do not delete here!
	nodeid			= 0;
	nodetw			= 0;
	nodetier		= 0;
	nodesiblingnum	= 0;
}

vector<Node *> Node::siblings() {
	vector<Node *> result;
	for (auto kid: nodeparent->children) {
		result.push_back( kid );
	}
	return result;
}

bool Node::isAncestor(Node* anc) {
	Node* cursor = this;
	while ( cursor && anc && cursor != anc && cursor->nodeparent != nullptr ) {
		cursor = cursor->nodeparent;
	}
	return anc && cursor == anc;
}

vector<Node *> Node::ancestors(Node* anc) {
	Node* cursor = this;
	vector<Node *> result;
	while ( (cursor != anc) && cursor->nodeparent != nullptr ) {
		result.push_back(cursor);
		cursor = cursor->nodeparent;
	}
	if(cursor == anc) {
		result.push_back(anc);
	}
	return result;
}


//internal recursive function used to gather nodes at a given depth...
void Node::addPeers(vector<Node *>& result,size_t depth) {
	if (depth < 1) return;
	for (auto kid: children) {
		if(depth == 1) {
			result.push_back( kid );
		} else {
			kid->addPeers(result,depth-1);
		}
	}
}

vector<Node *> Node::peers(Node* anc) {
	if(isAncestor(anc)) {
		vector<Node *> result;
		anc->addPeers(result,nodetier - anc->nodetier); //6-5 == 1 so it should return siblings..
		return result;
	} else {
		return siblings();
	}
}

size_t Node::size() const {
	size_t bsize = 1 + children.size();
	for (auto kid: children) {
		bsize += kid->size();
	}
	return bsize;
}

//-------------------------------------------------------------------
void Node::addchild(Node* k) {
	k->nodesiblingnum	= children.size();
	k->nodeparent		= this;
	children.push_back(k);
}

//-------------------------------------------------------------------
//Accessors below.
//-------------------------------------------------------------------
void Node::Suffix(size_t i , string s) {		//Set suffix at position.
	if ( i >= v->suffixes.size() ) return;				//No suffix
	v->suffixes[i] = std::move(s);
}
//-------------------------------------------------------------------
void Node::SetFfilename(size_t i , string s) {		//Set suffix at position.
	if ( i >= v->ffilename.size() ) return;				//No suffix
	v->ffilename[i] = std::move(s);
}
//-------------------------------------------------------------------
void Node::SetFilename(size_t i , string s) {		//Set suffix at position.
	if ( i >= v->filename.size() ) return;				//No suffix
	v->filename[i] = std::move(s);
}
//-------------------------------------------------------------------
size_t Node::getNumchildren() {
	return (size_t)children.size();
}
//-------------------------------------------------------------------
//Navigation functions follow
//-------------------------------------------------------------------
Node*  Node::child(Messages& errs,size_t x) {
	if ( x >= children.size() ) {
		auto cs = (size_t)children.size();
		ostringstream text;
		text << "A child index was greater than the number of children. Index:" << x << " Children: " << cs;
		errs << Message(error,text.str());
		return nullptr;
	}
	return children[x];
}

//-------------------------------------------------------------------
Node* Node::nodebyid(size_t idref) {
	if (v->idm != nullptr ) {
		auto i = v->idm->find(idref);
		if ( i != v->idm->end()) {
			return i->second;
		}
	}
	return nullptr;
}

//-------------------------------------------------------------------
Node* Node::nodebyidsilent(size_t idref) { //We don't want to report an error when loading the node-network!
	if (v->idm != nullptr ) {
		idmaptype::const_iterator i = v->idm->find(idref);
		if ( i != v->idm->end()) {
			return i->second;
		}
	}
	return nullptr;
}

//-------------------------------------------------------------------
Node* Node::nodebytw(Messages& errs,signed long offset) { //root isn't in here.. 0 will return end of tree.
	Node * retval = nullptr;
	size_t twref = nodetw + offset;
	if (v->rfm != nullptr ) {
		if (twref > v->twm->size()) { twref = 1; }
		if (twref < 1) { twref = v->twm->size(); } //now supports previous/next
		auto i = v->twm->find(twref);
		ostringstream text;
		if (i != v->twm->end()) {
			retval = i->second;
			if (errs.verbosity() > 4) {
				text << "Node id " << retval->id() << " with tw value '" << twref << "' was found.";
				errs << Message(info, text.str());
			}
		} else {
			if (errs.verbosity() > 4) {
				text << "Node with tw value '" << twref << "' was not found.";
				errs << Message(error, text.str());
			}
		}
	}
	return retval;
}

//-------------------------------------------------------------------
	Node* Node::nodebylinkref(Messages& errs,const string linkrefname) {
		Node* result = nullptr;
		if (v->rfm != nullptr) {
			auto i = v->rfm->find(linkrefname);
			if ( i != v->rfm->end()) {
				result = i->second;
				if (errs.verbosity() > 6 ) {
					errs << Message(info,"A node with linkref value '" + linkrefname + "' was found.");
				}
			} else {
				if (errs.verbosity() > 6 ) {
					errs << Message(error,"A node with linkref value '" + linkrefname + "' was not found.");
				}
			}
		}
		return result;
	}

//-------------------------------------------------------------------
//return the Node described by 'path'
// "/TCS!:n+-.A^RFBIO0123456789" every legal initial char
// "/TS!:AI123456789" absolute (for node) initial chars
// "C0n+-.^RFBO" relative (for node) initial chars
Node* Node::nodebypath(Messages& errs,const string& path) {
	Node* result = nullptr;
	if ((path.size() > 0) && (locator->root == rootc)) { 	// If specified startnode, use it
		const string relatives="C0n+-.^RFBO"; //yes, 0 is a relative address in this case!
		if (relatives.find(path[0]) != string::npos) {
			locator->setdirty();
		} else {
			if (path.length() > 1) {
				if (path[0] == 'I' && path[1] != '0') {
					locator->setdirty();
				} else {
					if (path[0] == 'A' && path[1] != '1') {
						locator->setdirty();
					}
				}
			}
		}
	}
//	NodeLocator::loc_path=path;
	result =  locator->locate(errs,this,path.begin(),path.end());
	if (result == nullptr) {
		errs << Message(error,"Path: " + path + " did not find a node.");
	}
	return result;
}

//-------------------------------------------------------------------
//return the Node described by 'path'
Node* Node::nodebypath(Messages& errs,string::const_iterator in, string::const_iterator out) {
	Node* result = nullptr;
	if ( (out - in) > 0) { 	// If specified startnode, use it
//		NodeLocator::loc_path=string(in,out);
		result =  locator->locate(errs,this,in,out);
	}
	return result;
}

//-------------------------------------------------------------------
// Reset browser-dependant fields of Node n -- currently useful only for content Node
void Node::reset() {
	v->reset();
	for (auto &i : children) {
		i->reset();
	}
}
//-------------------------------------------------------------------
Node* Node::nodelast(Messages& errs,Node* lroot) {
	Node* next = this;
	do {
		next = next->nodebytw(errs,-1);
	} while ( next->nodetier != nodetier );
	Node* rtest = next;
	while ( rtest != nullptr && rtest != lroot )
		rtest = rtest->nodeparent;
	if ( rtest == nullptr || next == this) return nullptr;
	else return next;
}
//-------------------------------------------------------------------
Node* Node::nodenext(Messages& errs,Node* lroot) {
	Node *next = this;
	do {
		next = next->nodebytw(errs, 1 );
	} while ( next->nodetier != nodetier );
	Node *rtest = next;
	while ( rtest != nullptr && rtest != lroot ) {
		rtest = rtest->nodeparent;
	}
	if ( rtest == nullptr || next == this ) return nullptr;
	else return next;
}
//-------------------------------------------------------------------
void Node::setLayouts(Messages& errs) {
	auto i = v->twm->begin();
	size_t twms =  v->twm->size();
	if (errs.verbosity() > 6 ) {
		ostringstream message;
		message << "Layouts iterator set for " << twms << " Nodes.";
		errs << Message(info,message.str());
	}
	while ( i != v->twm->end() ) {
		i->second->Layout(errs);
		i++;
	}
	if (errs.verbosity() > 5 ) {
		errs << Message(info,"Layouts are set");
	}
//	_Root = "/"; //.clear();
}
//-------------------------------------------------------------------
// load template list for the (content) node
// It should probably be part of nodeval
void Node::Layout(Messages& errs) {
	Build build = Build::b();
	if (errs.verbosity() > 6 ) errs << Message(info,"Loading template list for node " + idstr );
	Env& env = Env::e();
	if( nodeid != 0 ) {  //don't do anything for root
		vector<size_t> templateids;
		if (errs.verbosity() > 6 ) {
			ostringstream message;
			message << "Node " << idstr << " uses layout " << layout();
			errs << Message(info,message.str());
		}
		auto nmi = layoutList.find( layout() );
		if (nmi != layoutList.end()) {
			templateids = nmi->second; // My order
		} else {
			ostringstream text;
			text << "Found a Layout " << layout() << " at Node " + idstr + " for Technology " << build.tech() << " with a null templatelist.";
			errs << Message(error,text.str());
			return;
		}
		if (errs.verbosity() > 6 ) {
			size_t tids =  templateids.size();
			ostringstream text;
			text << "Node " << idstr << ", Layout " << layout()  << " has a template list with " << (size_t)tids << " template(s).";
			errs << Message(info,text.str());
		}
		if(!templateids.empty()) {	// template list not defined //
			// get template list and count number of templates in list //
			v->tplates = templateids;
			v->filename.resize(templateids.size(),"");		//Source filename (no suffix anymore)
			v->suffixes.resize(templateids.size(),"");		//
			v->ffilename.resize(templateids.size(),"");	//Makes space for the filenames & ffilenames.
			if (errs.verbosity() > 6 ) {
				ostringstream text;
				text << "Node " + idstr + " record has been resized for " << templateids.size() << " templates.";
				errs << Message(info,text.str());
			}
			NodeFilename *nfilename = NodeFilename::createInstance();
			if (errs.verbosity() > 6 ) {
				errs << Message(info, "Node " + idstr + " had it's filenaming instance created." );
				if(this == rootc) {
					errs << Message(info,"Node " + idstr + " is the root content node." );
				} else {
					ostringstream text;
					text << "Node " << idstr << " is NOT the root content node. The root content node id is:" << rootc->nodeid;
					errs << Message(info,text.str());
				}
			}
			if(this == rootc) {
				ostringstream text;
				text << "Node id: " + idstr + ", basefilename: " << basefilename() << ", rootfilename: " << kRootFilename;
				errs << Message(info,text.str());
				basefilename(kRootFilename); //Set basefilename
				errs << Message(info,"basefilename: " + basefilename() );
			}
			nfilename->set( this );
			string thisbasename;
			nfilename->getBaseName( thisbasename );
			if (errs.verbosity() > 6 ) errs << Message(info,"Node " + idstr + " Basename is set to:" + thisbasename );
			basefilename(thisbasename);
			if (errs.verbosity() > 6 ) errs << Message(info,"Node " + idstr + " About to add the pages. " );
			for(size_t i = 0; i < templateids.size(); i++) {
				addpage(errs,nfilename);		//<--- We are adding the pages here..
			}
			if (errs.verbosity() > 6 ) errs << Message(info,"Node " + idstr + " Finished adding pages. " );
			delete nfilename;
		}
	}
}

void Node::loadLayouts(Messages& errs,Connection& dbc) {
	ostringstream text;
	Env& env =Env::e();
	Timing& times = Timing::t();
	Build& build = Build::b();
	if (errs.verbosity() > 3) errs << Message(info,"Loading layouts and templates.");
	if (times.show()) { times.set("loadLayouts()"); }
	if (dbc.dbselected() && dbc.table_exists(errs,"bldlayout") && dbc.table_exists(errs,"bldlayouttechs") && dbc.table_exists(errs,"bldtemplate")) {
		ostringstream qstr;
		dbc.lock(errs, "bldlayout l read,bldlayouttechs x read,bldtemplate t read" ); //everyone can read only..
		qstr << "select layout,templatelist,name from bldlayout l, bldlayouttechs x where x.layout=l.id and technology=" << build.tech();
		auto* q = dbc.query(errs,qstr.str());
		if ( q->execute(errs) ) {
			string f_layout_id,f_templatelist,f_name;
			while(q->nextrow()) {
				q->readfield(errs,"layout",f_layout_id);			//discarding bool.
				q->readfield(errs,"templatelist",f_templatelist);	//discarding bool.
				q->readfield(errs,"name",f_name);					//discarding bool.
				vector<size_t> templatelist;
				size_t id = natural(f_layout_id);
				tolist( templatelist,f_templatelist);
				layoutList.insert({id, templatelist});
				layoutRefs.insert({f_name, id});
				layoutNames.insert({id,f_name});
			}
			if (errs.verbosity() > 3) {
				text <<  layoutRefs.size() << " layouts imported.";
				errs << Message(info,text.str()); text.str("");
			}
			qstr.str("");
			qstr << "select distinct(t.id) as id,t.templatemacro,t.suffix,t.break,t.comment from bldtemplate t,bldlayouttechs x where ";
			qstr <<  "find_in_set(t.id,x.templatelist) != 0 and x.technology="  << build.tech();
			q->setquery(qstr.str());
			if ( q->execute(errs) ) {
				string f_id,f_comment;
				while(q->nextrow()) {
					FileTemplate n;
					q->readfield(errs,"id",f_id);				//discarding bool.
					q->readfield(errs,"templatemacro",n.str);	//discarding bool.
					q->readfield(errs,"suffix",n.suffix);		//discarding bool.
					q->readfield(errs,"break",n.br);				//discarding bool.
					q->readfield(errs,"comment",f_comment);		//discarding bool.
					size_t id = natural(f_id);
					templateList.insert({id, pair<string, FileTemplate >(f_comment,n)});
				}
				if (errs.verbosity() > 3) {
					text << templateList.size() << " templates imported. ";
					errs << Message(info,text.str()); text.str("");
				}
			} else {
				errs << Message(error,"Build::loadLayouts: DB Error while loading templates.");
			}
		} else {
			errs << Message(error,"Build::loadLayouts: DB Error while loading templates.");
		}
		dbc.unlock(errs);
	}
	if (times.show()) { times.get(errs,"loadLayouts()"); }
	if (errs.verbosity() > 3) {
		errs << Message(info,"Layouts and templates loaded.");
	}
}

//-------------------------------------------------------------------
void Node::addpage(Messages& errs,NodeFilename* filename) {
	ostringstream text;
	Build& build = Build::b();
	if ( this == rootc ) {
		return;
	} else {
		size_t page = numpages(); //current page we are creating..
		if (errs.verbosity() > 6 ) {
			text << "Node " << idstr << " currently on page " << page << " page of " << (size_t)templateCount();
			errs << Message(info,text.str());
			text.str("");
		}
		string emptyname;

		if(page >= templateCount()) {
			text << " Attempt to create pages for too few templates; " << page << " requested; " << (size_t)templateCount() << " defined. Will leave it empty";
			errs << Message(warn,text.str());
			return;
		}
		filename->setPageName(basefilename(), page);
		filename->getFullName(Filename(page), emptyname);
		FileTemplate curtemplate;
		size_t tid = Template(page);
		if (errs.verbosity() > 6 ) {
			ostringstream text; text << "Node " << idstr << " page " << page << " has template ID " << tid;
			errs << Message(info,text.str() );
			text.str("");
		}

		auto nmi = templateList.find(tid);
		if (nmi != templateList.end()) {
			curtemplate = nmi->second.second; // My order
			nodeStack.push_back(this); //tid
			ostringstream tname;  tname << "Suffix " << tid;
			ostringstream result;
			string suff = Driver::expand(errs,curtemplate.suffix,tname.str());
			nodeStack.pop_back();
			Suffix(page,suff);        //Now we need to deal with it as macrotext..
			if (errs.verbosity() > 6 ) {
				text << "Node " << idstr << " page " << page << " has suffix " << suff;
				errs << Message(info,text.str());
				text.str("");
			}
			if ( roots == nullptr ) {
				errs << Message(error, "Suffix Root is nullptr!" );
			} else {
				//This has set up the suffixes..
				//We need to find the ancestor suffix so that any links we create can deal with the post-processed versions!
				Node *x = roots->nodebylinkref(errs, suff);

				if (x == nullptr)  {
					string fname = Filename(page);
					filename->getFullName(fname, suff);
					SetFfilename(page,fname);
				} else {
					Node *vx = x->nodebypath(errs,finalSuffix.begin(),finalSuffix.end());
					if ( vx == nullptr) {
						errs << Message(error, "Error of some sort in suffix table" );
					} else {
						suff = vx->suffix();
					}
					string fname = Filename(page);
					filename->getFullName(fname, suff);	//This now works!  Amazing.
					if (errs.verbosity() > 6 ) {
						text << "Node " + idstr + " page " << page << " is set with filename " << fname;
						errs << Message(info,text.str()); text.str("");
					}
					SetFfilename(page,fname);
				}
			}
			incPageCount();
		} else {
			Env env = Env::e();
			text << "Template " << tid << " at Node " << id() << ",Technology " << build.tech() << " referenced but does not exist.";
			errs << Message(error,text.str()); text.str("");
		}
	}
	if (errs.verbosity() > 6 ) errs << Message(info, "Node " + idstr + " Finished adding page " );
}
//-------------------------------------------------------------------
//This is the beginning of the node build.
void Node::gettextoutput(Messages& errs) {
	Env env = Env::e();
	Build build = Build::b();
	Timing timing = Timing::t();
	string in_s,baseurl_a,baseurl_s,langtech_s;
	timing.set('N');
	env.get("LTPATH",langtech_s);
	bool inTeam = build.user.mayTeamEdit(team());
	if (errs.verbosity() > 0) {
		ostringstream line_oss;
		if (inTeam) {
			baseurl_a = env.baseUrl(Editorial);
			if ( Regex::available(errs) ) {
				string editor;
				const string pattern = "^([^0]*)0([^0]*)0([^0]*)$";
				env.get("RS_BUILDNODEPATH",editor,"/en/nnedit.mxs?0&d&0"); //if none, keep default.
				std::ostringstream sub;
				sub << "\\1" << this->id() << "\\2" << build.lang() << "\\3";
				Regex::replace(errs,pattern,sub.str(),editor);
				line_oss << editor;
			} else {
				line_oss << "/en/nnedit.mxs?" << this->id() << "&d&" << build.lang();
			}
		}
		string Title = title();
		if (Title.empty()) Title = linkref();
		baseurl_s = env.baseUrl(build.current()); //current()
		if (Title.length() > 60) {
			Title = Title.substr(0,60);
			Title = Title.append("...");
		}
		bool did_url = false;
		for(size_t i = 0; i < templateCount() && !did_url; i++) {
			if (Suffix(i) != "XXX") {
				ostringstream line;
				if (inTeam) {
					line << "<a href='" << baseurl_a << line_oss.str() << "'>" << "*" << "</a>";
				} else {
					line << "+ ";
				}
				line << this->id() << " - <a href='" << baseurl_s << "/" << langtech_s << Ffilename(0)  << "'>" << Title << "</a>";
				errs << Message(even,line.str());
				did_url = true;
			}
		}
		if (!did_url) {
			ostringstream line;
			if (inTeam) {
				line << "<a href='" << baseurl_a << line_oss.str() << "'>" << "*" << "</a>";
			} else {
				line << "+ ";
			}
			line << this->id() << " - " << Title;
			errs << Message(struc,line.str());
		}
	}
	//--
	string bk;
	for(size_t i = 0; i < templateCount(); i++) {
		build.setPage(i);
		if (_showTemplates && templateCount() > 1) {
			ostringstream text;
			text << " Template (" << i + 1 << ")";
			errs << Message(info, text.str());
		}

		string templatename("Undefined Template");
		size_t templateId = Template(i);
		FileTemplate curtemplate;
		auto nmi = templateList.find(templateId);
		if (nmi != templateList.end()) {
			curtemplate = nmi->second.second;  // My order
			templatename = "[" + nmi->second.first + "]";  // The template's name
			build.setSuffix(curtemplate.suffix);
			in_s = curtemplate.str;  //Starter for template
			bk = curtemplate.br; //Template's own break.
		} else {
			ostringstream line;
			line << "Template " << templateId << " at Node " << this->id() << ",Technology " << build.tech()
				 << " referenced but does not exist.";
			errs << Message(warn, line.str());
			build.setSuffix("");
			in_s = "";
			bk = "";
		}

		//TODO:: Finish template generation!!
/*
		Macro::environ TemplateEnv = macro::topEnv();
		if ( bk.length() > 0 ) {
			macro::pushEnv({false,"\n","\t"}); //do not rembr, \n  = newline, tabs are tabs.
			nodeStack.push_back(this);
			string crlfName = "CR:" + to_string(String::Digest::hash(bk));
			TemplateEnv.crlf = macro::process(Logger::log,crlfName,bk);
			nodeStack.pop_back();
			macro::popEnv();
		}
		macro::pushEnv(TemplateEnv);
		{
			if (in_s.length() > 0) {
				string outval;
				if (in_s.length() > 0) {						//fandr may remove the last char!
					nodeStack.push_back(this);
					string tnumStr = to_string(tnum);
					outval = macro::process(Logger::log,"Template " + tnumStr,in_s);
					nodeStack.pop_back();
				}
				outputtofile(i,outval);
			}
			if (showTemplate && this->templateCount() > 1 && errs.verbosity()  > 0) {
				*Logger::log << O; //end of show template.
			}
			in_s.clear();
			bk.clear();
		}
		macro::popEnv();
*/
	}
	//TODO:: And the rest.
/*
	//-- This used to be in genfiles, but then happened once for each file, rather than one for each Node.
	if (dbc->dbselected() && dbc->table_exists("bldnode") ) {
		ostringstream qstr;
		qstr << "UPDATE LOW_PRIORITY bldnode SET filename='" << this->Ffilename(0) << "' WHERE id=" << this->id();
		Query* q = dbc->query(qstr.str());
		if (! q->execute() ) {
			errs << Message(error << "Node::gettextoutput(): DB Error" );
		}
		delete q;
	}
	resetNodePersistance();	//Delete *vars.
	if (showTiming && errs.verbosity()  > 3) {
		errs << Message(timing << Log::Ntime << " build time for node: " << linkref() );
	}
	if (errs.verbosity()  > 0) *Logger::log << O; //end of node.
	*/
}
//-----------------------------------------------------------------
// We shall generate each file here, and then post-process it until its suffix is at A2 of the processor tree.
// We use A2, because we are allowed multiple suffixes!
//-----------------------------------------------------------------
void Node::outputtofile(size_t page, string& out) {
	//TODO:: Finish outputtofile.

	/**
	File tmpScrp(scrDir);
	File tmpFil1(tmpDir);
	File tmpFil2(tmpDir);
	string tfilename;
	string suff;
	bool postprocessed=false;
	suff = Suffix(page);									// Starter position.
	if (suff != "XXX") {
		tfilename =  Filename(page);
		if (suff == "scssi") {					// Sass internal pre-process
			bld->scssifiles.push_back(tfilename);
			suff = "scss";									// flipped.
		}
		tfilename.append(suff);
		tmpFil1.setFileName(tfilename);					// Start off with the source..
		if (bld->showFiling) {
			size_t tflen =  out.length();
			errs << Message(info << "About to output file " << tfilename << " having " << (size_t)tflen << " bytes" ) ;
		}
		try {
			ofstream outFile(tmpFil1.output().c_str());
			outFile << out;
			outFile.close();
			chmod(tmpFil1.output().c_str(),S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		}	//create the file!
		catch(...) {
			errs << Message(error << "Failed to create file (" << tmpFil1.output() << ")" ) ;
		}
		out.clear();
		Node *x = roots->nodebylinkref(suff);		// Find the post-processor NodeSuffix.  We have the file for this now.
		if (x != nullptr) {								// We do have a post-processing entry, so lets use it.
			Node* y = x->nodebypath(finalsuffix.begin(),finalsuffix.end());
			while ( x != y && x != nullptr) {					// While x is not y, and x is not null, use the scriptpath for x
				if (!postprocessed && showFiling) {
					errs << Message(info << "for post-processing using: " << suff << " ";
					postprocessed = true;
				}
				tmpScrp.setFileName(x->script());		// Get the script name for x
				Node *z = x->parent();
				if (  x->batch() == 1  ) {							// this is to be batch-processed
					if (bld->showFiling) {
						*Logger::log << " as one of a batch." );
					}
					bld->batchscripts.insert(storage_map_type::value_type(tmpScrp.output(), bld->tmpDir.output() ));
				} else {
					string sfilename =  Filename(page) + x->suffix();
					tmpFil1.setFileName(sfilename);
					string dfilename =  Filename(page) + z->suffix();
					tmpFil2.setFileName(dfilename);
					if (bld->showFiling) *Logger::log << " -> "<< z->suffix();
					ostringstream command;
					string build=bld->dev ? "dev " : "live "; // + Environment::Path();
					command << tmpScrp.output() << " " << tmpFil1.output() << " " << bld->pagePath.output(true) << " " << build << " > " << tmpFil2.output();
					if (bld->showFiling) errs << Message(info << " About to run command:" << command.str() );
					MLog::stream errs("exec");
					FileUtils::exec(command.str(),errs);
					errs.str(Logger::log);
					if ( z->exec() == 1 ) {
						if (bld->showFiling) *Logger::log << " (Adding exec to:" << tmpFil2.output() << ")";
						if (bld->showFiling) errs << Message(info << " About to adjust permissions for execute." );
						chmod(tmpFil2.output().c_str(),S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH );
					}
					tmpFil1.removeFile();
				}
				x = z;
			}
		}
		if (bld->showFiling && !postprocessed)  errs << Message(info << " with no post-processing";
		if (bld->showFiling) errs << Message(end;
	}
	 **/
}

// -----------------------------------------------------------------------------------
// generate basic text output for all nodes below Node n in the Node tree */
// and 'n's direct ancestors for link generation */
void Node::generateOutput(Messages& errs,int level) {
	if(level >= 0 && nodeparent != nullptr ) {// generate output for all direct ancestors
		nodeparent->generateOutput(errs,level+1);
	}
	if( this != rootc) {
		gettextoutput(errs);
	}
	if(level <= 0) { 	// generate output for all descendents
		for (auto &i : children)
			i->generateOutput(errs,level-1);
	}
}
// -----------------------------------------------------------------------------------
// generate basic text output for all nodes below Node n in the Node tree */
void Node::generateBranch(Messages& errs,int level) {
	if( this != rootc)  gettextoutput(errs);
	if(level <= 0) { 	// generate output for all descendents
		for (auto &i : children)
			i->generateBranch(errs,level-1);
	}
}
// -----------------------------------------------------------------------------------
