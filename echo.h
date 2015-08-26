#ifndef ECHO_H
#define ECHO_H

#include "debug.h"
using namespace std;

class nout_c
{
	public:
	int mode{0};	
	template <typename T>
	nout_c& operator<<(const T& x );
	void where(string, int);	
};

#define mout nout.where(__FILE__, __LINE__), nout 

extern nout_c nout;

template <typename T>
nout_c& nout_c::operator<<(const T& x )
{
	if(mode == 0){
	}
	else {
		cout << x;
	}
	return *this;		
}

class echo_c 
{
	public:
	echo_c ();
	~echo_c ();
	stringstream ss2{};
	int mode{1};	
	template <typename T>
	echo_c& operator<<(const T& x );
	echo_c& operator<<(int& i );
	void where(string, int);	
	
	// function that takes a custom stream, and returns it
	typedef echo_c& (*echo_manipulator )(echo_c&);

	// take in a function with the custom signature
	echo_c& operator<<(echo_manipulator manip );
	
	// define the custom endl for this stream.
	// note how it matches the `echo_manipulator`
	// function signature
	static echo_c& endl ( echo_c& stream );
	static echo_c& hex(echo_c& stream );
	
	// this is the type of std::cout
	typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

	// this is the function signature of std::endl
	typedef CoutType& (*StandardEndLine )(CoutType&);
	

	// define an operator<< to take in std::endl
	echo_c& operator<<(StandardEndLine manip );
	
	//my
	typedef ios_base& (* Hex ) (ios_base&);
	echo_c & operator<< ( Hex flag);
	
	virtual void clear ();
	void flush ();
	bool roll;
	int signo{};
};

template <typename T>
echo_c& echo_c::operator<<(const T& x )
{
	if(mode!=0){
		ss2 << x;
	}
	return *this;		
}

#define echo necho.where(__FILE__, __LINE__), necho 
extern echo_c necho;

#endif


