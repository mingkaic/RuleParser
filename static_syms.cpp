//
//  static_syms.cpp
//
//  description: defines the abstract tree nodes
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#include "static_syms.hpp"

#ifdef __STATIC_SYMS__

#include <functional>
#include <memory> // todo: add shared_ptrs

// for display only
map<size_t, string> displayer;
#define MAX_RETRY 50

static super_lexer* lex = NULL;

enum GROUP_OPT {
	NONE=0,
	_GROUP,
	_OPTIONAL,
	_REPOPT,
	_REP,
};

static string convert_esc (string s) {
	string res = "";
	for (char c : s) {
		switch (c) {
			case '\\':
			case '[':
			case ']':
			case '{':
			case '}':
			case '(':
			case ')':
			case '.':
			case '*':
			case '?':
			case '+':
			case '^':
			case '|':
				res.push_back('\\');
				break;
		}
		res.push_back(c);
	}
	return res;
}

class symbol_rep {
	protected:
		GROUP_OPT option = NONE;
		signed symb_id;
		static map<size_t, symbol_rep*> id_reps;

		void rep_wrap(string& wrap) {
			switch (option) {
				case _GROUP:
					wrap = "(?:" + wrap + ")";
					break;
				case _OPTIONAL:
					wrap = "[" + wrap + "]";
					break;
				case _REPOPT:
					wrap = "(?:" + wrap + ")*";
					break;
				case _REP:
					wrap = "(?:" + wrap + ")+";
					break;
				case NONE:
				default:
					break;
			}
		}

	public:
		virtual ~symbol_rep (void) {}
		signed get_id (void) const { return symb_id; }
		void set_group (GROUP_OPT opt) {
			if (option < opt) {
				option = opt;
			} 
		}

		// represent as REGEX
		virtual string flatten (void) = 0;
		// represent as CFG
		virtual void flatten (vector<production*>&) = 0;
		// a symbol is first order if it holds no recursions in its definition
		// that is a first order symbol can be represented in regex
		virtual bool first_order (void) = 0;
		virtual bool first_order (size_t) { return true; }
		virtual bool iter (function<bool(symbol_rep*)> notused) { return true; }
};

class list_rep : public symbol_rep {
	private:
		// save time: run once
		bool* norec = NULL;

	protected:
		list<symbol_rep*> subsyms;
		list_rep (signed id) {
			symb_id = id;
			subsyms.clear();
		}

	public:
		static symbol_rep* get_list (void) { return new list_rep(-1); }
		static symbol_rep* get_list (string key) { 
			size_t h_key = hash<string>{}(key);
			map<size_t, symbol_rep*>::iterator s_it = symbol_rep::id_reps.find(h_key);
			if (symbol_rep::id_reps.end() == s_it) {
				symbol_rep::id_reps[h_key] = new list_rep((signed) h_key);
			}
			return symbol_rep::id_reps[h_key];
		}

		virtual ~list_rep (void) {
			if (norec) {
				delete norec;
			} 
		}

		void push_back (symbol_rep* rep) { subsyms.push_back(rep); }

		virtual bool first_order (void) {
			if (NULL == norec) {
				norec = new bool;
				*norec = true;
				for (symbol_rep* s_sym : subsyms) {
					*norec = *norec && s_sym->first_order(symb_id);
				}
			}

			return *norec;
		}

		virtual bool first_order (size_t id) {
			if (symb_id == id && id != -1) {
				return false;
			}
			
			list<symbol_rep*> sym_q = subsyms;
			set<symbol_rep*> sym_set;
			sym_set.insert(subsyms.begin(), subsyms.end());
			symbol_rep* buffer = NULL;

			function<bool(symbol_rep*)> check_procedure = 
				[this, &sym_set, &sym_q](symbol_rep* sym){
					bool retval = true;
					set<symbol_rep*>::iterator it = sym_set.find(sym);
					if (sym_set.end() == it) {
						sym_set.insert(sym);
						sym_q.push_back(sym);
					} 
					if (sym->get_id() != -1 && 
						false == sym->first_order()) {
						retval = false;
					}
					return retval;
				};
			
			while (false == sym_q.empty()) {
				buffer = sym_q.front();
				sym_q.pop_front();
				if ((-1 != id && buffer->get_id() == id) ||
					false == buffer->iter(check_procedure)) {
					return false;
				}
			}

			return true;
		}

