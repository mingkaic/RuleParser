//
//  static_parser.cpp
//
//  description: parses rules specified by metarules.txt
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#include "static_parser.hpp"

#ifdef __STATIC_PARSER__

vector<map<TOKEN, adhoc_parser::action*>*>* adhoc_parser::sr_table = NULL;
string adhoc_parser::tablecsv_name = "mr_sr_table.csv";
map<string, TOKEN> adhoc_parser::str_tokmapper = {
	{"Regex", REGEX},
	{"String", STRING},
	{"Var", VAR},
	{"Lparen", LPAREN},
	{"Rparen", RPAREN},
	{"Lsb", LSB},
	{"Rsb", RSB},
	{"Lcb", LCB},
	{"Rcb", RCB},
	{"Rcbplus", RCBPLUS},
	{"Alter", ALTER},
	{"Concat", CONCAT},
	{"End", END},
	{"Assign", ASSIGN},
	{"$", TERM$},
	{"Program", PROGRAM},
	{"Statement", STMT},
	{"Rvalue", RVALUE},
	{"Mid", MID},
	{"Base", BASE},
	{"Group", GROUP},
	{"Option", OPTION},
	{"Rep0", REP0},
	{"Rep1", REP1},
	{"Alterop", ALTEROP},
};

void adhoc_parser::build_table (void) {
	if (NULL == sr_table) {
		sr_table = new vector<map<TOKEN, adhoc_parser::action*>*>;
		ifstream csvfs(tablecsv_name);
        if (false == csvfs.good()) {
            throw runtime_error("shift reduce table csv file not found");
        }

		vector<TOKEN> tok_order;
		string block;
		char c = 'a';
		while (csvfs.good() && c != '\n') {
			c = csvfs.get();
			if (c == ',' || c == '\n') {
				tok_order.push_back(str_tokmapper[block]);
				block.clear();
			} else {
				block.push_back(c);
			}
		}
		map<TOKEN, action*>* state = NULL;
		size_t i = 0;
		while (csvfs.good() && (c = csvfs.get()) != EOF) {
			if (NULL == state) {
				state = new map<TOKEN, action*>();
			}
			if ((c == ',' || c == '\n')) {
				if (false == block.empty()) {
					int num_comp = stoi(block.substr(isalpha(block[0])), nullptr);
					TOKEN tk = tok_order[i];
					(*state)[tk] = new action(num_comp, block[0] == 'R');
				}
				i++;
				block.clear();
			} else {
				block.push_back(c);
			}
			if (c == '\n') {
				i = 0;
				sr_table->push_back(state);
				state = NULL;
			}
		}
		if (NULL != state) {
			sr_table->push_back(state);
		}
	}
}

// execute reduce functions
token* adhoc_parser::reduce (size_t reduce_id) {
	token* buffer;
	token* buffer2;
	token* ret;

	switch (reduce_id) {
		// ACCEPT!
		case 0:
			ret = NULL;
			break;
		// PROGRAM -> PROGRAM STMT
		case 1:
			buffer2 = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			dynamic_cast<program*>(buffer)->add(
				dynamic_cast<statement*>(buffer2));
			ret = buffer;
			break;
		// PROGRAM -> STMT
		case 2:
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			ret = new program(
				dynamic_cast<statement*>(buffer));
			break;
		// STMT -> VAR ASSIGN RVALUE END
		case 3:
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer2 = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			ret = new statement(buffer->content, 
				dynamic_cast<abs_node*>(buffer2));
			delete buffer;
			break;
		// RVALUE -> MID
		case 4:
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer->set_tok(RVALUE);
			ret = buffer;
			break;
		// RVALUE -> RVALUE CONCAT MID
		case 5:
			buffer2 = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			ret = new concatenation(
				dynamic_cast<abs_node*>(buffer), 
				dynamic_cast<abs_node*>(buffer2));
			break;
		// ALTEROP -> MID ALTER 
		case 6:
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			ret = new alternation(dynamic_cast<abs_node*>(buffer));
			break;
		// MID -> BASE
		case 7:
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer->set_tok(MID);
			ret = buffer;
			break;
		// MID -> ALTEROP BASE
		case 8:
			buffer2 = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			dynamic_cast<alternation*>(buffer)->assignright(
				dynamic_cast<abs_node*>(buffer2));
			ret = buffer;
			break;
		// BASE -> STRING
		case 9:
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			ret = new string_sym(buffer->content);
			delete buffer;
			break;
		// BASE -> VAR
		case 10:
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			ret = new variable(buffer->content);
			delete buffer;
			break;
		// BASE -> REGEX LPAREN STRING RPAREN
		case 11:
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			ret = new regex_sym(buffer->content);
			delete buffer;
			break;
		// BASE -> GROUP
		case 12:
		// BASE -> OPTION
		case 13:
		// BASE -> REP0
		case 14:
		// BASE -> REP1
		case 15:
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer->set_tok(BASE);
			ret = buffer;
			break;
		// GROUP -> LPAREN RVALUE RPAREN
		case 16:
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			ret = new group_sym(
				dynamic_cast<abs_node*>(buffer));
			break;
		// OPTION -> LSB RVALUE RSB
		case 17:
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			ret = new opt_sym(
				dynamic_cast<abs_node*>(buffer));
			break;
		// REP0 -> LCB RVALUE RCB
		case 18:
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			ret = new rep0_sym(
				dynamic_cast<abs_node*>(buffer));
			break;
		// REP1 -> LCB RVALUE RCBPLUS
		case 19:
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			buffer = sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			delete sr_symsout.back();
			sr_symsout.pop_back();
			sr_stack.pop();
			ret = new rep1_sym(
				dynamic_cast<abs_node*>(buffer));
			break;
		default:
			throw runtime_error("bad reduction");
			break;
	}
	return ret;
}

void adhoc_parser::parse (string fname) {
	list<token*> sr_symsin;
	lexer.lex(fname);
	token* tok;
	action* act;
	map<TOKEN, action*>* cur_state;
	sr_stack.push(0);
	TOKEN evaltok;

	// keep reducing until the top node
	do {
		if (false == sr_symsin.empty()) {
			tok = sr_symsin.front();
			sr_symsin.pop_front();
		} else if (false == lexer.empty()) {
			tok = lexer.pop();
		} else {
			tok = new token(TERM$);
		}
		evaltok = tok->get_tok();
		cur_state = (*sr_table)[sr_stack.top()];
		if (IN_MAP((*cur_state), evaltok)) {
			act = (*cur_state)[evaltok];
			if (act->reduce) {
				token* red = reduce(act->id);
				if (NULL != red) {
					sr_symsin.push_back(red); // store the ret token/node
				} else if (lexer.empty() && sr_symsin.empty()) {
					evaltok = ACCEPT;
				}
				if (TERM$ != evaltok) {
					sr_symsin.push_back(tok); // also store the lookahead
				} else {
					delete tok;
				}
			} else {
				sr_symsout.push_back(tok);
				sr_stack.push(act->id);
			}
		} else {
			throw runtime_error("invalid syntax: bad grammar");
		}
	} while (ACCEPT != evaltok);
}

#endif /* __STATIC_PARSER__ */