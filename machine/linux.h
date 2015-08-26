#ifndef LINUX_H
#define LINUX_H

class linux_c: public posix_sys_c
{
	public:
	linux_c ();
	
	void open ();
	void load_configuration ();
	void save_configuration ();
	int create_window ();
	void destroy_window ();
	bool is_idle ();
	void expose_image ();
	void* fb;
	struct fb_fix_screeninfo FixInfo;
	struct fb_var_screeninfo VarInfo;
	int m_FBFD;
	int fb_size;
	void output ( event_s&);
	void exit ( int );
	bool download ( string&, stringstream&);
	uint32_t bits_per_pixel;

	virtual void get_cwd ( string& directory );
	void window_management ( string cmd );
	string restart_cfg;
	bool full_screen;
};

#endif
