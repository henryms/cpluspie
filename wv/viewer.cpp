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
#include <thread>
#include <chrono>
#include <cmath>
#include <typeinfo>
#include <tuple>

#include "mstream.h"
#include "symbol/keysym.h"
#include "symbol.h"

#include "library/wdslib.h"
#include "gate.h"

#include "matrix.h"
#include "position.h"
#include "message.h"
#include "global.h"
#include "mouse.h"
#include "keyb.h"

#include "sens/retina.h"
#include "sens/spirit.h"
#include "object.h"
#include "line.h"
#include "spline.h"
#include "wr/texel.h"
#include "wr/shared.h"
#include "wr/layen.h"
#include "fox/edit.h"
#include "bookmarks.h"
#include "carte.h"
#include "image.h"

#include "land.h"

#include "regexp.h"
#include "shell.h"

#include "eyes.h"
#include "hand.h"
#include "home.h"

#include "cash.h"
#include "lamb.h"

#include "comtab4.h"
#include "edit_ct.h"
#include "lamb_ct.h"

#include "viewer.h"

int viewer_c::advance_pixmap_pointer()
{
	int hc = (columns-1)/window_width+1,
	hs = window_width;
	if ( hc > 1){
		hs -= (hc*window_width-columns)/(hc-1); 
	}
	int vc = (rows-1)/window_height+1,
	vs = window_height;
	if(vc>1){
		vs-= (vc*window_height-rows)/(vc-1); 
	}
	return ((strip-1)/hc)*columns*vs+((strip-1)%hc)*hs;
}

void viewer_c::convert_from_ppm ( string& fppm )
{
	ifstream fi ( fppm );
	string s,s1;
	fi >> s;
	uint32_t ref;
	fi >> columns >> rows >> s1;
	char c;
	fi.get(c);
//	cout << "icon.ppm: " << s << " " << columns << " " << rows << " " << s1 << endl; 
	if ( pi == nullptr){
		pi = new uint32_t [ columns*rows*4 ];
	}
	if(impression_grid == nullptr){
		impression_grid = new char[columns*rows];
	}
	auto pc = pi;
	for ( size_t ro = 0;ro < rows; ro++){
		for ( size_t co = 0; co < columns; co++){
			uint32_t i = 0;
			unsigned char c1,c2,c3;
			fi.read ( (char*)&c1, 1 );
			fi.read ((char*)&c2, 1 );
			fi.read ((char*)&c3, 1 );
			i = c1;
			i = (i << 8) + c2;
			i = (i << 8) + c3;
			*pc = i;
			pc++;
		}
	}
}

viewer_c::viewer_c ( ):
connection ( 0 ),
has_focus ( false )
{
}

viewer_c::~viewer_c ( ) 
{
	delete pi;
	delete impression_grid;
}

void viewer_c::draw_direct_1 ( uint32_t handle )
{
	auto p = surfaces.find ( handle );
	if ( p == surfaces.end() ){
		return;
	}
	int code;
	uint32_t* pc = pi + advance_pixmap_pointer();
	char* pig = impression_grid;
	uint32_t color;
	for ( int count = 0; count < 100; count++ ) {
		if ( ssi.access_request () ) {
			ssi >> code;
			if ( code == 0 ) {
				ssi << 5 << hsrc << object_id << "draw_direct_1" << impression_count;
				for (int cy = 0; cy < window_height; ++cy){	
					for ( int cx = 0; cx <= window_width; cx++ ) {
						if (*pig++){
							color = *pc;
							if ( state ) {
								if (color == 0xffffff){
									color &= 0xffd0d0;
								}	
							}
							else if ( has_focus ) {
								if (color == 0xffffff){
									color &= 0xd0ffd0;
								}
							}
							ssi << color;
						}
						++pc;	
					}
					pc += columns -window_width;
				}
				count = 0;
				ssi.access_release ();
				break;
			}
			ssi.access_release();
		}
		this_thread::sleep_for ( chrono::milliseconds ( 10 ) );
	}
}

