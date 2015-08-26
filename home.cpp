#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <complex>
#include <cmath>
#include <regex>

#include "symbol/keysym.h"
#include "symbol.h"

#include "library/wdslib.h"
#include "gate.h"
#include "mstream.h"

#include "matrix.h"
#include "position.h"
#include "fox/cash.h"

#include "sens/retina.h"
#include "sens/spirit.h"
#include "message.h"
#include "global.h"

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
#include "image.h"
#include "land.h"
#include "wr/font.h"
#include "wr/texel.h"
#include "wr/shared.h"
#include "keyb.h"
#include "edit.h"

#include "home.h"

extern land_c land;

extern int parrot;


int home_c::initial_sensor = 0;

window_manager_c::window_manager_c ()
{
}

void window_manager_c::transform ( int sx, int sy,  rectangle_c& r, int gravity , int revert  = NONE )
{
	// As a matter of mathematics, the only transformation which applied twice does not 
	// correspond with the origine are 90 and 180 degree rotation. 
	
	const int _90 = WEST_NORTH;  // _90 degree conterclockwise 
	const int _180 = NORTH_EAST; 
	const int _270 = EAST_SOUTH;
	const int MY = NORTH_WEST;	// mirror about an y axes 
	
	rectangle_c s;
	if ( gravity == SOUTH_WEST ) {
		s = r;
	}
	else if ( gravity == WEST_NORTH ) {
		if ( revert ) {
			return transform ( sx, sy, r, _270 , false );
		}
		else {
			s.right = sy - r.bottom;
			s.left = sy - r.top;
			s.top = r.right;
			s.bottom = r.left;
		}
	}
	else if ( gravity == NORTH_WEST ) {
		s.right = r.right;
		s.left = r.left;
		s.top = sy -  r.bottom;
		s.bottom = sy - r.top;
	}
	else if ( gravity == WEST_SOUTH ) {
		transform ( sx, sy, r, MY, false );
		return transform (sx ,sy ,r, _90, false ); 
	}
	else if ( gravity == NORTH_EAST ) {
		transform ( sx,sy , r, _90, false);
		return transform ( sy, sx , r, _90 , false );
	}
	else if ( gravity == EAST_NORTH ) {
		transform ( sx, sy , r, MY, false );
		return transform ( sx, sy , r, _90, false );
	}
	else if ( gravity == EAST_SOUTH ) {
		if ( revert ) {
			return transform ( sx, sy, r, _90, false );
		}
		else {
			transform ( sx, sy , r, _90, false );
			return transform ( sy, sx, r, _180, false );
		}
	}
	else if ( gravity == SOUTH_EAST ) {
		transform ( sx, sy , r, MY, false );
		return transform ( sx, sy, r, _180, false );
	}
	r = s;
	return;
}


void window_manager_c::manage ( lamb_c& lamb, rectangle_c& rectangle , int gravity )
{
	rectangle_c r = rectangle;
	
	int sx = lamb.eyes.width;
	int sy = lamb.eyes.height;
	rectangle_c rr ;
	if ( get_free_place (r, gravity, lamb.eyes.width, lamb.eyes.height )) {
		rr.left = r.left - sx/2;
		rr.right = r.right - sx/2;
		rr.bottom = r.bottom - sy/2;
		rr.top = r.top - sy/2;
		rectangles.push_back ( r );
	}
	rectangle = rr;
}

pair < int, int> mysort ( list < pair < int, int > >& sl , int max )
{
	sl.sort ( [] ( pair < int, int > p1, pair < int, int > p2 ) {
		if ( p1.first < p2.first ) {
			return true;
		}
		 else  {
			return false; 
		}
	} );
	int a = 0;
	for ( auto x : sl ) {
		if ( a < x.first ) {
			return { a, x.first };
		}
		else {
			a = x.second;
		}
	}
	if ( a < max ) {
		return { a, max };
	}
	return { 0, 0 };
}

