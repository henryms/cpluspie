#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <sstream>
#include<cassert>


#include "library/wdslib.h"
#include "mstream.h"
#include "gate.h"
#include "matrix.h"
#include "position.h"

#include "global.h"

#include "symbol/keysym.h"

#include "keyb.h"
#include "sens/retina.h"
#include "sens/spirit.h"
#include "message.h"
#include "cash.h"
#include "object.h"
#include "line.h"
#include "spline.h"
#include "mouse.h"
#include "eyes.h"
#include "home.h"
#include "bookmarks.h"
#include "wr/texel.h"
#include "wr/shared.h"
#include "wr/layen.h"
#include "fox/edit.h"
#include "hand.h"

#include "lamb.h"
#include "land.h"

extern land_c land;

/* If width or height is even the suplementary pixel is on the negative x or y.
 * The frame begins at the corner top left. */

void eyes_c::draw()
{
	if (off){
		border.segments.clear();
		border.knots.clear();
		focus.segments.clear();		
		focus.knots.clear();
		return;
	}
	lamb_c& lamb = *lamb_c::self;

	uint32_t color = 0x00;
	if ( lamb.sensor == this ){
		color = 0x00ff00;
	}
	int l =  frame_x - frame_width/2;
	int r = l + frame_width;
	int b = frame_y - frame_height/2;
	int t = b + frame_height;
	matrix_c < FT > v, lt;
	focus.color = color;	
	focus.control_points = {
		lt = lamb.view_to_world ( v = { l, t, 0} ),
		lamb.view_to_world ( v = { r, t, 0}  ),
		lamb.view_to_world ( v = { r, b, 0} ),	
		lamb.view_to_world ( v = { l, b, 0} ),
		lt
	};
	focus.draw ( );
	return;
	l =  - width/2;
	r = l + width - 1;
	b = - height/2;
	t = b + height - 1;
	border.color = 0xff0000;
	border.control_points = {
		lt = lamb.view_to_world ( v = { l, t, 0} ),
		lamb.view_to_world ( v = { r, t, 0}  ),
		lamb.view_to_world ( v = { r, b, 0} ),	
		lamb.view_to_world ( v = { l, b, 0} ),
		lt
	};
	border.draw ( );
}

void eyes_c::draw ( lamb_c& lamb )
{
	draw ();
}

int eyes_c::edit ( lamb_c& lamb, keyboard_c& keyb )
{
	string c = keyb.get_char ();
	int k = keyb.get_stroke ();
	unsigned long control = keyb.get_controls ();
	int inc = 8;
	if ( k == XK_j && control == LL_KA_LC ){
		frame_width -= inc;	
	}
	else if ( k == XK_k && control == LL_KA_LC ){
		frame_width += inc;
	}
	else if ( k == XK_i && control == LL_KA_LC ){
		frame_height += inc;
	}
	else if ( k == XK_m && control == LL_KA_LC ){
		frame_height -= inc;
	}
	else if ( k == XK_j && control == LL_KA_LC ){
		frame_width -= inc;	
	}
	else if ( k == XK_h ){
		frame_x-= inc;
	}
	else if ( k == XK_l ){
		frame_x+= inc;
	}
	else if ( k == XK_i ){
		frame_y+= inc;
	}
	else if ( k == XK_m ){
		frame_y-= inc;
	}
//	land.impress ( lamb );
	return false;
}

void eyes_c::grip ( matrix_c<FT> vA, unsigned long color )
{
	cout << "grip\n";
	int X = vA[1];
	int Y = vA[2];
	int d = 10;
	list<pair<float, uint32_t>>::iterator it;
	for ( int r=-d;r<= d;r++){
//		set_pixel ( 0, X+r, Y-d, 0, color );
//		set_pixel ( 0, X+r, Y+d, 0, color );
//		set_pixel ( 0, X-d, Y+r, 0, color );
//		set_pixel ( 0, X+d, Y+r, 0, color );
	}
}

void eyes_c::parallelogram ( matrix_c<FT> vA, matrix_c<FT> vx, matrix_c<FT> vy, bool frame, bool erase, unsigned long color ){
	int X = vA[1];
	int Y = vA[2];
	int ex = vx[1];
	int ey = vy[2];
	int sx = 0;
	int sy = 0;
	if ( sx>ex ){
	    std::swap ( sx, ex );
	}
	if ( sy>ey ){
	    std::swap ( sy, ey );
	}
	int x, y;
	unsigned long co = erase?0xffffff:color;
	list<pair<float, uint32_t>>::iterator it;
	if ( frame ){
		for ( x = sx;x<= ex;x++){
//			set_pixel ( 0, X+x, Y+sy, 0, co );
//			set_pixel ( 0, X+x, Y+ey, 0, co );
			if ( DEBUG ){
//				set_pixel ( 0, X+x, Y+(sy+ey )/2, 0, co );
			}
		}
		for ( y = sy;y<= ey;y++){
//			set_pixel ( 0, X+sx, Y+y, 0, co );
//			set_pixel ( 0, X+ex, Y+y, 0, co );
			if ( DEBUG ){
//				set_pixel ( 0, X+(sx+ex )/2, Y+y, 0, co );
			}
		}
	}
	if ( erase ){
		for ( int y = sy+1;y<ey;y++){
			for ( int x = sx+1;x<ex;x++){
//			    set_pixel ( 0, X+x, Y+y, 0, 0xffffff );
			}
		}	    
	}	
}

