#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_set>
#include <functional>

#include <regex>
#include <iterator>

using namespace std;


#include "debug.h"

#include "standard.h"

#include "symbol/keysym.h"
#include "symbol.h"

#include "library/wdslib.h"

#include "gate.h"
#include "matrix.h"
#include "position.h"
#include "mstream.h"
#include "fox/cash.h"

#include "sens/retina.h"
#include "sens/spirit.h"
#include "message.h"

#include "regexp.h"

#include "global.h"
#include "object.h"
#include "line.h"
#include "spline.h"
#include "keyb.h"
#include "wr/font.h"
#include "wr/texel.h"
#include "wr/shared.h"
#include "wr/layen.h"

#include "edit.h"

#include "land.h"
#include "mouse.h"
#include "eyes.h"
#include "home.h"
#include "bookmarks.h"
#include "hand.h"

#include "lamb.h"
#include "comtab4.h"
#include "edit_ct.h"
#include "bookmarks.h"
#include "image.h"
#include "make.h"
#include "shell.h"

extern land_c land;
extern image_c image;
extern int figure;

extern int parrot;

unsigned long editor_c::type = EDITOR;

comtab_editor_c editor_c::comtab_editor;

using namespace std;

long get_utf32 ( string& s, int& c );
int byte_index ( string& str, int index );

string clip_string;
bool system_clipboard=false;
clipboard_c clipboard;


string& clipboard_c::str(){
	if(system_clipboard){
		lamb_c& lamb = *lamb_c::self;
		lamb.get_clipboard(clip_string);
	}
	return clip_string;
}

void clipboard_c::str(string s)
{
	if(system_clipboard){
		lamb_c& lamb=*lamb_c::self;
		lamb.set_clipboard(s);
	}
	clip_string = s;
}

unsigned long editor_c::get_type ()
{
	return type;
}

bool editor_c::find_word ( string& s )
{
	if ( s == "" ){
		 return false;
	}
	static bool initialize = true;
	static unordered_set<string> my_set;
	string lambs_path = get_lambs_path ();
	string fn;
	string str;
	if ( initialize ){
		fn = lambs_path+"/../ogerman";
//		fn = lambs_path+"/../dictio2";
		ifstream file ( fn );
		string str;
		while ( file.good ()){
			file >> str;
			my_set.insert ( str );
			initialize = false;
		}
	}
	if ( my_set.find ( s ) == my_set.end ()){
		if ( islower ( s.at ( 0 ))){
			return false;
		}
		s [0] = tolower ( s[0] );
		if ( my_set.find ( s ) == my_set.end ()){
			return false;
		}
	}
	return true;
}

int editor_c::check_spell ( int flag )
{
	if ( flag == 0 ){
		echo << "spell check" << endl;
		string s;
		auto 
			ite = layen.texels.begin (),
			itb = ite;
		for (;ite!= layen.texels.end ();){
			s = get_next_dictionary_word ( itb, ite );
			if ( s != "" && !find_word ( s )){
				itb = layen.texels.insert (itb, new texel_selector_c);
				advance ( itb, s.size() + 1 );
				if ( itb == layen.texels.end ()){
					layen.texels.push_back ( new texel_selector_c );
					break;
				}
				else {
					itb = layen.texels.insert ( itb, new texel_selector_c );
				}
				itb ++;
				ite = itb;
				continue;
			}
		}
	}
	else if ( flag == 2 ){
		for ( auto it = layen.texels.begin() ;it!= layen.texels.end ();it++){
			if ( (*it)->is_selector ()  ) {
				if ( ! (*it)->is_caret() ) {
					it = layen.texels.erase ( it );
				}
			}			
		}			
	}
}

string editor_c::get_next_dictionary_word ( vector<texel_c*>::iterator& itb, vector<texel_c*>::iterator& ite )
{
	string s;
	itb = ite;
	uint32_t ch = 0;
	for (;ite!= layen.texels.end ();ite++){
		ch=(*ite )->get_char ();
		if ( isalpha ( ch )){
			itb = ite;
			break;
		}
	}
	for (;ite!= layen.texels.end ();ite++){
		ch=(*ite )->get_char ();
		if  ( !isalpha ( ch ) && ch!='-'){
			break;
		}
		s+= ch;
	}
	return s;
}

void editor_c::dump ( std::stringstream& ss )
{
	ss << "frame width: " << frame_width << endl;
	ss << "frame height: " << frame_height << endl;
	ss << "simple text: " << simple_text << endl;
	ss << "file path: " << file_path << endl;
}

int position_index ( string& str, int index )
{
	string::iterator it = str.begin ();
	int count = index;
	
	for (;it!= str.end ();it++){
		char c=*it;
		if ( c&0x80 ){
			if ((c&0xe0 ) == 0xc0 ){
				it++;
				count--;
				index--;
			}
			else if ((c&0xf0 ) == 0xe0 ){
				cout << "xxxx" << endl;
				it+= 2;
				count-= 2;
				index-= 2;
			}
			
			else if ((c&0xf8 ) == 0xf0 ){
				it+= 3;
				count-= 3;
				index-= 3;
			}
		}
		count--;
		if ( count == 0 ){
			break;
		}
	}
	return index;
}

int byte_index ( string& str, int index )
{
	string::iterator it = str.begin ();
	int count = index;
	for (; it!= str.end (); it++){
		count--;
		if ( count == 0 ){
			break;
		}
		uint8_t c=*it;
		if ( (c & 0x80 ) == 0 ){
			continue;
		}
		while ( c & 0x40 ){
			it++;
			index++;
			c = c << 1;
		}
	}
	return index;
}

editor_c* editor_c::create ()
{
    return new editor_c ();
}

void editor_c::serialize ( fstream &f, bool save ){
	if  ( !simple_text ){
		return;
	}	
	if ( save ){
		f << "EDITOR" << endl;
	}
	motion.serialize ( f, save );
	if ( save == false ){
		f >> simple_text;
		f >> frame_width;
		f >> frame_height;
		f >> gravity;
		string s;
		getline ( f, s );
		getline ( f, s );
		if ( s == "" ){
			getline ( f, s );
			layen.set_text ( s );
			layen.set_caret();
		}
		else {
			file_path = s;
			import_text ( s, true );
		}
	}
	else{
		f << simple_text << endl;
		f << frame_width << endl;
		f << frame_height << endl;
		f << gravity << endl;
		string s = file_path;
		f << s << endl;
		if ( s== "" ){
			f<< layen.text( 1 , -1) << endl;
		}
		else	{
			export_text ( s );
		}
	}
}
 
void editor_c::invalide_visual ( int level )
{
	valide = level;
}

editor_c::editor_c ():
	frame_width (0),
	frame_height (0),
	is_collapsed (false),
	overwrite (0),
	simple_text (false),
	valide (0),
	gravity (STATIC),
	is_menu (false)
{
	search_mode = 0;
	repeater_count = 0;
	repeater_info = 0;			
	layen.set_caret();
	layen.font.layen = &layen;
	layen.font.cell = 20;
	layen.font.tab_width = 40;
	
	mode = COMMAND;
	type = EDITOR;
	load_file_list ( "files_test" );
//	layen.resize ( 0, 0 );
	ui = die ();
	mx = { { 0, 1, 0 }, { 0, 0, -1 }, { 1, 0 ,0} };	
}

editor_c::~editor_c ()
{
	if ( impression_clients.retina != 0 ) {
		impression_clients.retina->unregister ( ui );				
	}		
	cout << "editor destruction\n";
}

void editor_c::load_file_list ( string path )
{
	if ( path == "" ){
		file_list.clear ();
		return;
	}
	string fn = get_lambs_path () + "/config/" + path;
	ifstream fin ( fn );
	if ( fin.fail ()){
//		echo << "no such a file_list:" << fn << endl;
		return;
	}
	file_list.clear ();
	stringstream ss;
	ss << fin.rdbuf ();
	while ( ss.good ()){
		string nn;
		ss >> nn;
		file_list.push_back ( nn );
	}
}

void editor_c::change_mode ( int para1, int para2 )
{
	lamb_c& lamb = *lamb_c::self;
	keyboard.stroke_list.clear ();
	if ( para1 == 0 ){
		if ( para2 == VISUAL ) {
			echo << "visual" << endl;
			layen.get_caret()->unbind_selector();
			layen.invalide_caret();
		}
		if ( para2 == COMMAND ) {
			layen.get_caret()->bind_selector();
			echo << "command" << endl;
			layen.invalide_caret();
		}
		mode = para2;
		overwrite = 0;
	}
	else if ( para1 == 2 ){
		lamb.mouth.mode = INSERT;
	}
	else if ( para1 == 3 ){
		if ( lamb.sensor->get_type () == EDITOR ){
			editor_c* ed=(editor_c*)lamb.sensor;
			ed->export_text ( ed->file_path );
		}
		if ( lamb.sensor->get_type () == EDITOR ){
			editor_c* ed=(editor_c*)lamb.sensor;			
			ed->import_text ( ed->file_path, true );
		}
	}
	else if ( para1 == 4 ){
		state = state == 0 ? 1 : 0;
	}
}

void editor_c::collapsing ( int para1, int para2 )
{
	layen.collapse();
}

