#ifndef MAIN_H
#define  MAIN_H

#define INFINITE 1000000

using namespace std;

class land_c;

class comtab1_lamb_c;
class comtab2_lamb_c;
template < typename T>
class command_table_c;

class lamb_c: public object_c
{
	public: 
	lamb_c ();
	~lamb_c ();
	matrix_c<FT> mx;
	matrix_c<FT> local_position;
	void local ( object_c&, matrix_c<FT> vA, matrix_c<FT> vb, matrix_c<FT> vx );	
	matrix_c<FT> view_to_world ( matrix_c<FT>& point );
	matrix_c<FT> world_to_view ( matrix_c<FT>& point );	
	matrix_c<FT> view_to_object ( object_c&, matrix_c<FT>&);
	matrix_c<FT> view_from_object ( object_c&, matrix_c<FT>&);
	virtual void coocked_to_local ( object_c* o, matrix_c < FT > & lbase, matrix_c <FT >& lvA, int& flag);
	mouse_c mouse;
	void draw ( lamb_c& );
	void draw ();
	void rotatesel ( int lr, int du, int zlr );
	void shift_selection ( int bf, int lr, int du );
	void copy_selection ();
	void point ( int lr, int du, int bf );  
	virtual void move ( int type, int x, int y, int z );
	void write ( bool is_pressed, uint16_t stroke );
	void change_movement ( int para1, int para2 );
	virtual int command2 ( unsigned char, string& , string&);
	bool command ( string str );
	void debug(string);
	void breakpoint();
	int command_number;
	keyboard_c keyboard;
	int movement;
	int shift_rotate;
	int keyboard_mouse;
	bool verbal;
	string directory;
	list <string> projects;	
	string project;
	string start_command;
	bool is_cursor_captured;
	string pathlamb;
	shortmarks_c ringmarks;
	marks_c bookmarks;
	automarks_c automarks;	
	mark_c temporary_mark;
	int return_code;
	static unsigned long type;
	virtual unsigned long get_type (){return type;};
	void grab_pointer ();
	void release_pointer ();
	virtual void mouse_move ( int, int );
	virtual void idle ();
	virtual void timer();
	virtual void notify(string& note);
	virtual void flush();
	void write_status ();	
	virtual void key_event ( bool, unsigned short, unsigned char );
	void to_default ( int, string );
	virtual void init ();
	virtual void expose ( int*, int*, char**, retina_c**);
	virtual void focus ( bool gained );
	int edit ( lamb_c&, keyboard_c&);
	virtual void grab_pointer ( int, int );
	virtual void button_pressed ( int );
	virtual void config_change ( int, int );
	void resize_fonts ( float f );
	virtual void matrices_to_view ( object_c& object, matrix_c<FT>* base, matrix_c<FT>* translation );
	void shift ( object_c*, int, int, int );
	void rotate ( object_c*, int, int, int );
	void shift3 ( matrix_c<FT>&);
	object_c* sensor;
	object_c* motor;
	object_c* hook;
	
	eyes_c eyes;
	hand_c hand;
	spirit_c spirit;
	editor_c ears;
	editor_c mouth;
	editor_c status;
	static lamb_c* self;	
	bool is_printing;
	
	string home;
	home_c home2;
	string grf_file;
	
	list<object_c*>motors;
	list<object_c*>::iterator next_object ( list<object_c*>&, object_c*&, int );
	list<object_c*>::iterator previous_object ( list<object_c*>&, object_c*&, int );
	
	bool none_event;
	bool no_refresh;	
	bool stream_in;	
	void testing ( stringstream& ss );
	
	window_manager_c window_manager;

	static comtab1_lamb_c comtab1_lamb;
	static comtab2_lamb_c comtab2_lamb;
	
	int ctab ( stringstream& ss );
	
	list < object_c* > mobilar;		
	void separate ( object_c* );				
	static string path_navigator;
	static string _path_pdf_viewer;
	
	cash_c file_cash;	
	
};
#endif
