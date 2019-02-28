

#include <utility>
#include "Node.h"
#include "NodeLocator.h"
#include "NodeVal.h"

#include "support/File.h"
#include "support/Regex.h"
#include "support/Convert.h"
#include "support/Env.h"
#include "support/Timing.h"
#include "support/db/Query.h"

using namespace Support;
using namespace Support::Db;

string Node::kRootFilename="index";


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
//	bld->_Root = "/"; //.clear();
}
//-------------------------------------------------------------------
// load template list for the (content) node
// It should probably be part of nodeval
void Node::Layout(Messages& errs) {
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
			text << "Found a Layout " << layout() << " at Node " + idstr + " for Technology " << env.technology().name << " with a null template list.";
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

//-------------------------------------------------------------------
void Node::addpage(Messages& errs,NodeFilename* filename) {
	if ( this == rootc ) {
		return;
	} else {
		size_t page = numpages(); //current page we are creating..
		if (errs.verbosity() > 6 ) {
			ostringstream text;
			text << "Node " << idstr << " currently on page " << page << " page of " << (size_t)templateCount();
			errs << Message(info,text.str());
		}
		string emptyname;

		if(page >= templateCount()) {
			ostringstream text;
			text << " Attempt to create pages for too few templates; " << page << " requested; " << (size_t)templateCount() << " defined. Will leave it empty";
			errs << Message(warn,text.str());
			return;
		}
		filename->setPageName(basefilename(), page);
		filename->getFullName(Filename(page), emptyname);
		filetemplate curtemplate;
		size_t tid = Template(page);
		if (errs.verbosity() > 6 ) {
			ostringstream text; text << "Node " << idstr << " page " << page << " has template ID " << tid;
			errs << Message(info,text.str() );
		}

		templatemap::const_iterator nmi = bld->TemplateList.find(tid);
		if (nmi != bld->TemplateList.end()) {
			curtemplate = nmi->second.second; // My order
			string suff;
			bld->node_stack.push_back(this); //tid
			ostringstream tname;  tname << "Suffix " << tid;
			suff = macro::process(Logger::log,tname.str(),curtemplate.suffix); //name must be unique...
			bld->node_stack.pop_back();
			Suffix(page,suff);        //Now we need to deal with it as macrotext..
			if (errs.verbosity() > 6 ) errs << Message(info, "Node " + idstr + " page " << page << " has suffix " << suff );
			if ( bld->roots == nullptr ) {
				errs << Message(error, "Suffix Root is nullptr!" );
			} else {
				//This has set up the suffixes..
				//We need to find the ancestor suffix so that any links we create can deal with the post-processed versions!
				Node *x = bld->roots->nodebylinkref(suff);

				if (x == nullptr)  {
					string fname = Filename(page);
					filename->getFullName(fname, suff);
					SetFfilename(page,fname);
				} else {
					Node *vx = x->nodebypath(bld->finalsuffix.begin(),bld->finalsuffix.end());
					if ( vx == nullptr) {
						errs << Message(error, "Error of some sort in suffix table" );
					} else {
						suff = vx->suffix();
					}
					string fname = Filename(page);
					filename->getFullName(fname, suff);	//This now works!  Amazing.
					if (errs.verbosity() > 6 ) errs << Message(info, "Node " + idstr + " page " << page << " is set with filename " << fname );
					SetFfilename(page,fname);
				}
			}
			incPageCount();
		} else {
			errs << Message(error, "Template " << tid << " at Node " << id() << ",Technology " << bld->Tech << " referenced but does not exist." );
		}
	}
	if (errs.verbosity() > 6 ) errs << Message(info, "Node " + idstr + " Finished adding page " );
}
//-------------------------------------------------------------------
//This is the beginning of the node build.
void Node::gettextoutput(Messages& errs) {
	string in_s,baseurl_a,baseurl_s,langtech_s;
	Timing::setTiming('N');
	Env::get("LTPATH",langtech_s);
	bool inteam = false;
	auto it = bld->Teams.find(team());
	if ( it != bld->Teams.end() ) inteam = true;
	if (errs.verbosity() > 0) {
		ostringstream line_oss;
		if (inteam) {
			bld->baseurl(baseurl_a,Build::Editorial);
			if ( Regex::available(errs) ) {
				const string pattern = "^([^0]*)0([^0]*)0([^0]*)$";
				string nodepath = bld->nodepath;
				ostringstream sub;
				sub << "\\1" << this->id() << "\\2" << bld->lng << "\\3";
				Regex::replace(pattern,sub.str(),nodepath);
				line_oss << nodepath;
			} else {
				line_oss << "/en/nnedit.mxs?" << this->id() << "&d&" << bld->lng;
			}
		}
		string enctitle = title();
		if (enctitle.empty()) enctitle = linkref();
		if (bld->dev) {
			bld->baseurl(baseurl_s,Build::Preview);
		} else {
			bld->baseurl(baseurl_s,Build::Public);
		}
		if (enctitle.length() > 60) {
			enctitle = enctitle.substr(0,60);
			enctitle = enctitle.append("...");
		}
		bool did_url = false;
		for(size_t i = 0; i < templateCount() && !did_url; i++) {
			if (Suffix(i) != "XXX") {
				errs << Message(even << I;
				if (inteam) {
					*Logger::log << urli << baseurl_a << line_oss.str() << urlt << "*" << urlo << " ";
				} else {
					*Logger::log << "+ ";
				}
				*Logger::log << this->id() << " - " << urli << baseurl_s << "/" << langtech_s << Ffilename(0) << urlt << Log::RI << enctitle << Log::RO << urlo );
				did_url = true;
			}
		}
		if (!did_url) {
			errs << Message(structure << I;
			if (inteam) {
				*Logger::log << urli << baseurl_a << line_oss.str() << urlt << "*" << urlo << " ";
			} else {
				*Logger::log << "+ ";
			}
			*Logger::log << this->id() << " - " << Log::RI << enctitle << Log::RO );
		}
	}
	//--
	string bk;
	for(size_t i = 0; i < this->templateCount(); i++) {
		if (bld->showTemplate && this->templateCount() > 1) {
			errs << Message(info << I << " Template (" << i + 1 << ")" );
		}
		bld->Page = i;

		size_t tnum = this->Template(i);
		filetemplate curtemplate;
		templatemap::const_iterator nmi = bld->TemplateList.find(tnum);
		string templatename("Undefined Template");
		if (nmi != bld->TemplateList.end()) {
			curtemplate = nmi->second.second;  // My order
			templatename = "[" + nmi->second.first + "]";  // The template's name
			bld->Suffix = curtemplate.suffix;
			in_s = curtemplate.str;  //Starter for template
			bk = curtemplate.br; //Template's own break.
		} else {
			errs << Message(warn << "Template " << tnum << " at Node " << this->id() << ",Technology " << bld->Tech << " referenced but does not exist." );
			bld->Suffix = "";
			in_s = "";
			bk = "";
		}

		Macro::environ TemplateEnv = macro::topEnv();
		if ( bk.length() > 0 ) {
			macro::pushEnv({false,"\n","\t"}); //do not rembr, \n  = newline, tabs are tabs.
			bld->node_stack.push_back(this);
			string crlfName = "CR:" + to_string(String::Digest::hash(bk));
			TemplateEnv.crlf = macro::process(Logger::log,crlfName,bk);
			bld->node_stack.pop_back();
			macro::popEnv();
		}
		macro::pushEnv(TemplateEnv);
		{
			if (in_s.length() > 0) {
				string outval;
				if (in_s.length() > 0) {						//fandr may remove the last char!
					bld->node_stack.push_back(this);
					string tnumStr = to_string(tnum);
					outval = macro::process(Logger::log,"Template " + tnumStr,in_s);
					bld->node_stack.pop_back();
				}
				outputtofile(i,outval);
			}
			if (bld->showTemplate && this->templateCount() > 1 && errs.verbosity()  > 0) {
				*Logger::log << O; //end of show template.
			}
			in_s.clear();
			bk.clear();
		}
		macro::popEnv();
	}

	//-- This used to be in genfiles, but then happened once for each file, rather than one for each Node.
	if (bld->dbc->dbselected() && bld->dbc->table_exists("bldnode") ) {
		ostringstream qstr;
		qstr << "UPDATE LOW_PRIORITY bldnode SET filename='" << this->Ffilename(0) << "' WHERE id=" << this->id();
		Query* q = bld->dbc->query(qstr.str());
		if (! q->execute() ) {
			errs << Message(error << "Node::gettextoutput(): DB Error" );
		}
		delete q;
	}
	bld->resetNodePersistance();	//Delete *vars.
	if (bld->showTiming && errs.verbosity()  > 3) {
		errs << Message(timing << Log::Ntime << " build time for node: " << linkref() );
	}
	if (errs.verbosity()  > 0) *Logger::log << O; //end of node.
}
//-----------------------------------------------------------------
// We shall generate each file here, and then post-process it until its suffix is at A2 of the processor tree.
// We use A2, because we are allowed multiple suffixes!
//-----------------------------------------------------------------
void Node::outputtofile(size_t page, string& out) {
	File tmpScrp(bld->scrDir);
	File tmpFil1(bld->tmpDir);
	File tmpFil2(bld->tmpDir);
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
		Node *x = bld->roots->nodebylinkref(suff);		// Find the post-processor NodeSuffix.  We have the file for this now.
		if (x != nullptr) {								// We do have a post-processing entry, so lets use it.
			Node* y = x->nodebypath(bld->finalsuffix.begin(),bld->finalsuffix.end());
			while ( x != y && x != nullptr) {					// While x is not y, and x is not null, use the scriptpath for x
				if (!postprocessed && bld->showFiling) {
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
}
// -----------------------------------------------------------------------------------
// generate basic text output for all nodes below Node n in the Node tree */
// and 'n's direct ancestors for link generation */
void Node::generateoutput(int level) {
	if(level >= 0 && nodeparent != nullptr ) {// generate output for all direct ancestors
		nodeparent->generateoutput(level+1);
	}
	if( this != bld->rootc) {
		gettextoutput();
	}
	if(level <= 0) { 	// generate output for all descendents
		for (auto &i : children)
			i->generateoutput(level-1);
	}
}
// -----------------------------------------------------------------------------------
// generate basic text output for all nodes below Node n in the Node tree */
void Node::generatebranch(int level) {
	if( this != bld->rootc)  gettextoutput();
	if(level <= 0) { 	// generate output for all descendents
		for (auto &i : children)
			i->generatebranch(level-1);
	}
}
// -----------------------------------------------------------------------------------
