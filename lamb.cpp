// lamb, the future of the lamb, the fox 

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
#include <tuple>
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

#include "fox/cash.h"
#include "message.h"
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

int parrot = 0;

string lamb_c::path_navigator = "/home/henry/netsurf-full-3.0/src/libnsfb-0.1.0/src/surface";


extern void ex_entry_comtab4 ( stringstream&);
extern void ex_entry_editor ( stringstream& );
extern void ex_entry_echo ( stringstream& );
extern void ex_entry_texel (stringstream&);
extern void ex_entry_lamb(stringstream&);

int byte_index ( string& str, int index );
int position_index ( string& str, int index );

using namespace std;

lamb_c* lamb_c::self;

comtab1_lamb_c lamb_c::comtab1_lamb;
comtab2_lamb_c lamb_c::comtab2_lamb;


unsigned long lamb_c::type = LAMB;


extern land_c land;


int figure = CARTESIAN;

lamb_c::lamb_c (): 
	hand ( ),
	local_position ( 3, 1 ),
	verbal ( true ),
	shift_rotate ( true ),
	keyboard_mouse ( false ),
	is_printing ( false ),
	hook ( nullptr ),
	none_event ( false ),
	no_refresh ( false ),
	stream_in ( false ),
	command_number ( COMPILE_SNOW )
{
	cout << "lamb_c construction" << endl;

	mouth.parent = this;
	string pathlamb =  get_lambs_path();
	string lambcfg = pathlamb + "/config/lambcfg";
	echo << "lambs path: " <<  pathlamb << endl;
	is_cursor_captured = false;
	movement = SHIFT_OBJECT;
//	movement = POINT;
	fstream file ( lambcfg, ios::in );
	string s1;
	string s2;
	string country;
	if ( file.is_open ()){
		cout << "load lamb's configuration " << endl;
		file >> eyes.width;
		file >> eyes.height;
		file >> eyes.frame_x;
		file >> eyes.frame_y;
		file >> eyes.frame_width;
		file >> eyes.frame_height;
		getline ( file, s1 );
		getline ( file, country );
		getline ( file, home );
		getline ( file, directory );
		getline ( file, grf_file );
		getline ( file, project );
		getline ( file, start_command );
		if ( start_command != "" ) {
			echo << "start command: " << start_command << endl;
		}
		file >> home_c::initial_sensor;
		getline ( file, s1);
		string coms;
		string ef;
		string info;
		string marks_name;
		marks_name = pathlamb + "/config/bookmarks";
		bookmarks.serialize ( marks_name, false );
		marks_name = pathlamb + "/config/ringmarks";
		ringmarks.serialize ( marks_name, false );
		marks_name = pathlamb + "/config/automarks";
		automarks.serialize ( marks_name, false );
	}
	else {
		eyes.width = 200;
		eyes.height = 100;
		home = "default";
	}
	file.close ();
	keyboard.set_layout ( country );
	mx = {{0, 1, 0},{0, 0, 1},{-1, 0, 0}};
	motion.set_object_vector ( 1,{0, 0, 0});
	motion.set_object_vector ( 2,{1, 0, 0});
	motion.set_object_vector ( 3,{0, 1, 0});
	
	eyes.set_size (*this, eyes.width, eyes.height );
	motor = &mouth;
	motors.push_back (&mouth );
	motors.push_back ( this );
	mouth.impression_clients.retina = &eyes.retina ;
	hand.impression_clients.retina = &eyes.retina ;
	eyes.impression_clients.retina = &eyes.retina ;
	status.impression_clients.retina = &eyes.retina ;
	ears.impression_clients.retina = &eyes.retina ;			
	mouth.layen.signo = 3;	
	projects = {
	"com compile", 
	"vwx viewerx", 
	"vwl viewerl",  
	"mol tex /home/me/desk/newvinland/mathoflife/mathoflife",
	"real tex /home/me/desk/cpp/natsoft/real/real",
	"c+g tex /home/me/desk/cpp/natsoft/c+g/c+g",
	"cwr tex /home/me/desk/cpp/natsoft/cwr",
	"tex textest /home/me/desk/cpp/natsoft/cwr",
	"cpp cpp /home/me/desk/cpp/school/cc/39locales/codecvt"
	};
	project = "com compile";
}

lamb_c::~lamb_c ()
{
	cout << "lamb destruction" << endl;
	string path =  get_lambs_path();
	fstream file ( path+"/config/lambcfg", ios::out );
	file << eyes.width << endl;
	file << eyes.height << endl;
	file << eyes.frame_x << endl;
	file << eyes.frame_y << endl;
	file << eyes.frame_width << endl;
	file << eyes.frame_height << endl;
	file << keyboard.country << endl;
	file << home << endl;
		
	directory = getenv ("PWD" );	
	file << directory << endl;
	file << grf_file << endl;	
	file << project << endl;
	file << start_command << endl;
	int c = mobilar.size ();
	for ( auto it=mobilar.rbegin(); it != mobilar.rend(); it ++ ) {
		if ( *it == sensor ) {
			break;
		}
		c--;
	}	
	file << c << endl;	
	string marks_name;
	marks_name = path + "/config/bookmarks";
	bookmarks.serialize ( marks_name, true );
	marks_name = path + "/config/ringmarks";
	ringmarks.serialize ( marks_name, true );
	marks_name = path + "/config/automarks";
	automarks.serialize ( marks_name, true );
}

void lamb_c::draw (lamb_c& lamb)
{
	draw();
}

map<unsigned int, string> movements_map2={
	{SHIFT,"s"},
	{ROTATE,"r"},
	{SHIFT_OBJECT,"so"},
	{ROTATE_OBJECT,"ro"},	
	{POINT,"p"}
};

map<int, string> edit_modes_map2={
	{INSERT,"i"},
	{COMMAND,"c"},
	{VISUAL,"v"},
};

map< int, string > figures_map2 = {
	{SPLINE,"spline"},
	{CARTESIAN,"cartesian"},
	{IMAGE,"image"},
	{EDITOR,"editor"},
	{ZERO,"zero"}
};

map<int, string> shift_rotate_map2 = {
	{0, "s"},
	{1, "r"}
};

map <int, string> mouse_keyboard_map2 = {
	{0, "m"},
	{1, "k"}
};

void lamb_c::draw ()
{
	lamb_c& lamb = *this;
	stringstream ss, ss0{project};
	string s0;
	ss0 >>s0;
	ss 
		<< "|" << movements_map2[lamb.movement] 
	//	<< "|" <<  projects_map2[lamb.command_number]
		<< "|" <<  s0
		<< "|" << figures_map2[figure]
		<< "|" << shift_rotate_map2[lamb.shift_rotate]
		<< "|" << mouse_keyboard_map2[lamb.keyboard_mouse]
		<< "|o." << land.llist.size ()
		<< "  ";
	mouth.info_strip = ss.str();
	mouth.draw ();
	write_status();
	status.draw ( );
	eyes.draw ( );
	hand.draw ( );	

	bool roll = true;
	texel_caret_c* tc = ears.layen.get_caret ();
	size_t pos = tc -> get_position (),
	pos_end = tc -> get_last_position ();
	size_t sel_pos = tc->get_selector_position();
	bool binded = tc->is_selector_binded();
	int scroll_up = tc->info.scroll_up;
	if ( pos != pos_end   ) {
		roll = false;
	}
	string st {necho.ss2.str()};
	ears.layen.set_text (st);
	ears.layen.set_caret ();
	ears.layen.font.set_rows(ears.layen);
	tc = ears.layen.get_caret();	
	if ( roll == true ) {
		pos_end = tc -> get_last_position () ;
		tc -> move_to ( pos_end );
		tc->bind_selector();
		ears.fit_scrolls ();
	}		
	else {
		tc -> move_to ( pos );
		if ( ! binded ) {
			tc->move_selector_to ( sel_pos );
		}
		tc->info.scroll_up = scroll_up;
		ears.fit_scrolls();
	}
	ears.draw (  );
} 

void lamb_c::local ( object_c& object, matrix_c<FT> vA, matrix_c<FT> vb, matrix_c<FT> vx )
{
	object.motion.set_object_vector ( 1, view_to_world ( vA ));
	object.motion.set_object_vector ( 2, view_to_world ( vb ));
	object.motion.set_object_vector ( 3, view_to_world ( vx ));
}

void lamb_c::rotatesel ( int lr, int du, int rlr )
{
	matrix_service_c<FT> matrix_service;
	matrix_c<FT> m = matrix_service.rotate3 ( du, lr,-rlr );	
	matrix_c<FT> A = motion.object_base ();
	matrix_c<FT> center = motion.object_vector ( 1 );
	center = A*mx*hand.pointer + center;
	matrix_c<FT> R = A*mx*m*~mx*~A;

	matrix_c<FT> vb, vx;
	list<object_c*>::iterator iter;
	object_c* obj;
	line_c* pline;
	iter = land.llist.begin ();
	for (; iter != land.llist.end (); iter++){
		obj = *iter;
		if ( obj->get_type () == LINE && obj->state == 1 ){
			pline = (line_c*)obj;
			matrix_c<FT> vA = pline->motion.object_vector ( 1 );
			vA = R*(vA-center ) + center;
			matrix_c<FT> B = pline->motion.object_base ();
			matrix_c<FT> base = ~B*R*B;
			vb = base.get_column ( 1 );
			vx = base.get_column ( 2 );
			pline->motion.set_object ( vA, vb, vx );
		}	
	}
}

