#ifndef MACHINE_H
#define MACHINE_H


class machine_c : public message_c
{
	public:
	machine_c(){};
	~machine_c(){};
	virtual void walk(string,list<string>&){};
	virtual void load_configuration(){};
	virtual void save_configuration(){};
	virtual void grab_system_cursor(int *x, int *y){};
	virtual void ungrab_system_cursor(int x,int y){};
	virtual void change_pwd(string directory){};	
	virtual void open(){};
	
	long image_width;
	long image_height;
	int pos_x;
	int pos_y;
	void system_output(bool bopen,FILE*,std::string*,FILE**);
	virtual void get_clipboard(string&);
	virtual void set_clipboard(string);
};

#endif
