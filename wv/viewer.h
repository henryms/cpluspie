#ifndef VIEWER_H
#define VIEWER_H	

class viewer_c : public object_c
{
public:
	viewer_c ();
	~viewer_c();
	void loop ();
	void draw ( uint32_t handle);
	void draw_direct_1 ( uint32_t handle);
	void convert_from_ppm( string& fppm );
	motion_3D_c<FT> view;
	string file_name;
	int columns;
	int rows;
	int window_width{100};
	int window_height{100};
	int strip{1};
	uint32_t* pi{nullptr};	
	char* impression_grid{nullptr};
	int impression_count{0};
	int advance_pixmap_pointer();	
	
	int advance_page ( int n );
	int page_numbers(int number = -1);	
	int connection;
	rstream_c ssi;
	rstream_c sso;
	bool fl;
	bool has_focus;
	map < uint32_t, surface_c > surfaces;	
	map < uint32_t, pair < motion_3D_c<FT>, matrix_c <FT> > > handle_motion;
	int hsrc;
	
};

#endif