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
#include <thread>
#include <chrono>

//#include <boost/thread.hpp>
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
#include "sens/spirit.h"
#include "message.h"
#include "global.h"

#include "machine/posix_sys.h"
#include "machine/socket.h"

#include "machine/machine.h"
#include "machine/unix11.h"
#include "machine/socket.h"
#include "machine/xsel.h"


void xselection_c::selection_request (XEvent* event ) 
{

	int xresult;

	XEvent& xevent = *event;
	
	XSelectionRequestEvent *req;
	XEvent sevent;
	int seln_format;
	unsigned long nbytes;
	unsigned long overflow;
	unsigned char *seln_data;

	req = &xevent.xselectionrequest;

	sevent.xany.type = SelectionNotify;
	sevent.xselection.selection = req->selection;
	sevent.xselection.target = None;
	sevent.xselection.property = None;
	sevent.xselection.requestor = req->requestor;
	sevent.xselection.time = req->time;
	xresult = XGetWindowProperty (
		x11.display,
		DefaultRootWindow ( x11.display ) ,
		XA_CUT_BUFFER0,
		0,
		10000,
//		INT_MAX/4,
		False,
		req -> target,
		&sevent.xselection.target,
		&seln_format,
		&nbytes,
		&overflow,
		&seln_data
	);
	if ( xresult == Success ) {
		Atom XA_TARGETS = XInternAtom(x11.display, "TARGETS", 0);
		if (sevent.xselection.target == req->target) {
			XChangeProperty(x11.display, req->requestor, req->property,
			sevent.xselection.target, seln_format, PropModeReplace,
			seln_data, nbytes);
			sevent.xselection.property = req->property;
		} 
		else if (XA_TARGETS == req->target) {
			Atom SupportedFormats[] = { sevent.xselection.target, XA_TARGETS };
			XChangeProperty(x11.display, req->requestor, req->property,
			XA_ATOM, 32, PropModeReplace,
			(unsigned char*)SupportedFormats,
			sizeof(SupportedFormats)/sizeof(*SupportedFormats));
			sevent.xselection.property = req->property;
		}
		XFree(seln_data);
	}
	XSendEvent(x11.display, req->requestor, False, 0, &sevent);
	XSync(x11.display, False);

}

int
xselection_c::X11_SetClipboardText(string s)
{

	Window window = x11.win;
	
	Atom format = XInternAtom ( x11.display, "UTF8_STRING", false );
	Atom XA_CLIPBOARD = XInternAtom ( x11.display, "CLIPBOARD", 0 );

	// Save the selection on the root window //

	XChangeProperty ( 
		x11.display, 
		DefaultRootWindow(x11.display),
		XA_CUT_BUFFER0, 
		format, 
		8, 
		PropModeReplace,
		(const unsigned char* )s.c_str (), 
		s.size () 
	);
	if ( XA_CLIPBOARD != None &&
	XGetSelectionOwner (x11.display, XA_CLIPBOARD) != window ) {
		XSetSelectionOwner ( x11.display, XA_CLIPBOARD, window, CurrentTime );
	}
	if ( XGetSelectionOwner ( x11.display, XA_PRIMARY) != window ) {
		XSetSelectionOwner ( x11.display, XA_PRIMARY, window, CurrentTime );
	}
	return 0;

}

int predicate ( Display* display, XEvent *event, XPointer arg )
{
	if ( event -> type == SelectionNotify ) {
		return true;
	}
	return false;
}


void foo  (Display* display, Atom XA_CLIPBOARD, Atom format, Atom selection, Window owner ) 
{
//	return;
//	echo << " foo" << endl;
	XConvertSelection ( 
		display, 
		XA_CLIPBOARD, 
		format, 
		selection, 
		owner,
		CurrentTime
	);
}

void bar ( int i ) 
{
	return;
}

void xselection_c:: X11_GetClipboardText ( string& t )
{
	

	int xresult;
	
	Window window = x11.win;
	Window owner;
	Atom selection;
	Atom seln_type;
	int seln_format;
	unsigned long nbytes;
	unsigned long overflow;
	unsigned char *src;
	Atom XA_CLIPBOARD = XInternAtom ( x11.display, "CLIPBOARD", 0 );
	if (XA_CLIPBOARD == None) {
		//echo << "no clipboard" << endl;
		return;
	}

	
// Get the window that holds the selection //

	Atom format = XInternAtom ( x11.display, "UTF8_STRING", false );
	
	owner = XGetSelectionOwner(x11.display, XA_CLIPBOARD);
	if ((owner == None) || (owner == window)) {
		owner = DefaultRootWindow(x11.display);
		selection = XA_CUT_BUFFER0;
	} 
	else {
// Request that the selection owner copy the data to our window //
		owner = window;

		selection = XInternAtom(x11.display, "WSS_SELECTION", False);


//		thread first ( foo,   x11.display,  XA_CLIPBOARD, format, selection, owner  ) ;
//		first.join ();		

		XConvertSelection ( 
			x11.display, 
			XA_CLIPBOARD, 
			format, 
			selection, 
			owner,
			CurrentTime
		);
		
		XEvent event;
		XPointer arg = "";
/*		
		using namespace std::chrono;
		steady_clock::time_point t1 = system_clock::now ();
		duration < double > time_span;
*/
		while ( ! XCheckTypedEvent ( x11.display, SelectionNotify, &event ) ) {
		usleep( 10000);
		/*
			system_clock::time_point t2 = system_clock::now ();
			time_span =	duration_cast < duration < double >> (t2 - t1 );
			if ( time_span.count () > .01 ) {
				//echo << "too long " << endl;
				break;
			}
		*/
		};
		//echo << time_span.count () << endl;
	}
		xresult = XGetWindowProperty (
		x11.display,
		owner,
		selection,
		0,
		10000,
//		INT_MAX/4,
		False,
		format,
		&seln_type,
		&seln_format,
		&nbytes,
		&overflow,
		&src
	 );
	if ( xresult == Success ) {
		if ( seln_type == format ) {
			t.assign ( (char* ) src, nbytes );
		}
		XFree ( src );
	}
}


