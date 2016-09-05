//
//  flexc_syms.hpp
//
//  description: defines the abstract tree nodes
//
//  Created by Ming Kai Chen on 2016-08-23.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#pragma once
#ifndef __FLEXC_SYMS__
#define __FLEXC_SYMS__

#include "../symbols.hpp"

namespace flexc {

class dyn_node : public token {
	private:
		std::vector<token*> subs;
	
	public:
		dyn_node (size_t id) { this->t_id = id; }
		void push_back (token* rep) { subs.push_back(rep); }
};

}

#endif /* __FLEXC_SYMS__ */