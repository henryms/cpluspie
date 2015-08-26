#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_set>
#include <functional>
#include <cctype>

//#include </home/henry/usr/include/c++/4.9.1/regex>
#include <regex>

#include "standard.h"

#include "symbol/keysym.h"

#include "library/wdslib.h"

#include "gate.h"
#include "matrix.h"
#include "position.h"
#include "mstream.h"
#include "sens/retina.h"
#include "sens/spirit.h"
#include "message.h"

#include "echo.h"

#include "global.h"

#include "object.h"
#include "line.h"
#include "spline.h"
#include "keyb.h"
#include "wr/font.h"
#include "wr/texel.h"
#include "wr/shared.h"
#include "wr/layen.h"
#include  "edit.h"
#include "regexp.h"


using namespace std;

reg_exp_c::reg_exp_c ():
	look_before ( 0 ),
	look_after ( 0 ),
	mode ( 0 ),
	is_search_raw ( false ),
	is_search_excluded ( false ),
	is_search_end_raw ( false ),
	is_search_end_excluded ( false )
{
}

reg_exp_c::~reg_exp_c ()
{
	cout << "regexp destruction\n";
}

reg_exp_c::reg_exp_c (const reg_exp_c& rx ) 
{
	look_before = rx.look_before;
	look_after = rx.look_after;
	mode = rx.mode;
	is_search_raw = rx.is_search_raw;
	is_search_excluded = rx.is_search_excluded;
	is_search_end_raw = rx.is_search_end_raw;
	is_search_end_excluded = rx.is_search_end_excluded;
	search = rx.search;
	search_end = rx.search_end;
	replace = rx.replace;
}

void reg_exp_c::dump ( stringstream& ss )
{
	ss << hex << mode << endl
	<< "search=============" << endl 
	<< search << endl 
	<< "search_end==========" << endl
	<< search_end << endl
	<< "replace=============" << endl
	<< replace << endl;
	echo << ss.str ();
}

void reg_exp_c::command ( stringstream& ss )
{
	string s1;
	ss >> s1;
	stringstream ss1;
	if ( s1 == "-d" ) {
		dump ( ss1 );
	}
	if ( s1 == "-s" ) {
		ss >> search;
	}
	else if ( s1 == "-sf" ) {
		ifstream f ( s1 );
		stringstream ss;
		ss << f.rdbuf ();
		search = ss.str (); 
		echo << search << endl;
	}
	else if ( s1 == "-se" ) {
		ss >> search_end;
	}
	else if ( s1 == "-sef" ) {
		ifstream f ( s1 );
		stringstream ss;
		ss << f.rdbuf ();
		search_end = ss.str (); 
		echo << search_end << endl;
	}
	else if ( s1 == "-r" ) {
		ss >> replace;
	}
	else if ( s1 == "-rf" ) {
		ifstream f ( s1 );
		stringstream ss;
		ss << f.rdbuf ();
		replace = ss.str (); 
		echo << replace << endl;
	}		
	else if (s1 == "-m" ) {
		int i;
		ss >> hex >> i;
		mode = i;
		is_search_raw = i & 0x8 ? 1:0;
		is_search_excluded = i & 0x4 ? 1:0;
		is_search_end_raw = i & 0x2 ? 1:0;
		is_search_end_excluded = i & 0x1 ? 1:0;	
	}
	else if (s1 == "-la" ) {
		ss >> look_before;
		ss >> look_after;		
		echo << "regex lookaround:" << look_before << ":" << look_after << endl;
	}
}


/*
{ XK_a, "xk_a" },
{XK_b, "xk_b"},
{XK_c, "xk_c"},
{0x30, XK_a},
{0x30, XK_b},
{0x2e, XK_c},
{0x20, XK_d},
{0x12, XK_e},
{0x21, XK_f},
{0x22, XK_g},
{0x23, XK_h},
{0x17, XK_i},
{0x24, XK_j},
{0x25, XK_k},
{0x26, XK_l},
{0x32, XK_m},
{0x31, XK_n},
{0x18, XK_o},
{0x19, XK_p},
{0x10, XK_q},
{0x13, XK_r},
{0x1f, XK_s},
{0x14, XK_t},
{0x16, XK_u},
{0x2f, XK_v},
{0x11, XK_w},
{0x2d, XK_x},
{0x15, XK_y},
{0x2c, XK_z},
{0x02, XK_1},
{0x03, XK_2},
{0x04, XK_3},
{0x05, XK_4},
{0x06, XK_5},
{0x07, XK_6},
{0x08, XK_7},
{0x09, XK_8},
{0x0a, XK_9},
{XK_0, "xk_0"},
{0x0c, XK_minus},
{0x0d, XK_equal},
{0x29, XK_grave},
{0x27, XK_semicolon},
{0x28, XK_apostrophe},
{0x2b, XK_backslash},
{0x33, XK_comma},
{0x34, XK_period},
{0x35, XK_slash},
{0x56, XK_less},
{0x1a, XK_bracketleft},
{0x1b, XK_bracketright},
{0x01, XK_Escape},
{0x39, XK_space},
{0x1c, XK_Return},
{0x0f, XK_Tab},
{0x0e, XK_BackSpace},
{0x2a, XK_Shift_L},
{0x36, XK_Shift_R},
{0x3a,XK_Caps_Lock},
{0x1d, XK_Control_L},
{0x61, XK_Control_R},
{0x38, XK_Alt_L},
{0x64, XK_Alt_R},
{0x3b, XK_F1},
{0x3c, XK_F2},
{0x3d, XK_F3},
{0x3e, XK_F4},
{0x3f, XK_F5},
{0x40, XK_F6},
{0x41, XK_F7},
{0x42, XK_F8},
{0x43, XK_F9},
{0x44, XK_F10},
{0x57, XK_F11},
{0x58, XK_F12}
};
*/
#include <regex>

