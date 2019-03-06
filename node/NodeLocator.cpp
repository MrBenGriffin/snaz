//
// Created by Ben on 2019-02-27.
//

#include "Node.h"
#include "NodeLocator.h"
#include "BadNodeLocatorPath.h"
#include "support/Convert.h"

bool NodeLocator::showPaths = false;
using namespace Support;

//-----------------------------------------------------------------------------
//This discards the processed output. (e.g. in nodelocation)
void NodeLocator::processinput(string::const_iterator& in,string::const_iterator& out) {
	bool done = in >= out;	// 'are we finished?' flag
	while(!done) 	{
		unsigned char fni = *in;
		while(in < out && fns[fni] == nullptr) in++;
		done = (this->*(fns[fni]))();
	}
}

//-----------------------------------------------------------------------------
// the null function
bool NodeLocator::dostop() {
	return true;	// termination condition reached
}

//-----------------------------------------------------------------------------

/*    given a string in the format shown above and null terminated      */
/* with no white spaces, returns the appropriate Node pointer (find)    */
/************************************************************************/
// / - root [doRoot]
// ! - linkref [doLinkRef]
// : - page [doPage]
// 0 - i-th child [doChildLeft]
// m - main node (that the content belongs to)
// n - (n-i)-th child [doChildRight]
// + - i-th sibling to right of me [doSiblingRight]
// - - i-th sibling to left of me [doSiblingLeft]
// ../ - parent [doParent]
// [0-9] - id [doID]
// An - ancestor at tier n
// ^n - ancestor up n tiers
// R[+/-] - recommended root [doRR]
// F() - fixed tier traversal; next peer [doPeerNext]
// B() - fixed tier traversal; previous peer [doPeerLast]
// I - call_stack from inside
// O - call_stack from outside
// @ - a slight possibility of an unexpressed macro?

/////////////////////////////////////////////////////////////
// Node Locator Methods //
/////////////////////////////////////////////////////////////
string NodeLocator::loc_path;

// set the default response to be 'illegal character', which throws an exception.
NodeLocator::NodeLocator() :  pageNum(0),find(nullptr),from(nullptr),root(nullptr) {
//	fns.fill(&NodeLocator::doIllegalCharacter);
	fns[static_cast<unsigned char>('T')] = (Fn) &NodeLocator::doTaxonomy;
	fns[static_cast<unsigned char>('C')] = (Fn) &NodeLocator::doContent;
	fns[static_cast<unsigned char>('S')] = (Fn) &NodeLocator::doSuffix;
	fns[static_cast<unsigned char>('!')] = (Fn) &NodeLocator::doLinkRef;
	fns[static_cast<unsigned char>(':')] = (Fn) &NodeLocator::doPage;
	fns[static_cast<unsigned char>('0')] = (Fn) &NodeLocator::doChildLeft;
	fns[static_cast<unsigned char>('m')] = (Fn) &NodeLocator::doMainNode;
	fns[static_cast<unsigned char>('n')] = (Fn) &NodeLocator::doChildRight;
	fns[static_cast<unsigned char>('+')] = (Fn) &NodeLocator::doSiblingRight;
	fns[static_cast<unsigned char>('-')] = (Fn) &NodeLocator::doSiblingLeft;
	fns[static_cast<unsigned char>('.')] = (Fn) &NodeLocator::doParent;
	for(auto c = static_cast<unsigned char>('1'); c <= static_cast<unsigned char>('9'); c++) {
		fns[c] = (Fn) &NodeLocator::doID;
	}
	fns[static_cast<unsigned char>('A')] = (Fn) &NodeLocator::doAncestorAbsolute;
	fns[static_cast<unsigned char>('^')] = (Fn) &NodeLocator::doAncestorRelative;
	fns[static_cast<unsigned char>('R')] = (Fn) &NodeLocator::doRR;
	fns[static_cast<unsigned char>('F')] = (Fn) &NodeLocator::doPeerNext;
	fns[static_cast<unsigned char>('B')] = (Fn) &NodeLocator::doPeerLast;
	fns[static_cast<unsigned char>('I')] = (Fn) &NodeLocator::doStackFromInside;
	fns[static_cast<unsigned char>('O')] = (Fn) &NodeLocator::doStackFromOutside;
}

void NodeLocator::setdirty() {
	if (!_dirty.empty()) {
		_dirty.top() = true;
	}
}