		virtual string flatten (void) {
			assert (first_order());
			stringstream ss;
			for (symbol_rep* reps : subsyms) {
				ss << reps->flatten();
			}
			string base = ss.str();
			rep_wrap(base);
			return base;
		}

		virtual void flatten (vector<production*>& stmt) { // concatenation
			if (0 == stmt.size()) {
				stmt.push_back(new production());
			}
			for (symbol_rep* subs : subsyms) {
				signed r_ids = subs->get_id();
				if (r_ids == -1) { // anonymous token or grouper (list or alt)
					subs->flatten(stmt);
				} else { // tokens or variables
					for (production* prod : stmt) {
						prod->line.push_back(r_ids);
					}
				}
			}
		}

		virtual bool iter (function<bool(symbol_rep*)> f_iter) {
			for (symbol_rep* rep : subsyms) {
				if (false == f_iter(rep)) {
					return false;
				}
			}
			return true;
		}
};

class alter_rep : public list_rep {
	private:
		alter_rep (signed id) : list_rep(id) {}

	public:
		static symbol_rep* get_alt (void) { return new alter_rep(-1); }
		static symbol_rep* get_alt (string key) { 
			size_t h_key = hash<string>{}(key);
			if (symbol_rep::id_reps.end() == symbol_rep::id_reps.find(h_key)) {
				symbol_rep::id_reps[h_key] = new alter_rep((signed) h_key);
			}
			return symbol_rep::id_reps[h_key];
		}

		virtual string flatten (void) {
			assert (first_order());
			if (this->subsyms.empty()) return "";
			stringstream ss;
			list<symbol_rep*>::iterator it = this->subsyms.begin();
			ss << (*it)->flatten();
			for (it++; it != this->subsyms.end(); it++) {
				ss << "|" << (*it)->flatten();
			}
			string base = ss.str();
			rep_wrap(base);
			return base;
		}

		virtual void flatten (vector<production*>& stmt) { // alteration / fork
			if (0 == stmt.size()) {
				stmt.push_back(new production());
			}
			vector<production*> extras;
			for (production* prod : stmt) {
				for (symbol_rep* subs : this->subsyms) {
					signed r_ids = subs->get_id();
					if (r_ids == -1) { // anonymous token or grouper (list or alt)
						vector<production*> inner;
						subs->flatten(inner);
						for (production* follow : inner) {
							extras.push_back(new production(*prod, *follow));
							delete follow;
						}
					} else { // tokens or variables
						production* cpy = new production(*prod);
						cpy->line.push_back(r_ids);
						extras.push_back(cpy);
					}
				}
				delete prod;
			}
			stmt.clear();
			stmt.insert(stmt.end(), extras.begin(), extras.end());
		}
};

class reg_rep : public symbol_rep {
	private:
		string rep; // implicit capture at conversion to regex

		reg_rep (void) {
			symb_id = -1;
		}

	public:
		static symbol_rep* get_reg (void) { return new reg_rep(); }

		void set_rep (string rep) { this->rep = rep; }

		virtual bool first_order (void) {
			return true;
		}

		virtual string flatten (void) {
			string base = rep;
			rep_wrap(base);
			return base;
		}

		virtual void flatten (vector<production*>& stmt) {
			// create anonymous tokens
			assert (NULL != lex);
			size_t t_id = rand() * rand();
			// anomaly check
			for (size_t retry = 0; 
				retry < MAX_RETRY && lex->tok_exist(t_id); 
				retry++) {
				t_id = rand() * rand();
			}
			lex->new_tok(t_id, rep);
			for (production* prod : stmt) {
				prod->line.push_back(t_id);
			}
			cout << "anonymous created " << t_id << endl;
		}
};

map<size_t, symbol_rep*> symbol_rep::id_reps;

symbol_rep* variable::rule_gen (void) {
	return alter_rep::get_alt(content);
}