void eyes_c::translate ( matrix_c<FT> vA, matrix_c<FT> va, matrix_c<FT> vb, matrix_c<FT> vc, matrix_c<FT> vt ) 
{
	echo << "translate" << endl;
	list<pair<float, uint32_t>>::iterator rit;
	int tx = vt[1];
	int ty = vt[2];
	int ex = va[1];
	int ey = vb[2];
	int X = vA[1];
	int Y = vA[2];
	int icx = 1;
	int icy = 1;
	int bx = 0;
	int by = 0;
	// tx = 0;
	icx=(tx > 0 )?-1:1;
	if ((bx > ex )^(icx == -1 )){
		std::swap ( bx, ex );
	}	
	icy=(ty > 0 )?-1:1;
	if ((by > ey )^(icy == -1 )){
		std::swap ( by, ey );
	}
// oout ("X", X ); oout ("bx", bx );oout ("ex", ex ); oout ("icx", icx ); oout ("tx", tx );
//oout ("Y", Y ); oout ("by", by );oout ("ey", ey ); oout ("icy", icy ); oout ("ty", ty );
	// if ( icy<0 ) ey+= 4*icy;
	// else by = by;
	for ( int y = by; y!= ey ; y+= icy ){
		for (int x = bx; x!= ex ; x+= icx ){
//			long pix = get_pixel ( X+x, Y+y );
//			set_pixel ( 0, X+x+tx, Y+y+ty, 0, pix );
		}
	}
}

bool eyes_c::set_superficial_pixel ( int x, int y, uint32_t color ) 
{
	int X0 = width >> 1;
	int Y0 = height >> 1;
	
	x = x + X0;
	y = y + Y0;
	if ( x < 0 || width <= x || y < 0 || height <= y ){
		return false;
	}
	int u = height - y - 1;

///	surface2_c & surface = retinaa.surfaces.l.front ();
///	uint32_t* pf = surface.frame;
///	pf [width*u + x ] = color;				
}
void eyes_c::set_superface ( uint32_t color ) 
{
/**
	 echo << "set_superface" << endl;
///	uint32_t* fp = retinaa.surfaces.l.front ().frame;
	int n = width*height;
	for ( int c = 0; c < n; c ++ ) {
		*fp = color;
		fp ++;
	}
**/
}

void eyes_c::set_ground ( long color )
{
	static int c = 0;
	c++;
}

void eyes_c::change_surface ( lamb_c& lamb, int i )
{
	echo << "change surface" << endl;
	mout << "change surface\n";
	list<surface_c>& l = retina.surfaces;
	if ( l.size () <= 1 ){
		assert(false);
		return;
	}
	if ( i == 0 ){
		l.pop_front ();
	}
	else {
		
		i = i%l.size ();
		if ( i == 0 ){
			assert(false);
			return;
		}
		auto it = l.begin ();
		std::move ( next (it, i ), next (it, i+1 ), it );
		cout << l.size ()<< "iii:" << i << endl;
	}
	surface_c& s = *l.begin ();
	lamb.eyes.zoom = s.zoom;
	lamb.motion = s.motion;
	lamb.mx = s.mx;
//	lamb.local_position = s.local_position;
//	lamb.hand.pointer = s.pointer;
	lamb.eyes.height = s.height;
	lamb.eyes.width = s.width;

/*
	echo << "change surface" << endl;
	cout << "change surface" << endl;
	list<surface2_c>& l = retinaa.surfaces.l;
	if ( l.size () <= 1 ){
		return;
	}
	if ( i == 0 ){
		l.pop_front ();
	}
	else {
		i = i%l.size ();
		if ( i == 0 ){
			return;
		}
		auto it = l.begin ();
	//	l.splice ( it, l, next ( it, i ));
		std::move ( next (it, i ), next (it, ++i ), it );
		echo << l.size () << endl;
	}
	surface2_c& s = *l.begin ();
	lamb.eyes.zoom = s.zoom;
	lamb.motion = s.motion;
	lamb.mx = s.mx;
	lamb.local_position = s.local_position;
	lamb.hand.pointer = s.pointer;
	lamb.eyes.height = s.height;
	lamb.eyes.width = s.width;
*/
}

void eyes_c::push_surface ( lamb_c& lamb )
{
	echo <<  "push surface" << endl;	
	retina .surfaces.push_front ( surface_c () ) ;
	surface_c& sf = retina .surfaces.front ();

	sf.initialize ( width, height, 0xffffff );	
	
	sf.zoom = lamb.eyes.zoom;
	sf.motion = lamb.motion;	
	sf.mx = lamb.mx;
	retina .owner = &lamb;	
	
	sf.width = lamb.eyes.width;
	sf.height = lamb.eyes.height;
	if ( retina .surfaces.size () > 5 ){
		retina .surfaces.pop_back ();
	}

}

void eyes_c::set_size ( lamb_c& lamb, int x, int y )
{
	width = x;
	height = y;
	push_surface ( lamb );
	return;
}

long eyes_c::get_pixel ( int x, int y )
{

	cout << "get pixel: " << endl;
	int x0 = width/2;
	int y0 = height/2;
	x = x+x0;
	y = y+y0;
	if ( x<0 || width<= x || y<0 || height<= y ){
		return 0;
	}
	//	return 0;

//	return retina.surfaces.l.front ().frame[((height-y-1 )*width+x )];
	return retina.surfaces.front ().frame[((height-y-1 )*width+x )];

}

eyes_c::eyes_c ():
	frame_width ( 100 ),
	frame_height ( 100 ),
	frame_x ( 0 ),
	frame_y ( 0 )
{

	width = 0;
	height = 0;
	dpm = 1;
	zoom = 1;
	valide = false;
	mout << "eyes construction\n";
}

eyes_c::~eyes_c ()
{
	mout << "eyes destruction\n";
}