// return the Node described by path from 'from'
Node* NodeLocator::locate(Messages& _errs,Node *fromref, string::const_iterator pin,string::const_iterator pout) {
	errs = &_errs; //load up current message context.
	message.str("");
	pageNum = 0;  start=pin; in = pin;  out = pout;
	if( (in < out) && (*in == '/') ) {
		if (showPaths) message << "/" ;
		from = root;
		in++;
	} else {
		from = fromref;
	}
	try {
		processinput(in,out);
	}
	catch(BadNodeLocatorPath& e) {
		find = nullptr; pin = pout;
	}
	catch( std::exception& e ) {
		message << "Location error at " << e.what() <<  " with location text:" << string(pin,pout);
		*errs << Message(error,message.str());
		find = nullptr;
		pin = pout;
	}
	catch(...) {
		message << "Unknown Location error in " << string(pin,pout);
		*errs << Message(error,message.str());
		find = nullptr;
		pin = pout;
	}
	if (showPaths) { *errs << Message(info,message.str()); }
	return find;
}

// continue to the next path section
bool NodeLocator::nextPathSection() {
	if(in >= out) {
		return true; // Finished path!
	} else {
		from = find;
		if( *in == '/') {
			if (showPaths) message << "|" ;
			in++;
			if (*in == '\0')
				return true;  // ../../
		} else {
			if( *in != ':') {
				throw BadNodeLocatorPath(errs,start,in,out);
			}
			if (showPaths) message << ":" ;
		}
		return false;
	}
}

// Content - switch over to global Content Nodetree (the default)
bool NodeLocator::doContent() {
	if (showPaths) message << "C" ;
	in++;
	from = Node::rootc;
	find = Node::rootc->nodebypath(*errs, in, out );
	return find != nullptr;
}

// Taxonomy - switch over to global Taxonomy Nodetree
bool NodeLocator::doTaxonomy() {
	if (showPaths) message << "T" ;
	in++;
	from = Node::roott;
	find = Node::roott->nodebypath(*errs, in, out );
	return find != nullptr;
}

// Content - switch over to global Suffix Nodetree
bool NodeLocator::doSuffix() {
	if (showPaths) message << "S" ;
	in++;
	from = Node::roots;
	find = Node::roots->nodebypath(*errs, in, out );
	return find != nullptr;
}

// next peer according to specified criteria
bool NodeLocator::doPeerNext() {
	if (showPaths) message << "F" ;
	in++;	// F
	// compulsary brackets
	if(in == out || *in != '(') {
		throw BadNodeLocatorPath(errs,start,in,out);
	}
	in++;

	// brackets must be balanced
	string rootString;
	for(int level = 1; level > 0;) {
		if(in == out)	{
			throw BadNodeLocatorPath(errs,start,in,out);
		} else {
			if(*in == '(') //bracket balancing.
				level++;
			else if(*in == ')')
				level--;
			rootString += *in;
			in++;
		}
	}
	rootString.erase(rootString.size()-1);
	NodeLocator rootLocator;
	rootLocator.setroot(root);
	NodeLocator::loc_path=rootString;
	Node* lRoot = rootLocator.locate(*errs, from, rootString.begin(), rootString.end() );
	find = from->nodenext(*errs,lRoot);
	if( find == nullptr)
		return true;
	else
		return nextPathSection();
}

// next peer according to specified criteria
bool NodeLocator::doPeerLast() {
	if (showPaths) message << "B" ;
	in++;	// B
	// compulsary brackets
	if(in == out || *in != '(') {
		throw BadNodeLocatorPath(errs,start,in,out);
	}
	in++;
	// brackets must be balanced
	string rootString;
	for(int level = 1; level > 0;) {
		if(in == out)	 {
			throw BadNodeLocatorPath(errs,start,in,out);
		} else {
			if(*in == '(')
				level++;
			else if(*in == ')')
				level--;
			rootString += *in;
			in++;
		}
	}
	// trailing ')'
	rootString.erase(rootString.size()-1);
	NodeLocator rootLocator;
	rootLocator.setroot(root);
	NodeLocator::loc_path=rootString;
	Node *lRoot = rootLocator.locate(*errs, from, rootString.begin(), rootString.end() );
	find = from->nodelast(*errs, lRoot);
	if( find == nullptr)
		return true;
	else
		return nextPathSection();
}

