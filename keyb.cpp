#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <utility>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <string>
#include <sstream>
#include "mstream.h"
#include "symbol/keysym.h"
#include "library/wdslib.h"
#include "gate.h"
#include "matrix.h"
#include "position.h"
#include "global.h"
#include "sens/retina.h"
#include "sens/spirit.h"
#include "message.h"
#include "cash.h"
#include "object.h"
#include "line.h"

#include "spline.h"
#include "mouse.h"
#include "eyes.h"
#include "keyb.h"
#include "home.h"
#include "bookmarks.h"
#include "wr/texel.h"
#include "wr/shared.h"
#include "wr/layen.h"
#include "fox/edit.h"
#include "hand.h"
#include "lamb.h"
#include "image.h"
#include "land.h"
#include "wr/font.h"
#include "edit.h"

#include "matrix.h"

using namespace std;

key_c::key_c ()
{
}

keyboard_layout_c::keyboard_layout_c ( initializer_list<ctn_c> list ) 
{
	initializer_list<ctn_c>::iterator it;
	it = list.begin ();
	int c = 1;
	key_c key;
	unsigned short us;
	bool stroke = false;
	for (;it!= list.end ();it++){
		if (!it->is_text ){
			us = it->us;
			it++;
			stroke = true;
			break;
		}
	}
	for (;it!= list.end ();it++){
		if (!it->is_text ){
			layout[us]= key;
			us = it->us;
			c = 1;
			key.dls="";
			key.uls="";
			key.drs="";
			key.urs="";
		}
		else{
			switch ( c ){
				case 1:
					key.dls = it->s;
					break;
				case 2:
					key.uls = it->s;
					break;
				case 3:
					key.drs = it->s;
					break;
				case 4:
					key.urs = it->s;
					break;
			}
			c++;
		}
	}
	if ( stroke ){
		layout [us] = key;
	}
}
		
void keyboard_layout_c::dump ()
{
	pair<unsigned short, key_c> p;
	map<unsigned short, key_c>::iterator it;
	int c = 0;
	echo << "keyboard_layout_c dump" << std::endl;
	it = layout.begin ();
	for (;it!= layout.end ();it++){
		cout << it->first << ":"  << std::endl;
		cout << it->second.dls << endl;
		cout << it->second.uls << endl;
		cout << it->second.drs << endl;
		cout << it->second.urs << endl;
	}
}


