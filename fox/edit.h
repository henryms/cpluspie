#ifndef EDIT_H
#define EDIT_H

#include "../wr/font.h"
#include "regexp.h"

//Unix LF; Window CR+LF ; Apple CR :   CR 0x07 (\r )  LF 0x0a (\n ), 

class comtab_editor_c;

using namespace std;

class editor_c: public object_c
{
	public :
	bool find_word ( string&);
	string get_next_dictionary_word ( vector<texel_c*>::iterator&, vector<texel_c*>::iterator&);
	int check_spell ( int flag );	
	void scrolling ( int, int );	
	void change_mode ( int, int );
	void editing ( int );
	void marking ( int, int );
	void moving ( int, int );
	void searching ( int, int );
	void deleting ( int, int );
	void replacing ( int, int );
	void inserting_overwriting ( int, int );
	void runing2 (string);
	void runing (int, int );
	void collapsing ( int, int );
	bool is_collapsed;
	void run_search_replace ( int& flag, uint16_t v, string& c );
	void searching ( size_t* position );
	list<string> file_list;
	list<string>::iterator itfl;
	void edit_file_cash();
	void write_file_cash();
	void write_file_cash_to_disk();
	void export_text ( string name );
	void import_text ( string name, bool refresh = false );
	void import_webpage ( string url );
	void refresh ( string name );

	virtual void serialize ( fstream& file, bool save );
	string str;
	matrix_c<FT> mx;
	matrix_c<FT> vA;
	virtual void draw ( );
	void drawi2 ( matrix_c<FT>& t, matrix_c<FT>& T);
	editor_c ();
	virtual ~editor_c ();
	string info_strip;
	int mode;
	int overwrite;
	int com ( unsigned short, string& c, string& s );
	int command2 ( unsigned short, string&, string& );	
	int edit ( lamb_c& lamb, keyboard_c& keyb );
	int sheck_string ( string&, unsigned char, unsigned char*);	

	int convert_to_CRLF_format ( string&);
	int convert_to_LF_format ( string&);
	
	layen_c layen;	
	layen_c ilayen;
	keyboard_c keyboard;
	string file_path;
	int gravity;	
	int frame_width;
	int frame_height;
	int cell;
	spline_c spline;		
	void shift_page ( int begin, int end, ll::Direction direction );
	void fit_scrolls ();
	void fit_caret ();	
	void screen ();
	virtual void clear ();
	bool simple_text;
	virtual void invalide_visual ( int level );	
	int valide;
	void dump ( stringstream& ss );	
	bool local;
	static editor_c* create ();
	static unsigned long type;
	virtual unsigned long get_type ();
	void load_file_list ( string );

	int repeater_count;
	string repeater_id;
	int repeater_info;		
	int repreater_reset_condition;		
				
	reg_exp_c regexp;
	bool is_menu;

	void shift3 ( matrix_c<FT>& v );

	int search_mode;

	void info ( stringstream& ss );	
	void test ( string& s );
	void menu ( unsigned long control, unsigned long v, string c); 	
	static comtab_editor_c comtab_editor;
	int ctab ( stringstream& ss );
	
	uint64_t ui;
	memory_c* memory;
};

class clipboard_c
{
public:
	clipboard_c(){}
	void str(string s);
	string& str();
	bool system_clipboard{true};
	string clip_string;
};


#endif //EDIT_H

