//
// Recreated by Ben on 2019-02-13.
//

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <functional>
#include <string>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef __MACH__
#include <copyfile.h>
#else
#include <sys/sendfile.h>  // sendfile
#include <fcntl.h>         // open
#endif

#include "support/File.h"
#include "support/Message.h"
#include "support/Regex.h"
#include "support/Date.h"
#include "support/Env.h"
#include "support/Fandr.h"
#include "support/Convert.h"
#include "support/Encode.h"

namespace Support {
	using namespace std;
	Path Path::siteRoot;

	std::stack<string> Path::wdstack;

	uintmax_t hashPath::operator()(const Path& b) const noexcept { //hash function
		std::hash<std::string> hash_str;
		uintmax_t result = 0x9e3779b97f4a7c17;
		for( auto& part : b.path ) {
			uintmax_t bit = hash_str(part);
			result =  result ^ (bit + 0x9e3779b97f4a7c17 + (result << 6) + (result >> 2));
		}
		return result;
	}

	uintmax_t hashFile::operator()(const File& b) const noexcept { //hash function
		std::hash<std::string> hash_str;
		uintmax_t result = hashPath()(b);
		result =  result ^ (hash_str(b.base) + 0x9e3779b97f4a7c17 + (result << 6) + (result >> 2));
		result =  result ^ (hash_str(b.extension) + 0x9e3779b97f4a7c17 + (result << 6) + (result >> 2));
		for( auto& arg : b.args ) {
			uintmax_t bit = hash_str(arg);
			result =  result ^ (bit + 0x9e3779b97f4a7c17 + (result << 6) + (result >> 2));
		}
		return result;
	}

	void Path::setSiteRoot(Path& root) {
		siteRoot = std::move(root);
	}

	bool Path::match(Messages& e,const string &text, const string &pattern) const {
		bool retval = false;
		if ( Regex::available(e) ) {
			retval = Regex::match(e,pattern,text);
		}
		return retval;
	}

	void Path::push_wd(std::string new_wd) {
		char tmp[255];
		if (!new_wd.empty()) {
			const char *wdptr = new_wd.c_str();
			if (chdir(wdptr) != 0) { /* report an error here. */ }
			else {
				getcwd(tmp, 200);
				string ccwd(tmp);
				wdstack.push(ccwd);
			}
		} else {
			getcwd(tmp, 200);
			string ccwd(tmp);
			wdstack.push(ccwd);
		}
	}

	void Path::pop_wd() {
		wdstack.pop();
		if (!wdstack.empty()) {
			const char *wdptr = wdstack.top().c_str();
			if (chdir(wdptr) != 0) { /* report an error here. */ }
		}
	}

	std::string Path::wd() {
		string retval;
		if (!wdstack.empty()) {
			retval = wdstack.top();
		}
		return retval;
	}

	void Path::cwd(std::string path) {
		const char *wdptr = path.c_str();
		if (chdir(wdptr) != 0) { /* report an error here. */ }
	}

	Path::Path() : path(), doWrite(true),relative(false) {
		path = siteRoot.path;
	}

	Path::Path(const Path &newpath) : doWrite(true) {
		*this = newpath;
	}

	Path::Path(const string newpath) : doWrite(true),relative(true), path() {
		setPath(newpath);
	}

	void Path::clear() {
		doWrite = true;
		path.clear();
	}

	Path &Path::operator=(const Path &o) {
		relative = o.relative;
		path.clear();
		path.reserve(o.path.size());
		for (auto& p : o.path) {
			path.push_back(p);
		}
		return *this;
	}

	bool Path::operator==(const Path& o) const {
		return relative == o.relative && path == o.path;
	}

	void Path::setPath(const string& newPath) {
		path.clear();
		cd(newPath);
	}

	void Path::addPath(const string& newPath) {
		unsigned long start = 0;
		unsigned long next = newPath.find(directory_separator, start);
		while (next < newPath.length()) {
			// Add an element
			if (next != start) {
				string str(newPath.substr(start, next - start));
				cd(str);
			}
				// if a root separator is the first character
			else if (next == 0) {
				cd("/");
			}
			start = next + 1;
			next = newPath.find(directory_separator, start);
		}
		// Add the last element to the path
		if (start != newPath.length()) {
			string str(newPath.substr(start, newPath.length()));
			cd(str);
		}
	}