void lamb_c::rotate ( object_c* object, int lr, int du, int rlr )
{
	//x' = Ai*x, x'' = Bi*x' => x'' = Bi*Ai*x <=> x'' = (A*B )i*x
	
	int step = 1;
	matrix_service_c < FT > matrix_service;
	matrix_c < FT > m;
	m = matrix_service.rotate3 ( du*step, lr*step, -rlr*step );	
	matrix_c < FT > base = object->motion.object_base ();
	base = base*mx*m*~mx;
	matrix_c < FT > 
		vA = object -> motion.object_vector ( 1 ),
		vb = base.get_column ( 1 ),
		vx = base.get_column ( 2 );
		
	object -> motion.set_object ( vA, vb, vx );

	for ( auto x : land.llist ) {
		if ( x->get_type () == EDITOR ) {
			((editor_c*)x)-> layen.clear_pixel_vectors();			
		}
	}	
	if ( object == this ) {
		eyes.retina .surfaces.front().motion = motion;
	}
}

void lamb_c::shift_selection ( int lr, int du, int bf )
{
	object_c* obj;
	line_c* pline;
	list<object_c*>::iterator iter;
	iter = land.llist.begin ();
	matrix_c<FT> v ({(float )lr,(float )du,(float )bf});

	for (; iter != land.llist.end (); iter++){
		obj = *iter;
		if ( obj->state == 1 ){
			obj->shift ( v );
		}
		if ( obj->get_type () == LINE && obj->state == 1 ){
			pline=(line_c*)obj;
//			matrix_c<FT> v ({(float )lr,(float )du,(float )bf});
			pline->vA = pline->vA + v;
		}
		else if ( obj->get_type () == EDITOR && obj->state == 1 ){
			editor_c* pedit;
			pedit = (editor_c*)obj;
//			matrix_c<FT> v ({(float )lr,(float )du,(float )bf});
			matrix_c<FT> vv;
			vv = pedit->motion.object_vector ( 1 );
			vv = vv + v;
			pedit->motion.set_object_vector ( 1, vv );			
		}
	}
}

void lamb_c::shift ( object_c* object, int lr, int du, int bf )
{
	echo << "lamb shift2..." << endl;
	int step = 10;
	matrix_c<FT> v ({(float )lr*step,(float )du*step,(float )bf*step});
	matrix_c<FT> v0 ({0, 0, 0});
	v = view_to_world ( v );
	v0 = view_to_world ( v0 );
	v = v - v0;
	if ( object != 0 ){
		echo << "not zero" << endl;
		object->shift3 ( v );
	}
}

void lamb_c::shift3 ( matrix_c<FT>& v )
{
	echo << "lamb shift3" << endl;
	matrix_c<FT> vA = motion.object_vector ( 1 );
	vA = vA+v;
	vA.out ( 1 );
	motion.set_object_vector ( 1, vA );
}

void lamb_c::point ( int lr, int du, int bf )
{
	hand.pointer.l[0]+= lr;
	hand.pointer.l[1]+= du;
	hand.pointer.l[2]+= bf;
}

void lamb_c::copy_selection ()
{
	list<object_c*>& l = land.llist;
	list<object_c*>::iterator it = l.begin ();
	for (; it != l.end (); it++){
		if ( (*it )->state == 1 ){
			object_c* object = (*it )->duplicate ();
			if ( object != 0 ){
				l.insert ( it, object );
				object->state = 1;		
				(*it )->state = 0;
			}
		}
	}	
}

void lamb_c::move ( int type, int nx, int ny, int nz )
{
	int mmx = nx;
	int my = ny;
	int mz = nz;
	switch ( type ){
	case POINT:
		point ( mmx, my, mz );
		if ((hand.pointer[1] < -eyes.width/2 - 1 ) || (hand.pointer[1] > eyes.width/2 + 1) 
		|| (hand.pointer[2] < -eyes.height/2 ) || (hand.pointer[2] > eyes.height/2 )){
			release_pointer ();
			break;
		}
		break;
	case ROTATE_OBJECT:
		rotatesel ( mmx, my, mz );
		break;
	case SHIFT_OBJECT:
		shift_selection ( mz, mmx, my );
		break;
	}
	stringstream ss;
	matrix_c<FT>vA, base;
	FT np_x, n_b, n_x, n_A;
	object_c::motion.get_object (&vA,&base,&np_x,&n_b,&n_x,&n_A );
	vA.out ( 0, ss );
	base.out ( 0, ss );
	local_position.out ( 0, ss );
	mx.out ( 0, ss );	
	hand.pointer.out ( 0, ss );
}

matrix_c<FT> lamb_c::view_to_world ( matrix_c<FT>& pt ){
	matrix_c<FT> point = pt;
	point = mx*point;
	point = point+local_position;
	matrix_c<FT> base = motion.object_base ();
	point = base*point;
	matrix_c<FT> vA = motion.object_vector ( 1 );
	point = point+vA;
	return point;
}

matrix_c<FT> lamb_c::world_to_view ( matrix_c<FT>& pt ){
	
	matrix_c<FT> point = pt;
	matrix_c<FT> vA = motion.object_vector ( 1 );
	point = point-vA;
	matrix_c<FT> base = motion.object_base ();
	point = ~base*point;
	point = point-local_position;
	point = ~mx*point;
	return point;
}

matrix_c<FT> lamb_c::view_to_object ( object_c& object, matrix_c<FT>& m ){
	matrix_c<FT> point = m;
	point = mx*point;
	point = to_object ( object, point );
	return point;
}

matrix_c<FT> lamb_c::view_from_object ( object_c& object, matrix_c<FT>& m )
{
	matrix_c<FT> point = m;
	point = from_object ( object, point );
	point = ~mx*point;
	return point;
}

void lamb_c::matrices_to_view ( object_c& object, matrix_c<FT>* base, matrix_c<FT>* translation )
{
	matrix_c<FT> a, b, c, A, B, C;
	a = object.motion.object_vector ( 1 );
	A = object.motion.object_base ();
	b = motion.object_vector ( 1 );
	B = motion.object_base ();
	c = local_position;
	C = mx;
	*base = ~A*B*C;
	*translation = ~A*((B*c ) + b - a );
}

void lamb_c::coocked_to_local ( object_c* o, matrix_c < FT > & lbase, matrix_c <FT >& lvA, int& flag)
{
	if ( o -> get_type () == EDITOR ) {
		matrix_c < FT > base = motion.object_base ();
		matrix_c < FT > vA = motion.vA;
		matrix_c < FT > obase = o -> motion.object_base ();
		matrix_c < FT > ovA = o -> motion.vA;
		lbase = base*obase;
		lvA = vA + base*ovA;
		flag = 0;
	}
};

void export_image2 ( lamb_c& lamb, string path, int zoom )
{
	if ( path.size () < 5 ){
		return;
	}
	int sysres = 0;
	string suffix = path.substr ( path.size () - 4, 4 );
	if ( suffix != ".ppm" && suffix != ".png" ){
		return;
	}
	string s = path.substr ( 0, path.size () - 4 );
	
	ofstream fout ( s + ".ppm" );
	bool go = false;
	long w, h, dx, dy;
	w = lamb.eyes.frame_width*zoom; 
	h = lamb.eyes.frame_height*zoom;	
	dx = lamb.eyes.frame_x*zoom; 
	dy = lamb.eyes.frame_y*zoom;
	if(go){			
	/*
		lamb.eyes.push_surface ( lamb );	
		lamb.eyes.change_surface(lamb, 1);
		return;
		*/

		lamb.is_printing = true;	
		zoom = 2;
		zoom = 1;
		lamb.eyes.zoom = zoom;	

		matrix_c<FT> m ({0, (float )lamb.eyes.frame_x, (float )lamb.eyes.frame_y});
		lamb.local_position = lamb.local_position + m;
		
		w = lamb.eyes.frame_width*lamb.eyes.zoom + 1;
		h = lamb.eyes.frame_height*lamb.eyes.zoom + 1;
	
		long ow = lamb.eyes.width,
		oh = lamb.eyes.height;
		

		auto& sf = lamb.eyes.retina.surfaces.front();
		if( sf.width != lamb.eyes.width || sf.height != lamb.eyes.height){
			echo << "error width\n";
		}
		else{
			cout << "width ok\n";
		
		}
		
//		lamb.eyes.width = w;
//		lamb.eyes.height = h;
		lamb.eyes.retina.surfaces.pop_front();
		lamb.eyes.push_surface ( lamb );	
//		lamb.eyes.change_surface(lamb, 1);
	//return;

//		lamb.is_printing = false;
		auto& sf2 = lamb.eyes.retina.surfaces.front();
		if(sf2.width!= ow || sf2.height!= oh){
			echo << "errorrrr\n";
			cout << "errorrrr\n";
		}	
//		lamb.eyes.width = ow;
//		lamb.eyes.height = oh;
		return;

		land.impress ();
		lamb.is_printing = false;
		return;
	}	
	fout << "P6\n" 
	<< w << '\n'
	<< h << '\n'
	<< 255 << '\n';
	
	cout << w << ":" << h << endl;
	for ( int y = (h-1 )/2; y >= -h/2;y--){
		for ( int x = -w/2; x<w-w/2; x++){
			long color = lamb.eyes.get_pixel ( dx+x, dy+y );
				fout.write ((char*)& color, 3 );
		}
	}
	fout.close ();
	if(go){
	}
	
	if ( suffix != ".png" ){
		return;
	}
	
	/*
	string command;
	command = "pnmtopng " + s + ".ppm " + "> " + s + ".png";
	sysres = system ( command.c_str ());
	cout <<  sysres << '\n';
	*/
	echo << "export image\n";
}

