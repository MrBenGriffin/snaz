
#include <string>
#include <dlfcn.h>
#include <iostream>
#include <cstring>

//#include "support/Env.h"
#include "support/Convert.h"
#include "support/Sass.h"
#include "support/Message.h"
#include "support/Dynamic.h"

using namespace std;

namespace Support {

	set<string> Sass::inc_paths;
	bool Sass::loadattempted = false;
	bool Sass::loaded = false;
	void* Sass::sass_lib_handle = nullptr;

	void 						(*Sass::sass_delete_compiler)(struct Sass_Compiler *)= nullptr;
	struct Sass_Data_Context*   (*Sass::sass_make_data_context)(char*)= nullptr;
	struct Sass_Options*        (*Sass::sass_make_options)()= nullptr;
	struct Sass_Options*		(*Sass::sass_data_context_get_options)(struct Sass_Data_Context *)= nullptr;
	struct Sass_Compiler*		(*Sass::sass_make_data_compiler)(struct Sass_Data_Context *)= nullptr;
	int							(*Sass::sass_compiler_parse)(struct Sass_Compiler *)= nullptr;
	int							(*Sass::sass_compiler_execute)(struct Sass_Compiler *)= nullptr;
	void 						(*Sass::sass_delete_options)(struct Sass_Options *)= nullptr;
	struct Sass_Context*        (*Sass::sass_data_context_get_context)(struct Sass_Data_Context*)= nullptr;
	void                        (*Sass::sass_data_context_set_options)(struct Sass_Data_Context*, struct Sass_Options*)= nullptr;
	int                         (*Sass::sass_compile_data_context)(struct Sass_Data_Context*)= nullptr;
	void                        (*Sass::sass_delete_data_context)(struct Sass_Data_Context*)= nullptr;
	const char*                 (*Sass::sass_context_get_output_string)(struct Sass_Context*)=nullptr;
	const char*                 (*Sass::sass_context_get_source_map_string)(struct Sass_Context*)=nullptr;
	const char*                 (*Sass::sass_context_get_error_json)(struct Sass_Context*)=nullptr;
	const char*                 (*Sass::sass_context_get_error_message)(struct Sass_Context*)=nullptr;
	int                         (*Sass::sass_context_get_error_status)(struct Sass_Context*)=nullptr;
	void                        (*Sass::sass_option_set_output_style) (struct Sass_Options*,enum Sass_Output_Style)=nullptr;
	void                        (*Sass::sass_option_set_source_map_file) (struct Sass_Options*,const char*)=nullptr;
	void                        (*Sass::sass_option_set_omit_source_map_url) (struct Sass_Options*,bool)=nullptr;
	void                        (*Sass::sass_option_set_is_indented_syntax_src) (struct Sass_Options*,bool)=nullptr;
	void                        (*Sass::sass_option_set_source_comments) (struct Sass_Options*,bool)=nullptr;
	void                        (*Sass::sass_option_set_source_map_contents) (struct Sass_Options*,bool)=nullptr;
	void                        (*Sass::sass_option_set_source_map_embed) (struct Sass_Options*,bool)=nullptr;
	void                        (*Sass::sass_option_set_include_path) (struct Sass_Options*, const char*)=nullptr;
	void                        (*Sass::sass_option_set_precision) (struct Sass_Options*,int)=nullptr;

	bool Sass::available(Messages& e) {
		if (!loadattempted) startup(e);
		return loaded;
	}