map < unsigned long, string > xsy_sem = {
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
{ XK_Caps_Lock, "xk_caps_lock" },
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

//============================================
//					us keyboard scan
//
//	`	1	2	3	4	5	6	7	8	8	0	-	=	BKS
//
//		TAB 	Q	W	E	R	T	Y	U	I	O	P	[	]	\¹	   
//
//		CAP	A	S	D	F	G	H	J	K	L	;	'	\	RET
//
// SHIFT	<	Z	X	C	V	B	N	M	,	.	/	SHIFT
//	
// ¹ backslash position on some keyboard
//=============================================


keyboard_layout_c fr_kbl = {
	XK_q, "a", "A",
	XK_w, "z", "Z",
	XK_z, "w", "W",
	XK_m,",","?",
	XK_0, "à", "0", 
	XK_1, "&", "1",
	XK_2, "é","2",
	XK_3,"\\","3",
	XK_4, "'", "4",
	XK_5, "(","5",
	XK_6, "-", "6",
	XK_7, "è", "7",
	XK_8, "_", "8",
	XK_9, "ç","9",
	XK_minus,")","°",
	XK_equal,"=","+", 
	XK_grave, "²", "³",
	XK_bracketright,"$","£",
	XK_bracketleft,"^ ^ e ê E Ê o ô O Ô ", "¨ ¨ o ö O Ö e ë E Ë  u ü U Ü ",
	XK_semicolon,"m","M",
	XK_apostrophe,"ù","%",
	XK_backslash,"*","µ",
	XK_comma,";",".",
	XK_period,":","/",
	XK_slash,"!","§",
	XK_less,"<",">","|",
	XK_space," "," "
};

keyboard_layout_c de_kbl = {
	XK_0, "0", "=", "}",
	XK_1, "1","!",
	XK_2,"2","\"","²",
	XK_3,"3","§","³",
	XK_4,"4","$",
	XK_5,"5","%",
	XK_6,"6","&",
	XK_7,"7","/","{",
	XK_8,"8","(","[",
	XK_9,"9",")","]",
	XK_e,"e","E","€",
	XK_q,"q","Q","@",
	XK_y,"z","Z",
	XK_z,"y","Y",
	XK_m,"m","M","µ",
	XK_minus,"ß","?","\\",
	XK_equal,"´ ´ e é E É ","` ` e è E È a à A À", "¸ c ç C Ç",
	XK_grave,"^ ^ e ê E Ê a â A Â o ô O Ô","°",
	XK_bracketright,"+","*","~",
	XK_bracketleft,"ü","Ü",
	XK_semicolon,"ö","Ö",
	XK_apostrophe,"ä","Ä",
	XK_backslash,"'","#",
	XK_comma,",",";",
	XK_period,".",":",
	XK_slash,"-","_",
	XK_less,"<",">","|",
	XK_space," "," "
};

keyboard_layout_c custom_kbl = {
	XK_0,"0",")","}",
	XK_1,"1","!",
	XK_2,"2","@",
	XK_3,"3","#",
	XK_4,"4","$",
	XK_5,"5","%",
	XK_6,"6","^",
	XK_7,"7","&",
	XK_8,"8","*",
	XK_9,"9","(",
	XK_e,"e","E","€",
	XK_minus,"-","_",
	XK_equal,"=","+",
	XK_grave,"`","~",
	XK_bracketright,"]","}",
	XK_bracketleft,"[","{",
	XK_semicolon,";",":","","a ä A Ä o ö O Ö u ü U Ü s ß < « > »",
	XK_apostrophe,"'","\"",
	XK_backslash,"\\","|",
	XK_comma, "," ,"<", "", "i î I Î o ô O Ô c ç C Ç e ê E Ê w é W É r è R È a â A Â s à S À",
	XK_period,".",">",
	XK_slash,"/","?",
	XK_less,"<",">","|",
	XK_space," "," "
};

keyboard_layout_c us_kbl = {
	XK_0,"0",")","}",
	XK_1,"1","!",
	XK_2,"2","@",
	XK_3,"3","#",
	XK_4,"4","$",
	XK_5,"5","%",
	XK_6,"6","^",
	XK_7,"7","&",
	XK_8,"8","*",
	XK_9,"9","(",
	XK_e,"e","E","€",
	//{XK_e,"e","E","\xe2\x82\xac"},
	XK_minus,"-","_",
	XK_equal,"=","+",
	XK_grave,"`","~",
	XK_bracketright,"]","}",
	XK_bracketleft,"[","{",
	XK_semicolon,";",":","",
 	XK_apostrophe,"'","\"",
	XK_backslash,"\\","|",
	XK_comma, "," ,"<", "", 
	XK_period,".",">",
	XK_slash,"/","?",
	XK_less,"<",">","|",
	XK_space," "," ",
};

void keyboard_c::set_layout ( string  country_id ) 
{
	if ( country_id == "cu" ) {
		keyboard_layout = &custom_kbl;
	}	
	else if ( country_id == "us" ) {
		keyboard_layout = &us_kbl;
	}	
	else if ( country_id == "de" ) {
		keyboard_layout = &de_kbl;
	}
	else if (country_id == "fr" ) {
		keyboard_layout = &fr_kbl;
	}
	else {
		return;
	}
	country = country_id;
}

keyboard_c::keyboard_c ( void ): caps_lock ( 0 )
{
	keyboard_layout = & custom_kbl;
	country = "us";
}

keyboard_c::~keyboard_c ( void )
{
}

bool keyboard_c::is_pressed ()
{
	return stroke_list.front ().second;
}

bool keyboard_c::is_representable ( unsigned short stroke )
{
	if ((0x20 <= stroke ) && (stroke < 0xff ) && (stroke != 0x7f )){
		return true; 
	}
	return false;
}

unsigned long keyboard_c::get_stroke ()
{
	return stroke_list.front ().first;
}

string keyboard_c::get_char ()
{
	return charakter;
}

string keyboard_c::get_stroke_semantic ()
{
	unsigned long stroke = get_stroke ();
	return xsy_sem [ stroke ]; 
}

void dump_stroke_list ( list<pair<unsigned short, bool>>& sl )
{
	echo << "dump stroke list" << endl;
		for ( auto x : sl ) {
		cout << x.first << "  " << x.second << endl;
	}
	echo << "----------------" << endl;
}

bool keyboard_c::on_key ( int pressed, unsigned short stroke )
{
	if ((XK_Caps_Lock == stroke ) && pressed ){
		caps_lock = !caps_lock;
	}
	stroke_list.push_front ( make_pair ( stroke, pressed ));
	static int max_stroke = 10;
	static int max_duplicated = 5;
	auto it = stroke_list.begin ();
	unsigned short rs = 0;
	int c = 1;
	unsigned short st = stroke;
	if ( stroke== XK_Control_R && ! pressed ) {
		//cout << "control: " << pressed << '\n';
	}
	for (; it != stroke_list.end (); c++){
		if ( c == max_duplicated ) {
			rs = it -> first;
		}
		if ( c > max_duplicated ){
			if ( it -> first == rs ){
				it = stroke_list.erase ( it );
				continue;
			}
		}
		it++;
	}
	
	if ( stroke_list.size () > max_stroke ){
		stroke_list.pop_back ();
	}
//	cout << "keyb " << st << ":" << is_pressed ( XK_Control_R ) << '\n';	
	string s;
	auto it2 = keyboard_layout->layout.find ( stroke );
	if ( it2 != keyboard_layout->layout.end () ) {
		key_c key = it2->second;
		if ( is_pressed ( XK_Shift_L ) || is_pressed ( XK_Shift_R ) || caps_lock ) {
			s = key.uls;
		}
		else if ( is_pressed ( XK_Alt_R )){
			s = key.drs;
		}
		else if ( is_pressed ( XK_Control_L ) ) {
			s = key.urs;
		}
		else {
			s = key.dls;
		}
	}
	else{
		if ( !is_pressed ( XK_Alt_R ) && is_representable ( stroke ) ) {
			if ( is_pressed ( XK_Shift_L ) || is_pressed ( XK_Shift_R ) || caps_lock ){
				if ( islower ( stroke )){
				stroke -= 0x20;
				}
			}
			s += stroke;
		}
	}

	if ( diastroke != "" && s != "" && stroke_list.front ().second == true ) {
		auto it = diastroke.find ( s );
		if ( it != string::npos ) {
			it = diastroke.find ( " ", it );
			it ++;
			auto it2 = diastroke.find ( " ", it );
			s = diastroke.substr ( it, it2-it );	
		}
		else {
			s = "";
		}
		diastroke = "";
	}
	else {
		if ( s != " " && s.find ( " " ) != string::npos ){
			diastroke = s;
			s = "";
			stroke_list.front ().first = 0;
		}
	}
	charakter = s; 
	return true;
}

bool keyboard_c::is_pressed ( unsigned short stroke )
{
	for ( auto& x : stroke_list ) {
		if ( x.first == stroke ) {
			if ( x.second == true ) {
				return true;
			}
			return false;
		 }
	}
	return false;	
}

int keyboard_c::get_controls ( bool coupled )
{
	unsigned char mask = 0;
	if ( is_pressed ( XK_Control_L )){
		mask |= LL_KA_LC;
	}
	if ( is_pressed ( XK_Control_R )){
		mask |= LL_KA_RC;
	}
	if ( is_pressed ( XK_Alt_L )){
		mask |= LL_KA_LM;
	}
	return mask;
}	

