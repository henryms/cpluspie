#ifndef EDIT_CT_H
#define EDIT_CT_H


class command_editor2_c : public command_base_c
{
public:
	virtual int run(editor_c* o) {}	
	virtual command_editor2_c* create () {};	
};

class comtab_editor_c : public comtab_c
{
public: 
	comtab_editor_c (){ cout << "com ed cons\n";}
	~comtab_editor_c(){ cout << "com_ed2 destruction\n";}
//	comtab_editor_c (const comtab_editor_c& ) {}
	void construct ( );
	void destruct ();
	bool find ( editor_c*, int, int, unsigned long, string, string );
	map < string, command_editor2_c* > msc;
	static string ct;	
	finder_c finder;
};

class editor_inserting_overwriting_c : public command_editor2_c 
{
public:
	virtual ~editor_inserting_overwriting_c (){}
	int para1;
	int para2;
	virtual int run(editor_c* o);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	editor_inserting_overwriting_c ( ) {};
	editor_inserting_overwriting_c ( string s ) { semantic = s; }
	command_editor2_c* create ();
};

class editor_collapsing_c : public command_editor2_c 
{
public:
	virtual ~editor_collapsing_c (){}
	int para1;
	int para2;
	virtual int run(editor_c* oo);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	editor_collapsing_c ( ) {};
	editor_collapsing_c ( string s ) { semantic = s; }
	command_editor2_c* create ();
};

class editor_marking_c : public command_editor2_c 
{
public:
	virtual ~editor_marking_c (){}
	int para1;
	int para2;
	virtual int run(editor_c* oo);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	editor_marking_c ( ) {};
	editor_marking_c ( string s ) { semantic = s; }
	command_editor2_c* create ();
};

class editor_moving2_c : public command_editor2_c 
{
public:
	virtual ~editor_moving2_c (){}
	int para1;
	int para2;
	virtual int run(editor_c* oo);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	editor_moving2_c ( ) {};
	editor_moving2_c ( string s ) { semantic = s; }
	command_editor2_c* create ();
};

class editor_deleting_c : public command_editor2_c 
{
public:
	virtual ~editor_deleting_c (){}
	int para1;
	int para2;
	virtual int run(editor_c* oo);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	editor_deleting_c ( ) {};
	editor_deleting_c ( string s ) { semantic = s; }
	command_editor2_c* create ();
};

class editor_change_mode_c : public command_editor2_c 
{
public:
	virtual ~editor_change_mode_c (){}
	int para1;
	int para2;
	virtual int run(editor_c* oo);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	editor_change_mode_c ( ) {};
	editor_change_mode_c ( string s ) { semantic = s; }
	command_editor2_c* create ();
};


class editor_scrolling_c : public command_editor2_c 
{
public:
	virtual ~editor_scrolling_c (){}
	int para1;
	int para2;
	virtual int run(editor_c* oo);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	editor_scrolling_c ( ) {};
	editor_scrolling_c ( string s ) { semantic = s; }
	command_editor2_c* create ();
};


class editor_runing_c : public command_editor2_c 
{
public:
	virtual ~editor_runing_c (){}
	int para1;
	int para2;
	virtual int run(editor_c* oo);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	editor_runing_c ( ) {};
	editor_runing_c ( string s ) { semantic = s; }
	command_editor2_c* create ();
};

class editor_editing_c : public command_editor2_c 
{
public:
	virtual ~editor_editing_c (){}
	int para1;
	virtual int run(editor_c* oo);	
	virtual void get_semantic ( string* sem );
	bool virtual set_parameter ( string par );
	string semantic;
	editor_editing_c ( ) {};
	editor_editing_c ( string s ) { semantic = s; }
	command_editor2_c* create ();
};


#endif
