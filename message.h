#ifndef MESSAGE_H
#define MESSAGE_H


#include "event.h"
using namespace std;

class message_c;
class retina_c;
class retina_c;
message_c* get_machine ();

class message_c
{	
	public:
	virtual int nop_system ( int );
	virtual int test ( stringstream& ss );
	virtual int system_echo ( string& command, stringstream& _echo );
	virtual int system_async_run( string& command, string& env);
	virtual int exec_async(vector<string>& command, vector<string>& env);	
	virtual int system_pipe( string& command);
	virtual void walk2(string,list<pair< int, string>>&);
	virtual void walk( string, list<string>&);
	virtual void get_cwd( string& directory );
	virtual void change_pwd( string directory );
	virtual void open();
	virtual void mouse_move( int, int );
	virtual void idle();
	virtual void timer();
	virtual void notify(string&);
	virtual void key_event ( bool, unsigned short, unsigned char );
	virtual void exit( int );
	virtual bool is_idle ();
	virtual void expose_image ();
	virtual bool download ( string&, stringstream&);
	virtual void ungrab_system_cursor ( int, int );
	virtual void init();
	virtual void expose( int*, int*, char**, retina_c**);
	virtual void focus( bool );
	virtual void grab_pointer( int, int );
	virtual void button_pressed( int );
	virtual void config_change( int, int );
	virtual void get_clipboard( string&);
	virtual void set_clipboard( string );	
	virtual void window_management( string );
	virtual void file_service(stringstream& ss );
	virtual void flush();
	virtual void memory_map( string name, size_t memory_size, void*& pointer, int* ref ); 
	virtual bool memory_map2( string name, size_t memory_size, char*& pointer ); 
	virtual void unmap_memory_map( void* pointer );
	virtual char* map_file( string& file_name);
	virtual void output( event_s& event);
	message_c* that;
};

#endif //MESSAGE_H
