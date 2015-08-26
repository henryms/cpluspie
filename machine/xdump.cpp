#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <stdint.h>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <list>
#include <map>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/xfixesproto.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "symbol/keysym.h"

#include "gate.h"
#include "matrix.h"
#include "position.h"
#include "library/wdslib.h"
#include "sens/retina.h"
#include "sens/spirit.h"
#include "sens/retina.h"
#include "message.h"
#include "global.h"

#include "posix_sys.h"
#include "socket.h"

#include "machine.h"
#include "unix11.h"
#include "xinput2.h"
#include "xdump.h"

xdump_c::xdump_c (x11_c&  _x11 )  : 
x11 ( _x11 ), 
debug ( false ), 
has_endl ( true),
has_echo ( true ),
has_cout ( true ) 
{
};

void xdump_c::d ( XEvent& e, int mode ) 
{
	if ( ! debug ) {
		return;
	}
	if ( e.type == MotionNotify ) {
		out ( ":", 0 , 0 );
		return;
	}
	if ( e.type == GenericEvent ) {
		out ( "|" , 0, 0 );
		return;
	}
	stringstream s;
	d ( e, s, mode );
	s << "     ";
	out ( s, 1, 0);
}

void xdump_c::d ( XEvent& e, stringstream& s, int mode ) 
{
	dump ( e, s, mode );
}

void xdump_c::out ( string s , bool endl_begin, bool endl_end )
{
	if ( ! debug ) {
		return;
	}
	stringstream ss (s );
	out ( ss,  endl_begin, endl_end);
}

void xdump_c::out ( stringstream& s, int endl_begin, int  endl_end ) 
{
	if (  ! has_endl && endl_begin ) {
		if ( has_echo ) {
			//echo << endl;
		}
		if ( has_cout ) {
			cout << endl;
		}
	}
	if ( has_echo ) {
		//echo << s.str ();
	}
	if ( has_cout ) {
		cout << s.str () << flush;
	}
	if ( endl_end ) {
		if ( has_echo ) {
			//echo << endl;
		}
		if ( has_cout ) {
			cout <<  endl;
		}
		has_endl = true;
	}
	else {
		has_endl = false;
	}
	
}



void xdump_c::d_grab_status ( int status, int mode ) 
{
	if ( ! debug ) {
		return;
	}	
	stringstream s;
	d_grab_status ( status, s, mode );
	out ( s, mode, false);
}

void xdump_c::d_grab_status ( int status, stringstream& s, int mode ) {
	static map < int, string > m = {
		{AlreadyGrabbed, "AlreadyGrabbed"},
		{GrabSuccess, "GrabSuccess"}
	};
	if ( m.count ( status ) > 0 ) {
		s << m [ status ];
	}
}

void xdump_c::d_notify_mode ( int notify_mode, stringstream& s, int mode )
{
	switch ( notify_mode ) {
	case NotifyNormal:
		s << "NotifyNormal";
		break;
	case NotifyGrab:
		s << "NotifyGrab";
		break;
	case NotifyUngrab:
		s << "NotifyUngrab";
		break;
	case NotifyWhileGrabbed:
		s << "NotifyWhileGrabbed";
		break;
	default:
		s << "unknown";
	}
}

void xdump_c::dump ( XCrossingEvent& e, stringstream& s, int mode )
{
	s << " ";
	d_notify_mode ( e.mode, s, mode );	
}

void xdump_c::dump ( XFocusChangeEvent& e, stringstream& s, int mode )
{
	s << " ";
	d_notify_mode ( e.mode, s, mode );	
}

void xdump_c::dump ( XEvent& e, stringstream& s, int mode ) 
{
		static map < int, string > m = {
		{KeyPress, "KeyPress"},
		{KeyRelease, "KeyRelease"},
		{ButtonPress,"ButtonPress"},
		{ButtonRelease,"ButtonRelease"},
		{MotionNotify,"MotionNotify"},
		{EnterNotify,"EnterNotify"},
		{LeaveNotify,"LeaveNotify"},
		{FocusIn,"FocusIn"},
		{FocusOut,"FocusOut"},
		{KeymapNotify,"KeymapNotify"},
		{Expose,"Expose"},
		{GraphicsExpose,"GraphicsExpose"},
		{NoExpose,"NoExpose"},
		{VisibilityNotify,"VisibilityNotify"},
		{CreateNotify,"CreateNotify"},
		{DestroyNotify,"DestroyNotify"},
		{UnmapNotify,"UnmapNotify"},
		{MapRequest,"MapRequest"},
		{ReparentNotify,"ReparentNotify"},
		{ConfigureNotify,"ConfigureNotify"},
		{ConfigureRequest,"ConfigureRequest"},
		{GravityNotify,"GravityNotify"},
		{ResizeRequest,"ResizeRequest"},
		{CirculateNotify,"CirculateNotify"},
		{CirculateRequest,"CirculateRequest"},
		{PropertyNotify,"PropertyNotify"},
		{SelectionClear,"SelectionClear"},
		{SelectionRequest,"SelectionRequest"},
		{SelectionNotify,"SelectionNotify"},
		{ColormapNotify,"ColormapNotify"},
		{ClientMessage,"ClientMessage"},
		{MappingNotify,"MappingNofity"},
		{GenericEvent,"GenericEvent"},
		{LASTEvent, "LASTEvent"}
	};
	if ( m.count ( e.type ) > 0 ) {
		s << m [ e.type ];
		return;
	}
	s << "Unknown Notification: " << e.type;
}