bool flag_ok;
void editor_c::runing2 (string str)
{
	lamb_c& lamb = *lamb_c::self;

	if (str == ""){
		str = lamb.project;
	}
	stringstream ss{str};
	export_text ( file_path );
	write_file_cash_to_disk();
	static shell_c shell;
	shell.run(ss);
}

void editor_c::runing (int para1, int para2 )
{
	runing2("");
	return;
}

void editor_c::searching (size_t* position)
{
	size_t pos = *position;
	string s = layen.text ();
	size_t inz= byte_index ( s, pos );
	string found;
	if  (regexp.find_section (s, inz, found)){
		texel_caret_c& caret = *layen.get_caret ();
		pos = position_index ( s, inz ) + 1;
		caret.move_to (pos + regexp.look_before);
		caret.move_selector_to (pos + found.size () + regexp.look_after);
//	layen.font.set_rows(layen);
		caret.info.scroll_up = 10;
		*position = pos+1;
		caret.info.set(true);
		return;
	}
	*position = string::npos;
	return;
} 

int flags = 0;
int flag = 0;

void editor_c::run_search_replace ( int& fflag, uint16_t v, string& c )
{
	lamb_c& lamb = *lamb_c::self;
	static size_t pos = 0;
	static string stro;
	static size_t pos_o;
	static size_t scroll_o;
	static string mem_file;
	bool automatic = false;
	if ( flag == -1 ) {
		automatic = true;
		flag = 0;
	}
	if ( flag == 0 ){
		export_text ( file_path );
		echo << "run search replace" << endl;
		clip_string = regexp.replace;
		itfl = file_list.begin ();
		echo << "file_list size " << file_list.size () << endl;
		flag = 3;
		c = "n";
		pos = 0;
		stro = file_path;
		auto& caret = *layen.get_caret();
		caret.info.valid = false;
		caret.info.set();
		pos = pos_o = caret.get_position ();
		scroll_o = caret.info.scroll_up;
		mode = 5;
	}
	do {
		if ( v == XK_Escape ){
			mout << "escape\n";
			export_text(file_path);
			file_path = stro;
			import_text(stro);
			layen.font.set_rows(layen, &layen.texels);
			auto& caret = *layen.get_caret();
			cout << "scroll_up:" << scroll_o << endl;
			cout << "position_:" << pos_o << endl;
			caret.info.valid = false;
			caret.info.set();
			caret.move_to(pos_o);
			caret.info.scroll_up = scroll_o;
			mode = COMMAND;
			flags = 0;
			flag = 0;		
		}
		else if ( c == "s" ) {
			layen.get_caret()->info.set(true);
			mode = INSERT;
			flags = flag;
			flag = 0;
		}
		else if ( c == "e" ) {
			layen.get_caret()->info.set(true);
			mode = INSERT;
			flags = 0;
			flag = 0;
		}
		else if ( c == "f" ) {
			layen.get_caret()->info.set(true);
			editing ( XCHANGE );
		}
		else if (c == "n" || c == "y" || c == "a"){
			static bool is_changed = false;
			if (pos != string::npos && pos != 0){
				if (c == "y" || c == "a"){
					texel_caret_c& caret = *layen.get_caret();
					caret.info.set(true);
					editing (CHANGE);
					is_changed = true;
					if ( c == "a" ) {
						automatic = true;
					}
				}
			}
			while (true){
				if (pos == string::npos || pos == 0){
					if (is_changed){
						echo << "exporting" << endl;
						echo << file_path << endl;
						export_text ( file_path );
						is_changed = false;
					}
					if (itfl == file_list.end ()){
						file_path = stro;
						import_text ( stro );
						layen.scroll_down = scroll_o;
						layen.get_caret ()->move_to ( pos_o );
						layen.get_caret()->bind_selector();
						mode = COMMAND;
						flag = 0;		
						automatic = false;
						break;
					}
					string fn = *itfl;
					if (fn == stro){
						itfl++;
						continue;
					}
					file_path = fn;
					echo << fn << endl;
					import_text ( fn );
					texel_caret_c& caret = *layen.get_caret();
					caret.info.valid = false;
					caret.info.set();
					itfl++;
					pos = 1;
					if (! automatic){
//						land.impress (lamb);
					}
				}
				searching ( &pos );
				if ( flag == 2 && pos >= pos_o ){
					flag = 1;
					pos = string::npos;
					continue;
				}
				if ( flag == 3 && pos == string::npos ){
					flag = 2;
					pos = 1;
					continue;
				}
				if ( pos != string::npos ) {
					if ( ! automatic ) {
//						land.impress ( lamb );
					}
					break;
				}
			}
		}
	} while ( automatic );
	
	draw();
}

