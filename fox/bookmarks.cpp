#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_set>
#include <functional>

#include <regex>
#include <iterator>
#include <iterator>


#include "standard.h"

#include "symbol/keysym.h"

#include "library/wdslib.h"

#include "gate.h"
#include "matrix.h"
#include "position.h"
#include "mstream.h"
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
#include "cash.h"
#include "lamb.h"

#include "bookmarks.h"
#include "image.h"
#include "make.h"

#include "global.h"

#include "bookmarks.h"

extern land_c land;

using namespace std;

mark_c::mark_c  () : position ( 1 ), scroll ( 0 ), line ( 1 ), position_in_line ( 1 ) , status ( 0 ) 
{
}

mark_c::mark_c  ( const mark_c& m)  
{
	name = m.name;
	pattern = m.pattern;
	position = m.position;
	scroll = m.scroll;
	scroll_up = m.scroll_up;
	line = m.line;
	position_in_line = m.position_in_line;
	tag = m.tag;
	info = m.info;
	status = m.status;
}

mark_c::mark_c (  editor_c& edit ) : status ( 0 ), line ( 0 )
{
	if ( edit.file_path == "" ) {
		return ;
	}
	if ( edit.file_path.substr ( 0, 1 ) != "/" ) {
		lamb_c::self -> get_cwd ( name ) ;
		name += "/";
	}
	name += edit.file_path;
	texel_caret_c* tc = edit.layen.get_caret ();
	position = tc -> get_position ();
	scroll_up = tc->info.scroll_up;
} 

size_t mark_c::get_position ()
{
	return position;
}

string mark_c::get_name ()
{
	return name;
}

marks_c::marks_c ()
{
}

void mark_c::go_to_mark ( editor_c& edit ) 
{
	edit.file_path = name;
	edit.import_text ( edit.file_path );
	auto& caret = *edit.layen.get_caret ();
	caret.info.scroll_up = scroll_up;
	caret.move_to_line(line, position_in_line);
	cout << "go to mark: " << line << ":" << position_in_line << '\n';
	return;
}

void marks_c::show ( editor_c& edit) 
{
	int c = l.size ();
	for ( auto it = l.rbegin (); it != l.rend (); it++, --c ) {
		mark_c x = *it;
		if ( true ) {
			echo << c << " " << x.name 
			<< " " << x.scroll << " " << x.scroll_up << " " <<x.position <<  " " <<  x.tag << endl;		
		}
		else {
			echo << c << " " << x.name <<  endl 
			<< x. position << endl 
			<< x. scroll <<  endl
			<< x. line << endl
			<< x. position_in_line << endl
			<< x. tag << endl
			<< x. info << endl
			<< x. status << endl;
		}
	}
	echo << "size:" << l.size () << endl;
}

void marks_c::mark ( editor_c& edit, mark_c mark, int index,  string tag )
{
	if ( index < 0 || index > l.size () + 1 ) {
		return;
	}
	if ( edit.file_path == "" ) {
		return;
	}
	auto& caret = *edit.layen.get_caret();
	string fn = edit.file_path; 
	if ( fn.substr ( 0, 1 ) != "/" ) {
		string ff;
		lamb_c::self -> get_cwd ( ff ); 
		fn = ff + "/" + fn;
	}
	mark.name = fn;
	mark.line = caret.line();
	mark.position_in_line = caret.position_in_line();
	mark.scroll_up = caret.scroll_up();
	
	mark.position = caret.get_position();
	mark.scroll = edit.layen.scroll_down;
	mark.tag = tag;

	size_t ll = caret.get_line_position ();
	mark.info = edit.layen.text ( ll, ll + 40);	
	mark.info = mark.info.substr ( 0 , mark.info.find ( "\n" ) );	
	
	echo << mark.name << endl
	<< mark.position << endl
	<< mark.scroll_up << endl	
	<< mark.scroll << endl
	<< mark.tag << endl ;

	auto it = l.begin ();
	if ( index == 0 ) {
		l.push_front ( mark);
	} 
	else if ( index == l.size () + 1 ) {
		l.push_back ( mark );
	}
	else if ( index <= l.size ()  ) {
		advance ( it, index - 1 );
		l.insert ( it, mark );
	}
}