void save_file ( string str )
{
	echo << "save file " << endl;
	fstream sfile ( str + ".grf", ios::out );
	echo << str + ".grf" << endl;
	for ( auto o : land.llist ){
		o->serialize ( sfile, true );
	}
	sfile.close ();
	lamb_c::self -> grf_file = str; 
}

void load_file ( string str )
{
	object_c* pobj;
	lamb_c::self -> grf_file = str;
	if ( str == "" ) {
		return;
	}
	ifstream ifile ( str + ".grf" );
	if ( ! ifile.good () ){
		echo << "can't open file:" << str + ".grf" << endl;
		return;
	}
	ifile.close ();
	fstream file ( str + ".grf" );
	echo << "load grafic file: " <<  str + ".grf" << endl;

	while ( file.good ()){
		uint32_t ty;
		string s;		
		file >> s;
		if (s == "EDITOR" ){
			pobj =  new spline_c ();
			pobj->serialize ( file, false );
			land.llist.push_back ( pobj );
		}
		else if ( s == "SPLINE" ){
			pobj =  new spline_c ();
			pobj->serialize ( file, false );
			land.llist.push_back ( pobj );
		}
		else if ( s == "CARTESIAN" ){
			pobj =  new cartesian_c ();
			pobj->serialize ( file, false );
			land.llist.push_back ( pobj );
		}
		else if ( s == "IMAGE" ) {
//			pobj =  new image_c ();
			pobj->serialize ( file, false );
			land.llist.push_back ( pobj );
		}
	}
	file.close ();
}

void new_cartesian ()
{
	cartesian_c* cartesian = new cartesian_c (); 
	
	lamb_c& lamb = *lamb_c::self;
	lamb.local ( *cartesian, lamb.hand.pointer, { 0, 0, -1 }, { 1, 0, 0 } );
	
	cartesian -> vA = { 0, 0 ,0 };
	cartesian -> vvx = { 100, 0, 0 };
	cartesian -> vvy = { 0, 100, 0 };
	cartesian -> vvz = {  0,  0, 100};
	/*
	matrix_c < FT > vvA = { 100, 0, 0 }, vvb = { 0, 1, 0 }, vvx = { 0, 0, 1 };
	cartesian -> tx.motion.set_object ( vvA, vvb, vvx );
	vvA = { 0, 100, 0 };
	cartesian -> ty.motion.set_object ( vvA, vvb, vvx );
	vvA = { 0, 0, 100 };
	cartesian -> tz.motion.set_object ( vvA, vvb, vvx );
	*/
	land.llist.push_front ( cartesian );
	lamb.sensor = cartesian;
	cartesian->draw();
}

void new_spline ()
{
	lamb_c& lamb = *lamb_c::self;
	spline_c* spline = new spline_c (); 
	matrix_c<FT> m = lamb.hand.pointer;
	m = lamb.view_to_world ( m );
	spline->control_points = { m};
	spline->order = 1;	
	spline->uniform = true;
	spline->open_uniform = true;
	spline->color = 0x0000ff;
	spline->doted = false;
	spline->accept_points = true;	
	land.llist.push_front ( spline );
	lamb.sensor = spline;
}

void new_editor ()
{
	echo << "new editor" << endl;
	editor_c* e = new editor_c ( );
	e->simple_text = false;
	lamb_c& lamb = *lamb_c::self;
	e->layen.signo = 4;
//	lamb.local (*e, lamb.hand.pointer, {1, 0, 0}, {0, 1, 0} );
	e->frame_height = 4;
	e->frame_width = 10;
	e->layen.resize ( 
		e->frame_width*e->layen.font.cell, 		
		(e->frame_height+1)*e->layen.font.cell,
		e->frame_height
	);
	e->impression_clients.retina = &lamb.eyes.retina ;
	string st {"hallo"};
	e->layen.set_text (st);
	e->layen.set_caret();

//	land.insert ( e);
	land.llist.push_front ( e);
}

void new_image ( string s )
{
	echo << "new image: " <<  s << endl;
	image_c* p_image;
	p_image = new image_c ( s );
	lamb_c* pl = lamb_c::self;
	pl -> local (*p_image, pl -> hand.pointer, {0, 0, -1}, {1, 0, 0} );
	land.llist.push_front ( p_image );
}

void new_wobject ( string s ) 
{
	echo << "new wobject: " << s << endl;
	char* newarg [3] ;
	static char c[100];
	strcpy ( c , s.c_str());
	newarg [0 ] = c;
	if ( 0 ) {
		static char chs [100];
		strcpy (chs, "/home/henry/shelf/cpp/wss/desk/upclose.ppm");
		newarg [ 1 ] = chs;
		newarg [ 2 ] = NULL;
	} 
	else {
		newarg [ 1 ]  = NULL;
	};
	char* newenv [] = {NULL};
	pid_t pid = fork ();
	if ( pid == 0 ) {
		execve ( newarg [0], newarg, newenv );
	}
	wobject_c* p;
	p = new wobject_c ( s );
	p -> object_id = land.get_handle();
	lamb_c* pl = lamb_c::self;
	pl -> local (*p, pl -> hand.pointer, {0, 0, -1}, {1, 0, 0} );
	land.llist.push_front ( p );
}

void ffind ( lamb_c& lamb, matrix_c<FT> pt )
{
	object_c* pobj;	
	list<object_c*>::iterator it;
	uint32_t m = lamb.keyboard.get_controls ();
	for ( it = land.llist.begin (); it!= land.llist.end (); it++){
		pobj = *it;
		if ( pobj->find ( lamb, pt )){
			if ( m == LL_KA_LC ){
				lamb.sensor = *it;
			}
			else{
				if ( pobj->state != 0 ){
					pobj->state = 0;
				}
				else{ 
					pobj->state = 1;
				}
			}
		}
	}
}

void lamb_c::focus ( bool gained )
{
	keyboard.stroke_list.clear ();	
	if ( ! gained ) {
		release_pointer ();
	}
}

void lamb_c::grab_pointer ( int x , int y )
{
	hand.pointer[1] = x - eyes.width/2;
	hand.pointer[2] = eyes.height/2 - y;
}

void lamb_c::release_pointer ()
{
	int x = hand.pointer [ 1 ] + eyes.width/2;
	int y = eyes.height/2 - hand.pointer [ 2 ];
	ungrab_system_cursor ( x, y );
}

void lamb_c::mouse_move ( int x, int y )
{
	mouse.inertia (&x, &y, 0 );
	move ( movement, x, y, 0 );
	hand.draw ();
	if ( is_idle () ){
		hand.draw ();
	}
}

void lamb_c::idle ()
{
/*
	for ( auto x : land.llist ) {
		if ( x->get_type() == IMAGE ) {
			x->draw();
		}
	}
*/
	for ( auto x : land.llist ) {
		x->draw();
		if ( ! is_idle() ) {
			expose_image();	
			return;
		}
	}
//	draw();
//	if ( ! eyes.retina .surfaces.front().is_valide ){
//		land.impress();
		expose_image();	
		eyes.retina .surfaces.front().is_valide = true;
//	}
}

void lamb_c::timer ()
{
//	cout << "timer" << endl;
	if ( stream_in ) {
		eyes.retina .stream_in2 ();
	}
	
}

void lamb_c::notify(string& note)
{
	echo << note << '\n';
	if(note=="CLIPBOARD"){
		string clip;
		get_clipboard(clip);
		echo << clip << '\n';
		set_clipboard(clip);
		for(auto x : land.llist){
			if(x->get_type()== EDITOR){
				editor_c* pe=(editor_c*)x;
				pe->editing(PASTE);	
				pe->moving(NEW_LINE_D, 0);
				pe->moving(NEW_LINE_D, 0);
			}
		}
	}
}

void lamb_c::write_status ()
{
	status.clear ();
	stringstream ss;
	matrix_c < FT > m = motion.object_base ();
	m.out ( 3, ss );
	m = ~motion.object_vector ( 1 );
	m.out ( 3, ss );
	ss << "pointer world ";	
	m = ~view_to_world ( hand.pointer );
	m = land.rasterize ( m );
	m.out ( 1, ss );
	string s;
	get_cwd ( s );
	ss << "pointer z:" << hand.pointer [3] << endl
	<< "raster: " << land.raster << endl <<endl
	<< "work directory: " << s << endl
	<< "grafic file: " << grf_file << endl
	<< "project: " << project << endl;
	string st;
	status.layen.set_text ( st = ss.str () );
	status.layen.set_caret();
}

void lamb_c::key_event ( bool pressed, unsigned short sym, unsigned char c )
{
	write ( pressed, sym );
}

void lamb_c::expose ( int* px, int* py, char** pcp, retina_c** retine )
{
	*px = eyes.width;
	*py = eyes.height;
	*pcp = 0;
	*retine = &eyes.retina ;
}