int editor_c::command2 ( unsigned short v, string& c, string& st )
{
	lamb_c& lamb = *lamb_c::self;
	static int repeat = 0;
	if ( v == XK_Escape ) {
		echo << "command2 escape" << endl;
	}
//	echo << "repeat is_menu flag v::" << repeat  << ":" << is_menu << ":" << flag << ":" << v << endl;
	if ( ! is_menu ) {
		repeat = 0;
	}
	string s, s1, s2, s3, s4 ,s5;
	stringstream ss ( st );
	ss >> s;
	echo << "s..:" << s << '\n';	
	if ( s == ":s" or s == ":s1" || flag > 0 ){
		if ( flag == 0 ) {
			lamb.motors.remove (&lamb );
			lamb.motors.push_front (&lamb );
			lamb.motor = &lamb;
			ss >> s1;
			if ( s1 != "" ){
				regexp.search = s1;
				if ( s == ":s" ) {
					char dum;
					ss >> noskipws >> dum;
					getline ( ss, regexp.replace );
				}
				else {
					ss >> s2;
					ifstream ifs ( s2 );
					stringstream sss;
					sss << ifs.rdbuf();
					regexp.replace = sss.str();
				}
			}
		}
		run_search_replace ( flag, v, c );
		return 1;
	}
	if ( flag == 0 ){
		if ( v == XK_Return ){
			if ( st.substr ( 0, 1 ) =="." ){
				string str = st.substr ( 1, st.size()-2 );
				echo << st << endl;
				stringstream sss;
				lamb.system_echo ( str, sss );
				if ( str.substr(0,2) == "ls" ) {
					matrix_c <FT> mx;
					mx = layen.font.get_text_size ( "\t" );
					int z, y, x = mx [1] ;
					echo << "ss: " << x << endl;
					stringstream ssn;
					string sb;
					assert ( x != 0 );
					for ( ; sss.good (); ) {
						if (  !getline ( sss, sb ) ) {
							break;
						}
						ssn << sb;
						mx = layen.font.get_text_size ( sb ) ;
						y = mx [ 1 ];
						z = 4 - y/x;
						for (; z >=0; --z) {
							ssn << "\t";
						}

						if (  !getline ( sss, sb ) ) {
							break;
						}

						ssn << sb;
						mx = layen.font.get_text_size ( sb ) ;
						y = mx [ 1 ];
						z = 4 - y/x;
						for (; z >=0; --z){
							ssn << "\t";
						}
						getline(sss, sb);
						ssn << sb;
						ssn << endl;
					}
					echo << ssn.str() << endl;
				}
				else {
					echo << sss.str() << endl;
				}
			}
			else if (st.substr( 0, 1 )!=":"){
					lamb.command(st);
			}
			if (s == "trace"){
				layen.trace = ! layen.trace;
			}
			if ( s == "font" || s =="f1" || s =="f2" || s == "f3"){
				string s1;
				if ( s == "f1" ) {
					s1 = "Vera-code.ttf";
				}
				else if ( s == "f2" ) {
					s1 = "DejaVuSansMono.ttf";
				}
				else if ( s == "f3") {
					s1 = "DejaVuSans.ttf";
				}
				else {
					ss >> s1;
				}
				echo << "font: " << s1 << '\n';
				auto f = layen.font.set_face (s1,13,1);
				if ( f !=nullptr ) {
					layen.face = f;
				}
				// ----
				layen.clear_texel_map();
				for ( auto x : layen.texels){
					x->graphies->face =f;
				}
			}
			if ( s == "reset_repeat" ) {
				echo << "reset_repeat" << endl;
				repeat = 0;
				return 0;
			}
			if ( s== "ss" ) {
				ss >> search_mode;
				echo << "search mode is: " << search_mode << endl;
			}
			if( s== "testutf8"){
				ss >> s1;
				ifstream ifs(s1);
				
				stringstream ss2;
				ss2 << ifs.rdbuf();	
				string src{ss2.str()},
				des;
				to_utf8(src, des, 0);
				ss >> s2;
				ofstream ofs(s2);
				ofs << des;
			}
			if ( s == "spa" ) {
				ss >> s1;
				parrot = stoi ( s1 );  
			}
			if ( s == "ctab" ) {
				auto pos = ss.tellg ();
				ss >> s1;
				ss.clear ();
				if ( s1 != "0" ) {
					ss.seekg ( pos );
				}
				if ( s1 != "1" && s1 != "2" ) {
					ctab ( ss ) ;
				}
				else {
					lamb.ctab (ss );
				}
			}
			if ( s == ":rx" ) {
				regexp.command ( ss );
			}
			else if ( s == ":q" ){
				echo  << "file path:" << file_path << endl;
				
				auto it = lamb.automarks.find_name ( file_path );
				if ( it != lamb.automarks.l.end () ){
					lamb.automarks.l.erase ( it );
				}			
				lamb.automarks.push_mark ( *this, mark_c () ); 											lamb.exit ( wcp::quit );
			}
			else if (s == ":wq"){
				export_text ( file_path );
				write_file_cash_to_disk();
				string s{":q"};
				command2( v, c, s);
			}
			else if ( s == ":new" ) {
				clear ();				
				file_path = "";
			}
			else if ( s == ":w" ){
				export_text ( file_path );
				write_file_cash_to_disk();
			}
			else if ( s == ":wtd" ){
				write_file_cash_to_disk ( );
			}
			else if ( s == ":r" ){
				import_text ( file_path );
			}
			else if(s==":clear"){
				editing(DELETE_FILE);
			}
			else if ( s == ":run" ) {
				shell_c shell;
				ss >> s1 >> s2;
				s = s1 + " " + s2;
				ss.clear ();
				ss.str ( s );
				shell.run ( ss );
				echo << s << endl;
			}
			else if ( s ==":sh"){
				export_text(file_path);
				write_file_cash_to_disk();				
				shell_c shell;
				int pa = 0;			
				ss >> s1 >> s2 >> s3;
				s = s1 + " " + s2 + " " + s3;
				ss.clear ();
				ss.str ( s );
				shell.sh ( ss );
				echo << s << endl;
				
			}
			else if ( s == ":es" ){
				ss >> s1;
				string  s0 = ":e " + get_lambs_path () + "/" + s1;
				command2 ( v, c, s0 );
			}
			else if ( s == ":we" ) {
				ss >> s1;
				string s0 = ":w " + s1;
				command2 ( v, c, s0 );				
				ss >> s2;
				s0 = ":e " + s1 + " " + s2;
				command2( v, c, s0 ); 
			}
			else if (s == ":wfc"){
				write_file_cash();
			}
			else if (s == ":efc"){
				mark_c mark;			
				if (file_path != "") {
					auto it = lamb.automarks.find_name(file_path);
					echo  << "file path:" << file_path << endl;
					cout << "+e 0 file path:" << file_path << endl;

					if ( it != lamb.automarks.l.end () ){
						lamb.automarks.l.erase ( it );
						cout << "+e 1" << endl;
					}			
					lamb.automarks.push_mark ( *this, mark ); 												}
				file_path ="";
				edit_file_cash();
			}
			else if (s == ":e"){
				echo << "in edit command2 :e\n";
				mark_c mark;			
				if (file_path != "") {
					auto it = lamb.automarks.find_name(file_path);
					echo  << "file path:" << file_path << endl;
					cout << "+e 0 file path:" << file_path << endl;

					if ( it != lamb.automarks.l.end () ){
						lamb.automarks.l.erase ( it );
						cout << "+e 1" << endl;
					}			
					lamb.automarks.push_mark ( *this, mark ); 												}
				cout << "+e 2" << endl;
				ss >> s1;
				file_path = s1;
				import_text ( s1 );
				ss >> s2 ;
				if ( s2 != "" ) {
					layen.font.set_rows(layen, &layen.texels);
					int lineNo = stoi ( s2 );
					echo << "ee: " << lineNo << '\n';	
						cout << "ee: " << lineNo << '\n';	
						auto &caret = *layen.get_caret();
						caret.info.valid = false;
						caret.info.set();
						int d = lineNo - caret.info.row;
						if ( d > 0){
							caret.to_row_down(d);
						}
						else{
							caret.to_row_up(-d);
						}
					layen.scroll_down = lineNo - 5;
					if ( layen.scroll_down < 0 ){
						layen.scroll_down = 0;
					}
				} 
				else { 
					auto it = lamb.automarks.find_name ( file_path );
					if ( it != lamb.automarks.l.end () ) {
						it -> go_to_mark ( *this );
					}
				}
				valide = 1;
				lamb.motors.remove (&lamb );
				lamb.motors.push_front (&lamb );
				lamb.motor = &lamb;
				mode = COMMAND;
				draw();
			}
			else if ( s == ":u" ){
				ss >> s1;
				import_webpage ( s1 );
				mode = COMMAND;
			}
			else if ( s == ":h0" ){
				lamb.mx = {{0, 1, 0},{0, 0, 1},{-1, 0, 0}};
				lamb.hand.pointer = {0, 0, 0};
				lamb.local_position = {0, 0, 0};
			}
			else if ( s == ":h1" ){
				lamb.mx = {{0, 1, 0},{0, 0, 1},{-1, 0, 0}};
				lamb.hand.pointer = {0, 0, 0};
				lamb.local_position = {-310, 190, 0};
			}
			else if ( s == ":sc0" ){
				check_spell ( 0 );
			}
			else if ( s == ":sc1" ){
				check_spell ( 1 );
			}
			else if ( s == ":sc2" ){
				check_spell ( 2 );
			}
			else if ( s == ":proper" ) { //:proper <path> to have clean source
				ss >> s1 >> s2 >> s3;
				s1 = s1 + " " + s2 + " " + s3;
				static make_c make;
				make.proper2 ( s1 );
			}
			else if ( s == ":m2" ){
				ss >> s1 >> s2 >> s3 >> s4;
				make_c make;
				make.make ( s1, s2, s3, s4 );
			}
			else if ( s == ":scan" ) {
				ss >> s1 >> s2;
				make_c make;
				make.scan ( s1, s2 ); 	
			}
			else if ( s == ":cbs" ){
				lamb.set_clipboard ( clip_string );
			}
			else if ( s == ":cbg" ){
				lamb.get_clipboard ( clip_string );
			}
			else if ( s == ":lp" ){
				system ("lp -d HP-LaserJet-1100 -P 8 /home/henry/shelf/newvinland/mathoflife/mathoflife.pdf" );
			}
			else if ( s == ":wm" ){
				ss >> s1;
				lamb.window_management ( s1 );
			}
			else if ( s == ":ss" ){
				unsigned char ch;
				string s = layen.text( 1 , -1);
				sheck_string ( s, 0,&ch );
			}
			else if ( s == ":fn" ){
				ss >> s1;
				file_path = s1;
			}
			else if ( s == ":dump" ){
				stringstream sss;			
				dump ( sss );
				echo << ss.str () << endl;
			}
			else if ( s == ":res" ) {
				make_c make;
				make.remove_experimental_section ("../td" );
			}
			else if ( s == ":lfl" ) {
				ss >> s1;
				load_file_list ( s1 );				
				echo << "load file list " << endl;
			}
			else if ( s == ":kbl" ) {
				lamb_c& lamb = *lamb_c::self;
				ss >> s1;
				lamb.keyboard.set_layout ( s1 ); 
			}
			else if ( s == ":scan" ) {
				ss >> s1 >> s2;
				make_c make;
				make.scan ( s1, s2 ); 	
			}
			else if ( s == ":t" ) {
				echo << "hi: " << echo_c::hex << 15 << '\n' << "hello";
			}
			else if ( s == ":mark" ) {
//				echo << "mark" << endl;
				ss >> s1;
				list < string >* marks;
				if ( s1 == "show" ) {
					ss >> s2;
					if ( s2 == "rm" ) {
						echo << "show ring m" << endl;
						lamb.ringmarks.show ( *this );
					}
					if ( s2 == "am" ){
						echo << "show auto m" << endl;
						lamb.automarks.show (* this );
					}
					else if ( s2 == "m" ) {
						echo << "show bookmarks" << endl;
						lamb.bookmarks.show ( *this );
					}
				}
				else if ( s1 == "mark" ) {
					ss >> s2;
					int n;
					ss >> n;
					if ( ss.fail () ) {
						n = 1;
					} 
					lamb.bookmarks.mark ( *this, mark_c (), n, s2 ); 
					valide = 1;
					lamb.motors.remove (&lamb );
					lamb.motors.push_front (&lamb );
					lamb.motor = &lamb;
					mode = COMMAND;
				}
				else if ( s1 == "to" ) {
					ss >> s2;
					auto it = lamb.bookmarks.find_tag ( s2  );
					if ( it != lamb.bookmarks.l.end () ) {
						it -> go_to_mark ( *this );
//						lamb.bookmarks.lgo_to_mark ( *this , *it );
					}
					valide = 1;
					lamb.motors.remove (&lamb );
					lamb.motors.push_front (&lamb );
					lamb.motor = &lamb;
					mode = COMMAND;
				}
				else if ( s1 == "toline" ) {
//					echo << "toline" << endl;
					marks_c* marks;
					ss >> s2;
					int n;
					ss >> n;
					if ( s2 == "m" ) {
						marks = &lamb.bookmarks;
					}
					else if ( s2 == "am" ) {
//						echo << "am.." << endl;
						marks = &lamb.automarks;
					}
					if ( n < 0 || n > marks -> l.size () ) {
						return 0;
					}
					mark_c mark;
					if ( n == 0 ) {
						mark = marks -> l.back ();
					}
					else {
						auto it = marks -> l.begin ();
						advance ( it, n -1 );
						mark = *it;
					}
					mark.go_to_mark ( *this );
//					marks -> go_to_mark (*this, mark );
					valide = 1;
					lamb.motors.remove (&lamb );
					lamb.motors.push_front (&lamb );
					lamb.motor = &lamb;
					mode = COMMAND;
				}
				else if ( s1 == "clear" ) {
					ss >> s2;
					if ( s2 == "m" ) {
						lamb.bookmarks.l.clear ();						
					}
					else if ( s2 == "am" ) {
						lamb.automarks.l.clear ();	
					}
					else if ( s2 == "rm" ) {
						lamb.ringmarks.l.clear ();	
					}
				}
				else if ( s1 == "erase" ) {
					echo << "erase" << endl;				
					marks_c* marks;
					ss >> s2;
					if (  s2 == "m" ) {
						marks = &lamb.bookmarks;
					}
					else if ( s2 == "am" ) {
						marks = &lamb.automarks;
					}
					else if ( s2 == "rm" ) {
						marks = &lamb.ringmarks;
					}
					else { 
						return 0;
					}
					int n;
					ss >> n;
					if ( ! ss.fail () ) {
						if ( n <= marks -> l.size () ) {
							auto it = marks -> l.begin ();
							advance ( it , --n );
							marks  -> l.erase ( it );
						}
					}
				}
				
				else if ( s1 == "move" ) {
					marks_c* marks;
					ss >> s2;

					if (  s2 == "m" ) {
						marks = &lamb.bookmarks;
					}
					else if ( s2 == "am" ) {
						marks = &lamb.automarks;
					}
					else {
						return 0;
					}
					int n1, n2;
					ss >> n1 >> n2;
					echo << "move:" << s2 << ":" << n1 <<":" << n2 << endl;

					if ( ! ss.fail () ) {
						if ( n1 != n2 && max ( n1, n2 ) <= marks -> l.size () ) {
							echo << "move ok "<< n1 << n2 << endl;
							auto its = marks -> l.begin (),
							ite = marks -> l.begin ();
							advance ( its, n1 - 1 );
							if ( n2 == marks->l.size () ) {
								marks->l.push_back ( *its);
							}
							else {
								if ( n1  >  n2 ) {
									n2--;
								}
								advance ( ite,  n2 );
								marks -> l.insert ( ite, *its );
							}
							marks -> l.erase ( its );
						}
					}
					else {
						echo << " move fail " << endl;
					}
				}
				else if ( s1 == "set" ) {
					if  ( repeat == 0 ) {
						ss >> repeat; 
						is_menu = true;

						lamb_c& lamb = *lamb_c::self;
						lamb.mouth.mode = COMMAND;
						lamb.motors.remove (&lamb );
						lamb.motors.push_front (&lamb );
						lamb.motor = &lamb;
													
//						echo << "set:" << repeat << endl;
					}	
					else {
						int n, n2;
						int v = XK_Return;
						ss >> n >> n2;
//						echo << "::" << n << "::" << n2;
						if ( n2 == 1 ) {
							stringstream sss;
							sss << ":mark move m " << repeat << " " <<  repeat +1; 
							string ds1;
							string ds2 = sss.str ();
							command2 ( v, ds1, ds2 );
							is_menu = true;
							repeat++;
						}
						if ( n2 == -1 ) {
							stringstream sss;
							sss << ":mark move m " << repeat << " " <<  repeat -1; 
							string ds1;
							string ds2 = sss.str ();
							command2 ( v, ds1, ds2 );
							is_menu = true;
							repeat --;
						}
						if ( n2 == 0 ) {
							stringstream sss;
							sss << ":mark erase m "  << repeat;
							string ds1;
							string ds2 = sss.str ();
							command2 ( v, ds1, ds2 );
							is_menu = true;
							repeat;
						}
					}	
				}
			}
		}
	}
	return 0;
}

