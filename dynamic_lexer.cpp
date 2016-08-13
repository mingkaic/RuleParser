//
//  dynamic_lexer.cpp
//
//  description: tokenizes rules specified by input rule
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#include "dynamic_lexer.hpp"

#ifdef __DYN_LEXER__

#include <regex>

void super_lexer::lex (istream& content) {
	string line_str;
	size_t line_no = 1;
	while (getline(content, line_str)) {
		size_t col_no = 1;
		while (false == line_str.empty()) {
			size_t match_id;
			string long_match;
			string residual = line_str;
			smatch sm;
			for (auto t_pair : toks) {
				regex e("("+t_pair.second+")");
				if (true == regex_search(line_str, sm, e) &&
					0 == sm.position()) {
					string buffer = sm[1];
					if (buffer.size() > long_match.size()) {
						match_id = t_pair.first;
						long_match = buffer;
						residual = sm.suffix();
					}
				}
			}
			if (long_match.size() == 0) {
				stringstream ss;
				ss << "invalid dynamic token line ";
				ss << line_no << " col " << col_no << endl;
				throw runtime_error(ss.str());
			}
			col_no += long_match.size();
			t_que.push_back(new tok_pair(match_id, long_match));
			line_str = residual;
		}
		// add newline token if necessary
		line_no++;
	}
}

#endif /* __DYN_LEXER__ */