void window_manager_c::shr ( rectangle_c r )
{
	echo << "l: " << r.left <<  " " << r.right << " "
		<< r.bottom << " " << r.top << endl;
}

void window_manager_c::shrl ( list < rectangle_c > rs )
{
	echo << "rectangle list size:" << rs.size ();
	for ( auto r : rs ) {
		echo << "l: " << r.left <<  " " << r.right << " "
		<< r.bottom << " " << r.top << endl;
	}
}

void window_manager_c::sort_list (int  sx, int sy, list < rectangle_c >& rectangles ) 
{
	rectangles.sort ( [] ( rectangle_c r1 , rectangle_c r2 ) {
		if (  r1.bottom < r2.bottom || ( r1.bottom == r2.bottom && r1.left < r2.left ) )
			return true; else return false;});
				
	rectangle_c r, r0;
	int level = 0;				
	r.left =  r.right = r.bottom = r.top = 0;
	rectangles.push_front ( r );
	auto i = rectangles.begin ();
	for (; i != rectangles.end(); i ++ ) {
		if ( i->bottom > level ) {
			r.left = r.right = sx;
			r.bottom = r.top = level;
			rectangles.insert (i, r );
			level = i->bottom;
			r.left = r.right = 0;
			r.bottom = r.top = level;
			rectangles.insert (i, r );
		}
	}
	r.left = r.right = sx;
	r.bottom = r.top = level;
	rectangles.push_back ( r ) ;
	
	if ( rectangles.size () > 2 ) {
		int roof = sy;
		for (i = rectangles.begin (); i != rectangles.end(); i ++ ) {
			if ( i->top < roof && i-> top >level ) {
				roof = i->top;
			}
		}
		r.left = r.right = 0;
		r.bottom = r.top = roof;
		rectangles.push_back ( r );
		r.left = r.right = sx;
		rectangles.push_back ( r );
	}
}

bool fflag = false;

void window_manager_c::test ( lamb_c& lamb, string s ) 
{
static list < spline_c > sl;
static spline_c* sp = nullptr;
static spline_c spl0; 

	fflag  = true;
	
	echo << "wm test " << endl;
	
	if ( s == "delete" ){
		sl.clear ();
		return;
	}
			
	spl0 = { 0, 100 , 0,  50, false };
	spl0.draw ( );	
	
	rectangle_c r;
	
	r = { 0,20, 0, 15 };
	get_free_place (r, SOUTH_WEST, 100,50 ); 
	rectangles.push_back ( r );

	r = { 0,100, 0, 10 };
	get_free_place (r, SOUTH_WEST, 100,50 ); 
	rectangles.push_back ( r );
	
	r = { 0,35, 0, 40 };
	get_free_place (r, SOUTH_WEST, 100,50 ); 
	rectangles.push_back ( r );
	
	 r = { 0, 20, 0, 10 };
	get_free_place (r, EAST_SOUTH, 100,50 ); 
	rectangles.push_back ( r );
	
	r = { 0,10, 0, 45 };
	get_free_place (r, EAST_SOUTH, 100,50 ); 
	rectangles.push_back ( r );
	
	r = { 0,15, 0, 40 };
	get_free_place (r, EAST_SOUTH, 100,50 ); 
	rectangles.push_back ( r );
	
	r = { 0,25, 0, 15 };
	get_free_place (r, EAST_SOUTH, 100,50 ); 
	rectangles.push_back ( r );
	
	r = { 0,10, 0, 10 };
	get_free_place (r,NORTH_WEST, 100,50 ); 
	rectangles.push_back ( r );
	
	sl.clear();
	for ( auto x : rectangles ) {
		sl.push_back ( {x.left, x.right, x.bottom, x.top, true } );
	}
	
	for ( auto it = sl.begin(); it != sl.end (); it ++ ) {
		it->draw ();
	}
	
	fflag = false;
}


