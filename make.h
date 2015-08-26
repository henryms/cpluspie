#ifndef MAKE_H
#define MAKE_H

using namespace std;

class make_c : public object_c
{
	public:
	make_c ();
	void make ( string&, string& mfile, string& para1, string& para2);
	void mkproper ( string&, string&);
	void read_file ( string, list<string>&);
	void change_list ( list<string>&, list<string>&, string );
	string get_path ( string&);
	string get_suffix ( string&);
	string get_name ( string&);
	string get_shortname ( string&);
	string get_path_shortname ( string&);
	bool is_in_list ( string&, list<string>&);
	bool is_suffix ( string&, string );
	void scan ( string, string );	
	void work ();
	void copyright ( int nr );
	bool filter ( string&, list<string>&, list<string>&);
	bool wild_card ( string&, string&);
	bool wild_card2 ( string&, string&);
	void proper ( int part, string, string );
	void proper2 ( string );
	void remove_experimental_section ( string );
	virtual int edit ( lamb_c& lamb, keyboard_c& keyb );
	string proper_directory_name;
	int flag_thread;
	
	reg_exp_c regexp;
};


#endif
