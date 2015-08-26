#ifndef LINE_H
#define LINE_H

class lamb_c;
class object_c;

using namespace std;

class line_c: public object_c
{
	public:
	line_c ();
	line_c ( const line_c&);
	line_c ( matrix_c<FT>& );
	line_c ( matrix_c<FT>& vA, 	matrix_c<FT>& vb, FT r, 
		uint32_t color_ = 0x00, int doted_ = false );
	virtual ~line_c ();
	matrix_c<FT> vA;
	matrix_c<FT> vb;
	matrix_c<FT> vx;

	matrix_c<FT> object_to_world ( matrix_c<FT>&);
	matrix_c<FT> world_to_object ( matrix_c<FT>&);	
	long color;
	int doted;
	void copy ( line_c&);
	virtual void serialize ( fstream &file, bool bsave );
	virtual bool find ( lamb_c &lamb, matrix_c<FT> pt );
	virtual void draw ( );
	virtual void draw_transcient ( lamb_c& lamb );
	void setpoint ( lamb_c& lamb, bool clear, bool _3points, bool automatic, bool create );
	bool seek ( matrix_c<FT> pt0, matrix_c<FT> pt1, matrix_c<FT> pt2, long h );
	virtual void edit ( lamb_c& lamb, message& mes );
	bool transcient;
	
	uint64_t ui;	
	
	static line_c* create ();
	static const unsigned long type;
	virtual unsigned long get_type ();
};

#endif