void window_manager_c::cut_rectangle3
	( int sx, int sy, list < rectangle_c>& rectangles, rectangle_c& rectangle ) 
{
	vector < rectangle_c > stack;
	rectangle_c r,r0,r2;
	
	r0.bottom = 0;
	for (;;) {
		r0.left = 0;
		r0.right = sx;
		r0.top = sy;
		for (auto i = rectangles.begin ();i != rectangles.end ();) {
			r = *i;
			if ( r.bottom <= r0.bottom && r.top > r0.bottom ) {
				if ( r.right <= r0.left ) {
				} 
				else if ( r.right < r0.right ) {
					if ( r.left <= r0.left ) {
						r0.left = r.right;
					}
					else {
						r2.left = r.right;
						r2.right = r0.right;
						stack.push_back ( r2 );
						r0.right = r.left;
					}
				}
				else if ( r.right >= r0.right ) {
					if ( r.left <= r0.left ) {
						r0.left = r0.right;
					}
					else if ( r.left < r0.right ) {
						r0.right = r.left;
					}
					else {
					}
				}
			}
			if ( r0.left == r0.right ) {
				if ( stack.empty ()) {
					break;
				}
				else {
					r0 = stack.back ();
					 stack.pop_back();
					i = rectangles.begin ();
					continue;
				}
			}
			i ++;
		}

		if ( r0.left != r0.right ) {
			break;
		}
		for ( auto r : rectangles ) {
			if ( r.top > r0.bottom  && r.top < r0.top ) {
				r0.top = r.top;
			}
		}
		if ( r0.bottom == r0.top ) {
			break;
		}
//		echo << "level: " << r0.bottom << endl;
		r0.bottom = r0.top;
	}
	
	r0.top = sy;
	for ( auto it = rectangles.begin (); it != rectangles.end (); it ++ ) {
		if ( it->bottom > r0.bottom) {
			if (  ( it->right > r0.left && it->right < r0.right ) 
				|| (it->left > r0.left && it->left < r0.right )
				|| (it-> left < r0.left && it->right > r0.right )) {
				
				if (  it->bottom < r0.top ) {
					r0.top = it->bottom;  
				}
			}
		}
	}
	rectangle = r0;	
}

bool window_manager_c::get_free_place ( rectangle_c& s, int gravity , int sx , int sy ){
	list< pair < int, int> > sl;
	pair < int, int > p;
	list < rectangle_c > nrs;
	rectangle_c ns = s;
	transform ( sx, sy, ns, gravity, false ); 
	ns.right = ns.right - ns.left;
	ns.left =  0;
	ns.top = ns.top - ns.bottom;
	ns.bottom = 0; 
	for ( auto x : rectangles ) {
		transform ( sx, sy, x, gravity, false );
		nrs.push_back ( x );
	}		
	rectangle_c rsxy = { 0, sx, 0, sy };
	transform ( sx, sy, rsxy, gravity, false );

//		echo << "cut_rectangle " << endl;
		rectangle_c r,rr;
		cut_rectangle3 ( rsxy.right, rsxy.top, nrs, r );
		rr.left = r.left;
		rr.right = r.left + min ( ns.right - ns.left, r.right - r.left );
		rr.bottom = r.bottom;
		rr.top = r.bottom + min ( ns.top - ns.bottom, r.top - r.bottom );
		ns = rr;
	
	transform ( rsxy.right, rsxy.top, ns, gravity, true );
	s = ns;
	return true;
}

void window_manager_c::set ( lamb_c& lamb, editor_c& o, rectangle_c r, int gravity )
{
		manage ( lamb, r , gravity );
		o.frame_width =  (r.width ()) /20;
		o.frame_height = r.height ()/20;
		o.mx = { { 0, 1, 0}, { 0, 0, -1 }, { 1, 0 , 0 }};
		o.vA = { 0, r.left, r.top };
//		lamb.local ( o, { r.left, r.top, 0}, {1, 0, 0}, {0, 1, 0});
} 

home_c::home_c ( lamb_c& lamb, string tapestry )
{
	list<object_c*> l;
	home ( tapestry, lamb, l );
}

