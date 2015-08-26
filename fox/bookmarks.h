#ifndef BOOKMARKS_H
#define BOOKMARKS_H

using namespace std;

class editor_c;
class mark_c 
{
	public:
	mark_c();
	mark_c ( const mark_c& mark );
	mark_c ( editor_c& edit );
	void serialize ( fstream & file, bool bsave );
	void go_to_mark ( editor_c& edit );
	string name;
	string pattern;
	size_t position;
	int scroll;
	int scroll_up{0};
	size_t line;
	size_t position_in_line;
	string tag;
	string info;
	int status;
	size_t get_position ();
	string get_name ();
};

class marks_c
{
	public:
	marks_c();
	list < mark_c > l;
	void serialize ( string file_name, bool bsave );
	void serialize ( fstream &file, bool bsave );
	virtual void push_mark ( editor_c& edit, mark_c, bool toggle = false );
	void mark ( editor_c&, mark_c mark, int index, string tag);
	virtual void to_next_mark ( editor_c& edit, int mode );
	void show ( editor_c& edit );
	list < mark_c >::iterator find_name ( string& name );
	list < mark_c >::iterator find_tag ( string& tag );
	list < mark_c >::iterator find_name_and_position ( string& name, size_t position );
};

class shortmarks_c : public marks_c
{
public:
	void to_next_mark ( editor_c& edit, int mode ); 
};

class automarks_c : public marks_c
{
public:
	void push_mark ( editor_c& edit, mark_c, bool toggle = false );
};

class bookmarks_c
{
	public:
	bookmarks_c();
	bool look_in_cash(string&,string&);
	void add_to_cash(string&,stringstream&);
};

#endif