void editor_c::inserting_overwriting ( int para1, int para2 )
{
	echo << "overwriting, para1: " << para1 << endl;
	if ( para1  == 1 ) {
		overwrite = 1;
		mode = INSERT;
	}
	else if ( para1 == 2 ) {
		overwrite = 2;
		mode = INSERT;
	}
	else if ( para1 == 3 ) {
		moving (INC_COLUMN, 0);
		mode = INSERT;
	}
	else if ( para1 == 4 ) {
		moving ( LINE_END, 0 );
		mode = INSERT;
		
	}
}

void editor_c::deleting ( int para1, int para2 )
{
	if ( para1 == CHARACTER){
		texel_caret_c& caret = *layen.get_caret();
		caret.unbind_selector();
		caret.to_right(1);
		layen.erase_text();
		caret.bind_selector();
		caret.set_column_memory();
		fit_scrolls();
	}
	else if ( para1 == TAB ) {
		layen.remove_rows();
		layen.font.set_rows(layen);
		int selector_trigger = 0,
		row_trigger = 0;
		for (auto it = layen.texels.begin(); it != layen.texels.end();) {
			if((*it)->is_selector()){
				selector_trigger++;
				if(selector_trigger==1){
					for (; it != layen.texels.begin() && (!(*it)->is_row()); it--); 
					continue;
				}
			}
			else if ( selector_trigger ) { 
				if (  (*it)->is_row() ) {
					assert ( selector_trigger <=3);
					if ( selector_trigger == 3 ) 
						break;
					row_trigger = 1;
				}
				else if ( row_trigger &&  (*it)->is_text() ) {
					it = layen.texels.insert( it, texel_char_c::create( '\t', layen.face,0, layen.shared_map));
					row_trigger = 0;
				}
			}
			it++;
		}
	}
	else if ( para1 == SPACE ) {
	
		layen.remove_rows();
		layen.font.set_rows(layen);
		int	selector_trigger = 0,
			row_trigger = 0;
		for ( auto it = layen.texels.begin(); it != layen.texels.end(); ) {
			if ( (*it)->is_selector() ) {
				selector_trigger++;
				if ( selector_trigger == 1 ) {
				for (; it != layen.texels.begin() && ( ! (*it)->is_row()) ; it-- ); 
					continue;
				}
			}
			else if ( selector_trigger ) { 
				if (  (*it)->is_row() ) {
					assert ( selector_trigger <=3);
					if ( selector_trigger == 3 )
						break;
					row_trigger = 1;
				}
				else if ( row_trigger &&  (*it)->is_text() ) {
					row_trigger = 0;
					if ( (*it)->is_tab() ) {
						it = layen.texels.erase( it);
						continue;
					}
				}
			}
			it++;
		}
	}
}

void editor_c::editing ( int para )
{
	lamb_c& lamb = *lamb_c::self;
	
	if ( para == CUT){
		clipboard.str(layen.cut_text());
	}
	else if ( para == DELETE )
		layen.erase_text();
	else if ( para == COPY )
		clipboard.str(layen.copy_text());
	else if(para == PASTE){
			layen.replace_text(clipboard.str());
			texel_caret_c& caret = *layen.get_caret();
			caret.bind_selector();
	}
	else if ( para == CHANGE ) 
		layen.replace_text(clipboard.str());
	else if ( para == XCHANGE ) {
		string s = clipboard.str();
		clipboard.str(layen.copy_text());
		layen.replace_text(s);
	}
	else if (para == CUT_LINE){
		auto& caret = *layen.get_caret();		
		caret.unbind_selector();
		caret.to_line_end();
		caret.to_right(1);
		clipboard.str(layen.cut_text());
		caret.bind_selector();
	}
	else if ( para == COPY_LINE ) {
		auto& caret = *layen.get_caret();		
		caret.unbind_selector();
		caret.to_line_end();
		caret.to_right(1);
		clipboard.str(layen.copy_text());
		caret.swap_selector();
		caret.bind_selector();
	}
	else if ( para == CUT_WORD ) {
		texel_caret_c& caret = *layen.get_caret();		
		caret.unbind_selector();
		caret.next_word();
		clipboard.str(layen.cut_text());
		caret.bind_selector();
	}
	
	else if(para ==DELETE_FILE){
		texel_caret_c& caret = *layen.get_caret();		
		caret.to_file_begin();		
		caret.unbind_selector();
		caret.to_file_end();
		layen.erase_text();
//		clipboard.str(layen.cut_text());
		caret.bind_selector();
	}	
	
}

void editor_c::scrolling ( int para1, int para2 )
{
	auto& caret = *layen.get_caret ();
	if ( para1 == PAGE_U ){
		cout << "page up\n";
		caret.scroll_up(caret.info.scroll_up + frame_height - 2);	
		fit_caret();	
	}
	else if ( para1 == PAGE_D ){
		cout << "page down\n";
		caret.scroll_up(caret.info.scroll_up - (frame_height-2));
	}
	else if ( para1 == HOME ){
		echo << "HOME" << endl;
		size_t rows = 0,
		row_pos = 1,
		pos = 1;
		auto& caret = *layen.get_caret();
		auto ic = caret.iterator();
		auto it = layen.texels.begin();
		for (; it != layen.texels.end(); ++it){
			if (it == ic)
				return;
			if ((*it)->is_row()){
				++rows;
				if (rows == layen.frame_height){
					break;
				}
				row_pos = pos;
			}	
			else if ((*it)->is_text())
				++pos;	
		}
		auto p = min(row_pos + caret.column_memory , pos);
		caret.move_to2(p);
		caret.info.scroll_up = rows;
		return;
	}
	else if ( para1 == END ){
		auto& caret = *layen.get_caret();
		size_t lpos = caret.get_last_position(),
			row_position = lpos;
		auto it = --layen.texels.end(),
			ic = caret.iterator();
		size_t rows = 1;	
		for (;; --it){

			if (it == ic)
				return;
			if ((*it)->is_row()){
				++rows;	
				if (rows == layen.frame_height)
					break;
				row_position = lpos;
			}		
			else if ((*it)->is_text())
				--lpos;
			if (it == layen.texels.begin())
				return;
		} 
		size_t c = 0;			for (++it;; ++it){
			if ((*it)->is_new_line() || (*it)->is_row())
				break;
			if ((*it)->is_text()){
				++c;
				if (c == caret.column_memory)
					break;
			} 
		}
//			auto p = min(lpos + caret.column_memory -1 , row_position -1);
		c = c == -1 ? 0:c;
		caret.move_to(lpos + c -1);
		caret.info.scroll_up = 0;				
		return;
	}
	fit_caret ();
}

