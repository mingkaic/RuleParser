//
//  dynamic_lexer.hpp
//
//  description: tokenizes rules specified by input rule
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#pragma once
#ifndef __DYN_LEXER__
#define __DYN_LEXER__

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream> 
#include <cassert>
#include <stdexcept>
#include <map>
#include <list>

using namespace std;

struct tok_pair {
	size_t t_id;
	string t_str;
	tok_pair (size_t id, string str) {
		t_id = id;
		t_str = str;
	}
};

class super_lexer {
	private:
		map<size_t, string> toks; // list of pairs work too
		list<tok_pair*> t_que;
	
	public:
		super_lexer (void) {}

		bool tok_exist (size_t t_id) { return toks.end() != toks.find(t_id); }

		void new_tok (size_t t_id, string lexeme) {
			if (toks.end() != toks.find(t_id)) {
				throw logic_error("token id collision occurred");
			}
			toks[t_id] = lexeme;
		}

		void lex (istream& content);

		bool empty (void) const { return t_que.empty(); }

		tok_pair* pop (void) {
			tok_pair* id = t_que.front();
			t_que.pop_front();
			return id;
		}
};

#endif /* __DYN_LEXER__ */