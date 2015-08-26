#ifndef EYES_H
#define EYES_H

class eyes_c : public object_c
{
public:
	eyes_c ();
	eyes_c ( int, int );
	~eyes_c ();
	void draw ( lamb_c&);
	void draw ();
	int edit ( lamb_c&, keyboard_c&);
	bool set_superficial_pixel ( int x, int y, uint32_t color );
	bool set_pixel ( object_c*, int x, int y, float z, long color, bool b = true );
	long get_pixel ( int x, int y );
	void set_ground ( long color );
	void set_superface ( uint32_t color );
	void set_size ( lamb_c&, int x, int y );
	void push_surface ( lamb_c&);
	void change_surface ( lamb_c&, int );
	
	int width;
	int height;
	int frame_width;
	int frame_height;
	int frame_x;
	int frame_y;
	spline_c focus;
	spline_c border;
	float dpm;
	float zoom;
	
	retina_c retina ;
		
	void translate ( matrix_c<FT> A, matrix_c<FT> x, matrix_c<FT> y, matrix_c<FT> z, matrix_c<FT> vt );
	void parallelogram ( matrix_c<FT> A, matrix_c<FT> x, matrix_c<FT> y, bool frame, bool erase, unsigned long color = 0x00 );
	void grip ( matrix_c<FT> vA, unsigned long color = 0x00 );	
	bool valide;
	bool off{false};
};
#endif 
