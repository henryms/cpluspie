#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>

#include <linux/fb.h>
#include <linux/input.h>

#include "debug.h"

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
#include "image.h"

extern land_c land;

const unsigned long image_c::type = IMAGE;

using namespace std;

image_c::image_c ( string& _file_name ): 
page ( 1 ), 
map2 ( nullptr ),
columns ( 0 ),
rows ( 0 ),
file_name ( _file_name )
{
	int sz = file_name.size ();
	if ( file_name.size () >= 4 && file_name.substr ( file_name.size () - 4 ) == ".map" ) {	
		string directory = lamb_c::path_navigator;
		cout << directory << endl;
		cout << "image" << endl;	
		string path = directory + "/input";
		foutput.open ( path , ios::out | ios:: in );
		parameter = 1;
		string path3 = "mptr2.map";
//		lamb_c& lamb = *lamb_c::self;						
		lamb_c& lamb = *lamb_c::self;						
		lamb.memory_map ( path3, 0, map2, 0 );
		if ( map2 ) { 
			int32_t* p32 = ( int32_t* ) map2;
			p32 += 2;
			columns = *p32;
			rows = *++p32;
			echo << "map: " << map2 << endl;
		}
		else  {
			echo << "map failed" << endl;
		}
		is = "surface";
	//	transformation.setsize ( width * height );
	}
	else	if ( file_name.size () >= 4 && file_name.substr ( file_name.size () - 4) == ".ppm" ){
		convert_ppm_to_map ( file_name, fd2, map2, columns, rows );
		is = "surface";		
	}
	else if ( file_name.size () >= 4 && file_name.substr ( file_name.size () - 4) == ".vio" ) {
		is = "stream";
	}
}

image_c::~image_c ()
{
	echo  << "destruct image" << endl;
//	lamb_c& lamb = *lamb_c::self;
	lamb_c& lamb = *lamb_c::self;
	fstream fs( "/home/henry/wsh/rin" ) ;
	fs << 4 << " " << object_id << " ";
	fs.close ();
	
	if ( impression_clients.retina != 0 ) {
		impression_clients.retina->unregister ( ui );				
	}
//	lamb.eyes.retina .stream_in ();
//	munmap (map2 );
}

int image_c::edit ( lamb_c&, keyboard_c& keyb )
{
	string s = keyb.get_char ();
	if ( s == "s" ) {
		state = ! state;
	}
	return 0;
}

unsigned long image_c::get_type ()
{
	return type;
}

void image_c::convert_ppm_to_map ( string& fppm, int& fd, void*& map, size_t& columns, size_t& rows  )
{
//	lamb_c& lamb = *lamb_c::self;
	lamb_c& lamb = *lamb_c::self;
	ifstream fi ( fppm );
	string s;
	fi >> s;
	echo << s << endl;
	uint32_t ref;
	fi >> columns;
	fi >> rows;
	fi >> s;
	char c;
	fi.get(c);
	string mp;
	size_t sz = fppm.rfind ( "/" );
	if ( sz == string::npos ) {
		mp = fppm;				
	}
	else {
		mp = fppm.substr ( ++sz);
	}
	
	lamb.memory_map ( mp, 4*(columns*rows + 4), map, 0 ); 
	uint32_t* p = (uint32_t* ) map ;		
	*++p = ref;
	*++p = columns;
	*++p = rows; 
	for ( size_t ro = 0;ro < rows; ro++){
		for ( size_t co = 0; co < columns; co++){
			uint32_t pi = 0;
			unsigned char c1,c2,c3;
			fi.read ( (char*)&c1, 1 );
			fi.read ((char*)&c2, 1 );
			fi.read ((char*)&c3, 1 );
			pi = c1;
			pi = (pi << 8) + c2;
			pi = (pi << 8) + c3;
			*++p = pi;
		}
	}
}

void image_c::draw_stream ( lamb_c& lamb ) 
{
	if ( object_id == 0 ) {
		int code, id;
		fstream f ( "/home/henry/wsh/rin" );
		f << 1 << " ";
		f.close ();
		lamb.eyes.retina .stream_in2 ();
		f.open ( "/home/henry/wsh/rou" );
		f >> code >> id;
		echo << "code: " << code << " object id: " << id << endl;							object_id = id;
	}	
	
	ifstream ifs ( file_name );
	uint32_t u;
	int i , i2, i3;
	ifs >> i;
	ifs.seekg ( i );
	ifs >> i2;
	ifs >> i3;

	echo << "draw stream: " << file_name << " " << i << " " <<  i2 << " " << i3  << endl;	if ( i2 == 0 ) {
		return;
	}
	
	int x, y;
	float z;
	unsigned long color;
	size_t pos = ifs.tellg ();	
	
//	ofstream os ( "/home/henry/wsh/rin_s" );
	ofstream os ( "/home/henry/wsh/rin" );
	os << 2 << " "<< 1 << " " << i3 << " ";	
	for ( int c = 0; c < i3; c ++ ) {
		ifs >> dec >> x >> y >> z >> hex >> color;
		if ( state ) {
			color &= 0xffff;
			color |= 0x800000;
		} 
		else if ( this == lamb.sensor ) {
			color &= 0xff00ff;
			color |= 0x008000;
		}
		os << dec << x << " " << y  << " " << z << " "<< hex <<  color << " ";
	}	
	os.close ();
	
//	stringstream ss;
//	string command {"cp /home/henry/wsh/rin_s /home/henry/wsh/rin" };
//	lamb.system_echo ( command, ss );
	lamb.eyes.retina .stream_in2 ();

	return;	
	ifs.seekg ( pos );		
	for ( int c = 0 ; c < i3; c ++ ) {
		ifs >> dec >>  x  >> y >> z >> hex >> color;
		lamb.eyes.set_pixel ( 0, x, y, z, color, true );
	}
}