void lamb_c::button_pressed ( int number )
{
	if ( none_event ) {
		if ( ! no_refresh ) {
//			land.impress ( *this );
		}
		return;
	}
	if (number == 4 || number == 5 ){
		int s = number == 4 ? -1 : 1;
		move ( movement, 0, 0, s );
//		land.impress (*this );
	}
	else if ( number == 1 ){
		if ( figure == SPLINE ){
			if ( sensor != 0 ){
				if ( !sensor->set_point (*this ) ){
					new_spline ();
//					sensor = land.llist.back ();
				}
			}
			else{
				new_spline ();
			}
//			pointer = land.rasterize ( pointer );
//			nline.setpoint ( *this, false, false, true, false );
		}
		else if ( figure== CARTESIAN ){
			new_cartesian ();
		}
		else if (figure == EDITOR ) {
			new_editor ();
		}
		else if ( figure == IMAGE ) {
			new_image ( "" );
		}
	}
	else if ( number == 2 ){
		echo << "button 2" << endl;
		ffind (*this, hand.pointer );
	}
}

void lamb_c::config_change ( int x, int y )
{
	eyes.set_size ( *this, x, y );
	list < object_c* > lst;
	home2.home ( home , *this, lst);
	if ( start_command != "" ){
		command ( start_command );
		start_command = "";
	}

	for ( auto object : land.llist ){
		object->invalide_visual ( 0 );
	}
}

bool finde ( string& s, size_t i, string& file_name, int * line, int * column )
{
	if ( false ) {
		s = " sophie\nmachine/lamb.cpp:379:46: error ichaud juli ";
	//	i = s.size ();
	}
	size_t ii;
	echo << "string size:" << i << endl;
	reg_exp_c regex;
	string find;
	string sm1,sm2,sm3;
	i = 0;
	ii = 0;
	bool res = true;
	string search = "\\d+";
	while ( res ) {
		ii = i;
		res = regex.find ( s, ii , 0 , 0,  search, find );  
		if ( ! res ) { 
			break;
		}
		sm1 = find;
		i = ii + find.size ();
	}
	echo << "s1:" << i << endl;
	s = s.substr ( 0, i - find.size () );
	
		echo << endl << "str2:" << s << endl;
	i = 0;
	ii = 0;
	res = true;
	while ( res ) {
		ii = i;
		res = regex.find ( s, ii , 0 , 0,  search, find );  
		if ( ! res ) { 
			break;
		}
		sm2 = find;
		i = ii + find.size ();
	}
	s = s.substr ( 0, i - find.size () );
	echo << endl << "str3:" << s << endl;
	i = 0;
	ii = 0;
	res = true;
	search = "[-_/1-9A-Za-z.]+";
	while ( res ) {
		ii = i;
		res = regex.find ( s, ii , 0 , 0,  search, find );  
		if ( ! res ) { 
			break;
		}
		sm3 = find;
		i = ii + find.size ();
	}
	*line = stoi ( sm2 );
	*column = stoi ( sm1 );
	file_name = sm3;	
	echo << endl << "finde:" << sm3 << "|" << sm2 << "|" << sm1  << endl; 
} 

void lamb_c::resize_fonts ( float f )
{
	mout << "resize fonts\n";
	echo << "resize fonts\n";
	auto it = land.llist.begin ();
	float ff;
	for (; it != land.llist.end (); it++){
		if ((*it )->get_type () == EDITOR ){
			if ((*it )->parent == 0 || is_printing ){
				ff = f;
			}
			else {
				ff = 1;
			}
			ff = f;
			editor_c* pe = (editor_c*)*it;
//			pe->layen.clear_pixel_vectors();
//			pe->layen.clear_glyph_bitmap();
			pe->layen.face = pe->layen.font.set_face ("Vera-code.ttf", 13, f);
			pe->ilayen.face = pe->layen.face;
			pe->layen.clear_texel_map();
			pe->ilayen.clear_texel_map();
			mout << "editor\n";
		}
	}	
}

int lamb_c::command2 ( unsigned char c, string& m, string& s )
{
	return command ( s );
}

void ttt();

void lamb_c::breakpoint()
{
	cout << "break 1\n";
	cout << "break 2\n";
}

void lamb_c::debug( string str)
{
	auto env = environment();
	if ( env.empty()){
		return;
	}
	auto context = env.front();
	echo << "context: " << context << '\n';
	stringstream ss{str};
	string s, s1, s2, s3;
	ss >> s >> s1 >> s2 >> s3;
	
	if( get_lambs_path() == "/home/henry/shelf/cpp/clone"){
		cout << "in debugee\n";
		if ( s1 == "bkpt"){
			breakpoint();
		}
		return;
	}
	if ( s1 == "1" ) {
		d.f1 = stoi ( s2 ) ;  
	}
	else if ( s1 == "2" ) {
		d.f2 = stoi ( s2 );
	}
	else if ( s1 == "3" ) {
		d.f3 = stoi (s2);
	}
	else if ( s1 == "4"){
		int v = stoi(s2);
		d.caret_check = v;
		echo << "caret check: " << d.caret_check << '\n';
	}
	else if (s1 == "5"){
		d.rel_a = stoi(s2);
		echo << "rel_a: " << d.rel_a << '\n';
	}
	else if (s1 == "6"){
		d.rel_b = stoi(s2);
		echo << "rel_b: " << d.rel_b << '\n';
	}
	else if (s1 == "10"){
		d.f10 = stoi(s2);
		echo << "f10: " << d.f10 << '\n';
	}
	else if ( s1 == "clone"){
		string path = "/home/henry/shelf/cpp";
		string lamb = path + "/wss/build/";
		if ( context == "x11"){
			lamb += "x11/lamb";
		}
		else if ( context == "linux"){
			lamb += "linux/lamb";
		}
		stringstream ss;
		string com = "cp -v " + lamb + " " + path + "/clone/lambd";
		system_echo(com, ss);
		echo << ss.str() << '\n';
		ss.str("");
		com = "cp -v " + path + "/wss/config/environment " + path + "/clone/config";
		system_echo(com, ss);
		echo << ss.str() << '\n';
	}
	else if ( s1 == "bkpt"){
		breakpoint();
	}
	else if ( s1 == "b" ){
		string st{"cd /home/henry/shelf/cpp/cs && ./a"};
		stringstream ss;
		system_echo(st, ss);
		echo << ss.str()<<'\n';
	}
	else if ( s1 == "bb" ){
		string st{"cd /home/henry/libmigdb && ./a"};
		stringstream ss;
		system_echo(st, ss);
		echo << ss.str()<<'\n';
	}
	else if ( s == "dbc") {
		d.runc(s1 + " " + s2);
	}
	else if (s == "dbcc" ){
		d.runcc(context + " " + s1 + " " + s2);
	}
	else if ( s == "db"){
		d.gdb(context + " " + s1 + " " + s2 + " " + s3);
	}
	else if ( s == "dbs"){
		d.send(s1 + " " + s2);
	}
	else if ( s == "dbj"){
		d.send("job " + s1);
	}
}