	string Path::getPath(size_t offset) const {
		ostringstream result;
		for (size_t i = offset; i < path.size(); i++) {
			result << path[i] << directory_separator;
		}
		return result.str();
	}

	string Path::getDirAt(size_t index) const {
		if (index < path.size()) {
			return path.at(index);
		}
		return "";
	}

	int Path::getPathCount() const {
		return (int) path.size();
	}

	string Path::getEndPath() const {
		return path.back();
	}

	void Path::cd(const string& newPath, bool append) {
		/**
		 * Append=true only has an effect if the initial character of the newpath is a '/'
		 * So, normally we have append = false - if we set this to "/tmp/x" we want to change the directory.
		 */
		if (!newPath.empty()) {
			if (newPath == "/") {
				clear();
				relative = false;
			} else {
				size_t dsep = newPath.find(directory_separator, 0);
				if (dsep < newPath.size()) {
					if (append && dsep == 0 && !path.empty()) {
						addPath(newPath.substr(1));
					} else {
						addPath(newPath);
					}
				} else {
					if (newPath == "..") {
						pop();
					} else {
						if (newPath != ".") {
							this->path.push_back(newPath);
						}
					}
				}
			}
		}
	}

	void Path::pop() {
		if(!path.empty()) {
			path.pop_back();
		}
	}

	void Path::listDirs(vector<Path *> *list, bool recursive) const
	{
		struct dirent *dent;
		DIR *dir = opendir(output(false).c_str());
		while ((dent = readdir(dir)) != nullptr) {
			string tfilename = dent->d_name;
			// We ignore system directories
			if (tfilename != "." && tfilename != "..") {
				auto *path1 = new Path(*this);
				path1->cd(dent->d_name);
				if (path1->exists()) {
					list->push_back(path1);
					if (recursive)
						path1->listDirs(list, recursive);
				}
				else
					delete path1;
			}
		}
		closedir(dir);
	}


	void Path::listFilesA(Messages& e, vector<File *> *list,string regexp) const {
		struct dirent *dent;
		DIR *dir = opendir(output(false).c_str());
		while ((dent = readdir(dir)) != nullptr)
		{
			File *file1 = new File(*this, dent->d_name);
			if (match(e,file1->output(false), regexp))
			{
				if (file1->exists())
				{
					list->push_back(file1);
					continue;
				}
			}
			delete file1;
		}
		closedir(dir);
	}

	void Path::listFiles(Messages &e, vector<File *> *list, bool recursive, const string regexp) const {
		auto *dirList = new vector<Path *>;
		// Create s list of directories to scan
		dirList->push_back(new Path(*this));
		if (recursive)
			listDirs(dirList, true);
		for (auto &it : *dirList)
			((Path *) it)->listFilesA(e, list, regexp);
	}

	string Path::output(bool trailing) const {
		ostringstream result;
		if(!relative) {
			result << "/";
		}
		if(!path.empty()) {
			size_t p = path.size() - 1;
			for (size_t i = 0; i < p; i++) {
				result << path[i] << directory_separator;
			}
		}
		result << path.back();
		if(trailing) {
			result << directory_separator;
		}
		return result.str();
	}

	bool Path::exists() const {
		int result;
		string tempPath = output(true);
		if (!tempPath.empty())
			tempPath = tempPath.erase(tempPath.length() - 1, 1);
		struct stat buf{};
		result = stat(tempPath.c_str(), &buf);
		return result == 0 && buf.st_mode & S_IFDIR;
	}

	bool Path::makeDir(Messages &e, bool recursive) const {
		int result;
		string dirStr;
		if(relative) {
			Path full(*this);
			full.makeAbsoluteFrom(siteRoot);
			dirStr = full.output();
		} else {
			dirStr = output();
		}
		if(relative || isInside(siteRoot)) {
			result = mkdir(dirStr.c_str(),(mode_t) S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);    //Find these at /usr/include/sys/stat.h
			if (result == 0)
				return true;
			else if (errno == EEXIST || errno == EISDIR)    // If already exists or is a directory
				return true;
			else if ((errno == ENOENT) && recursive) {    // If not exists and recursive
				Path path1 = *this;
				path1.cd("/");
				for (size_t i = 0; i < path.size() - 1; i++) {
					path1.cd(this->getDirAt(i));
					if (!path1.makeDir(e,false))
						return false;
				}
				return this->makeDir(e,false);
			}
		} else {
			string rootStr(siteRoot.output(true));
			e << Message(range,"The path " + dirStr + " is not inside the directory " + rootStr);
		}
		return false;
	}

