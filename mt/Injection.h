//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_INJECTION_H
#define MACROTEXT_INJECTION_H

#include <string>
#include "support/Message.h"
#include "Token.h"
#include "mt/using.h"

namespace mt {

    using namespace Support;
    class Injection : public Token {

    private:
        enum class It { plain,text,current,count,size }; //not iteration,not injection,i,j,k,n respectively
        It type;            // tells us the iteration type
        size_t value;       // the parameter number being asked for (when plain)
        size_t sValue;      // the stack offset
        long offset;        // the offset value (signed!)
        bool modulus;       // if the offset is a modulus.
        bool stack;         // if this is a full stack trace
        bool list;          // if we are passing a list of parameters over (eg %(3+))
        std::string basis;  // What we are working with.

        void adjust(size_t& base) const;  //modulus etc.
        void parseStart();
        void parseIterated();
        void parsePName();
        void parseBrackets();
        void parseParent();

    public:
		bool iterator;      // This is injection defines the macro as an iterator.

		Injection();
        explicit Injection(std::string);
		~Injection() override = default;
//		Injection(const Injection& ) = default;

		std::string get() const override;             //return text.
		bool empty() const override;
		void final(std::ostream&) const override;     //return final text.
		std::ostream& visit(std::ostream&) const override;
		void inject(Messages&,mtext&,mstack&) const override;
		void expand(Messages&,mtext&,mstack&) const override;
//		void add(mtext&) override;
		std::string name() const override { return "`injection`"; }


    };
}

#endif //MACROTEXT_INJECTION_H
