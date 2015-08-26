#ifndef DEBUG_H
#define DEBUG_H

#ifndef DEBUG1
#include "mi_gdb.h"
#endif

class debug_c 
{
	public:
	debug_c (){};
	int f1{};
	int f2{};
	int f3{1};
	int f10{0};
	int caret_check{1};	
	int rel_a{0};
	int rel_b{0};	
	int gdb(std::string);
	std::string open_terminal(std::string name, std::string option);
	std::string open_terminal2(std::string name);
	std::string open_terminal3(std::string name);
	void close_terminal();
	int connect();
	void disconnect();
	int to_gdb[2];
	int from_gdb[2];		
	int to_terminal[2];
	FILE *to{};
	FILE *from{};	
	pid_t pid{};
	pid_t pid_terminal{};	
	void send(std::string);
	std::string receive();	
	
	
	void run();
	int runcc(std::string);
	int runc(std::string);
#ifndef DEBUG1
	MIDebugger d;
#endif
};



extern debug_c d;
#endif