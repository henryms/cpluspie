#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

#include "debug.h"
#include "echo.h"
#include "mstream.h"
#include "gate.h"
#include "matrix.h"
#include "position.h"
#include "retina.h"
#include "message.h"
#include "object.h"

#include "symbol/keysym.h"
#include "symbol.h"

#include "land.h"

extern land_c land;

memory_client_c::memory_client_c(): object ( 0 )
{
}

surface_c::surface_c ():
frame ( 0 ),
is_valide ( false )
{
	mx = { { 0, 1, 0}, { 0, 0, 1} , { - 1, 0, 0 }};
	vA = { 0, 0 ,0 };
}

surface_c::~surface_c ()
{
/*
	assert ( parent != 0);
	for ( auto memories : parent -> memories_list ){
		memories -> remove ( this );
	}
	delete frame;
*/	
}

matrix_c<FT> surface_c::world_to_view ( matrix_c<FT>& pt ){
	
	matrix_c<FT> point = pt;
	matrix_c<FT> vA = motion.object_vector ( 1 );
	point = point-vA;
	matrix_c<FT> base = motion.object_base ();
	point = ~base*point;
//	point = point-local_position;
	point = ~mx*point;
	return point;
}

void surface_c::initialize ( int x, int y, uint32_t color )
{
	assert ( frame == 0 );
	frame = new uint32_t [ x*y ];
	cout << "surface init: " << x << ":" << y << '\n';
	uint32_t* frame_xy = frame;
	v.resize ( y );
	for ( int cy = 0 ; cy < y ; cy++ ){
		v [ cy ].resize ( x );
		for (int cx = 0; cx < x ; cx++ ){
			v [ cy ] [ cx ].zpix.push_front ( {10000, color} );
			v [ cy ] [ cx ].frame_xy = frame_xy;
			*frame_xy = color;
			frame_xy++;
		}
	}
}

memory_c::memory_c ()
{
}

memory_c::memory_c ( list < surface_c>::iterator _surface ):
surface ( _surface )
{
}

memory_c::memory_c ( const memory_c& m)
{
	surface = m.surface;
}

void memory_c::disapear ()
{
	int c = 0;
	for ( auto& p : l ) {
		auto zl = p.first;
		auto zi = p.second;
		zl -> zpix.erase ( zi ); 
		*( zl -> frame_xy ) = zl -> zpix.begin () -> second;		
		c++;
	}
//	cout << "disapear count: " <<  c << endl;

	l.resize ( 0 );
}

retina_c::retina_c () 
{
	is_invalide = false;
}

retina_c::~retina_c () 
{
	for ( auto& x : memory_clients ){
		x. object->retina = 0;
	}
}

void retina_c::enregister (uint64_t ui )
{
	memory_clients.push_front ( memory_client_c() );
	handle_to_list.insert ( make_pair ( ui, memory_clients.begin() ));
}

void retina_c::enregister (uint64_t ui, impression_clients_c* ics )
{
	if (handle_to_list.find ( ui ) != handle_to_list.end () ) {
		return;
	}
	memory_clients.push_front ( memory_client_c() );
	memory_clients.front().object = ics;
	ics->retina = this;
	handle_to_list.insert ( make_pair ( ui, memory_clients.begin() ));
}

void retina_c::unregister(uint64_t ui )
{
	int flag = 0;
	if ( ui == 3 ){
		flag = 1;
	}
	if ( handle_to_list.find (ui ) == handle_to_list.end() ) {
		return;
	}
	auto lst = handle_to_list.at ( ui );
//	cout << "memories count: " << lst -> memories.size () << endl;
	for ( auto& a : lst->memories ) {
		a.disapear ();
	}
	lst -> memories.clear ();
	if ( lst -> object != 0 ) {
		lst -> object->retina = 0;	
	}
	memory_clients.erase (lst );
	handle_to_list.erase ( ui );
}

void retina_c::get_memory ( uint64_t ui, impression_clients_c*p , memory_c** m )
{
	if ( handle_to_list.find ( ui ) == handle_to_list.end() ){
		enregister ( ui, p );
	}
//	cout <<"get mem: " << handle_to_list.size () << '\n';

	auto lst = handle_to_list.at ( ui );
	for ( auto& x : lst->memories ) {
		if ( x.surface == surfaces.begin() ) {
			*m = &x;
			return; 
		}
	}
	lst->memories.push_front ( memory_c ( surfaces.begin() ) );
	*m= &lst->memories.front();	
	return;
}

void retina_c::erase_memories ( uint64_t ui )
{
	
	auto& lst = handle_to_list.at ( ui );
	if ( lst->memories.size () > 0 ){
//		cout << "A l.size: " << lst->begin()->l.size () << endl;
	}
	for ( auto& a : lst->memories ) {
		 a.disapear ();
	}
}

void retina_c::set_pixel (memory_c& memory , int x, int y, float z, long color)
{
	static int cc = 0;
	if ( cc == 100 ) {
		cout << "hi, lamb!\n";
	}
	cc++;

	surface_c& surface = surfaces.front ();
	width = surface.width;
	height= surface.height;
	int X0 = width >> 1;
	int Y0 = height >> 1;
	
	x = x + X0;
	y = y + Y0;
	if ( x < 0 || width <= x || y < 0 || height <= y ){
		return;
	}
	int u = height - y - 1;

	zlist_c& zzl2 = surface.v [ u ][ x ];
	list < pair < float, uint32_t > >& zl2 = zzl2.zpix;
	auto itz2 = zl2.begin ();
	auto itzm2 = itz2;
	for (; itz2 != zl2.end (); itz2++ ){
		if ( z <= itz2 -> first ){
			if ( true ) {
//			if ( z != itz2 -> first ){
				itzm2 = zl2.insert ( itz2, {z, color} );
				memory.l.push_back ( { &zzl2, itzm2 } );

			}
			if ( itzm2 == zl2.begin () ){
				*zzl2.frame_xy = color;
				surface.is_valide = false;
				if ( surface.is_valide ) {
//					owner.invalide ( 1 );
				}
			}
			break;
		}
	}
}

