//
// Created by Ben on 2019-03-19.
//

#ifndef MACROTEXT_NODE_CONTENT_H
#define MACROTEXT_NODE_CONTENT_H

#include "support/Message.h"
#include "support/db/Connection.h"
#include "node/Node.h"

using namespace Support;
using namespace Db;

namespace node {
	struct FileTemplate {
		string str;
		string suffix;
		string br;
	};

	using idTemplateMap = unordered_map<size_t, pair<string, FileTemplate >>;
	using idIdListMap = unordered_map<size_t, vector<size_t> >;
	using refMap = unordered_map<string, size_t>;    //given a ref, return it's id.
	using invRefMap = unordered_map<size_t, string>; //given an id, return it's ref.
//	using refNodeMap = unordered_map<string, Node *>;
//	using idNodeMap = unordered_map<size_t, Node *>;
//	using idmaptype=unordered_map<size_t, Node *>;
//	using rfmaptype=unordered_map<string, Node *>;

	class Content : public Node {
	private:
		static constexpr auto rootFilename 	= "index";
		static bool _showTemplates;   //show templates in logs..


		string baseFileName;
		string scratchpad;    //used for iForPeers <- maybe should go elsewhere..
		vector<string> filenames;           // This is the initial page filename (includes . )
		vector<string> suffixes;            // This is the initial page suffix.
		vector<string> ffilename;           // This is the full, final filename (post-processed)
		vector<size_t> tplates;             // list of template ID's to use


		void addpage(Messages &, NodeFilename *);
		void outputtofile(size_t page, string &out);
//Generation
		void gettextoutput(Messages &);
		void generateOutput(Messages &, int);
		void generateBranch(Messages &, int);
//		void reset(); //override

	public:
		static deque<Content *> nodeStack;    // current node - used to pass to built-in functions

		bool   get(Messages&,boolValue) const override;
		size_t get(Messages&,uintValue) const override;
		string get(Messages&,textValue) const override;
		Date   get(Messages&,dateValue) const override;


//		enum boolValue  { exec,batch};
//		enum uintValue  { team,layout,pages};
//		enum textValue  { title,shortTitle,tierRef,baseFilename,scope,classCode,synonyms,keywords,descr,fileSuffix,script};
//		enum dateValue  { birth,death};

		flavour cultivar() const override { return content; }
		static void load(Messages&, Connection&);

//		virtual ~Content();            //deleting a Node deletes all of it's children
//		string &getfilename(size_t);
//		string &getsuffix(size_t);
//		string &getffilename(size_t);
//		size_t gettemplate(size_t);

		static void setShowTemplates(bool Show = true) { _showTemplates = Show; }
		static void loadLayouts(Messages &, Connection &);
		static idTemplateMap templateList;
		static idIdListMap layoutList;                //Stores a layout-id -> template-list structure
		static refMap layoutRefs;                     //Layout names -> ids
		static invRefMap layoutNames;                 //Layout ids->names

//		const string &filename(size_t)  const;   //This is the initial page filename (includes . )
//		const string &filename(size_t) const;   //This is the full, final filename (post-processed)
//		string &Suffix(size_t) const;        	//This is the initial page suffix.
//		size_t Template(size_t) const;    		// list of template ID's to use

		void Suffix(size_t, string);           // Set suffix at position.
//		void SetFfilename(size_t, string);     // Set Ffilename at position.
//		void SetFilename(size_t, string);      // Set Filename at position.

		void Layout(Messages &); 				//set the templates vector, resize, and add pages accordingly
		void setLayouts(Messages &); 			//Set all the layouts for a nodetree

//		size_t filenameCount() const;
//		size_t ffilenameCount() const;
//		size_t suffixCount() const;
//		size_t templateCount() const;
//
//		void scratch(string);            //Set scratchpad.
//		string &scratch() const;    	//get scratchpad.
//
//		void basefilename(string);    	//Set basefilename.
//		void incPageCount();    		//increment pagecount (numpages)


	};
}

#endif //MACROTEXT_NODE_CONTENT_H