void marks_c::push_mark ( editor_c& edit, mark_c mark, bool toggle ) 
{
	stringstream ss;
	string s, t;
	if ( mark.name == "" ){
		if ( edit.file_path == "" ) {
			return ;
		}
		if ( edit.file_path.substr ( 0, 1 ) != "/" ) {
			lamb_c::self -> get_cwd ( s ) ;
			ss << s << "/";
			s += "/";
		}
		s += edit.file_path;
		auto& caret = *edit.layen.get_caret();
		caret.info.set(true);
//		ss << edit.file_path << " " << edit.layen.scroll_down << " " << 
		caret.get_position ();
		mark.name = s;
		mark.position = caret.get_position ();
		mark.scroll_up = caret.info.scroll_up;
		mark.scroll = edit.layen.scroll_down;
		mark.line = caret.line();
		mark.position_in_line = caret.position_in_line();
	}
	
	auto it = l.begin ();	
	for ( ; it != l.end (); it++ ) {
		if ( it -> get_name () == mark.get_name () 
			&& it -> get_position () == mark.get_position () ){
			if ( it == l.begin () ) {
				if ( toggle ) {
					l.pop_front ();
					echo << "remove mark" << endl;
				}
				return;
			}
			else {
				l.push_front ( *it );
				l.erase ( it );
				echo << "restack mark" << endl;
			}
		}
	}
	
	if ( it == l.end () ){
		echo << "set mark" << endl;
		echo << mark.name ;		
		auto& caret = *edit.layen.get_caret ();
		caret.info.set(1);		
		size_t pos;
		size_t line = caret.get_line ( &pos, 0 ); 
		echo << "   " << line << "  " << caret.get_position() - pos << endl;

		size_t ll = caret.get_line_position();
						
		mark.info = edit.layen.text ( ll, ll + 80);
		if ( mark.info != "" ){
			mark.info = mark.info.substr ( 0, mark.info.find ( "\n" ) );
		}
		l.push_front ( mark );
		if ( l.size () > 10 ) {
			l.resize ( 10 );
		}
	}
	return;
}

void shortmarks_c::to_next_mark ( editor_c& edit, int mode ) 
{
	stringstream ss;
	string s, t;
	edit.export_text ( edit.file_path );
	if ( edit.repeater_count < 0 || edit.repeater_id != "go to mark" ) {
		edit.repeater_id = "go to mark";
		edit.repeater_info = -1;
	}
	edit.repeater_count = 1;
	edit.repeater_info++;
//	echo << endl << "repeater_info:" << edit.repeater_info << endl;
	
	int c = edit.repeater_info;
	if ( c >= l.size () ) {
		return;
	}
	if ( c == 0 ) {
		mark_c m ( edit ); 
		lamb_c& lamb = *lamb_c::self;
		lamb.temporary_mark = m; 
		if ( m.get_name () == l.front ().get_name () 
			&& m.get_position () == l.front ().get_position () ){
			echo << " is equal " << endl;
			edit.repeater_info++;
			c ++;
			if ( c >= l.size () ) {
				return;
			}
		}
		else if ( m.get_name () != l.front ().get_name () ) {
			echo << " different name " << endl;

			edit.repeater_info++;
			c ++;
			if ( c >= l.size () ) {
				return;
			}
		}
		else {
			echo << " is not equal " << endl;
			l.front (). go_to_mark ( edit );
			return;
		}
	}
	auto it = l.begin ();
	advance ( it, c  );
	while ( it -> get_name () != edit.file_path ) {
		it++;
		c ++;
		if ( c >= l.size () ) {
			return;
		}
	}
	mark_c m ( l.front () );
	l.pop_front ();
	l.insert ( it, m );
	l.push_front ( *it );
	l.erase ( it );
	l.front () .go_to_mark ( edit );
}

void marks_c::to_next_mark ( editor_c& edit, int mode ) 
{
	stringstream ss;
	string s, t;
	edit.export_text ( edit.file_path );
	if(edit.repeater_count < 0 || edit.repeater_id != "go to mark"){
		edit.repeater_id = "go to mark";
		edit.repeater_info = -1;
	}
	edit.repeater_count = 1;
	edit.repeater_info++;
//	echo << endl << "repeater_info:" << edit.repeater_info << endl;
	
	int c = edit.repeater_info;
	if ( c >= l.size () ) {
		return;
	}
	
	if ( c == 0 ) {
		if ( mode == 0 ) {
			mark_c m ( edit ); 
			lamb_c& lamb = *lamb_c::self;
			lamb.temporary_mark = m; 
			if(m.get_name() == l.front().get_name() 
				&& m.get_position() == l.front().get_position()){
				edit.repeater_info++;
				c ++;
				if ( c >= l.size () ) {
					return;
				}
			}
			else if(m.get_name () != l.front().get_name()){
				edit.repeater_info++;
				c ++;
				if( c >= l.size()){
					return;
				}
			}
			else {
				echo << "go to mark..\n";
				m.go_to_mark(edit);
				return;
			}
		}
		else if ( mode == 1 ) {
			if ( edit.file_path == l.front().get_name()){
				edit.repeater_info++;
				c ++;
				if ( c >= l.size ()){
					return;
				}
			}
			else {
				l.front().go_to_mark(edit);
				return;
			}
			
		}
	}
	auto it = l.begin ();
	advance ( it, c  );
	if ( mode == 0 ) {
		while(it -> get_name () != edit.file_path){
			it++;
			c ++;
			if ( c >= l.size ()){
				return;
			}
		}
	}
	mark_c m(l.front());
	l.pop_front();
	l.insert(it, m);
	l.push_front(*it);
	l.erase(it);
	l.front().go_to_mark(edit);
}


