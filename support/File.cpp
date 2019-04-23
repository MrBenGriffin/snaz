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

#include "File.h"
#include "Message.h"
#include "Regex.h"
#include "Date.h"

namespace Support {
	using namespace std;

	std::stack<string> Path::wdstack;
/**
 * Device::Device() { init(); }
	Device::Device(const string &newdevice) : device(newdevice) { init(); }
	Device::Device(const Device &newdevice) {
		init();
		device = newdevice.device;
		root_separator = newdevice.root_separator;
	}

	Device::~Device() = default;

	void Device::init() {
		root_separator = "/";
	}

	void Device::clear() {
		device = "";
	}

	void Device::setRootSeparator(const string &separator) {
		root_separator = separator;
	}

	const string Device::getRootSeparator() const {
		return root_separator;
	}

	void Device::setDevice(const string &newDevice) {
		device = newDevice;
	}

	const string Device::getDevice() const {
		return device;
	}

	const string Device::getDeviceName(bool abs) const {
		string result;
		if (!device.empty()) {
			if (device == root_separator)
				result = root_separator;
			else {
				result = device;
				result.append(root_separator);
			}
		} else {
			if (abs) {
				result = root_separator;
			}
		}
		return result;
	}

	const string Device::output(bool abs=false) const {
		return getDeviceName(abs);
	}
**/
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

	Path::Path() : path(), directory_separator("/"),doWrite(true) {
	}

	Path::Path(const Path &newpath) : doWrite(true) {
		*this = newpath;
	}

	Path::Path(const string newpath) : doWrite(true), path(), directory_separator("/") {
		setPath(newpath);
	}

	void Path::clear() {
		doWrite = true;
		path.clear();
	}

	Path &Path::operator=(const Path &newpath) {
		clear();
		for (auto it : newpath.path) {
			path.push_back(it);
		}
		directory_separator = newpath.directory_separator;
		return *this;
	}

	void Path::setDirectorySeparator(const string separator) {
		directory_separator = separator;
	}

	const string Path::getDirectorySeparator() const {
		return directory_separator;
	}

	void Path::setPath(const string newpath) {
		path.clear();
		cd(newpath);
	}

	void Path::addPath(const string newpath) {
		unsigned long start = 0;
		unsigned long next = newpath.find(directory_separator, start);
		while (next < newpath.length()) {
			// Add an element
			if (next != start) {
				string str(newpath.substr(start, next - start));
				cd(str);
			}
				// if a root separator is the first character
			else if (next == 0) {
				cd("/");
			}
			start = next + 1;
			next = newpath.find(directory_separator, start);
		}
		// Add the last element to the path
		if (start != newpath.length()) {
			string str(newpath.substr(start, newpath.length()));
			cd(str);
		}
	}

	const string Path::getPath() const {
		string result;
		for (int i = 0; i < getPathCount(); i++)
			result += getPath(i) + directory_separator;
		return result;
	}

	const string Path::getPath(size_t index) const {
		if (index < path.size()) {
			return path.at(index);
		}
		return "";
	}

	const int Path::getPathCount() const {
		return (int) path.size();
	}

	const string Path::getEndPath() const {
		return path.back();
	}

	void Path::cd(const string newpath, bool append) {
		if (!newpath.empty()) {
			if (newpath == "/") {
				clear();
			} else {
				size_t dsep = newpath.find(directory_separator, 0);
				if (dsep < newpath.size()) {
					if (append && dsep == 0 && !path.empty()) {
						addPath(newpath.substr(directory_separator.size()));
					} else {
						addPath(newpath);
					}
				} else {
					if (newpath == "..") {
						if (!path.empty()) {
							this->path.pop_back();
						}
					} else {
						if (newpath != ".") {
							this->path.push_back(newpath);
						}
					}
				}
			}
		}
	}