void editor_c::marking ( int para1, int para2 ){
	lamb_c* lamb = lamb_c::self;
	marks_c& ringmarks = lamb_c::self -> ringmarks;
	marks_c& automarks = lamb_c::self -> automarks;
	if ( para1 == GO_TO_MARK ){
		if ( para2 == 1 ) {
			echo << "to next mark\n";
			mark_c  m( *this );
			ringmarks.to_next_mark ( *this, 0);
		}
		else if ( para2 == 2 ) {
			if ( this != lamb->sensor ) {
				return;
			}
			mark_c mark;			
			if ( file_path != "" ) 
				automarks.push_mark(*this, mark);
			echo << "auto\n";			
			string fnb =  file_path;
			automarks.to_next_mark(*this, 1);
			
			refresh ( fnb );
		}
		else if ( para2 == 3 ) {
			echo << "temp mark" << endl;
			lamb_c* lamb = lamb_c::self;						
			mark_c m  = lamb->temporary_mark;
			m.go_to_mark(*this);
		}
	}
	else if ( para1 == 3 ) {
		is_menu = is_menu ? false : true;
	}
	else if ( para1 == 4 ) {
		ringmarks.push_mark ( *this, mark_c(), true );
	}
}

void editor_c::moving ( int para1, int para2 )
{
	texel_caret_c& caret = *layen.get_caret ();
	if (para1 == DEC_COLUMN){
		caret.to_left(1);
		if(caret.line_length() > 1 && caret.is_line_end())
			caret.to_left(1);
		caret.set_column_memory ();
	}
	else if (para1 == INC_COLUMN ){
		caret.to_right(1);
		if(caret.line_length() > 1){
			if (caret.is_line_end())
				caret.to_right(1);
			else if (caret.is_eof())
				caret.to_left(1);
		}
		caret.set_column_memory ();
	}
	else if (para1 == DEC_ROW){
		caret.dec_line();
		if(caret.is_line_end() && caret.line_length() > 1)
			caret.to_left(1);
	}
	else if (para1 == INC_ROW){
		caret.inc_line();
		if(caret.line_length() > 1 )
			if(caret.is_line_end() || caret.is_eof())
				caret.to_left(1);
	}		
	else if (para1 == NEXT_WORD_BEG)
		caret.next_word();
	else if (para1 == PREV_WORD_BEG)
		caret.previous_word ();
	else if (para1 == NEXT_WORD_END)
		caret.next_word_end();
	else if (para1 == LINE_BEG){
		caret.to_line_begin();
		return;
	}
	else if (para1 == LINE_LAST_CHARACTER) {		
		caret.to_line_end();
		if (caret.line_length() > 1)
			caret.to_left(1);
		return;	
	}
	else if (para1 == LINE_END){
		caret.to_line_end();
		return;		
	}
	else if ( para1 == NEW_LINE_U ){
		caret.to_line_begin();		
		caret.unbind_selector();
		auto& caret = *layen.get_caret();
		string s{'\n'};	
		s.insert(s.end(), caret.get_leading_tabs(), '\t');
		layen.replace_text(s);
		caret.bind_selector();
	}
	else if ( para1 == NEW_LINE_D ){
		caret.to_line_end();		
		auto& caret = *layen.get_caret();
		string s{'\n'};	
		s.insert(s.end(), caret.get_leading_tabs(), '\t');
		layen.replace_text(s);
		caret.bind_selector();
	}
	fit_scrolls ();
	
}

void editor_c::clear ()
{
	string st;
	layen.set_text ( st);
	layen.set_caret();
	layen.get_caret()->move_to ( 1 );
	fit_scrolls ();		
}

bool _download ( string& url, stringstream& ss )
{
	string lynxs_path="/home/henry/shelf/cpp/lamb/lynx";
	ifstream f ( lynxs_path +"/pages/pb/" + url );
	if ( !f.good()){
		return false;
	}
	ss << f.rdbuf ();
	return true;
}

void editor_c::import_webpage (string url)
{
	lamb_c& lamb = *lamb_c::self;
	static int c = 0;
	string s, s1;
//	url="www.a"+ to_string (++c );
	s="hier is the world\n hihi" + sc11::to_string (++c );
	bookmarks_c bm;
	string page;
	string file_name;
	stringstream ss;
	echo << "url: " << url << endl;
	if ( !bm.look_in_cash ( url, file_name )){
		if ( _download ( url, ss )){
			bm.add_to_cash ( url, ss );
			cout << "url pseudo download" << endl;
		}
		else if ( lamb.download ( url, ss )){
			bm.add_to_cash ( url, ss );
			cout << "url downloaded" << endl;
		}
		else{
			cout << "url download fail" << endl;
			return;
		}
	}
	else {
		ifstream f ( file_name );
		ss << f.rdbuf ();
		cout << "file name:" << file_name << endl;
		cout << "url from cash" << endl;
	}
	str = ss.str ();
	file_path = url;
	convert_to_LF_format ( str );
	layen.set_text ( str );
	layen.set_caret ();
	fit_scrolls ();
	draw ( );
}

void editor_c::edit_file_cash()
{
	lamb_c& lamb = *lamb_c::self;
	string s;
	for ( auto x: lamb.file_cash.files){
		s += x.first + '\n';
	}
	layen.set_text(s);
	layen.set_caret();
}

void editor_c::write_file_cash()
{
	lamb_c& lamb = *lamb_c::self;
	string s = layen.text();
	stringstream ss{s};
	vector<string> v;
	for (;getline(ss,s);){
		v.push_back(s);
	}
	auto & files = lamb.file_cash.files;
	for ( auto i = files.begin(); i != files.end();) {
		auto x = v.begin();
		for (; x != v.end(); x++ ) {
			if (*x == i->first){
				break;
			}
		}
		if (x == v.end()){
			i == files.erase(i);
		}
		else{
			i++;
		}
	}
}

void editor_c::write_file_cash_to_disk ()
{
	echo << "write to disk" << endl;
	lamb_c& lamb = *lamb_c::self;
	ofstream fout;
	for ( auto &x : lamb.file_cash.files ) {
		echo << x.first <<'\n';
		fout.open ( x.first );
		fout << x.second;
		fout.close();
	}
}

void editor_c::export_text ( string filename )
{
	if (filename == "" || is_collapsed)
		return;
	lamb_c& lamb = *lamb_c::self;
	string str1 = layen.text();
	lamb.file_cash.files[filename] = str1 ;	
}

void editor_c::import_text ( string filename, bool refresh )
{
	lamb_c& lamb = *lamb_c::self;
	if ( filename == "" ){
		echo << "no file name\n";
		return;
	}
	stringstream ss;
	auto it = lamb.file_cash.files.find ( filename ); 
	if(it == lamb.file_cash.files.end()){
		ifstream fin ( filename );
		if(fin.good())
			ss << fin.rdbuf ();
		str = ss.str ();
		unsigned char chdum;
		sheck_string ( str, 0, &chdum );
		convert_to_LF_format ( str );
		echo << "not in file cash\n";
	}
	else{
		str = it->second;
	}
	layen.set_text(str);
	layen.set_caret();
}

void editor_c::refresh ( string name )
{
	for ( auto o : land.llist ) {
		if ( o ->get_type () == EDITOR ) {
			auto ed = static_cast < editor_c* > ( o );
			if (ed->file_path == name){
				texel_caret_c& caret = *ed->layen.get_caret ();
				size_t pos = caret.get_position ();
				ed->import_text ( name, true );
				caret.move_to(pos);
			}
		}
	}
}

void editor_c::fit_caret ()
{
	texel_caret_c& caret = *layen.get_caret ();
	caret.info.set();	
	if (caret.info.scroll_up < 0){
		caret.to_row_down(-caret.info.scroll_up);
		caret.info.scroll_up = 0;
	}	
	else if(caret.info.scroll_up >= frame_height-1){
		caret.to_row_up(caret.info.scroll_up - (frame_height-1)+1);	
		caret.info.scroll_up = frame_height-2;
	}
}

void editor_c::fit_scrolls()
{
	auto& caret = *layen.get_caret ();
	size_t row, column, size;
	if (!layen.texels.empty() && layen.texels.front()->is_row()){
		caret.info.set();
		row = caret.info.row;
	}
	else{
		layen.invalide_caret();
		row = caret.get_row ( 0, 0, 0 );
	}
	if (caret.info.scroll_up < 0 ){
		caret.info.scroll_up = 0;
	} 
	else if (caret.info.scroll_up >= frame_height-1){
		caret.info.scroll_up = frame_height-2;
	}
}

