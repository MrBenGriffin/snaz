#include "Internal.h"
#include "InternalInstance.h"
#include "support/Message.h"
#include "support/Encode.h"
#include "node/Tree.h"
#include "node/Content.h"
#include "content/Layout.h"
#include "content/Segment.h"
#include "content/Editorial.h"

namespace mt {
	using namespace Support;
	using namespace node;

	void iTitle::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);

		const Node* interest = my.node(1);
		if (interest != nullptr) {
			my.logic(interest->sGet(e,title),2);
		}
	}
	void iTeam::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const Node* interest = my.node(1);
		if (interest != nullptr) {
			my.logic(interest->iGet(e,team),2);
		}
	}
	void iSuffix::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
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
	void iShortTitle::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const Node* interest = my.node(1);
		if (interest != nullptr) {
			my.logic(interest->sGet(e,shortTitle),2);
		}
	}

	void iSegmentName::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		if(my.metrics->segmentStack.empty()) {
			e << Message(error,_name + " called outside any segment context.");
		} else {
// Errors will be shown for segments that are out of context for the layout.
// Should this only work with the topmost current_node/segment?
// What about if this is in a segment of another segment?
// We DO have both the metrics segmentStack and the nodeStack..
			auto* layout = my.metrics->current->layout();
			auto* segment = layout->segment(e,my.parm(1));
			my.logic(segment != nullptr && segment->id == my.metrics->segmentStack.top()->id,2);
		}
	}

	void iLayout::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
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
	void iLayoutName::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
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
	void iLink::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		pair<const Node*,size_t> interest = my.nodePage(1);
		if (interest.first != nullptr && interest.second != UINTMAX_MAX ) {
			my.logic(interest.first->content()->filename(e,interest.second),2);
		}
	}
	void iLinkRef::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const Node* interest = my.node(1);
		if (interest != nullptr) {
			string value = interest->ref();
			my.logic(value,2);
		}
	}
	void iID::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const Node* interest = my.node(1);
		if (interest != nullptr) {
			my.logic(interest->id(),2);
		}
	}
	void iBirth::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const node::Node* interest = my.node(1);
		if(interest) {
			my.logic(interest->dGet(e,birth).str(),2);
		}
		my.logic(false,1);
	}
	void iContent::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const node::Content* interest = my.node(1)->content();
		if(interest) {
			const content::Layout* layout = interest->layout();
			if(layout) {
				auto* segment = layout->segment(e,my.parm(2));
				bool advanced = false;
				if(my.count > 2) {
					auto p3 = my.parm(3);
					if (p3 == "A") {
						advanced = true;
					}
				}
				if(my.count == 2 || advanced) {
					auto code = content::Editorial::e().get(e,interest,segment,advanced);
					if(!code.first) { //need to do IO as well..
						auto* metrics = const_cast<Metrics*>(my.metrics);
						metrics->push(interest,segment);
						code.second->expand(e,o,context);
						metrics->pop();
					} else {
						o.add(*code.second);
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
	void iDeath::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const node::Node* interest = my.node(1);
		if(interest) {
			my.logic(interest->dGet(e,death).str(),2);
		}
		my.logic(false,1);
	}
	void iExistContent::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const node::Content* interest = my.node(1)->content();
		if(interest) {
			const content::Layout* layout = interest->layout();
			if(layout) {
				auto segName = my.parm(2);
				auto segment = layout->segmentInScope(e,segName);
//				if(e.justMarked()) {
//					ostringstream msg;
//					msg << "Parameter 2: Segment '" << segName << "' not in scope for layout '" << layout->ref << "'";
//					e.push(Message(error,msg.str()));
//					doTrace(e, context);
//					e.pop();
//				}
				my.logic(segment.first,3); // segment first is whether or not it was found.
			} else {
				e << Message(error,_name + " has no layout.");
				my.logic(false,3);
			}
		}
	}
	void iTW::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const Node* interest = my.node(1);
		if (interest != nullptr) {
			my.logic(interest->tw(),2);
		}
	}

}
