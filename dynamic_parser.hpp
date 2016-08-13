//
//  dynamic_parser.hpp
//
//  description: parses rules specified by input rule
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#pragma once
#ifndef __DYN_PARSER__
#define __DYN_PARSER__

#include "dynamic_lexer.hpp"
#include <vector>

struct production {
	vector<size_t> line;
	production (void) {}
	production (const production& src) {
		concat(src);
	}
	production& operator = (const production& src) {
		if (&src != this) {
			line.clear();
			concat(src);
		}
		return *this;
	}
	production (const production& left, const production& right) {
		concat(left);
		concat(right);
	}
	void concat (const production& src) {
		if (false == src.line.empty()) {
			line.insert(line.end(), src.line.begin(), src.line.end());
		}
	}
};

class super_parser {
	private:
		super_lexer* lex = NULL;
	
	public:
		super_parser (super_lexer* lex) : lex(lex) {}

		void parse (istream& content);
};

#endif /* __DYN_PARSER__ */