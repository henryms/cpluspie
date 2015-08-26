#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <algorithm>
#include <list>
#include <vector>
#include <map>
#include <complex>
#include <chrono>
#include <cmath>
#include <typeinfo>
#include<tuple>
#include <thread>
#include <unistd.h>


#include "debug.h"


#include "mstream.h"
#include "symbol/keysym.h"
#include "symbol.h"

#include "library/wdslib.h"
#include "gate.h"

#include "matrix.h"
#include "position.h"
#include "message.h"
#include "cash.h"

#include "mouse.h"
#include "keyb.h"
#include "sens/retina.h"
#include "sens/spirit.h"
#include "object.h"
#include "line.h"
#include "spline.h"

#include "land.h"
#include "global.h"

#include "wr/texel.h"
#include "wr/shared.h"
#include "wr/layen.h"
#include "fox/edit.h"
#include "bookmarks.h"
#include "carte.h"
#include "image.h"
#include "wobject.h"
#include "eyes.h"
#include "hand.h"
#include "home.h"
#include "lamb.h"

#include "comtab4.h"
#include "edit_ct.h"
#include "lamb_ct.h"


#include "make.h"
#include "regexp.h"
#include "shell.h"

extern land_c land;
extern int parrot;

//#include </home/henry/usr/include/c++/4.9.1/regex>
//#include </home/henry/usr/include/c++/4.9.1/iterator>

#include <regex>
#include <iterator>

int main ( int argc, char* argv [] ) 
{
	nout.mode = 0;	

	string s, s0, s1, s2, s3;
	stringstream ss;
	for (int c = 0; c < argc; ++c)
		ss << string{argv[c]} << " ";
	ss >> s0 >> s1 >> s2 >> s3;
	
	if (1){
		mout << "hi lamb!\n" 	
		<< ss.str() << '\n';
	}
			
	if( argc == 2){
		if ( s1.substr(0, 6) == "-path="){
			set_lambs_path(s1.substr(6));
		}
	}
	d.f1 = 0;
	d.f2 = 1;
	d.f3 = 0;
	d.f10 = 1;
	d.caret_check = 0;
	d.rel_a = 1;
	d.rel_b = 0;

	font_c::init ();
//	font_c::system_face = font_c::set_face("DejaVuSansMono.ttf",13,1);	
	font_c::system_face = font_c::set_face("Vera-code.ttf",13,1);	
//	font_c::system_face = font_c::set_face("Vera-code.ttf",13,.9);	
	lamb_c::comtab1_lamb.construct();
	lamb_c::comtab2_lamb.construct();
	editor_c::comtab_editor.construct ();
	
	parrot = 7;	
	echo << "hey april!\n";
	lamb_c::self = new lamb_c;
	land.insert(lamb_c::self);
	lamb_c& lamb = *lamb_c::self;
	
	echo << "lamb's id: " << lamb.object_id << endl;	
	
	event_s ee;	
	if ( argc >= 2 ){
		if (s1 == "-d"){
			 lamb.debug("db "+s2+" "+s3);  
		}		
		else if ( s1.substr(0, 6) != "-path="){
			s = "ed "+ s1;
			if ( argc >= 3 ) {
				s += " " + s2 ;
			}
			lamb.start_command = s;
			cout << lamb.start_command;
			echo << lamb.start_command;
		}
	}
	
	message_c* pm = get_machine ();	
	if ( pm == nullptr ){
		cout << "machine fail" << endl;
		return EXIT_FAILURE;
	}
	lamb.that = pm;
	pm->that = &lamb;
	
	retina_c& r3 = lamb.eyes.retina ;	

	char* pi;	
	if ( ! lamb.memory_map2 ( "retina_smi", 50000000, pi ) ) {
		cout << "shared memory smi bad" << endl; 
	}
	r3.ssi = rstream_c (pi) ;
	*(uint8_t*)(r3.ssi.psm + 4)= 0;
	r3.ssi<<0;
	r3.ssi.access_release();
	if ( !	lamb.memory_map2 ( "retina_smo", 100000, pi ) ) {
		cout << "shared memory smo bad" << endl;
	} 
	r3.sso = rstream_c ( pi ); 
	*(uint8_t*)(r3.sso.psm + 4)= 0;
	r3.sso <<  0;
	r3.ssi.access_release ();

//	r3.test ( 0 ,"" );		

	lamb.stream_in = true;					
	lamb.status.layen.signo = 2;
	lamb.status.draw();
	
	lamb.open ();
	land.vanish();
	
	font_c::done();
	
	lamb_c::comtab2_lamb.destruct();
	lamb_c::comtab1_lamb.destruct();
	editor_c::comtab_editor.destruct();
	return 0;
}
