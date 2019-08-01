//
// Created by Ben on 2019-02-13.
//

#ifndef MACROTEXT_FILE_H
#define MACROTEXT_FILE_H

#include <string>
#include <vector>
#include <stack>
#include <ctime>

#include "Message.h"

namespace Support {

	using namespace std;

	class File;
	class hashFile;
	class hashPath;

	class Path {
	private:
		bool doWrite;

		string generateTempName(const string &) const;

	protected:
		friend class hashPath;
		static std::stack<string> wdstack;
		vector<string> path;
		static constexpr auto directory_separator = "/";

	protected:
//		int getSizeA() const;

	public:
		Path();

		Path(const Path &);

		explicit Path(string);
//		Path(string,string);

		~Path() = default;

		virtual void clear();

		Path &operator=(const Path &);

		bool operator==(const Path &) const;

		void listFiles(Messages &, vector<File *> *, bool, string) const;

		void listFilesA(Messages &, vector<File *> *, string) const;

//		void setDirectorySeparator(string separator);
//		const string getDirectorySeparator() const;
		void setPath(string);

		void addPath(string);

		const string getPath() const;

		const string getPath(size_t) const;

		string getEndPath() const;

		const int getPathCount() const;

		void cd(const string&, bool= false);

		bool match(Messages &, const string &, const string &) const;

		bool isInside(const Path &) const;

		virtual string output(bool = true) const;

		virtual bool exists() const;

		bool makeDir(Messages &, bool= false) const;

		bool removeDir(Messages &, bool= false, bool= false) const;

		bool moveTo(Messages &, const Path &) const;

		bool copyTo(const Path &, Messages &, bool= false, bool= false) const;

		bool mergeTo(const Path &, Messages &, bool= true, bool= false) const;

		bool head(const Path &, Messages &);

		void listDirs(vector<Path *> *, bool= false) const;

		void setNoOutput() { doWrite = false; }

		bool makeRelativeTo(const Path &);

		bool makeAbsoluteFrom(const Path &);

		bool makeRelative(vector<Path *> *);

		bool makeAbsolute(vector<Path *> *);

		bool makeTempDir(Messages &, string= "TEMP");

		static std::string wd();

		static void cwd(std::string);

		static void push_wd(std::string);

		static void pop_wd();
	};

	class File : public Path {
	protected:
		friend class hashFile;
		string base;
		char extension_separator;
		string extension;
		vector<string> args;

	public:
		File();

		File(const File &);

		explicit File(Path);

		File(Path, const string&);

		explicit File(string);

		File(string, string);

		~File() = default;

		File &operator=(const File &o);

		bool operator==(const File &) const;

		void addArg(const string&);
		void resetArgs();


		void clear() override;

		void setExtensionSeparator(char);

		const char getExtensionSeparator() const;

		void setBase(string);

		string getBase() const;

		void setExtension(string);

		const string getExtension() const;

		void setFileName(const string&, bool= false);

		string getFileName() const;

		const string getDir() const;

		string output(bool) const override;

		bool exists() const override;

		bool moveTo(Messages &,const File&) const;

		bool moveTo(Messages &,const Path&) const;

		bool removeFile() const;

		size_t getSize() const;

		::time_t getCreateDate() const;

		::time_t getModDate() const;

		bool copyTo(const File&, Messages &, bool = false) const;

		bool copyTo(const Path&, Messages &, bool = false) const;

		string readFile() const;

		bool empty() const { return path.empty(); }

		string exec(Messages &, const string& ="") const;
		void write(Messages&,const string&,bool=true) const;

	};    // namespace Support

	struct hashPath {
		uintmax_t operator()(const Path &b) const noexcept; //hash function
	};
	struct hashFile {
		uintmax_t operator()(const File &b) const noexcept; //hash function
	};

};
#endif //MACROTEXT_FILE_H