bool reg_exp_c::match ( string& s, string pat ) 
{
/*
#ifdef BOOST
	boost::regex rpat{ pat };
	return boost::regex_match ( s, rpat );
#else
*/
	regex rpat{pat };
	match_results<string::iterator> smr;
	return regex_match ( s, rpat );
/*
#endif
*/
}

bool reg_exp_c::ssearch ( string& s, string pat ) 
{
/*
#ifdef BOOST
	boost::regex rpat{ pat };
	return boost::regex_search( s, rpat );
#else
*/
	regex rpat{ pat };
	return regex_search( s, rpat );
/*
#endif
*/
}


void reg_exp_c::change ( string & in ,string & out ) 
{
	return;
	stringstream ss { in };
	out = in.substr ( 6 );
	in = out;
	for ( int z = 0; z < in.size (); z++ ) in [ z ] = tolower ( in [ z ]  );
	out = " " + out + ", " + "\"" + in + "\" ";
	return;
}

bool reg_exp_c::find ( 
	string& text, 
	size_t& position, 
	size_t position_end,
	bool is_raw, 
	string& search,
	string& found ) 
{
	if ( ! is_raw ) {
		if ( position == string::npos || position >= text.size () ) {
			found = "";
			return false;
		}
		regex rgx ( search );
		match_results < string::iterator > smr;
		string::iterator its = text.begin ();
		advance ( its , position );
		if ( regex_search ( its, text.end (), smr, rgx ) ) {
			position += smr.position ();
			found = smr.str ();
			echo << "found:" << found;
			return true;
		}
		position = string::npos;
		return false;
	}
	else {
		size_t pm = position;
		for (; position < text.size (); position ++ ) {
			if ( position + search.size () > text.size () ) {
				found = "";
				return false;
			}
			for ( int c = 0; c < search.size (); c++ ) {
				if ( text [ position + c ] != search [ c ] ) {
					break;
				}
				if ( c + 1 == search.size () ) {
					found = search;
					return true;
				}
			}
		}
		found = "";
	}
}

bool reg_exp_c::find ( 
	string& text, 
	string::iterator& it, 
	string::iterator& ite,
	string& search,
	string& found ) 
{

	regex rgx ( search );
	match_results < string::iterator > smr;
	if ( regex_search ( it, ite, smr, rgx ) ) {
		found = smr.str ();
		echo << "found:" << found;
		return true;
	}
	return false;
}

//43  49949034
bool reg_exp_c::find_section ( string& s, size_t& position, string& found )
{

	if ( search_end == "" ) {
		return find ( s, position, 0, is_search_raw, search, found ) ; 
	}

	if ( find ( s, position, 0, is_search_raw, search, found ) ) {
		if ( search_end == "" ) {
			return true;
		}
		size_t pos = position;
		if ( find ( s, pos, 0, is_search_end_raw, search_end, found ) ) {
			found = s.substr ( position, pos - position + found.size () );
			return true;
		}
	} 	
	found = "";
	return false;
}

