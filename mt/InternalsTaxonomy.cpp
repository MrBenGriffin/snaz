#include "Internal.h"
#include "InternalInstance.h"
#include "support/Message.h"
#include "support/Convert.h"
#include "node/Taxon.h"
#include "node/Content.h"

namespace mt {
	using namespace Support;

	void iTax::expand(Messages &e, MacroText &o, Instance &instance, mstack &context) const {
		/**
			@iTax(4218,id)	4218	4218
			@iTax(4218,scope)	FX	FX
			@iTax(4218,linkref)	FX	FX
			@iTax(4218,tier)	4	4
			@iTax(4218,team)	1	1
			@iTax(4218,title)	Flat	Flat
			@iTax(4218,shorttitle)	Flat	Flat
			@iTax(4218,classcode)	FT	FT
			@iTax(4218,synonyms)	Apartment	Apartment
			@iTax(4218,article)	A self-contained set of rooms.	A self-contained set of rooms.
			@iTax(4218,title,Flat,T,F)	T	T
			@iTax(4218,title,Flot,T,F)	F	F
		 */
		InternalInstance my(this, e, o, instance, context);
		const node::Node *interest = my.node(1, node::taxon);
		if (interest != nullptr) {
			auto fs = interest->field(e, my.parm(2));
			switch (fs.second) {
				case node::vString:
					my.logic(interest->sGet(e, fs.first), 3);
					break;
				case node::vNumber:
					my.logic(interest->iGet(e, fs.first), 3);
					break;
				case node::vBool:
					my.logic(interest->bGet(e, fs.first), 3);
					break;
				case node::vDate:
					my.logic(interest->dGet(e, fs.first).str(), 3);
					break;
			}
		}
	}

	void iForTax::expand(Messages &e, MacroText &o, Instance &instance, mstack &context) const {
		// Returns all the taxons (optionally rooted by a taxon) affiliated with a Node.
		InternalInstance my(this, e, o, instance, context);
		auto *interest = my.node(1);
		if (interest) {
			mt::nlist nodes;
			const node::Taxon *root = nullptr;
			root = my.node(2, node::taxon)->taxon();
			if (!root) {
				root = node::Taxon::root();
			}
			if (root) {
				root->contentNodes(e, nodes, interest);
				my.generate(nodes, my.praw(4), my.parm(3)); //template,node*,position.
			}
		}
	}

	void iForTaxNodes::expand(Messages &e, MacroText &o, Instance &instance, mstack &context) const {
		// Returns all the nodes subscribed to a specific taxonomy subject.
		// @iForTaxNodes(TAX_NODE_ID,REPLACE_STR,NUM_STR,SORT,CONTEXT)
		// @iForTaxNodes(!flat,*,$,+t,[@iTitle(*)])
		InternalInstance my(this, e, o, instance, context);
		auto *interest = my.node(1);
		if (interest) {
			mt::nlist nodes;
			interest->taxon()->nodeContent(e, nodes);
			doSort(e, nodes, my.parm(4), &context, const_cast<node::Metrics *>(my.metrics));
			my.generate(nodes, my.praw(5), my.parm(2), my.parm(3));   //parms,code,vToken,cToken
		}
	}

	void iExistSimilar::expand(Messages &e, MacroText &o, Instance &instance, mstack &context) const {
		InternalInstance my(this, e, o, instance, context);
		if (sql != nullptr && sql->isopen()) {
			auto *node = my.node(1);
			if (node) {
				auto found = node::Taxon::hasSimilar(e, *sql, node->content());
				my.logic(found, 2);
			}
		} else {
			e << Message(error, _name + " requires an open sql connection.");
		}
	}

	void iForSimilar::expand(Messages &e, MacroText &o, Instance &instance, mstack &context) const {
		//@iForSimilar(!foo,*,#,Format,6,[@iLinkRef(*):#])	T
		InternalInstance my(this, e, o, instance, context);
		if (sql != nullptr && sql->isopen()) {
			auto *interest = my.node(1);
			if (interest) {
				//nodeScores is a pair<long double,const Content*>
				//nscores here is const nodeScores*
				auto* scores = node::Taxon::getSimilar(e, *sql, interest->content() );
				if(scores) {
					auto user_limit_str = my.parm(5);
					size_t limit = scores->size();
					if(!user_limit_str.empty()) {
						size_t user_limit = Support::natural(user_limit_str);
						limit = min(limit,user_limit);
					}
					string format = my.parm(4);
					const MacroText* program = my.praw(6);
					if(program != nullptr && !program->empty()) { // from an empty parm..
						Definition def;
						def.setIterationOffset(e,2);
						forStuff stuff(my.parm(2),my.parm(3));
						stuff.iterInj = Injection("i+1");
						program->doFor(e,def.expansion,stuff,context);
						node::Metrics local(my.metrics);
						auto inst = Instance(&local);
						inst.generated = true;
						for(auto& i : *scores) {
							MacroText parmN;
							parmN.add(i.second->ids());
							inst.parms.emplace_back(std::move(parmN));
							MacroText parmS;
							parmS.add(tostring(i.first,format));
							inst.parms.emplace_back(std::move(parmS));
						}
						inst.parms.resize(limit << 1);
						def.expand(e,o,inst,context);
					}
				}
			}
		} else {
			e << Message(error, _name + " requires an open sql connection.");
		}
	}

} //mt