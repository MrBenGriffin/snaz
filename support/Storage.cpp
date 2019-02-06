
#include <sstream>
#include <vector>
#include <iterator>
#include "Storage.h"
#include "Regex.h"

namespace Support {

	using namespace std;

	LStore::LStore() = default;

	LStore::LStore(const LStore &o) {
		store.insert(o.store.begin(), o.store.end());
	}

	LStore &LStore::operator=(const LStore &o) {
		if (this != &o) {
			store.insert(o.store.begin(), o.store.end());
		}
		return *this;
	}

	size_t LStore::size(const string name) const {
		size_t size = 0;
		auto it = store.find(name);
		if (it != store.end()) {
			size = it->second.size();
		}
		return size;
	}

	bool LStore::exists(const string name) const {
		auto it = store.find(name);
		return it != store.end();
	}

	const listType *LStore::get(const string name) const {
		const listType *result = nullptr;
		auto it = store.find(name);
		if (it != store.end()) {
			result = &(it->second);
		}
		return result;
	}

	bool LStore::has(const string name, const string value) const {
		bool found = false;
		if (name.length() > 0) {
			auto ppit = store.find(name);
			if (ppit != store.end()) {
				found = (ppit->second.count(value) > 0);
			}
		}
		return found;
	}

	size_t LStore::count(const string name, const string value) const {
		size_t number = 0;
		if (name.length() > 0) {
			auto ppit = store.find(name);
			if (ppit != store.end()) {
				number = ppit->second.count(value);
			}
		}
		return number;
	}

	void LStore::add(const string name, string value) {
		if (name.length() > 0 && value.length() > 0) {
			auto it = store.find(name);
			if (it != store.end()) {
				it->second.emplace(value);
			} else {
				store.insert({name, listType({value})});
			}
		}
	}

	void LStore::uadd(const string name, string value) {
		if (name.length() > 0 && value.length() > 0) {
			auto it = store.find(name);
			if (it != store.end()) {
				if (it->second.count(value) == 0) {
					it->second.emplace(value);
				}
			} else {
				store.insert({name, listType({value})});
			}
		}
	}

	void LStore::clear() {
		store.clear();
	}

	//remove 1.
	void LStore::pop(const string name, const string value) {
		if (name.length() > 0) {
			auto ppit = store.find(name);
			if (ppit != store.end()) {
				auto vit = ppit->second.find(value);
				if (vit != ppit->second.end()) {
					ppit->second.erase(vit);
				}
			}
		}
	}

	//remove all.
	void LStore::erase(const string name, const string value) {
		if (name.length() > 0) {
			auto ppit = store.find(name);
			if (ppit != store.end()) {
				auto& map = ppit->second;
				auto vit = map.find(value);
				for(;vit != map.end();vit = map.find(value)) {
					map.erase(vit);
				}
			}
		}
	}

	Storage::Storage() = default;

	Storage::Storage(const Storage &o) {
		store.insert(o.store.begin(), o.store.end());
	}

	Storage &Storage::operator=(const Storage &o) {
		if (this != &o) {
			store.insert(o.store.begin(), o.store.end());
		}
		return *this;
	}

	size_t Storage::size() const {
		return store.size();
	}

	mark Storage::begin() const {
		return store.cbegin();
	}

	kv Storage::get(mark iter) const {
		return *iter;
	}


	mark Storage::end() const {
		return store.cend();
	}

	void Storage::keys(vector<string> &repo) const {
		for (auto item: store) {
			repo.push_back(item.first);
		}
	}

	void Storage::values(vector<string> &repo) const {
		for (auto item: store) {
			repo.push_back(item.second);
		}
	}

	string Storage::as_string(const string p1, const string p2, bool trailing) const {
		ostringstream result;
		size_t count = store.size();
		size_t point = 1;
		for (auto item: store) {
			result << item.first << p1 << item.second;
			if (trailing || point != count) {
				result << p2;
			}
			point++;
		}

		for (auto it = store.cbegin(); it != store.cend(); ++it) {
		}
		return result.str();
	}

	bool Storage::has(const string name) const {
		bool found = false;
		if (name.length() > 0) {
			auto ppit = store.find(name);
			found = ppit != store.end();
		}
		return found;
	}

	storageResult Storage::find(const string name) const {
		storageResult result;
		if (name.length() > 0) {
			auto ppit = store.find(name);
			result.found = ppit != store.end();
			if (result.found) {
				result.result = ppit->second;
			}
		}
		return result;
	}

	storageResult Storage::take(const string name) {
		storageResult result;
		if (name.length() > 0) {
			storage_map_type::const_iterator ppit = store.find(name);
			result.found = ppit != store.end();
			if (result.found) {
				result.result = ppit->second;
				store.erase(ppit);
			}
		}
		return result;
	}