	bool Path::removeDir(Messages& e,bool recursive, bool stop_on_error) const {
		int result;
		string dirstr(output(false).substr(0, output(false).length() - 1));
		// Assume the directory is a link first
		result = unlink(dirstr.c_str());
		if (result == 0)
			return true;
		// Not a link, treat as a normal directory
		result = rmdir(dirstr.c_str());
		if (result == 0) {
			return true;
		} else if (recursive && errno == ENOTEMPTY) {
			bool result2 = true;
			auto *dirlist = new vector<Path *>;
			auto *filelist = new vector<File *>;
			// Build a list of directories to scan
			dirlist->push_back(new Path(*this));
			listDirs(dirlist, true);
			// Build a list of files
			for (auto &it : *dirlist)
				((Path *) it)->listFiles(e,filelist,false,".*");
			// Removes all the files
			for (auto &it2 : *filelist) {
				bool res = ((File *) it2)->removeFile();
				if (!res && stop_on_error)
					return false;
				result2 &= res;
			}
			// Remove all the directories in reverse order
			for (auto it3 = dirlist->rbegin(); it3 != dirlist->rend(); it3++) {
				bool res = ((Path *) *it3)->removeDir(e,recursive,stop_on_error);
				if (!res && stop_on_error)
					return false;
				result2 &= res;
			}
			return result2;
		}
		return false;;
	}

