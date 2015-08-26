#ifndef SHARED_H
#define SHARED_H

#include <unordered_map>
using namespace std;

class shared_c
{
public:
	shared_c();
	~shared_c();
	texel_c* clone(uint32_t, size_t);
	graphies_c* clone_row_graphies(size_t);
	unordered_map<uint32_t, map<size_t, texel_c*>> char_map;
	bool test_uniqueness();
	unordered_map<size_t, graphies_c> row_graphies_map;
	vector<texel_c*> unique_texels;
	vector<graphies_c*>unique_graphies;
};


#endif