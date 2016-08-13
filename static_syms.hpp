//
//  static_syms.hpp
//
//  description: defines the abstract tree nodes
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#pragma once
#ifndef __STATIC_SYMS__
#define __STATIC_SYMS__

#include "static_lexer.hpp"
#include "dynamic_parser.hpp"

class symbol_rep;

class abs_node : public token {
	public:
		virtual ~abs_node (void) {}
		virtual symbol_rep* rule_gen (void) = 0;
};

class variable : public abs_node {
	private:
		string content;
		TOKEN tval = BASE;
	public:
		variable (string content) : content(content) {}
		virtual TOKEN get_tok (void) { return tval; }
		virtual void set_tok (TOKEN tk) { tval = tk; }
		virtual symbol_rep* rule_gen();
};

class string_sym : public abs_node {
	private:
		string content;
		TOKEN tval = BASE;
	public:
		string_sym (string content) : content(content) {}
		virtual TOKEN get_tok (void) { return tval; }
		virtual void set_tok (TOKEN tk) { tval = tk; }
		virtual symbol_rep* rule_gen();
};

class regex_sym : public abs_node {
	private:
		string content;
		TOKEN tval = BASE;
	public:
		regex_sym (string content) : content(content) {}
		virtual TOKEN get_tok (void) { return tval; }
		virtual void set_tok (TOKEN tk) { tval = tk; }
		virtual symbol_rep* rule_gen();
};

class group_sym : public abs_node {
	private:
		abs_node* center;
		TOKEN tval = GROUP;
	public:
		group_sym (abs_node* center) : center(center) {}
		~group_sym (void) {
			delete center;
		}
		virtual TOKEN get_tok (void) { return tval; }
		virtual void set_tok (TOKEN tk) { tval = tk; }
		virtual symbol_rep* rule_gen();
};

class opt_sym : public abs_node {
	private:
		abs_node* center;
		TOKEN tval = OPTION;
	public:
		opt_sym (abs_node* center) : center(center) {}
		~opt_sym (void) {
			delete center;
		}
		virtual TOKEN get_tok (void) { return tval; }
		virtual void set_tok (TOKEN tk) { tval = tk; }
		virtual symbol_rep* rule_gen();
};

class rep0_sym : public abs_node {
	private:
		abs_node* center;
		TOKEN tval = REP0;
	public:
		rep0_sym (abs_node* center) : center(center) {}
		~rep0_sym (void) {
			delete center;
		}
		virtual TOKEN get_tok (void) { return tval; }
		virtual void set_tok (TOKEN tk) { tval = tk; }
		virtual symbol_rep* rule_gen();
};

class rep1_sym : public abs_node {
	private:
		abs_node* center;
		TOKEN tval = REP1;
	public:
		rep1_sym (abs_node* center) : center(center) {}
		~rep1_sym (void) {
			delete center;
		}
		virtual TOKEN get_tok (void) { return tval; }
		virtual void set_tok (TOKEN tk) { tval = tk; }
		virtual symbol_rep* rule_gen();
};

class concatenation : public abs_node {
	private:
		abs_node* lside;
		abs_node* rside;
		TOKEN tval = RVALUE;
	public:
		concatenation (abs_node* lside, abs_node* rside) 
			: lside(lside), rside(rside) {}
		~concatenation() {
			delete lside;
			delete rside;
		}
		virtual TOKEN get_tok (void) { return tval; }
		virtual void set_tok (TOKEN tk) { tval = tk; }
		virtual symbol_rep* rule_gen();
};

class alternation : public abs_node {
	private:
		abs_node* lside;
		abs_node* rside;
		TOKEN tval = ALTEROP;
	public:
		alternation (abs_node* lside) 
			: lside(lside) {}
		~alternation (void) {
			delete lside;
			delete rside;
		}
		void assignright (abs_node* rside) {
			this->rside = rside;
			tval = MID;
		}
		virtual TOKEN get_tok (void) { return tval; }
		virtual void set_tok (TOKEN tk) { tval = tk; }
		virtual symbol_rep* rule_gen();
};

class statement : public abs_node {
	private:
		string lside;
		abs_node* rside;
		TOKEN tval = STMT;
	public:
		statement (string left, abs_node* right)
			: lside(left), rside(right) {}
		~statement (void) {
			delete rside;
		}
		virtual TOKEN get_tok (void) { return tval; }
		virtual void set_tok (TOKEN tk) { tval = tk; }
		virtual symbol_rep* rule_gen();
};

class program : public abs_node {
	private:
		list<statement*> statements;
		TOKEN tval = PROGRAM;

	public:
		super_parser* parse = NULL;

		program (statement* s) {
			statements.push_back(s); 
		}
		~program () {
			for (statement* stmt : statements) {
				delete stmt;
			}
		}
		void add (statement* s) { statements.push_back(s); }
		virtual TOKEN get_tok (void) { return tval; }
		virtual void set_tok (TOKEN tk) { tval = tk; }
		virtual symbol_rep* rule_gen();
};

#endif /* __STATIC_SYMS__ */