void viewer_c::draw ( uint32_t handle )
{
	auto p = surfaces.find ( handle );
	if ( p == surfaces.end() ){
		return;
	}	
	matrix_c < FT > 
	a = motion.object_vector ( 1 ),
	A = motion.object_base (),
	b = p->second.motion.object_vector ( 1 ),
	B = p->second.motion.object_base (),
	C = p->second.mx,
	X =~C*~B*A,
	x = ~C*~B* ( a - b ),
	t ( 3, 1 ), 
	r;
	t [ 3 ] = 0;
	int32_t xl = -p->second.width/2,
			xr = +p->second.width/2-1,
			yb = -p->second.height/2, 
			yt = +p->second.height/2-1;
		mout << xl << ":" << xr << ":" << yb << ":" << yt << "**\n";

	if(1){		
		window_width = columns;
		window_height = rows;	
	}
	else{
		window_width = 100;
		window_height = 200;	
	}
	if (p->second.height < rows){
		window_height = p->second.height;
	}
		
	uint32_t* pc = pi + advance_pixmap_pointer();
		
		
	char* pig = impression_grid;
	
	
	
	size_t pos_put;	
	int code;
	int count = -1;
	char* pp0;
	if ( object_id != 0 ) {
		mout << "object id: " << object_id << '\n';
		for ( count = 0; count < 100; count++ ){
			if ( ssi.access_request () ) {
				ssi >> code;
				if ( code == 0 ) {
					mout << "draw access\n";
//					ssi <<  5 << hsrc << object_id << "draw" << columns*rows;
					ssi <<  5 << hsrc << object_id << "draw";
					pp0 = ssi.pp;
					ssi << columns*rows;
					impression_count = 0;
					for ( int i = window_height - 1; i >= 0; i -- ) {
						for ( int j = 0; j < window_width; j ++ ) {
							t [ 1 ] = j;
							t [ 2 ] = i;
							r = X*t + x;
							int32_t ix = r [ 1 ];
							int32_t iy = r [ 2 ];
							if ( xl <= ix && ix <= xr && yb <= iy && iy <= yt){
								float z = r [ 3 ];
								uint32_t color = *pc;
								if ( state ) {
									if(color == 0xffffff){
										color &= 0xffd0d0;
									}
								}
								else if ( has_focus ) {
									if(color == 0xffffff){
										color &= 0xd0ffd0;
									}
								}
								ssi  <<  ix << iy  << z << color;
								*pig++ = 1;
								++impression_count;
							}
							else{
								*pig++ = 0;
							}
							++pc;
						}
						pc += columns - window_width;
					}
					ssi.pp = pp0;
					ssi << impression_count;	
					count = 0;
					ssi.access_release ();
					break;
				}
				ssi.access_release();
			}
			this_thread::sleep_for ( chrono::milliseconds ( 10 ) );
		}	
	}		
	if ( connection > 0 ) {
		connection = 3;
	}
	mout << "connection 3: " << connection << '\n';
}

using namespace std;
int viewer_c::page_numbers(int n)
{
	static int page_numbers{0};	
	static string name;	
	if ( n != -1 || file_name.empty()){
		name = "";
		return page_numbers = 0;
	}
	if (page_numbers == 0 || name != file_name){
		name = file_name;
		string s, s0, s1;
		size_t pos = name.rfind('-');
		
		if (pos == string::npos){
			page_numbers = 1;	
			return 1;
		}
		s = name.substr(0, pos);
		list<string> sl{"1","01","001","0001"};
		ifstream ifs;
		for(;!sl.empty();){
			ifs.open(s + "-" + sl.front()+ ".ppm");	
			if(ifs){
				break;
			}
			sl.pop_front();					
		}	
		if (sl.empty()){
			page_numbers = 0;
		}
		int d = sl.front().size(),
		a = 1,
		b,
		c = pow(10,d);
		mout << "pow..." << c << '\n';
		stringstream ss;
		ss.width(d);
		ss.fill('0');
		bool success = false;	
		for(int count = 100; cout >0; --count){
			if(success){
				a = c;
			}
			else{
				b = c;
			}
			if ( (a+1) == b){
				page_numbers = a;
				break;
			}
			c = a+(b-a)/2;	
			ss.str("");
			ss.clear();
			ss.width(d);
			ss.fill('0');
			ss << c;
			ss >> s0;
			s1 = s + "-" + s0 + ".ppm";
			mout << s0<<"::" << c << '\n';
			ifstream ifs(s1);
			if(ifs){
				success = true;
				ifs.close();
			}	
			else{
				success = false;
			}
		} 
		mout << "page numbers: " << page_numbers << '\n';
	}
	return page_numbers;
}

