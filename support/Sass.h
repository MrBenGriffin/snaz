//
// Created by Ben on 2019-07-30.
//

#ifndef MACROTEXT_SASS_H
#define MACROTEXT_SASS_H


#include <string>
#include <map>
#include <unordered_map>
#include <sass.h>
#include <sass/context.h>
#include "Message.h"

using namespace std;

extern "C" {
typedef struct sass_context sass;
}

namespace Support {

	class Sass {
	private:
		static void * sass_lib_handle;
		static bool loadattempted;    //used to show if the service is up or down.
		static bool loaded;    //used to show if the service is up or down.

		static string inc_paths;    //colon delimited set of search paths for file inclusion.

		//The sass API that we use.
		static struct Sass_Data_Context *(*sass_make_data_context)(char *);
		static struct Sass_Options *(*sass_make_options)(void);
		static struct Sass_Context *(*sass_data_context_get_context)(struct Sass_Data_Context *);
		static void (*sass_data_context_set_options)(struct Sass_Data_Context *, struct Sass_Options *);
		static int (*sass_compile_data_context)(struct Sass_Data_Context *);
		static void (*sass_delete_data_context)(struct Sass_Data_Context *);
		static const char *(*sass_context_get_output_string)(struct Sass_Context *);
		static const char *(*sass_context_get_source_map_string)(struct Sass_Context *);
		static const char *(*sass_context_get_error_json)(struct Sass_Context *);
		static const char *(*sass_context_get_error_message)(struct Sass_Context *);
		static int (*sass_context_get_error_status)(struct Sass_Context *);

//option functions
		static void (*sass_option_set_output_style)(struct Sass_Options *, enum Sass_Output_Style);
		static void (*sass_option_set_source_map_file)(struct Sass_Options *, const char *);
		static void (*sass_option_set_omit_source_map_url)(struct Sass_Options *, bool);
		static void (*sass_option_set_is_indented_syntax_src)(struct Sass_Options *, bool);
		static void (*sass_option_set_source_comments)(struct Sass_Options *, bool);
		static void (*sass_option_set_source_map_contents)(struct Sass_Options *, bool);
		static void (*sass_option_set_source_map_embed)(struct Sass_Options *, bool);
		static void (*sass_option_set_include_path)(struct Sass_Options *, const char *);
		static void (*sass_option_set_precision)(struct Sass_Options *, int);


	public:
		static bool startup(Messages&);
		static bool available(Messages&);
		static bool shutdown();

		static void resetpath();
		static void addpath(const string &);
		static bool expand(Messages&, const string&, string&, string&, string&,const string&, bool = true);   //expand a string.
	};

}

#endif //MACROTEXT_SASS_H