	bool Path::moveTo(Messages& e,const Path &newpath) const {
		ostringstream errs;
		std::string pathName = output(false);
		if (exists()) {
			Path path1 = newpath;
			// If the destination already exists
			// we move into the destination directory
			if (newpath.exists())
				path1.cd(getEndPath());

			std::string path1Name = path1.output(false);
			// If this new directory already exists, we fail
			if (path1.exists()) {
				errs << "Path::moveTo error:" << path1Name.substr(0, pathName.length() - 1).c_str() << " already exists.";
				e << Message(error,errs.str());
				return false;
			} else {
				int result = rename(pathName.substr(0, pathName.length() - 1).c_str(),
									path1Name.substr(0, path1Name.length() - 1).c_str());
				if (result == 0) {
					chmod(path1Name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
					return true;
				} else {
					result = errno;  //a stdio global, of all things.
					string error_result;
					switch (result) {
						case EACCES:
							error_result = "Permission denied";
							break;
						case EBUSY:
							error_result = "Device or Resource busy";
							break;
						case EEXIST:
							error_result = "File exists";
							break;
						case EINVAL:
							error_result = "Invalid argument";
							break;
						case EIO:
							error_result = "Input/output error";
							break;
						case EISDIR:
							error_result = "Is a directory";
							break;
						case ELOOP:
							error_result = "Too many levels of symbolic links";
							break;
						case EMLINK:
							error_result = "Too many links";
							break;
						case ENAMETOOLONG:
							error_result = "Name is too long";
							break;
						case ENOENT:
							error_result = "No such file or directory";
							break;
						case ENOSPC:
							error_result = "No space left on device";
							break;
						case ENOTDIR:
							error_result = "Not a directory";
							break;
						case ENOTEMPTY:
							error_result = "Directory not empty";
							break;
						case EPERM:
							error_result = "Operation not permitted";
							break;
						case EROFS:
							error_result = "Read-only file system";
							break;
						case EXDEV:
							error_result = "Cross-device link";
							break;
						default:
							error_result = "unknown";
							break;
					}
					errs << "FileUtils::Path::moveTo error: " << result << " [" << error_result
						 << "] for rename " << pathName.substr(0, pathName.length() - 1) << " to "
						 << path1Name.substr(0, path1Name.length() - 1);
					e << Message(error,errs.str());
				}
			}
		} else {
			errs << "FileUtils::Path::moveTo error: " << pathName.substr(0, pathName.length() - 1) << " doesn't exist.";
			e << Message(error,errs.str());
		}
		return false;
	}

	/**
	void Path::listDirs(vector<Path *> *list, bool recursive) const {
		struct dirent *dent;
		DIR *dir = opendir(output().c_str());
		while ((dent = readdir(dir)) != nullptr) {
			string tfilename = dent->d_name;
			// We ignore system directories
			if (tfilename != "." && tfilename != "..") {
				auto *path1 = new Path(*this);
				path1->cd(dent->d_name);
				if (path1->exists()) {
					list->push_back(path1);
					if (recursive)
						path1->listDirs(list, recursive);
				} else
					delete path1;
			}
		}
		closedir(dir);
	}
	 **/

	bool Path::copyTo(const Path &newpath, Messages& e, bool overwrite, bool stop_on_error) const {
		if (!exists()) {
			return false;
		}
		auto *dirlist = new vector<Path *>;
		auto *filelist = new vector<File *>;
		bool dest_exists = false;
		// if dest exists, we copy ourselves into the directory
		if (newpath.exists())
			dest_exists = true;

		// Make a list of directories
		dirlist->push_back(new Path(*this));
		listDirs(dirlist, true);
		// Make a list of Files
		for (auto &it : *dirlist)
			((Path *) it)->listFiles(e,filelist,false,".*");
		// Make the directories themselves
		for (auto &it2 : *dirlist) {
			Path tnewpath(newpath);
			if (dest_exists)
				tnewpath.cd(getEndPath());
			Path tpathdest((Path) *it2);
			tpathdest.makeRelativeTo(*this);
			tpathdest.makeAbsoluteFrom(tnewpath);
			if (!tpathdest.makeDir(e) && stop_on_error) {
				delete filelist;
				return false;
			}
		}
		for (auto &it3 : *filelist) {
			File tnewfile(newpath);
			if (dest_exists)
				tnewfile.cd(getEndPath());
			File tfiledest((File) *it3);
			tfiledest.makeRelativeTo(*this);
			tfiledest.makeAbsoluteFrom(tnewfile);
			if (!((File *) it3)->copyTo(tfiledest, e, overwrite) && stop_on_error) {
				return false;
			}
		}
		return true;
	}

	//-------------------------------------------------------------------------
	// mERGES an entire directory tree to a specified path
	// overwrite = true, Overwrites existing files
	// stop_on_error = true, stops as soon as an error occurs
	//-------------------------------------------------------------------------
	bool Path::mergeTo(const Path &newpath, Messages& e, bool overwrite, bool stop_on_error) const {
		if (!exists())
			return false;
		auto *dirlist = new vector<Path *>;
		auto *filelist = new vector<File *>;
		// if dest exists, we copy ourselves into the directory
		if (!newpath.exists()) {
			delete dirlist;
			delete filelist;
			return copyTo(newpath,e);
		}
		// Make a list of directories
		dirlist->push_back(new Path(*this));
		listDirs(dirlist, true);
		// Make a list of Files
		for (auto &it : *dirlist)
			((Path *) it)->listFiles(e,filelist,false,".*");
		// Make the directories themselves
		for (auto &it2 : *dirlist) {
			const Path &tnewpath(newpath);
			Path tpathdest((Path) *it2);
			tpathdest.makeRelativeTo(*this);
			tpathdest.makeAbsoluteFrom(tnewpath);
			if (!tpathdest.makeDir(e) && stop_on_error) {
				delete filelist;
				return false;
			}
		}
//		size_t filecount = filelist->size(); //put in a filecount status for every 100 files.
		size_t count = 0;
		for (auto &it3 : *filelist) {
			File tnewfile(newpath);
			File tfiledest((File) *it3);
			tfiledest.makeRelativeTo(*this);
			tfiledest.makeAbsoluteFrom(tnewfile);
			//cout << "Merging " << ((File *)*it3)->output() << " to " << tfiledest.output() << "<br>" << std::end;
			if (!((File *) it3)->copyTo(tfiledest, e, overwrite) && stop_on_error) {
				return false;
			}
			if (count++ >= 100) {
				count = 0;
				cout << "." << flush;
			}
		}
		return true;
	}

	bool Path::isInside(const Path &basePath) const {
		if (path.size() < basePath.path.size())
			return false;
		for (size_t count = 0; count < basePath.path.size(); count++) {
			if (path[count] != basePath.path[count])
				return false;
		}
		return true;
	}

	//-------------------------------------------------------------------------
	// Makes the path relative to a specified path.
	//-------------------------------------------------------------------------
	bool Path::makeRelativeTo(const Path &newpath) {
		if (!relative) {
			relative = true;
			// relative path must be a subset of the current path
			if (path.size() < newpath.path.size())
				return false;
			int count;
			// Find out when the paths diverge
			for (count = 0; count < newpath.getPathCount(); count++) {
				if (getDirAt(count) != newpath.getDirAt(count))
					return false;
			}
			// Removes the relative path elements
			path.erase(path.begin(), path.begin() + count);
		}
		return true;
	}

	//-------------------------------------------------------------------------
	// Makes the current path absolute using a specified path
	//-------------------------------------------------------------------------
	bool Path::makeAbsoluteFrom(const Path &base) {
		auto old = path;
		path = base.path;
		size_t i=0;
		if(!relative) {
			for (; i < old.size() && i < path.size(); i++) {
				if (old[i] != path[i]) break;
			}
		}
		for(; i < old.size(); i++) {
			path.emplace_back(old[i]);
		}
		relative = false;
		return true;
	}

	//-------------------------------------------------------------------------
	// Make all the specified paths relative to the current path
	//-------------------------------------------------------------------------
	bool Path::makeRelative(vector<Path *> *list) {
		for (auto &it : *list) {
			if (!((Path *) it)->makeRelativeTo(*this))
				return false;
		}
		return true;
	}

	//-------------------------------------------------------------------------
	// Make all the specified paths absolute to the current path
	//-------------------------------------------------------------------------
	bool Path::makeAbsolute(vector<Path *> *list) {
		for (auto &it : *list) {
			if (!((Path *) it)->makeAbsoluteFrom(*this))
				return false;
		}
		return true;
	}

/**
 * head uses this path as the head of the tail, and adds tail to this.
 * Eg, this = /var/www/alpha/foo and bar= /wibble/alpha/foo/bar
 *     this => /var/www/alpha/bar
 */
	bool Path::head(const Path& tail, Messages& errs) {
		auto& tailPath = tail.path;
		size_t i=0,j=0;
		if(!tail.relative) {
			for (; i < path.size(); i++) {
				for (j = 0; j < path.size() && j < tailPath.size(); j++) {
					if (path[i] == tailPath[j]) break;
				}
				if (j < tailPath.size()) {
					break;
				}
			}
		} else {
			i = tailPath.size();
		}
		path.resize(i);
		for(; j < tailPath.size(); j++) {
			path.emplace_back(tailPath[j]);
		}
		return true;
	}

	string Path::generateTempName(const string &newfilename) const {
		ostringstream result;
		Date now;
		string loctime;
		now.getLocalDateStr("%Y%m%d_%H%M%S",loctime);
		result << newfilename << "_" << loctime;
		return result.str();
	}


	bool Path::makeTempDir(Messages& e,const string name) {
		cd(generateTempName(name));
		return makeDir(e);
	}




	//-------------------------------------------------------------------------
	// Basic Constructor
	//-------------------------------------------------------------------------
	File::File() : Path(),extension_separator('.') {
	}

	//Assignment operator.
	File &File::operator=(const File &o) {
		if (this != &o) {
			Path::operator=(o);
			base = o.base;
			extension_separator = o.extension_separator;
			extension = o.extension;
			args.clear();
			args.reserve(o.args.size());
			for (auto& arg : o.args) {
				args.push_back(arg);
			}
		}
		return *this;
	}

	bool File::operator==(const File& o) const {
		return Path::operator==(o)
			&& base == o.base
			&& extension_separator == o.extension_separator
			&& extension == o.extension
			&& args == o.args;
	}

	//-------------------------------------------------------------------------
	// Constructs a new File given a specified File
	//-------------------------------------------------------------------------
	File::File(const File &ofile) : Path(ofile) {
		base = ofile.base;
		extension_separator = ofile.extension_separator;
		extension = ofile.extension;
		args = ofile.args;
	}

	//-------------------------------------------------------------------------
	//  Constructs a new File given a Path
	//-------------------------------------------------------------------------
	File::File(const Path newpath) : Path(newpath),extension_separator('.') {
	}

	void File::resetArgs() {
		args.clear();
	}

	//-------------------------------------------------------------------------
	//  Execute a file..
	//-------------------------------------------------------------------------
	string File::exec(Messages& errs,const string& extra) const {
		string result;
		if (exists()) {
			ostringstream execute;
			execute << output(true);
			for (auto& arg : args) {
				execute << " " << arg;
			}
			if(!extra.empty()) {
				execute << " " << extra;
			}
			execute << " 2>&1";
			const int max_buffer = 256;
			char buffer[max_buffer] = {0};
			string command = execute.str();
			errs << Message(debug, command);

			FILE* pipe = popen(command.c_str(),"r");
			while (!feof(pipe)) {
				char* size = fgets(buffer, max_buffer, pipe);
				if(size) {
					result.append(buffer);
				}
			}
			pclose(pipe);
		} else {
			string name = output(true);
			errs << Message(error,name + " does not exist.");
		}
		vector<string> lines;
		tolist(lines,result,"\n");
		for(auto &line : lines) {
			channel kind = debug;
			trim(line);
			if(line.size() > 4) {
				string uline=line;
				toupper(uline);
				if((uline.find("FATAL") != string::npos)) kind = fatal;
				if(kind==debug && (uline.find("SYNTAX") != string::npos)) kind = 	syntax;
				if(kind==debug && (uline.find("PARSE") != string::npos))  kind = 	syntax;
				if(kind==debug && (uline.find("ERROR") != string::npos)) kind =  	error;
				if(kind==debug && (uline.find("WARN") != string::npos))  kind =  	warn;
				if(kind==debug && (uline.find("UNEXPECTED") != string::npos)) kind= warn;
				if(kind==debug && (uline.find("DEPRECATED") != string::npos)) kind= deprecated;
			}
			errs << Message(kind,line);
		}
		return result;
	}

	//-------------------------------------------------------------------------
	// Constructs a new File given a Path and a filename
	//-------------------------------------------------------------------------
	File::File(const Path newpath, const string& newfilename) : Path(newpath),extension_separator('.') {
		setFileName(newfilename);
	}

	//-------------------------------------------------------------------------
	// Constructs a new File given a filename
	//-------------------------------------------------------------------------
	File::File(const string newFilename) : Path(),extension_separator('.') {
		setFileName(newFilename);
	}


	void File::addArg(const string& argument) {
		args.push_back(argument);
	}

	//-------------------------------------------------------------------------
	// Clears the File
	//-------------------------------------------------------------------------
	void File::clear() {
		Path::clear();
		base = "";
		extension = "";
		args.clear();
	}


	//-------------------------------------------------------------------------
	// Sets the extension separator
	//-------------------------------------------------------------------------
	void File::setExtensionSeparator(const char separator) {
		extension_separator = separator;
	}


	//-------------------------------------------------------------------------
	// Gets the extension separator
	//-------------------------------------------------------------------------
	const char File::getExtensionSeparator() const {
		return extension_separator;
	}

	const string File::getDir() const {
		return Path::output(true);
	}

	//-------------------------------------------------------------------------
	// Sets the filename Base
	//-------------------------------------------------------------------------
	void File::setBase(const string newbase) {
		base = newbase;
	}


	//-------------------------------------------------------------------------
	// Gets the filename Base
	// if withPath is true, then it is the same as output(true) without the extension.
	//-------------------------------------------------------------------------
	string File::getBase() const {
			return base;
	}


	//-------------------------------------------------------------------------
	// Gets the filename Extension
	//-------------------------------------------------------------------------
	void File::setExtension(const string newextension) {
		extension = newextension;
	}


	//-------------------------------------------------------------------------
	// Sets the filename Extension
	//-------------------------------------------------------------------------
	const string File::getExtension() const {
		return extension;
	}

	//-------------------------------------------------------------------------
	// Sets the filename
	//-------------------------------------------------------------------------
	void File::setFileName(const string& newfilename, bool ignoreRoot) {
		string thefile(newfilename);
		if (thefile.compare(0, 1, directory_separator) == 0) {
			size_t slash = thefile.rfind(directory_separator);
			cd(thefile.substr(0, slash), ignoreRoot);
//				setPath(thefile.substr(0, slash));
			thefile = thefile.substr(slash + 1, string::npos);
		}
		size_t dot = thefile.rfind(getExtensionSeparator());
		if (dot != string::npos) {
			// if the filename ends with a extension separator
			if (dot == thefile.length() - 1) {
				base = thefile.substr(0, string::npos);
				extension = "";
			} else {
				base = thefile.substr(0, dot);
				extension = thefile.substr(dot + 1, string::npos);
			}
		}
			// No extension separator
		else {
			base = thefile;
			extension = "";
		}
	}


	//-------------------------------------------------------------------------
	// Gets the formatted FileName
	//-------------------------------------------------------------------------
	string File::getFileName() const {
		string result = base;
		if (!extension.empty())
			result.append(extension_separator + extension);
		return result;
	}

	//-------------------------------------------------------------------------
	// Returns a formatted FileName
	// abs = true, forces an absolute FileName
	//-------------------------------------------------------------------------
	string File::output(bool abs=false) const {
		return Path::output(true) + getFileName();
	}

	//-------------------------------------------------------------------------
	// Checks whether the file exists
	//-------------------------------------------------------------------------
	bool File::exists() const {
		bool existence = false;
		struct stat buf{};
		string fileToTest = output(true);
		int result = stat(fileToTest.c_str(), &buf);
		if (result == 0 && (buf.st_mode & S_IFREG))  {
			existence = true;
		}
		return existence;
	}
	//-------------------------------------------------------------------------
	// Moves the current File to the specified File
	//-------------------------------------------------------------------------
	bool File::moveTo(Messages&,const File& file) const {
		if (file.exists())
			return false;
		File newfile = file;
		// Empty filename, sets as current filename
		if (newfile.getFileName().empty())
			newfile.setFileName(getFileName());
		int result = rename(output().c_str(), newfile.output().c_str());
		if (result == 0) {
			chmod(output().c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
			return true;
		}
		return false;
	}


	//-------------------------------------------------------------------------
	// Moves the current File to the specified Path
	//-------------------------------------------------------------------------
	bool File::moveTo(Messages&,const Path& newpath) const {
		if (!newpath.exists())
			return false;
		File file1(newpath, getFileName());
		if (!file1.exists()) {
			int result = rename(output().c_str(), file1.output().c_str());
			if (result == 0) {
				chmod(output().c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
				return true;
			}
		}
		return false;
	}


	//-------------------------------------------------------------------------
	// Removes the current File
	//-------------------------------------------------------------------------
	bool File::removeFile() const {
		bool result = false;
		if (unlink(output().c_str()) == 0) {
			result = true;
		}
		return result;
	}


	//-------------------------------------------------------------------------
	// Gets the size of the current File
	//-------------------------------------------------------------------------
	size_t File::getSize() const {
		size_t result;
		struct stat buf{};
		result = stat(output(true).c_str(), &buf);
		if (result == 0 && buf.st_mode & S_IFREG) {
			return (size_t) buf.st_size; //64 bit to 32 bit conversion...
		}
		return 0;
	}

	//-------------------------------------------------------------------------
	// Gets the creation date of the current File
	//-------------------------------------------------------------------------
	::time_t File::getCreateDate() const {
		::time_t result = 0;
		struct stat buf{};
		result = stat(output().c_str(), &buf);
		if (result == 0 && buf.st_mode & S_IFREG) {
			result = buf.st_ctime;
		}
		return result;
	}

	//-------------------------------------------------------------------------
	// Gets the last modification date of the current File
	//-------------------------------------------------------------------------
	::time_t File::getModDate() const {
		::time_t result = 0;
		struct stat buf{};
		if ((stat(output().c_str(), &buf) == 0) && (buf.st_mode & S_IFREG)) {
			result = buf.st_mtime;
		}
		return result;
	}

	//-------------------------------------------------------------------------
	// Copies the current File to the specified File
	// overwrite = true, overwrites the dest
	//-------------------------------------------------------------------------
	bool File::copyTo(const File& newfile, Messages& errstream, bool overwrite) const {
		bool retval = false;
		string from_path = output();
		string dest_path = newfile.output();
		//cout << "Copying " << from_path.c_str() << " to " << dest_path.c_str() << std::end;
		if ((newfile.exists() && overwrite) || !newfile.exists()) {
#ifdef __MACH__
			copyfile(from_path.c_str(), dest_path.c_str(), nullptr, COPYFILE_ALL);
#else
			FILE *in = fopen(from_path.c_str(), "rb");
				if (in != nullptr) {
					FILE *out = fopen(dest_path.c_str(), "w+b");
					struct stat stat_source;
					int success = fstat(fileno(in), &stat_source);
					if (success == 0 && S_ISREG(stat_source.st_mode)) {
						ssize_t result = sendfile(fileno(out),fileno(in), 0, stat_source.st_size);
						if (result == -1)	{
							switch (errno) {
								case EAGAIN:
									errstream << Message(error,"EAGAIN: Nonblocking I/O has been selected using O_NONBLOCK and the write would block.");
									break;
								case EBADF:
									errstream << Message(error,"EBADF: The input file was not opened for reading or the output file was not opened for writing.");
									break;
								case EFAULT:
									errstream << Message(error,"EFAULT: Bad address.");
									break;
								case EINVAL:
									errstream << Message(error,"EINVAL: Descriptor is not valid or locked, or an mmap(2)-like operation is not available for in_fd.");
									break;
								case EIO:
									errstream << Message(error,"EIO: Unspecified error while reading from in_fd.");
								case ENOMEM:
									errstream << Message(error,"ENOMEM: Insufficient memory to read from in_fd.");
									break;
								case ENOSYS:
									errstream << Message(error,"ENOSYS: The system doesn't support that function.");
									break;
								default: break;
								}
								if (errno == EINVAL || errno == ENOSYS) {
									retval = true;
									struct timeval times[2] = {{0,0},{0,0}};	/* initialise microseconds to zero */
									times[0].tv_sec = stat_source.st_atime;			/* time of last access */
									times[1].tv_sec = stat_source.st_mtime;			/* time of last modification */
									int err = chmod(dest_path.c_str(),S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
									if (err != 0) { string ers = strerror(errno);
										errstream << Message(error,ers);
										retval = false;

									}
									char buf[8192];
									size_t i;
									while((i = fread(buf, 1, 8192, in)) != 0) {
										fwrite(buf, 1, i, out);
									}
									err = utimes(dest_path.c_str(),times);
									if (err != 0) {
										string ers = strerror(errno);
										errstream << Message(error,ers);
										retval = false;
									}
								}
						}
					}
					fclose(out);
				}
				fclose(in);
#endif
		}
		return retval;
	}

	//-------------------------------------------------------------------------
	// Copy the current File to the specified Path
	// overwrite = true, overwrites the dest
	//-------------------------------------------------------------------------
	bool File::copyTo(const Path& newpath, Messages& errs, bool overwrite) const {
		File file1(newpath, getFileName());
		return copyTo(file1, errs, overwrite);
	}

	//-------------------------------------------------------------------------
	// writes a string into the file...
	// overwrite = true, overwrites the dest - otherwise skip.
	//-------------------------------------------------------------------------
	void File::write(Messages& errs,const string& contents,bool overwrite) const {
		size_t size = contents.size();
		if (size > 0 && (overwrite || !exists())) {
			string name = output(true);
			ofstream outFile(name.c_str());
			outFile << contents;
			outFile.close();
			chmod(name.c_str(),S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		}
	}

	//-------------------------------------------------------------------------
	// Copies a file into a string
	//-------------------------------------------------------------------------
	string File::readFile() const {
		string result;
		if (exists()) {
			size_t size = getSize();
			if (size > 0) {
				FILE *in = fopen(output(true).c_str(), "rb");
				if (in) {
					auto *tbuf = new char[size + 1];
					fread(tbuf, 1, size, in);
					fclose(in);
					result.assign(tbuf, size);
					delete[] tbuf;
				}
			}
		}
		return result;
	}

	//-------------------------------------------------------------------------
	// Return file as a URL.
	//-------------------------------------------------------------------------
	// 	Path Env::basedir(buildSpace space) {

//	std::string Env::baseUrl(buildArea area) {
	string File::url(Messages& errs,buildSpace space) const {
		if(relative) {
			return "/" + output(true);
		} else {
			string result;
			ostringstream msg;
			Path urlBase = Env::e().urlRoot(space);
			auto baseSize = urlBase.path.size();
			auto pathSize = path.size();
			if (isInside(siteRoot)) {
				if (pathSize == baseSize) {
					result = getFileName();
				}
				return getPath(baseSize - path.size()) + result;
			} else {
				string file = output(true);
				msg << "The file " << file << " isn't within the site url root";
				errs << Message(warn, msg.str());
			}
			return result;
		}
	}

}
