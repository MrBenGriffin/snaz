//
// Created by Ben Griffin on 2019-01-31.
//

#include <iostream>

#include "Internal.h"
#include "InternalInstance.h"

#include "support/Infix.h"
#include "support/Message.h"
#include "support/Convert.h"
#include "node/Locator.h"

namespace mt {
	using namespace Support;

	void iExists::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		string key = my.parm(1);
		auto sr = storage.find(key);
		my.logic(sr.found,2); //@iExists(A,T,F)=1
	}

	void iGet::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		string key = my.parm(1);
		auto sr = storage.find(key);
		if(sr.found) {
			my.logic(sr.result,2); //@iGet(A,?,T,F)=2
		}
	}

	void iSet::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		string key = my.parm(1);
		string value;
		if(my.count == 2) {
			value = my.parm(2);
		}
		storage.set(key,value);
	}


	void iAppend::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		string key = my.parm(1);
		if(my.count == 2) {
			string value = my.parm(2);
			storage.append(key,value);
		}
	}

	void iReset::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		string key = my.parm(1);
		storage.erase(key);
	}


	void iSetCache::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		if (my.count == 2 && my.metrics != nullptr) {
			string key = my.parm(1);
			my.metrics->locator->track();
			string value = my.parm(2);
			if(my.metrics->locator->untrack()) { //was dirty.
				e << Message(error,key + " was not cacheable. The variable was still set.");
			}
			storage.set(key,value);
		}
	}

	void iSig::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		string key = my.parm(1);
		auto sr = storage.find(key);
		my.logic(sr.found && !sr.result.empty(),2); //@iSig(A,T,F)=2
	}

	void iUse::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		string key = my.parm(1);
		auto sr = storage.take(key);
		my.set(sr.result);
	}

	void iKV::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		string p1, p2, basis;
		string repo_name = my.parm(1);
		if (!repo_name.empty()) {
			string func = my.parm(2);
			if (!func.empty()) {
				Storage* book = library.book(repo_name); //tr_en
				if (my.count > 2 && func[0]!='e' ) {
					p1 = my.parm(3);
					if (my.count > 3 && func[0] != 'h') {
						p2 = my.parm(4);
						if (!func.empty() && my.count > 4) {
							basis = my.parm(5);
						}
					}
				}
				switch (func[0]) {
					case 'a': { //array without trailing delimiter - cf. list
						if (book != nullptr) {
							my.set(book->as_string(p1,p2,false));
						}
					} break;
					case 'c': { //clear.
						library.destroy(repo_name);
					} break;
					case 'd': { //delete
						library.erase(repo_name,p1);
					} break;
					case 'e': { //exists (and not-empty)
						my.logic(book && ! book->empty(),3); //@iKV(A,e,T,F)=2
						return; //do not want to append again.
					} break;
					case 'f': { //find
						if (book != nullptr) {
							Storage result;
							book->regex(e,result,basis);
							my.set(result.as_string(p1,p2,true));
						}
					} break;
					case 'g': { //get
						if (book != nullptr) {
							storageResult item = book->find(p1);
							if(item.found) {
								my.set(item.result);
							}
						}
					} break;
					case 'h': { //has
						bool has=false;
						if (book != nullptr) {
							storageResult item = book->find(p1);
							has = item.found;
						}
						my.logic(has,4); // @iKV(foo,has,A,T,F)
						return;
					} break;
					case 'k': { //keys
						if (book != nullptr) {
							vector<string> keys;
							book->keys(keys);
							auto &end = keys.back();
							string result;
							for (const auto &key : keys) {
								result.append(key);
								if(end != key) {
									result.append(p1);
								}
							}
							my.set(result);
						}
					} break;
					case 'l': { //list (delimiter_chars) with trailing delimiter cf. array
						if (book != nullptr) {
							my.set(book->as_string(p1,p2,true));
						}
					} break;
					case 'm': { //merge
						const Storage* addon = library.book( p1 );
						if (addon != nullptr) {
							if (book == nullptr) {
								book = library.touch( repo_name );
							}
							book->merge(addon);
						}
					} break;
					case 'n': { //new - set if it does not pre-exist.
						if (book == nullptr) {
							book = library.touch( repo_name );
						}
						book->set_if_new(p1,p2);
					} break;
					case 'r': { //read - parse a string into a map.
						if (book == nullptr) {
							book = library.touch( repo_name );
						}
						book->clear();
						vector<string> keyvals;
						Support::tolist(keyvals,basis,p2);
						for (const auto &keyval : keyvals) {
							vector<string> kv;
							Support::tolist(kv, keyval,p1);
							string& key=kv[0];
							string value;
							if(kv.size() > 1) {
								value=kv[1];
							}
							book->set(key,value);
						}
					} break;
					case 's': { //set if it does or does not pre-exist.
						if (func.size() > 1 && func[1] == 'i') { //size
							if (book != nullptr) {
								my.set(to_string(book->size()));
							} else {
								my.set("0");
							}
						} else {
							library.set(repo_name,p1,p2);
						}
					} break;
					case 'u': { //use - get with delete.
						if (book != nullptr) {
							storageResult item = book->find(p1);
							if(item.found) {
								my.set(item.result);
								book->erase(p1);
							}
						}
					} break;
					case 'v': { //values
						if (book != nullptr) {
							vector<string> items;
							book->values(items);
							string result;
							for (const auto &item : items) {
								result.append(item);
								result.append(p1);
							}
							my.set(result);
						}
					} break;
					default: {
					} break;
				}
			} else {
				e << Message(error,"Second parameter is the function for the key-value store. It cannot be empty.");
			}
		} else {
			e << Message(error,"First parameter is the name of the key-value store. It cannot be empty.");
		}
	}

	void iList::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		//fn: add,delete,pop,unique_add,has,count,size,nodes[:max],for[:max]
		string name = my.parm(1);
		if (!name.empty()) {
			string func = my.parm(2);
			if (!func.empty()) {
				switch (func[0]) {
					case 'a': { //add
						lStore.add(name,my.parm(3));
					} break;
					case 'c': { //count instances of value in store.
						size_t count = lStore.count(name,my.parm(3));
						my.logic(count,4); //@iList(foo,count,bar,[?,T,F])
						return;
					} break;
					case 'd': { //delete
						lStore.erase(name,my.parm(3));
					} break;
					case 'p': { //pop (remove 1).
						lStore.pop(name,my.parm(3));
					} break;
					case 'u': { //unique add
						lStore.uadd(name,my.parm(3));
					} break;
					case 'h': { //has value in store
						my.logic(lStore.has(name,my.parm(3)),4); //@iList(foo,h,x,T,F)
					} break;
					case 'e': { //store exists..
						my.logic(lStore.exists(name),3); //@iList(foo,e,T,F)
					} break;
					case 's': { //size of store // const string name, const string value)
						size_t count = lStore.size(name);
						my.logic(count,3); //@iList(foo,size,[?,T,F])
						return;
					} break;
					case 'n': { //forNodes
						pair<string,string> value = split(':',func);
						size_t maxItems = value.second.empty() ? string::npos : natural(value.second);
						//@iList(foo,nodes,R,*,[*])
						vector<string> list;
						lStore.get(name,list);
						if(!list.empty()) {
							plist parms = my.toNodeParms(this,list,my.parm(3),maxItems);
							my.generate(parms,my.praw(5),my.parm(4),"");
						}
					} break;
					case 'f': { //for
						pair<string,string> value = split(':',func);
						size_t maxItems = value.second.empty() ? string::npos : natural(value.second);
						//@iList(foo,for,R,*,[*])
						vector<string> list;
						lStore.get(name,list);
						if(!list.empty()) {
							plist parms = toParms(list,my.parm(3),maxItems);
							my.generate(parms,my.praw(5),my.parm(4),"");
						}
					} break;
				}
			}
		}
	}

}