// recommended root (RR) (traversal of the entire tree)
// this does a depth-first traversal rewritten without recursion!
bool NodeLocator::doRR() {
	in++;	// R
	if(in == out) {
		find = from->nodebytw(*errs, +1);
	} else {
		switch ( *in ) {
			case '-':
				if (showPaths) message << "R-" ;
				in++;
				find = from->nodebytw(*errs, -1);
				break;
			case '+':
				if (showPaths) message << "R+" ;
				find = from->nodebytw(*errs, +1);
				in++;
				break;
			case 'L': { //find first node with matching layout in descendents of current node (depth first traversal).
				//current 110/RL (use layout of 110 and find next)
				in++;
				bool next = false;
				size_t layout = from->layout(); //default
				if (in != out) {
					if(*in=='N') { //RLN find next layout (unrooted).
						if (showPaths) message << "RLN{" << layout  << "}";
						next = true;
						in++;
					} else {
						pair<size_t,bool> ilayout= znatural(in); //eg RL33
						if (!ilayout.second) {
							throw BadNodeLocatorPath(errs,start,in,out);
						} else {
							layout = ilayout.first;
						}
						if (showPaths) message << "RL" <<  layout;
					}
				}
				//at this point we have some sort of layout.
				find = from->nodebytw(*errs, +1); //next node..
				if (showPaths) message << "(" << find->id() << "; " << find->layout() << ")";
				if (next) { //carry on..
					while (find->id() != from->id() && find->layout() != layout) {
						find = find->nodebytw(*errs, +1); //next node..
						if (showPaths) message << "(" << find->id() << "; " << find->layout() << ")";
					}
					//at this point either find = from or find has a layout..
					if(find->id() == from->id() ) {
						if (showPaths) message <<  ";RLN [from " << from->id() << ", with layout " << layout << "] Not found";
						find = nullptr;
					} else {
						if (showPaths) message << ";RLN [from " << from->id() << ", with layout " << layout << "] found at " << find->id();
					}
				} else {
					while (find->tier() > from->tier() && find->layout() != layout) {
						find = find->nodebytw(*errs, +1); //next node..
						if (showPaths) message << "(" << find->id() << "; " << find->layout() << ")";
					}
					if(find->tier() <= from->tier()) {
						if (showPaths) message << ";RL" << layout << "[from " << from->id() << "] Not found";
						find = nullptr;
					} else {
						if (showPaths) message << ";RL" << layout << " found at " << find->id() ;
					}
				}
			} break;
			default: {
				throw BadNodeLocatorPath(errs,start,in,out);
			}
		}
	}
	if(find == nullptr)  return true;
	return nextPathSection();
}

// ID
bool NodeLocator::doID() {
	pair<size_t,bool> id= znatural(in);
	if (!id.second) {
		throw BadNodeLocatorPath(errs,start,in,out);
	} else {
		if (showPaths) message << "id(" << id.first << ")";
		find = from->nodebyid(id.first);
		if (find != nullptr) {
			return nextPathSection();
		} else {
			return true;
		}
	}
}

// mainNode
bool NodeLocator::doMainNode() {
	find = nullptr;
	if (showPaths) message << "m";
	in++;
	setdirty();
	if(!Node::nodeStack.empty()) {
		find = Node::nodeStack.front();
		return nextPathSection();
	} else {
		return true;
	}
}


// parent
bool NodeLocator::doParent() {
	if (showPaths) message << "^";
	if(from == root) {
		find = nullptr;
		return true;
	}
	in++;
	if(in == out || *in != '.') {
		throw BadNodeLocatorPath(errs,start,in,out);
	}
	in++;
	find = from->parent();
	if( find == nullptr)  return true;
	return nextPathSection();
}

// i-th sibling to right of me or me (with +0)
bool NodeLocator::doSiblingRight() {
	if (showPaths) message << "+";
	in++;	// +
	if(from == root) {
		if(in != out && *in == '0') {
			find = root;
			in++;
		} else {
			find = nullptr;
			return true;
		}
	} else {
		if (in != out) {
			if (*in == '0' && (in+1 == out || !isdigit(in[1]))) {  //self.
				in++;
				find = from;
			} else {
				Node *pn = from->parent();
				pair<size_t,bool> offset= znatural(in);
				if (!offset.second) {
					throw BadNodeLocatorPath(errs,start,in,out);
				} else {
					if (showPaths) message << "[" << offset.first << "]";
					unsigned long sibnum = from->siblingnum() + offset.first;
					if(sibnum >= pn->getNumchildren()) {
						find = nullptr;
						return true;
					}
					find = (pn->child(*errs, (size_t)sibnum));
					if( find == nullptr) { return true; }
				}
			}
		}
	}
	return nextPathSection();
}

