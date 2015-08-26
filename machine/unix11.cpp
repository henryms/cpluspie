/* lamb  UNIX X11  system interface */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
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

#include "debug.h"

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

#define XI2
#define XSEL

using namespace std;

x11_c x11;

message_c* get_machine ()
{
	return &x11;	
}

message_c* get_x11 ()
{
	return &x11;
}

x11_c::x11_c () : xi2 ( *this ), xsel ( *this ), xd ( *this ) 
{
}

x11_c::~x11_c ()
{
}

//============================================
//					us keyboard scan
//
//	`	1	2	3	4	5	6	7	8	8	0	-	=	BKS
//
//		TAB 	Q	W	E	R	T	Y	U	I	O	P	[	]	\¹	   
//
//		CAP	A	S	D	F	G	H	J	K	L	;	'	\	RET
//
// SHIFT	<	Z	X	C	V	B	N	M	,	.	/	SHIFT
//	
// ¹ backslash position on some keyboard
//=============================================

map < int, int > scan_x11_x11 = {
	{47, XK_semicolon},
	{34,XK_bracketleft},
	{35,XK_bracketright},
	{20,XK_minus},
	{21,XK_equal},
	{22,XK_BackSpace},
	{9,XK_Escape},
	{51,XK_backslash},	
	{61,XK_slash},
	{36, XK_Return},
	{48,XK_apostrophe},
	{49, XK_grave},
	{24, XK_q },
	{25, XK_w},
	{26, XK_e },
	{27, XK_r },
	{28, XK_t },
	{29,XK_y},
	{52,XK_z}
};




map < int, int >& scan = scan_x11_x11;

void x11_c::save_configuration ()
{
	string lambs_path = get_lambs_path ();
	string unixcfg;
	unixcfg = lambs_path + "/config/unixcfg";
	
	int 
	xp = 0,
	yp = 0;
	unsigned int
	sxp = 0,
	syp = 0,
	bwp = 0,
	depthp = 0,
	children_n = 0;
	
	Window wroot, wparent, wdum, child_return;

	Window* children_list;
	
	XQueryTree ( display, win, &wdum, &wparent, &children_list, &children_n );	
	XFree ( children_list );
	
	XGetGeometry ( display, wparent, &wroot, &xp, &yp, &sxp, &syp, &bwp, &depthp );
	
	int dest_x_return, dest_y_return;
	
//	wroot = DefaultRootWindow ( display );
	
	XTranslateCoordinates ( display, win, wroot, -xp, -yp, &dest_x_return, &dest_y_return, &child_return );
	cout << xp<< ":" << yp << ":" << sxp << ":" << syp << ":" << image_width << ":" <<image_height << ":border:" << bwp << "\n" << 
	dest_x_return << ":" << dest_y_return << "\n";
	
	ofstream file ( unixcfg );
	if ( file.is_open () ) {
	
		file << image_width << endl;
		file << image_height << endl;
		
//		file << dest_x_return << endl;
//		file << dest_y_return << endl;
		file << xp << endl;
		file << yp << endl;
		
	}
	else{
		cout << "unixcfg: file error" << endl;
	}
	file.close ();
}

void x11_c::load_configuration ()
{
	string unixcfg = get_lambs_path ();
	unixcfg += "/config/unixcfg";
	ifstream file ( unixcfg );
	if ( file.is_open () ) {
		file >> image_width;
		file >> image_height;
		file >> pos_x;
		file >> pos_y;
	}
	else{
		image_width = 200;
		image_height = 200;
		pos_x = 20;
		pos_y = 20;
	}
	file.close ();
}

Window x11_c::create_window ( int x, int y, int width, int height )
{
	unsigned long attributes_mask;
	XSetWindowAttributes window_attributes;	

	attributes_mask = CWBackPixel | CWBorderPixel;	
	window_attributes.border_pixel = BlackPixel ( display, screen );
	window_attributes.background_pixel = WhitePixel ( display, screen );
	int px = pos_x;
	int py = pos_y;
	int sx = width;
	int sy = height;
	win = XCreateWindow(
		display,
		DefaultRootWindow (display ),
		px, py,
		sx, sy,
		0,
		DefaultDepth (display, screen ),
		InputOutput, 
		DefaultVisual (display, screen ),
		attributes_mask,
		&window_attributes
	);
	return win;
}


