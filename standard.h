#ifndef STANDARD_H
#define STANDARD_H


namespace sc11{
template<class T>
std::string to_string(const T& x)
{ 
	std::stringstream ss;
	ss << x;
//	return string();
	return std::string(ss.str());};

};





#endif
