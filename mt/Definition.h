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

#include "mt/Handler.h"
#include "mt/mt.h"
#include "mt/Internal.h"

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
		mtext expansion;

		static void trim(plist&);

	protected:
		size_t counter;

	public:
		bool iterated, trimParms, preExpand;

		Definition(Messages&,std::string, std::string, long = 0, long = -1, bool= true, bool= true, bool= false);
		Definition(const mtext&, long = 0, long = -1, bool= true, bool= true, bool= false);

		// Handler virtuals.
		bool inRange(size_t) const override;
		bool parmCheck(Messages&,size_t) const;
		std::ostream &visit(std::ostream &) const override;
		void expand(Messages&,mtext&,Instance&,mstack&) const override;

		std::string name() const override;
		static bool test_adv(const std::string &);
		//        static void add(Definition&);
		static void del(const std::string&);
		static bool has(const std::string&);

		//By Library call..
		static void vis(const std::string&,std::ostream&);
		static void exp(const std::string,Messages&,mtext&,Instance&,mstack&);
		static void list(std::ostream&);


	};
}

#endif //DEFINITION_H