void retina_c::stream_out ()
{
	int code;
	for (;;) {
		if ( sso.access_request () ) {
			sso >> code;
			if ( code == 1 ) {
				cout << "wait client response\n";
				
			}
			else {
				sso << 102;
				sso << 3;
				sso <<  "erase";
			}
			sso.access_release ();
			break;
		}
		this_thread::sleep_for ( chrono::milliseconds ( 1 ) ) ;
	}
}

int retina_c::test (int i , string s) 
{
//	cout << "llist size: " << land.llist.size() << endl;
}

void retina_c::stream_in2 ()
{
	static int xch = 0;
	int code, code_out;
	int client_id;
	if ( ssi.access_request () ) {
		if ( xch == 0 ) {
			xch = 1;
//			cout << "r. ssi access\n";
		}
		ssi >> code;
//		cout << code;
		if ( code == 0 ) {
			ssi.access_release ();
			return;
		}
		if ( code == 1 ) {
			cout << "code 1" << endl;	
			if ( sso.access_request () ) {
				int c = land.get_handle();
				enregister ( c );
				cout << "handle: " << c << endl 
				<< "handle_to_list size: " << handle_to_list.size() << endl
				<< " client mem size: " << memory_clients.size () 
				<< endl;
				sso << 1 << c;
				ssi << 2;
				sso.access_release ();
				ssi.access_release ();
				return;
			}
		}
		if ( code == 2 ) {
//			cout << "code 2" << endl;
			if ( sso.access_request () ) {
				sso >> code_out;
				if ( code_out ==  0 ){
					int hdes;
					sso >> hdes;
					motion_3D_c <FT > mo = surfaces.front().motion;
					matrix_c <FT> ma = surfaces.front().mx;
					sso << 5 << hdes << owner->object_id << "motion";
					mo.serialize ( sso, true);
					ma.serialize ( sso, true);  
					sso << surfaces.front().width;
					sso << surfaces.front().height;
					ssi << 0;
				}
				sso.access_release();
			}
			ssi.access_release ();
			return;
		}
		else if ( code == 5 ) {
			int hdes, hsrc;
			string cmd;
			ssi >> hdes;
			if ( hdes == owner->object_id ) {
				ssi >>  client_id >> cmd;
				if ( cmd == "exit" ) {
					cout << "code 5 , client_id: " <<  client_id << " exit" << endl;
					unregister ( client_id );					
					ssi << 0;
					ssi.access_release();
					land.impress ();
					return;
				}
				else if ( cmd == "draw_direct_1" ) {
					int i;
					ssi >> i;
					uint32_t color;
//					cout  << "draw d1; id:" << client_id << " size:" << i << endl;
					mout  << "draw d1; id:" << client_id << " size:" << i << '\n';
					auto& lst = handle_to_list.at (client_id);
//					auto it = lst ->begin();
					auto it = lst ->memories.begin();
					for (; it != lst ->memories.end (); it ++ ) {
						if ( it -> surface == surfaces.begin () ) {
//							cout << "surface identity" << endl;
							break;
						}
					}
					auto& v = it -> l;
					for ( int c = 0; c < i ; c++ ) {
						ssi >> color;
						auto& zl = v [c].first;
						auto & zit = v [c].second;
						zit->second = color;
						if ( zit == zl->zpix.begin() ) {
							*zl -> frame_xy = color;
						}
					}						
					ssi << 0;		
					ssi.access_release ();
					land.expose();			
					return;
				}
				else if ( cmd == "draw" ) {
					int 
					x, y, i;
					float z;
					ssi >> i;
//					mout  << "draw;  id: " << client_id << "  size: " << i << endl;
					mout  << "draw;  id: " << client_id << "  size: " << i << '\n';
					erase_memories ( client_id );
					
					auto& lst = handle_to_list.at ( client_id );
					uint32_t color;
//					auto it = lst ->begin();
					auto it = lst ->memories.begin();
					for (; it != lst ->memories.end (); it ++ ) {
						if ( it -> surface == surfaces.begin () ) {
//							cout << "surface identity" << endl;
							break;
						}
					}
//					if ( it ==  lst -> end () ) {
					if ( it ==  lst -> memories.end () ) {
						cout << "lst insert " << endl;			
//						it = lst -> insert ( it, memory_c ( surfaces.begin () ) );
						it = lst -> memories.insert ( it, memory_c ( surfaces.begin () ) );
					}
		//			cout << "lst size: " << lst -> size () << endl;
		//			cout << "B l.size: " << it->l.size () << "i: " << i  << endl;
					for ( int c = 0 ; c < i ; c ++ ) {
						ssi >> x  >> y >> z >> color;
						set_pixel ( *it, x, y, z, color);
					}		
					ssi << 0;		
					ssi.access_release ();
					land.expose();			
					return;
		//			cout << "l.size: " << it->l.size () << endl;
				} 
			}
		}
		ssi.access_release ();
	}
	else {
		if ( xch != 0 ) {
			xch = 0;
//			cout << "r. access busy\n";
		}
	}
//	#endif
}

void retina_c::focus ( object_c* object ) 
{
/*
	for ( auto x : memory_clients ) {
		if ( x.object == object ) {
			x.object->impression_clients.has_focus = true;
		}
		else {
			x.object->impression_clients.has_focus = false;
	}
*/
}

void retina_c::motor ( object_c* object ) 
{
}
void retina_c::select ( object_c*, int state ) 
{
}




