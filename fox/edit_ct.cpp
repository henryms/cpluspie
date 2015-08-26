#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <cmath>

#include "standard.h"

#include "symbol/keysym.h"
#include "symbol.h"

#include "library/wdslib.h"
#include "gate.h"

#include "matrix.h"
#include "position.h"
#include "mstream.h"
#include "sens/retina.h"
#include "sens/spirit.h"

#include "message.h"
#include "cash.h"

#include "global.h"



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
#include "wr/texel.h"
#include "wr/shared.h"
#include "keyb.h"
#include "edit.h"
#include "home.h"
#include "make.h"
#include "comtab4.h"
#include "fox/edit_ct.h"

extern land_c land;
extern lamb_c lamb;


void comtab_editor_c::construct ( )
{
	push_vector_c<command_editor2_c>  v;
	
	v << new editor_inserting_overwriting_c ( "editor inserting_overwriting" );
	v << new editor_collapsing_c ( "editor collapsing" );
	v << new editor_marking_c ( "editor marking" );
	v << new editor_moving2_c ( "editor moving" );
	v << new editor_deleting_c ( "editor deleting" );
	v << new editor_scrolling_c ( "editor scrolling" );	
	v << new editor_change_mode_c ( "editor change_mode" );	
	v << new editor_runing_c ( "editor runing" );	
	v << new editor_editing_c ( "editor editing" );
	
	set (ct, v.lst, msc );
}

void comtab_editor_c::destruct ( ) 
{
	comtab_c::destruct ( msc );
}


bool comtab_editor_c::find ( editor_c* o, int mode, int control, unsigned long v, string c, string stroke_semantic )
{
	return finder.find ( *this, o, mode, control, v, c, stroke_semantic );
}

bool finder_c::find ( comtab_editor_c& ce, editor_c*o,  int mode, int control, unsigned long stroke, string c, string stroke_semantic )
{
	if ( o == 0 ) {
		echo << "parser find object = 0" << endl;
		return false;
	}
	static vector< string > ca { 2 };
	auto  fz = [&] ( ) { 	for ( auto &x : ca ) { x = "";} };

	string s, scs, sct;
	if ( mode == COMMAND ) {
		s = "c";
	}
	if ( mode == INSERT ) {
		s = "i";
	}
	if ( mode == VISUAL ) {
		s = "v";
	}
	if ( control == 0 ) {
		s += " nm";
	}
	if ( control == LL_KA_LC ) {
	
		s += " cl";
	}
	if ( control == LL_KA_RC ) {
		s += " cr";
	}
	 sct = s + " " + stroke_semantic;
	auto p = ce.msc.find ( sct ); 
	if ( p != ce.msc.end() ) {
//		echo << "found" << endl; 
		int res = p ->second->run ( o );
//		echo << "results: " << res << endl;
		fz ();
		return true;
	}
	if ( c == "" ) 
		return false;
	for ( int i = ca.size () - 1 ; i > 0  ; i-- ) {
		ca [ i ] = ca [ i -1 ] + c; 		
	}
	ca [ 0 ] = c;
	
	for ( int i  = ca.size () - 1 ; i >= 0; i--){
		if ( ca [ i ] != "" ) {
			sct = s + " '" + ca [ i ] + "'";
			p = ce.msc.find ( sct ); 
			if ( p != ce.msc.end() ) {
//				echo << "found double" << endl; 
				int res = p ->second->run ( o );
//				echo << "results: " << res << endl;
				fz ();
				return true;
			}
		}
	}
	return false;
}


command_editor2_c* editor_inserting_overwriting_c::create ()
{
	return new editor_inserting_overwriting_c ();
}

int editor_inserting_overwriting_c::run(editor_c* o) 
{  
	o->inserting_overwriting ( para1, para2 );
}

void editor_inserting_overwriting_c::  get_semantic ( string* s )
{
	*s = semantic;
}

bool editor_inserting_overwriting_c:: set_parameter ( string s )
{
	stringstream ss { s };
//	echo << "set para: " << s << endl;
	ss >> s;
	if ( s == "one_character" ) {
		para1 = 2;
	}
	else if ( s == "next_character" ) {
		para1 = 3;
	}
	else if ( s == "one_character_past_line_end" ) {
		para1 = 4;
	}
	else {
		para1 = 1;
	}
	para2 = 0;
}


command_editor2_c* editor_collapsing_c::create ()
{
	return new editor_collapsing_c ();
}

int editor_collapsing_c::run(editor_c* o) 
{  
	o->collapsing ( para1, para2 );
	return 0;
}

void editor_collapsing_c::  get_semantic ( string* s )
{
	*s = semantic;
}

bool editor_collapsing_c:: set_parameter ( string s )
{
	stringstream ss { s };
//	echo << "set para: " << s << endl;
	para1 = 0;
	para2 = true;
}


command_editor2_c* editor_marking_c::create ()
{
	return new editor_marking_c ();
}

int editor_marking_c::run(editor_c* o) 
{  
	o->marking ( para1, para2 );
	return 0;
}

void editor_marking_c::  get_semantic ( string* s )
{
	*s = semantic;
}

bool editor_marking_c:: set_parameter ( string s )
{
	stringstream ss { s };
//	echo << "set para: " << s << endl;
	ss >> s;
	if ( s == "go_to_mark" ) {
		para1 = GO_TO_MARK;
	}
	else  {
		para1 = stoi ( s );
	}
	ss >> para2;
}


command_editor2_c* editor_moving2_c::create ()
{
	return new editor_moving2_c ();
}

