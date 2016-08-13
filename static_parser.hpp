//
//  static_parser.hpp
//
//  description: parses rules specified by metarules.txt
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#pragma once
#ifndef __STATIC_PARSER__
#define __STATIC_PARSER__

#include "static_syms.hpp"
#include <stack>
#include <fstream>
#include <cctype>
#include <cassert>

class adhoc_parser {
	private:
		struct action {
			bool reduce = false;
			signed id;
			action(signed id) : id(id) {}
			action(signed id, bool reduce) : id(id), reduce(reduce) {}
		};

		static vector<map<TOKEN, action*>*>* sr_table;
		static map<string, TOKEN> str_tokmapper;
		static string tablecsv_name;
		
		static void build_table (void);

		list<token*> sr_symsout;
		stack<size_t> sr_stack;
		adhoc_lexer lexer;

		token* reduce (size_t);
	public:
		adhoc_parser (void) { build_table(); }

		// lex and parse input filename 
		void parse (string);

		// get the top node of abstract tree
		program* top (void) { 
			return dynamic_cast<program*>(sr_symsout.front());
		}
};

#endif /* __STATIC_PARSER__ */