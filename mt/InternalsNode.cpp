#include "Internal.h"
#include "InternalInstance.h"
#include "support/Message.h"
#include "node/Tree.h"
#include "node/Content.h"
#include "content/Layout.h"
#include "content/Segment.h"
#include "content/Editorial.h"

namespace mt {
	using namespace Support;
	using namespace node;

	void iTitle::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);

		const Node* interest = my.node(1);
		if (interest != nullptr) {
			string result = interest->get(e,title);
			my.logic(result,2);
		}
	}
	void iTeam::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const Node* interest = my.node(1);
		if (interest != nullptr) {
			my.logic(interest->get(e,team),2);
		}
	}
	void iSuffix::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		pair<const Node*,size_t> interest = my.nodePage(1);
		if (interest.first != nullptr && interest.second != UINTMAX_MAX ) {
			auto* layout = interest.first->content()->layout();
			if(layout) {
				auto* suffix = layout->templates[interest.second]->suffix->last;
				if(suffix) {
					my.logic(suffix->ref(),2);
				}
			}
		}
	}
	void iShortTitle::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const Node* interest = my.node(1);
		if (interest != nullptr) {
			string result = interest->get(e,shortTitle);
			my.logic(result,2);
		}
	}

	void iSegmentName::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}

	void iLayout::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const Node* interest = my.node(1);
		if (interest != nullptr) {
			const node::Content* content = interest->content();
			if(content != nullptr) {
				if(content->layout() != nullptr) {
					my.logic(content->layout()->id,2);
				}
			}
		}
	}
	void iLayoutName::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const Node* interest = my.node(1);
		if (interest != nullptr) {
			const node::Content* content = interest->content();
			if(content != nullptr) {
				if(content->layout() != nullptr) {
					my.logic(content->layout()->ref,2);
				}
			}
		}
	}
	void iLink::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		pair<const Node*,size_t> interest = my.nodePage(1);
		if (interest.first != nullptr && interest.second != UINTMAX_MAX ) {
			my.logic(interest.first->content()->filename(e,interest.second),2);
		}
	}
	void iLinkRef::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const Node* interest = my.node(1);
		if (interest != nullptr) {
			string value = interest->ref();
			my.logic(value,2);
		}
	}
	void iID::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const Node* interest = my.node(1);
		if (interest != nullptr) {
			my.logic(interest->id(),2);
		}
	}
	void iBirth::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const node::Node* interest = my.node(1);
		if(interest) {
			my.logic(interest->get(e,birth).str(),2);
		}
		my.logic(false,1);
	}
	void iContent::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const node::Content* interest = my.node(1)->content();
		if(interest) {
			const content::Layout* layout = interest->layout();
			if(layout) {
				auto* segment = layout->segment(e,my.parm(2));
				Metrics* metrics = context.back().second.metrics;
				if(my.count == 2) {
					if(metrics) { //eep
						metrics->nodeStack.push_back(interest);
					}
					const mt::mtext* code = content::Editorial::e().get(e,interest,segment);
					if(code) { //need to do IO as well..
						mt::Wss::push(&(segment->nl));
						for(auto& token: *code) {
							token->expand(e,o,context);
						}
						mt::Wss::pop();
					}
					if(metrics) { //eep
						metrics->nodeStack.pop_back();
					}
				} else {
					my.set(content::Editorial::e().getMeta(e,interest,segment,my.parm(3)));
				}
			} else {
				e << Message(error,_name + " has no layout.");
				my.logic(false,3);
			}
		}
	}
	void iDeath::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const node::Node* interest = my.node(1);
		if(interest) {
			my.logic(interest->get(e,death).str(),2);
		}
		my.logic(false,1);
	}
	void iExistContent::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const node::Content* interest = my.node(1)->content();
		if(interest) {
			const content::Layout* layout = interest->layout();
			if(layout) {
				Messages suppress;
				auto* segment = layout->segment(suppress,my.parm(2));
				my.logic(content::Editorial::e().has(suppress,interest,segment),3);
			} else {
				e << Message(error,_name + " has no layout.");
				my.logic(false,3);
			}
		}
	}
	void iTW::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const Node* interest = my.node(1);
		if (interest != nullptr) {
			my.logic(interest->tw(),2);
		}
	}

}