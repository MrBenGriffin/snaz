//
// Created by Ben on 2019-03-19.
//

#include "Taxon.h"
namespace node {

//	refMap   Taxon::fields;                //A quick reference to the fields by name (as in iTax)

	void Taxon::load(Messages& log, Connection& sql) {
//			fields.insert({"id", 0});
//			fields.insert({"scope", 1});
//			fields.insert({"linkref", 2});
//			fields.insert({"tier", 3});
//			fields.insert({"team", 4});
//			fields.insert({"title", 5});
//			fields.insert({"shorttitle", 6});
//			fields.insert({"classcode", 7});
//			fields.insert({"synonyms", 8});
//			fields.insert({"article", 9}); //descr

//		static void inittaxfields();

	}

	bool   Taxon::get(Messages& errs,boolValue field) const {
		bool result=false;
		switch(field) {
			case exec: break;
			case batch: break;
		}
		return result;
	};
	size_t Taxon::get(Messages& errs,uintValue field) const {
		size_t result=0;
		switch(field) {
			case team: break;
			case layout: break;
			case page: break;
			case templates: result=0; break;
		}
		return result;
	};
	string Taxon::get(Messages& errs,textValue field) const {
		string result;
		switch(field) {
			case title: break;
			case shortTitle: break;
			case tierRef: break;
			case baseFilename: break;
			case scope: break;
			case classCode: break;
			case synonyms: break;
			case keywords: break;
			case descr: break;
			case fileSuffix: break;
			case script: break;
		}
		return result;
	};
	Date   Taxon::get(Messages& errs,dateValue field) const {
		Date result;
		switch(field) {
			case birth: break;
			case death: break;
		}
		return result;
	};

	Taxon::~Taxon() {}

}