#ifndef IMAGE_H
#define IMAGE_H

class image_c: public object_c
{
	public:
	image_c ( string& file_name );
	~image_c ();
	virtual void draw ( );
	void draw_surface ( lamb_c& );
	void draw_stream ( lamb_c& );
	string is;
	int edit ( lamb_c&, keyboard_c&);
	void load ( string&);
	string file_name;
	void convert ( string );
	void convert_ppm_to_image ( string&, string&);
	void convert_ppm_to_map ( string&, int& fd, void*& map, size_t&, size_t& );	
	void convert_pdf_to_png ( string );
	void convert_png_to_ima ( string );
	int page;
	spline_c frame;
	static const unsigned long type;
	virtual unsigned long get_type ();
	ifstream fi;
	fstream foutput;
	fstream finput;
	fstream file;
	int parameter;
	int fd2;
	size_t columns;
	size_t rows;
	void* map2;
	vector < int > transformation;
	int ttest;
	int status;	
	uint64_t ui;
};

#endif 