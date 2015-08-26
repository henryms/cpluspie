#ifndef WOBJECT_H
#define WOBJECT_H

using namespace std;

class lamb_c;
class keyboard_c;
struct message;
class message_c;
class surface_c;
class map_memory_c;
class map_memory2_c;


class wobject_c : public object_c
{
public:
	wobject_c();
	wobject_c(string s);
	void quit ();
	void draw ( );
	void shift3 ( matrix_c<FT>& v );
	int edit ( lamb_c&, keyboard_c&);
	unsigned long get_type ();
	static const unsigned long type;
	static wobject_c* create ();
	virtual int command2 ( unsigned short, string&, string&);
};

#endif
