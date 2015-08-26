#ifndef LLOOP_H
#define LLOOP_H

using namespace std;



class loop_c
{
	public:
	loop_c ();
	~loop_c ();
	int initialize ();	
	int input ( event_s&);
	bool wait_for_event ( event_s&);
	list<event_s> le;	
	int mouse;
	int keyb;
	int touchpad;
	char* KEYBOARD;
	char* MOUSE;
	char* TOUCHPAD;
	struct termios saved_terminal;

};


#endif