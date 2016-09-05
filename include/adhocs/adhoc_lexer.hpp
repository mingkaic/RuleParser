//
//  adhoc_lexer.hpp
//
//  description: tokenizes rules specified by metarules.txt
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#pragma once
#ifndef __ADHOC_LEXER__
#define __ADHOC_LEXER__

#include <queue>
#include <cstdio>
#include <stdexcept>
#include <sstream> 
#include <string>

#include "../symbols.hpp"

namespace adhoc {

enum TOKEN {
	VAR,
	STRING,
	ALTER,
	CONCAT,
	ASSIGN,
	REGEX,
	LPAREN,
	LPAREN_NULL,
	RPAREN,
	LSB,
	RSB,
	LCB,
	RCB,
	RCBPLUS,
	END,
	// unique super states
	RSIDE, // {lparen, lsb, lcb}
	LSIDE, // {rparen, rsb, rcb, rcbplus}
	// grammar states
	TERM$,
	PROGRAM,
	STMT,
	RVALUE,
	MID,
	BASE,
	CAPTURE,
	GROUP,
	OPTION,
	REP0,
	REP1,
	ALTEROP,
	ACCEPT,
};

class adhoc_lexer : public lexer {
	private:
		// super state is a collection of tokens sharing similar behavior
		// e.g.: LSIDE represent tokens LPAREN, LSB, and LCB
		struct superstate {
			std::unordered_map<size_t, superstate*> nexts;
			std::unordered_set<size_t> CONCAT_trans;
			TOKEN tval;
			superstate (TOKEN tval) : tval(tval) {}
		};

		// state graph matching metarule specifications
		static superstate* entry;
		static const TOKEN valid_exit = END;

		// construct the state graph when necessary
		static void adhoc_construct (void);

		// traverse the graph while appending to toks
		void step_state (TOKEN, std::string&);

		// throw errors
		void error_throw (std::string);

		size_t lineno = 0;
		size_t colno = 0;
		superstate* step;

	public:
		// destroy static superstate graph
		// WARNING: very thread unsafe
		static void adhoc_delete (void);

		adhoc_lexer (void) { adhoc_construct(); }
		virtual ~adhoc_lexer (void) {}

		// lex given input filename
		void lex (std::istream&);

		// getter
		bool empty (void) const { return toks.empty(); }

		// get next token
		token* pop (void) { 
			token* t = toks.front(); 
			toks.pop_front(); 
			return t; 
		}
};

}

#endif /* __ADHOC_LEXER__ */