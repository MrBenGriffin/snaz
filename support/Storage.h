#ifndef storage_h
#define storage_h

//STL objects used here
#include <string>
#include <unordered_map>
#include <map>
#include <set>

#include "support/Definitions.h"
#include "support/Message.h"
#include "support/db/Connection.h"

namespace Support {

	using listType = std::multiset<std::string>;
	using kv = std::pair<std::string, std::string>;
	using kvec = std::pair<std::string, std::vector<std::string> >;
	using storage_map_type = std::unordered_map<std::string, std::string>;
	using list_map_type = std::unordered_map<std::string, listType>;
	using mark = storage_map_type::const_iterator;
	using item = list_map_type::const_iterator;

	class LStore {
	protected:
		list_map_type store;
	public:
		LStore &operator=(const LStore &o);

		LStore(const LStore &o);

		LStore();

		size_t size(std::string) const;

		const listType *get(std::string) const;
		void get(const std::string&,vector<string>&) const;

		bool has(std::string, std::string) const;
		size_t count(std::string, std::string) const;
		void add(std::string, std::string);
		void uadd(std::string, std::string);
		void erase(std::string, std::string);
		void pop(std::string, std::string);
		bool exists(std::string) const;
		void clear();
	};

	class storageResult {
	public:
		bool found;
		std::string result;

		storageResult() : found(false), result() {}
	};

	class Storage {
	protected:
		storage_map_type store;                            //Used to store/retreive build variables (iSet/iGet, etc)
		std::set<std::string> erased;
	public:
		Storage &operator=(const Storage &o);

		Storage(const Storage &o);

		Storage();

		bool has(std::string) const;

		storageResult find(std::string) const;

		storageResult take(std::string); //erase if found
		std::string as_string(std::string, std::string, bool) const;

		void append(std::string, std::string);

		void set(std::string, std::string);

		void set_if_new(std::string, std::string);

		void erase(std::string);

		void reset(std::string); //remove range..
		void regex(Messages&, Storage &, std::string) const;

		void merge(const Storage *);

		mark begin() const;

		mark end() const;

		kv get(mark) const;

		void keys(std::vector<std::string> &) const;

		void values(std::vector<std::string> &) const;

		size_t size() const;

		void clear();

		bool empty() const;

		void load(Messages&,Db::Connection&,buildKind);
		void save(Messages&,Db::Connection&,buildKind);

	};


	typedef std::map<std::string, Storage> library_map_type;

	class Library {
		library_map_type store;                            //Used to store/retreive storage_map_type
	public:
		void clear();

		storageResult find(std::string, std::string) const;

		bool has(std::string) const; //has book
		bool has(std::string, std::string) const; // has page in a book.
		void set(std::string, std::string, std::string);

		void erase(std::string, std::string);

		void reset(std::string); //remove range..
		void destroy(std::string); //delete book.
		Storage *touch(std::string);   //return a ptr to but create an empty book if there is none.
		Storage *book(std::string);    //return a ptr to the book.
	};
}

#endif /* storage_h */