using namespace chrono;
bool lamb_c::command ( string str )
{
	stringstream ss ( str );
	string s, s1 , s2 , s3;
	ss >> s >> s1 >> s2 >> s3;
	ss.clear ();
	ss.str (str );
	if ( s.substr(0, 2) == "db"){
		debug(str);
		return 0;
	}
	if ( s == "sc" ){
		echo << "start c:" ;
		command ( start_command );
	}
	else if ( s == "setsc" ) {
		string sc = s1 + " " + s2 + " " + s3;
		echo << "set start command:" << sc << endl;
		start_command = sc;
	}
	else if ( s == "cd" ){
		echo << "cd: " << s1 << endl;
		if ( s1 != "" ) {
			shell_c shell;
			shell.envstr ( s1 ); 
			change_pwd ( s1 );
		}
		echo << s1 << endl;
		get_cwd ( directory );
		echo << directory << endl;
	}
	else if ( s == "project" ){
		if ( s1 != "?" ){
			project = s1;
		}
		echo << project << endl;
//		land.impress (*this );
	}
	else if ( s == "hand"){
		if (s1 == "off"){
			hand.off = true;		
		}
		else {
			hand.off = false;
		}
	}
	else if ( s == "eyes"){
		if (s1 == "off"){
			eyes.off = true;		
		}
		else {
			eyes.off = false;
		}
	}
	else if ( s == "ed" ){
		editor_c* editor = nullptr;
		for ( auto x : land.llist ) {
			if ( x -> get_type () == EDITOR && x != &mouth ) {
				editor =  ( editor_c* ) x;
				break;
			}
		}
		if ( editor == nullptr ) {
			return 0;
		}
		string dum;		
		s = ":e " + s1;
		echo << "s:" << s << endl;
		editor -> command2 ( XK_Return, dum, s );
		return 0;		
	}
	if ( s == "e" ){
		stringstream ss (str );
		string s;
		ss >> s;
		testing ( ss );
	}
	if ( s == "ip" ){
		is_printing = ! is_printing;
		resize_fonts ( eyes.zoom );
	}
	if ( s == "base" ){
		land.show_base = ! land.show_base;
	}   
	if ( s == "clear" ){
		echo.clear ();
	}
	if ( s == "ras" ){
		land.raster = land.raster == 0?10:0;
	}    
	if ( s == "d" ){
		echo << "remove selection" << endl;
		land.remove_selection ();
	}
	if ( s == "ase" ){
		echo << "new editor" << endl;
		new_editor ();
	}
	if ( s == "ass" ){
		echo << "new spline" << endl;
		new_spline ();				
	}
	if ( s == "asb" ) {
		echo << "browser " << endl;
		char* newarg [] = {"/home/henry/shelf/cpp/wss/ans", NULL};
//		char* newarg [] = {"/home/henry/netsurf-full-3.0/src/netsurf-3.0/nsfb", "-f", "linux", NULL } ;
		char* newenv [] = {NULL};
		pid_t pid = fork ();
		if ( pid == 0 ) {
			cout << " execve" << endl;
			int res = execve ( newarg [0], newarg, newenv );
			cout << "errror" << endl;
		}
		this_thread::sleep_for( milliseconds { 300 } );
		new_image ( ".map" );
	}
	if ( s == "ast" ) {
		echo << "ast y\n";
		string es { "cd $mol" };
		shell_c shell;
		shell.envstr ( es );
		echo << es;
	}
	if ( s == "screen" ) {
		if ( s1 == "full" ) {
			window_management ( "full_screen yes" );
		}
		else {
			window_management ( "full_screen no" );
		}
		exit(wcp::restart);
	}
	else if(s == "binhex"){
		vector<char> v;
		char ch;
		ifstream ifs(s1);
		for(;;){
			ifs.read(&ch,1);
			if(ifs.eof()){
				break;
			}
			v.push_back(ch);
		}	
		stringstream ss;
		binary_to_text(v, ss);
		ofstream ofs(s2);
		ofs << ss.str();
	}
	else if ( s == "pdf" ) {
		stringstream ss0;
		echo << "pdf..." << endl;
		string tmp = get_lambs_path() + "/tmp";
		string st, st0;
		if ( s1.size() > 4 && s1.substr ( s1.size() - 4 ) == ".pdf" ) {
			st = s1.substr ( 0, s1.size() - 4);
		}
		else {
			st = s1;
			s1 += ".pdf";
		}
//		scale ( 300, 773, 205, 205 ,120, 90, st0);
		scale ( eyes.height+350, 773, 205, 205 ,120, 90, st0);
//		scale ( eyes.height-2, 773, 190, 190 ,120, 90, st0);
//		scale ( eyes.height-20, 773, 190, 190 ,120, 90, st0);
//		scale ( eyes.height-20, 709, 300 , 50 , 50, 50, st0);
		string cmd = "pdftoppm " + st0 + " " + s1 + " " + tmp + "/" + st;
		echo << cmd << endl;
		system_echo ( cmd, ss0 );
		ifstream ifs ( tmp + "/" + st+"-1.ppm");
		if (ifs ) {
			st0 = "-1";
			ifs.close ();
		}		
		else {
			st0 = "-01";
		}
		cmd = "asv " + tmp + "/" + st + st0 + ".ppm";
		command ( cmd );
	}

	if ( s == "asv" ) {
		shell_c shell;
		shell.envstr ( s1 );
		echo << "aassv: " << s1 << endl;
		if ( s1 == "e" ) {
			eyes.retina .stream_out();
			return 1;
		}
		if ( s1 == "w" ) {
			new_wobject ( "/home/henry/shelf/cpp/wss/wv/linux" );
			return 1;
		}
		if ( s1 == "t" ) {
			for ( auto e : land.llist ){
				echo << e->object_id << endl;
			}
			eyes.retina.test ( 0 ,"" );
			return 1;
		}
		echo << "viewer " << endl;
		
		string lbsp = get_lambs_path();		
		ifstream ifs (lbsp + "/config/environment"); 		
		string ws;
		getline ( ifs, ws );
		ws = lbsp + "/build/" + ws + "/wv/viewer";
		if ( s1 == "ll" ) {
			s1 = "/home/me/desk/cpp/wss/desk/upclose.ppm";
		}
//		string st2{"LL_PATH=/home/henry/shelf/cpp/wss LD_LIBRARY_PATH=/home/henry/usr/lib64"};
		string st2{"LD_LIBRARY_PATH=/home/henry/usr/lib64 LL_PATH=/home/henry/shelf/cpp/wss"};
		string st1 = ws + " " + s1;
		system_async_run(st1, st2);	
		return 0;
	}
	
	if (s == "g" or s == "gdb"){
		static FILE* f = nullptr;
		if ( s == "g"){
			string st = s1 + " " + s2;
			fwrite(st.c_str(), 1, st.size()+1, f);
			return 0;
		}
		if ( s1 == "m"){
			d.run();
			return 0;
		}
		if ( s1 == "n"){
			echo << "next\n";
			string st{"n"};
			fwrite(st.c_str(), 1, st.size()+1, f);
			return 0;
		}

		if ( s1 == "r"){
			echo << "run\n";
			string st{"r"};
			fwrite(st.c_str(), 1, st.size()+1, f);
			return 0;
		}
		if( s1 == "b"){
			string st{"b les1.cpp:5"};
			fwrite(st.c_str(), 1, st.size()+1, f);	
			return 0;
		}
		if ( s1 == "q"){
			string st{"q"};
			fwrite(st.c_str(),1,st.size()+1, f);
			return 0;
		}
		shell_c shell;
		shell.envstr ( s1 );
		echo << "gdb\n";
		const char* newarg[3]{};
	
		//string ws = get_lambs_path();		
		newarg[0] = "gdb";		
		newarg[1] = "/home/henry/shelf/cpp/cs/les1";
		echo << "viewers paths: " << newarg[0] << endl;
		echo << "viewers arg: " << newarg[1] << endl;
//		char * a1{"gdb --tty=/dev/pts/1 /home/henry/shelf/cpp/cs/les1"};
//		char * a1{"gdb -i=mi /home/henry/shelf/cpp/cs/les1 >t0 "};
		char * a1{"gdb /home/henry/shelf/cpp/cs/les1"};
		char * a2{"w"};		
		f = popen(a1,a2);
		return 0;
	}
	if ( s == "ws" ){
		save_file ( s1 );
		echo << s1 << endl; 
	}
	if ( s == "wl" ){
		load_file ( s1 );
	}
	if ( s == "z" ){
//		eyes.zoom = stof ( s1 );
		eyes.retina.surfaces.front().zoom = stof(s1);
//		resize_fonts ( eyes.zoom );
	}
	if ( s == "rf" ){
		resize_fonts ( stof ( s1 ) );
	}
	if ( s == "xg" ){
		float z = eyes.retina.surfaces.front().zoom;
		export_image2 ( *this, s1, z );
		echo << s1 << '\n'; 
	}
	if ( s == "fig" ){
		if ( s1  == "sp" ){
			figure = SPLINE;
		}
		else if ( s1 == "ed" ){
			figure = EDITOR;
		}
		else if ( s1 == "ca" ) {
			figure = CARTESIAN;
		}
		else if ( s1 == "im" ) {
			figure = IMAGE;
		}
		else if ( s1 == "ze" ) {
			figure = ZERO;
		}
	}
	if ( s == "v" ){
		if ( s1 == "" ){
			eyes.push_surface (*this );
		}
		else{
			eyes.change_surface ( *this, stoi ( s ) );
		}	
	}
	if ( s == "w" ){
		movement = movement<3?movement+1:0;
	}
	else if ( s == "cp" ){
		copy_selection ();
	}
	else if ( s == "echo"){
		echo.mode = stoi(s1);	
	}
	else if ( s == "mout"){
		nout.mode = stoi(s1);	
	}
	else if ( s == "ho" ){
		if ( s1 == "?" ) {
			echo << "home is " << home << endl;
		}
		else {
			mouth.layen.clear_texel_map();
			mouth.ilayen.clear_pixel_vectors();
			home = s1;
			list < object_c*> lst;
			home2.home (s1, *this, lst );
		}
	}
	else if ( s == "h0" ){
		mx = {{0, 1, 0},{0, 0, 1},{-1, 0, 0}};
		motion.set_object_vector ( 1,{0, 0, 0});
		motion.set_object_vector ( 2,{1, 0, 0});
		motion.set_object_vector ( 3,{0, 1, 0});
		list < object_c* >::iterator it;
		it = land.llist.begin ();
		for (; it != land.llist.end (); it++){
			if ((*it )->get_type () == EDITOR ){
				editor_c* pe = (editor_c*) *it;
				pe -> layen.clear_pixel_vectors ();
				pe -> layen.clear_texel_map ();
			}
		}	

		hand.pointer = {0, 0, 0};
		local_position = {0, 0, 0};
	}
	else if ( s == "h1" ){
		mx = {{0, 1, 0},{0, 0, 1},{-1, 0, 0}};
		hand.pointer = {0, 0, 0};
		local_position = {-310, 190, 0};
	}
	else if ( s == "raster" ){
		land.raster = stoi ( s1 );
	}
	else if ( s == "mouse" ){
		keyboard_mouse = stoi ( s1 );
		echo << "mousx";
	}
}

void  ttt () {
	ifstream ifs ( "/home/henry/wsh/viewer.vio" );
	uint32_t u;
	int i , i2, i3;
	ifs >> i;
	ifs.seekg ( i );
	ifs >> i2;
	ifs >> i3;

	echo << "draw stream: viewer.vio: " << i << " " <<  i2 << " " << i3  << endl;	if ( i2 == 0 ) {
		return;
	}
	int x, y;
	float z;
	unsigned long color;
	size_t pos = ifs.tellg ();	
	
	fstream os ( "/home/henry/wsh/rin" );
	os << 2 << " "<< 1 << " " << i3 << " ";	
	for ( int c = 0; c < i3; c ++ ) {
		ifs >> dec >> x >> y >> z >> hex >> color;
		os << dec << x << " " << y  << " " << z << " "<< hex <<  color << " ";
	}	
}

