#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <iostream>

#include "library/wdslib.h"
#include "symbol/keysym.h"
#include "symbol.h"
#include "gate.h"
#include "mstream.h"
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
#include "wr/texel.h"
#include "wr/shared.h"
#include "wr/layen.h"
#include "fox/edit.h"
#include "home.h"
#include "bookmarks.h"
#include "hand.h"

#include "lamb.h"
#include "line.h"
#include "spline.h"
#include "land.h"
#include "carte.h"

extern land_c land;

const int cartesian_c::type = CARTESIAN;

unsigned long cartesian_c::get_type ()
{
	return type;
}

void cartesian_c::coocked_to_local ( object_c* o, matrix_c < FT > & cbase, matrix_c <FT >& cA, int& flag){
	stringstream ss;
	
	lamb_c& lamb = *lamb_c::self;

	matrix_c < FT > 
		oA = o -> motion.object_vector ( 1 ),
		b0 = motion.object_base (),
		A0 = motion.object_vector ( 1 );
	
	cbase = lamb.motion.object_base ();
	cA = A0 + ( b0*oA );
}

void cartesian_c::object_to_local ( object_c* o, matrix_c < FT >& lbase, matrix_c < FT >& lvA ) {
	matrix_service_c < FT > mxsrv;
	lbase = mxsrv . ortho3 ( vvx, vvy, 0, 0, 0 );	
	lvA = vA;
};


void cartesian_c::dump ()
{
	stringstream ss, s2;
	matrix_c < FT > m = tx.motion.object_vector ( 1 );
	matrix_c < FT > base = tx.motion.object_base ();
	matrix_c < FT > mx = { 0, 3, 4};
	mx.out ( 3, ss );
	m.out ( 3, ss );
	base.out ( 3, ss );
	echo << ss.str ();
}

void cartesian_c::locals_to_world ( vector < matrix_c < FT >  > & l )
{
	matrix_c < FT > b0 = motion.object_base ();
	matrix_c < FT > a0 = motion.object_vector ( 1 );
	matrix_service_c < FT > mxsrv;
	matrix_c < FT > b = mxsrv.ortho3 ( vvx, vvy, 0, 0, 0 );
	b = { vvx, vvy, vvz };
	b = .01*b;
	matrix_c < FT > a = vA;
	matrix_c < FT > base = b0*b;
	
	matrix_c < FT > A = a0 + b0*a;
	auto it = l.begin ();	
	for ( ; it != l.end(); it ++ ){
		*it = base*(*it) + A;
	}
}

cartesian_c::cartesian_c () :
	right_hand ( true ),
	dimension ( 3 )
{
	lamb_c& lamb = *lamb_c::self;
	string st;
	tx . layen.set_text (st = "x");
	tx . parent = this;
	tx . simple_text = true;
	ty . layen.set_text ( st = "y" );
	ty . parent = this;
	ty. simple_text = true;
	tz . layen.set_text ( st = "z" );
	tz . parent = this;
	tz . simple_text = true;
	
	int cell = 20;
	tx.frame_height = 1;
	tx.frame_width = 10;
	tx.layen.wi = tx.frame_width*cell; 
	tx.layen.he = ( tx.frame_height )*cell; 
	tx.layen.si = tx.layen.wi*tx.layen.he; 
	tx.layen.pi = new uint32_t[tx.layen.si];
	tx.impression_clients.retina = &lamb.eyes.retina ;
	
	ty.frame_height = 1;
	ty.frame_width = 10;
	ty.layen.wi = ty.frame_width *cell; 
	ty.layen.he = ( ty.frame_height + 1 )*cell; 
	ty.layen.si = ty.layen.wi*ty.layen.he; 
	ty.layen.pi = new uint32_t[ty.layen.si];
	ty.impression_clients.retina = &lamb.eyes.retina ;

	tz.frame_height = 1;
	tz.frame_width = 10;
	tz.layen.wi = tz.frame_width*cell; 
	tz.layen.he = ( tz.frame_height + 1 )*cell; 
	tz.layen.si = tz.layen.wi*tz.layen.he; 
	tz.layen.pi = new uint32_t[tz.layen.si];
	tz.impression_clients.retina = &lamb.eyes.retina ;

	matrix_c < FT > vvA = { 100, 0, 0 }, vvb = { 1, 0, 0 }, vvvx = { 0, 1, 0 };
//	tx.vA = { 100, 0, 0 };
//	ty.vA = { 0, 100 , 0 };
//	tz.vA = { 0, 0 , 100 };	
	tx.motion.set_object ( vvA, vvb, vvvx );
	vvA = { 0, 100, 0 };
	ty.motion.set_object ( vvA, vvb, vvvx );
	vvA = { 0, 0, 100 };
	tz.motion.set_object ( vvA, vvb, vvvx );
	matrix_c<FT> m = tx.motion.object_base();
	stringstream ss;
	m.out ( 2, ss );
	echo << "t\n" << ss.str();
}

