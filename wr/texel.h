#ifndef TEXEL_H
#define TEXEL_H

#include "debug.h"

#include "font.h"
class shared_c;
class texel_c
{
public:
	virtual void construct (){}
	virtual void deconstruct() {}
	virtual void draw ( layen_c&)  {}
	virtual uint32_t get_char (){return 0;}
	virtual void dump ( stringstream& ss){}
	texel_c ();
	virtual ~texel_c ();
	virtual bool is_caret () { return false;}
	virtual bool is_glyph () { return false;}
	virtual bool is_new_line() { return false;}
	virtual bool is_tab() { return false;}
	virtual bool is_printable() { return false; }
	virtual bool is_text () { return false; }
	virtual bool is_char () { return false; }
	virtual bool is_selector () { return false; }
	virtual bool is_collapse () { return false; }
	virtual bool is_row () { return false; }
	virtual bool is_word_part () { return false; }
	virtual bool is_punct () { return false; }
	virtual bool is_class ( int ) { return false; }
	virtual int advance_x () { return adv_x; }
	virtual int advance_y () { return adv_y; }
	virtual void clear_pixel_vectors ();
	virtual void clear_glyph_bitmap ();
	graphies_c* graphies{};
	int adv_x{};
	int adv_y{};
	int reference{1};
	virtual int dec_reference(){return --reference;}
};

class texel_caret_c;

class caret_representation_c
{
public:
	texel_caret_c& caret;
	caret_representation_c(texel_caret_c& _caret):caret(_caret){};
	void set(bool force = false);
	bool check();
	bool valid{false};	
	vector<texel_c*>::iterator iterator;
	size_t line{0};
	int scroll_up{0};
	size_t selector_line{0};
	size_t selector_position_in_line{0};
	size_t position_in_line{0};
	size_t position{0};
	size_t column{0};
	size_t row_size{0};
	size_t row_position{0};
	size_t line_position{0};
	size_t row{0};
	size_t selector_position{0};
	void dump(stringstream& ss);
	void dump();
};

class row_representation_c
{
public:	
	texel_caret_c& caret;
	row_representation_c(texel_caret_c& _caret):caret(_caret){};
	void set(size_t);
	size_t row{0};
	size_t position{0};
	size_t row_size{0};
	vector<texel_c*>::iterator iterator;
};

class texel_caret_c:public texel_c
{
public:
	texel_caret_c ();
	~texel_caret_c();
	virtual void construct();
	virtual void draw(layen_c& layen);
	int advance_x () ;
	int advance_y () ;
	layen_c* layen;
	void inc_line();
	void dec_line();
	void bind_selector();
	void unbind_selector();
	void swap_selector();
	bool is_selector_binded();
	int get_leading_tabs();
	void next_word_end();
	void next_word();
	void previous_word();
	void to_line_end();
	void to_last_character();
	void to_line_begin();
	void to_file_begin();
	void to_file_end();	
	void set_to(size_t pos, int scroll_down);
	void move_to_line(size_t line, size_t position_in_line);
	size_t move_to2 ( size_t index );
	size_t move_to ( size_t index );
	int scroll_up(int d);
	void move_from_to(vector<texel_c*>::iterator , int to);
	void to_right(int);
	void to_left(int);
	void to_row_up(int);
	void to_row_down(int);
	int line_length();	
	void right(vector<texel_c*>::iterator);
	void left(vector<texel_c*>::iterator);
	size_t move_selector_to(size_t index);
	size_t offset(int delta);
	size_t size();
	vector<texel_c*>::iterator iterator(int pos = 0);
	size_t position_in_line();
	size_t line();
	size_t row();
	int scroll_up();
	size_t position();
	size_t get_position ();
	size_t get_selector_position ();
	size_t get_position(std::vector<texel_c*>::iterator);
	size_t get_last_row(size_t till = 0);
	size_t get_last_line();
	size_t get_last_position();
	size_t get_row(int, size_t* pos, size_t* size);	
	size_t get_line_position(int line = 0);
	size_t get_line(size_t* pos, size_t* size);
	size_t get_line_from_position(size_t position, size_t* offset = nullptr) ;
	bool is_line_end();
	bool is_eof();
	void move_to_next_of_sort(string s);
	vector<texel_c*>* texels;
	size_t column_memory;	

	bool in_selector_mode;
	
	caret_representation_c info;
	row_representation_c row_info;
	
	void set_column_memory ();
	
	void dump (stringstream& ss );
	bool is_caret (){ return true;}
	bool is_selector() { return true; };
	vector<texel_c*>::iterator selector(bool* leading = nullptr);
	bool trailing_caret{};
};

class texel_collapse_c: public texel_c
{
public:
	texel_collapse_c ();
	bool is_collapse ();
};

class texel_selector_c: public texel_c
{
public:
	texel_selector_c ();
	bool is_selector () { return true; }
	void dump(stringstream& );
};

class texel_tab_c: public texel_c
{
public:
	texel_tab_c ();
	~texel_tab_c();
	virtual uint32_t get_char (){return '\t';}
	int size{40};
	bool is_tab (){ return true;}
	bool is_text() { return true; }
	void dump(stringstream& );
	void draw (layen_c& layen);
	void construct();
	int advance_x () ;
	int advance_y () ;
};

class texel_newline_c: public texel_c
{
public:
	texel_newline_c ();
	~texel_newline_c ();
	virtual uint32_t get_char (){return '\n';}
	bool is_new_line (){ return true;}
	bool is_text (){ return true;}
	void draw (layen_c& layen);
	void construct();
	void dump (stringstream& ss );
};

class texel_row_c: public texel_c
{
public:
	texel_row_c ();
	texel_row_c(size_t size, shared_c& shared);
	~texel_row_c();
	bool is_row () { return true; }
	void dump ( stringstream& ss );
	bool last_row;
	bool is_class ( int symbol ) { return symbol == class_symbol;}
	int class_symbol;
	virtual void draw (layen_c&);
	virtual void construct();
};

class texel_color_c: public texel_c
{
public:
	texel_color_c ();
	int count;	
	uint32_t color;
};

class texel_char_c: public texel_c
{
public:
	static texel_char_c* create ( uint32_t, FT_Face, size_t, shared_c&);
	texel_char_c ( unsigned int );
	~texel_char_c ();
	virtual void draw(layen_c&);
	bool is_char () { return true; }
	bool is_text () { return true; }
	bool is_word_part ();
	bool is_punct ();
	void dump ( stringstream& ss );
	virtual uint32_t get_char (){return character;}
	virtual void construct ();
	uint32_t character;
	int advance_x () ;
	int advance_y () ;
};

#endif \\TEXEL_H
