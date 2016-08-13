//
//  static_lexer.hpp
//
//  description: tokenizes rules specified by metarules.txt
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#pragma once
#ifndef __STATIC_LEXER__
#define __STATIC_LEXER__

#include <iostream>
#include <list>
#include <set>
#include <map>
#include <queue>
#include <cstdio>
#include <stdexcept>
#include <sstream> 
#include <string>

using namespace std;

#define IN_MAP(tmap, t) tmap.end() != tmap.find(t)

enum TOKEN {
	VAR,
	STRING,
	ALTER,
	CONCAT,
	ASSIGN,
	REGEX,
	LPAREN,
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
	GROUP,
	OPTION,
	REP0,
	REP1,
	ALTEROP,
	ACCEPT,
};

class token {
	TOKEN tval;
public:
	string content;
	token (void) {}
	token (TOKEN tval) : tval(tval) {}
	token (TOKEN tval, string content) : tval(tval), content(content) {}
	virtual ~token (void) {}
	virtual void set_tok (TOKEN tk) { tval = tk; }
	virtual TOKEN get_tok (void) { return tval; }
};


class adhoc_lexer {
	private:
		// super state is a collection of tokens sharing similar behavior
		// e.g.: LSIDE represent tokens LPAREN, LSB, and LCB
		struct superstate {
			map<TOKEN, superstate*> nexts;
			set<TOKEN> CONCAT_trans;
			TOKEN tval;
			superstate (TOKEN tval) : tval(tval) {}
		};

		// state graph matching metarule specifications
		static superstate* entry;
		static const TOKEN valid_exit = END;

		// construct the state graph when necessary
		static void adhoc_construct (void);

		// traverse the graph while appending to toks
		void step_state (TOKEN, string&);

		// throw errors
		void error_throw (string);

		// token collection
		list<token*> toks;

		size_t lineno = 0;
		size_t colno = 0;
		superstate* step;

	public:
		// destroy static superstate graph
		// WARNING: very thread unsafe
		static void adhoc_delete (void);

		adhoc_lexer (void) { adhoc_construct(); }
		~adhoc_lexer (void) { for (token* t : toks) { delete t; } }

		// lex given input filename
		void lex (string);

		// getter
		bool empty (void) const { return toks.empty(); }

		// get next token
		token* pop (void) { 
			token* t = toks.front(); 
			toks.pop_front(); 
			return t; 
		}
};

#endif /* __STATIC_LEXER__ */