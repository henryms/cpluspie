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

static void parse_valuators ( const double *input_values, unsigned char *mask, int mask_len,
	double *output_values, int output_values_len )
{
	int const MAX_AXIS = 16;
	int i = 0, z = 0;
	int top = mask_len * 8;
	if (top > MAX_AXIS){
		top = MAX_AXIS;
	}

	memset( output_values, 0, output_values_len * sizeof(double));
	for (; i < top && z < output_values_len; i++) {
		if (XIMaskIsSet ( mask, i ) ) {
			const int value = (int) *input_values;
			output_values[z] = value;
			input_values++;
		}
		z++;
	}
}

void
xinput2_c::handle_input2_event(XGenericEventCookie *cookie)
{
	if ( cookie->extension != xinput2_opcode ) {
		return ;
	}
	switch(cookie->evtype) {
	case XI_RawMotion:
		const XIRawEvent *rawev = (const XIRawEvent*)cookie->data;
		double relative_cords [2];
	parse_valuators ( rawev->raw_values,rawev->valuators.mask,
		rawev->valuators.mask_len, relative_cords, 2);
//		echo << "move" <<  ( int ) relative_cords [ 0 ] << endl;
		
		x11 . mouse_move ( ( int ) relative_cords [ 0 ], ( int ) - relative_cords [ 1 ] );
	}
}

void
xinput2_c::init_input2()
{
	Display* display = x11.display;

	XIEventMask eventmask;
	unsigned char mask[3] = { 0,0,0 };
	int event, err;
	int major = 2, minor = 0;
	int outmajor, outminor;
	XQueryExtension(display, "XInputExtension", &xinput2_opcode, &event, &err); 

	outmajor = major;
	outminor = minor;
	XIQueryVersion(display, &outmajor, &outminor);
	
	if ( outmajor * 1000 + outminor < major * 1000 + minor ) {
		//echo << "to old  XInputExtension" << endl;
	        return;
	}
	eventmask.deviceid = XIAllMasterDevices;
	eventmask.mask_len = sizeof(mask);
	eventmask.mask = mask;

	XISetMask ( mask, XI_RawMotion );
	XISelectEvents ( display, DefaultRootWindow(display), &eventmask, 1 ); 
}