void x11_c::expose_image ()
{
	if ( xd.debug ) {
		if ( false ) {
			static int  i = 0;
			if ( i ==  0 ) {
				xd.out ( "*", 0, 0 );
				i = 1;
				flush ();
				return;
			}
			else {
				i = 0;
			}
		}
		else {
			xd.out ( "*", 0, 0 );
		}
	}
	XImage* image;
	int w, h;
	char *ar;
	
	retina_c* retine;
	expose(&w,&h,&ar,&retine );
	ar = (char*)retine->surfaces.front ().frame;
	
	image = XCreateImage ( display, visual, 24, ZPixmap, 0, ar, w, h, 32, 0 );
	XInitImage ( image );
	XPutImage ( display, win, copyGC, image, 0, 0, 0, 0, w, h );
}

void x11_c::exit ( int value )
{
	save_configuration ();
	cout << "exit value:" << value << endl;
	string path = get_lambs_path ();
	path += "/config/exitcfg";
	ofstream  f ( path.c_str ());
	f << value;
	f.close ();
	quit = true;
}

void x11_c::hide_system_cursor ()
{
				
	XColor xcol1, xcol2, xcol3, xcol4, xcol5;
	
	xcol1.pixel = 0xff0000;
	xcol1.red = 0xffff;
	xcol1.green = 0x0;
	xcol1.blue = 0x0;
	xcol1.flags = DoRed | DoGreen | DoBlue;
	xcol1.pad = 0;
	
	xcol2.pixel = 0xff00;
	xcol2.red = 0x00;
	xcol2.green = 0xffff;
	xcol2.blue = 0x00;
	xcol2.flags = DoRed | DoGreen | DoBlue;
	xcol2.pad = 0;

	xcol3.pixel = 0xff;
	xcol3.red = 0x00;
	xcol3.green = 0x00;
	xcol3.blue = 0xffff;
	xcol3.flags = DoRed | DoGreen | DoBlue;
	xcol3.pad = 0;
	
	xcol4.pixel = 0xffffff;
	xcol4.red = 0xffffff;
	xcol4.green = 0xffffff;
	xcol4.blue = 0xffffff;
	xcol4.flags = DoRed | DoGreen | DoBlue;
	xcol4.pad = 0;

	xcol5.pixel = 0x00;
	xcol5.red = 0x00;
	xcol5.green = 0x00;
	xcol5.blue = 0x00;
	xcol5.flags = DoRed | DoGreen | DoBlue;
	xcol5.pad = 0;
	
	unsigned char bmp1 [] = { 0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff };	
	unsigned char bmp2 [] = { 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81};
	unsigned char bmp3 [] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };	
	unsigned char bmp4 [] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	
	Pixmap pixmap1, pixmap2;
	pixmap1 = XCreatePixmapFromBitmapData ( display, win, (char*)bmp2, 8, 8, 1, 0, 1 );	
	pixmap2 = XCreatePixmapFromBitmapData ( display, win, (char*)bmp2, 8, 8, 0, 0, 1 );	
	Cursor cursor = XCreatePixmapCursor ( display, pixmap1, pixmap2, &xcol1, &xcol5, 1, 1 );
	XDefineCursor ( display, win, cursor );

}

void x11_c::show_system_cursor ()
{
	XUndefineCursor ( display, win );
}

bool x11_c::is_idle ()
{
	if ( 0 == XEventsQueued ( display, QueuedAlready ) ) {  //QueuedAfterFlush
		return true;
	}
	return false;
}

void x11_c::grab_system_cursor ( int* x, int* y )
{
	if ( x != nullptr && y != nullptr ) {
		Window root_return, child_return;
		int x_root, y_root;
		unsigned int mask_return;
		XQueryPointer ( display, win, &root_return, &child_return, &x_root, &y_root, x, y, &mask_return );
	}
	grab_scursor ();	
}

bool x11_c::grab_scursor ()
{
	unsigned int mask = ButtonPressMask | ButtonReleaseMask;
	int status = XGrabPointer ( 
		display, win, 
		true, mask, 
		GrabModeAsync, GrabModeAsync, 
		win, None, CurrentTime 
	);
	if ( status == GrabSuccess ) {
		system_cursor_grabed = true;
	}
	else {
		system_cursor_grabed = false;
	}
	xd.d_grab_status ( status );
} 