void editor_c::shift3(matrix_c <FT>& v)  
{
	echo << "lamb shift3" << endl;
	matrix_c<FT> vA = motion.object_vector ( 1 );
	vA = vA+v;
	motion.set_object_vector(1, vA);
}

void editor_c::test(string& s)
{
	echo << "test 00" << endl;
	cout << "here " <<endl;
	echo << s << endl;
}

int editor_c::ctab(stringstream& ss) 
{
	echo << "ctab: " << ss.str () << endl;
	string s;
	ss >> s;
	comtab_editor_c& t = comtab_editor;
	if ( s == "d" ) {
		t.destruct ();
	}
	else if ( s == "du" ) {
		echo << "here" << endl;
		t.dump ( t.msc );
	}
	else {
		t.construct();
	}	
}

string comtab_editor_c::ct = R"***(
class editor
c , nm , 'r'  	;	inserting_overwriting one_character
c , nm , 'R'  	;	inseting_overwriting
c , nm , 'a'  	;	inserting_overwriting next_character
c , nm , 'A'  	;	inserting_overwriting one_character_past_line_end
v , nm , 'A'  	;	inserting_overwriting one_character_past_line_end
c , nm , 'n'  	;	collapsing
c , nm , 'm'  	;	marking 3 1
c , cr , 'r'  	;	marking	go_to_mark 3
c , cr , 'g'  	;	marking	go_to_mark 1
c , cl , 'm'  	;	marking	go_to_mark 3
c i , cl , 'u'	;	marking	go_to_mark 2
c , cr , 'e'  	;	marking 4 1
v , nm , xk_h 	;	moving left select
c , nm , xk_h 	;	moving left
v , nm , xk_l 	;	moving right select
c , nm , xk_l 	;	moving right
v , nm , xk_k 	;	moving up select
c , nm , xk_k 	;	moving up
v , nm , xk_j 	;	moving down select
c , nm , xk_j 	;	moving down
v , nm , 'w'	;	moving next_word_begin select
c , nm , 'w'	;	moving next_word_begin 
v , nm , 'b'	;	moving previous_word_begin select
c , nm , 'b'	;	moving previous_word_begin
v , nm , 'e'	;	moving next_word_end select
c , nm , 'e'	;	moving next_word_end 
v , nm , '0'	;	moving line_begin select
c , nm , '0'	;	moving line_begin 
v , nm , '$'	;	moving line_last_character select
c , nm , '$'	;	moving line_last_character 
c , nm , 'O'	;	moving new_line_up
c , nm , 'o'	;	moving new_line_down
c , nm , 'i'	;	change_mode insert
c , nm , 'v'	;	change_mode visual
c , nm , 's'	;	change_mode select
c i v , nm , xk_escape	;	change_mode command
c , cr , 'b'  	;	scrolling page_up
v , cr , 'b'  	;	scrolling page_up select
c , cr , 'f'  	;	scrolling page_down
v , cr , 'f'  	;	scrolling page_down select
c , nm , '1G' 	;	scrolling home 
v , nm , '1G' 	;	scrolling home select
c , nm , 'G'  	;	scrolling end
v , nm , 'G'  	;	scrolling end select
c , nm , 'x'  	;	deleting character
v , nm , 'a'  	;	deleting space
v , nm , 's'  	;	deleting tabulator
c i , cl , 'o'	;	runing stack
c i , cr , 't'	;	runing run
c , nm , 'dd' 	;	editing cut_line
c , nm , 'dw' 	;	editing cut_word
c , nm , 'yy' 	;	editing copy_line
v , nm , 'y'  	;	editing copy
v , nm , 'd'  	;	editing cut
v , nm , 'c'  	;	editing change
c v , nm , 'p'	;	editing paste
v , nm , 'f'  	;	editing xchange
)***";

void editor_c::menu(unsigned long control, unsigned long v, string c)
{
	static string menu_char_accumulator;
	string& ca = menu_char_accumulator;
	ca += c;
	echo << "menu::" << ca << endl;
	bool ok = false;	
	lamb_c& lamb = *lamb_c::self;
	texel_caret_c* tc = layen.get_caret ();
	stringstream ss;
	ss << file_path << " " << layen.scroll_down << " " << tc -> get_position ();
	string mark = ss.str();
	if (v == XK_Escape) {
		is_menu = false;
		ca = "";
		return;
	}
	if (ca == "e") {
		string s = ":mark show m";
		string c;
		unsigned long v = XK_Return;
		command2 ( v, c, s );
		lamb_c& lamb = *lamb_c::self;
		lamb.next_object ( lamb.motors, lamb.motor, 0 );
		lamb.mouth.invalide_visual ( 0 );
		land.invalide_all_objects ( 0 );
		string st;
		lamb.mouth.layen.set_text ( st = ":mark mark " );
		lamb.mouth.layen.set_caret( );
		lamb.mouth.layen.scroll_down = 0;
		texel_caret_c* tc = lamb.mouth.layen.get_caret ();
		tc->move_to(tc->get_last_position());
		lamb.mouth.mode = INSERT;
		is_menu = false;
		ca = "";
		return;
	}	
	if (ca == "tt") {
		lamb_c& lamb = *lamb_c::self;
		lamb.next_object ( lamb.motors, lamb.motor, 0 );
		lamb.mouth.invalide_visual ( 0 );
		land.invalide_all_objects ( 0 );
		string st;
		lamb.mouth.layen.set_text ( st = ":mark to " );
		lamb.mouth.layen.set_caret ( );
		lamb.mouth.layen.scroll_down = 0;
		texel_caret_c* tc = lamb.mouth.layen.get_caret ();
		tc->move_to ( tc->get_last_position () );
		lamb.mouth.mode = INSERT;
		is_menu = false;
		ca = "";
		return;
	}	
	if ( ca == "tm" ) {
		string s = ":mark show m";
		string c;
		unsigned long v = XK_Return;
		command2 ( v, c, s );
		lamb_c& lamb = *lamb_c::self;
		lamb.next_object ( lamb.motors, lamb.motor, 0 );
		lamb.mouth.invalide_visual ( 0 );
		land.invalide_all_objects ( 0 );
		string st;
		lamb.mouth.layen.set_text ( st = ":mark toline m " );
		lamb.mouth.layen.set_caret ();
		lamb.mouth.layen.scroll_down = 0;
		texel_caret_c* tc = lamb.mouth.layen.get_caret ();
		tc->move_to ( tc -> get_last_position () );
		lamb.mouth.mode = INSERT;
		is_menu = false;
		ca = "";
		return;
	}	
	if ( ca == "ta" ) {
		string s = ":mark show am";
		string c;
		unsigned long v = XK_Return;
		command2 ( v, c, s );
		lamb_c& lamb = *lamb_c::self;
		lamb.next_object ( lamb.motors, lamb.motor, 0 );
		lamb.mouth.invalide_visual ( 0 );
		land.invalide_all_objects ( 0 );
		string st;
		lamb.mouth.layen.set_text ( st = ":mark toline am " );
		lamb.mouth.layen.set_caret ( );
		lamb.mouth.layen.scroll_down = 0;
		texel_caret_c* tc = lamb.mouth.layen.get_caret ();
		tc->move_to ( tc -> get_last_position () );
		lamb.mouth.mode = INSERT;
		is_menu = false;
		ca = "";
		return;
	}	
	if ( ca == "j" ) {
		for ( int c = 0; c < 50; c++ ) {
			echo << endl;	
		}
		string s = ":mark show m";
		unsigned long v = XK_Return;
		string c;
		command2 ( v , c , s );
		lamb_c& lamb = *lamb_c::self;
		lamb.next_object ( lamb.motors, lamb.motor, 0 );
		lamb.mouth.invalide_visual ( 0 );
		land.invalide_all_objects ( 0 );
		string st;
		lamb.mouth.layen.set_text (st = ":mark set  ");
		lamb.mouth.layen.set_caret ( );
		lamb.mouth.layen.scroll_down = 0;
		texel_caret_c* tc = lamb.mouth.layen.get_caret ();
		tc->move_to ( tc -> get_last_position () );
			lamb.mouth.mode = INSERT;
		ca = "j";
	}
	if ( ca == "jd"  || ca == "ju" ) {
		string s;
		if ( ca == "jd" ) {
			s = ":mark set 0 -1";
		}
		else {
			s = ":mark set 0 1";
		}
		unsigned long v = XK_Return;
		string c;
		is_menu = true;
		command2( v, c , s );
		s = ":mark show m";	
		is_menu = true;
		command2( v, c, s );
		
		ca = "j";
		return;
	}
	if ( ca == "je" ) {
		string s;
		s = ":mark set 0 0 ";
		unsigned long v = XK_Return;
		string c;
		is_menu = true;
		command2( v, c , s );
		s = ":mark show m";	
		is_menu = true;
		command2( v, c, s );
		ca = "j";
		return;
	}
	if ( ca != "" && isdigit (ca[0]  ) ){
		int i = stoi ( ca.substr ( 0, 1 ) );
		if ( lamb.bookmarks .l.size () >= i ) {
			auto it = lamb.bookmarks.l.begin ();
			advance ( it, i-1 );
			it -> go_to_mark ( *this );
			ca = "";
			is_menu = false;
			return;
		} 	
	}
	if ( ca == "1" ) {
		if ( lamb.bookmarks .l.size () >= 1 ) {
			auto it = lamb.bookmarks.l.begin ();
			advance ( it, 0 );
			it -> go_to_mark ( *this );
//			lamb.bookmarks.go_to_mark ( *this, *it );
			is_menu = false;
			ca = "";
			return;
		} 	
	}
	if ( ca == "sm" ) {
		string s = ":mark show m";
		string c;
		unsigned long v = XK_Return;
		command2 ( v, c, s );
		is_menu = false;
		ca = "";
		return;	
	}
	if ( ca == "sa" ) {
		string s = ":mark show am";
		string c;
		unsigned long v = XK_Return;
		command2 ( v, c, s );
		is_menu = false;
		ca = "";
		return;	
	}
	if ( ca == "sr" ) {
		string s = ":mark show rm";
		string c;
		unsigned long v = XK_Return;
		command2 ( v, c, s );
		is_menu = false;
		ca = "";
		return;	
	}
	if ( ca == "oc"){
		lamb_c& lamb = *lamb_c::self;
		string s0 = lamb.project + " clean"; 
		is_menu = false;
		ca = "";
		runing2(s0);
		return;
	}
	if ( ca.front() == 'o' && ca.size() == 2){
		lamb_c& lamb = *lamb_c::self;
		string s{ca.substr(1,1)};
		if ( isdigit(s[0])){
			int d = stoi(s);
			lamb.project = *next(lamb.projects.begin(), d-1);
			is_menu = false;
			ca = "";
		}
		return;
	} 
}

