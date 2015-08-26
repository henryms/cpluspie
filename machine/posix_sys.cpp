#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <list>
#include <vector>
#include <string>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <fts.h>
#include <sys/mman.h>

#include "library/wdslib.h"
#include "gate.h"
#include "matrix.h"
#include "position.h"
#include "mstream.h"
#include "sens/retina.h"
#include "sens/spirit.h"
#include "message.h"
#include "object.h"
#include "posix_sys.h"
#include "line.h"
#include "spline.h"
#include "global.h"

using namespace std;

int posix_sys_c::test ( stringstream& ss ) 
{
}

void posix_sys_c::get_clipboard(string&)
{
}

void posix_sys_c::set_clipboard(string)
{
}

bool posix_sys_c::memory_map2 ( string name, size_t ms, char*& pointer ) 
{
	mkdir("/tmp/wshm",0777);
	chmod("/tmp/wshm",0777);
//	string path = "/home/me/wshm/" + name;
	string path = "/tmp/wshm/" + name;
	int fd = ::open ( path.c_str (), O_RDWR);
	if ( fd == -1 ) {
		if ( ms == 0 ) {
			cout << "no file with this name" << endl;
			pointer = 0;
			return false;
		}
		fd = ::open ( path.c_str (), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
		if ( fd == -1 ) {
			cout << "file error: " << path << endl;
			pointer = 0;
			return false;
		}
	}
	if ( ms != 0 ){
		lseek ( fd, ms -1, SEEK_SET ); 
		write ( fd, "", 1 );
	}
	else {
		ms = lseek ( fd, 0, SEEK_END);
		cout << "file size: " << ms << endl;
	}
	void* p = (char*)mmap ( 0, ms, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 );
	if ( p == MAP_FAILED ) {
		cout << "mmap failed\n";		
		return false;
	}
	else {
		cout << "mmap ok\n";
	}
	pointer = (char*) p;
	return true;
	
	
	/*
	fd = shm_open ( name.c_str (), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
	if ( fd == -1 ) {
		cout << "shm_open fail" << endl;
		return;
	}
	else {
		cout << "shm_open ok" << endl;
	}
	pointer = mmap ( 0, ms, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 );
	if ( MAP_FAILED == pointer ) {
		cout << "mmap fail" << endl;
	}
	else {
		cout << "mmap ok" << endl;
	}	
	*/
}

void posix_sys_c::memory_map ( string name, size_t ms, void*& pointer, int* ref ) 
{
	string path = "/home/henry/mm/" + name;
	int fd = ::open ( path.c_str (), O_RDWR);
	if ( fd == -1 ) {
		cout << "xx1" << endl;
	}
	else {
		cout << "xx2" << endl;
	}
	if ( fd == -1 ) {
		if ( ms == 0 ) {
			cout << "errr" << endl;
			pointer = 0;
			return;
		}
		fd = ::open ( path.c_str (), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
		if ( fd == -1 ) {
			cout << "name: " << path << endl;
			pointer = 0;
			return;
		}
	}
	if ( ms != 0 ){
		write ( fd, &ms, 4 );
		lseek ( fd, ms -1, SEEK_SET ); 
		write ( fd, "", 1 );
	}
	else {
		read ( fd, &ms, 4 );
	}
	pointer = mmap ( 0, ms, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 );
	cout << "pointer: " << pointer << endl;
}

void posix_sys_c::unmap_memory_map ( void* adress )
{
	uint32_t length =  * static_cast < uint32_t* > ( adress );  
	munmap (adress, length); 
}

char* posix_sys_c::map_file (string& file_name )
{
	int fd = ::open ( file_name.c_str (), O_RDWR );
	if ( fd == -1 ) {
		return nullptr;
	}
	uint32_t size;
	int result = read ( fd, (char*)&size, 4 );
	if ( result < 4 ) {
		::close ( fd );
		return nullptr;
	}
	void * pointer = mmap ( 0, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 );
	if ( pointer ==  MAP_FAILED ) {
		return nullptr;
	}
}

void posix_sys_c::file_service ( stringstream& ss )
{
}

void posix_sys_c::get_cwd ( string& directory )
{
	char result [ 1024 ];
	getcwd ( result , sizeof ( result ) );
	directory = string ( result ) ;	
}

void posix_sys_c::change_pwd ( string s )
{
	chdir ( s.c_str ());	
}

void posix_sys_c::walk2 ( string path, list < pair< int ,string>>& l ) 
{
	cout << "machine walk: " <<  path << endl;
	if ( path.find ( "/home/henry/") == string::npos ) {
		cout << "walk2 blocked\n";
		return;
	}
	char* dl[]={(char*)path.c_str (), 0};
	FTS* fts = fts_open ( dl, FTS_PHYSICAL, 0 );
	FTSENT* ftsent;
	while ( ftsent = fts_read ( fts )){
		int fty = 0;
		if ( ftsent -> fts_info ==  FTS_F ) {
			fty = 1;
		} 
		else if ( ftsent -> fts_info == FTS_D ) {
			fty = 2;
		}
		if ( fty != 0 ){
			l.push_back (make_pair( fty,  ftsent->fts_path)  );
		}
	}
	fts_close ( fts );
	return;
	
}

void posix_sys_c::walk ( string path, list<string>& l )
{
	cout << "machine walk" << endl;
	char* dl[]={(char*)path.c_str (), 0};
	FTS* fts = fts_open ( dl, FTS_PHYSICAL, 0 );
	FTSENT* ftsent;
	while ( ftsent = fts_read ( fts )){
		string s = ftsent->fts_name;
		if ( true ){
			l.push_back ( ftsent->fts_path );
		}
	}
	fts_close ( fts );
	return;
}

int posix_sys_c::system_echo ( string& com , stringstream& _echo )
{
	string path = get_lambs_path ();
	string fn_err = path + "/tmp/landerr";
	string fn_out = path + "/tmp/landout";
	
	ofstream f ( fn_err );
	f.close ();
	f.open ( fn_out );
	f.close ();
	
	string redirect;
	redirect = " 1>" + fn_out  + " 2>" + fn_err;

	string s = com + redirect;
	system ( s.c_str () );
	
	ifstream f_e ( fn_err );
	while ( f_e.good () ) {
		char c = f_e.get ();
		if ( ! f_e.eof () ){
			_echo << c;
		}
	}
	ifstream f_o ( fn_out );
	while ( f_o.good () ) {
		char c = f_o.get ();
		if ( ! f_o.eof () ){
			_echo << c;
		}
	}
	return 0;
}

int posix_sys_c::exec_async( vector<string>& com, vector<string>& env){
	const char* newarg[100];
	const char* newenv[100];
	int c;
	for(c=0; c<com.size();++c)
		newarg[c]=com[c].c_str();
	newarg[c]=nullptr;
	for(c=0; c<env.size();++c)
		newenv[c]=env[c].c_str();
	newenv[c]=nullptr;
	pid_t pid = fork ();
	if (pid == 0) {
		execve ((char*)newarg [0], (char**)newarg, (char**)newenv);
	}
	return 0;
}

int posix_sys_c::system_async_run(string& com, string& env)
{
	const char* newarg[10]{};
	stringstream ss{com};
	vector<string> sts;
	string s;
	for (int i = 0; ( ss >> s ) && i < 10; i++){
		sts.push_back(s);
		newarg[i] = sts.back().c_str();
	}
	const char* newenv[]{env.c_str(), nullptr};
	pid_t pid = fork ();
	if (pid == 0) {
		execve ((char*)newarg [0], (char**)newarg, (char**)newenv);
	}
	return 0;
}

int posix_sys_c::system_pipe(string& com)
{
}

//expsecb
/*
void posix_sys_c::system_output(bool bopen,FILE* stdf,std::string* path,FILE** pfile)
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
*/
//expsece
