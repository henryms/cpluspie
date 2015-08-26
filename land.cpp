#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <vector>
#include <sstream>

#include "debug.h"

#include "mstream.h"
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
#include "object.h"
#include "wobject.h"
#include "line.h"
#include "spline.h"
#include "land.h"
#include "matrix.h"
#include "keyb.h"
#include "mouse.h"
#include "eyes.h"
#include "hand.h"
#include "wr/texel.h"
#include "wr/shared.h"
#include "wr/layen.h"
#include "fox/edit.h"
#include "home.h"
#include "fox/bookmarks.h"
#include "lamb.h"
#include "global.h"

using namespace std;

land_c land;
matrix_c<FT> land_c::rasterize ( matrix_c<FT> pt ){
	if ( raster == 0 ) {
		return pt;
	}
	int n;
	float f;
	f = pt[1];
	if ( f<0 ){
		f-= raster/2;
	}
	else {
		f+= raster/2;
	}
	n = f/raster;
	pt[1]= n*raster;
	f = pt[2];
	if ( f<0 ){
		f-= raster/2;
	}
	else{
		f+= raster/2;
	}
	n = f/raster;
	pt[2]= n*raster;
	f = pt[3];
	if ( f<0 ){
		f-= raster/2;
	}
	else{
		f+= raster/2;
	}
	n = f/raster;
	pt[3]= n*raster;
	return pt;
}

void land_c::invalide_all_objects ( int level )
{
	for ( auto x : llist ) {
		x->invalide_visual ( level );
	}
}

int land_c::get_handle ()
{
	vector <int > v;
	for ( auto x : llist ) {
		if ( x->object_id >= v.size () ){
			v.resize ( x->object_id +1 );
		}
		v[x->object_id] =  1 ;
	}
	int x = 1;
	for ( ; x < v.size () ; x ++ ) {
		if ( v.at(x) == 0 ) {
			break;
		}
	}
	string s;
	wobject_c*  p = new wobject_c ( s );
	p -> object_id = x;
//	lamb_c* pl = lamb_c::self;
//	pl -> local (*p, pl -> hand.pointer, {0, 0, -1}, {1, 0, 0} );
	land.llist.push_front ( p );
	return x;
}

void land_c::insert ( object_c* o )
{
	vector <int > v;
	for ( auto x : llist ) {
		if ( x->object_id >= v.size () ){
			v.resize ( x->object_id +1 );
			v[x->object_id] =  1 ;
		}
	}
	int x = 1;
	for ( ; x < v.size () ; x ++ ) {
		if ( v.at(x) == 0 ) {
			break;
		}
	}
	o ->object_id = x;
	llist.push_back ( o );
}

void land_c::expose ()
{
	lamb_c::self->expose_image();
}

void land_c::impress ()
{	
	for ( auto x : llist ) {
		x ->draw();
	}
}

land_c::land_c ()
{
	raster = 10;
	show_base = false;
}

void land_c::remove_object ( object_c* object_pointer )
{
	for ( auto it = llist.begin (); it != llist.end (); ){
		if (*it == object_pointer ){
			it = llist.erase ( it );
			continue;
		}
		it ++;
	}
}

void land_c::remove_selection ()
{
	object_c* pobj;
	for ( auto it = llist.begin (); it != llist.end ();){
		pobj = *it;
		if ( pobj->state == 1 ){
			(*it) -> quit();
			echo << "erase" << endl;
			it = llist.erase ( it );
			delete pobj;
			continue;
		}	
		it++;
	}
}

void land_c::vanish()
{
	for ( auto& x : llist ){
		 delete x;
	}
}

land_c::~land_c ()
{
	cout << "land destruct" << endl;
}

