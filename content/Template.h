//
// Created by Ben on 2019-03-27.
//

#ifndef MACROTEXT_TEMPLATE_H
#define MACROTEXT_TEMPLATE_H


#include "support/Definitions.h"
#include "support/Message.h"
#include "support/db/Connection.h"
#include "node/Content.h"
#include "node/Node.h"
#include "node/Suffix.h"
#include "mt/mt.h"

#include "Layout.h"

using namespace Support;
using namespace Db;

namespace content {
	class Template {
	public:
		size_t id;
		string name;
		mt::mtext code; //Parsed template.
		mt::mtext nl; 	//Parsed newline.
		const node::Suffix* suffix;

		static unordered_map<size_t,Template>   	templates;		//Where templates are stored. Loaded once per build.
		static bool _show;

		static const Template*  get(Messages &,size_t);		//return by id,

		static void load(Messages&,Connection&,buildKind);
		static void show(bool Show = true) { _show = Show; }

	};
}



#endif //MACROTEXT_TEMPLATE_H
