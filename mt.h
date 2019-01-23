//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_MT_H
#define MACROTEXT_MT_H


#include <any>
#include <istream>
#include <vector>
#include <unordered_map>
#include <deque>
#include <functional>

//#include <type_traits>
//#include <cstddef>
//#include <forward_list>

class userMacro;

namespace mt {
    using mtext=std::deque<std::any>;
    using parse_result=std::pair<mtext, bool>;
    using plist=std::vector<mtext>;
    using mstack=std::deque<std::pair<const userMacro*, plist>>;
    using vMap=std::unordered_map<size_t, std::function<void(std::any &, std::ostream &)>>;
    using eMap=std::unordered_map<size_t, std::function<void(std::any &, std::ostream &, mstack &)>>;
}

#endif //MACROTEXT_MT_H
