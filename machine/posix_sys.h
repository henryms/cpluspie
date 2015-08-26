#ifndef POSIX_SYS_H
#define POSIX_SYS_H


class posix_sys_c : public message_c
{
	public:
	posix_sys_c(){};
	~posix_sys_c(){};
	virtual void walk(string,list<string>&);
	virtual void walk2(string,list<pair< int, string>>&);
	virtual void load_configuration(){};
	virtual void save_configuration(){};
	virtual void grab_system_cursor(int *x, int *y){};
	virtual void ungrab_system_cursor(int x,int y){};
	virtual void change_pwd(string directory);
	void get_cwd ( string& directory );
	virtual void open(){};
	long image_width;
	long image_height;
	int pos_x;
	int pos_y;
	int system_echo (string& com, stringstream& _echo);
	int exec_async( vector<string>& command, vector<string>& env);
	int system_async_run (string& com, string& env);
	int system_pipe(string& com);
	virtual void get_clipboard(string&);
	virtual void set_clipboard(string);
	virtual void file_service(stringstream& ss);
	virtual void memory_map(string name, size_t ms, void*& pointer, int* ref ); 
	virtual bool memory_map2(string name, size_t ms, char*& pointer); 
	virtual void unmap_memory_map(void* adress );
	virtual char* map_file(string& file_name);
	virtual int test(stringstream& ss);
};

#endif