int editor_c::edit( lamb_c& lamb, keyboard_c& keyb )
{
	keyboard_c& kb = keyb;
	bool proceeded = false;

	if ( valide > 1 ){
		 valide = 1;
	}
	if ( !kb.is_pressed () ) {
		cout << "key releases" << endl;
		draw();
		return false;
	}
	string c = kb.get_char ();
	int v = kb.get_stroke ();
	unsigned long control = kb.get_controls ();
	string semstr = kb.get_stroke_semantic ();

	if ( v== XK_Escape ) {
//		 echo << "edit escape" << endl;
	}	
	if ( flags != 0 ){
		echo << "edit flag != 0 " << endl;
		 if ( v== XK_Escape ){
			echo << "edit flag != 0 escape" << endl;
			flag = flags;
			flags = 0;		
			string nc = "n";
			string ns = "n";
			command2(0, nc, ns);
			mode = 5;
			return false;
		}
	}
	if ( repeater_count > -100 ) {
		repeater_count --;	
	}
	if(v == XK_Escape){
		cout << "escape1" << endl;	

		layen.invalide_caret();
		string d1, d2("reset_repeat");	
		command2(XK_Return, d1, d2);
	}
	if ( is_menu ) {
		menu ( control, v, c );
		return 0;
	}
	if ( mode == 5 ){
		cout << "mode 5\n";
		string sss = "";
		command2 ( v, c, sss );
		return false;
	}
	if (editor_c::comtab_editor.find ( this, mode, control, v , c, semstr ) ) {
		return true;
	} 	
	if ( mode == 4 ){
		return false;
	}
	string st;
	if(  (!kb.is_representable(v) ||  control != 0)  )
	{
		if ( v == XK_Down && control == LL_KA_C ){
			layen.scroll_down++;
			fit_caret ();
			proceeded = true;
		}
		else if ( v == XK_Up && control == LL_KA_C ){
			layen.scroll_down--;
			fit_caret ();
			proceeded = true;
		}
		else if ( v == XK_Escape ) {
			keyboard.stroke_list.clear ();
			proceeded = true;
		}
		else if ( mode == INSERT && v == XK_BackSpace ){
			texel_caret_c& caret = *layen.get_caret();
			caret.unbind_selector();
			caret.to_left(1);	
			layen.erase_text();
			caret.bind_selector();
			caret.set_column_memory();
			fit_scrolls();
			proceeded = true;
		}
		else if ( mode == INSERT && v == XK_Return ){
			auto& caret = *layen.get_caret();
			string s = '\n' + string(caret.get_leading_tabs(),'\t');
			layen.replace_text(s);
			caret.bind_selector();
			fit_scrolls();
			proceeded = true;
		}
		else if ( mode == INSERT && v == XK_Tab && control == 0 ){
			string s{"\t"};
			layen.replace_text(s);
			auto& caret = *layen.get_caret();
			caret.bind_selector();
			fit_scrolls();
			proceeded = true;
		}		
	}
	else {
		if ( c !="" && mode== INSERT && !is_collapsed ){
			layen.replace_text(c);
			auto& caret = *layen.get_caret();
			caret.bind_selector();
			fit_scrolls();
			proceeded = true;
		}
	}
	if ( lamb.motor == &lamb.mouth ){
		string s;
		if ( v == XK_Return ){
			texel_caret_c& caret = *layen.get_caret();
			caret.unbind_selector();
			caret.to_left(1);
			caret.to_line_begin();			
			s = layen.copy_text();
			caret.swap_selector();
			caret.bind_selector();
			if(lamb.sensor != 0)
				lamb.sensor->command2(v, c, s);
			proceeded = true;
		}
	}
	return proceeded;
}

void editor_c::shift_page ( int begin, int end, ll::Direction direction )
{
	lamb_c& lamb = *lamb_c::self;
	int fline;
	int fbegin;
	int fend;
	if ( begin == 0 ){
		fbegin = 1; 
	}
	else if ( begin==-1 ){
		fbegin = frame_height;
	}
	else{
		fbegin = begin-layen.scroll_down;
	}
	if ( end == 0 ){
		fend = 1;
	}
	else if ( end==-1 ){
		fend = frame_height;
	}
	else{
		fend = end-layen.scroll_down;
	}
	if ( fbegin <= fend ){
		if ( fbegin > frame_height || fend<1 ){
			 return;
		}
	}
	else if ( fbegin>fend ){
		if ( fbegin<1 || fend>frame_height ){
			return;
		}
	}
	matrix_c<FT> pA = motion.object_vector ( 1 );
	matrix_c<FT> m = motion.object_base ();
	matrix_c<FT> pab ({0,(FT )-20*fbegin, 0});
	matrix_c<FT> pa ({(FT )20*frame_width, 0, 0});
	matrix_c<FT> pb ( 3, 1 );
	matrix_c<FT> pc ( 3, 1 );
	matrix_c<FT> pt ( 3, 1 );
	pb[2]=(FT )-20*(fend-fbegin );
	if ( direction == ll::up ){
		pt[2]= 20;							
	}
	else {
		pt[2]=-20;
		pab[2]= pab[2]+20;
	}			
	pa = m*pa; pb = m*pb; pab = m*pab; pc = m*pb; pt = m*pt;
	pA = pA+pab;
	lamb.eyes.translate ( pA, pa, pb, pc, pt );
	lamb.expose_image ();
}

void editor_c::screen ()
{
	lamb_c& lamb = *lamb_c::self;
	lamb.expose_image ();
}

map<int, string> edit_modes_map={
	{INSERT,"i"},
	{COMMAND,"c"},
	{VISUAL,"v"},
};

void editor_c::drawi2 ( matrix_c<FT>& t, matrix_c<FT>& T ) 
{
	retina_c& retina = *impression_clients.retina;	
	uint32_t* pi = layen.pi;
	t = retina.surfaces.front().zoom*t;	
	float 
		x0 = t (1,1),
		y0 = t (2,1),
		a11 = T (1,1),
		a21 = T (2,1),
		a12 = T ( 1,2),
		a22 = T ( 2,2),
		cy = layen.he,
		cx = layen.wi,
		x, y, x1, y1;
	int d;
//		cout << a11 << ":" << a21 << ":" << a12 << ":" << a22 << '\n';
//		cout << x0 << "::" << y0 << '\n';
	for ( y =0 ; y < cy; y++ ) {
		x1 = x0 + y*a12;
		y1 = y0 + y*a22; 
		d = 0;
		for ( x = 0; x < cx; x++ ) {
			if ( *pi != 0x00ffffff ) {
				if ( d == 1 ) {
					x1 += a11;
					y1 += a21;
				}
				else {
					x1 += d*a11;
					y1 += d*a21;
				}
				retina.set_pixel ( *memory, x1, y1 , 0 , *pi );
				d = 0;
			}
			d++;
			pi++;
		}
	}
}

