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

#include "mstream.h"
#include "symbol/keysym.h"
#include "symbol.h"

#include "standard.h"

#include "library/wdslib.h"
#include "gate.h"

#include "matrix.h"
#include "position.h"
#include "cash.h"

#include "sens/retina.h"
#include "sens/spirit.h"
#include "message.h"

#include "global.h"

#include "bookmarks.h"
#include "object.h"
#include "line.h"
#include "spline.h"

#include "mouse.h"

#include "keyb.h"
#include "home.h"
#include "wr/texel.h"
#include "wr/shared.h"
#include "wr/layen.h"
#include "fox/edit.h"
#include "hand.h"
#include "eyes.h"


#include "lamb.h"

#include "line.h"
#include "spline.h"
#include "image.h"
#include "land.h"
#include "wr/font.h"
#include "wr/texel.h"
#include "wr/shared.h"
#include "keyb.h"
#include "edit.h"
#include "carte.h"
#include "home.h"
#include "make.h"
#include "regexp.h"

#include "shell.h"

using namespace std;

int byte_index ( string& str, int index );
int position_index ( string& str, int index );

extern land_c land;

shell_c::shell_c ()
{
}

bool shell_c::envstr ( string& s ) 
{
	size_t pos = s.find ( '$' );
	if ( pos != s.npos ) {
		string s2, v, var, value;
		size_t pos2 = s.find_first_of(" /", pos);
		if ( pos2 != string::npos ){
			s2 = s.substr ( pos2 );
		}
		v = s.substr ( pos+1, pos2- pos -1);
		s = s.substr ( 0, pos );
		string path = get_lambs_path();
		path += "/config/envvar";
		ifstream ifs( path );
		while (ifs) {
			getline ( ifs, var, '=' ); 		
			getline ( ifs, value );
			if ( var == v ) {
				s = s + value + s2; 
				return true;
			} 
		}	
		echo << "es: " << v;
		char * cp = getenv ( v.c_str () );
		if ( cp != NULL ) {
			value = cp;	
			s = s + value + s2;
			return true;
		}
		return false;
	}
	return true;
}

