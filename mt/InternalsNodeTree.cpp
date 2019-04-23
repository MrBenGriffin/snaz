#include "Internal.h"
#include "InternalInstance.h"
#include "support/Message.h"
#include "support/Convert.h"
#include "node/Tree.h"
#include "node/Content.h"
#include "content/Layout.h"

namespace mt {
	using namespace Support;
	using namespace node;
	void iNumChildren::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		const Node* main = node::Content::editorial.byPath(e,my.parm(1));
		if (main != nullptr) {
			my.logic(main->getChildCount(),2);
		}
	}
	void iNumGen::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		const Node* main = node::Content::editorial.byPath(e,my.parm(1));
		if (main != nullptr) {
			my.logic(main->tier(),2);
		}
	}
	void iNumGens::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		const Node* main = node::Content::current();
		if (main != nullptr) {
			my.logic(main->tree()->depth(),1);
		}
	}
	void iNumPage::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		const Node* main = node::Content::current();
		if (main != nullptr) {
			size_t result = main->get(e,page);
			my.logic(result,2);
		}
	}
	void iNumPages::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		const Node* main = node::Content::editorial.byPath(e,my.parm(1));
		if (main != nullptr) {
			const node::Content* content = main->content();
			if(content && content->layout()) {
				my.logic(content->layout()->pages(), 2);
			} else {
				my.logic(size_t(0), 2);
			}
		}
	}
	void iNumSib::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		const Node* main = node::Content::editorial.byPath(e,my.parm(1));
		if (main != nullptr) {
			my.logic(main->sibling(), 2);
		}
	}
	void iEqFamily::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		//Is parameter 1 the same as or descendant of parameter 2
		InternalInstance my(this,e,o,instance,context);
		const Node* n1 = node::Content::editorial.byPath(e,my.parm(1));
		const Node* n2 = node::Content::editorial.byPath(e,my.parm(2));
		if (n1 != nullptr && n2 != nullptr) {
				my.logic(n1->hasAncestor(n2),3);
		}
	}
	void iEqNode::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		//Is parameter 1 the same as parameter 2?
		InternalInstance my(this,e,o,instance,context);
		const Node* n1 = node::Content::editorial.byPath(e,my.parm(1));
		const Node* n2 = node::Content::editorial.byPath(e,my.parm(2));
		if (n1 != nullptr && n2 != nullptr) {
			my.logic(n1->id() == n2->id(),3);
		}
	}
	void iEqSibs::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		const Node* n1 = node::Content::editorial.byPath(e,my.parm(1));
		const Node* n2 = node::Content::editorial.byPath(e,my.parm(2));
		if (n1 != nullptr && n2 != nullptr) {
			my.logic(n1->parent() == n2->parent(),3);
		}
	}
	void iExistNode::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		Messages suppress;
		const Node* main = node::Content::editorial.byPath(suppress,my.parm(1));
		my.logic(main != nullptr, 2);
	}
	void iForAncestry::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		const Node *n = node::Content::editorial.byPath(e, my.parm(1));
		if (n != nullptr) {
			vector<const Node*> nodelist;
			string terminator = my.parm(3);
			if (terminator.size() > 0) {
				switch (terminator[0]) {
					case 'c':
					case 'C': { //Count.
						terminator.erase(0,1);  // remove marker
						size_t count = natural(terminator);
						while ( (count > 0) && n->parent() != nullptr ) {
							nodelist.push_back(n);
							n = n->parent();
							count--;
						}
					} break;
					case 't':
					case 'T': { //Tier
						terminator.erase(0,1);  // remove marker
						size_t tierNumber = natural(terminator);
						while ( n->tier() > tierNumber && n->parent() != nullptr ) {
							nodelist.push_back(n);
							n = n->parent();
						}
						nodelist.push_back(n);
					} break;
					case 'l':
					case 'L': { //Layout.
						terminator.erase(0,1); // remove marker
						size_t layoutNumber = natural(terminator);
						const node::Content* content = n->content();
						while ( content->layout() && content->layout()->id != layoutNumber && content->parent() != nullptr ) {
							nodelist.push_back(content);
							content = content->parent()->content();
						}
						nodelist.push_back(content);
					} break;
					default: {
						const Node *a = node::Content::editorial.byPath(e,terminator);
						if(n->hasAncestor(a)) {
							nodelist = n->ancestors(a);
						} else {
							e << Message(error,"Terminating node is not an ancestor");
						}
					};
				}
			} else {
				nodelist = n->ancestors();
			}
			if ( my.reverse(2) ) {
				std::reverse(nodelist.begin(), nodelist.end());
			}
			my.generate(nodelist,my.praw(5),my.parm(4));   //parms,code,vToken,cToken
		}
	}
	void iForPeers::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		//@iForPeers(n,a,t1,t2,sortparm,txt)
		InternalInstance my(this, e, o, instance, context);
		const Node *main = node::Content::editorial.byPath(e, my.parm(1));
		const Node *root = node::Content::editorial.byPath(e, my.parm(2));
		if (main != nullptr) {
			vector<const Node *> peers = main->peers(root);
			doSort(e, peers, my.parm(5));
			my.generate(peers,my.praw(6),my.parm(3),my.parm(4));   //parms,code,vToken,cToken
		}
	}
	void iForNodes::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		vector<string> nodeList;
		toDecimalList(nodeList,my.parm(1));
		plist parms = toNodeParms(e,nodeList,my.parm(4)); //nodes, and sorted..
		my.generate(parms,my.praw(5),my.parm(2),my.parm(3));
	}
	void iForSibs::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		const Node* main = node::Content::editorial.byPath(e,my.parm(1));
		if (main != nullptr) {
			vector<const node::Node *> sibs = std::move(main->siblings());
			my.generate(sibs,my.praw(4),my.parm(2),my.parm(3)); //template,node*,position.
		}
	}
	void iSize::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		const Node* main = node::Content::editorial.byPath(e,my.parm(1));
		if (main != nullptr) {
			my.logic(main->size(), 2);
		}
	}

}