symbol_rep* string_sym::rule_gen (void) {
	symbol_rep* base = reg_rep::get_reg();
	reg_rep* reg = dynamic_cast<reg_rep*>(base);
	if (NULL == reg) {
		throw runtime_error("strings presented as list");
	}
	reg->set_rep(convert_esc(content));
	return reg;
}

symbol_rep* regex_sym::rule_gen (void) {
	symbol_rep* base = reg_rep::get_reg();
	reg_rep* reg = dynamic_cast<reg_rep*>(base);
	if (NULL == reg) {
		throw runtime_error("regex presented as list");
	}
	reg->set_rep(content);
	return reg;
}

symbol_rep* group_sym::rule_gen (void) {
	symbol_rep* base = center->rule_gen();
	base->set_group(_GROUP);
	return base;
}

symbol_rep* opt_sym::rule_gen (void) {
	symbol_rep* base = center->rule_gen();
	base->set_group(_OPTIONAL);
	return base;
}

symbol_rep* rep0_sym::rule_gen (void) {
	symbol_rep* base = center->rule_gen();
	base->set_group(_REPOPT);
	return base;
}

symbol_rep* rep1_sym::rule_gen (void) {
	symbol_rep* base = center->rule_gen();
	base->set_group(_REP);
	return base;
}

symbol_rep* concatenation::rule_gen (void) {
	list_rep* lptr = dynamic_cast<list_rep*>(
		list_rep::get_list());
	if (NULL == lptr) {
		throw runtime_error("list generation failed");
	}
	lptr->push_back(lside->rule_gen());
	lptr->push_back(rside->rule_gen());
	return lptr;
}

symbol_rep* alternation::rule_gen (void) {
	list_rep* lptr = dynamic_cast<list_rep*>(
		alter_rep::get_alt());
	if (NULL == lptr) {
		throw runtime_error("list generation failed");
	}
	lptr->push_back(lside->rule_gen());
	lptr->push_back(rside->rule_gen());
	return lptr;
}

symbol_rep* statement::rule_gen (void) {
	symbol_rep* base = alter_rep::get_alt(lside);
	alter_rep* lptr = dynamic_cast<alter_rep*>(base);
	if (NULL == lptr) {
		throw runtime_error("statement presented as non-list");
	}
	lptr->push_back(rside->rule_gen());
	displayer[lptr->get_id()] = lside;
	return lptr;
}

symbol_rep* program::rule_gen (void) {
	const size_t start_id = hash<string>{}("start");
	set<symbol_rep*> symb_set;
	for (statement* stmt : statements) {
		symb_set.insert(stmt->rule_gen());
	}
	cout << "preliminary success\n";
	// identify rule's tokens and extract their lexeme in ECMAScript regex form
	set<symbol_rep*> tokens;
	set<symbol_rep*> grammars;
	for (symbol_rep* assignment : symb_set) {
		assert (tokens.end() == tokens.find(assignment));
		if (assignment->first_order()) {
			tokens.insert(assignment);
		} else {
			grammars.insert(assignment);
		}
	}
	cout << "grammar-token separation success\n";
	cout << "tokens/grammar ratio: " << tokens.size() << "/" << grammars.size() << endl;
	lex = new super_lexer();
	cout << "TOKENS:\n============\n";
	for (symbol_rep* toks : tokens) {
		cout << displayer[toks->get_id()] << endl;
		lex->new_tok(toks->get_id(), toks->flatten());
	}
	cout << "lexer constructed\n";
	parse = new super_parser(lex);
	cout << "GRAMMARS:\n============\n";
	map<size_t, vector<production*> >  g_prods;
	for (symbol_rep* gram : grammars) {
		gram->flatten(g_prods[gram->get_id()]);
	}
	cout << "parser constructed\n";
	cout << "STATEMENTS:\n============\n";
	for (auto g_pair : g_prods) {
		for (production* prod : g_pair.second) {
			cout << displayer[g_pair.first] << "->";
			for (size_t id : prod->line) {
				cout << displayer[id] << " ";
			}
			cout << endl;
		}
	}

	return NULL;
}

#endif /* __STATIC_SYMS__ */