void mark_c::serialize ( fstream&f, bool bsave ) 
{
	string dummy;
	if  ( ! bsave ) {
		getline ( f, name );
		if ( f.eof () ){
			return;
		}		
		getline ( f, pattern );
		f >> position;
		f >> scroll_up;
		f >> scroll;
		f >> line;
		f >> position_in_line;
		getline ( f, dummy );
		getline ( f, tag );
			
		getline ( f , info );
		f >> status;
		getline ( f, dummy );
	}
	else {
		f << name << endl;
		f << pattern << endl;
		f << position << endl;
		f << scroll_up << endl;
		f << scroll << endl;
		f << line << endl;
		f << position_in_line << endl;
		f << tag << endl;
		f  << info << endl;
		f << status <<endl;
	}
}

void marks_c::serialize ( string file_name, bool bsave ) 
{
	if ( ! bsave ) {
		l.clear ();
		fstream f ( file_name, ios::in );
		if ( f.fail () ) {
			return;
		}
		serialize ( f, bsave );
		return;
	}
	else {
		fstream f ( file_name, ios::out );
		serialize ( f, bsave );
		return;
	}	
}

void marks_c::serialize ( fstream& f, bool bsave ) 
{
	if ( ! bsave ) {
		int c = 10;
		while ( c-- ) {
			l.push_back ( mark_c () );
			l.back ().serialize ( f, bsave );
			if ( f.eof () ) {
				l.pop_back ();
				break;
			}
		}
	}
	else {
		for ( auto x : l ) {
			x.serialize ( f, bsave );
		}
	}	
}

list < mark_c >::iterator marks_c::find_tag ( string& tag )
{
	echo << "find_tag" << endl;
	auto it = l.begin ();
	for (; it != l.end (); it ++ ) {
		if ( it -> tag == tag ) {
			echo << "find_0" << endl;
			return it;
		}
	}
	return it;
}

list < mark_c >::iterator marks_c::find_name ( string& name ) 
{
	string path;
	if ( name.substr ( 0, 1 ) != "/" ) {
		lamb_c::self -> get_cwd ( path );
		path = path + "/";
	}
	path = path+ name;
	auto it = l.begin ();
	for (; it != l.end (); it++ ) {
		if ( it -> name == path ) {
			return it;
		}
	}
	return it;
}

list < mark_c >::iterator marks_c::find_name_and_position ( string& name, size_t position ) 
{
	string path;
	if ( name.substr ( 0, 1 ) != "/" ) {
		lamb_c::self -> get_cwd ( path );
		path = path + "/";
	}
	path = path+ name;
	auto it = l.begin ();
	for (; it != l.end (); it++ ) {
		if ( it -> name == path  && it -> position == position ) {
			return it;
		}
	}
	return it;
}


void automarks_c::push_mark ( editor_c& editor, mark_c mark, bool toogle )
{
	lamb_c& lamb =* lamb_c::self;
	auto it = lamb.automarks.find_name ( editor.file_path );
	if ( it != lamb.automarks.l.end () ){
		lamb.automarks.l.erase ( it );
	}			
	marks_c::push_mark ( editor, mark, toogle );
}


bookmarks_c::bookmarks_c()
{
}

void bookmarks_c::add_to_cash(string& url,stringstream& page)
{
	string lynxs_path="/home/henry/shelf/cpp/lamb/lynx";
	int max_nr=10;
	string cash=lynxs_path+"/cash";
	string path=lynxs_path+"/pages";
	string nr_s;	
	int nr;
	fstream f;
	f.open(cash,ios::in);
	if(!f.is_open()){
		cout << "not good" << endl;
		f.close();
		f.open(cash,ios::out);
		nr =1;
		f << nr << endl;
		f << url;
	}
	else{
		cout << "good" << endl;
		stringstream ss1;
		ss1 << f.rdbuf();
		f.close();		
		f.open(cash,ios::out);
		getline(ss1,nr_s);
//!!!		nr=stoi(nr_s);
		if(nr < max_nr){
			nr++;
		}
		else{
			nr=1;
		}
		f << nr << endl;
		cout << nr << endl;
		for(int c=1;c <= max_nr;c++){
			string s1;
			getline(ss1,s1);
			if(c!=nr){
				f << s1 << endl;
			}
			else{
				f << url << endl;
			}
			if(ss1.eof() && nr<=c){
				break;
			}
		}
	}
	ofstream f_page;
	f_page.open(path+ "/" + sc11::to_string(nr));
	f_page << page.str();
}

bool bookmarks_c::look_in_cash(string& url, string& name)
{
	string lynxs_path="/home/henry/shelf/cpp/lamb/lynx";
	string line;
	ifstream f(lynxs_path + "/cash");
	if(!f.good()){
		return false;
	}
	getline(f,line);
	for(int page=1;!f.eof();page++){
		getline(f,line);
		if(line==url){
			name=lynxs_path+"/pages/"+sc11::to_string(page);
			return true;
		}			
	}
	return false;	
}



