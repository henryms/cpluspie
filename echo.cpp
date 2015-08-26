#include <cstdlib>
#include <cstdio>
#include <vector>
#include <list>
#include <sstream>
#include <map>
#include <iostream>
#include <cmath>

#include "debug.h"

#include "global.h"
#include "echo.h"


nout_c nout;
echo_c necho;

void nout_c::where(string file, int line)
{
	if(mode==2){
		cout << file << ":" << line << '\n';
	}
}

void echo_c::where(string file, int line)
{
	if(mode==2){
		ss2 << file << ":" << line << '\n';
	}
}

void echo_c::flush (){
	ss2.clear();
	ss2.str("");
}

echo_c & echo_c::operator<< ( Hex flag)
{
	ss2 << flag;	
	return *this;
}

echo_c& echo_c::operator << (StandardEndLine manip )
{
	ss2 << manip;
	return *this;
}

echo_c& echo_c::operator << (int& i )
{
	ss2 << i;	
	return *this;
}

echo_c& echo_c::endl ( echo_c& stream )
{
	// do other stuff with the stream
	// cout, for example, will flush the stream
	return stream;
}

echo_c& echo_c::hex ( echo_c& stream )
{
	// do other stuff with the stream
	// cout, for example, will flush the stream
	return stream;
}

echo_c& echo_c::operator << (echo_manipulator manip )
{
	// call the function, and return it's value
	return manip (*this );
}

void echo_c::clear ()
{
	ss2.clear();
	ss2.str("");
}

echo_c::echo_c ():roll ( true )
{
}

echo_c::~echo_c ()
{
}

namespace ex_echo {
void entry ( stringstream& ss )
{
}

}// end namespace exeditor

void ex_entry_echo (stringstream& ss )
{
	ex_echo::entry ( ss );

}
