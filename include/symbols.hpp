//
//  symbols.hpp
//
//  description: structures used in the symbol program
//
//  Created by Ming Kai Chen on 2016-08-18.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#include "utils.hpp"

#pragma once
#ifndef __SYM_STRUCT__
#define __SYM_STRUCT__

#include <list>
#include <stack>
#include <vector>
#include <unordered_map>

#ifndef _TERMINAL_
	#include "shared_map.hpp"
	#include <iostream>
#endif

class token {
	protected:
		size_t t_id;
	public:
		std::string content;
		token (void) {}
		token (size_t t_id) : t_id(t_id) {}
		token (size_t t_id, std::string content) : t_id(t_id), content(content) {}
		virtual ~token (void) {}
		virtual void set_tok (size_t t_id) { this->t_id = t_id; }
		virtual size_t get_tok (void) { return t_id; }
};

class sr_table {
	private:
		std::vector<std::unordered_map<size_t, std::pair<size_t, bool> > >table;

	public:
		void insert (size_t state, size_t t_id,
			size_t act_id, bool reduce = false);
	
		bool exists (size_t state, size_t t_id) {
			return state < table.size() &&
				true == IN_MAP(table[state], t_id);
		}

		// returns the action id and true if the action is reduce
		std::pair<size_t, bool>& find (size_t state, size_t t_id);
	
		// TEST
		#ifndef _TERMINAL_
		void print (void) {
			size_t state = 0;
			for (std::unordered_map<size_t, std::pair<size_t, bool> > mtable : table) {
				std::cout << "state " << state++ << ":\n";
				for (auto mpair : mtable) {
					std::cout << "token<" << get(mpair.first) << ">(";
					if (mpair.second.second) {
						std::cout << "REDUCE";
					} else {
						std::cout << "SHIFT";
					}
					std::cout << mpair.second.first << ")\n";
				}
			}
		}
		#endif
};

class lexer {
	protected:
		std::list<token*> toks;

	public:
		virtual ~lexer (void) { for (token* t : toks) { delete t; } }

		virtual void lex (std::istream&) = 0;
		virtual bool empty (void) const = 0;
		virtual token* pop (void) = 0;
};

class parser {
	protected:
		lexer* lex = NULL;
		std::list<token*> sr_symsout;
		std::stack<size_t> sr_stack;
		size_t term;
	
		sr_table* table = NULL;
		
		virtual token* reduce (size_t reduce_id) = 0;
	
	public:
		const static size_t NULL_RESERVE = 0;
	
		// denotes the terminating token value
		// must be unique from tokens
		void set_term (size_t terminator) {
			term = terminator;
		}
	
		virtual ~parser (void) { if (NULL != lex) delete lex; }
		
		// lex and parse input stream
		virtual void parse (std::istream& content);
		
		// get the top node of abstract tree
		virtual token* top (void) {
			return sr_symsout.front();
		}
};

#endif /* __SYM_STRUCT__ */