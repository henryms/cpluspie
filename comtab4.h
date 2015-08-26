#ifndef COMTAB4_H
#define COMTAB4_H

#include <tuple>

class comtab_editor_c;
class comtab1_lamb_c;
class comtab2_lamb_c;
class finder_c
{
public:
	finder_c (){};
	bool find (comtab_editor_c&, editor_c*o,  int mode, int control, unsigned long stroke, string c, string stroke_semantic );
	bool find (comtab1_lamb_c&, lamb_c*o,  int mode, int control, unsigned long stroke, string c, string stroke_semantic );
	bool find (comtab2_lamb_c&, lamb_c*o,  int mode, int control, unsigned long stroke, string c, string stroke_semantic );

};

class parser_c
{
public:
	parser_c () {};
	void demultiplex_condition ( string& , list < string > & );
	vector < tuple < list < string >, string ,string >> lst;
	void parse ( string& s );
};


class command_base_c
{
public:
	command_base_c (){}
	virtual ~command_base_c() {}
	virtual int run(object_c* o) {}
	command_base_c( const command_base_c& x ) {}
	virtual void get_semantic ( string* semantic ) {}
	bool virtual set_parameter ( string parameters  ) {}
	void construct ( );
	
};

class comtab_c  
{
public:
	template < typename T >
	void destruct (  map < string, T*>& );
	template < typename T >
	void set ( string&, vector < T*>& , map < string, T* >& ); 
	template < typename T >
	void dump ( vector < T*>& );
	template < typename T >
	void dump ( map <string, T*>&);
};

template < typename T >
void comtab_c::dump ( vector < T*>& vec_c ) 
{
	echo << "dump vector < command_base_c*> command_base_c::vec _c" << endl;
	echo << "size : " << vec_c.size () << endl;
	for ( auto c : vec_c ) {
		string s;
		c -> get_semantic (&s );
		echo << s << endl;
	}	
}

template < typename T >
void comtab_c::dump ( map < string, T* >&msc ) 
{
	echo << "dump map < string, command_base_c*> msc" << endl;
	for ( auto c : msc ) {
		echo << c.first << endl;
	}	
}

template < typename T >
void comtab_c::destruct (  map < string, T* >& msc)
{
	vector <T* > pp;
	for ( auto p : msc ) {
		if ( p.second == 0 ) {
			continue;
		}
		bool b = true;		
		for ( auto x : pp ){
			if ( x  == p.second ) {
				b = false;
				break;
			}
		}
		if ( b ){
			pp.push_back (p.second);
			delete p.second;
		}
	}
	msc.clear();
}

template < typename T >
void comtab_c::set ( string& ct, vector < T* > &vec_c, map < string, T * >& msc)
{
	parser_c parser;
	parser.parse ( ct );
	bool good = 0;
	T* pc = 0;
	for ( auto tu : parser.lst ) {
		pc = 0;
		for ( auto p : vec_c ) {
			string sem;
			p -> get_semantic (&sem); 
//			echo << "semantic: " << sem << endl;
			if ( sem == get < 1 > ( tu ) ){
				pc = p ->create ();
				break;
			}
		} 
		if ( pc != 0 ) {
			for ( auto k : get < 0 > ( tu ) ) {
//				echo << "key: " << k << "  para: " << get < 2 > ( tu ) << endl;
				good = pc-> set_parameter ( get < 2 > ( tu ) );
				if ( good ) {
//					echo << "command table good\n";
					msc [ k ] = pc; 
				}
				else {
					echo << "parameter fault\n";
					delete  pc;
				}
			}
		}
	}
} 

template < typename T >
class push_vector_c
{
public:
	void operator << ( T* x ) { lst.push_back ( x ); return;} 
	vector < T* > lst;
	operator  vector < T* >&  () { return lst; }
	~push_vector_c () { for ( auto x : lst ) delete x;}
};

#endif