void lamb_c::change_movement ( int para1, int para2 )
{
	echo << "change movement: " << para1 << " : " << para2 << endl;
	if ( para2 == 7 ){
		keyboard_mouse = !keyboard_mouse;
	}
	else if ( para2 == 2 ){
		movement = movement<POINT?++movement:SHIFT;
	}
	else if ( para2 == 1 ){
		movement = movement>0?--movement:POINT;
	}
	else if ( para2 == 5 ){
		object_c* &po = sensor;
		if ( po->get_type () == EDITOR ){
			editor_c* ed=(editor_c*)po;
			ed->export_text ( ed->file_path );
		}
		if ( po->get_type () == EDITOR ){
			editor_c* ed = (editor_c*)po;
			ed->import_text ( ed->file_path, true );
		}
	}
//	land.impress (*this );	
}

void lamb_c::to_default ( int a, string s )
{
	mx={{0, 1, 0},{0, 0, 1},{-1, 0, 0}};
	motion.set_object_vector ( 1,{0, 0, 0});
	motion.set_object_vector ( 2,{1, 0, 0});
	motion.set_object_vector ( 3,{0, 1, 0});
	list<object_c*>::iterator it;
	it = land.llist.begin ();
	for (;it!= land.llist.end ();it++){
		if ((*it )->get_type () == EDITOR ){
			editor_c* pe = (editor_c*)*it;
			pe->layen.clear_pixel_vectors ();
			pe->layen.clear_texel_map ();
		}
	}	

	hand.pointer = { 0, 0, 0 };
	local_position = { 0, 0, 0 };
	eyes.zoom = 1;
	resize_fonts ( eyes.zoom );
//	land.impress ( *this );
}

void lamb_c::flush () {
//	land.impress ( *this );
}

void lamb_c::init ()
{
	sensor = this;
	movement = POINT;
	load_file ( grf_file );
}

bool condition ( int category )
{
	lamb_c& lamb = *lamb_c::self;
	if ( category == 0 ){
		return true;	
	}
	else if ( category == 1 ){
		if ( lamb.sensor == &lamb ||
		lamb.sensor == &lamb.status ||
		lamb.sensor == &lamb.mouth ||
		lamb.sensor == &lamb.eyes ||
		lamb.sensor == &lamb.ears   
		){
			return false;
		}
		else{
			return true;
		}
	}
	else if ( category == 2 ){
	if ( lamb.sensor == &lamb ||
		lamb.sensor == &lamb.status ||
		lamb.sensor == &lamb.mouth ||
		lamb.sensor == &lamb.eyes ||
		lamb.sensor == &lamb.ears  
		){
			return true;
		}
		else{
			return false;
		}
	}
}

list<object_c*>::iterator
lamb_c::next_object ( list<object_c*>& l, object_c*& sou, int category )
{
	list<object_c*>::iterator it, it_begin;
	it_begin = it = std::find ( l.begin (), l.end (), sou );
	if ( it != l.end ()){
		do{
			it++;
			if ( it == l.end ()){
				it = l.begin ();
			}
			sou = *it;
		}while ( it != it_begin && condition ( category ) == false );
	}
}

list<object_c*>::iterator 
lamb_c::previous_object ( list<object_c*>& l, object_c*& sou, int category )
{
	list<object_c*>::reverse_iterator it, it_begin;
	it_begin = it = std::find ( l.rbegin (), l.rend (), sou );
	if ( it != l.rend ()){
		do{
			it++;
			if ( it == l.rend ()){
				it = l.rbegin ();
			}
			sou = *it;
		}while ( it != it_begin && condition ( category ) == false );
	}
}

void lamb_c::separate ( object_c* o ) 
{
	for ( auto it = mobilar.begin() ; it != mobilar.end() ; it++ ) {
		if ( *it == o ) {
			mobilar.erase ( it );
			break;			
		}
	}
}

int lamb_c::ctab ( stringstream& ss ) 
{
	string s1,s2 ;
	ss >> s1 >> s2;
//	echo << "ctab: " << ss .str () << endl;
//	echo << "s1, s2: " << s1 << " : " << s2  << endl;
	comtab1_lamb_c& t = lamb_c::comtab1_lamb;
	comtab2_lamb_c& t2 = lamb_c::comtab2_lamb;
	if ( s1 == "1" ) {
		if ( s2 == "d" ) {
			t.destruct ();
		}
		else if ( s2 == "du" ) {
			t.dump ( t.msc ); 
			 
		}
		else {
			t.construct();
		}	
	}
	else if ( s1 == "2" ) {
		if ( s2 == "d" ) {
			t2.destruct ();
		}
		else if ( s2== "du" ) {
			echo << "comtab " << endl;
			echo << "command " << endl;
		 	t.dump ( t2.msc  ); 
		}
		else  {
			t2.construct();
		}	
	}
}

const int step = 10;

string comtab1_lamb_c::ct = R"***(
class lamb
c , nm , xk_slash	;	change_movement 0 7 
c , nm , xk_comma	;	change_movement 0 1
c , nm , xk_period	;	change_movement 0 2
)***";

string comtab2_lamb_c::ct = R"***(
class lamb
p , nm , xk_h		;	move point -10 0 0
p , nm , xk_l		;	move point 10 0 0
p , nm , xk_j		;	move point 0 0 10
p , nm , xk_k		;	move point 0 0 -10
p , nm , xk_i		;	move point 0 10 0
p , nm , xk_m		;	move point 0 -10 0
s , nm , xk_h		;	shift	&lamb.sensor		-1 	0	0
s , nm , xk_l		;	shift &lamb.sensor 	1 	0	0
s , nm , xk_j		;	shift &lamb.sensor 	0	0	-1
s , nm , xk_k		;	shift &lamb.sensor  	0	0	1
s , nm , xk_i		;	shift &lamb.sensor  	0	1	0
s , nm , xk_m		;	shift &lamb.sensor 	0	-1	0
r , nm , xk_h		;	rotate &lamb.sensor -5 0 0
r , nm , xk_l		;	rotate &lamb.sensor 5 0 0
r , nm , xk_j		;	rotate &lamb.sensor 0 0 5
r , nm , xk_k		;	rotate &lamb.sensor 0 0 -5
r , nm , xk_i		;	rotate &lamb.sensor 0 5 0
r , nm , xk_m		;	rotate &lamb.sensor 0 -5 0
p , nm , xk_y		;	button_pressed 1 
p , nm , xk_u		;	button_pressed 2
p , nm , xk_n		;	button_pressed 3
p s r so ro , nm , xk_slash	;	change_movement 0 7
p s r so ro , nm , xk_comma ;		change_movement 0 1
p s r so ro , nm , xk_period ;		change_movement 0 2
)***";

void lamb_c::write ( bool is_pressed, uint16_t stroke ){
	
	if ( sensor == 0 ){
		echo << "active object is null" << endl;
		sensor = this;
	}
	
//	cout << stroke << endl;		
	keyboard.on_key ( is_pressed, stroke );
	if ( is_pressed == false ){
		if ( none_event ) {
//			land.impress ( * this );
		}
//		draw ( *this );
//		cout << "lamb write return\n";
		return;
	}

	if ( hook != nullptr ) {
		hook -> edit (*this , keyboard );
		echo << "hook" << endl;
		if ( keyboard.get_char () == string ("e" )){
			hook = nullptr;
		}
		return;
	}
	if ( stroke == XK_F3 ){
		string s { "db 3 0 "};
		command ( s );		
		to_default ( 0,"" );
		return;
	}
	if ( stroke == XK_F4 ){
		string s { "db 3 1 "};
		command ( s );		
		to_default ( 0,"" );
		return;
	}
	if ( stroke == XK_F5 ) {
		none_event = ! none_event;
		if ( none_event ) {
			echo << "event no" << endl;
		}
		else {
			echo << "event yes" << endl;
		}
	}
	if ( stroke == XK_F9 ) {
		land.impress();
		return;		
		no_refresh = ! no_refresh;
		if ( no_refresh ) {
			echo << "refresh no" << endl;
		}
		else {
			echo << "refresh yes" << endl;
		}
	}
	unsigned long v = keyboard.get_stroke ();
	string c = keyboard.get_char ();
	unsigned long m = keyboard.get_controls ();
	string stroke_semantic = keyboard.get_stroke_semantic ();	
	if ( keyboard.is_pressed ( XK_Alt_L ) 
		&& keyboard.is_pressed ( XK_Control_L ) 
		&&  ( v == XK_F1 || v == XK_F7 )
		&& is_pressed )
	{
		echo << "none" << endl;
		none_event = ! none_event;
	}
	if ( none_event ) {
		if ( ! no_refresh ) {		
//			land.impress ( *this );
		}
		return ;
	}
	if ( keyboard_mouse ){
		if ( parrot & 4 ) {
			lamb_c::comtab2_lamb.find ( this, movement, m, v, c, stroke_semantic );	
		}			
		
		for ( auto x : land.llist ) {
			if ( x -> get_type() == IMAGE ) {
				((image_c*) x)  -> transformation.resize ( 0 );
			}
		}
		eyes.set_superface ( 0xffffff );
		land.impress ( );
		return;
	}
	int shema = 2;
	if (shema == 1 ){
		if ( !edit ( *this, keyboard ) ) {
			object_c* object = sensor;
			if ( motor == &mouth ) {
				object = &mouth;
			}
			object->edit ( *this, keyboard );
		}
	}
	else if ( shema == 2 ){
		object_c* object = sensor;
		if ( motor == &mouth ){
			object = &mouth;
		}
		if ( !object->edit ( *this, keyboard ) ){
			if ( object != this ){
				edit (*this, keyboard );
			}
			object->invalide_visual ( 0 );
			land.invalide_all_objects ( 0 );
		}
	}
	return;
}