void image_c::draw ()
{
	lamb_c& lamb= *lamb_c::self;
	memory_c* memory;
	if ( 1 ) {
		lamb.eyes.retina .get_memory ( ui, & impression_clients,  &memory  );
		lamb.eyes.retina .erase_memories ( ui );
	}
	echo << "image draw: " << is <<  "." << endl;
	if ( is == "stream" ) {
		return draw_stream ( lamb );
	}
//	this_thread::sleep_for ( chrono::milliseconds { 100 } );	
	matrix_c < FT >
	vA = motion.object_vector ( 1 ),
	base = motion.object_base (),
	ux = base .get_column ( 2 ),
	uy = base.get_column (  3 );
	
	string lambs_path = get_lambs_path ();
	string s, s2;
	ifstream fi;	
	
	void* mptr = 0;
	auto p32 = static_cast < uint32_t* > ( map2 );
	if ( 0 ) {
		p32 += 2;
		columns = *p32;
		rows = *++p32;
		p32++;
	}
	else {
		p32 += 4;
	}											
	echo << "columns: " << columns << " rows: " << rows << endl; 
	if ( parameter == 1 ) {
		foutput.seekp ( 0 );
		if ( lamb.sensor == this ) {
			foutput << 1 << endl;
			echo << "sensor_image" << endl;
		}
		else { 
			foutput << 0 << endl;
		}
		foutput.flush ();
	}
	
	matrix_c < FT > 
	v00 = { 0, 0, 0 },
	v0 = lamb.world_to_view ( v00 ),
	vt,
	v;

	v = lamb.world_to_view ( vA );	
	matrix_c < FT > vvA = { v[1], v[2] };
	vt = base.get_column ( 2 );
	v =  lamb.world_to_view ( vt ) - v0;
	matrix_c < FT > vvx = { v[1], v[2] };
	vt = base.get_column ( 3 );
	v = lamb.world_to_view ( vt ) -  v0;	
	matrix_c < FT > vvy = { v[1], v[2] };
	
	matrix_c < FT > m = vvA ;
	matrix_c < FT > mm ( 2, 1 );
	m = m + rows* vvy;
	bool direct = false;
	int width;
	int height;
	uint32_t* pf;
	if ( direct ) {
//		p32 = lamb.eyes.retina.surfaces.l.front ().frame;		
///		p32 = lamb.eyes.retinaa.surfaces.l.front ().frame;		
		width = lamb.eyes.width;
		height = lamb.eyes.height;
	}
	//if ( transformation.size () == 0 ) {
	if ( 1 ) {
		transformation.resize ( columns * rows );
		for  ( int y = 0; y < rows ; y ++ ) {
			m =  m - vvy;
			mm = m;
			for ( int x = 0; x < columns; x ++ ) {
				mm = mm + vvx; 
				if ( false ) {
					int32_t color;
					fi.read ((char*)&color, sizeof ( int32_t ));
					lamb.eyes.set_pixel ( 0, mm [1], mm [2], 0, color, true );
				}
				else {
					int32_t color2;
					
					color2 = * p32;
					p32++;

					if ( direct ) {
					
	//					*pf = color2;
						int xx = mm [ 1 ];
						int yy = mm [ 2 ];
						
						int X0 = width >> 1;
						int Y0 = height >> 1;
	
						xx = xx + X0;
						yy = yy + Y0;
						if ( xx < 0 || width <= xx || yy < 0 || height <= yy ){
							return;
//							return false;
						}
						int u = height - yy - 1;
					
						transformation.at ( x + columns* y ) = xx + width * u; 
	//					* ( pf  +  (width * yy + xx ) )  = color2;
						lamb.eyes.set_superficial_pixel ( mm [1], mm[2], color2 );
					}
					else {
						lamb.eyes.retina .set_pixel ( *memory, mm [1], mm [2], 0, color2 );
					}
				}
			}
		}
	}
	else {
		int c = columns * rows;
		for ( int x = 0; x < c ; x++ ) {
			 *(pf + transformation [ x ] ) = *(uint32_t *)mptr;
			mptr += 4;
		}
	}

	ux = columns*ux;
	uy = rows*uy;
	frame.control_points = { 
		vA,
		vA + ux,
		vA + ux + uy,
		vA + uy,
		vA
	};
	if ( state ) {
		frame.color = 0xff0000;
	}
	else if ( lamb.sensor == this ) {
		frame.color = 0xff00;
	}
	else {
		frame.color = 0x00;
	}
	frame.draw ();
}

