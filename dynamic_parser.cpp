//
//  dynamic_parser.cpp
//
//  description: parses rules specified by input rule
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#include "dynamic_parser.hpp"

#ifdef __DYN_PARSER__

void super_parser::parse(istream& content) {
	lex->lex(content);

	while (false == lex->empty()) {
		tok_pair* tp = lex->pop();
		cout << tp->t_id << " " << tp->t_str << endl;
	}
}

#endif /* __DYN_PARSER__ */