//
//  flexc_parser.hpp
//
//  description: parses rules specified by input rule
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#pragma once
#ifndef __FLEXC_PARSER__
#define __FLEXC_PARSER__

#include <algorithm>

#include <vector>
#include <stack>

#include "flexc_lexer.hpp"
#include "flexc_syms.hpp"
#include "../symbols.hpp"

namespace flexc {

struct production {
	std::vector<size_t> line;
    
	production (void) {}

	production (const production& src) {
		concat(src);
	}
    
	production (const production& left, const production& right) {
		concat(left);
		concat(right);
	}
    
	production& operator = (const production& src) {
		if (&src != this) {
			line.clear();
			concat(src);
		}
		return *this;
	}
    
	void concat (const production& src) {
		if (false == src.line.empty()) {
			line.insert(line.end(), src.line.begin(), src.line.end());
		}
	}
    
    std::unordered_set<size_t> follow (size_t id, std::unordered_map<size_t, std::unordered_set<size_t> >& firsts) {
		std::unordered_set<size_t> followset;
		size_t t_id;
        auto it = line.begin();
        while (line.end() != it) {
			t_id = *it;
			it++;
            if (id == t_id) {
				if (line.end() == it) {
					t_id = parser::NULL_RESERVE;
					followset.insert(t_id);
				} else if (true == IN_MAP(firsts, *it)) {
					followset.insert(firsts[*it].begin(), firsts[*it].end());
				} else {
					followset.insert(*it);
				}
            }
        }
		return followset;
    }
};

class super_parser : public parser {
	private:
		// products of build_table
		std::unordered_map<production*, size_t> prod2id;
		std::vector<production*> allprods;

		// used in build_table
        class closure;
    
        void build_table (size_t start_id,
			std::unordered_map<size_t, std::vector<production*> >& prods,
			std::unordered_set<size_t>& all_ids);
	
		virtual token* reduce (size_t reduce_id);
	
	public:
		super_parser (super_lexer* lex,
            size_t start_id,
            std::unordered_map<size_t, std::vector<production*> >&  prods,
			std::unordered_set<size_t>& all_ids) {
			this->lex = lex;
            build_table(start_id, prods, all_ids);
			term = parser::NULL_RESERVE;
			#ifndef _TERMINAL_
			table->print();
			#endif
        }
		
		virtual ~super_parser (void) {
			for (production* p : allprods) {
				delete p;
			}
		};
};

}

#endif /* __FLEXC_PARSER__ */