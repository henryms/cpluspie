#ifndef XINPUT2_H
#define XINPUT2_H

class x11_c;
class xinput2_c 
{
public:
	xinput2_c (x11_c& _x11): x11 ( _x11 ) {};
	x11_c& x11;
	void init_input2 ();
	void handle_input2_event( XGenericEventCookie *cookie );
// * Opcode returned XQueryExtension
// * It will be used in event processing
// * to know that the event came from
// * this extension */
	int xinput2_opcode;

};




#endif