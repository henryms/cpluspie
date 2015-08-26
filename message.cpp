#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <sstream>

#include "mstream.h"
#include "gate.h"
#include "matrix.h"
#include "position.h"
#include "sens/retina.h"
#include "sens/spirit.h"
#include "message.h"
#include "event.h"

using namespace std;

void message_c::walk2(string s,list<pair< int, string>>& l)
{
	that->walk2(s,l);
} 

void message_c::walk(string s,list<string>& l)
{
	that->walk(s,l);
} 

void message_c::flush ()
{
	that -> flush ();
}

int message_c::system_echo ( string& command, stringstream& _echo )
{
	return that -> system_echo ( command, _echo );
}

int message_c::system_async_run( string& command, string& env)
{
	that ->system_async_run(command, env);
}

int message_c::exec_async( vector<string>& command, vector<string>& env)
{
	that ->exec_async(command, env);
}

int message_c::system_pipe(string& command)
{
	that ->system_pipe(command);
}

void message_c::change_pwd(string directory)
{
	that->change_pwd(directory);
}

void message_c::get_cwd ( string& directory )
{
	that->get_cwd ( directory );
}

int message_c::nop_system(int x)
{
	return that->nop_system(x);
}

int message_c::test ( stringstream & ss)
{
	return that->test ( ss );
}

void message_c::open()
{
	that->open();	
}

void message_c::mouse_move(int x,int y)
{
	that->mouse_move(x,y);
}

void message_c::idle()
{
	that->idle();
}

void message_c::timer()
{
	that->timer();
}

void message_c::notify(string& s)
{
	that->notify(s);
}

void message_c::key_event(bool pressed,unsigned short us, unsigned char uc)
{
	that->key_event(pressed,us,uc);
}

void message_c::exit(int code)
{
	that->exit(code);
}

bool message_c::is_idle()
{
	return that->is_idle();
}

void message_c::expose_image()
{
	that->expose_image();
}

bool message_c::download(string& url,stringstream& result)
{
	return that->download(url, result);
}

void message_c::ungrab_system_cursor(int x, int y)
{
	that->ungrab_system_cursor(x,y);
}

void message_c::init()
{
	that->init();
}

void message_c::expose(int* px, int* py, char ** pcp, retina_c** retine)
{
	that->expose(px,py,pcp,retine);
}

void message_c::config_change(int x,int y)
{
	that->config_change(x,y);
}
		
void message_c::focus ( bool focused)
{
	that->focus ( focused );
}

void message_c::grab_pointer(int x, int y)
{
	that->grab_pointer(x,y);
}

void message_c::button_pressed(int x)
{
	that->button_pressed(x);
}

void message_c::get_clipboard(string& s)
{
	that->get_clipboard(s);
}

void message_c::set_clipboard(string s)
{
	that->set_clipboard(s);
}

void message_c::file_service ( stringstream& ss )
{
	that -> file_service ( ss );
}

void message_c::window_management(string s)
{
	that->window_management(s);
}				

void message_c::memory_map ( string name, size_t ms, void*& pointer, int* ref ) 
{	
	that-> memory_map ( name, ms,  pointer, ref );
}

bool message_c::memory_map2 ( string name, size_t ms, char*& pointer ) 
{	
	return that-> memory_map2 ( name, ms,  pointer );
}

void message_c::unmap_memory_map ( void* pointer ) 
{	
	that-> unmap_memory_map ( pointer ); 
}

char* message_c::map_file ( string& file_name )
{
	that -> map_file ( file_name );
}

void message_c::output ( event_s& event )
{
	that -> output ( event );
}





