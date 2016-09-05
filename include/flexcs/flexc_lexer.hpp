//
//  flexc_lexer.hpp
//
//  description: tokenizes rules specified by input rule
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#pragma once
#ifndef __FLEXC_LEXER__
#define __FLEXC_LEXER__

#include <regex>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream> 
#include <cassert>
#include <stdexcept>
#include <map>
#include <list>

#include "../symbols.hpp"

namespace flexc {

class super_lexer : public lexer {
	private:
		std::unordered_map<size_t, std::string> tok_map; // list of pairs work too
	
	public:
		super_lexer (void) {}

		bool tok_exist (size_t t_id) { return IN_MAP(tok_map, t_id); }

		void new_tok (size_t t_id, std::string lexeme) {
			if (true == IN_MAP(tok_map, t_id)) {
				throw std::logic_error("token id collision occurred");
			}
			tok_map[t_id] = lexeme;
		}

		void lex (std::istream& content);

		bool empty (void) const { return toks.empty(); }

		token* pop (void) {
			token* id = toks.front();
			toks.pop_front();
			return id;
		}
};

}

#endif /* __FLEXC_LEXER__ */