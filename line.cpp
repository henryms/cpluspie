#include <math.h>
#include <cstdio>
#include <cassert>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>

#include "symbol/keysym.h"
#include "symbol.h"
#include "library/wdslib.h"
#include "gate.h"
#include "mstream.h"
#include "matrix.h"
#include "position.h"
#include "sens/retina.h"
#include "sens/spirit.h"
#include "message.h"
#include "global.h"
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
#include "echo.h"
#include "bookmarks.h"
#include "hand.h"
#include "lamb.h"


extern int flag1;
extern land_c land;
extern long flag;

const unsigned long line_c::type = LINE;

line_c* line_c::create ()
{
    return new line_c ();
}

unsigned long line_c::get_type ()
{
	return type;
}

bool line_c::find ( lamb_c &lamb, matrix_c<FT> point )
{
	matrix_c<FT> b, v;
	lamb.matrices_to_view (*this, &b, &v );
	
	matrix_c<FT> v1 = ~b*(vA-v );
	matrix_c<FT> v2 = ~b*(vA+vb-v );
	return seek ( point, v1, v2, 10 );
}

void line_c::serialize ( fstream &file, bool bsave )
{
	if ( bsave ){
		file << "LINE" << endl;
	}
	vA.serialize ( file, bsave );
	vb.serialize ( file, bsave );
	vx.serialize ( file, bsave );
	motion.serialize ( file, bsave );

	if (!bsave ){
		file >> color;
		file >> doted;
	}
	else{
		file << color << endl;
		file << doted << endl;
	}
}

bool line_c::seek ( matrix_c<FT> p0, matrix_c<FT> p1, matrix_c<FT> p2, long h )
{
	long x, y, x1, x01, y1, y01, x2, x02, y2, y02;
	x = p0.l[0]; y = p0.l[1];
	if (p1.l[0]<= p2.l[0]){
		x01 = p1.l[0];
		x02 = p2.l[0];
	}
	else {
		x01 = p2.l[0];
		x02 = p1.l[0];
	} 
	if (p1.l[1]<= p2.l[1]){
		y01 = p1.l[1];
		y02 = p2.l[1];
	}
	else{
		y01 = p2.l[1];
		y02 = p1.l[1];
	} 
	x1 = p1.l[0]; y1 = p1.l[1]; x2 = p2.l[0]; y2 = p2.l[1];
	if (x>= x01-h && x<= x02+h && y>= y01-h && y<= y02+h &&
	 x*(y2-y1 )-y*(x2-x1 )+y1*x2-x1*y2+h*(fabs ((float )y2-y1 )+fabs ((float )x2-x1 ))>= 0 &&
	 x*(y2-y1 )-y*(x2-x1 )+y1*x2-x1*y2-h*(fabs ((float )y2-y1 )+fabs ((float )x2-x1 ))<= 0 ){
		return true;
	}
	return false;
}

matrix_c<FT> line_c::object_to_world ( matrix_c<FT>& pt ){
	matrix_c<FT> point = pt;
	matrix_c<FT> base = motion.object_base ();
	point = base*point;
	matrix_c<FT> vA = motion.object_vector ( 1 );
	point = point+vA;
	return point;
}

matrix_c<FT> line_c::world_to_object ( matrix_c<FT>& pt ){
	
	matrix_c<FT> point = pt;
	matrix_c<FT> vA = motion.object_vector ( 1 );
	point = point-vA;
	matrix_c<FT> base = motion.object_base ();
	point = ~base*point;
	return point;
}

void line_c::draw_transcient ( lamb_c& lamb ){
	transcient = true;
	draw (  );
	transcient = false;
}

void line_c::copy ( line_c& l )
{
	vA = l.vA;
	vb = l.vb;
	vx = l.vx;
	doted = l.doted;
	color = l.color;
	motion = l.motion;
}