int lamb_c::edit ( lamb_c& lamb, keyboard_c& keyb )
{
	uint16_t v = keyboard.get_stroke ();
	string c = keyboard.get_char ();
	uint32_t m = keyboard.get_controls ();
	string stroke_semantic = keyboard.get_stroke_semantic ();
	bool proceeded = false;	
	object_c 
	* old_sensor = sensor, 
	* old_motor =  motor;
	for (;;){
		if ( m == LL_KA_RC && v == XK_s ){
			motor->impression_clients.is_motor = false;
			next_object ( motors, motor, 0 );
			motor->impression_clients.is_motor = true;
			lamb.mouth.invalide_visual ( 0 );
			land.invalide_all_objects ( 0 );
//			land.impress (*this );		
			
			proceeded = true;
			break;
		}
		else{
			if ( motors.front () != motor ){	
				motors.remove ( motor );
				motors.push_front ( motor );
			}
		}	
		if ( m == LL_KA_RC && (v == XK_space || v == XK_Tab )){
			if ( condition ( 1 )  ) { 
				sensor->impression_clients.has_focus = false;
				if ( v == XK_space ){
					next_object ( land.llist, sensor, 1 );
				}
		  		else {
					previous_object ( land.llist, sensor, 1 );
				}
				sensor->impression_clients.has_focus = true;
				if ( motor == &mouth ) {
					motor->impression_clients.is_motor= false;
					next_object ( motors, motor, 0 );
					motor->impression_clients.is_motor=true;
					motors.remove ( motor );
					motors.push_front ( motor );
				}
			}
			else{
				sensor->impression_clients.has_focus = false;
				if ( sensor == &lamb.ears ) {
					sensor = &lamb.status;
				}
				else if ( sensor == &lamb.status ) {
					sensor = &lamb.eyes;
				}
				else if ( sensor == &lamb.eyes ) {
					sensor = &lamb;
				}
				else {
					sensor = &lamb.ears;
				}
				sensor->impression_clients.has_focus = true;
			}
			
			land.invalide_all_objects ( 0 );
			break;
		}
		else if ( m == LL_KA_RC && v == XK_a ) {
			if ( condition ( 2 ) ) {
				sensor->impression_clients.has_focus= false;
				sensor = land.llist.front () ;
				sensor->impression_clients.has_focus= false;
				next_object ( land.llist, sensor, 1 );
				sensor->impression_clients.has_focus= true;
				echo << "a" << endl;
			}
			else{
				sensor->impression_clients.has_focus= false;
				sensor = &lamb.ears;
				sensor->impression_clients.has_focus= true;
			}
			land.invalide_all_objects ( 0 );
			break;
			return true;
		}
		else{
			if ( condition ( 1 ) ) {
				if ( land.llist.front () != sensor ){
					land.llist.remove ( sensor );
					land.llist.push_front ( sensor );
				}
			}
		}
		
		if ( parrot & 2 ) {
			lamb_c::comtab1_lamb.find ( this, COMMAND, m, v, c, stroke_semantic );	
		}
		break;
	}		
	
	if ( old_sensor != sensor ) {
		old_sensor->draw();
		sensor->draw();	
		mout << "mm\n";
	}
	if ( old_motor != motor ) {
		old_motor->impression_clients.is_motor = false;
		old_motor->draw();
		mout << "mm\n";
		motor->draw();
	}		
	return proceeded;
}

void lamb_c::testing ( stringstream& ss )
{
	string s;
	static int d = 2;
	ss >> s ;
	echo << "testing: " << s << '\n';
	if ( s.substr ( 0, 2 ) == "--" ) {
		s = s.substr (2,2);
		d = stoi ( s );
	}
	else {
		ss.seekg ( 0 );
		ss >> s;
	}
	switch ( d ){
	case 0: 
		ex_entry_editor ( ss );
		break;
	case 1: 
		ex_entry_echo ( ss );
		break;
	case 2:
		ex_entry_lamb(ss);
		break;
	case 4:
		ex_entry_comtab4 ( ss );
		break;	
	}
}


extern char **environ;
#include <regex>

namespace ex_lamb
{
	string book_dir{"/home/me/7.6-systemd/errata"};
//	string book_dir{"/home/henry/7.6-systemd/errata/20150206-systemd"};
	string wl_dir{"/home/me/desk/cpp/wss/machine/wl"};
	string is_test{"no"};
	string chroot{"no"};
	string root_dir{"/"};
	string awl_dir{wl_dir};
	string sources_dir{"/mnt/lfs/sources"};
	string pw_dir{""};
	string script{"script"}; 
	string default_page{"chapter06/bc.html"};
	string page = book_dir + "/"+default_page;
	
	string title;	
	string package;
	string untared_name;
	string download;
	
	string stop{"stop"};
	string fresult{"/tmp/myhtml"};
	int scriptno=1;

