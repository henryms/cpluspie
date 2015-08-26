#ifndef SPLINE_H
#define SPLINE_H

class spline_c : public object_c
{
	public:
	spline_c ();
	spline_c ( const spline_c& ); 
	spline_c ( spline_c&);
	spline_c ( int left, int right, int bottom, int top, bool fill );
	~spline_c ();
	virtual object_c* duplicate ();
	matrix_c<FT> cubic_B_spline ( float, matrix_c<FT>& P0, matrix_c<FT>& P1, matrix_c<FT>& P2, matrix_c<FT>& P3 );
	float basis_B_spline ( float u, int i, int k, vector<float>& t );
	bool B_spline ( float u, vector<float>& t, int k, vector<matrix_c<FT>>& p, matrix_c<FT>& s );
	
	virtual void draw ();
	virtual void draw_B_spline ( lamb_c& lamb );
	vector < matrix_c < FT > > control_points;
	vector < float > knots;
	
	list < line_c > segments;
	vector < line_c > controls;
	
	int order;
	bool open_uniform;
	bool uniform;
	bool close;
	uint32_t color;
	bool doted;
	int active_point;
	virtual int command2 ( unsigned short, string&, string&);
	int edit ( lamb_c& lamb, keyboard_c& keyb );
	int set_point ( lamb_c&);
	void shift3 ( matrix_c<FT>&);
	virtual void shift ( matrix_c<FT>&);
	virtual void serialize ( fstream &file, bool bsave );
	virtual bool find ( lamb_c &lamb, matrix_c<FT> pt );
	static spline_c* create ();
	static const unsigned long type;
	virtual unsigned long get_type ();
	bool accept_points;
	void dump ();
	bool transcient;
};

#endif