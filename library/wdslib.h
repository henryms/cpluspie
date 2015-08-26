#ifndef CLIB_H
#define CLIB_H


#include <array>

using namespace std;
string get_lambs_path();
vector<string> environment();
void set_lambs_path( std::string path);
void scale ( int h0, int wpc,  int lpc, int rpc, int tpc, int bpc, string& s );
void binary_to_text(vector<char>& bytes, stringstream& text);
string utf32_to_utf8(uint32_t);
void to_utf8(string& src, string& des, int code);
uint64_t die();


class gray_8_rgb_c
{
public:
	gray_8_rgb_c();
	array<uint32_t,256> table;
};

class pixel_c
{
public:
	pixel_c ();
	pixel_c ( const pixel_c&);
	pixel_c ( int x, int y, uint32_t color );
	int x;
	int y;
	uint32_t color;
};

#endif