	bool Sass::startup(Messages& errors) {
		bool err = false; //necessary IFF script uses pcre.
		if ( ! loadattempted ) {
//			errors << Message(debug,"Sass: Attempting to load dynamic library.");

			loadattempted = true;
			loaded = false;
			string sasslib = SO(libsass); //directory
			sass_lib_handle = dlopen(sasslib.c_str(),RTLD_GLOBAL | RTLD_NOW);
			err = dlerr(errors);

			try {
				if (!err && sass_lib_handle != nullptr ) {
					if(!err) { sass_delete_compiler = (void (*)(struct Sass_Compiler *)) dlsym(sass_lib_handle,"sass_delete_compiler"); err = dlerr(errors); }
					if(!err) { sass_compiler_parse = (int (*)(struct Sass_Compiler *)) dlsym(sass_lib_handle,"sass_compiler_parse"); err = dlerr(errors); }
					if(!err) { sass_compiler_execute = (int (*)(struct Sass_Compiler *)) dlsym(sass_lib_handle,"sass_compiler_execute"); err = dlerr(errors); }
					if(!err) { sass_data_context_get_options = (struct Sass_Options*(*)(struct Sass_Data_Context *)) dlsym(sass_lib_handle,"sass_data_context_get_options"); err = dlerr(errors); }
					if(!err) { sass_make_data_compiler = (struct Sass_Compiler*(*)(struct Sass_Data_Context *)) dlsym(sass_lib_handle,"sass_make_data_compiler"); err = dlerr(errors); }
					if(!err) { sass_make_data_context = (struct Sass_Data_Context*(*)(char*)) dlsym(sass_lib_handle,"sass_make_data_context"); err = dlerr(errors); }
					if(!err) { sass_data_context_get_context = (struct Sass_Context*(*)(struct Sass_Data_Context*)) dlsym(sass_lib_handle,"sass_data_context_get_context"); err = dlerr(errors); }
					if(!err) { sass_data_context_set_options = (void (*)(struct Sass_Data_Context*, struct Sass_Options*)) dlsym(sass_lib_handle,"sass_data_context_set_options"); err = dlerr(errors); }
					if(!err) { sass_compile_data_context = (int (*)(struct Sass_Data_Context*)) dlsym(sass_lib_handle,"sass_compile_data_context"); err = dlerr(errors); }
					if(!err) { sass_delete_data_context = (void (*)(struct Sass_Data_Context*)) dlsym(sass_lib_handle,"sass_delete_data_context"); err = dlerr(errors); }
					if(!err) { sass_make_options = (struct Sass_Options* (*)()) dlsym(sass_lib_handle,"sass_make_options"); err = dlerr(errors); }
					if(!err) { sass_delete_options = (void (*)(struct Sass_Options*)) dlsym(sass_lib_handle,"sass_delete_options"); err = dlerr(errors); }
					if(!err) { sass_context_get_output_string = (const char* (*)(struct Sass_Context*)) dlsym(sass_lib_handle,"sass_context_get_output_string"); err = dlerr(errors); }
					if(!err) { sass_context_get_source_map_string=(const char* (*)(struct Sass_Context*)) dlsym(sass_lib_handle,"sass_context_get_source_map_string"); err = dlerr(errors); }
					if(!err) { sass_context_get_error_message=(const char* (*)(struct Sass_Context*)) dlsym(sass_lib_handle,"sass_context_get_error_message"); err = dlerr(errors); }
					if(!err) { sass_context_get_error_json=(const char* (*)(struct Sass_Context*)) dlsym(sass_lib_handle,"sass_context_get_error_json"); err = dlerr(errors); }
					if(!err) { sass_context_get_error_status=(int (*)(struct Sass_Context*)) dlsym(sass_lib_handle,"sass_context_get_error_status"); err = dlerr(errors); }
					if(!err) { sass_option_set_output_style = (void (*) (struct Sass_Options*,enum Sass_Output_Style)) dlsym(sass_lib_handle,"sass_option_set_output_style"); err = dlerr(errors); }
					if(!err) { sass_option_set_source_map_file = (void (*) (struct Sass_Options*,const char*)) dlsym(sass_lib_handle,"sass_option_set_source_map_file"); err = dlerr(errors); }
					if(!err) { sass_option_set_omit_source_map_url = (void (*) (struct Sass_Options*,bool)) dlsym(sass_lib_handle,"sass_option_set_omit_source_map_url"); err = dlerr(errors); }
					if(!err) { sass_option_set_is_indented_syntax_src = (void (*) (struct Sass_Options*,bool)) dlsym(sass_lib_handle,"sass_option_set_is_indented_syntax_src"); err = dlerr(errors); }
					if(!err) { sass_option_set_source_comments = (void (*) (struct Sass_Options*,bool)) dlsym(sass_lib_handle,"sass_option_set_source_comments"); err = dlerr(errors); }
					if(!err) { sass_option_set_source_map_contents = (void (*) (struct Sass_Options*,bool)) dlsym(sass_lib_handle,"sass_option_set_source_map_contents"); err = dlerr(errors); }
					if(!err) { sass_option_set_source_map_embed = (void (*) (struct Sass_Options*,bool)) dlsym(sass_lib_handle,"sass_option_set_source_map_embed"); err = dlerr(errors); }
					if(!err) { sass_option_set_include_path = (void (*) (struct Sass_Options*, const char*)) dlsym(sass_lib_handle,"sass_option_set_include_path"); err = dlerr(errors); }
					if(!err) { sass_option_set_precision =(void (*) (struct Sass_Options*,int)) dlsym(sass_lib_handle,"sass_option_set_precision"); err = dlerr(errors); }
					loaded = !err;
				}
			} catch (exception &e) {
				string what = e.what();
				errors << Message(error,what);
			}
			if(loaded) {
				errors << Message(debug,"Sass::startup() The sass library was found and successfully loaded.");
			} else {
				errors << Message(error,"Sass::startup() The sass library was not found.");
			}
		}
		return loaded;
	}