int viewer_c::advance_page ( int step)
{
	stringstream ss;
	string s, s0, &n = file_name;
	size_t pos = n.rfind ('-');
	if ( pos == string::npos){
		return 1;	
	};
	++pos;
	size_t pos2 = n.rfind ('.');
	s = n.substr ( 0, pos);
	s0 = n.substr ( pos,  pos2 - pos );
	int page = stoi ( s0 );
	page += step;
	if (page < 1){
		page = 1;
	}
	else if (page > page_numbers()){
		page = page_numbers();
	}
	int digits = log10(page_numbers()) + 1;
	mout << "digits:" << digits << '\n';
	ss << s;
	ss.fill('0');
	ss.width (digits);
	ss << page << ".ppm";
	ifstream ifs ( ss.str () );
	if ( ifs ) {
		file_name = ss.str();
	}
	mout << "fn: " << file_name.substr(file_name.size()-7, 7) << '\n';
	convert_from_ppm( file_name );
	return page;
}

void viewer_c::loop()
{
	for ( ;; ) {
		int code, id, hdes;
		string cmd;
		if ( sso.access_request () ) {
			sso >> code;
			if ( code == 5 ) {
				sso >> hdes >> hsrc >> cmd; 
				if ( hdes == object_id ) {  
					if ( cmd == "quit" ) {
						mout << "wv quit\n";
						sso << 0;
						sso.access_release ();
						for (;;) {
							if ( ssi.access_request () ) {	
								ssi >> code;
								if ( code == 0 ) {
									mout << "wv exit\n";
									ssi << 5 << hsrc << object_id << "exit";
									ssi.access_release ();
									return;
								}
							}
							this_thread::sleep_for ( chrono::milliseconds ( 10 ) );
						}
					}
					else if ( cmd == "focus_yes" ) {
						has_focus = true;
						sso << 0;
						sso.access_release ();
						draw_direct_1 ( hsrc );
						continue;
					}
					else if ( cmd == "focus_no" ) {
						has_focus = false;
						sso << 0;						
						sso.access_release ();
						draw_direct_1 ( hsrc );
						continue;
					}
					else if ( cmd == "shift" ) {
						mout << "wv shift\n";
						matrix_c <FT> v;
						v.serialize ( sso, false );
						v.out ( 1 );
						matrix_c <FT> a = motion.object_vector ( 1 );	
						a.out ( 1 );
						motion.set_object_vector ( 1, a + v );
					}
					else if ( cmd == "state" ) {
						mout << "wv state\n";
						sso >> state;
						sso << 0;
						sso.access_release ();
						draw_direct_1 ( hsrc );
						continue;
					}
					else if ( cmd == "motion" ) {
						mout <<  "wv motion\n";
						size_t gt = sso.tellg();
						surfaces.insert ( make_pair ( hsrc, surface_c() ) );											auto p = surfaces.find ( hsrc ); 						
						p->second.motion.serialize ( sso, false );
						p->second.mx.serialize ( sso, false );
						sso >> p->second.width;
						sso >> p->second.height;
						
						mout << "width: " << p ->second.width << '\n';
						mout << "hegth: " << p ->second.height << '\n';		
						matrix_c<FT>
//					vA { 0, -p->second.width/2 +450, -p->second.height/2 },
						vA { 0, -p->second.width/2 + 0, -p->second.height/2 -0},
						vb { 0, 1, 0 },
						vx { 0, 0, 1 };
						motion.set_object( vA, vb, vx);
					}
					else if ( cmd == "next_page" ) {
						if (strip >= ((columns-1)/window_width+1)*((rows-1)/window_height+1)){
							if (advance_page(0) != advance_page(1)){
								strip = 1;	
							}
							mout << "stripin:" << strip << '\n';
						}
						else{
							++strip;
							mout << "strip++\n";
						}
						mout << "strip:" << strip << '\n';
						sso << 0;
						sso.access_release ();
						draw_direct_1 ( hsrc );
						continue;
					}
					else if ( cmd == "prev_page" ) {
						if (strip == 1){
							if (advance_page(0) != advance_page(-1)){
								strip = ((columns-1)/window_width+1)*((rows-1)/window_height+1);	
							}
						}
						else{
							--strip;	
						}
						sso << 0;
						sso.access_release ();
						draw_direct_1 ( hsrc );
						continue;
					}
					else if ( cmd == "refresh" ) {
						mout << "wv refresh\n";
						page_numbers(0);
						advance_page ( 0 );
						sso << 0;
						sso.access_release ();
						draw_direct_1 ( hsrc );
						continue;
					}
					sso << 0;
					sso.access_release ();
					draw ( hsrc );
					continue;
				}
			}	
			sso.access_release ();
		}
		this_thread::sleep_for ( chrono::milliseconds ( 10 ) );
	}	
}

