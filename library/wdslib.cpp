#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <random>

using namespace std;

#include "wdslib.h"

string lambs_path;

void set_lambs_path(string path)
{
	lambs_path = path;
}


vector<string> environment()
{
	string s = get_lambs_path();
	if ( s == ""){
		return {};
	}
	s += "/config/environment";
	ifstream ifs{s};
	string l;
	vector<string>v;
	for(;getline(ifs, l);){
		v.push_back(l);
	}
	return v;
}

string get_lambs_path()
{
	if ( not lambs_path.empty()){
		return lambs_path;
	}
	char* pe=getenv("LL_PATH");
	if(pe==0){
		cout << "environment error" << endl;
		return {};
	}
	else {
		return {pe};
	}
}

gray_8_rgb_c::gray_8_rgb_c()
{
	unsigned char byte=0;
	uint32_t rgb;
	for (int c = 0; c < 256; c++){
		byte = ~c;
		rgb = 0;
		rgb = byte;
		rgb <<= 8;		
		rgb |=byte;
		rgb <<=8;
		rgb |= byte;
		table[c] = rgb;
	}
}

void binary_to_text(vector<char>& bytes, stringstream& ss)
{
	int limit = 10000;
	ss << std::hex;
	ss.fill('0');
	string s;
	int c = 0;
	for( auto x : bytes){
		if( c%8 == 0){
			if(c%16 == 0){
				if(c!=0){
					ss << "   " << s << '\n';	
					s = "";
				}
				ss.width(8);
				ss << c << "  " ;
				ss.width(2);
			}
			else{
				ss << " ";
				s += " ";
			}
		}	
		ss.width(2);
		ss.fill('0');
		ss << (unsigned)(unsigned char)x << " ";
		if ( 0x30 <= x && x <= 0x80){
			s += (char)x;
		}
		else{
			s += ".";
		}
		++c;	
		if(--limit == 0){
			break;
		}
	}
	int shift = 16 - c%16;
	if (shift > 0){
		shift *= 2;
		if (shift <=16){
			++shift;
		}
		for(;shift > 0; --shift){
			ss << " ";
		}
		ss << s;
	}
}

int text_to_binary(string &text, vector<char>& bytes)
{
	
} 

class utff{
	public:
	utff(unsigned char b1, unsigned char b2, unsigned char b3)
	{vuc.push_back(b1); vuc.push_back(b2); vuc.push_back(b3); cout << "con3\n";}
	utff(unsigned char b1, unsigned char b2)
	{vuc.push_back(b1); vuc.push_back(b2);};
	utff(){vuc.clear();};	
	vector<unsigned char> vuc{};
};

void to_utf8(string& src, string& des, int code)
{
	vector<utff> ar;
	for(int c=0; c < 256; ++c){
		ar.push_back(utff());	
		ar.back().vuc.clear();		
	}	
	ar[0x93]={0xe2, 0x80, 0x9c};	
	ar[0x94]={0xe2, 0x80, 0x9d};
	ar[0x97]={0xe2, 0x80, 0x94};
	ar[0xae]={0xc2, 0xae};
	ar[0xa9]={0xc2, 0xa9};
	ar[0x96]={0xe2, 0x80, 0x93};
	ar[0xc4]={0xc3, 0x84};
	ar[0xe9]={0xc3, 0xa9};
	cout << ar[0x93].vuc.size() << "...\n";
	des = "";
	int count = 0;
	for(auto x : src){	
		if(ar[(int)(unsigned char)x].vuc.size() == 0){
			if((x&0x80) == 0){
				des.push_back(x);	
			}
			else
				cout << hex << (int)(unsigned char) x << endl;
		}
		else{
			int c = 0;
//			cout << ar[(int)(unsigned char)x].vuc.size() << endl;
			for(; c < ar[(int)(unsigned char)x].vuc.size(); ++c){
				des.push_back(ar[(int)(unsigned char)x].vuc[c]);	
			}
			++count;
//			cout << hex << (int)(unsigned char)x << endl;
		}
	}

	/*
	for(auto x : src){	
		
		if((x & 0x80) == 0)
			des.push_back(x);	
		else{
			++count;
			cout << hex << (int)(unsigned char)x << endl;
		}
	}
	*/
	cout << "not utf8:" << count << '\n';	
}


void scale ( int h0, int wpc,  int lpc, int rpc, int tpc, int bpc, string& s ) 
{
//	echo << h0 << endl;
	int 
	h= h0*1000/ ( 1000 - tpc - bpc ),
	w = h*wpc/1000,
	l = w*lpc/1000,
	r = w*rpc/1000,
	t = h*tpc/1000,
	b = h*bpc/1000;
	stringstream ss;
	ss 
	<< "-scale-to " << h
//	<< "-scale-x-to " << w
//	<< " -scale-y-to " << h
	<< " -y " << t
	<< " -H " <<  h - b  - t
	<< " -x " << l
	<< " -W " << w - r - l ;
	s = ss.str ();
}

uint64_t die()
{
	static uint64_t c = 0;
	if ( c == 0 ){
		default_random_engine re {};
		uniform_int_distribution<>  one{1, 0xffffFFFFffffFFFF};
		c = one ( re );
	}
	return c++;
}

pixel_c::pixel_c ()
{
}

pixel_c::pixel_c ( const pixel_c& pixel )
{
	x = pixel.x;
	y = pixel.y;
	color = pixel.color;
}

pixel_c::pixel_c ( int _x, int _y, uint32_t _color ):
	x ( _x ),
	y ( _y ),
	color ( _color )
{
}

string utf32_to_utf8(uint32_t chr)
{
	std::string s;
	unsigned char ch;
	if(chr <= 0x7F){
		s+=chr;
		return s;
	}
	if(chr <= 0x7FF){
		ch=chr >> 6;
		ch=ch | 0xC0; 
		s+=ch;
		ch= chr & 0x3F;
		ch= ch | 0x80;
		s+=ch;
		return s;
	}
	if(chr <= 0xFFFF){
		ch=chr >> 12;
		ch=ch | 0xE0;
		s+=ch;
		ch= chr >> 6;
		ch= ch & 0x3F;
		ch = ch | 0x80;
		s+=ch;
		ch= chr & 0x3F;
		ch= ch | 0x80;
		s+=ch;
		return s;		
	}
	if(chr <= 0x1FFFFF){
	}
		return s;
}



