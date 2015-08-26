#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>


#include "library/wdslib.h"
#include "mstream.h"

#include "symbol/keysym.h"
#include "symbol.h"
#include "gate.h"
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
#include "land.h"
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

extern land_c land;

using namespace std;

const unsigned long spline_c::type = SPLINE;

void  spline_c::dump ()
{
	echo << "dump spline" << endl;
	echo << "order: " << order << endl;
	echo << "uniform: " << uniform << endl;
	echo << "open_uniform: " << open_uniform << endl;
	echo << "close: " << close << endl;
	echo << "color: " << color << endl;
	echo << "doted: " << doted << endl;
	echo << "control points: " << control_points.size () << endl;
	echo << "active point: " << active_point << endl;
	
	vector<matrix_c<FT>>::iterator it;
	if ( active_point < control_points.size ()){
		it = control_points.begin ();
		it = next ( it, active_point );
		matrix_c<FT> m = *it;
		stringstream ss;
		m.out ( 3, ss );
		echo << ss.str () << endl;
	}
}

spline_c* spline_c::create ()
{
    return new spline_c ();
}

unsigned long spline_c::get_type ()
{
	return type;
}

spline_c::spline_c ( const spline_c& s ) 
{
	control_points = s.control_points;
	order = s.order;
	uniform = s.uniform;
	open_uniform = s.open_uniform;
	close = s.close;	
	color = s.color;
	doted = s.doted;
	accept_points = false;	
	transcient = false;
}

spline_c::spline_c ( spline_c& s )
{
	control_points = s.control_points;
	order = s.order;
	uniform = s.uniform;
	open_uniform = s.open_uniform;
	close = s.close;	
	color = s.color;
	doted = s.doted;
	accept_points = true;	
	transcient = false;
}

spline_c::spline_c ( int left, int right, int bottom, int top, bool fill )
{
	int dx = -200;
	int dy = 200;
	left = left + dx;
	right =right + dx;
	bottom = bottom + dy;
	top = top + dy;
	lamb_c&  lamb = *lamb_c::self;
	FT ft;
	matrix_c <FT> m1, m2, m3, m4;
	m1 = {left , bottom, 0 };
	m2 = { left, top, 0 };
	m3 = { right, top, 0 };
	m4 = { right, bottom, 0};
	m1 = lamb.view_to_world ( m1 );	
	m2 = lamb.view_to_world ( m2 );	
	m3 = lamb.view_to_world ( m3 );	
	m4 = lamb.view_to_world ( m4 );	
	if (fill ) {
		control_points.push_back ( m1 ); 
		control_points.push_back ( m2 ); 
		control_points.push_back ( m3 ); 
		control_points.push_back ( m4 );
		control_points.push_back ( m1 ); 
		control_points.push_back ( m3 ); 
//		motion.set_object_vector ( 1,{0, 0, 0});
//		motion.set_object_vector ( 2,{1, 0, 0});
//		motion.set_object_vector ( 3,{0, 1, 0});
		active_point = 0;
		close = false;
		open_uniform = false;
		uniform = true;
	}
	else {
		control_points.push_back ( m1 ); 
		control_points.push_back ( m2 ); 
		control_points.push_back ( m3 ); 
		control_points.push_back ( m4 ); 
		motion.set_object_vector ( 1,{0, 0, 0});
		motion.set_object_vector ( 2,{1, 0, 0});
		motion.set_object_vector ( 3,{0, 1, 0});
		active_point = 0;
		close = true;
		open_uniform = true;
		uniform = true;
	}
	order = 1;
	accept_points = false;
	doted = false;
	color = 0x00;
	transcient = false;
}

object_c* spline_c::duplicate ()
{
	cout << "duplicate spline" << endl;
	return new spline_c(*this );	
}

void spline_c::serialize ( fstream &file, bool bsave )
{
	if ( bsave ){
		file << "SPLINE" << endl;
	}
	motion.serialize ( file, bsave );
	if(!bsave ){
		uint32_t n;
		file >> n; 
		control_points.clear ();
		for(; n>0; n--){
			matrix_c<FT> m;		
			m.serialize ( file, bsave );
			control_points.push_back ( m );
		}
		file >> order;
		file >> uniform;
		file >> open_uniform;
		file >> close;
		file >> color;
		file >> doted;
	}
	else{
		file << (uint32_t )control_points.size () << endl;
		vector<matrix_c<FT>>::iterator it = control_points.begin ();
		for(;it!= control_points.end ();it++){
			it->serialize ( file, bsave );
		}
		file << order << endl;
		file << uniform << endl;
		file << open_uniform << endl;
		file << close << endl;
		file << color << endl;
		file << doted << endl;
	}
}