cartesian_c* cartesian_c::create ()
{
    return new cartesian_c ();
}

int cartesian_c::edit ( lamb_c& lamb, keyboard_c& keyb)
{
	uint16_t v = keyb.get_stroke ();
	string c = keyb.get_char ();
	if ( c == "s" ) {
		state = ! state;
	}
	if ( c== "c" ) {
		if ( dimension == 3 ) {
//			land.impress ( lamb );
			matrix_c < FT > swap = vvz;
			vvz = vvx, vvx = vvy, vvy = swap;
			dimension = 2;
		}
		else {
			matrix_c < FT > swap = vvz;
			vvz = vvy, vvy = vvx, vvx = swap;
			dimension = 3;
		}
	}
	if ( c == "p" ) {
		vvx = { 100, -100, - 100 };	
		vvx = 1/sqrt ( 2 )* vvx;
	}
	if (c == "q" ) {
		vvx = { 100, 0, 0 };
	}
	if ( c == "d" ) {
		dump ();
	}
	return 0;
}

void cartesian_c::draw ()
{
	draw ( *lamb_c::self);
}

void cartesian_c::draw ( lamb_c& lamb )
{
	float pi = 3.1814;
	echo << "draw cartesian" << endl;
	matrix_c < FT > b0 = motion.object_base ();
	matrix_c < FT > a0 = motion.vA;
	matrix_service_c < FT > mxsrv;
	matrix_c < FT > b = mxsrv.ortho3 ( vvx, vvy, 0, 0, 0 );
	b = { vvx, vvy , vvz };
	b = .01*b;
	matrix_c < FT > a = vA;
	matrix_c < FT > base = b0*b;
	
	matrix_c < FT > A = a0 + b0*a;
	base = 100*base;
		
	vector < matrix_c < FT > > l = {
	};
	for ( float x = 0; x <= 100; x += 1 ) {
		float y;
		y = sqrt ( x );
		y = 25* cos ( 4*pi*x/100.0 );
		matrix_c < FT > m;
		m = { x, y, 0 };
		l.push_back ( m );
	}
	
	locals_to_world ( l );
	g.order = 2;
	g.control_points = l;
	g.draw (  );	
	
	x . color = 0xff0000;
	y . color = 0xff00;
	z . color = 0xff;
	int32_t color = 0x00;
	if ( lamb.sensor == this ) {
		color |= 0xff00;
	}
	if ( state == 1 ) {
		color |= 0xff0000;
	}
	if ( state == 1 || lamb.sensor == this ) {
		x . color = color;
		y . color = color;
		z . color = color;
	}
	string st;	
	if ( dimension == 3 ) {
		tz.layen.set_text ( st = "z" );
	}
	else {
		tz.layen.set_text ( st = "" );
	}

	tx . draw (  );
	ty . draw (  );
	tz . draw (  );
	tx . draw (  );
	ty . draw (  );
	tz . draw (  );

	x.control_points = { 
		{ A },
		{A + base.get_column ( 1 )}
	};
	y.control_points = { 
		{ A },
		{ A + base.get_column ( 2 )}
	};
	if ( dimension == 3 ) {
		z.control_points = { 
			{ A },
			{ A + base.get_column ( 3 )}
		};
	}
	else {
		z.control_points = { 
		};
	}
	x.draw (  );
	y.draw (  );
	z.draw ( );
	x.draw (  );
	y.draw (  );
	z.draw (  );
}

void cartesian_c::serialize ( fstream &file, bool bsave )
{
	if( bsave ){
		file << "CARTESIAN" << endl;
	}
	motion.serialize ( file, bsave );
	if(!bsave ){
		vA.serialize ( file, bsave );
		vvx.serialize ( file, bsave );
		vvy.serialize ( file, bsave );
		vvz.serialize ( file, bsave );
		string s;		
		file >> s;
		tx.serialize ( file, bsave );
		file >> s;
		ty.serialize ( file, bsave );
		file >> s;
		tz.serialize ( file, bsave );
		file >> dimension;
	}
	else{
		vA.serialize ( file, bsave );
		vvx.serialize ( file, bsave );
		vvy.serialize ( file, bsave );
		vvz.serialize ( file, bsave );
		tx.serialize ( file, bsave );
		ty.serialize ( file, bsave );
		tz.serialize ( file, bsave );
		file << dimension << endl;
	}
}

namespace ex_cartesian{

}// end namespace ex_cartesian

void ex_entry_cartesian ( int a, string s, void* vp )
{
	static cartesian_c cartesian;
}