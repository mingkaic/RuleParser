//
//  adhoc_syms.hpp
//
//  description: defines the abstract tree nodes
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#pragma once
#ifndef __ADHOC_SYMS__
#define __ADHOC_SYMS__

#include "adhoc_lexer.hpp"
#include "../flexcs/flexc_parser.hpp"

namespace adhoc {

class symbol_rep;

class abs_node : public token {
	public:
		virtual ~abs_node (void) {}
		virtual symbol_rep* rule_gen (void) = 0;
};

class variable : public abs_node {
	public:
		variable (std::string content) {
			t_id = BASE;
			this->content = content;
		}
		virtual ~variable (void) {}
		virtual symbol_rep* rule_gen (void);
};

class string_sym : public abs_node {
	public:
		string_sym (std::string content) {
			t_id = BASE;
			this->content = content;
		}
		virtual ~string_sym (void) {}
		virtual symbol_rep* rule_gen (void);
};

class regex_sym : public abs_node {
	public:
		regex_sym (std::string content) {
			t_id = BASE;
			this->content = content;
		}
		virtual ~regex_sym (void) {}
		virtual symbol_rep* rule_gen (void);
};

class capture_sym : public abs_node {
	private:
		abs_node* center;
    
	public:
		capture_sym (abs_node* center) : center(center) { t_id = BASE; }
		virtual ~capture_sym (void) {
			delete center;
		}
		virtual symbol_rep* rule_gen (void);
};

class group_sym : public abs_node {
	private:
		abs_node* center;
    
	public:
		group_sym (abs_node* center) : center(center) { t_id = BASE; }
		virtual ~group_sym (void) {
			delete center;
		}
		virtual symbol_rep* rule_gen (void);
};

class opt_sym : public abs_node {
	private:
		abs_node* center;
    
	public:
		opt_sym (abs_node* center) : center(center) { t_id = BASE; }
		virtual ~opt_sym (void) {
			delete center;
		}
		virtual symbol_rep* rule_gen (void);
};

class rep0_sym : public abs_node {
	private:
		abs_node* center;
    
	public:
		rep0_sym (abs_node* center) : center(center) { t_id = BASE; }
		~rep0_sym (void) {
			delete center;
		}
		virtual symbol_rep* rule_gen (void);
};

class rep1_sym : public abs_node {
	private:
		abs_node* center;
    
	public:
		rep1_sym (abs_node* center) : center(center) { t_id = BASE; }
		virtual ~rep1_sym (void) {
			delete center;
		}
		virtual symbol_rep* rule_gen (void);
};

class concatenation : public abs_node {
	private:
		abs_node* lside;
		abs_node* rside;
    
	public:
		concatenation (abs_node* lside, abs_node* rside) 
			: lside(lside), rside(rside) { t_id = RVALUE; }
		virtual ~concatenation() {
			delete lside;
			delete rside;
		}
		virtual symbol_rep* rule_gen (void);
};

class alternation : public abs_node {
	private:
		abs_node* lside;
		abs_node* rside;
    
	public:
		alternation (abs_node* lside) 
			: lside(lside) { t_id = ALTEROP; }
		virtual ~alternation (void) {
			delete lside;
			delete rside;
		}
		void assignright (abs_node* rside) {
			this->rside = rside;
			t_id = MID;
		}
		virtual symbol_rep* rule_gen (void);
};

class statement : public abs_node {
	private:
		abs_node* rside;
    
	public:
		statement (std::string left, abs_node* right) : rside(right) {
			t_id = STMT;
			content = left;
		}
		virtual ~statement (void) {
			delete rside;
		}
		virtual symbol_rep* rule_gen (void);
};

class program : public abs_node {
	private:
		std::list<statement*> statements;

	public:
		flexc::super_parser* parser = NULL;
		program (void) { t_id = PROGRAM; }
		program (statement* s) {
			statements.push_back(s); 
		}
		virtual ~program () {
			for (statement* stmt : statements) {
				delete stmt;
			}
		}
		void add (statement* s) { statements.push_back(s); }
		virtual symbol_rep* rule_gen (void);
};

}

#endif /* __ADHOC_SYMS__ */