editor_c* home_c::get_or_create_editor ( int number ) 
{
	lamb_c& lamb = *lamb_c::self;
	auto it = lamb.mobilar.begin ();
	editor_c* po = nullptr;
	int n = number;
	for ( ; it != lamb.mobilar.end ()  and n > 0; it++ ) {
		if ( (*it) -> get_type() == EDITOR  and  --n == 0 )  {
			po =  dynamic_cast < editor_c* > (*it );
			break;
		}
	}
	if ( po == nullptr ) {
		po = new editor_c ();
		po -> parent = &lamb;
		lamb.mobilar.push_back ( po );
		land.llist.push_back (po);
	}
	if ( number == initial_sensor ) {
		lamb.sensor = po;
		initial_sensor = 0;
	}
	string sd, s;
	stringstream ss;
	ss << ":mark toline am " << number;
	s = ss.str ();
	po -> command2 ( XK_Return, sd , s); 		

	return po;
}

mobilar_c::mobilar_c ( editor_c* _ed, int _x , int _y, int _direction ):
editor ( _ed ),
tag ( "" ),
x ( _x),
y ( _y),
direction ( _direction )
{
}

mobilar_c::mobilar_c (string _tag, int _x , int _y, int _direction ):
tag ( _tag ),
editor ( nullptr ),
x ( _x),
y ( _y),
direction ( _direction )
{
}

mobilar_c::mobilar_c ( const mobilar_c& m ) 
{
	x = m.x,
	y = m.y,
	tag = m.tag,
	editor = m.editor;
	direction = m.direction;	
}

home_c::home_c ()
{
}

void home_c::set ( initializer_list < mobilar_c > lst_m ) 
{
	int i_sensor = 0;
	lamb_c& lamb = *lamb_c::self;
	int c = mobilar_lst.size (); 
	for ( auto i = mobilar_lst.rbegin (); i != mobilar_lst.rend(); i++ ){
		if ( i->editor->impression_clients.has_focus ) {
//		if ( i ->editor == lamb.sensor ) {
			i_sensor = c;
		}
		if ( i ->tag == "editor" ) {
			lamb.automarks.push_mark ( *i ->editor , mark_c () );
			land.remove_object ( i ->editor );
			delete i->editor;
		}
		c --;
	}
	echo << "i sensor: " << i_sensor << endl;	
	mobilar_lst.clear ();
	for ( auto it = lst_m.begin (); it != lst_m.end() ; it++ ) {
		mobilar_lst.push_back ( *it );		
	}	
	window_manager_c& wm = lamb.window_manager;
	
	if ( initial_sensor != 0 ) {
		i_sensor = initial_sensor;
		initial_sensor = 0;
	}
	if ( i_sensor > mobilar_lst.size () ) {
		i_sensor = 0;
	}
	c = 1;
	for ( auto &x : mobilar_lst ) {
		editor_c*& e = x.editor;		
		if ( x.tag == "editor" ) {
			e = new editor_c();
			e->parent = &lamb;
			land.insert ( e );
			if ( i_sensor == 0 ) {
				lamb.sensor = e;
				e->impression_clients.has_focus=true;
				i_sensor = -1;
			}
		}
		if ( i_sensor == c ) {
			lamb.sensor = e;
			e->impression_clients.has_focus=true;
		}
		wm.set ( lamb, *e, { x.x, x.y }, x.direction );

		e->layen.resize ( 
			e->frame_width*e->layen.font.cell, 		
			(e->frame_height)*e->layen.font.cell,
			e->frame_height
		);
		c++;
	}	
	int number = 1;
	string sd, s;
	stringstream ss;
	for ( auto x : mobilar_lst ) {
		if ( x.tag == "editor" ) {
			string sd, s;
			stringstream ss;
			ss << ":mark toline am " << number;
			s = ss.str ();
			x.editor-> command2 ( XK_Return, sd , s); 		
			number ++;
			x.editor->layen.signo = 12;
		}
		x.editor->impression_clients.retina = &lamb.eyes.retina ;
		x.editor->draw();
	}
	lamb.status.layen.signo = 2;
}