	void Storage::append(const string name, const string value) {
		if (name.length() > 0 && value.length() > 0) {
			pair<storage_map_type::iterator, bool> ins;
			ins = store.insert(storage_map_type::value_type(name, value));
			if (!ins.second)    // Cannot insert (something already there with same ref
				(ins.first)->second += value;
		}
	}

	void Storage::set(const string name, const string value) {
		if (name.length() > 0) {
			pair<storage_map_type::iterator, bool> ins;
			ins = store.insert(storage_map_type::value_type(name, value));
			if (!ins.second) {    // Cannot insert (something already there with same ref
				store.erase(ins.first);
				store.insert(storage_map_type::value_type(name, value));
			}
			erased.erase(name);
		}
	}

	void Storage::set_if_new(const string name, const string value) {
		if (name.length() > 0) {
			store.insert(storage_map_type::value_type(name, value));
			erased.erase(name);
		}
	}

	bool Storage::empty() const {
		return store.empty();
	}

	void Storage::clear() {
		store.clear();
	}

	void Storage::erase(const string name) {
		if (name.length() > 0) {
			auto ppit = store.find(name);
			if (ppit != store.end()) {
				store.erase(ppit);
			}
		}
		erased.insert(name);
	}

	void Storage::regex(Messages& e,Storage& result, const string basis) const {
		if(Regex::available(e)) {
			for (auto item: store) {
				if (Regex::match(e,basis,item.first)) {
					result.set(item.first,item.second);
				}
			}
		}
	}

	void Storage::reset(const string prefix) {
		size_t psize = prefix.length();
		if (psize > 0) {
			for (auto it = store.begin(); it != store.end();) {
				string name = it->first;
				if (name.substr(0, psize) == prefix) {
					it = store.erase(it);
					erased.insert(name);
				} else {
					it++;
				}
			}
		}
	}

	void Storage::merge(const Storage *other) {
		store.insert(other->store.begin(), other->store.end());
	}

//check if the book is in the library.
	bool Library::has(const string book) const {
		bool found = false;
		if (book.length() > 0) {
			auto ppit = store.find(book);
			found = (ppit != store.end());
		}
		return found;
	}

//check if a page of a book is in the library.
	bool Library::has(const string book, const string page) const {
		bool found = false;
		if (book.length() > 0 && page.length() > 0) {
			auto ppit = store.find(book);
			if (ppit != store.end()) {
				found = ppit->second.has(page);
			}
		}
		return found;
	}

	Storage *Library::book(const string key) {
		Storage *result = nullptr;
		if (key.length() > 0) {
			auto ppit = store.find(key);
			if (ppit != store.end()) {
				result = &(ppit->second);
			}
		}
		return result;
	}

//find a page in the book.
	storageResult Library::find(const string key, const string name) const {
		storageResult result;
		if (key.length() > 0 && name.length() > 0) {
			auto ppit = store.find(key);
			if (ppit != store.end()) {
				result = ppit->second.find(name);
			}
		}
		return result;
	}

//add a book.
	Storage *Library::touch(const string key) {
		Storage *result = nullptr;
		if (key.length() > 0) {
			auto ppit = store.find(key);
			if (ppit != store.end()) {
				result = &(ppit->second);
			} else {
				Storage book;
				pair<library_map_type::iterator, bool> ins;
				ins = store.insert(library_map_type::value_type(key, book));
				result = &(ins.first->second);
			}
		}
		return result;
	}


//add a page to a book.
	void Library::set(const string key, const string name, const string value) {
		if (key.length() > 0 && name.length() > 0) {
			auto ppit = store.find(key);
			if (ppit != store.end()) {
				ppit->second.set(name, value);
			} else {
				Storage book;
				book.set(name, value);
				pair<library_map_type::iterator, bool> ins;
				ins = store.insert(library_map_type::value_type(key, book));
			}
		}
	}

	void Library::reset(const string prefix) {
		size_t psize = prefix.length();
		if (psize > 0) {
			bool found = true;
			while (found) {
				auto it = store.lower_bound(prefix);
				auto title = it->first;
				title = title.substr(0, psize);
				if (title == prefix) {
					store.erase(it);
				} else {
					found = false;
				}
			}
		}
	}

//delete page in book.
	void Library::erase(const string key, const string name) {
		if (key.length() > 0 && name.length() > 0) {
			auto ppit = store.find(key);
			if (ppit != store.end()) {
				ppit->second.erase(name);
			}
		}
	}

	void Library::clear() {
		store.clear();
	}

//delete book.
	void Library::destroy(const string key) {
		if (key.length() > 0) {
			auto ppit = store.find(key);
			if (ppit != store.end()) {
				store.erase(ppit);
			}
		}
	}

}