void line_c::draw ( )
{
	lamb_c& lamb = *lamb_c::self;
	static uint64_t count = 0;
	if ( count < 10 ) {
//		echo << ui << endl;
	}
	memory_c* memory;
	object_c* tthis;
	if ( transcient ){
		assert (false );
		tthis = 0;
	}
	else {
		lamb.eyes.retina .get_memory ( ui, & impression_clients,  &memory  );
		lamb.eyes.retina .erase_memories ( ui );
//			cout << "mem: " << memory << endl;
	}
	list<pair<float, uint32_t>>::iterator rit;
	matrix_c<FT> eb, ev, iv, ib, zb;
	lamb.matrices_to_view (*this,&eb,&ev );
	float zzz = lamb.eyes.retina.surfaces.front().zoom;
	float zz = lamb.eyes.dpm*zzz;
//	float zz = lamb.eyes.dpm*lamb.eyes.zoom;
	
	iv=(-1 )*ev;
	zb = zz*~eb;	
	
	float d;
	matrix_c<FT> mx;
	int n;
	if ((vb|vb ) == 0 ){
		mx={{1, 0, 0},{0, 1, 0},{0, 0, 1}};		
		d = sqrt ( vx|vx );
		n = 0;
	}
	else{
		matrix_service_c<FT> mxsrv;
		FT npx = 0;
		mx = mxsrv.ortho3 ( vb, vx,&npx, 0, 0 );
		d = npx;
		n = sqrt ( vb|vb );
	}
	float k = 1.5;
	int long ncolor = color;
	if ( this == lamb.sensor ){
		lamb.eyes.grip ( zb*(vA+iv ), 0x00ff00 );
	}
	else if ( state == 1 ){
		lamb.eyes.grip ( zb*(vA+iv ), 0xff0000 );
	}	
	if ( vx|vx ){
		float pi2 = 2*3.1416;
		matrix_c<FT> vec ( 3, 1 ), vector1 ( 3, 1 ), vector2 ( 3, 1 ), vector3 ( 3, 1 );
//		float d;
//		matrix_c<FT> mxx = vx-((vx|vb )/(vb|vb ))*vb;
//		d = sqrt ( mxx|mxx );
		float c, e, g, h;
		c = pi2*d;
		for ( float f = 0; f < c*zz/2; f++) {
			e = d*cos ( f/(d*zz ));
			g = d*sin ( f/(d*zz ));	
			h = pi2*e;
			for ( float a = 0; a<h*zz; a++) {
				vector1[1]=-g;	
				vector1[2]= e*cos ( a/(e*zz ));
				vector1[3]= e*sin ( a/(e*zz ));
				vector2 = mx*vector1;
				vector2 = vector2+vA;
				vec = zb*(vector2+iv );
				lamb.eyes.retina .set_pixel ( *memory, vec[1], vec[2], vec[3], ncolor );					vector1[1]= g+n;
				vector2 = mx*vector1;
				vector2 = vector2+vA;
				vec = zb*(vector2+iv );
				lamb.eyes.retina .set_pixel ( *memory, vec[1], vec[2], vec[3], ncolor );				}
		}
		c = pi2*d;
		vector1[1]= 0;
		for ( float a = 0;a<c*zz*k;a++){
			vector1[2]= d*cos ( a/(d*zz*k ));
			vector1[3]= d*sin ( a/(d*zz*k ));
			vector2 = mx*vector1;
			vector2 = vector2+vA;
			float nzzk = n*zz*k;
			float zzk = zz*k;
			
			for ( int c = 0;c<= nzzk;c++){
				//vector3 = vector2+(c/(n*zz*k ))*vb;
				int modulo=(int )(c/(zzk ))%20;
				if ( modulo == 0 || !doted ){
					vector3 = vector2+(c/(nzzk ))*vb;
					vec = zb*(vector3+iv );
					lamb.eyes.retina .set_pixel ( *memory, vec[1], vec[2], vec[3], ncolor );				}
			}
			/*
			for ( int c = 0;c<= n*zz*k;c++){
				//vector3 = vector2+(c/(n*zz*k ))*vb;
				int modulo=(int )(c/(zz*k ))%20;
				if ( modulo == 0 || !doted ){
					vector3 = vector2+(c/(n*zz*k ))*vb;
					vec = zb*(vector3+iv );
					lamb.eyes.set_pixel (&rit, vec[1], vec[2], vec[3], ncolor );
				}
			}
			*/
		}
	}

}

void line_c::setpoint ( lamb_c& lamb, bool clear, bool _3points, bool automatic, bool create )
{
	matrix_c<FT> b, v, p;
	lamb.matrices_to_view (*this,&b,&v );
	p = lamb.hand.pointer;
	echo << "set point\n";
	static int count = 0;
	if ( clear ){
		count = 0;
		return;
	}
	matrix_c<FT> vec= b*p+v;
	if ( count == 0 ) {
		vA = vec; 
		vb = 0*vb;
		vx = 0*vx;
		++count;
	}	
	else if ( count == 1 ) { 
		vx = 0*vx;
		vA = vA+vb;
		vb = vec-vA;
		if (!_3points ) {
			matrix_c<FT> vectorb=~b*(vb-v );
			matrix_c<FT> vectorz ({0, 0, 1});
			matrix_c<FT> mxs={vectorb, vectorz};
			matrix_c<FT> vectorc = mxs.surface ().get_column ( 3 );
			vx = b*vectorc+v;
			vx = 1/(vx||vx )*vx;
			if ( create ){
				line_c* pline = new line_c;
				pline->vA = vA;
				pline->vb = vb;
				pline->vx = vx;
				pline->color = color;
				land.llist.push_front ( pline );
			}
			if ( automatic ){
			}
			else{
				count = 0;
			}
		}
		else{
			++count;
		}
	}
	else if ( count == 2 ){
		vx = vec-vA;
		if ( create ){
		}
		if ( automatic ){
			count = 1;
		}
		else{
			count = 0;
		}
	}
}

void line_c::edit ( lamb_c& lamb, message& m_sense )
{
}

line_c::line_c (): vA ( 3, 1 ), vb ( 3, 1 ), vx ( 3, 1 )
{
	ui = die();
	motion.set_object_vector ( 1,{0, 0, 0});
	motion.set_object_vector ( 2,{1, 0, 0});
	motion.set_object_vector ( 3,{0, 1, 0});

	doted = false;
	color = 0x000000;
	transcient = false;
}

line_c ::line_c ( matrix_c<FT>& v )
{
	ui = die();
}

line_c::line_c ( const line_c& line )
{
	motion = line.motion;
	doted = line.doted;
	color = line.color;
	vA = line.vA;
	vb = line.vb;
	vx = line.vx;
	transcient = line.transcient;
	ui = die();
}

line_c::line_c ( matrix_c<FT>& _vA, matrix_c<FT>& _vb, FT r, uint32_t color_, int doted_ )
{
	motion.set_object_vector ( 1,{0, 0, 0});
	motion.set_object_vector ( 2,{1, 0, 0});
	motion.set_object_vector ( 3,{0, 1, 0});

	vA = _vA;
	vb = _vb;
	if ( vb[1] == 0 && vb[2]== 0 && vb[3]== 0 ){
		vx= matrix_c<FT> (3, 1 ) ;
		vx={r, 0, 0};
	}
	else{
		matrix_service_c<FT> m_s;
		vx = m_s.vx3 ( vb, r );
	}
	doted = doted_;
	color = color_;
	transcient = false;
	ui = die();
}

line_c::~line_c ()
{
//	cout << "line destruction\n";
	if ( impression_clients.retina != 0 ) {
		impression_clients.retina->unregister ( ui );				
	}
} 
