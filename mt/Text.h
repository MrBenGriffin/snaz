//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_TEXT_H
#define MACROTEXT_TEXT_H

#include <string>
#include <vector>
#include "mt.h"

namespace mt {
	using namespace Support;
	class Text {
	friend class Internals;
	private:
		std::string text;
		void doCount(mtext&,const std::string&,size_t,std::string&) const;
		void doCountAndValue(Messages&,mtext&,const mstack&) const;

	public:
		Text() = default;
		Text(const std::string &);
		std::ostream& visit(std::ostream&) const;
		void expand(Messages&,mtext&,const mstack&) const;
		void add(mtext&);
		std::string get() const;
		void append(std::string);
		bool empty() const { return text.empty(); }
		void subs(mtext&,const vector<string>&,string) const;
	};

};

#endif //MACROTEXT_TEXT_H

