#ifndef	CARTESIAN_H
#define 	CARTESIAN_H

class cartesian_c: public object_c
{
	public:
	cartesian_c ();
	void draw ( lamb_c& );
	void draw ( );
	virtual int edit ( lamb_c&, keyboard_c& );
	void serialize ( fstream &file, bool bsave );
	virtual void object_to_local ( object_c* o, matrix_c < FT >& lbase, matrix_c < FT >& lvA );
	virtual void coocked_to_local ( object_c* o, matrix_c < FT > & lbase, matrix_c <FT >& lvA, int& flag);
	static cartesian_c* create ( );
	void locals_to_world ( vector < matrix_c < FT > >& vectors);
	void dump ();
	matrix_c<FT>  vA;
	matrix_c < FT > vvx;
	matrix_c < FT > vvy;
	matrix_c < FT > vvz;
	bool right_hand;
	int dimension;
	spline_c x;
	spline_c y;
	spline_c z;
	editor_c tx;
	editor_c ty;
	editor_c tz;
	spline_c g;
	virtual unsigned long get_type ();
	static const int type;
};

#endif