bool spline_c::find ( lamb_c &lamb, matrix_c<FT> pt )
{
	line_c line;
	for ( int c = 0; c < control_points.size () ; c++){
		matrix_c<FT> point = control_points[c];
		matrix_c<FT> m = {0, 0, 0};
		line = line_c ( point, m,.1, 0x00ff00, false );	
		if ( line.find ( lamb, pt )){
			return true;
		}
	}
	return false;
}


void spline_c::shift ( matrix_c<FT>& v )
{
	echo << "spline shift" << endl;
	for ( int c = 0; c < control_points.size (); c++){
		control_points[c] = control_points[c] + v;
	}
}

void spline_c::shift3 ( matrix_c<FT>& v )
{
	echo << "spline shift3" << endl;
	for ( int c = 0; c < control_points.size (); c++){
		if ( active_point == control_points.size () || active_point == c ){
			control_points[c] = control_points[c] + v;
		}
	}
}

int spline_c::command2 ( unsigned short c, string& v, string& s )
{
	echo << "spline command" << endl;
	
	cout << "spline command" << endl;
	stringstream ss ( s );
	string s0, s1, s2 , s3;	
	ss >> s0;
	if ( s0 == "ord" ){
		ss  >> order;
		return true;
	}
	if ( s0 == "color" ){
		echo << "spline color" << endl;
		ss >> hex >> color;
		return true;
	}
	if ( s0.substr ( 0, 5 ) == "point" ){
		string is = "";
		FT ft;
		
		int ap = active_point;
		if ( s0.size () == 6 ){
			is = s0.substr ( 5, 1 );
		}
		if ( is == "x" || is == "" ){
			ss >> ft;
			control_points[ap][1] = ft + control_points[ap][1]; 
		}
		if ( is == "y" || is == "" ){
			ss >> ft;
			control_points[ap][2] = ft + control_points[ap][2]; 
		}
		if ( is == "z" || is == "" ){
			ss >> ft;
			control_points[ap][3] = ft + control_points[ap][3]; 
		}
	}
}

int spline_c::set_point ( lamb_c& lamb )
{
	cout << "set_point" << endl;
	if ( accept_points == false ){
		return false;
	}
	matrix_c<FT> m = lamb.hand.pointer;
	m = lamb.view_to_world ( m );	
	m = land.rasterize ( m );
	control_points.push_back ( m );
}

int spline_c::edit ( lamb_c& lamb, keyboard_c& keyb )
{
	string c = keyb.get_char ();
	int v = keyb.get_stroke ();
	unsigned long control = keyb.get_controls ();
	vector<matrix_c<FT>> &p = control_points;
	if ( v == XK_space ){
		if ( active_point < p.size ()){
			active_point++;
		}
		else{
			active_point = 0;
		}
	}
	else if (v == XK_n ){
		matrix_c<FT> pA, pB, pC;
		if( active_point == p.size ()) {
			pA = p.back ();
			pB = p.front ();
			pC = .5*(pA + pB );
			p.insert ( p.begin (), pC );
			active_point++;
		}
		else {
			matrix_c<FT> pA = p[active_point];
			matrix_c<FT> pB;
			if (active_point < p.size () - 1 ){
				pB = p[active_point + 1];
			}
			else {
				pB = p[0];
			}
			pC = .5*(pA + pB );
			if ( active_point == p.size ()-1 ){
				p.push_back ( pC );
				active_point++;
			}
			else {
				vector<matrix_c<FT>>::iterator it = p.begin ();
				for( int c = 0; c <= active_point; c++){
					it++;
				}
				p.insert ( it, pC );
				active_point++;
			}
		}
	}
	if( v == XK_Escape ){
		accept_points = false;
	}
	if( v == XK_o ){
		open_uniform = !open_uniform;
	}
	if( v == XK_c ){
		close = !close;
	}
	if( v == XK_x ){
		if ( active_point < control_points.size ()){
			vector<matrix_c<FT>>::iterator it = control_points.begin ();
			for ( int c = 0; c < control_points.size (); c++){
				if( c == active_point ){
					control_points.erase ( it );
					break;
				}
				it++;
			}
		}
	}
	if( v == XK_i ){
		dump ();
	}
	return 0;
}

