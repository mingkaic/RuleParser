//
//  utils.hpp
//
//  description: string, map, and set utility functions
//
//  Created by Ming Kai Chen on 2016-07-29.
//  Copyright (c) 2016 Ming Kai Chen. All rights reserved.
//

#include <string>
#include <algorithm>
#include <unordered_set>

#pragma once
#ifndef __UTILS__
#define __UTILS__

// find in map or set
#define IN_MAP(tmap, t) !(tmap.end() == tmap.find(t))

// trim from start
std::string& ltrim(std::string& s);

// trim from end
std::string& rtrim(std::string& s);

// trim from both ends
std::string& trim(std::string& s);

// insert character pad in front of all string specs characters found in string src
std::string pre_pad (std::string src, char pad, std::string specs);

#endif /* __UTILS__ */