// i-th sibling to left of me
bool NodeLocator::doSiblingLeft() {
	if (showPaths) message << "-";
	in++;
	if(from == root) {
		if(in != out && *in == '0') {
			find = root;
			in++;
		} else {
			find = nullptr;
			return true;
		}
	} else {
		Node *pn = from->parent();
		pair<size_t,bool> offset= znatural(in);
		if (!offset.second) {
			throw BadNodeLocatorPath(errs,start,in,out);
		} else {
			if (showPaths) message << "[" << offset.first << "]";
			unsigned long sibnum = from->siblingnum() - offset.first;
			if(sibnum >= pn->getNumchildren()) {
				find = nullptr;
				return true;
			}
			find = (pn->child(*errs, (size_t)sibnum));
			if( find == nullptr) { return true; }
		}
	}
	return nextPathSection();
}

// i-th child from left
bool NodeLocator::doChildLeft() {
	if (showPaths) message << "0";
	if (from == nullptr) from = root;
	in++;
	if(in == out) {
		throw BadNodeLocatorPath(errs,start,in,out);
	} else {
		switch(*in) {
			case '+': { //0+xx
				in++;
				if (in != out) {
					pair<size_t,bool> sibnum= znatural(in);
					if (!sibnum.second) {
						throw BadNodeLocatorPath(errs,start,in,out);
					} else {
						if (showPaths) message << "[" << sibnum.first << "]";
						if(sibnum.first < 1 || sibnum.first > from->getNumchildren()) {
							find = nullptr;
							return true; //RANGE error
						}
						find = (from->child(*errs, sibnum.first - 1 ));
						if( find == nullptr)  { return true;}
						if (showPaths) { message << "={" << find->id() << "}"; }
					}
				} else {
					throw BadNodeLocatorPath(errs,start,in,out);
				}
			} break;
			case 'L': { //0L12 return the first *child* node that matches the layout 'xx'
				in++;
				if (in != out) {
					pair<size_t,bool> ilayout= znatural(in);
					if (!ilayout.second) {
						throw BadNodeLocatorPath(errs,start,in,out);
					} else {
						if (showPaths) message << "0L[" << ilayout.first << "]";
						find = nullptr; // not found yet
						size_t kids = from->getNumchildren();
						for (size_t i =0; i < kids; i++) {
							Node* cn = from->child(*errs, i);
							if (cn->layout() == ilayout.first) {
								find = cn;
								break;
							}
						}
						if(find == nullptr) {
							return true;
						}
					}
				} else {
					throw BadNodeLocatorPath(errs,start,in,out);
				}
			} break;
			default: {
				throw BadNodeLocatorPath(errs,start,in,out);
			}
		}
	}
	return nextPathSection();
}

// i-th child from right
bool NodeLocator::doChildRight() {
	if (showPaths) message << "N-";
	in++;
	if(in == out || *in != '-') {
		throw BadNodeLocatorPath(errs,start,in,out);
	}
	in++;
	pair<size_t,bool> offset= znatural(in);
	if (!offset.second) {
		throw BadNodeLocatorPath(errs,start,in,out);
	} else {
		if (showPaths) message << "[" << offset.first << "]";
		size_t sibnum = from->getNumchildren() - offset.first;
		if(sibnum < 1 || sibnum > from->getNumchildren()) {
			find = nullptr;
			return true;
		}
		find = (from->child(*errs,sibnum-1));
		if( find == nullptr) { return true;}
	}
	return nextPathSection();
}

// page
bool NodeLocator::doPage() {
	in++;
	find = from;
	pair<size_t,bool> page= znatural(in);
	if (page.second) {
		if (showPaths) message << "[" << page.first << "]";
		if ( page.first >= find->templateCount() ) {
			find = nullptr;	//No pages at that count.
		} else {
			pageNum = page.first;
		}
		if(in != out) {
			throw BadNodeLocatorPath(errs,start,in,out);
		}
	} else {
		pageNum = 0;
	}
	return true;	// stop now
}

// link ref
bool NodeLocator::doLinkRef() {
	in++;       // skip '!'
	string::const_iterator inx = in;
	while (in != out && *in != ':' && *in != '/' ) in++;
	string linkref(inx,in);
	if (showPaths) message << "!" << linkref;
	Node* result = root->nodebylinkref(*errs,linkref);
	if (result != nullptr ) {
		find = result;
	} else {
		find = nullptr;
		return true;
	}
	return nextPathSection();
}

