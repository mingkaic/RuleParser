//
//  static_lexer.cpp
//
//  description: tokenizes rules specified by metarules.txt
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#include "static_lexer.hpp"

#ifdef __STATIC_LEXER__

adhoc_lexer::superstate* adhoc_lexer::entry = NULL;

void adhoc_lexer::error_throw (string msg) {
	stringstream ss;
	ss << "invalid static syntax: " << msg << " line " << lineno << ", col " << colno;
	throw runtime_error(ss.str()); 
}

void adhoc_lexer::adhoc_construct (void) {
	if (NULL == entry) {
		entry = new superstate(VAR);

		// intermediates
		superstate* lside = new superstate(LSIDE);
		superstate* rside = new superstate(RSIDE);
		superstate* join = new superstate(ALTER);
		superstate* str = new superstate(STRING);
		superstate* assign = new superstate(ASSIGN);
		superstate* end = new superstate(END);

		entry->nexts[VAR] = lside->nexts[VAR] = 
		rside->nexts[VAR] = join->nexts[VAR] = 
		assign->nexts[VAR] = end->nexts[VAR] = 
		str->nexts[VAR] = entry;

		lside->nexts[LSIDE] = rside->nexts[LSIDE] = 
		str->nexts[LSIDE] = join->nexts[LSIDE] = 
		assign->nexts[LSIDE] = entry->nexts[LSIDE] = lside;

		entry->nexts[STRING] = lside->nexts[STRING] = 
		rside->nexts[STRING] = join->nexts[STRING] = 
		assign->nexts[STRING] = str->nexts[STRING] = str;

		rside->nexts[RSIDE] = str->nexts[RSIDE] = 
		entry->nexts[RSIDE] = rside;

		rside->nexts[ALTER] = str->nexts[ALTER] = 
		entry->nexts[ALTER] = join;

		rside->nexts[END] = str->nexts[END] = 
		entry->nexts[END] = end;

		entry->nexts[ASSIGN] = assign;

		// and space transition
		entry->CONCAT_trans.insert(VAR);
		entry->CONCAT_trans.insert(STRING);
		entry->CONCAT_trans.insert(LSIDE);
		str->CONCAT_trans.insert(VAR);
		str->CONCAT_trans.insert(STRING);
		str->CONCAT_trans.insert(LSIDE);
		rside->CONCAT_trans.insert(LSIDE);
		rside->CONCAT_trans.insert(STRING);
		rside->CONCAT_trans.insert(VAR);
	}
}

void adhoc_lexer::adhoc_delete (void) {
	set<superstate*> uniques;
	queue<superstate*> q;
	q.push(entry);
	for (superstate* buffer = entry; false == q.empty(); buffer = q.front()) {
		q.pop();
		if (uniques.end() == uniques.find(buffer)) {
			uniques.insert(buffer);
			for (auto next : buffer->nexts) {
				q.push(next.second);
			}
		}
	}
	for (auto pars : uniques) {
		delete pars;
	}
}

void adhoc_lexer::step_state (TOKEN check, string& buffer) {
	if (VAR == step->tval && 0 == buffer.compare("regex")) { // special case
		toks.push_back(new token(REGEX));
		if (LSIDE == check) {
			step = step->nexts[check];
		} else {
			error_throw("symbol regex not followed by (");
		}
	} else {
		if (VAR == step->tval || STRING == step->tval) {
			toks.push_back(new token(step->tval, buffer));
		}
		if (IN_MAP(step->nexts, check)) {
			if (IN_MAP(step->CONCAT_trans, check)) { // edge is and
				toks.push_back(new token(CONCAT));
			}
			step = step->nexts[check];
		} else {
			// took part of parser's job
			error_throw("bad symbol sequence");
		}
	}
	buffer.clear();
}

void adhoc_lexer::lex (string filename) {
	FILE* fp = fopen(filename.c_str(), "r");
    if (NULL == fp) {
        throw runtime_error("file not found\n");
    }
	step = entry;
	lineno = 1;
	colno = 0;
	bool spec_char = true;
	bool invar = true;
	bool instr = false;
	bool escape = false;
	char c;
	string s;

	while ((c = fgetc(fp)) != EOF) {
		colno++;
		spec_char = true;
		if ((STRING == step->tval) && instr) {
			switch (c) {
				case '"':
					if (escape) {
						escape = false;
						s.push_back(c);
					} else {
						instr = false;
					}
					break;
				case '\\':
					escape = !escape;
					s.push_back(c);
					break;
				case 'a':
				case 'b':
				case 't':
				case 'n':
				case 'v':
				case 'f':
				case 'r':
					escape = false;
				default:
					s.push_back(c);
					if (escape) {
						error_throw("bad escape character");
					}
			}
		} else {
			spec_char = true;
			switch (c) {
				case '\n':
					lineno++;
					colno = 0;
				case '\a':
				case '\b':
				case '\t':
				case '\v':
				case '\f':
				case '\r':
				case ' ':
					break;

				case '=':
					step_state(ASSIGN, s);
					toks.push_back(new token(ASSIGN, "="));
					break;

				case '|':
					step_state(ALTER, s);
					toks.push_back(new token(ALTER, "|"));
					break;

				case ';':
					step_state(END, s);
					toks.push_back(new token(END, ";"));
					break;

				// leftsides
				case '(':
					step_state(LSIDE, s);
					toks.push_back(new token(LPAREN, "("));
					break;
				case '[':
					step_state(LSIDE, s);
					toks.push_back(new token(LSB, "["));
					break;
				case '{':
					step_state(LSIDE, s);
					toks.push_back(new token(LCB, "{"));
					break;

				// rightsides
				case ')':
					step_state(RSIDE, s);
					toks.push_back(new token(RPAREN, ")"));
					break;
				case ']':
					step_state(RSIDE, s);
					toks.push_back(new token(RSB, "]"));
					break;
				case '}':
					step_state(RSIDE, s);
					toks.push_back(new token(RCB, "}"));
					break;
				case '+':
					if (RCB == toks.back()->get_tok()) {
						toks.back()->set_tok(RCBPLUS);
						toks.back()->content.push_back(c);
					} else {
						error_throw("symbol + not following }");
					}
					break;

				// strings
				case '"':
					instr = true;
					step_state(STRING, s);
					break;
				case '\\':
					error_throw("symbol \\ out of string");
				default:
					if (false == invar) {
						step_state(VAR, s);
					}
					invar = true;
					spec_char = false;
					s.push_back(c);
			}
			// delimited
			if (spec_char) {
				invar = false;
			}
		}
	}
	if (valid_exit != step->tval) {
		error_throw("missing final terminal symbol");
	}
}

#endif /* __STATIC_LEXER__ */