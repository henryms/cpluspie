#ifndef XSEL_H
#define XSEL_H

class xselection_c 
{
	public:
	xselection_c (x11_c& _x11): x11 ( _x11 ) {};
	x11_c& x11;
	void selection_request ( XEvent* );
	int X11_SetClipboardText( string );
	void X11_GetClipboardText ( string& );
};

#endif