int main ( int argc, char * argv[] )
{
	mout << "hi viewer!\n";
	nout.mode = 1;
	viewer_c o;
	o.fl = true;
	if ( argc == 2 ) {
		o.file_name = string ( argv [ 1 ] );
	}
	else {
		o.file_name = "/home/henry/shelf/cpp/wss/desk/icon.ppm";
	} 
	
	message_c* pm = get_machine ();	
	
	if ( pm == nullptr ){
		cout << "machine fail\n";
		return EXIT_FAILURE;
	}
	message_c i;
	i.that = pm;
	pm->that = &i;
	
	matrix_c<FT> 
	vA { 0, -800, -500 },
	vb { 0, 1, 0 },
	vx { 0, 0, 1};
	o.motion.set_object( vA, vb, vx);						
	
	char* pi;	
	i.memory_map2 ( "retina_smi", 0, pi ); 
	o.ssi = rstream_c ( pi );
	i.memory_map2 ( "retina_smo", 0, pi ); 
	o.sso = rstream_c ( pi );
	 
	int code, id, code_out, count = 0;
	for (count = 0; count < 100; count++){
		if ( o.ssi.access_request() ) {
			mout << "ac 1\n";
			o.ssi >> code;
			if ( code == 0 ) {
				o.ssi << 1;
				o.ssi.access_release ();
				count = 0;
				break;
			}
			o.ssi.access_release();
		}
		this_thread::sleep_for ( chrono::milliseconds ( 10 ) );
	}
	if ( count != 0 ) {
		o.connection = 1;
	}
	mout << "connection 1: " << o.connection << '\n';

	for (count = 0 ; count < 100  && o.connection == 0 ; count++ ){
		if ( o.sso.access_request() ) {
			o.sso >> code_out;
			if ( code_out == 1 ) {
				o.sso >> o.object_id;
				o.sso << 0;
				o.sso.access_release ();
				count = 0;
				break;
			}
			o.sso.access_release();
		}
		this_thread::sleep_for ( chrono::milliseconds ( 10 ) );
	}
	
	if ( count != 0 ) {
		o.connection = 2;
	}
	
	mout << "connection 2: " << o.connection << '\n';
	
	mout << o.file_name << '\n';
	
	o.convert_from_ppm( o.file_name );
	o.loop ();
			
	mout << "goodby viewer!\n";
	return 0;
}