void editor_c::draw ( )
{
	retina_c& retina = *impression_clients.retina;	
	surface_c& surface = retina.surfaces.front();
	if (surface.zoom != layen.font_zoom){
		layen.font_zoom= surface.zoom;
		ilayen.font_zoom = surface.zoom;
		layen.face = layen.font.set_face ("Vera-code.ttf", 13, surface.zoom);
		ilayen.face = layen.face;
		
		layen.clear_texel_map();
		ilayen.clear_texel_map();
		layen.resize(frame_width*layen.font.cell*surface.zoom, frame_height*layen.font.cell*surface.zoom, frame_height);
	}
	uint32_t* pi = layen.pi;
	if(pi ==nullptr){
		return;
	}
	size_t linex = 0,
	pilx = 0,
	tsizex =0;
	int cell = layen.font.cell*surface.zoom;
	layen.line_width = frame_width *cell - 2;
	layen.font.set_rows(layen);
	if ( layen.signo == 12){
		static int c = 0;
		mout << "draw: " << c++ << '\n';
	}
	texel_caret_c* tc = layen.get_caret ();
	if ( tc != nullptr ) {	
		if (file_path.size() < 6 || file_path.substr(file_path.size()-5) != ".html0"){
			linex = tc->line();
			pilx = tc->position_in_line();	
		}
	}
	tsizex = layen.texels.size();
	stringstream ss;
	matrix_c <FT> b, B;
	if ( parent != 0 ){
		int flag = 0;
		parent -> coocked_to_local ( this, B, b, flag );
	}
	else {
		b = motion.object_vector ( 1 ),
		B = motion.object_base ();
	}
	matrix_c<FT> 
		&a = vA,
		&A = mx,
		c = surface.motion.object_vector ( 1 ),
		C = surface.motion.object_base (),
		&d = surface.vA,
		&D = surface.mx,
		w = B*a + b,
		W = B*A,
		t = ~D*(~C*(w - c ) - d ), 
		T = ~D*~C*W,
		ua, ubb ,uA;
	
//!	float ozoom = lamb.eyes.zoom;

//!	float z = lamb.eyes.zoom;
	float z = 1;
	
	ua = W.get_column ( 1 )*(frame_width*layen.font.cell );
	ubb = W.get_column ( 2 )*(frame_height-1 )*layen.font.cell;

	uint32_t frame_color = 0x0;
	if ( state == 1 ){
		frame_color = 0xff0000;
	}
	else if ( impression_clients.is_motor && ! impression_clients.has_focus ){
		frame_color = 0xff;
	}
	else if ( impression_clients.is_motor && impression_clients.has_focus ) {
		cout << "is_motor\n";
		frame_color = 0xffff;
	}
	else if ( impression_clients.has_focus ) {
		frame_color = 0xff00;
	}
	
	retina.get_memory ( ui, &impression_clients, &memory); 
	retina.erase_memories ( ui );
	if (  simple_text == false || impression_clients.has_focus  ){
		spline.color = frame_color;
		spline.control_points = {
			{w}, 
			{w + ubb}, 
			{w + ua + ubb}
		};
		spline.draw ( );
	}
	valide = 2;
	
	uA = z*surface.world_to_view ( w );
	if ( simple_text == true && (state == 1 || impression_clients.has_focus )){
		uint32_t color = state == 1?0xff0000:0xff00;
//			lamb.eyes.grip ( uA, color );
	}
	
	layen.is_selected = false;
	
	int frameshift_X = 2;
	int frameshift_Y = -3;

	layen.min_y = layen.scroll_down*cell;
	layen.max_y=(frame_height + layen.scroll_down )*cell;
	layen.min_x = 0;
	layen.max_x = frame_width*cell;

	layen.x = frameshift_X;
	if ( ::d.rel_a == 1){			
		layen.y=-frameshift_Y;	
	}
	else{
		layen.y=-frameshift_Y+layen.min_y;	
	}
	layen.penX = 0;
	layen.penY = 0;
	
	if ( simple_text && ! impression_clients.has_focus ) { 
		layen.show_cursor = false;
	}
	else {
		layen.show_cursor = true;
	}
//	glyph_env.zoom=lamb.eyes.zoom;
	layen.zoom=z;
	for ( uint32_t* pe = pi + layen.si -1 ; pe >= pi ; pe-- ) {
		*pe=0x00ffffff;		
	}

	layen.font.engrave ( layen );

	if ( simple_text == false ){
		ilayen.font.layen=&ilayen;
		ilayen.zoom=z;
		ilayen.font.cell = cell;
		ilayen.font.tab_width = 40;
		
		ilayen.min_x= 0;
		ilayen.max_x= 0;
		ilayen.min_y = 0;
		ilayen.max_y = cell+1;
		ilayen.x = 0;
		ilayen.y = -((frame_height-1)*cell+frameshift_Y);
		ilayen.penX = 0;
		ilayen.penY = 0;
		ilayen.line_width = layen.line_width;
		ilayen.wi = layen.wi;
		ilayen.he = layen.he;
		ilayen.si = layen.si;
		ilayen.pi = layen.pi;
		
		stringstream ss;
		
		ss << " " <<  edit_modes_map[mode]
		<< info_strip
		<<  "  " << linex  
		<< ":" << pilx
		<< "\t  " << file_path ;		
		string st;
		ilayen.set_text(st =  ss.str ());
		ilayen.font.set_rows(ilayen);
		ilayen.font.engrave ( ilayen );
		ilayen.pi = nullptr;
	}
//	lamb.eyes.zoom = ozoom;
	drawi2( t, T);
	return;
}

long get_utf32 ( string& s, int& c )
{
	long lc = s.at ( c );
	c++;
	if ((lc&0x80 ) == 0 ){
		return lc;
	};
	if ((lc&0xE0 ) == 0xC0 ){
		long l2 = s.at ( c );
		c++;
		lc = lc & 0x1F;
		lc= lc << 6;
		l2 = l2 & 0x3F;
		lc = lc | l2;
		return lc;
	}
	if ((lc&0xF0 ) == 0xE0 ){
		long l2 = s.at ( c );
		c++;
		long l3 = s.at ( c );
		c++;
		lc = lc & 0x0F;
		lc= lc << 12;
		l2 = l2 & 0x3F;
		l2= l2 << 6;
		l3= l3 & 0x3F;
		lc = lc | l2 | l3;
		return lc;
	}
	if ( lc&0xF8 == 0xF0 ){
		long l2 = s.at ( c );
		c++;
		long l3 = s.at ( c );
		c++;
		long l4 = s.at ( c );
		c++;
		return 0;
	}
	echo << "err" << endl;
	return lc;
}

// ä
int editor_c::sheck_string ( string& s, unsigned char, unsigned char*)
{
	int i = 1;
	for ( auto c : s ){
//		if (  ( unsigned char ) c <  0x1F  &&  c != '\t' && c !='\n' || c >0x7f ){
		if (  ( unsigned char ) c <  0x1F  &&  c != '\t' && c !='\n' || (unsigned char )c >0x7f ){
		//******** hex is not defined !?
//			echo << hex << (int)(unsigned char )c << endl;
//			echo << "at position:" << i << "  char:" << ( int )( unsigned char ) c << endl;
		}	
		i++;
	}	
}

int editor_c::convert_to_LF_format ( string& str )
{
	int c = 0;
	bool dirty = false;
	for (;;){
		c = str.find ('\r', c );
		if ( c == string::npos ){
			break;
		}
		dirty = true;
		if ((c>0 && str.at ( c-1 ) =='\n') || ((c+1 )<str.size () && str.at ( c+1 ) =='\n') ){
			str.erase ( c, 1 );
		} 
		else{
			str.at ( c ) ='\n';
		}
	}
	if ( dirty ){
		echo << "converted to LF format"  <<  endl;
	}
	return 0; 
}

int editor_c::convert_to_CRLF_format ( string& str )
{
	echo << "convert to CR-LF" << endl;
	int c = 0;
	for (;;){
		c = str.find ('\r', c );
		if ( c==-1 ) break;
		c++;
		if ((c < str.size ()) && str.at ( c )!='\n') {
			str.replace ( c, 0, 1,'\n');
			c++;
		}
	}
	for ( c = 0;;){
		c = str.find ('\n', c );
		if ( c==-1 ) break;
		if ((c == 0 ) || (str.at ( c-1 )!='\r')){
			str.replace ( c, 0, 1,'\r');
			c++;
		}
		c++;
	}
	return 0;
}

long _get_utf32 ( string& s, int& c )
{
	long lc = s.at ( c );
	c++;
	if ((lc&0x80 ) == 0 ){
		return lc;
	};
	if ((lc&0xE0 ) == 0xC0 ){
		long l2 = s.at ( c );
		c++;
		lc = lc & 0x1F;
		lc= lc << 6;
		l2 = l2 & 0x3F;
		lc = lc | l2;
		return lc;
	}
	if ((lc&0xF0 ) == 0xE0 ){
		long l2 = s.at ( c );
		c++;
		long l3 = s.at ( c );
		c++;
		lc = lc & 0x0F;
		lc= lc << 12;
		l2 = l2 & 0x3F;
		l2= l2 << 6;
		l3= l3 & 0x3F;
	
		lc = lc | l2 | l3;
		return lc;
	}
	if ( lc&0xF8 == 0xF0 ){
		long l2 = s.at ( c );
		c++;
		long l3 = s.at ( c );
		c++;
		long l4 = s.at ( c );
		c++;
		return 0;
	}
	echo << "err" << endl;
	return lc;
}


namespace ex_editor{
void entry ( stringstream& ss )
{
	echo << "edit test\n";
	editor_c* p;
	lamb_c& lamb = *lamb_c::self;
	int c = 0;
	auto it = land.llist.begin ();
	for (; it != land.llist.end (); ++it){
		if ( (*it )->get_type () == EDITOR ){
			++c;
			p = static_cast < editor_c*> (*it);
		}
	}
	string s;
	stringstream sss;	
	p->layen.dump(s,sss);		
	echo << sss.str();
}

}// end namespace ex_editor

void ex_entry_editor (stringstream& ss )
{
	ex_editor::entry ( ss );
}