	bool Sass::shutdown() {											 //necessary IFF script uses sass.
		if ( sass_lib_handle != nullptr ) {
			dlclose(sass_lib_handle);
		}
		return true;
	}

	void Sass::addpath(Messages& log, const Path &path) {
		string pathStr = path.output();
		if(path.exists()) {
//			log << Message(debug,"Sass: Adding path " + pathStr);
			inc_paths.insert(pathStr);
		} else {
			log << Message(warn,"Sass: Path " + pathStr + " does not exist so it will not be included for Sass.");
		}
	}

	void Sass::resetpath() {
		inc_paths.clear();
	}

	//• --------------------------------------------------------------------------
	//•	sass expansion.
	bool Sass::expand(Messages& log,string &source,string &result,string &map,string& map_file,bool nestit) {
		int retval=0;
		string empty;
		log << Message(debug,source);
		string pathList;
		for (auto& path : inc_paths) {
			pathList.append(path);
			pathList.push_back(':');
		}
		pathList.pop_back();

		char* src = new char[source.size() + 1];  // ffs. maybe sass decides to free this itself.
		::strcpy(src, source.c_str());

//      https://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char/4152881#4152881
		auto* data_ctx = sass_make_data_context(src);
		auto* ctx = sass_data_context_get_context(data_ctx);
		auto* options = sass_data_context_get_options(data_ctx);

		sass_option_set_precision (options,6);
		sass_option_set_include_path(options,&pathList[0]);  //this is a : delimited list.

		if (nestit) {
			sass_option_set_output_style(options,SASS_STYLE_NESTED);
			sass_option_set_source_map_file(options,&map_file[0]);
			sass_option_set_omit_source_map_url(options,false);
			sass_option_set_is_indented_syntax_src(options,false);
			sass_option_set_source_comments(options,true);
			sass_option_set_source_map_contents(options,true);
			sass_option_set_source_map_embed(options,true);
		} else {
			sass_option_set_output_style(options,SASS_STYLE_COMPRESSED);
			sass_option_set_source_map_file(options,&empty[0]);
			sass_option_set_omit_source_map_url(options,true);
			sass_option_set_is_indented_syntax_src(options,false);
			sass_option_set_source_comments(options,false);
			sass_option_set_source_map_contents(options,true);
			sass_option_set_source_map_embed(options,true);
		}
		sass_data_context_set_options(data_ctx, options);

		//capture cerr.
		ostringstream msge;
		std::streambuf *tmpe = cerr.rdbuf(msge.rdbuf());
		try {
			log << Message(debug,"Sass::compilation about to commence.");
			retval = sass_compile_data_context(data_ctx);
			log << Message(debug,"Sass::compilation is complete.");
		} catch (...) {
			log << Message(error,"Unexpected sass throw. ");
		}

		cerr.rdbuf( tmpe );
		string warnings = msge.str();
		if (!warnings.empty()) {
			log.push(Message(warn,"Sass Warnings that went to cerr!"));
			deque<string> msgs;
			tolist(msgs,warnings,"WARNING: "); //given a cutter(string) delimited set of strings, return a vector of strings.
			for (size_t i=0; i < msgs.size(); i++) {
				if (! msgs[i].empty()) {
					log << Message(warn,msgs[i]);
				}
			}
			log.pop();
		}

		if (retval != 0) {
			switch (retval) {
				case 1: {
					log << Message(error,"Scss Parse/Evaluation error");
				} break;
				case 2: {
					log << Message(error,"Scss Memory allocation error");
				} break;
				case 3:
				case 4:
				case 5: {
					string value = tostring(retval);
					log << Message(error,"Scss Unexpected/Unknown exception error type: " + value);
				} break;
				default: {
					string value = tostring(retval);
					log << Message(error,"Scss Unknown error: " + value);
				} break;
			}
		}
		const char* sass_err  = sass_context_get_error_message(ctx);
		const char* json_err  = sass_context_get_error_json(ctx);
		if (sass_err != nullptr) {
			string sassErrors(sass_err);
			if (! sassErrors.empty()) {
				deque<string> subs;
				Support::tolist(subs, sassErrors, "\n");
				for (size_t j=0; j < subs.size(); j++) {
					log << Message(error, subs[j]);
				}
			}
		}
		if (json_err != nullptr) {
			log << Message(error,string(json_err));
		}
		const char* sass_result = sass_context_get_output_string(ctx);
		if (sass_result != nullptr) {
			result = string(sass_result);
		}
		const char* map_result = sass_context_get_source_map_string(ctx);
		if (map_result != nullptr) {
			map = string(map_result);
		}
		sass_delete_data_context(data_ctx);
		return (retval == 0); //no error..
	}

}