void home_c::home ( string tapestry, lamb_c& lamb, list<object_c*> l )
{
	if ( tapestry == "" ) {
		tapestry = "default";
		lamb.home = tapestry;
	}
	echo << "home: " << tapestry << endl;
	
	window_manager_c& wm = lamb.window_manager;
	
	wm.rectangles.clear ();
	
	lamb.ears.layen.clear_pixel_vectors();
	lamb.status.layen.clear_pixel_vectors();
	lamb.mouth.layen.clear_pixel_vectors();
	
	lamb.ears.parent = &lamb;
	lamb.status.parent = &lamb;
	lamb.mouth.parent = &lamb;

	if ( tapestry == "t" ) {
		wm.test ( lamb, "");
		lamb.home = "white " ;
		return;
	}	
	else if ( tapestry == "lfs" ){
		set ( {				
			{ &lamb.mouth,  30*20, 3*20, EAST_SOUTH },		
			{ "editor",  30*20, 50*20 , EAST_SOUTH },
			{ &lamb.ears, 30*20, 53*20 ,WEST_SOUTH },
//			{ &lamb.status, 24*20, 14*20 , NORTH_WEST }
		} );
		lamb.ears.parent = &lamb;
		lamb.status.parent = &lamb;
	}

	else if ( tapestry == "draw" ){
		set ( {				
			{ &lamb.mouth,  40*20, 3*20, EAST_SOUTH },		
			{ "editor",  40*20, 50*20 , EAST_SOUTH },
			{ &lamb.ears, 30*20, 30*20 ,WEST_SOUTH },
			{ &lamb.status, 10*20, 10*20 , NORTH_WEST }
		} );
		lamb.ears.parent = &lamb;
		lamb.status.parent = &lamb;
	}
	else if ( tapestry == "draw2" ){
		set ( {				
			{ &lamb.mouth,  40*20, 3*20, EAST_SOUTH },		
			{ "editor",  40*20, 50*20 , EAST_SOUTH },
			{ &lamb.ears, 30*20, 20*20 ,WEST_SOUTH },
			{ &lamb.status, 24*20, 15*20 , NORTH_WEST }
		} );
		lamb.ears.parent = &lamb;
		lamb.status.parent = &lamb;
	}
	else if ( tapestry == "man" ){
		set ( {				
			{ &lamb.mouth,  30*20, 3*20, EAST_SOUTH },		
			{ "editor",  30*20, 50*20 , EAST_SOUTH },
			{ &lamb.ears, 45*20, 53*20 ,WEST_SOUTH },
//			{ &lamb.status, 24*20, 14*20 , NORTH_WEST }
		} );
		lamb.ears.parent = &lamb;
		lamb.status.parent = &lamb;
	}
	else if ( tapestry == "cpp2" ){
		set ( {				
			{ &lamb.mouth,  40*20, 3*20, EAST_SOUTH },		
			{ "editor",  40*20, 50*20 , EAST_SOUTH },
			{ &lamb.ears, 30*20, 53*20 ,WEST_SOUTH },
//			{ &lamb.status, 24*20, 14*20 , NORTH_WEST }
		} );
		lamb.ears.parent = &lamb;
		lamb.status.parent = &lamb;
	}
	else if ( tapestry == "cpp3" ){
		set ( {				
			{ &lamb.mouth,  35*20, 3*20, EAST_SOUTH },		
			{ "editor",  35*20, 50*20 , EAST_SOUTH },
			{ &lamb.ears, 30*20, 53*20 ,WEST_SOUTH },
//			{ &lamb.status, 24*20, 14*20 , NORTH_WEST }
		} );
		lamb.ears.parent = &lamb;
		lamb.status.parent = &lamb;
	}
	else if ( tapestry == "tex" ) {
			set ( {
			{ &lamb.mouth, 50*20, 3*20, EAST_SOUTH },
			{ "editor", 50*20, 60*20, EAST_SOUTH },
//			{ &lamb.status, 10*20, 10*20, NORTH_WEST },
			{ &lamb.ears, 0*20, 0*20, WEST_SOUTH },
		});	
		lamb.ears.parent = &lamb;
		lamb.status.parent = &lamb;
	}
	else if ( tapestry == "tex2" ) {
			set ( {
			{ &lamb.mouth, 30*20, 3*20, EAST_SOUTH },
			{ "editor", 30*20, 60*20, EAST_SOUTH },
//			{ &lamb.status, 10*20, 10*20, NORTH_WEST },
			{ &lamb.ears, 0*20, 0*20, WEST_SOUTH },
		});	
		lamb.ears.parent = &lamb;
		lamb.status.parent = &lamb;
	}
	else if ( tapestry == "tex3" ) {
			set ( {
			{ &lamb.mouth, 30*20, 3*20, EAST_SOUTH },
			{ "editor", 30*20, 60*20, EAST_SOUTH },
//			{ &lamb.status, 10*20, 10*20, NORTH_WEST },
			{ &lamb.ears, 30*20, 20*20, WEST_NORTH},
		});	
		lamb.ears.parent = &lamb;
		lamb.status.parent = &lamb;
	}
	else if ( tapestry == "new" ){
		set ( {
			{ &lamb.ears, 40*20, 60*20, EAST_SOUTH },
			{ "editor", 40*20, 60*20, EAST_SOUTH },
			{ &lamb.status, 10*20, 10*20, NORTH_WEST },
			{ &lamb.mouth, 40*20, 3*20, EAST_SOUTH },
		});	
	}
	else if ( tapestry == "ear" ){
		set ( {
			{ &lamb.mouth, 40*20, 4*20, EAST_SOUTH },
			{ "editor", 40*20, 60*20, EAST_SOUTH },
			{ &lamb.ears, 40*20, 60*20, EAST_SOUTH },
			{ &lamb.status, 10*20, 10*20, NORTH_WEST }
		});	
	}
	else if ( tapestry == "ear2" ){
		set ( {
			{ &lamb.mouth, 40*20, 4*20, EAST_SOUTH },
			{ "editor", 40*20, 60*20, EAST_SOUTH },
			{ &lamb.ears, 40*20, 4*20, EAST_SOUTH },
			{ &lamb.status, 10*20, 10*20, NORTH_WEST }
		});	
	}
	else if ( tapestry == "dual" ){
		set({ 
			{ &lamb.mouth, 40*20, 3*20, EAST_SOUTH },
			{ "editor", 40*20, 50*20, EAST_SOUTH },
			{ &lamb.ears, 40*20, 4*20, EAST_SOUTH },
			{ "editor", 40*20, 50*20, EAST_SOUTH },
			{ &lamb.status, 10*20, 10*20, NORTH_WEST }
		});
	}	
	else if ( tapestry == "school" ){
		set({ 
			{ &lamb.mouth, 40*20, 4*20, EAST_SOUTH },
			{ "editor", 40*20, 50*20, EAST_SOUTH },
			{ &lamb.ears, 40*20, 8*20, EAST_SOUTH },
			{ "editor", 40*20, 50*20, EAST_SOUTH },
			{ &lamb.status, 10*20, 10*20, NORTH_WEST }
		});
	}	
	else if ( tapestry == "debug" ) {
		set({ 
			{ &lamb.mouth, 40*20, 3*20, EAST_SOUTH },
			{ "editor", 40*20, 50*20, EAST_SOUTH },
			{ &lamb.ears, 40*20, 8*20, EAST_SOUTH },
			{ "editor", 40*20, 50*20, EAST_SOUTH },
			{ &lamb.status, 10*20, 10*20, NORTH_WEST }
		});
		lamb.ears.parent = &lamb;
		lamb.status.parent = &lamb;
	}
	else if ( tapestry == "bare" ){
		set ( {
			{ &lamb.mouth, 40*20, 3*20, EAST_SOUTH },
			{&lamb.ears, 40*20, 20*20, EAST_SOUTH },
			{&lamb.status, 10*20, 10*20, NORTH_WEST }
		} );	
		lamb.ears.parent = &lamb;
		lamb.status.parent = &lamb;
	}
	else if ( tapestry == "default" ){
		set ( {	
			{ &lamb.mouth,   40*20, 3*20 , EAST_SOUTH },		
			{ "editor",   40*20, 48*20  , EAST_SOUTH },
			{ &lamb.ears,   30*20, 24*20  , SOUTH_WEST },
			{ &lamb.status,   32*20, 24*20  , SOUTH_WEST }
		} );
		lamb.ears.parent = &lamb;
		lamb.status.parent = &lamb;
	}
}