float spline_c::basis_B_spline ( float u, int i, int k, vector<float>& t )
{
	int m = t.size ();
	if ( m == 0 || u < t[0] || u > t[m-1] ){
		return -1;
	}
	if( i >= m - k - 1 ){
		return -1;
	} 
	/*
	vector<float> b ( m - 1 );
	for ( int c = 0; c < m - 1; c++){
		if( u >= t[c] && u < t[c+1]){
			b[c] = 1;
		}
		else {
			b[c] = 0;
		}
	}
	*/
	vector<float> b ( m - 1, 0 );
	for ( int c = 0; c < m - 1; c++){
		if ( c < k ){
			if ( u >= t[c] && u < t[c+1] ){
				b[c] = 1;
				break;
			}
		}
		else if ( c == k ){
			if( u >= t[c] && u <= t[c+1]){
				b[c] = 1;
				break;
			}
		}
		else {
			if( u > t[c] && u <= t[c+1]){
				b[c] = 1;
				break;
			}
		}
	}
	for ( int j = 1; j <= k; j++){
		for ( int c = 0; c <= m - j - 1; c++){
			float d0;
			if ( t[c + j] - t[c] != 0 ){
				b[c] = (u - t[c])/(t[c + j] - t[c])*b[c];
			}
			else{
				b[c] = 0;
			}
			if ( t[c + j + 1] - t[c + 1] != 0 ){
				b[c] += (t[c + j + 1] - u )/(t[c + j + 1] - t[c + 1])*b[c + 1];
			}
		}
	}	
	return b[i];
}

bool spline_c::B_spline ( float u,  vector<float>& t, int k, vector<matrix_c<FT>>& p, matrix_c<FT>& s ){
		s = matrix_c<FT>(3, 1 );
		for ( int c = 0; c < p.size (); c ++){
			float b = basis_B_spline ( u, c, k, t );
//			cout << b << endl;
			s = s + b*p[c]; 
		}	
	return true;
}

void spline_c::draw_B_spline ( lamb_c& lamb )
{
	segments.clear();
	controls.clear();
	knots = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int k = order;
	vector<matrix_c<FT>> p = control_points;
	if ( close ){
		for ( int c = 0; c <= k; c++){
			p.push_back ( p[c]);
		}
	}
	vector<float>& t = knots;
	int m = p.size ();
	
	if ( uniform ){
		t.clear ();
		t.resize ( 0 );
		int i = 0;
		for ( int c = 0; c < m + k + 1; c++){
			t.push_back ( i );
			if ( c >= k && c < m || !open_uniform ){
				i++;
			}
		}
	}	
	/*
	for (auto x : t ){
			echo << x << ",";
	}
	echo << endl;
	*/
	matrix_c<FT> pa, pb, vb;
	float d;
	if( k == 1 ){
		d = 1;
	}
	else {
		d = .1;
		d = (t[t.size () - 1] - t[0])/20.0;
	}
	float u = t[k];
	B_spline ( u, t, k, p, pb );
	u += d;
	for(; u <= t[m]; u += d ){
		pa = pb;
		if( B_spline ( u, t, k, p, pb ) ){
			segments.push_back ( line_c ( pa, vb = pb - pa, .01, color ));
			if ( transcient ){
//				segments.back ().draw_transcient ( lamb );
			}
			else{
				segments.back ().draw ( );
			}
		}
	}
	if ( state == 1 || this == lamb.sensor ){
		int size = p.size ();
		if ( close ){
			size = size - k - 1;
		}
		for ( int c = 0; c < size; c++){
			uint32_t col_= 0x00;
			if ( c == active_point || active_point == size ){
				col_ = 0xff;
			}
			else {
				col_ = state == 1 ? 0xff0000:0x0;
				col_ |= this == lamb.sensor ? 0xff00:0x0; 
			}
			matrix_c<FT> f = {0, 0, 0};
			controls.push_back ( line_c ( p[c], f, 4, col_, false ) );
			if ( transcient ){
//				segments.back ().draw_transcient ( lamb );
			}
			else{
				controls.back ().draw ( );
			}
		}
	}
	return;
}

void spline_c::draw ( )
{
	draw_B_spline ( *lamb_c::self );
	return;
}

spline_c::spline_c ()
{
	motion.set_object_vector ( 1,{0, 0, 0});
	motion.set_object_vector ( 2,{1, 0, 0});
	motion.set_object_vector ( 3,{0, 1, 0});
	active_point = 0;
	close = false;
	open_uniform = true;
	uniform = true;
	order = 1;
	accept_points = false;
	doted = false;
	color = 0x00;
	transcient = false;
}

spline_c::~spline_c ()
{
	//mout << "spline dest\n";
}