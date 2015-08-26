#ifndef EVENT_H
#define EVENT_H

struct event_s
{
	int type;
	int param1;
	int param2; 
};

#define LL_pointer_button_1 1
#define LL_pointer_button_2 2
#define LL_pointer_button_3 3
#define LL_pointer_button_4 4
#define LL_pointer_button_5 5

#define LL_key_pressed 6
#define LL_key_released 7
#define LL_mouse_move 8
#define LL_no_event 9
#define LL_quit 10
#define LL_timeout 11
#endif