//
// Created by Ben on 2019-01-10.
//

#ifndef DEFINITION_H
#define DEFINITION_H

#include <unordered_map>
#include <string>
#include <vector>
#include "mt.h"
#include "Internals.h"

//using namespace std;
namespace mt {

    class Definition {
    public:
        static void initialise();

    private:
        long minParms, maxParms;
        std::string _name;
        static Definition empty;
        static std::unordered_map<std::string, Handler> library;

        static void trim(plist&);

    protected:
        size_t counter;
        mt::mtext expansion;

    public:
        std::string name() const { return _name; }
        bool inRange(size_t i) const { return minParms <= i <= maxParms;}
        bool iterated, trimParms, preExpand;

        std::ostream &visit(std::ostream &);

        Definition(std::string, std::string, long, long, bool= true, bool= true, bool= false);

        void expand(mtext&,Instance&,mstack&);

        static bool test_adv(std::string &);

        static void add(Definition&);

        static void del(std::string);

        static bool has(std::string);

        //By Library call..
        static void vis(std::string,std::ostream&);

        static void exp(std::string,mtext&,Instance&,mstack&);

        static void list(std::ostream&);


    };
}

#endif //DEFINITION_H
