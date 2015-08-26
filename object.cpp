#include <stdio.h>
#include <cmath>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <sstream>

#include "mstream.h"
#include "gate.h"
#include "matrix.h"
#include "position.h"
#include "sens/retina.h"
#include "sens/spirit.h"
#include "event.h"
#include "message.h"

#include "object.h"

using namespace std;

object_c::object_c (): 
object_id ( 0 )
{
	state = 0;
	parent = 0;
}

object_c::~object_c ()
{
//	cout << "object destruction\n";
}

void object_c::shift ( matrix_c<FT>&)
{
}

object_c* object_c::duplicate ()
{
	return 0;
}

void object_c::quit ()
{
}

void object_c::object_to_local ( object_c* o, matrix_c < FT >& lbase, matrix_c < FT >& lvA )
{
	lbase = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };
	lvA = { 0, 0, 0 };
}

void object_c:: coocked_to_local ( object_c* o, matrix_c < FT >& lbase, matrix_c < FT >& lvA, int& flag )
{
	lbase = motion.object_base ();
	lvA = motion.vA;
}

matrix_c<FT> object_c::to_object ( object_c& object, matrix_c<FT>& pt )
{
	matrix_c<FT> point, base1, base2, vA1, vA2;
	point = pt;
	base1 = motion.object_base ();
	vA1 = motion.object_vector ( 1 );
	base2 = object.motion.object_base ();
	vA2 = object.motion.object_vector ( 1 );
	point = base1*point;
	point = point+vA1;
	point = point-vA2;
	point=~base2*point;
	return point;	
}

matrix_c<FT> object_c::from_object ( object_c& object, matrix_c<FT>& pt )
{
	matrix_c<FT> point, base1, base2, vA1, vA2;
	point = pt;
	base1 = motion.object_base ();
	vA1 = motion.object_vector ( 1 );
	base2 = object.motion.object_base ();
	vA2 = object.motion.object_vector ( 1 );
	point = base2*point;
	point = point+vA2;
	point = point-vA1;
	point=~base1*point;
	return point;	
}

bool object_c::find ( lamb_c &lamb, matrix_c<FT> pt )
{
	return false;
}

void object_c::serialize ( fstream &pfile, bool bsave )
{
}
