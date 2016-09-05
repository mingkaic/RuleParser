//
//  adhoc_parser.hpp
//
//  description: parses rules specified by metarules.txt
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#pragma once
#ifndef __ADHOC_PARSER__
#define __ADHOC_PARSER__

#include <fstream>
#include <cctype>
#include <cassert>

#include "adhoc_syms.hpp"

namespace adhoc {

class adhoc_parser : public parser {
	private:
        static sr_table* sr;
    
		static std::unordered_map<std::string, TOKEN> str_tokmapper;
		static std::string tablecsv_name;
		
		static void build_table (void);

		virtual token* reduce (size_t);
		
	public:
		adhoc_parser (void) {
			lex = new adhoc_lexer();
			build_table();
			table = sr;
			term = TERM$;
		}
	
		virtual ~adhoc_parser (void) {};
};

}

#endif /* __ADHOC_PARSER__ */