int editor_moving2_c::run(editor_c* o) 
{  
	o->moving ( para1, para2 );
	return 0;
}

void editor_moving2_c::  get_semantic ( string* s )
{
	*s = semantic;
}

bool editor_moving2_c:: set_parameter ( string s )
{
	stringstream ss { s };
//	echo << "set para: " << s << endl;
	ss >> s;
	if ( s == "left" ) {
		para1 = DEC_COLUMN;
	}
	else if ( s == "right" ) {
		para1 = INC_COLUMN;
	}
	else if ( s == "up" ) {
		para1 = DEC_ROW;
	}
	else if ( s == "down" ) {
		para1 = INC_ROW;
	}
	else if ( s == "next_word_begin" ){
		para1 = NEXT_WORD_BEG;
	}
	else if ( s == "previous_word_begin" ) {
		para1 = 	PREV_WORD_BEG;
	}
	else if ( s== "next_word_end" ) {
		para1 = NEXT_WORD_END;
	}
	else if ( s == "line_begin" ) {
		para1 = LINE_BEG;
	}
	else if ( s== "line_end" ) {
		para1 = LINE_END;
	}
	else if ( s== "line_last_character" ) {
		para1 = LINE_LAST_CHARACTER;
	}
	else if ( s == "new_line_up" ) {
		para1 = NEW_LINE_U;
	}
	else if ( s == "new_line_down" ) {
		para1 =NEW_LINE_D;
	}
	ss >> s;
	if ( s == "select" ) {
		para2 = 1;
	}
	else {
		para2 = 0;
	}
}


command_editor2_c* editor_deleting_c::create ()
{
	return new editor_deleting_c ();
}

int editor_deleting_c::run(editor_c* o) 
{  
	o->deleting ( para1, para2 );
}

void editor_deleting_c::  get_semantic ( string* s )
{
	*s = semantic;
}

bool editor_deleting_c:: set_parameter ( string s )
{
	stringstream ss { s };
//	echo << "set para: " << s << endl;
	ss >> s;
	if ( s == "character" ) {
		para1 = CHARACTER;
	}
	else if ( s == "space" ) {
		para1 = SPACE;
	}
	else if ( s == "tabulator" ) {
		para1 = TAB;
	}
	para2 = 1;	
}


command_editor2_c* editor_scrolling_c::create ()
{
	return new editor_scrolling_c ();
}

int editor_scrolling_c::run(editor_c* o) 
{  
	o->scrolling ( para1, para2 );
}

void editor_scrolling_c::  get_semantic ( string* s )
{
	*s = semantic;
}

bool editor_scrolling_c:: set_parameter ( string s )
{
	stringstream ss { s };
//	echo << "set para: " << s << endl;
	ss >> s;
	if ( s == "page_up" ) {
		para1 = PAGE_U;
	}
	if ( s == "page_down" ) {
		para1 = PAGE_D;
	}
	if ( s == "home" ) {
		para1 = HOME;
	}
	if ( s == "end" ) {
		para1 = END;
	}
	ss >> s;
	if ( s == "select" ) {
		para2 = true;
	}
	else {
		para2 = false;
	}
}


command_editor2_c* editor_change_mode_c::create ()
{
	return new editor_change_mode_c ();
}

int editor_change_mode_c::run(editor_c* o) 
{  
	o->change_mode ( para1, para2 );
}

void editor_change_mode_c::  get_semantic ( string* s )
{
	*s = semantic;
}

bool editor_change_mode_c:: set_parameter ( string s )
{
	stringstream ss { s };
//	echo << "set para: " << s << endl;
	ss >> s;
	if ( s == "insert" ) {
		para2 = INSERT;
	}
	else if ( s== "visual" ) {
		para2 = VISUAL;
	}
	else if ( s == "command" ) {
		para2 = COMMAND;
	}
	else if ( s == "select" ) {
		para1 = 4;
		return true;
	}
	para1 = 0;

}
	
	
command_editor2_c* editor_runing_c::create ()
{
	return new editor_runing_c ();
}

int editor_runing_c::run(editor_c* o) 
{  
	o->runing ( para1, para2 );
}

void editor_runing_c::  get_semantic ( string* s )
{
	*s = semantic;
}

bool editor_runing_c:: set_parameter ( string s )
{
	stringstream ss { s };
//	echo << "set para: " << s << endl;
	ss >> s;
	if ( s == "stack" ) {
		para1 = 0;
	}
	else if ( s == "run" ) {
		para1 = 1;
	}
	para2 = 0;	
}


command_editor2_c* editor_editing_c::create ()
{
	return new editor_editing_c ();
}

int editor_editing_c::run(editor_c* o) 
{  
	o->editing ( para1 );
}

void editor_editing_c::  get_semantic ( string* s )
{
	*s = semantic;
}

bool editor_editing_c:: set_parameter ( string s )
{
	stringstream ss { s };
//	echo << "set para: " << s << endl;
	ss >> s;
	if ( s == "cut_line" ) {
		para1 = CUT_LINE;
	}
	else if ( s == "cut_word" ) {
		para1 = CUT_WORD;
	}
	else if ( s == "copy_line" ) {
		para1 = COPY_LINE;
	}
	else if ( s == "copy" ) {
		para1 = 	COPY;
	}
	else if ( s == "cut" ) {
		para1 = 	CUT;
	}	
	else if ( s == "change" ) {
		para1 = 	CHANGE;
	}	
	else if ( s == "paste" ) {
		para1 = 	PASTE;
	}	
	else if ( s == "xchange" ) {
		para1 = 	XCHANGE;
	}	
}
