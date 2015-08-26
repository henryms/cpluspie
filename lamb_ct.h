#ifndef LAMB_CT_H
#define LAMB_CT_H





class command_lamb2_c : public command_base_c
{
public:
	virtual int run(lamb_c* oo) {}	
	virtual command_lamb2_c* create () {};	
};

class comtab1_lamb_c : public comtab_c
{
public:
	comtab1_lamb_c (){ cout <<  "com1 lamb cons\n";}
	void construct ( );
	~comtab1_lamb_c() { cout << "comtab1 des" << endl;}
	void destruct ();
	bool find ( lamb_c*, int, int, unsigned long, string, string );
	map < string, command_lamb2_c* > msc;
	static string ct;	
	finder_c finder;
};

class comtab2_lamb_c: public comtab_c
{
public:
	comtab2_lamb_c (){cout << "com2 lamb const\n";}
	//comtab2_lamb_c (const comtab2_lamb_c& ) {}
	~comtab2_lamb_c() { cout << "comtab2 des" << endl;}

	void construct ( );
	void destruct ();
	bool find ( lamb_c*, int, int, unsigned long, string, string );
	map < string, command_lamb2_c* > msc;
	static string ct;	
	finder_c finder;
};

class lamb_change_movement2_c : public command_lamb2_c 
{
public:
	lamb_change_movement2_c (){}
	lamb_change_movement2_c ( string s ){ semantic = s;}
	virtual ~lamb_change_movement2_c (){}
	int para1;
	int para2;
	virtual int run(lamb_c* o);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	command_lamb2_c* create ();	
};

class lamb_button_pressed2_c : public command_lamb2_c 
{
public:
	lamb_button_pressed2_c ( string s ) { semantic = s; }
	virtual ~lamb_button_pressed2_c (){}
	int para1;
	virtual int run(lamb_c* oo);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	lamb_button_pressed2_c ( ) {};
	command_lamb2_c* create ();
};

class lamb_move_c : public command_lamb2_c 
{
public:
	lamb_move_c( string s ) { semantic = s ; }
	virtual ~lamb_move_c (){}
	int para1;
	int para2;
	int para3;
	int para4;
	lamb_c * o;
	virtual int run(lamb_c* o);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	lamb_move_c ( ) {};
	command_lamb2_c* create ();
};

class lamb_shift2_c : public command_lamb2_c 
{
public:
	lamb_shift2_c( string s ) { semantic = s ; }
	virtual ~lamb_shift2_c (){}
	object_c* po;
	int para2;
	int para3;
	int para4;
	lamb_c * o;
	virtual int run(lamb_c* o);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	lamb_shift2_c ( ) {};
	command_lamb2_c* create ();
};

class lamb_rotate_c : public command_lamb2_c 
{
public:
	lamb_rotate_c( string s ) { semantic = s ; }
	virtual ~lamb_rotate_c (){}
	object_c* po;
	int para2;
	int para3;
	int para4;
	lamb_c * o;
	virtual int run(lamb_c* o);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	lamb_rotate_c ( ) {};
	command_lamb2_c* create ();
};



#endif