#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <cmath>

#include "mstream.h"
#include "standard.h"

#include "symbol/keysym.h"
#include "symbol.h"

#include "library/wdslib.h"
#include "gate.h"

#include "matrix.h"
#include "position.h"
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
#include "line.h"
#include "spline.h"
#include "image.h"
#include "land.h"
#include "wr/font.h"
#include "keyb.h"
#include "home.h"
#include "make.h"
#include "comtab4.h"
#include "lamb_ct.h"
extern land_c land;
extern lamb_c lamb;

void comtab1_lamb_c::construct (  )
{
	push_vector_c < command_lamb2_c > v;
	
	v << new lamb_change_movement2_c ( "lamb change_movement" );
	
	set ( ct, v.lst, msc );
}

void comtab1_lamb_c::destruct  () {
	comtab_c::destruct ( msc );	
}

bool comtab1_lamb_c::find ( lamb_c* o, int mode, int control, unsigned long v, string c, string stroke_semantic )
{
	return finder.find ( *this, o, mode, control, v, c, stroke_semantic );
}

void comtab2_lamb_c::construct (  )
{
	push_vector_c < command_lamb2_c > v;

	v << new lamb_button_pressed2_c ( "lamb button_pressed" );
	v << new lamb_change_movement2_c  ( "lamb change_movement" );
	v << new lamb_shift2_c ( "lamb shift" );
	v << new lamb_rotate_c ( "lamb rotate" );
	v << new lamb_move_c ("lamb move" );
	set ( ct, v.lst, msc );
}

void comtab2_lamb_c::destruct  () {
	comtab_c::destruct ( msc );	
}

bool comtab2_lamb_c::find ( lamb_c* o, int mode, int control, unsigned long v, string c, string stroke_semantic )
{
	return finder.find ( *this, o, mode, control, v, c, stroke_semantic );
}


command_lamb2_c* lamb_change_movement2_c::create ()
{
	return new lamb_change_movement2_c ();
}

int lamb_change_movement2_c::run(lamb_c* o) 
{  
	echo << "run change_movement" << endl; 
	o->change_movement ( para1, para2 );
	return 1;
}

void lamb_change_movement2_c::get_semantic ( string* s )
{
	*s = semantic;
}

bool lamb_change_movement2_c::set_parameter ( string s )
{

	stringstream ss { s };
	ss >> para1 >> para2;
//	echo << "set para: " << s << endl;
	return true;
}


command_lamb2_c* lamb_button_pressed2_c::create ()
{
	return new lamb_button_pressed2_c ();
}

int lamb_button_pressed2_c::run(lamb_c* o) 
{  
	o->button_pressed ( para1 );
	return 1;
}

void lamb_button_pressed2_c::get_semantic ( string* s )
{
	*s = semantic;
}

bool lamb_button_pressed2_c::set_parameter ( string s )
{
	stringstream ss { s };
//	echo << "set para: " << s << endl;
	ss >> para1;
	return true;
}


command_lamb2_c* lamb_move_c:: create ()
{
	return new lamb_move_c ();
}

int lamb_move_c::run(lamb_c* o) 
{  
	o->move ( para1, para2, para3, para4 );
	return 1;
}

void lamb_move_c::  get_semantic ( string* s )
{
	*s = semantic;
}

bool lamb_move_c:: set_parameter ( string s )
{
	stringstream ss { s };
	
//	echo << "set para lamb move: " << s << endl;
	ss >> s;
	if ( s == "point" ) {
		para1 = POINT;
	}
	else if ( s == "shift_object" ) {
		para1 = SHIFT_OBJECT;
	}
	else if ( s == "rotate") {
		para1 = ROTATE;
	}
	else if ( s == "rotate_object" ) {
		para1 = ROTATE_OBJECT;
	}
	
	ss >> para2 >> para3 >> para4;
	return true;
}


command_lamb2_c* lamb_shift2_c:: create ()
{
	return new lamb_shift2_c ();
}

int lamb_shift2_c::run(lamb_c* o) 
{  
	o->shift ( o->sensor, para2, para3, para4 );
	return 1;
}

void lamb_shift2_c::  get_semantic ( string* s )
{
	*s = semantic;
}

bool lamb_shift2_c:: set_parameter ( string s )
{
	stringstream ss { s };
	
//	echo << "set para: " << s << endl;
	ss >> s;
	ss >> para2 >> para3 >> para4;
	return true;
}

command_lamb2_c* lamb_rotate_c:: create ()
{
	return new lamb_rotate_c ();
}

int lamb_rotate_c::run(lamb_c* o) 
{  
	o->rotate ( o->sensor, para2, para3, para4 );
	return 1;
}

void lamb_rotate_c::  get_semantic ( string* s )
{
	*s = semantic;
}

bool lamb_rotate_c:: set_parameter ( string s )
{
	stringstream ss { s };
	
//	echo << "set para: " << s << endl;
	ss >> s;
	ss >> para2 >> para3 >> para4;
	return true;
}


bool finder_c::find ( comtab1_lamb_c& ce, lamb_c*o,  int mode, int control, unsigned long stroke, string c, string stroke_semantic )
{
	if ( o == 0 ) {
		echo << "parser find object = 0" << endl;
		return false;
	}
		
	string s, scs, sct;
	if ( mode == COMMAND ) {
		scs = sct = "c";
	}
	if ( mode == VISUAL ) {
		scs = sct = "v";
	}
	if ( control == 0 ) {
		sct += " nm";
	}
	sct += " " + stroke_semantic;

//	echo << "findd: " << sct << endl;
	for ( auto x : ce.msc ) {
//		echo << "..: " << x.first << endl;
	}	
	
	auto p = ce.msc.find ( sct ); 
	if ( p != ce.msc.end() ) {
//		echo << "found" << endl; 
		int res = p ->second->run ( o );
//		echo << "results: " << res << endl;
		return true;
	}
	return false;
}

bool finder_c::find ( comtab2_lamb_c& ce, lamb_c*o,  int movement, int control, unsigned long stroke, string c, string stroke_semantic )
{
	if ( o == 0 ) {
		echo << "parser find object = 0" << endl;
		return false;
	}
	string s, scs, sct;
	if ( movement == ROTATE ) {
		echo << " rotate yes " << endl;
	}
	switch ( movement ) {
		case  POINT: sct = "p"; break;
		case SHIFT: sct = "s"; break;
		case ROTATE: sct = "r"; break;
		case SHIFT_OBJECT: sct = "so"; break;
		case ROTATE_OBJECT: sct = "ro";
	}
	if ( control == 0 ) {
		sct += " nm";
	}
	
	sct += " " + stroke_semantic;

	echo << "find tab2: " << sct << endl;
	for ( auto x : ce.msc ) {
//		echo << "..: " << x.first << endl;
	}	
	
	auto p = ce.msc.find ( sct ); 
	if ( p != ce.msc.end() ) {
		echo << "found" << endl; 
		int res = p ->second->run ( o );
		echo << "results: " << res << endl;
		return true;
	}
	return false;
}




