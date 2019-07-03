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

	class Path {
	private:
		bool doWrite;
		string generateTempName(const string &) const;
	protected:
		static std::stack<string> wdstack;
		vector<string> path;
		static constexpr auto directory_separator = "/";

	protected:
//		int getSizeA() const;

	public:
		Path();
		Path(const Path&);

		explicit Path(string);
		Path(string,string);

		~Path() = default;

		virtual void clear();
		Path& operator=(const Path&);
		void listFiles(Messages &,vector<File *> *,bool,string) const;
		void listFilesA(Messages&,vector<File *> *,string) const;
//		void setDirectorySeparator(string separator);
//		const string getDirectorySeparator() const;
		void setPath(string);
		void addPath(string);
		const string getPath() const;
		const string getPath(size_t) const;
		const string getEndPath() const;
		const int getPathCount() const;
		void cd(string,bool=false);
		bool match(Messages&,const string&,const string&) const;
		bool isInside(const Path &) const;
		virtual const string output(bool = true) const;

		virtual bool exists() const;
		bool makeDir(Messages&,bool=false) const;
		bool removeDir(Messages&,bool= false, bool= false) const;
		bool moveTo(Messages&,const Path &) const ;
		bool copyTo(const Path&,  Messages&, bool=false,bool=false) const;
		bool mergeTo(const Path&, Messages&, bool=true,bool=false) const;
		bool head(const Path&, Messages&);

		void listDirs(vector<Path *>*, bool=false) const;

		void setNoOutput() { doWrite = false; }

		bool makeRelativeTo(const Path&);
		bool makeAbsoluteFrom(const Path&);

		bool makeRelative(vector<Path *> *);
		bool makeAbsolute(vector<Path *> *);
		bool makeTempDir(Messages&,string="TEMP");

		static std::string wd();
		static void cwd(std::string);
		static void push_wd(std::string);
		static void pop_wd();
	};
	class File : public Path {
	protected:
		string base;
		char extension_separator;
		string extension;

	public:
		File();
		File(const File &);

		explicit File(Path);
		File(Path,string);

		explicit File(string);
		File(string,string);
		~File()= default;
		void clear() override;
		void setExtensionSeparator(char);
		const char getExtensionSeparator() const;
		void setBase(string);
		const string getBase() const;
		void setExtension(const string);
		const string getExtension() const;
		void setFileName(string,bool=false);
		const string getFileName() const;
		const string getDir() const;
		const string output(bool) const override;
		bool exists() const override;
		bool moveTo(Messages&,File) const;
		bool moveTo(Messages&,Path) const;
		bool removeFile() const;
		size_t getSize() const;
		::time_t getCreateDate() const;
		::time_t getModDate() const;
		bool copyTo(File, Messages&, bool = false) const;
		bool copyTo(Path, Messages&, bool = false) const;
		string readFile() const;
		bool empty() const { return path.empty(); }
		string exec(Messages&,const string&);
		File &operator=(const File &o);

	};

}	// namespace Support


#endif //MACROTEXT_FILE_H