	void Path::listDirs(vector<Path *> *list, bool recursive) const
	{
		struct dirent *dent;
		DIR *dir = opendir(output(false).c_str());
		while ((dent = readdir(dir)) != nullptr)
		{
			string tfilename = dent->d_name;
			// We ignore system directories
			if (tfilename != "." && tfilename != "..")
			{
				auto *path1 = new Path(*this);
				path1->cd(dent->d_name);
				if (path1->exists())
				{
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

	int Path::getSizeA() const {
		string tempPath = output(false);
		// stat does not like '/' terminated paths
		if (!tempPath.empty())
			tempPath = tempPath.erase(tempPath.length() - 1, 1);
		int result;
		struct stat buf{};
		result = stat(tempPath.c_str(), &buf);
		if (result == 0 && buf.st_mode & S_IFDIR)
			return (int)buf.st_size; //64 bit to 32 bit conversion...
		return 0;
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


	const string Path::output(bool abs) const {
		string retval = abs ? "/" : "";
		retval.append(getPath());
		return retval;
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
		string dirstr(output(false).substr(0, output(false).length() - 1));
		result = mkdir(dirstr.c_str(),(mode_t) S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);    //Find these at /usr/include/sys/stat.h
		if (result == 0)
			return true;
		else if (errno == EEXIST || errno == EISDIR)    // If already exists or is a directory
			return true;
		else if ((errno == ENOENT) && recursive) {    // If not exists and recursive
			Path path1 = *this;
			path1.cd("/");
			for (size_t i = 0; i < this->getPathCount() - 1; i++) {
				path1.cd(this->getPath(i));
				if (!path1.makeDir(e,false))
					return false;
			}
			return this->makeDir(e,false);
		} else if (!recursive || errno != ENOENT) {
			return false;
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


	//-------------------------------------------------------------------------
	// Makes the path relative to a specified path.
	//-------------------------------------------------------------------------
	bool Path::makeRelativeTo(const Path &newpath) {
			// relative path must be a subset of the current path
			if (path.size() < newpath.path.size())
				return false;
			int count;
			// Find out when the paths diverge
			for (count = 0; count < newpath.getPathCount(); count++) {
				if (getPath(count) != newpath.getPath(count))
					return false;
			}
			// Removes the relative path elements
			path.erase(path.begin(), path.begin() + count);
			return true;
	}

	//-------------------------------------------------------------------------
	// Makes the current path absolute using a specified path
	//-------------------------------------------------------------------------
	bool Path::makeAbsoluteFrom(const Path &newPath) {
		path = newPath.path;
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

	string Path::generateTempName(const string &newfilename) const {
		ostringstream result;
		Date now;
		string loctime;
		now.getLocalDateStr("%Y%m%d_%H%M%S",loctime);
		result << newfilename << "_" << loctime;
		return result.str();
	}


	bool Path::makeTempDir(Messages& e,const string name) {
		string tail = generateTempName(name);
		cd(tail);
		int i = 0;
		while (!makeDir(e) && i < 10000) {
			cd("..");
			ostringstream newtail;
			newtail << tail << "_" << i;
			cd(newtail.str());
			i++;
		}
		return i >= 10000 ? false : true;
	}

	//-------------------------------------------------------------------------
	// Basic Constructor
	//-------------------------------------------------------------------------
	File::File() : Path(),extension_separator('.') {
	}

	//-------------------------------------------------------------------------
	// Constructs a new File given a specified File
	//-------------------------------------------------------------------------
	File::File(const File &newfile) : Path(newfile) {
		base = newfile.base;
		extension_separator = newfile.extension_separator;
		extension = newfile.extension;
	}


	//-------------------------------------------------------------------------
	//  Constructs a new File given a Path
	//-------------------------------------------------------------------------
	File::File(const Path newpath) : Path(newpath),extension_separator('.') {
	}


	//-------------------------------------------------------------------------
	// Constructs a new File given a Path and a filename
	//-------------------------------------------------------------------------
	File::File(const Path newpath, const string newfilename) : Path(newpath),extension_separator('.') {
		setFileName(newfilename);
	}


	//-------------------------------------------------------------------------
	// Constructs a new File given a filename
	//-------------------------------------------------------------------------
	File::File(const string newfilename) : Path(),extension_separator('.') {
		setFileName(newfilename);
	}


	//-------------------------------------------------------------------------
	// Constructs a new File given a path and filename
	//-------------------------------------------------------------------------
	File::File(const string newpath, const string newfilename) : Path(newpath),extension_separator('.') {
		setFileName(newfilename);
	}


	//-------------------------------------------------------------------------
	// Constructs a new File given a device, path, filename
	//-------------------------------------------------------------------------
//	File::File(const string newdevice, const string newpath, const string newfilename) : Path(newdevice, newpath),extension_separator('.') {
//		setFileName(newfilename);
//	}
//

	//-------------------------------------------------------------------------
	// Construct a new File given a device, path, base, extension
	//-------------------------------------------------------------------------
//	File::File(const string newdevice, const string newpath, const string newbase, const string newextension) : Path(
//			newdevice, newpath),extension_separator('.') {
//		setBase(newbase);
//		setExtension(newextension);
//	}


	//-------------------------------------------------------------------------
	// Clears the File
	//-------------------------------------------------------------------------
	void File::clear() {
		Path::clear();
		base = "";
		extension = "";
	}


	//-------------------------------------------------------------------------
	// Copies the contents from the specified File
	//-------------------------------------------------------------------------
	File &File::operator=(const File newfile) {
		Path::operator=(newfile);
		base = newfile.base;
		extension_separator = newfile.extension_separator;
		extension = newfile.extension;
		return *this;
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
	//-------------------------------------------------------------------------
	const string File::getBase() const {
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
	void File::setFileName(const string newfilename, bool ignoreRoot) {
		string thefile(newfilename);
		if (thefile.compare(0, directory_separator.length(), directory_separator) == 0) {
			size_t slash = thefile.rfind(directory_separator);
			cd(thefile.substr(0, slash), ignoreRoot);
//				setPath(thefile.substr(0, slash));
			thefile = thefile.substr(slash + directory_separator.length(), string::npos);
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
	const string File::getFileName() const {
		string result;
		// No extesnion, return just Base
		if (extension.empty())
			result = getBase();
		else
			result = getBase() + getExtensionSeparator() + getExtension();
		return result;
	}


	//-------------------------------------------------------------------------
	// Returns a formatted FileName
	// abs = true, forces an absolute FileName
	//-------------------------------------------------------------------------
	const string File::output(bool abs=false) const {
		return Path::output(abs) + getFileName();
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
	bool File::moveTo(Messages& e,const File file) const {
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
	bool File::moveTo(Messages& e,const Path newpath) const {
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
	time_t File::getCreateDate() const {
		time_t result = 0;
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
	time_t File::getModDate() const {
		time_t result = 0;
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
	bool File::copyTo(const File newfile, Messages& errstream, bool overwrite) const {
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
	bool File::copyTo(const Path newpath, Messages& errs, bool overwrite) const {
		File file1(newpath, getFileName());
		return copyTo(file1, errs, overwrite);
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

}