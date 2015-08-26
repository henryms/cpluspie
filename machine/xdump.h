#ifndef XDUMP_H
#define XDUMP_H

class x11_c;
class xdump_c 
{
public:
	xdump_c (x11_c& _x11);
	x11_c& x11;
	void dump ( XFocusChangeEvent& e, stringstream& s, int mode);
	void dump ( XCrossingEvent& e, stringstream& s, int mode );
	void dump ( XEvent& e, stringstream& s, int mode );
	void d ( XEvent& e, int mode = 0 );
	void d ( XEvent& e, stringstream&s, int mode = 0 );
	void d_grab_status ( int status, int mode  = 0 );	
	void d_grab_status ( int status, stringstream&s, int mode );	
	void d_notify_mode ( int notify_mode, stringstream& s, int mode );
	void out ( stringstream&, int mode, int endl_end );
	void out ( string s, bool, bool);
	bool debug;
	bool has_endl;
	bool has_echo;
	bool has_cout;
};

#endif