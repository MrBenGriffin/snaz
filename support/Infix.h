//
// Created by Ben Griffin on 2019-01-30.
//

#ifndef MACROTEXT_INFIX_H
#define MACROTEXT_INFIX_H

#include <map>
#include <vector>
#include <sstream>
#include <cmath>
#include <cfloat>
#include <unordered_map>

#include "Message.h"

namespace Support {
	namespace Infix {
		using namespace std;

		class Op {
		public:
			typedef enum {none, left, right} association;
			int precedence; //the order in which things are done. low = first.
			association assoc;
			size_t parms;

			explicit Op(int prec,association a = left,size_t p = 2) : precedence(prec),assoc(a),parms(p) {}
			virtual char sig() const = 0;
			virtual long double evaluate(const long double,const long double,const long double) const {
				return nanl("");
			}
			virtual ~Op() = default;
		};
		class same : public Op {
		public:
			same() : Op(7){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return '=';}
		};
		class subtract : public Op {
		public:
			subtract() : Op(6){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return '-';}
		};
		class add : public Op {
		public:
			add() : Op(6){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return '+';}
		};
		class multiply : public Op {
		public:
			multiply() : Op(5){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return '*';}
		};
		class divide : public Op {
		public:
			divide() : Op(5){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return '/';}
		};
		class uminus : public Op {
		public:
			uminus() : Op(3,right,1){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return '_';}
		};
		class quotient : public Op {
		public:
			quotient() : Op(5){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return '\\';}
		};
		class lb : public Op {
		public:
			lb() : Op(1000,none,0) {}
			char sig() const override { return '(';}
		};
		class rb : public Op {
		public:
			rb() : Op(1000,none,0) {}
			char sig() const override { return ')';}
		};
		class delim : public Op {
		public:
			delim() : Op(999,none,0) {}
			char sig() const override { return ',';}
		};
		class powfn : public Op {
		public:
			powfn() : Op(2){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class maxfn : public Op {
		public:
			maxfn() : Op(2){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class minfn : public Op {
		public:
			minfn() : Op(2){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class ltfn : public Op {
		public:
			ltfn() : Op(2){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class gtfn : public Op {
		public:
			gtfn() : Op(2){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class ltefn : public Op {
		public:
			ltefn() : Op(2){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class modulo : public Op {
		public:
			modulo() : Op(2){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class gtefn : public Op {
		public:
			gtefn() : Op(2){}
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class bandfn : public Op {
		public:
			bandfn() : Op(2){} //as a function = 2; as characters (10)
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class bxorfn : public Op {
		public:
			bxorfn() : Op(2){} //as a function = 2; as characters (11)
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class bnotfn : public Op {
		public:
			bnotfn() : Op(2,left,1) {} //as a function = 2; as characters (11)
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class borfn : public Op {
		public:
			borfn() : Op(2){} //as a function = 2; as characters (12)
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class log2fn : public Op {
		public:
			log2fn() : Op(2,left,1){} //as a function = 2
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class roundfn : public Op {
		public:
			roundfn() : Op(2,left,1){} //as a function = 2
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class floorfn : public Op {
		public:
			floorfn() : Op(2,left,1){} //as a function = 2
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class ceilfn : public Op {
		public:
			ceilfn() : Op(2,left,1){} //as a function = 2
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class absfn : public Op {
		public:
			absfn() : Op(2,left,1){} //as a function = 2
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class rolfn : public Op { //number,shifts,wordsize(in bits)
		public:
			rolfn() : Op(2,left,3){} //as a function = 2
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class rorfn : public Op { //number,shifts,wordsize(in bits)
		public:
			rorfn() : Op(2,left,3){} //as a function = 2
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};
		class iftrue : public Op { //  x != 0? y : z
		public:
			iftrue() : Op(2,left,3){} //as a function = 2
			long double evaluate(long double, long double, long double) const override;
			char sig() const override { return 'f';}
		};

		class Evaluate {
		private:
			typedef map<string, Op*> lut_t;
			static lut_t lut;

			string expr;
			vector<const  Op*>  opstack;
			vector<long double>	valstack;
			unordered_map<string,long double> parms;

			bool get(string::const_iterator&,long double&);
			size_t name(string::const_iterator&,string&);
			long double getvalue(string&);
			char evalstack(Messages&);
			const  Op* get(string::const_iterator&,size_t&);
			const  Op* get(string);

		public:
			static void startup();
			static void shutdown();
			Evaluate() = default;

			~Evaluate() = default;

			long double process(Messages&);
			void set_expression(string);
			void add_parm(string,long double);
		};
	}

}

#endif //MACROTEXT_INFIX_H
