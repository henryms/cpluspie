#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <fstream>
#include <string>

#include "gate.h"

using namespace std;

string 
	entity{"lamb"},  //main program
	entityb, // the backup for recovery
	entityo, // the build output
	platform,
	np_path,
	bin_path,
	libsens{"sens/libsens.so"},
	libsensb, 
	libsenso, 
	liblayen{"wr/liblayen.so"},
	liblayenb, 
	liblayeno, 

	exit_cfg,
	make,
	exe;


int main (int argc, char* arg[])
{
	char* pe;
	pe=getenv ("LL_PATH");
	if (pe==0){
		cout << "environment variable LL_PATH not set\n LL_PATH must be set to np path" << endl;
		return 0;
	}


	system("clear");	
	np_path  = pe;
	
	string s;
	string a0,a1,a2,a3;
	if (argc>=2){
		a1=arg[1];
		if ( argc >=3 ) {
			a2 = arg [2];
		}
		if (argc >=4){
			a3 = arg [3];
		}
	}
	if (0){
		cout << "gate:: argc: " << argc << '\n';
		for (int c = 0;  c < argc ; ++c)	
			cout <<  arg[c] << '\n';	
		cout << "....\n";	
	}
	if (a1== "-s"){
		if (argc < 3 ){
			return 0;
		}
		ofstream f (np_path+ "/config/environment");
		a2=arg[2];
		if (a2 == "linux"){
			f << "linux" << endl;
			f << "build/linux" << endl;
		}
		else if (a2=="x11"){
			f << "x11" << endl;
			f << "build/x11" << endl;
		}
		else if (a2=="wayl"){
			f << "wayl\n" 
			<< "build/wayl\n";
		}
		return 0;
	}
	ifstream f (np_path+"/config/environment");
	if (!f.good ()){
		cout << "error: no environment file\n ./np -s <>, with <> linux, wayl or  x11" << endl;
		return 0;
	}
	f >> platform;
	f >> bin_path;
	f.close ();
	
	s = np_path+"/"+bin_path + "/";
	entity = s + entity;
	entityo = entity + "_out"; 
	entityb = entity + "_save";
	
	libsens = s + libsens;
	libsenso = libsens + "_out";
	libsensb = libsens + "_save";
	
	liblayen = s + liblayen;
	liblayeno = liblayen + "_out";
	liblayenb = liblayen + "_save";
	
	exit_cfg=np_path + "/config/exitcfg";
	int result;	
	if (a1=="-r"){
		ofstream f (exit_cfg);
		f << wcp::restore << endl;
		f.close ();
	}
	else if (a1=="-save"){
		s="cp " + libsens + " " + libsensb;
		system (s.c_str ());
		s="cp " + liblayen + " " + liblayenb;
		system (s.c_str ());
		s="cp " + entity + " " + entityb;
		system (s.c_str ());
		return 0;
	}
	else {	
		ofstream f (exit_cfg);
		f << wcp::start << endl;
		f.close ();
	}
	while ( true ) {
		ifstream fi ( exit_cfg );
		if ( fi.fail () ) {
			break;
		}
		fi >> result;
		fi.close ();
		ofstream fo ( exit_cfg );
		fo << wcp::quit;
		fo.close ();
		fi.open ( exit_cfg );
		int val;
		fi >> val;
		cout << "val:" << val << endl;
		fi.close ();
//		fstream f ( exit_cfg, fstream::in | fstream::out );
				
		//restore
		if ( result==wcp::restore ){   
			s="cp " + libsensb + " " + libsens;
			system (s.c_str ());
			s="cp " + liblayenb + " " + liblayen;
			system (s.c_str ());
			s="cp " + entityb + " " + entity;
			system (s.c_str ());
			result=system (entity.c_str ());
			continue;
		}
		//quit
		if (result==wcp::quit){
			break;
		}
		//save
		if (result==wcp::save){
			s = "cp " + entity + " " + entityb;	
			system (s.c_str ());
			s = entity;
			result = system (s.c_str ());
			continue;
			
		}
		//start
		if (result==wcp::start){
			s = entity + " " + a1 + " " + a2 + " " + a3;
			cout << "entity::" << s << endl;
			system (s.c_str ());
			
			continue;
		}
		//restart
		if (result==wcp::restart){
			system("clear");	
			s="cp "+ libsenso + " " + libsens;
			system (s.c_str ());
			s="cp "+ liblayeno + " " + liblayen;
			system (s.c_str ());
			s="cp "+ entityo + " " + entity;
			system (s.c_str ());
			system (entity.c_str ());
			continue;
		}
		break;
	}	
	cout << "launcher exit"<< endl;
	return 0;
}
