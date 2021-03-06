//
// Created by Ben on 2019-02-27.
//

#include "node/Node.h"
#include "node/Tree.h"
#include "node/Locator.h"
#include "node/BadLocatorPath.h"
#include "node/Content.h"
#include "node/Taxon.h"
#include "node/Suffix.h"
#include "support/Convert.h"
#include "content/Layout.h"

using namespace Support;
namespace node {

	bool Locator::showPaths = false;

	//-----------------------------------------------------------------------------
//This discards the processed output. (e.g. in nodelocation)
	void Locator::process(Messages &errs) {
		bool done = in >= out;    // 'are we finished?' flag
		while (!done) {
			switch(*in) {
				case '!':done = doLinkRef(errs); break;
				case '+':done = doSiblingRight(errs); break;
				case '-':done = doSiblingLeft(errs); break;
				case '.':done = doParent(errs); break;
				case '0':done = doChildLeft(errs); break;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':done = doID(errs); break;
				case ':':done = doPage(errs); break;
				case '^':done = doAncestorRelative(errs); break;
				case 'A':done = doAncestorAbsolute(errs); break;
				case 'B':done = doPeerLast(errs); break;
				case 'C':done = doContent(errs); break;
				case 'F':done = doPeerNext(errs); break;
				case 'I':done = doStackFromInside(errs); break;
				case 'm':done = doMainNode(errs); break;
				case 'n':done = doChildRight(errs); break;
				case 'O':done = doStackFromOutside(errs); break;
				case 'R':done = doRR(errs); break;
				case 'S':done = doSuffix(errs); break;
				case 'T':done = doTaxonomy(errs); break;
				case 'W':done = doTW(errs); break;
				default:
					done = doIllegalCharacter(errs);
			}
		}
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
//string Locator::loc_path;


// set the default response to be 'illegal character', which throws an exception.
	Locator::Locator() : pageNum(0), find(nullptr), from(nullptr), root(nullptr) {}

	Locator::Locator(const Metrics* m,node::Node const* _root, node::Node const* _from) : Locator() {
		metrics = m;
		root = _root; from=_from;
	}

	Locator::Locator(const Metrics* m) : Locator() {
		metrics = m;
	}

	Locator::Locator(const Locator* o,const Metrics* m) :
			pageNum(o->pageNum),find(o->find),from(o->from),root(o->root),metrics(m) {
	}

	void Locator::setdirty() {
		if (!_dirty.empty()) {
			_dirty.top() = true;
		}
	}

	void Locator::setFrom(const Node* node) {
		from = node;
	}

	void Locator::setRoot(const Node* node) {
		root = node;
	}

	const Node* Locator::get(Support::Messages&errs,const string& path) {
		return locate(errs,path.begin(),path.end());
	}

	// return the Node described by path from 'from'
	const Node* Locator::locate(Messages &errs, string::const_iterator pin, string::const_iterator pout) {
		message.str("");
		pageNum = 0;
		start = pin;
		in = pin;
		out = pout;
		if ((in < out) && (*in == '/')) {
			if (showPaths) message << "/";
			from = root;
			find = root;
			in++;
		}
		try {
			process(errs);
		}
		catch (BadLocatorPath &e) {
			find = nullptr;
			pin = pout;
		}
		catch (std::exception &e) {
			message << "Location error at " << e.what() << " with location text:" << string(pin, pout);
			errs << Message(error, message.str());
			find = nullptr;
			pin = pout;
		}
		catch (...) {
			message << "Unknown Location error in " << string(pin, pout);
			errs << Message(error, message.str());
			find = nullptr;
			pin = pout;
		}
		if (showPaths) { errs << Message(info, message.str()); }
		return find;
	}

// continue to the next path section
	bool Locator::nextPathSection(Messages &errs) {
		if (in >= out) {
			return true; // Finished path!
		} else {
			from = find;
			if (*in == '/') {
				if (showPaths) message << "|";
				in++;
				if (*in == '\0')
					return true;  // ../../
			} else {
				if (*in != ':') {
					throw BadLocatorPath(errs, start, in, out);
				}
				if (showPaths) message << ":";
			}
			return false;
		}
	}

	bool Locator::doTW(Messages &errs) {
		pair<size_t, bool> tw = znatural(++in);
		if (!tw.second) {
			throw BadLocatorPath(errs, start, in, out);
		} else {
			if (showPaths) {
				ostringstream log; log << "tw(" << tw.first << ")";
				errs << Message(debug,log.str());
			}
			find = from != nullptr ? from->tree()->tw(errs,tw.first,0) : nullptr;
			if (find != nullptr) {
				return nextPathSection(errs);
			} else {
				return true;
			}
		}
	}

// Content - switch over to global Content Nodetree (the default)
	bool Locator::doContent(Messages &errs) {
		if (showPaths) message << "C";
		in++;
		from = Content::editorial.root();
		root = from;
		find = nullptr;
		return false;
	}


	// Taxonomy - switch over to global Taxonomy Nodetree
	bool Locator::doTaxonomy(Messages &errs) {
		if (showPaths) message << "T";
		in++;
		from = Taxon::taxonomies.root();
		root = from;
		find = nullptr;
		return false;
	}

// Content - switch over to global Suffix Nodetree
	bool Locator::doSuffix(Messages &errs) {
		if (showPaths) message << "S";
		in++;
		from = Suffix::suffixes.root();
		root = from;
		find = nullptr;
		return false;
	}

	const Node* Locator::peerPrep(Messages &errs) {
		// compulsory brackets
		if (in == out || *in != '(') {
			throw BadLocatorPath(errs, start, in, out);
		}
		in++;

		// brackets must be balanced
		string rootString;
		for (int level = 1; level > 0;) {
			if (in == out) {
				throw BadLocatorPath(errs, start, in, out);
			} else {
				if (*in == '(') //bracket balancing.
					level++;
				else if (*in == ')')
					level--;
				rootString += *in;
				in++;
			}
		}
		rootString.erase(rootString.size() - 1);
		Locator rootLocator(metrics,root,from); //
		return rootLocator.locate(errs, rootString.begin(), rootString.end());
	}

// next peer according to specified criteria
	bool Locator::doPeerNext(Messages &errs) {
		if (showPaths) { message << "F"; }
		in++;    // F
		const Node *lRoot = peerPrep(errs);
		find = from->tree()->peer(errs, from , +1, lRoot);
		if (find == nullptr)
			return true;
		else
			return nextPathSection(errs);
	}

// next peer according to specified criteria
	bool Locator::doPeerLast(Messages &errs) {
		if (showPaths) message << "B";
		in++;    // B
		const Node *lRoot = peerPrep(errs);
		find = from->tree()->peer(errs, from, -1, lRoot);
		if (find == nullptr)
			return true;
		else
			return nextPathSection(errs);
	}

// recommended root (RR) (traversal of the entire tree)
// this does a depth-first traversal rewritten without recursion!
	bool Locator::doRR(Messages &errs) {
		in++;    // R
		if (in == out) {
			find = from->offset(errs,+1);
		} else {
			switch (*in) {
				case '-':
					if (showPaths) message << "R-";
					in++;
					find = from->offset(errs,-1);
					break;
				case '+':
					if (showPaths) message << "R+";
					find = from->offset(errs,+1);
					in++;
					break;
				case 'L': { //find first node with matching layout in descendents of current node (depth first traversal).
					//current 110/RL (use layout of 110 and find next)
					in++;
					bool next = false;
					size_t layoutId = from->iGet(errs,layout);
					if (in != out) {
						if (*in == 'N') { //RLN find next layout (unrooted).
							if (showPaths) message << "RLN{" << layoutId << "}";
							next = true;
							in++;
						} else {
							pair<size_t, bool> ilayout = znatural(in); //eg RL33
							if (!ilayout.second) {
								throw BadLocatorPath(errs, start, in, out);
							} else {
								layoutId = ilayout.first;
							}
							if (showPaths) message << "RL" << layoutId;
						}
					}
					auto* layout = content::Layout::get(errs,layoutId);
					if (layout != nullptr) {
						//at this point we have some sort of layoutId.
						auto* content = from->offset(errs,+1)->content();
						if (showPaths) message << "(" << content->id() << "; " << layoutId << ")";
						if (next) { //carry on..
							while (content->id() != from->id() && content->layout()->id != layoutId) {
								content = content->offset(errs,+1)->content();
								if (showPaths) message << "(" << content->id() << "; " << content->layout()->id << ")";
							}
							//at this point either content = from or content has a layout..
							if (content->id() == from->id()) {
								if (showPaths)
									message << ";RLN [from " << from->id() << ", with layout " << layout << "] Not found";
								throw BadLocatorPath(errs, start, in, out);
							} else {
								if (showPaths)
									message << ";RLN [from " << from->id() << ", with layout " << layout << "] found at "
											<< content->id();
							}
						} else {
							while (content->tier() > from->tier() && content->layout()->id != layout->id) {
								content = content->offset(errs,+1)->content();
								if (showPaths) message << "(" << content->id() << "; " << content->layout()->id << ")";
							}
							if (content->tier() <= from->tier()) {
								if (showPaths) message << ";RL" << layout << "[from " << from->id() << "] Not found";
								throw BadLocatorPath(errs, start, in, out);
							} else {
								if (showPaths) message << ";RL" << layout << " found at " << content->id();
							}
						}
						find = content;
					} else {
						throw BadLocatorPath(errs, start, in, out);
					}
				}
					break;
				default: {
					throw BadLocatorPath(errs, start, in, out);
				}
			}
		}
		if (find == nullptr) return true;
		return nextPathSection(errs);
	}

// ID
	bool Locator::doID(Messages &errs) {
		pair<size_t, bool> id = znatural(in);
		if (!id.second) {
			throw BadLocatorPath(errs, start, in, out);
		} else {
			if (showPaths) {
				ostringstream log; log << "id(" << id.first << ")";
				errs << Message(debug,log.str());
			}
			if(from != nullptr) {
				find = from->tree()->node(errs,id.first);
				if (find != nullptr) {
					return nextPathSection(errs);
				} else {
					return true;
				}
			} else {
				errs << Message(error,"from is null within node locator");
				return false;
			}
		}
	}

// mainNode
	bool Locator::doMainNode(Messages &errs) {
		find = nullptr;
		if (showPaths) message << "m";
		in++;
		setdirty();
		find = metrics->current;
		if (find != root) {
			return nextPathSection(errs);
		} else {
			return true;
		}
	}


// parent
	bool Locator::doParent(Messages &errs) {
		if (showPaths) message << "^";
		if (from == root) {
			find = nullptr;
			return true;
		}
		in++;
		if (in == out || *in != '.') {
			throw BadLocatorPath(errs, start, in, out);
		}
		in++;
		find = from->parent();
		if (find == nullptr) return true;
		return nextPathSection(errs);
	}

// i-th sibling to right of me or me (with +0)
	bool Locator::doSiblingRight(Messages &errs) {
		if (showPaths) message << "+";
		in++;    // +
		if (from == root) {
			if (in != out && *in == '0') {
				find = root;
				in++;
			} else {
				find = nullptr;
				return true;
			}
		} else {
			if (in != out) {
				if (*in == '0' && (in + 1 == out || !isdigit(in[1]))) {  //self.
					in++;
					find = from;
				} else {
					const Node *pn = from->parent();
					pair<size_t, bool> offset = znatural(in);
					if (!offset.second) {
						throw BadLocatorPath(errs, start, in, out);
					} else {
						if (showPaths) message << "[" << offset.first << "]";
						unsigned long sibnum = from->sibling() + offset.first;
						if (sibnum >= pn->getChildCount()) {
							find = nullptr;
							return true;
						}
						find = pn->child(errs, (size_t) sibnum);
						if (find == nullptr) { return true; }
					}
				}
			}
		}
		return nextPathSection(errs);
	}

// i-th sibling to left of me
	bool Locator::doSiblingLeft(Messages &errs) {
		if (showPaths) message << "-";
		in++;
		if (from == root) {
			if (in != out && *in == '0') {
				find = root;
				in++;
			} else {
				find = nullptr;
				return true;
			}
		} else {
			const Node *pn = from->parent();
			pair<size_t, bool> offset = znatural(in);
			if (!offset.second) {
				throw BadLocatorPath(errs, start, in, out);
			} else {
				if (showPaths) message << "[" << offset.first << "]";
				unsigned long sibnum = from->sibling() - offset.first;
				if (sibnum >= pn->getChildCount()) {
					find = nullptr;
					return true;
				}
				find = pn->child(errs, (size_t) sibnum);
				if (find == nullptr) { return true; }
			}
		}
		return nextPathSection(errs);
	}

// i-th child from left
	bool Locator::doChildLeft(Messages &errs) {
		if (showPaths) message << "0";
//		if (from == nullptr) from = root;
		in++;
		if (in == out) {
			throw BadLocatorPath(errs, start, in, out);
		} else {
			switch (*in) {
				case '+': { //0+xx
					in++;
					if (in != out) {
						pair<size_t, bool> sibnum = znatural(in);
						if (!sibnum.second) {
							throw BadLocatorPath(errs, start, in, out);
						} else {
							if (showPaths) message << "[" << sibnum.first << "]";
							if (sibnum.first == 1 && from == root) {
								string test(start,out);
								if(test == "/0+1") {
									find = root; //This IS a bit weird.
									errs << Message(deprecated,"2019: /0+1 is deprecated for Root. Use W1 or A1 or /");
								} else {
									if (sibnum.first < 1 || sibnum.first > from->getChildCount()) {
										find = nullptr;
										return true; //RANGE error
									}
									find = (from->child(errs, sibnum.first));
									if (find == nullptr) { return true; }
								}
							} else {
								if (sibnum.first < 1 || sibnum.first > from->getChildCount()) {
									find = nullptr;
									return true; //RANGE error
								}
								//FYI. the node::child() method is 1-indexed!
								find = (from->child(errs, sibnum.first));
								if (find == nullptr) { return true; }
							}
							if (showPaths) { message << "={" << find->id() << "}"; }
						}
					} else {
						throw BadLocatorPath(errs, start, in, out);
					}
				}
					break;
				case 'L': { //0L12 return the first *child* node that matches the layout 'xx'
					in++;
					if (in != out) {
						pair<size_t, bool> ilayout = znatural(in);
						if (!ilayout.second) {
							throw BadLocatorPath(errs, start, in, out);
						} else {
							if (showPaths) message << "0L[" << ilayout.first << "]";
							find = nullptr; // not found yet
							size_t kids = from->getChildCount();
							for (size_t i = 1; i <= kids; i++) {
								const Node *cn = from->child(errs, i);
								if (cn->iGet(errs,layout) == ilayout.first) {
									find = cn;
									break;
								}
							}
							if (find == nullptr) {
								return true;
							}
						}
					} else {
						throw BadLocatorPath(errs, start, in, out);
					}
				}
					break;
				default: {
					throw BadLocatorPath(errs, start, in, out);
				}
			}
		}
		return nextPathSection(errs);
	}

// i-th child from right
	bool Locator::doChildRight(Messages &errs) {
		if (showPaths) message << "N-";
		in++;
		if (in == out || *in != '-') {
			throw BadLocatorPath(errs, start, in, out);
		}
		in++;
		pair<size_t, bool> offset = znatural(in);
		if (!offset.second) {
			throw BadLocatorPath(errs, start, in, out);
		} else {
			if (showPaths) message << "[" << offset.first << "]";
			size_t sibnum = from->getChildCount() - offset.first;
			find = from->child(errs, sibnum);
			if (find == nullptr) { return true; }
		}
		return nextPathSection(errs);
	}

// page
	bool Locator::doPage(Messages &errs) {
		in++;
		find = from;
		pair<size_t, bool> page = znatural(in);
		if (page.second) {
			if (showPaths) message << "[" << page.first << "]";
			if (page.first >= find->iGet(errs,templates)) {
				pageNum = UINTMAX_MAX;    //No pages at that count.
			} else {
				pageNum = page.first;
			}
			if (in != out) {
				throw BadLocatorPath(errs, start, in, out);
			}
		} else {
			pageNum = UINTMAX_MAX;
		}
		return true;    // stop now
	}

// link ref
	bool Locator::doLinkRef(Messages &errs) {
		in++;       // skip '!'
		string::const_iterator inx = in;
		while (in != out && *in != ':' && *in != '/') in++;
		string linkref(inx, in);
		if (showPaths) message << "!" << linkref;
		const Node *result = root != nullptr ? root->tree()->byRef(errs,linkref) : nullptr;
		if (result != nullptr) {
			find = result;
		} else {
			find = nullptr;
			return true;
		}
		return nextPathSection(errs);
	}

// Ancestor at tier n
	bool Locator::doAncestorAbsolute(Messages &errs) {
		if (from == root) {    // root illegal here
			if (showPaths) message << "(/A illegal!)";
			find = nullptr;
			return true;
		}
		in++;
		if (in != out && *in == 'L') {
			in++;
			pair<size_t, bool> ilayout = znatural(in);
			if (!ilayout.second) {
				throw BadLocatorPath(errs, start, in, out);
			} else {
				if (showPaths) message << "AL[" << ilayout.first << "]";
				find = nullptr; // not found yet
				const Node *cn = from;
				while (cn != root) {
					if (cn->iGet(errs,layout) == ilayout.first) {
						find = cn;
						cn = root;
					} else {
						cn = cn->parent();
					}
				}
				if (find == nullptr) {
					return true;
				}
			}
		} else {
			pair<size_t, bool> itier = znatural(in);
			if (!itier.second) {
				throw BadLocatorPath(errs, start, in, out);
			} else {
				if (showPaths) message << "A[" << itier.first << "]";
				find = from;
				while ( find != nullptr && (find->tier() > itier.first) ) {
					find = find->parent();
				}
			}
		}
		if (find == nullptr) return true;
		return nextPathSection(errs);
	}

// Ancestor up n tiers
	bool Locator::doAncestorRelative(Messages &errs) {
		if (from == root) { // root illegal here
			if (showPaths) message << "(/^ illegal!)";
			find = nullptr;
			return true;
		}
		in++;
		if (in != out && *in == 'L') {
			in++;
			pair<size_t, bool> ilayout = znatural(in);
			if (!ilayout.second) {
				throw BadLocatorPath(errs, start, in, out);
			} else {
				if (showPaths) message << "^L[" << ilayout.first << "]";
				find = from;
				while (find->iGet(errs,layout) != ilayout.first && find != nullptr) { //this assumes that the node is actually content!
					find = find->parent();
				}
			}
		} else {
			pair<size_t, bool> up = znatural(in);
			if (!up.second) {
				throw BadLocatorPath(errs, start, in, out);
			} else {
				if (showPaths) message << "^[" << up.first << "]";
				find = from;
				while (up.first > 0 && find != nullptr) {
					up.first--;
					find = find->parent();
				}
			}
		}
		return nextPathSection(errs);
	}

	bool Locator::doStackFromInside(Messages &errs) {
		in++;       // skip 'I'
		if(root->cultivar() == content) {
			pair<size_t, bool> offset = znatural(in);
			if (!offset.second) {
				throw BadLocatorPath(errs, start, in, out);
			} else {
				if(metrics != nullptr) {
					size_t stackSize = metrics->nodeStack.size();
					if (offset.first != 0) {
						setdirty();
					}
					if (showPaths) message << "I" << offset.first;
					const node::Content *result = nullptr;
					if (stackSize == 0) { //we are in template.
						if ((size_t) offset.first < stackSize) {
							result = metrics->current;
						}
					} else {
						if ((size_t) offset.first < stackSize) {
						    //stack =2, offset = 2, offset
							result = metrics->nodeStack[stackSize - (offset.first+1)]; //offset=0, size=1; 1-(0+1) = 0;
						}
					}
					if (result != nullptr) {
						find = result;
						return nextPathSection(errs);
					} else {
						find = nullptr;
						return true;
					}
				} else {
					errs << Message(fatal,"Metrics are necessary for node location");
					return true;
				}
			}
		} else {
			errs << Message(error,"I{number} can only be used while parsing Content locations.");
			throw BadLocatorPath(errs, start, in, out);
		}
	}

	bool Locator::doStackFromOutside(Messages &errs) {
		in++;       // skip 'O'
		if(root->cultivar() == content) {
			pair<size_t, bool> offset = znatural(in);
			if (!offset.second) {
				throw BadLocatorPath(errs, start, in, out);
			} else {
				if(metrics != nullptr) {
					size_t stackSize = metrics->nodeStack.size();
					setdirty();
					if (showPaths) message << "O" << (size_t) offset.first;
					const Content *result = nullptr;
					if (stackSize == 0) { //we are in template.
						if ((size_t) offset.first < stackSize) {
							result = metrics->current;
						}
					} else {
						if ((size_t) offset.first < stackSize) {
							result = metrics->nodeStack[offset.first]; //offset=0, size=1; 1-(0+1) = 0;
						}
					}
					if (result != nullptr) {
						find = result;
						return nextPathSection(errs);
					} else {
						find = nullptr;
						return true;
					}

				} else {
					errs << Message(fatal,"Metrics are necessary for node location");
					return true;
				}
			}
			return nextPathSection(errs);
		} else {
			errs << Message(error,"O{number} can only be used while parsing Content locations.");
			throw BadLocatorPath(errs, start, in, out);
		}
	}

// illegal character
	bool Locator::doIllegalCharacter(Messages &) {
		if (showPaths) message << "(illegal character)";
		find = nullptr;
		return true;
	}
}