void x11_c::ungrab_system_cursor ( int x, int y )
{
	if ( ! system_cursor_grabed ) {
		return;
	}
	int bx = x, by = y;
	if ( pos_x + image_width >= screen_width && x > image_width ) {
		bx = image_width ;
	}
	else if ( pos_x <= 0 && x < 0 ) {
		bx = 0;
	}
	if ( pos_y + image_height >= screen_height  && y > image_height ) {
		by = image_height ;
	}
	else if ( pos_y <= 0  && y < 0 ) {
		by = 0;
	}
	if ( x != bx || y != by ) {
		grab_pointer ( bx, by );
		return;
	}
	xd.out ( "XUngrabPointer", 0, 0 );
	XUngrabPointer ( display, CurrentTime );
	XWarpPointer ( display, None, win, 0, 0, 0, 0, x, y );
	show_system_cursor ();
	system_cursor_grabed = false;
}

Bool XNextEventTimed(Display* dsp, XEvent* event_return, struct timeval*
tv) {

  // optimization
	if (tv == NULL) {
		XNextEvent(dsp, event_return);
		return True;
	}
	
  // the real deal

	if (XPending(dsp) == 0) {
		int fd = ConnectionNumber(dsp);
		fd_set readset;
		FD_ZERO(&readset);
		FD_SET(fd, &readset);
		if (select(fd+1, &readset, NULL, NULL, tv) == 0) {
			return False;
		} 
		else {
			XNextEvent(dsp, event_return);
			return True;
		}
	} 
	else {
		XNextEvent(dsp, event_return);
		return True;
	}
}


void x11_c::notify(string& cmd)
{
	cout << "notify: " << cmd << '\n';

	if(cmd =="get_focus"){
		cout << "notify: " << cmd << '\n';
		XSetInputFocus(display, win, RevertToNone,CurrentTime);
	}
	else if(cmd=="CLIPBOARD"){
		message_c::notify(cmd);	
	}
}



struct timeval tv = { 1, 0 };

