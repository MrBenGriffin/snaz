//
// Created by Ben on 2019-03-19.
//

#include "Suffix.h"
namespace node {

	void Suffix::load(Messages& log, Connection& sql) {

	}
	Suffix::~Suffix() {}
	bool   Suffix::get(Messages& errs,boolValue field) const {
		bool result=false;
		switch(field) {
			case exec: break;
			case batch: break;
		}
		return result;
	};
	size_t Suffix::get(Messages& errs,uintValue field) const {
		size_t result=0;
		switch(field) {
			case team: break;
			case layout: break;
			case page: break;
			case templates: result=0; break;
		}
		return result;
	};
	string Suffix::get(Messages& errs,textValue field) const {
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
	Date   Suffix::get(Messages& errs,dateValue field) const {
		Date result;
		switch(field) {
			case birth: break;
			case death: break;
		}
		return result;
	};

}