	bool init = true;
	
void set(string s){
	if(s=="3"){
		auto& lamb = *lamb_c::self;

//		book_dir="/home/henry/7.6-systemd";
		book_dir="/home/me/7.6-systemd/blfs";
		page = book_dir + "/multimedia/alsa-plugins.html";
		chroot = "no";
		root_dir="/";
		awl_dir=wl_dir;
		sources_dir="";
		sources_dir="/opt/x";
		lamb.get_cwd(pw_dir);
	}
}	

void info(string cmd)
{
	if(cmd =="i" || cmd =="ii")
		 echo << "page:" << page
		<< "\ntitle: " <<  title
		<< "\npackage: " << package
		<< "\nuntared: " << untared_name 
		<< "\ndownload: " << download
		<< "\nchroot: " << chroot 
		<< "\nroot_dir: " << root_dir		
		<< "\npw_dir: " << pw_dir
		<< "\nawl_dir: " << awl_dir
		<< "\nsources_dir: " << sources_dir
		<< "\nstop: " << stop
		<< "\ntesting ----" 
		<< "\ntest: " << is_test		
		<< "\nscript: " << script << '\n';


	if(cmd == "ii" || cmd =="iii"){
		ifstream fi{fresult};
		stringstream ss;
		ss << fi.rdbuf();
		echo << "result -->\n" << ss.str()<< "\n<--\n";	
	}
	 		 
}
string run(string p1, string p2, string p3, string p4="")
{
	auto& lamb = *lamb_c::self;
	string wl_name{wl_dir +"/lfshtml.s.cc"};
	string cmd = "c "+wl_name + " " + p1 + " " + p2 + " " + p3+ " " + p4;
	echo << ".:" << cmd << '\n';
	stringstream ss;
	lamb.system_echo(cmd, ss);
	echo << ss.str() << '\n';	
	ifstream fin(fresult);
	if(!fin){
		echo << "file error..\n";
		return "";
	}
	stringstream sss;
	sss << fin.rdbuf();
	string r;
	r =sss.str();
//	echo << "result ,,,\n" << r << "\n,,,\n";
	return r;
}

void edit(string cmd, string value)
{
	auto& lamb = *lamb_c::self;

	editor_c* pe = nullptr;
	for(auto x : land.llist)
		if(x->get_type()== EDITOR){
			pe=(editor_c*)x;
			break;
		}
	if(pe!=nullptr){
		short c = XK_Return;
		string dum;	
		if(cmd==":clear"){
			pe->command2(c,dum,cmd);
		}
		else if(cmd == ":w"){
			pe->command2(c,dum,cmd);	
		}
		else if(cmd ==":e"){
			cmd += " " + value;
			pe->command2(c,dum,cmd);
		}

		else if (cmd =="paste"){
			lamb.set_clipboard(value);
			pe->editing(PASTE);	
			pe->moving(NEW_LINE_D, 0);
			pe->moving(NEW_LINE_D, 0);
		}	
	}
}

string row{R"(sed -i 's/if \((code.*))\)/if (\1 \&amp;\&amp; \!DEBUG_INSN_P (insn))/' gcc/sched-deps.c)"};

void test(string s)
{
//t
}

void go_to(string command, string page)
{
	auto& lamb = *lamb_c::self;
//	string cmd{"firefox -remote openFile("+ s + ")"};
	string cmd1{"firefox -remote \"openURL(file:///home/me/7.6-systemd/chapter05/sed.html)\""};
	string cmd2{"firefox " + page};
	string cmd3{"firefox -new-window " + page};
	string cmd;
	if(command == "1")
		cmd=cmd1;
	else if(command == "2")
		cmd=cmd2;
	else if(command == "3")
		cmd=cmd3;
	cmd = cmd2;	
	echo << cmd << '\n';
	stringstream sse;
	lamb.system_echo(cmd, sse);
	echo << sse.str();	
	cmd="get_focus";
//	this_thread::sleep_for(milliseconds{100});
	lamb.message_c::notify(cmd);
}

auto myp=make_pair("x","/opt/x");

map<string,vector<pair<string,string>>>pool_dir{
{"/home/me/7.6-systemd/blfs", 
{
{"x", "/opt/x"},
{"*", "/opt/pkgs"}
}
},
{"/home/me/7.6-systemd/errata",
{
{"x", "/opt/x"},
{"*", "/opt/pkgs"}
}
}
};


string adapt_pwd(string pg)
{
	auto& lamb = *lamb_c::self;
		
	pg = pg.substr(book_dir.size()+1);
	string new_wd, all_wd;
	auto pm = pool_dir.find(book_dir);
	echo << "adpt_pwd pg: " << pg << '\n';	
	for(auto x: pm->second){
		echo << "adapt_pwd pd: " << x.first << '\n';
		if(pg.substr(0, x.first.size())==x.first){
			new_wd = x.second;	
			break;
		}
		if(x.first=="*")
			all_wd = x.second;
	}	
	if(!new_wd.empty() || !all_wd.empty()){
		if(new_wd.empty())
			new_wd=all_wd;
		pw_dir=new_wd;
		lamb.command("cd " +new_wd);	
	}	
		
	return "";		
}

void input_keys(string what)

{
	 auto& lamb =*lamb_c::self;
	if(what=="ctrl_s"){
		vector <pair<bool, short>> ks ={
		{1, XK_Control_R},
		{1, XK_s},
		{0, XK_s},
		{0, XK_Control_R},
		};
		for (auto k : ks)
			lamb.key_event(k.first, k.second,0);	
	}
	else if(what=="com_e"){
			vector <pair<bool, short>> ks ={
		{1, XK_e},
		{0, XK_e},
		{1, XK_space},
		{0, XK_space}
		};
		for (auto k : ks)
			lamb.key_event(k.first, k.second,0);	
	}
}

void entry (stringstream& ss)
{
	auto& lamb = *lamb_c::self;
	stringstream sse;
	string s1, s2, s3, s4;
	
	ss >> s1 >> s2 >> s3 >> s4;	
//	echo << ":" << s1 << " " << s2 << " " << s3 << '\n';


	if(init){
		set("3");
		init = false;
	}	
	
	if(s1 =="o"){
		test("");
		return;		
	}
	else if(s1 =="te"){
		string r = run("te","lynx","lamb");			
		echo << r << '\n';
	}	
	else if(s1 =="reset"){
		page = book_dir + "/" +default_page;
		title=="";
		package = "";
		untared_name = "";
		ofstream fo{fresult};
		fo << "";
		return;
	}
	else if(s1=="s"){
		echo << "in s\n";
		if(s2=="chroot"){
			if(s3=="n"){
				chroot = "no";
				root_dir = "/";
				awl_dir = wl_dir;
			}
			else if(s3=="y"){
				root_dir = "/mnt/lfs";
				chroot = "yes";
				awl_dir = "/awl";
				sources_dir="/sources";
			}
			else if(s3=="new"){
				root_dir = "/mnt/lfs";
				chroot = "new";
				awl_dir = "/awl";
				sources_dir="/sources";
			}
			else {
				chroot = "error";
			}
		}
		if(s2=="script")
			script=s3;	
		if(s2=="test")
			is_test=s3;
		if(s2=="stop")
			stop=s3;
		if(s2=="sources_dir")
			sources_dir = s3;
		info("i");
		input_keys("com_e");	
		
	}
	else if(s1.substr(0,1) == "x" ){
		bool ok = is_test=="no"?true:false;
		if (ok)
			echo << "x ok\n";
		else
			echo << "x not ok\n";
		vector<string>en;
		auto envi=environ;
		for(;*envi!=nullptr;++envi){
			en.push_back(string{*envi});
		}		
		vector<string>cmd;
		if(s1=="x" || s1=="xt"){
			string lfsscript;
			if(s1=="x")
				if(s2=="")
					lfsscript = "./lfsscript.sh";
				else
					lfsscript = s2;
			cmd = {
				wl_dir+"/ex.sh",
				lfsscript
			};
		}
		else if(s1=="xm"){
			cmd = {
				wl_dir+"/ex.sh",	
				"m"
			};
			lamb.exec_async(cmd, en);
			return;
		}
		else if(s1=="xp"){
			echo << "x1\n";
			stringstream sse;
			if(ok){
				edit(":w","");
			}
			if(ok){
				ofstream fwriting{root_dir+awl_dir + "/tmp/writing"};
				fwriting.close();
			}
			this_thread::sleep_for(milliseconds{10});
			ifstream frunning{root_dir+ awl_dir + "/tmp/running"};
			if(! frunning){
				echo << "x2\n";
				string sys{"rm -v "+ root_dir + awl_dir+"/tmp/script*.sh"};
				echo << "xp1:" << sys << '\n';
				if(ok)
					lamb.system_echo(sys, sse);
				echo << sse.str();
				scriptno = 1;
				echo << "is not running...\n";
			}
			else
				 echo << "is running...\n";
			stringstream ss;
			ss << root_dir << awl_dir << "/tmp/script" << scriptno << ".sh";
			string des{ss.str()},
			src{wl_dir+"/tmp/lfsscript.sh"};
			string sys{"cp -v " +src + " " +des};
			echo << "xp2:" << sys << '\n';
			sse.str("");
			if(ok)
				lamb.system_echo(sys, sse);
			echo << sse.str();	
			sys="rm -v " + root_dir + awl_dir + "/tmp/writing";
			echo << "xp3:" << sys << '\n';
			sse.str("");
			if(ok)
				lamb.system_echo(sys, sse);
			echo << sse.str();	
			++scriptno;
			if(frunning)
				return;
			cmd = {
				wl_dir+"/ex.sh",	
				chroot,
				awl_dir,
				pw_dir,
				script,
				stop	
			};
		}
		echo << "x cmd:\n";
		for(auto x: cmd)					
			echo << x << '\n';	
		if(ok)
			lamb.exec_async(cmd, en);
		
		input_keys("com_e");	
	
		return;
	}
	else if(s1 =="d"){
		vector<string>en;
		auto envi=environ;
		for(;*envi!=nullptr;++envi){
			en.push_back(string{*envi});
		}		
		vector<string>cmd;
		cmd.push_back("/home/me/desk/cpp/wss/machine/wl/ex.sh");
		cmd.push_back("down");
		cmd.push_back(download);
		lamb.exec_async(cmd,en);
		return;
	}
	else if(s1 =="dl"){
		vector<string>en;
		auto envi=environ;
		for(;*envi!=nullptr;++envi){
			en.push_back(string{*envi});
		}		
		vector<string>cmd;
		cmd.push_back("/home/me/desk/cpp/wss/machine/wl/ex.sh");
		cmd.push_back("downlst");
		cmd.push_back("http://xorg.freedesktop.org/releases/individual/font/");
		cmd.push_back("/opt/x/font-7.7.md5");
		lamb.exec_async(cmd,en);
		
	}
	else if(s1 == "t1"){
		test("");
	}
	else if(s1 =="t2"){
	}
	else if(s1.substr(0,1) == "i"){
		info(s1);
		input_keys("com_e");
		return;
	}
	else if(s1 == "b"){
		string cmd{"ct "+wl_dir+"/04build.s.cc"};
		lamb.system_echo(cmd, sse);
		echo << sse.str() << '\n';
		return;			
	}
	else if(s1 == "ms" || s1 == "mst"){
		string res = run(s1, page, package);	
		lamb.set_clipboard(res);
		return;
	}
	else if(s1 == "m"){
		edit(":e", "lfsscript.sh");	
		edit(":clear","");
		string res = run("ms", page, package);
		edit("paste", res);
		res = run("mc", page, package);
		edit("paste", res);
		return;
	}
	else if(s1 == "mc"){
		string res = run(s1, page, "");
		lamb.set_clipboard(res);
	}
	else if(s1 =="p" || s1 == "n" || s1=="cp"){
		string result;
		if(s1=="p" || s1 =="n")
			page= run(s1, page, s3);	
		else if(s2 !="")			
			page = book_dir + "/" + s2;
		adapt_pwd(page);
		
		result = run("t", page, "mode3");
		echo << "here..\n";
		stringstream ss{result};
		getline(ss, title);
		getline(ss, package);
		getline(ss,untared_name);
		getline(ss,download);
		adapt_pwd(page);	
				
		
		edit(":e", wl_dir+"/tmp/lfsscript.sh");	
		edit(":clear","");
		string res = run("mall",page,package,untared_name);
		edit("paste", res);
		
		info("i");
		go_to("", page);		
		input_keys("ctrl_s");
		input_keys("com_e");
		return;
	}
	else if(s1 =="t"){
		string result = run("t",page,"");
		stringstream ss{result};
		ss >> title;
		ss >> package;
		ss >> untared_name;
		echo << result << '\n';
		return;
	}
	else if(s1 =="g"){
		go_to(s2, page);
	}
}	

}	//end namespace ex_lamb

void ex_entry_lamb ( stringstream& ss )
{
	echo << ":..:" << ss.str() << '\n';
	ex_lamb::entry(ss);
}