bool reg_exp_c::find_section ( 
	string& s, size_t& position,
	bool is_sb_raw, bool is_sb_excluded, string &search_begin,
	bool is_se_raw, bool is_se_excluded, string &search_end,
	string& found )
{

	if ( search_end == "" ) {
		return find ( s, position, 0, is_sb_raw, search_begin, found ) ; 
	}

	if ( find ( s, position, 0, is_sb_raw, search_begin, found ) ) {
		if ( search_end == "" ) {
			return true;
		}
		size_t pos = position;
		if ( find ( s, pos, 0, is_sb_raw, search_end, found ) ) {
			found = s.substr ( position, pos - position + found.size () );
			return true;
		}
	} 	
	found = "";
	return false;
}
/*
{ XK_a, "xk_a" },
{ XK_b, "xk_b" },
{ XK_c, "xk_c" },
{ XK_d, "xk_d" },
{ XK_e, "xk_e" },
{ XK_f, "xk_f" },
{ XK_g, "xk_g" },
{ XK_h, "xk_h" },
{ XK_i, "xk_i" },
{ XK_j, "xk_j" },
{ XK_k, "xk_k" },
{ XK_l, "xk_l" },
{ XK_m, "xk_m" },
{ XK_n, "xk_n" },
{ XK_o, "xk_o" },
{ XK_p, "xk_p" },
{ XK_q, "xk_q" },
{ XK_r, "xk_r" },
{ XK_s, "xk_s" },
{ XK_t, "xk_t" },
{ XK_u, "xk_u" },
{ XK_v, "xk_v" },
{ XK_w, "xk_w" },
{ XK_x, "xk_x" },
{ XK_y, "xk_y" },
{ XK_z, "xk_z" },
{ XK_1, "xk_1" },
{ XK_2, "xk_2" },
{ XK_3, "xk_3" },
{ XK_4, "xk_4" },
{ XK_5, "xk_5" },
{ XK_6, "xk_6" },
{ XK_7, "xk_7" },
{ XK_8, "xk_8" },
{ XK_9, "xk_9" },
{ XK_0, "xk_0" },
{ XK_minus, "xk_minus" },
{ XK_equal, "xk_equal" },
{ XK_grave, "xk_grave" },
{ XK_semicolon, "xk_semicolon" },
{ XK_apostrophe, "xk_apostrophe" },
{ XK_backslash, "xk_backslash" },
{ XK_comma, "xk_comma" },
{ XK_period, "xk_period" },
{ XK_slash, "xk_slash" },
{ XK_less, "xk_less" },
{ XK_bracketleft, "xk_bracketleft" },
{ XK_bracketright, "xk_bracketright" },
{ XK_Escape, "xk_escape" },
{ XK_space, "xk_space" },
{ XK_Return, "xk_return" },
{ XK_Tab, "xk_tab" },
{ XK_BackSpace, "xk_backspace" },
{ XK_Shift_L, "xk_shift_l" },
{ XK_Shift_R, "xk_shift_r" },
{0x3a,XK_Caps_Lock},
{ XK_Control_L, "xk_control_l" },
{ XK_Control_R, "xk_control_r" },
{ XK_Alt_L, "xk_alt_l" },
{ XK_Alt_R, "xk_alt_r" },
{ XK_F1, "xk_f1" },
{ XK_F2, "xk_f2" },
{ XK_F3, "xk_f3" },
{ XK_F4, "xk_f4" },
{ XK_F5, "xk_f5" },
{ XK_F6, "xk_f6" },
{ XK_F7, "xk_f7" },
{ XK_F8, "xk_f8" },
{ XK_F9, "xk_f9" },
{ XK_F10, "xk_f10" },
{ XK_F11, "xk_f11" },
{ XK_F12, "xk_f12" }
};

map<int, int> linux_x11={ 
{XK_a, xk_a},
{XK_b, xk_b},
{XK_c, xk_c},
{XK_d, xk_d},
{XK_e, xk_e},
{XK_f, xk_f},
{XK_g, xk_g},
{XK_h, xk_h},
{XK_i, xk_i},
{XK_j, xk_j},
{XK_k, xk_k},
{XK_l, xk_l},
{XK_m, xk_m},
{XK_n, xk_n},
{XK_o, xk_o},
{XK_p, xk_p},
{XK_q, xk_q},
{XK_r, xk_r},
{0x1f, XK_s},
{0x14, XK_t},
{0x16, XK_u},
{0x2f, XK_v},
{0x11, XK_w},
{0x2d, XK_x},
{0x15, XK_y},
{0x2c, XK_z},
{0x02, XK_1},
{0x03, XK_2},
{0x04, XK_3},
{0x05, XK_4},
{0x06, XK_5},
{0x07, XK_6},
{0x08, XK_7},
{0x09, XK_8},
{0x0a, XK_9},
{0x0b, XK_0},
{0x0c, XK_minus},
{0x0d, XK_equal},
{0x29, XK_grave},
{0x27, XK_semicolon},
{0x28, XK_apostrophe},
{0x2b, XK_backslash},
{0x33, XK_comma},
{0x34, XK_period},
{0x35, XK_slash},
{0x56, XK_less},
{0x1a, XK_bracketleft},
{0x1b, XK_bracketright},
{0x01, XK_Escape},
{0x39, XK_space},
{0x1c, XK_Return},
{0x0f, XK_Tab},
{0x0e, XK_BackSpace},
{0x2a, XK_Shift_L},
{0x36, XK_Shift_R},
{0x3a, XK_Caps_Lock},
{0x1d, XK_Control_L},
{0x61, XK_Control_R},
{0x38, XK_Alt_L},
{0x64, XK_Alt_R},
{0x3b, XK_F1},
{0x3c, XK_F2},
{0x3d, XK_F3},
{0x3e, XK_F4},
{0x3f, XK_F5},
{0x40, XK_F6},
{0x41, XK_F7},
{0x42, XK_F8},
{0x43, XK_F9},
{0x44, XK_F10},
{0x57, XK_F11},
{0x58, XK_F12}
};
*/
