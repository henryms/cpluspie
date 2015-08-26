#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>

#include "debug.h"

#include "symbol/keysym.h"
#include "symbol.h"
#include "library/wdslib.h"

#include "mstream.h"
#include "matrix.h"
#include "position.h"
#include "sens/retina.h"
#include "sens/spirit.h"
#include "global.h"
#include "font.h"
#include "texel.h"
#include "shared.h"
#include "layen.h"

using namespace std;

bool shared_c::test_uniqueness()
{
	vector<texel_c*> v;
	for ( auto x : char_map ){
		for ( auto y : x.second ) {
			for ( auto z : v ) {
				if ( z == y.second ) {
					return false;
				}
				v.push_back(y.second);
			}
		}
	}
	return true;
}

texel_c* shared_c::clone ( uint32_t chr, size_t size )
{
	if ( chr == '\t' ){
		return new texel_tab_c();
	}
	map<size_t,texel_c*>* pm;
	auto it = char_map.find(chr);
	if (it != char_map.end()){
		auto it2 = it->second.find(size );
		if ( it2 != it->second.end()){
			it2->second->reference++;
			return it2->second;
		}
		pm = &it->second;
	}
	else {
		auto q = char_map.insert(make_pair(chr,map<size_t,texel_c*>()));
		pm = &(q.first->second);
	}
	texel_c* p;
	switch ( chr ){
		case '\n':
			p = new texel_newline_c ();
			break;
		default :
			p = new texel_char_c ( chr );
	}
	pm->insert (make_pair(size, p));
	p->reference++;
	return p;
}

graphies_c* shared_c::clone_row_graphies(size_t size)
{
	auto it = row_graphies_map.find(size);
	if(it == row_graphies_map.end()) {
		auto p = row_graphies_map.insert(make_pair(size,graphies_c()));
		p.first->second.reference++;
		return &(p.first->second);
	}
	it->second.reference++;
	return &it->second;
}

shared_c::shared_c()
{
}

shared_c::~shared_c()
{
	for ( auto x : char_map ) {
		for ( auto y : x.second ){
			assert (y.second->dec_reference() == 0 );
			delete y.second;
		}
	}
	for ( auto &x : row_graphies_map ) {
//		assert (x.second.dec_reference() == 0 );
	}
}