// Ancestor at tier n
bool NodeLocator::doAncestorAbsolute() {
	if(from == root) {	// root illegal here
		if (showPaths) message << "(/A illegal!)";
		find = nullptr;
		return true;
	}
	in++;
	if(in != out && *in == 'L') {
		in++;
		pair<size_t,bool> ilayout= znatural(in);
		if (!ilayout.second) {
			throw BadNodeLocatorPath(errs,start,in,out);
		} else {
			if (showPaths) message << "AL[" << ilayout.first << "]";
			find = nullptr; // not found yet
			Node* cn = from;
			while(cn != root) {
				if(cn->layout() == ilayout.first) {
					find = cn;
					cn = root;
				} else {
					cn = cn->parent();
				}
			}
			if(find == nullptr) {
				return true;
			}
		}
	} else {
		pair<size_t,bool> itier= znatural(in);
		if (!itier.second) {
			throw BadNodeLocatorPath(errs,start,in,out);
		} else {
			if (showPaths) message << "A[" << itier.first <<  "]" ;
			find = from;
			while(( find->tier() > itier.first) && (find != root)) {
				find = find->parent();
			}
		}
	}
	if( find == nullptr)  return true;
	return nextPathSection();
}

// Ancestor up n tiers
bool NodeLocator::doAncestorRelative() {
	if(from == root) { // root illegal here
		if (showPaths) message << "(/^ illegal!)";
		find = nullptr;
		return true;
	}
	in++;
	if(in != out && *in == 'L') {
		in++;
		pair<size_t,bool> ilayout= znatural(in);
		if (!ilayout.second) {
			throw BadNodeLocatorPath(errs,start,in,out);
		} else {
			if (showPaths) message << "^L[" << ilayout.first << "]";
			find = from;
			while( find->layout() != ilayout.first) { //this assumes that the node is actually content!
				find = find->parent();
				if(find == root) {
					find = nullptr;
					return true;
				}
			}
		}
	} else {
		pair<size_t,bool> up= znatural(in);
		if (!up.second) {
			throw BadNodeLocatorPath(errs,start,in,out);
		} else {
			if (showPaths) message << "^[" << up.first <<  "]" ;
			find = from;
			while(up.first > 0) {
				up.first--;
				find = find->parent();
				if(find == root) {
					find = nullptr;
					return true;
				}
			}
		}
	}
	return nextPathSection();
}

bool NodeLocator::doStackFromInside() {
	in++;       // skip 'I'
	pair<size_t,bool> offset= znatural(in);
	if (!offset.second) {
		throw BadNodeLocatorPath(errs,start,in,out);
	} else {
		size_t stsize = 	Node::nodeStack.size();
		if (offset.first!=0) {
			setdirty();
			//			dirtmsg << "I-Stack must be 0 Path:"+NodeLocator::loc_path;
		}
		if (showPaths) message << "I" << offset.first;
		Node* result = nullptr;
		if (stsize < 2) { //we are in template.
			if ((size_t)offset.first < stsize) {
				result = Node::nodeStack[stsize-((size_t)offset.first+1)]; //offset=0, size=1; 1-(0+1) = 0;
			}
		} else {
			if ((size_t)offset.first < (stsize-1)) {
				result = Node::nodeStack[stsize-((size_t)offset.first+1)]; //offset=0, size=1; 1-(0+1) = 0;
			}
		}
		if (result != nullptr ) {
			find = result;
		} else {
			find = nullptr;
			return true;
		}
	}
	return nextPathSection();
}

bool NodeLocator::doStackFromOutside() {
	in++;       // skip 'O'
	pair<size_t,bool> offset= znatural(in);
	if (!offset.second) {
		throw BadNodeLocatorPath(errs,start,in,out);
	} else {
		size_t stsize = 	Node::nodeStack.size();
		setdirty();
		if (showPaths) message << "O" << (size_t)offset.first;
		Node* result = nullptr;
		if (stsize < 2) { //we are in template.
			if (offset.first < stsize) {
				result = Node::nodeStack[offset.first];
			}
		} else {
			if (1+offset.first < stsize) {
				result = Node::nodeStack[1+offset.first];
			}
		}
		if (result != nullptr ) {
			find = result;
		} else {
			find = nullptr;
			return true;
		}
	}
	return nextPathSection();
}

// illegal character
bool NodeLocator::doIllegalCharacter() {
	if (showPaths) message << "(illegal character)" ;
	find = nullptr;
	return true;
}
