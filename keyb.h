#ifndef KEYBOARD_H
#define KEYBOARD_H

#define LL_KA_0   0x0
#define LL_KA_LS  0x1 //key Alteration left shift
#define LL_KA_RS  0x2 //right shift
#define LL_KA_LC  0x4 //left control
#define LL_KA_RC  0x8  
#define LL_KA_LM  0x10  //left menu
#define LL_KA_RM	0x20 // right menu
#define LL_KA_SP	 0x40// space 
#define LL_KA_S		LL_KA_LS+LL_KA_RS
#define LL_KA_C		LL_KA_LC+LL_KA_RC
#define LL_KA_M		LL_KA_LM+LL_KA_RM

using namespace std;

class ctn_c
{
	public:
	ctn_c ( int num ){us = num; is_text = false;};
	ctn_c ( const char* str ){s = str; is_text = true; };
	unsigned short us;
	string s;
	bool is_text;
};

class key_c{
	public :
	key_c ();
	string dls;
	string uls;
	string drs;
	string urs;
};

class keyboard_layout_c
{
	public:
	keyboard_layout_c ( initializer_list < ctn_c > list );
	map < unsigned short, key_c > layout;
	void dump ();
};

class keyboard_c
{
public:
	keyboard_c ();
	virtual ~keyboard_c ();    
	list<pair<unsigned short, bool>> stroke_list;
	keyboard_layout_c* keyboard_layout;
	bool on_key ( int iMsg, unsigned short stroke );
	bool is_pressed ();
	bool is_pressed ( unsigned short );
	int get_controls ( bool couple = true );
	bool is_representable ( unsigned short stroke );
	string get_char ();
	void set_layout ( string );
	string country;
	unsigned long get_stroke ();
	string get_stroke_semantic ();
	string diastroke;
	string charakter;
	bool caps_lock;
};

#endif //KEYB_H