bool find_gcc_output ( string& s, size_t i, string& file_name, int * line, int * column )
{
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

bool shell_c::show_compiler_error (
	string makefile_path, 
	editor_c& edit, 
	stringstream& ss) 
{
		lamb_c& lamb = *lamb_c::self;
		string s, s1, s2;
		bool flag_ok = true;
		s = ss.str();
		if ( string::npos != s.find("error:") || string::npos != s.find ( "Error" ) ){
			flag_ok = false;
		}
		if ( flag_ok == false ){
			string str = ss.str ();
			echo.ss2.str("");
			echo.ss2 << str;
			lamb.ears.layen.set_text(str);
			lamb.ears.layen.set_caret();
			lamb.ears.layen.font.set_rows(lamb.ears.layen, &lamb.ears.layen.texels);
			string find;
			string search(" error:");
			size_t indez = 0;
			lamb.ears.regexp.find(str, indez, 0, 0, search, find);
			texel_caret_c* tc = lamb.ears.layen.get_caret();
			size_t position = position_index(str, indez);
			cout  << "pos:" << position << endl;
			tc->move_to2(position + 1);
			tc->move_selector_to (position + find.size () + 2);
			tc->info.scroll_up = 15;
			lamb.ears.layen.scroll_down = max ((int)tc->info.row - 15, 0);			
			int line, column;
			string file_name;
			string ses = str.substr ( 0, indez );
			if (find_gcc_output(ses, indez, file_name, &line, &column)) {
				echo << endl <<  "+++" << file_name << " line:" << line << " col:" << column << endl; 
				cout << endl <<  "+++" << file_name << " line:" << line << " col:" << column << endl; 
	
				edit.file_path = makefile_path+ "/" + file_name;
 				edit.import_text ( edit.file_path );
				auto& caret = *edit.layen.get_caret ();
				edit.layen.font.set_rows(edit.layen, &edit.layen.texels);
				size_t pp = caret.get_line_position(line);
				caret.move_to2(pp+ column-1);
				caret.info.scroll_up = 15;
				edit.layen.scroll_down = max ( (int)caret.info.row - 15, 0 );
			}
			return true;
		}	
	return false;
}

bool shell_c::show_pdflatex_error (
	string makefile_path, 
	editor_c& edit, 
	stringstream& ss) 
{
		lamb_c& lamb = *lamb_c::self;
		string s, s1, s2;
		bool flag_ok = true;
		s = ss.str();
		if ( string::npos != s.find("error:") || string::npos != s.find ( "Error" ) ){
			flag_ok = false;
		}
		if ( flag_ok == false ){
			string str = ss.str ();
			echo.ss2.str("");
			echo.ss2 << str;
			lamb.ears.layen.set_text(str);
			lamb.ears.layen.set_caret();
			lamb.ears.layen.font.set_rows(lamb.ears.layen, &lamb.ears.layen.texels);
			string find;
			string search(" Error:");
			size_t indez = 0;
			lamb.ears.regexp.find(str, indez, 0, 0, search, find);
			texel_caret_c* tc = lamb.ears.layen.get_caret();
			size_t position = position_index(str, indez);
			cout  << "pos:" << position << endl;
			tc->move_to2(position + 1);
			tc->move_selector_to (position + find.size () + 2);
			tc->info.scroll_up = 15;
			lamb.ears.layen.scroll_down = max ((int)tc->info.row - 15, 0);			
			/*
			int line, column;
			string file_name;
			string ses = str.substr ( 0, indez );
			if (find_gcc_output(ses, indez, file_name, &line, &column)) {
				echo << endl <<  "+++" << file_name << " line:" << line << " col:" << column << endl; 
				cout << endl <<  "+++" << file_name << " line:" << line << " col:" << column << endl; 
	
				edit.file_path = makefile_path+ "/" + file_name;
 				edit.import_text ( edit.file_path );
				auto& caret = *edit.layen.get_caret ();
				edit.layen.font.set_rows(edit.layen, &edit.layen.texels);
				size_t pp = caret.get_line_position(line);
				caret.move_to2(pp+ column-1);
				caret.info.scroll_up = 15;
				edit.layen.scroll_down = max ( (int)caret.info.row - 15, 0 );
			}
			*/
			return true;
		}	
	return false;
}

void shell_c::sh(stringstream& ss)
{
	lamb_c& lamb = *lamb_c::self;
	string com = ss.str();
	stringstream sse;
	lamb.system_echo(com, sse);
	echo << sse.str();
}

void shell_c::run ( stringstream& ss ) 
{
	lamb_c& lamb = *lamb_c::self;
	string lambs_path = get_lambs_path();
	string si, s, s1, s2, s3;

	ss >> si >> s >> s1;
	echo << s << "::" << s1 << endl;
	if ( s == "compile" ) {
		string com = "rm /home/henry/shelf/cpp/wss/lamb.o";
		stringstream sss;
		lamb.system_echo ( com, sss );
		
		echo.clear ();
		editor_c* peditor;
		for ( auto it=land.llist.begin() ; it != land.llist.end (); it++ ){
			object_c* po = *it;
			if ( po->get_type () == EDITOR &&  po != &lamb.mouth ){
				peditor = ( editor_c* ) *it;
				break;
			}	
		}
		editor_c& edit = *peditor;
		string platform, bin_path, path, make, exe;
		path = get_lambs_path ();
		ifstream f ( path + "/config/environment" );
		f >> platform;
		f >> bin_path;
		f.close ();
		make = "make" + platform;
		string command;
		stringstream ss;
		if ( s1 ==  "clean" ){
			exe =  " EXE=../" + bin_path + "/sens/libsens.so_out";
			command = "cd "+ path + "/sens && make -f " + make + exe + " clean";
			lamb.system_echo ( command, ss );
			
			exe =  " EXE=../" + bin_path + "/wr/liblayen.so_out";
			command = "cd "+ path + "/wr && make -f " + make + exe + " clean";
			lamb.system_echo ( command, ss );
			
			exe =  " EXE=" + bin_path + "/lamb_out";
			command = "cd "+ path + " && make -f " + make + exe + " clean";
			lamb.system_echo ( command, ss );
			
			echo << ss.str ();
			return;
		}
		echo << "file path\n" << edit.file_path << endl;
		home_c& home = lamb.home2;
		for ( auto it = home.mobilar_lst.rbegin () ; it != home.mobilar_lst.rend () ; it ++ ) {
			if ( it->tag == "editor" ) {
				mark_c mark;
				lamb.automarks.push_mark ( *it -> editor, mark ); 
			}
		}						
		
		lamb.start_command = "";
		ss.seekp ( 0 );
		
		exe =  " EXE=../" + bin_path + "/sens/libsens.so_out";
		command = "cd " + path + "/sens  && make -j6 -f "+ make + exe;
		lamb.system_echo ( command, ss );

		if ( show_compiler_error (path +"/sens", edit, ss ) ) {
			lamb.ears.valide = 0;
			return;
		}
		
		exe =  " EXE=../" + bin_path + "/wr/liblayen.so_out";
		command = "cd " + path + "/wr  && make -j6 -f "+ make + exe;
		lamb.system_echo ( command, ss );

		if ( show_compiler_error (path +"/wr", edit, ss ) ) {
			lamb.ears.valide = 0;
			return;
		}
		
		exe =  " EXE=" + bin_path + "/lamb_out";
		command = "cd "+ path + " && make -j6 -f " + make + exe;
		lamb.system_echo ( command, ss );
	
		if ( show_compiler_error (path, edit, ss ) ) {
			lamb.ears.valide = 0;
			return;
		}
		
		lamb.release_pointer ();
		lamb.exit ( wcp::restart );
	}
	else if ( s.substr(0,6)  == "viewer" ) {
		string pltfrm;
		if ( s == "viewerl" ) {
			pltfrm = "linux";
		} 
		else {
			pltfrm = "x11";
		}
		editor_c* peditor;
		auto it  = land.llist.begin ();
		for (; it != land.llist.end (); it++ ){
			object_c* po = *it;
			if ( po->get_type () == EDITOR &&  po != &lamb.mouth ){
				peditor = ( editor_c* ) *it;
				break;
			}	
		}
		echo.clear ();
		string 
		cmd = "cd /home/henry/shelf/cpp/wss/wv && make -j6 -f make"+pltfrm,
		cmdc = cmd + " clean";
		stringstream ss0;
		ss >> s1;
		if ( s1 == "clean" ) {
			lamb.system_echo (cmdc, ss0 );
			return;
		}
		lamb.system_echo ( cmd, ss0 );
		if (  show_compiler_error ( "/home/henry/shelf/cpp/wss/wv", *peditor, ss0 ) ) {
			echo << ss0.str ();
		}
		else {
			echo << "viewer build " << pltfrm << " OK" << endl;
		}
	}
	else if ( s == "cpp"){
		editor_c* peditor;
		auto it  = land.llist.begin ();
		for (; it != land.llist.end (); it++ ){
			object_c* po = *it;
			if ( po->get_type () == EDITOR &&  po != &lamb.mouth ){
				peditor = ( editor_c* ) *it;
				break;
			}	
		}
		echo.clear ();
		string 
		cmdr,
		cmd = "make -j6" ,
		cmdc = cmd + " clean";
		stringstream ss0;
		ss >> s1;
		if ( s1 == "clean" ) {
			lamb.system_echo (cmdc, ss0 );
			return;
		}
		lamb.system_echo(cmd, ss0);
		if(show_compiler_error(".", *peditor, ss0)){
			echo << ss0.str ();
			return;
		}
		echo << ss0.str();
		echo << "build ok" << endl;
		cmdr = s1;
		ss0.str("");
		lamb.system_echo(cmdr, ss0);
		echo <<  ss0.str() << endl;
	}
	else if ( s == "tex" ) {
		echo << "here text\n";
		string path, name, cmd, sc, tmp;
		tmp = get_lambs_path() + "/tmp";
		size_t 
		pos = s1.rfind ('/'),
		pos2 = s1.rfind ('.');
		if ( pos == string::npos ){
			path = ".";
			name = s1.substr (0, pos2 );
		}
		else {
			path = s1.substr ( 0, pos );
			name = s1.substr ( pos + 1, pos2 - pos  - 1);
		}
	
		
		cmd = "cd " + path + "&& pdflatex -interaction=nonstopmode " + name  + ".tex";
		echo << "shell: " << cmd << endl;
		
		ss.clear ();
		ss.str ( "" );
		lamb.system_echo ( cmd, ss );
		editor_c* peditor{nullptr};	
		if ( show_pdflatex_error ( "", *peditor, ss ) ) {
			lamb.command("ho cpp2");	
			echo << ss.str ();
			return;
		}
		
		list<string> vs{"1", "01", "001", "0001"};
		ifstream ifs;
		for(int c = vs.size(); c > 0 ; --c){
			vs.push_back(vs.front());
			vs.pop_front();
//			move(vs.begin(), next(vs.begin(), 1), vs.end());
			
			echo << vs.back() << " ";
			ifs.open(tmp + "/" + name + "-" + vs.back() + ".ppm");
			if(ifs){
				ifs.close();
				break;
			}
		}	
		for ( auto x : vs){
//			echo << x << " ";
		}
		echo << endl;
		ss.clear();		
		ss.str ( "" );
		scale(lamb.eyes.height+350, 773, 205, 205 ,120, 90, sc);
//		scale ( 300, 773, 205, 205 ,120, 90, sc);
		cmd = "pdftoppm " + sc + " " + path + "/" + name + ".pdf " + tmp + "/" + name;
		echo << cmd << endl;
		lamb.system_echo ( cmd, ss );
	
		for(;!vs.empty();){
			ifs.open(tmp + "/" + name + "-" + vs.front() + ".ppm");	
			if (ifs){
				ifs.close();
				break;
			}	
			vs.pop_front();
		}	
		if(vs.size() > 1){
			string qm;
			qm.assign(vs.back().size(),'?');
			cmd = "rm " + tmp + "/" + name + "-" + qm + ".ppm";
			ss.clear();
			ss.str("");
			echo << cmd << '\n';
			
			lamb.system_echo(cmd, ss);
		}					
		else{
			echo << "vs.size():" << vs.size() << endl;
		}
//		cmd = "ho tex2";
//		lamb.command(cmd);	
	
		for ( auto p : land.llist ) {
			if ( p -> get_type () == WOBJECT ) {
				mout << "refresh shell\n";
				string s { "refresh"}, s2;
				p -> command2 (0, s, s2 );
				return;
			}	
		}
		lamb.eyes.off = true;
		lamb.hand.off = true;
		cmd = "asv " + tmp + "/" + name + "-" + vs.front() + ".ppm";

		lamb.command(cmd);
		return;
	}
	else if ( s == "textest" ) {
		string path, name, cmd, sc, tmp;
		tmp = get_lambs_path() + "/tmp";
		size_t 
		pos = s1.rfind ('/'),
		pos2 = s1.rfind ('.');
		if ( pos == string::npos ){
			path = ".";
			name = s1.substr (0, pos2 );
		}
		else {
			path = s1.substr ( 0, pos );
			name = s1.substr ( pos + 1, pos2 - pos  - 1);
		}
		cmd = "cd " + path + "&& pdflatex -interaction=nonstopmode " + name  + ".tex";
		echo << "shell: " << cmd << '\n';
		ss.clear ();
		ss.str ( "" );
		lamb.system_echo ( cmd, ss );
		echo << ss.str ();
		ss.clear();		
		ss.str ( "" );
//		scale(lamb.eyes.height+350, 773, 205, 205 ,120, 90, sc);
		scale ( 350, 773, 205, 205 ,120, 90, sc);
		cmd = "pdftoppm " + sc + " " + path + "/" + name + ".pdf " + tmp + "/" + name;
		echo << cmd << '\n';
		lamb.system_echo ( cmd, ss );
		
		for ( auto p : land.llist ) {
			if ( p -> get_type () == WOBJECT ) {
				mout << "refresh shell\n";
				string s { "refresh"}, s2;
				p -> command2 (0, s, s2 );
				return;
				
			}	
		}
		string sp{tmp+"/"+name},
		sf;	
		vector<string> files{"-0001","-001","-01", "-1"};	
		ifstream ifs;
		for(;!files.empty();){
			sf = sp+files.back()+".ppm";
			ifs.open(sf);
			if(ifs){
				break;
			}
			files.pop_back();
		}
		ifs.close();
		cmd = "asv " + sf;
		lamb.command(cmd);
		return;
	}
	else if ( s == "pdf" ) {
		echo.clear ();
		echo << "fbgs" << endl;
		string command = "fbgs " + s1 ;
		ss.clear ();
		ss.str ("");
		system ( command.c_str () );
	}
	else if ( s == "nsb" ) {
		echo.clear ();
		editor_c* peditor;
		auto it  = land.llist.begin ();
		for (; it != land.llist.end (); it++ ){
			object_c* po = *it;
			if ( po->get_type () == EDITOR &&  po != &lamb.mouth ){
				peditor = ( editor_c* ) *it;
				break;
			}	
		}

		ss >> s1;
		echo << "nsbx" << endl;
		string command = "cd /home/henry/netsurf-full-3.0 && ./a " +  s1;
		ss.clear ();
		lamb.system_echo ( command, ss );
		if ( s1 != "clean" ) {
			if (  show_compiler_error ( "/home/henry/netsurf-full-3.0/src/libnsfb-0.1.0", *peditor, ss ) ) {
				echo << ss.str ();
			}
			else {
				echo << "ns build OK" << endl;
			}
		}
	}
	else if ( s == "ns" ) {
		echo.clear ();
//		string command = "source /home/henry/netsurf-full-3.0/aa" ;
		string command = "sh /home/henry/netsurf-full-3.0/aa &" ;
		ss.clear ();
		system ( command.c_str () );
	}
	echo << "shell.run" << endl;
}