//****************

void image_c::convert_ppm_to_image ( string& arg1, string& arg2 )
{
//		cout << "ppm to image" << endl;
//		cout << "arg1:" << arg1 << endl;		
		ofstream fo ( arg2 );
		ifstream fi ( arg1 );
		string s;
		fi >> s;
		cout << s << endl;
//		size_t columns, rows;		
		fi >> columns;
		fi >> rows;
		fi >> s;
		char c;
		fi.get(c);

//		cout << "columns:" << columns << endl;	
//		cout << "rows:" << rows << endl;
//		cout << "color:" << s << endl;
		fo.write ((char*)&columns, sizeof ( int32_t ));
		fo.write ((char*)&rows, sizeof ( int32_t ));
		for ( size_t ro = 0;ro < rows; ro++){
			for ( size_t co = 0; co < columns; co++){
				uint32_t pi = 0;
				unsigned char c1,c2,c3;
				fi.read ( (char*)&c1, 1 );
				fi.read ((char*)&c2, 1 );
				fi.read ((char*)&c3, 1 );
				pi = c1;
				pi = (pi << 8) + c2;
				pi = (pi << 8) + c3;
				fo.write ((char*)&pi, sizeof ( uint32_t ));
			}
		}
		return; 
}

void image_c::convert ( string name )
{
	cout << "xxxxx convert" << endl;
	string lambs_path =  get_lambs_path();
	lambs_path+="/";
	cout << "convert name:" << name << endl;
	if ( name.substr ( name.size ()-4 )!=".pdf" ){
		return;
	}
	string s = name.substr ( 0, name.size ()-4 );

	string st;
	//st="/home/henry/shelf/cpp/lamb/doc/";
	
	ifstream fi ( st + s + ".pdf" );
	if (!fi.good ()){
		return;
	}
	fi.close ();
	stringstream ss;
	string crop;
//	crop  = " -x 150 -y 100 -W 600 -H 1000 ";	
//	crop  = " -x 200 -y 200 -W 700 -H 1000 -r 130 ";	
	crop  = " -x 200 -y 150 -W 700 -H 1000 -r 120 ";	
	string str ="pdftoppm " + crop  + st + s + ".pdf " + lambs_path + "tmp/" + s;
	cout << str << endl;
//	lamb.system_shell ( str, ss );
	lamb_c::self->system_echo ( str, ss );
//	lamb_c::self -> system_echo ( str, ss );
	for ( ptrdiff_t c = 1;;c++){
		string n = sc11::to_string ( c );

		string so = lambs_path + "tmp/" + s + "-" + n + ".ppm";
		fi.open ( so );
		cout << "file source:" << so << endl;

		if (!fi.good ()){
			fi.close ();
			so = lambs_path+ "tmp/" + s + "-0" + n + ".ppm";
			fi.open ( so );
			if (!fi.good ()){
				break;
			}
		}
		string de = lambs_path + "tmp/" + s + "-" + n;
		
		convert_ppm_to_image ( so, de );
		cout << "command:" << str << endl;
		fi.close ();
	}
	return;
}

void image_c::load ( string& name )
{
	string lambs_path =  get_lambs_path();
	lambs_path+="/";
	string s;
	s="./myimage "+lambs_path+"png4.png " + lambs_path+"image3";
	system ( s.c_str ());
	ifstream fi;
	fi.open ( lambs_path+"image3", ios::binary );
	fi.read ((char*)&columns, sizeof ( int32_t ));
	fi.read ((char*)&rows, sizeof ( int32_t ));
	for ( size_t ro = 0; ro < rows ; rows++){
		for ( size_t co = 0; co < columns ; columns++){
						
		}
	}
}

void serialize ( fstream &file, bool bsave )
{
}

void image_c::convert_pdf_to_png ( string project_name )
{
}

void image_c::convert_png_to_ima ( string name )
{
	name="natpro-1.png";	
	string lambs_path =  get_lambs_path();
	lambs_path+="/";
	string str = name;
	if ( str.substr ( str.size ()-4 )!=".png" ){
		return;
	}
	
	string s = str.substr ( 0, str.size ()-4 );

	//string com="convert " + lambs_path + "tmp/" + s + ".png " + "-resize 500x600 " +  lambs_path + "tmp/" + s + ".bmp";
	
	string com="convert " + lambs_path + "tmp/" + s + ".png " +  lambs_path + "tmp/" + s + ".png";

//	system ( com.c_str ());
		
	cout << "ok" << endl;
	
//	string so = lambs_path + "tmp/" + s + ".bmp";
	string so = lambs_path + "tmp/" + s + ".pmc";
	ifstream fi;
	fi.open ( so );
	if (!fi.good ()){
		return;
	}
	fi.close ();
	string de = lambs_path + "tmp/" + s;
	str = lambs_path + "imex/myimage " + so + " " + de;
	system ( str.c_str ());
	
	fi.close ();
}

