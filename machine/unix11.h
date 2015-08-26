#ifndef UNIX11_H
#define UNIX11_H

#include "xinput2.h"
#include "xsel.h"
#include "xdump.h"
class x11_c: public posix_sys_c
{
	public:
	x11_c ();
	~x11_c ();

	Display* display;
	int screen;
	int depth;
	Visual* visual;
	Window win;
	GC copyGC;
	void expose_image ();
	void open ();
	Window create_window ( int x, int y, int width, int height );
	Window create_input_window ( int x, int y, int width, int height );
	virtual void exit ( int );
	virtual bool is_idle ();
	virtual void notify(string&);
	
	void hide_system_cursor ();
	void show_system_cursor ();
	void grab_system_cursor ( int* x, int *y );
	bool grab_scursor ( );
	void ungrab_system_cursor ( int x, int y );	
//	virtual void flush ();
	bool system_cursor_grabed;
	bool has_focus;
	bool quit;
	void save_configuration ();
	void load_configuration ();
	
	void selection_request ( XEvent* );
	
	int screen_width;
	int screen_height;	
	
	xinput2_c xi2;	
	xselection_c xsel;
	
	virtual void set_clipboard ( string );
	virtual void get_clipboard ( string& );
	
	bool debug;	
	xdump_c xd;
	void dump_event ( XEvent e );
	int test ( stringstream& ss );
};
#endif