namespace exhome{
void entry ( int a, string s, void* pv )
{

	lamb_c& lamb = *lamb_c::self;
	window_manager_c wm;
	string str;
	if ( a == 0 ) {
		str = "delete";
	}
	else {
		str = "new";
	}
	echo << str << endl;
	wm.test ( lamb, str );
//	echo << "exhome" << endl;
	return;
}

} //end namespace exhome

void ex_entry_home (int a, string s, void* pv )
{
	exhome::entry ( a, s, pv );	
}

/*
void window_manager_c::transform ( int sx, int sy,  rectangle_c& r, int gravity , int revert  = NONE )
{
	echo << "NONE:" << NONE << endl;
	rectangle_c s;
	if ( gravity == SOUTH_WEST ) {
		s = r;
	}
	else if ( gravity == EAST_SOUTH ) {
		if ( revert ) {
			return transform ( sx, sy, r, WEST_NORTH, false );
		}
		else {
			s.right = r.top;		
			s.left = r.bottom;
			s.top = sx - r.left;
			s.bottom = sx - r.right;
		}
	}
	else if ( gravity == WEST_NORTH ) {
		if ( revert ) {
			return transform ( sx, sy, r, EAST_SOUTH, false );
		}
		else {
			s.right = sy - r.bottom;
			s.left = sy - r.top;
			s.top = r.right;
			s.bottom = r.left;
		}
	}
	else if ( gravity == SOUTH_EAST ) {
		if ( revert ) {
			return transform ( sx, sy, r, SOUTH_EAST, false );
		}
		else {
			s.right = sx - r.left;
			s.left = sx - r.right;
			s.top = r.top;
			s.bottom = r.bottom;
		}
	}
	else if ( gravity == NORTH_WEST ) {
		if ( revert ) {
			return transform ( sx, sy, r, NORTH_WEST, false );
		}
		else {
			s.bottom = sy - r.top;
			s.top = sy - r.bottom;
			s.right =  r.right;
			s.left = r.left;
		}
	}
	
	else if ( gravity == WEST_SOUTH ) {
		if ( revert ) {
			return transform ( sx, sy, r, WEST_SOUTH, false );
		}
		else {
			s.bottom = r.left;
			s.top = r.right;
			s.right =  r.top;
			s.left = r.bottom;
		}
	}
	else if ( gravity == NORTH_EAST ) {
	
		if ( revert ) {
			return transform ( sx, sy, r, NORTH_EAST, false );
		}
		else {
			s.bottom = sy - r.top;
			s.top = sy - r.bottom;
			s.right =  sx - r.left;
			s.left = sx - r.right;
		}
	}
	else if ( gravity == EAST_NORTH ) {
		if ( revert ) {
			return transform ( sx, sy, r, EAST_NORTH, false );
		}
		else {
			s.bottom = sx - r.right;
			s.top = sx - r.left;
			s.right =  sy - r.bottom;
			s.left = sx - r.top;
		}
	}
	r = s;
	return;
}
*/

