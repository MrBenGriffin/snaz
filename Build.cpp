//
// Created by Ben Griffin on 2019-03-06.
//

#include "support/Message.h"
#include "support/db/Connection.h"

#include "Build.h"

using namespace std;
using namespace Support;

Build& Build::b() {
	static Build singleton;
	return singleton;
}

Build::Build() : _current(Final) {

}
/*
 * Currently just load the teams that are available?
 * We could also process the build here.
 * This is NOT 'buildsettings v2' (I hope).
 * */
void Build::load(Support::Messages &errs, Support::Db::Connection &dbc) {

}