void x11_c::open ()
{
	xd.debug = false;
	debug = true;
	
	display = XOpenDisplay ( ":0" );
	bool without_cursor = true;	
	
	if ( display == 0 ) {
		cout << "display error" << endl;
		return;
	}
	
	screen = DefaultScreen ( display );
	depth = DefaultDepth ( display, screen );
	visual= DefaultVisual ( display, screen );
	
	screen_width = XDisplayWidth ( display, screen );
	screen_height = XDisplayHeight ( display, screen );	
	load_configuration ();	
	if ( xd.debug ) {
		stringstream ss;
		ss << "screen:" << screen_width << ":" << screen_height << endl	
		<< pos_x <<":"<< pos_y<< ":" << image_width << ":" << image_height << endl;		xd.out ( ss.str () , 1, 0 );
	}
	
	win = create_window ( pos_x, pos_y, image_width, image_height );
	XStoreName ( display, win, "Lamb: x11" );	


	XClassHint* class_hint = XAllocClassHint();
	class_hint->res_name = "lamb";
	class_hint->res_class = "Lamb";
	XSetClassHint(display, win, class_hint);
	XFree(class_hint);

					
	unsigned long mask = PPosition;	
	XSizeHints* size_hints = XAllocSizeHints ();
	size_hints->flags = mask;
	XSetWMNormalHints ( display, win, size_hints );
	XFree ( size_hints );
	Atom wmDeleteMessage = XInternAtom ( display, "WM_DELETE_WINDOW", False );
	XSetWMProtocols(display, win, &wmDeleteMessage, 1);
	
	copyGC = XCreateGC ( display, win, 0, NULL );
	mask = 
		StructureNotifyMask | 
		EnterWindowMask |
//		LeaveWindowMask |
		FocusChangeMask |
//		PointerMotionMask |
//		ButtonPress |
//		ButtonRelease |
		KeyReleaseMask | 
		KeyPressMask; 

		
	XSelectInput ( display, win, mask );
	XMoveResizeWindow(display, win , pos_x, pos_y, image_width, image_height);
	XMapRaised ( display, win );
	xi2.init_input2 ();
	init ();	
	
	int timi = 0;
	system_cursor_grabed = false;
	Window wroot = DefaultRootWindow ( display );
	quit = false;	
	has_focus = false;
	image_width = 0;
	while ( ! quit ) {
		XEvent e, event_return;
		Window child_return;
		if ( true ) {
			tv = { 0, 1000 };
			if (XNextEventTimed ( display, &e, &tv ) == false ) {

				timer ();
//				cout << "time :" << timi++ << '\n';
				continue;
			}
		}
		else {		
			XNextEvent ( display, &e );
		}		
		
		xd.d ( e );
		string selection;
		switch ( e.type ) {
		case ClientMessage:
			if (  e.xclient.data.l [0] == wmDeleteMessage) { 
				exit ( 0 );
			}
			break;
		case GenericEvent:  
			if ( system_cursor_grabed ) {
				if ( XGetEventData ( display, &e.xcookie ) ) {
					xi2.handle_input2_event ( &e.xcookie );
					XFreeEventData(display, &e.xcookie);
				}
			}
			break;		
		case SelectionClear:
			echo << "selection clear\n";
			if(e.xselection.send_event)
				echo << "from send_event\n";
			else
				echo << "not from send_event\n";
			selection=XGetAtomName(display, e.xselection.selection);
			notify(selection);
			break;
		case SelectionRequest:
			xsel.selection_request ( & e );
			break;
		case SelectionNotify:
			break;
		case PropertyNotify:
			break;
		case ConfigureNotify:
			while ( XCheckTypedEvent ( display, ConfigureNotify, & event_return )  ) {
				e = event_return;
			}
			if ( e.xconfigure.width != image_width || 
				e.xconfigure.height != image_height ){
				image_width = e.xconfigure.width;
				image_height = e.xconfigure.height;
				config_change ( image_width, image_height );
			}
			pos_y = e.xconfigure.y;
			pos_x = e.xconfigure.x;			
			break;
		case FocusIn:
			if(without_cursor)
				break;
			if ( e.xfocus.mode == NotifyNormal ) {
				Window root_return;
				int	root_x_return;
				int root_y_return;
				int x,  y;
				unsigned int mask_return;
				XQueryPointer ( 
					display, win, 
					&root_return, &child_return, 
					&root_x_return, &root_y_return, 	
					&x, &y , 
					& mask_return 
				);
			
				if ( x >= 0 && x < image_width && 
					y >= 0 && y < image_height ) {
					grab_scursor ();
					grab_pointer( x, y );
					hide_system_cursor ();	
				}
				focus ( true );
				has_focus = true;
				break;
			} 
			break;
		case FocusOut:
			if ( e.xfocus.mode == NotifyNormal ) {
				has_focus = false;
				focus ( false );
			}
			break;
		case EnterNotify: 
			if ( e.xcrossing.mode == NotifyNormal || 
				e.xcrossing.mode == NotifyUngrab )
				if ( ! system_cursor_grabed && has_focus ) {
					grab_scursor ();
					grab_pointer( e.xcrossing.x, e.xcrossing.y );
					hide_system_cursor ();	
				}
			break;
		case LeaveNotify:
			break;
		case MotionNotify:
			break;
		case ButtonRelease:
			break;
		case ButtonPress:
			int number;
			switch ( e.xbutton.button ){
			case Button4 :
				number = 4; break;
			case Button5 :
				number = 5; break;
			case Button1 :
				number = 1; break;
			case Button2 :
				number = 2; break;
			case Button3 :
				number = 3; break;
			default :
				number = 0;
			}
			button_pressed ( number );
			break;
		case KeyPress:
		case KeyRelease:
			unsigned short sym;
			sym = XLookupKeysym ( &e.xkey, 0 );

			unsigned short keycode;
			keycode = e.xkey.keycode;
			bool pressed;
			pressed = e.type == KeyPress ? true : false;
			if ( pressed ) {
//				echo << "sym:" << sym << "   code:" << hex << keycode << endl;
			}
			switch ( sym ){
			case XK_F2:
				quit = true;
				break;
			default:
				if ( scan.count (  (unsigned short )e.xkey.keycode ) > 0 ) {
					sym = scan [ (unsigned short) e.xkey.keycode];
				}
				key_event ( pressed, sym, 0 );
			}
		}
		if (  0 ==  XEventsQueued ( x11.display, QueuedAlready ) ) {
			xd.out ( ".", 0, 0 );
			if ( xd.debug ) {
				flush ();
			}
			idle ();
		}
	}
	
	XDestroyWindow ( display, win );
	XCloseDisplay ( display );
	return;
}

void x11_c::set_clipboard ( string s )
{	
	xsel.X11_SetClipboardText ( s );
	return;	
}

void x11_c::get_clipboard ( string& s )
{
	xsel.X11_GetClipboardText ( s );
	//echo << s << endl;		
	return;
}

int x11_c::test ( stringstream& ss ) 
{
}




