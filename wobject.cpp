#include <stdio.h>
#include <cmath>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <sstream>
#include <thread>
#include <chrono>

#include "mstream.h"
#include "symbol/keysym.h"
#include "symbol.h"
#include "standard.h"

#include "matrix.h"
#include "position.h"
#include "gate.h"
#include "global.h"
#include "library/wdslib.h"
#include "position.h"
#include "sens/retina.h"
#include "sens/spirit.h"
#include "message.h"
#include "cash.h"
#include "object.h"
#include "line.h"
#include "spline.h"
#include "land.h"
#include "keyb.h"
#include "mouse.h"
#include "eyes.h"
#include "wr/texel.h"
#include "wr/shared.h"
#include "wr/layen.h"
#include "fox/edit.h"
#include "home.h"
#include "bookmarks.h"
#include "hand.h"
#include "lamb.h"

#include "object.h"
#include "wobject.h"

extern land_c land;

const unsigned long wobject_c::type = WOBJECT;

wobject_c::wobject_c ()
{
}

wobject_c::wobject_c ( string s )
{
}

unsigned long wobject_c::get_type ()
{
	return type;
}

void wobject_c::quit ()
{
	lamb_c& lamb = *lamb_c::self;
	rstream_c& ssi = lamb.eyes.retina .ssi;
	rstream_c& sso = lamb.eyes.retina .sso;
	int sufix;
//	echo << "wobject quit" << endl;
	for (;;) {
		if ( sso.access_request() ) {
			sso >> sufix;
			if ( sufix != 0 ) {
				sso.access_release ();
				return;
			}
			sso << 5 << object_id << lamb.object_id <<  "quit";
			sso.access_release ();
			break;
		}
		this_thread::sleep_for ( chrono::milliseconds ( 1 ));
	}
}

int wobject_c::command2 ( unsigned short stroke, string& cmd, string& ) 
{
	lamb_c& lamb = *lamb_c::self;
	rstream_c& ssi = lamb.eyes.retina .ssi;
	rstream_c& sso = lamb.eyes.retina .sso;
	int code;
	if ( sso.access_request () ) {
		sso >> code;
		if ( cmd == "refresh" ) {
			sso << 5 << object_id << lamb.object_id <<  "refresh";
		}
		sso.access_release ();
	}
}

int wobject_c::edit ( lamb_c& lamb, keyboard_c& keyb ) 
{
	rstream_c& ssi = lamb.eyes.retina .ssi;
	rstream_c& sso = lamb.eyes.retina .sso;
	int code;
	int stroke = keyb.get_stroke();	
//	echo << "wobject edit" << endl;
	if ( sso.access_request () ) {
		sso >> code;
		if ( code == 0 ){
			if ( stroke ==  XK_s ) {
				state = !state;
				sso << 5 << object_id << lamb.object_id <<  "state" << state;
			}
			else if ( stroke == XK_n ) {
				sso << 5 << object_id << lamb.object_id <<  "next_page";
			}
			else if ( stroke == XK_p ) {
				sso << 5 << object_id << lamb.object_id <<  "prev_page";
			}
		}
		sso.access_release ();
	}
	return 0;
}

void wobject_c::shift3 ( matrix_c <FT>& v )
{
	echo << "wobject shift" << endl;
	lamb_c& lamb = *lamb_c::self;
	rstream_c& ssi = lamb.eyes.retina .ssi;
	rstream_c& sso = lamb.eyes.retina .sso;
	int sufix;
	if (  ! ssi.access_request () ) {
		cout << "wobject shift ssi busy" << endl;
		return;
	}
	ssi.access_release ();
	for (;;) {
		if ( sso.access_request() ) {
			sso >> sufix;
			if ( sufix != 0 ) {
				sso.access_release ();
				return;
			}
			stringstream ss;
			sso << 5 << object_id << lamb.object_id <<  "shift";
			v.out ( 1 );
			v.serialize ( sso, true );
			sso.access_release ();
			break;
		}
//		break;
		this_thread::sleep_for ( chrono::milliseconds ( 1 ));
	}
}

void wobject_c::draw ( ) 
{
	lamb_c& lamb = *lamb_c::self;
//	cout << "draw wobject" << endl;
	rstream_c& ssi = lamb.eyes.retina .ssi;
	rstream_c& sso = lamb.eyes.retina .sso;
	int code;
	if ( sso.access_request() ) {
		sso >> code;
		if ( code == 0 ) {
			stringstream ss;
			if ( lamb.sensor == this ) {
				ss << "focus_yes";
			}
			else {
				ss << "focus_no";
			}
			sso << 5 << object_id << lamb.object_id <<  ss.str();
		}
		sso.access_release();
	}
}


wobject_c* wobject_c::create ()
{
	return new wobject_c ();
}


