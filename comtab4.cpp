#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <cmath>
#include <tuple>
#include <typeinfo>

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

extern land_c land;
extern lamb_c lamb;

void command_base_c::construct ( )
{
}

void parser_c::demultiplex_condition ( string& line, list <  string>&lst ) 
{
	const bool debug = 0;
	list < list <string > > lt;
	string  s0, s1;
	stringstream ss ( line );
	if ( debug ) {
		echo << "demux: " << ss.str () << endl;
	}
	if ( line.size () == 0 ) {
		return;
	}
	lt.push_back ( list < string > () );	
	auto  it = lt.begin ();	
	for ( ;; ) {
		
		ss >> s0;
		
		if ( s0 == ";" ) {
			break;
		}
		if ( s0 == "," ) {
			lt.push_back ( list < string > () );	
			it ++;
			continue;
			break;
		}
		for ( auto i = it -> begin (); i != it -> end(); i++ ){
			s1 = *i + " " + s0;
			it -> push_front ( s1 );
		}
		it -> push_front ( s0 ); 
	}
	/*
	if ( debug ) {
		for ( auto x : lt ) {
			for ( auto y : x ) {
				echo << y << endl;
			}
		}
	}
	*/
	for ( it = lt.begin (); ; it ++) {
		if ( it == lt.end () ) {
			return;
		}
		if ( it -> size () != 0 ) {
			break;
		}
	}
	auto it2 = it;
	for ( it2++; it2 != lt.end () ; it2++ ) {
		if ( it2 -> size () == 0 ) {
			continue;
		}
		auto i = it -> begin ();
		auto io = i;
		for (; i !=  it -> end () ; i ++ ) {
			for ( auto ii = it2 -> begin (); ii != it2 ->end () ; ii++ ) {
				it -> push_front ( *i + " " + *ii );
			}
		}
		it -> erase ( io, it -> end () );
	}			
	lst = *lt.begin ();
	if ( debug ) {
		for ( auto c : lst ) {
			echo << c << endl;
		}
		echo << "...." << endl;
	}
}

void parser_c::parse ( string& ct ) 
{
	list < string > key_lst;
	string class_name, action, semantic, parameter, s, line;
	stringstream sl ( ct ), ss;
	getline ( sl, line );
	for ( ;getline ( sl, line ); ) {
		ss .clear ();
		ss.str ( line );
		ss >> s;
		if ( s == "class" ) {
			ss >> class_name;
			continue;
		}
		demultiplex_condition ( line, key_lst ); 

		getline ( ss, s, ';' );
		ss >> action;
		parameter = "";
		ss >> parameter;
		getline (ss , s );
		parameter += s;
		semantic = class_name + " " + action;
//		echo << "semantic: " << semantic << endl;
//		echo << "parameter: " << parameter << endl;
		lst.push_back ( make_tuple ( key_lst, semantic, parameter ) );	
	}
}



editor_c* get_editor();

void ex_entry_comtab4 ( stringstream& ss )
{
	return;
}

editor_c* get_editor ( )
{
	editor_c* p;
	lamb_c& lamb = *lamb_c::self;
	auto it = land.llist.begin ();
	for (; it != land.llist.end (); it++){
		if ( (*it )->get_type () == EDITOR ){
			if (*it != &lamb.mouth && *it != &lamb.status ){
				p = static_cast < editor_c*> (*it);
				return p;
			}
		}				
	}
	return nullptr;
}

