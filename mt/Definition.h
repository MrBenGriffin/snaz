//
// Created by Ben on 2019-01-10.
//

#ifndef DEFINITION_H
#define DEFINITION_H

#include <unordered_map>
#include <string>
#include <vector>

#include "support/Definitions.h"
#include "support/db/Connection.h"
#include "support/Message.h"

#include "mt/mt.h"
#include "mt/Handler.h"
#include "mt/Internal.h"
#include "mt/MacroText.h"

namespace mt {
	using namespace Support;

	//'user macro definitions'
	class Definition : public Handler {
	public:
		static void startup(Messages&);
		static void load(Messages&,Db::Connection&,buildKind);
		static void shutdown(Messages&,Db::Connection&,buildKind);

		//this library should belong to the Handler class....
		static std::unordered_map<std::string, std::unique_ptr<Handler>> library;
	private:
		size_t minParms, maxParms;
		std::string _name;

		static void trim(plist&);

	protected:
		size_t counter;
		size_t iterationOffset;

	public:
		MacroText expansion;
		bool iterated, trimParms, preExpand;
		string example;

		Definition(const std::string&);
		Definition(Messages&,const std::string&,const std::string&, long = 0, long = -1, bool= true, bool= true, bool= false);
		void setIterationOffset(Messages&, size_t);

		// Handler virtuals.
		bool inRange(size_t) const override;
		bool unlimited() const { return maxParms == INT_MAX; }
		bool internal() const override { return false; }
		bool parmCheck(Messages&,size_t,const location*) const;
		std::ostream &visit(std::ostream &) const override;
		void expand(Messages&,MacroText&,Instance&,mstack&) const override;
		void check(Messages&,Instance&,const location&) const override;


		std::string name() const override;
		static bool test_adv(const std::string &);
		static void del(const std::string&);
		static bool has(const std::string&);

		//By Library call..
		static void vis(const std::string&,std::ostream&);
		static void exp(const std::string,Messages&,MacroText&,Instance&,mstack&);
		static void list(std::ostream&);


	};
}

#endif //DEFINITION_H
