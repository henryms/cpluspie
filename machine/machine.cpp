#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <iostream>

#include <string>

#include "symbol/keysym.h"
#include "../gate.h"
#include "../matrix.h"
#include "../position.h"
#include "../library/wdslib.h"
#include "mstream.h"
#include "../sens/retina.h"
#include "../message.h"
#include "posix_sys.h"
#include "socket.h"

#include "../global.h"

#include "../message.h"
#include "machine.h"

using namespace std;

void machine_c::get_clipboard(string&)
{
	
}

void machine_c::set_clipboard(string)
{
	
}

void machine_c::system_output(bool bopen,FILE* stdf,std::string* path,FILE** pfile)
{
	FILE* file;
	static int fnstdout=0;
	static int fnstderr=0;
	int* fn;
	if(stdf==stdout){
		fn=&fnstdout;
	}
	else if(stdf==stderr){
		fn=&fnstderr;
	}
	else{
		;
		//return;
	}
	if(bopen){
		if(*fn!=0){
			return ;
		}
		cout << "stdf," << endl;
		string name=*path;

		*fn=dup(fileno(stdf));
		FILE* file=fopen(name.c_str(),"w");
		dup2(fileno(file),fileno(stdf));
		close(fileno(file));
	}
	else{
		fflush(stdf);
		dup2(*fn,fileno(stdf));
		close(*fn);
		*fn=0;
		//clearerr(stdf);
		//fsetpos(stdf,&posout);
		cout << "stdf back again" << endl;
	}
	return ;

}
