#ifndef MSTREAM_H
#define MSTREAM_H

using namespace std;
#include <cstring>
#include <sstream>
class tstream_c;

tstream_c& hex( tstream_c& stream );
tstream_c& dec( tstream_c& stream );

class rstream_c
{
public:
	rstream_c(): access ( false ) {};
	rstream_c( char* p){psm = p; pg = pp = p + 8; access= false;}
	rstream_c (const rstream_c& t ) { psm = t.psm; pg = t.pg; pp = t.pp; access = t.access; }
	char* psm;
	char* pg;
	char* pp;
	rstream_c & operator << (int x ) { *(int*)pp = x; pp += sizeof (int ); }
	rstream_c & operator << (uint32_t x ) { *(uint32_t*)pp = x; pp += 4; }
	rstream_c & operator << (long x ) { *(long*)pp = x; pp += sizeof ( long ); }
	rstream_c & operator << (float x) { *(float*)pp = x; pp += sizeof (float ); }
	rstream_c & operator << (string s ) { strcpy ( pp, s.c_str() ); pp += s.size() + 1;  }
	rstream_c & operator << (const char*	pc ) { strcpy ( pp, pc ); pp += strlen (pp) + 1;  }
	rstream_c & operator << (unsigned char c) { *(unsigned char*) pp = c; pp++ ;}
	rstream_c & operator >> (long& x ) { x = *(long*)pg; pg += sizeof (long);}
	rstream_c & operator >> (int& x ) { x = *(int*)pg; pg += sizeof (int);}
	rstream_c & operator >> (uint32_t& x ) { x = *(uint32_t*)pg; pg += 4;}
	rstream_c & operator >> (float& x ) { x = *(float*)pg; pg += sizeof (float);}
	rstream_c & operator >> (string& s ) { s  = pg; pg += s.size () + 1;}
	rstream_c & operator >> (unsigned char& c) { c = *(unsigned char*) pg; pg++ ;}

	void seekp ( size_t pos ) { pp = psm + pos + 8; }
	void seekg ( size_t pos ) { pg = psm + pos + 8; }
	size_t tellp ( ) { return pp - psm - 8;}
	size_t tellg () { return pg - psm  - 8 ;}
	bool access;
	bool access_request () { 
		if ( *(uint8_t*)(psm + 4) == 1 ) {
			access = false;
			return false;
		}
		access = true;
		*(uint8_t*)(psm + 4 ) = 1;
		pg = pp = psm + 8;		
		return true;
	}
//	void access_release () { if ( access ) { *(uint8_t*)(psm+4) =  0; access = false; }  }
	void access_release () { *(uint8_t*)(psm+4) =  0; }
};

class tstream_c 
{
public:
	tstream_c ();
	tstream_c (const tstream_c& t ) { psm = t.psm; pg = t.pg; pp = t.pp; }
	tstream_c ( char* pm ){ psm = pg = pp = pm + 4;}
	char* psm;
	char* pg;
	char* pp;
	int number_system;
	uint32_t mutex;
	bool access_request ();
	void access_release ();
	
	tstream_c & operator<< (int x );
	tstream_c & operator<< (float );
	tstream_c & operator >> ( string& s);
	void seekp ( size_t pos ) { pp = psm + pos + 4; }
	void seekg ( size_t pos ) { pg = psm + pos + 4; }
	size_t tellp ( ) { return pp - psm - 4;}
	size_t tellg () { return pp - psm  - 4 ;}
	
	typedef tstream_c& (*tstream_manipulator )(tstream_c&);
	tstream_c& operator<<(tstream_manipulator manip ){manip ( *this); }
	tstream_c& operator>>(tstream_manipulator manip ) {manip ( *this);}
	
};

class mstream_c : public stringstream
{
public:
	mstream_c ();
	void set_mm(void* p, size_t );
};

#endif