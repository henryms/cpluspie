#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include "mstream.h"

tstream_c& hex( tstream_c& stream )
{ cout << "in hex" << endl; stream.number_system = 16; };

tstream_c& dec ( tstream_c& stream )
{ stream.number_system = 10;};

bool tstream_c::access_request ()
{
	uint32_t mu;
	mu =  *(uint32_t*)psm;
	if ( mu != 0 ) {
		return false;
	}
	* ( uint32_t*) psm = 1;
}

void tstream_c::access_release ()
{
	* ( uint32_t* ) psm = 0;
}

tstream_c::tstream_c(): number_system ( 10 )
{
}

tstream_c& tstream_c::operator<< (int x )
{
	stringstream ss;
	switch ( number_system ) {
		case 10: ss << dec; break;
		case 16: ss << hex;break;
	}
	number_system = 0;
	ss << x;
	strcpy ( pp, ss.str().c_str() );
	pp+= sizeof ( ss.str ().c_str() );
}

tstream_c& tstream_c::operator<< (float fl )
{
}

tstream_c& tstream_c::operator>> (string& s )
{
	string s1;
	for (; ; pg++) {
		if ( *pg != ' ' && *pg != '\n' ) 
			break;				
	}
	for (;;pg++ ) {
		if ( *pg != ' ' && *pg != '\n' )
			s1 += *pg;
		else 
			break;		
	}
//	string s0 { pg };
	stringstream ss { s1 };
	cout << "num sys: "<< number_system << endl;
	switch ( number_system ) {
		case 10: ss >> dec; break;
		case 16: ss >> hex;break;
	}
	number_system =0 ;
	ss >> s;
//	pg += s.size ();
}


void mstream_c::set_mm ( void* p, size_t size ) 
{
	char* pc = static_cast < char*> ( p );
	auto bf = rdbuf();
}

mstream_c::mstream_c()
{
}
