
#pragma once

#include <string>
#include <mutex>

#include "tree.hh"
#include <jsoncpp/json/json.h>

namespace cadabra {

	/// \ingroup clientserver
	///
	/// DataCells are the basic building blocks for a document. They are stored 
	/// in a tree inside the client. A user interface should read these cells
	/// and construct corresponding graphical output for them. All cell types are
	/// leaf nodes except for the section cell which generates a new nested structure.
	
	class DataCell {
		public:
			enum class CellType { 
					   document,   ///< head node, only one per document
						input,      ///< code input in verbatim form
						output,     ///< latex-parseable output
						//						comment, 
						//						texcomment, 
						latex,      ///< user-entered LaTeX text
						error,      ///< latex-parseable error text
						// section
						};
			
			DataCell(CellType t=CellType::input, const std::string& str="", bool texhidden=false);
			DataCell(const DataCell&);
			
			CellType                      cell_type;
			std::string                   textbuf;
			std::string                   cdbbuf;       // output only: the output in cadabra input format
			bool                          tex_hidden;   // tex only
			bool                          sensitive;
			bool                          running;

			// Each cell is identified by a serial number 'id' which is used
			// to keep track of it across network calls.

			uint64_t                      id() const;
			
		private:

			static std::mutex             serial_mutex;
			uint64_t                      serial_number;
			static uint64_t               max_serial_number;
	};

	typedef tree<DataCell> DTree;

	/// Serialise a document into .cj format, which is a JSON version of
	/// the document tree.

	std::string JSON_serialise(const DTree&);
	void        JSON_recurse(const DTree&, DTree::iterator, Json::Value&);

	/// Load a document from .cj format, i.e. the inverse of the above.

	void        JSON_deserialise(const std::string&, DTree&);
	void        JSON_in_recurse(DTree& doc, DTree::iterator loc, const Json::Value& cells);
}
