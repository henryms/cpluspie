#ifndef POSITION_H
#define POSITION_H

#define ORIGINE_woAbx 1
#define ORIGINE_owAbx 2

#include <cmath>
#include "mstream.h"

template<class T>
class motion_c{
	public:
};

template<class T>
class motion_3D_c : public motion_c<T>
{
public:
	motion_3D_c ();
	matrix_c<T> mo;
	matrix_c<T> vA;
	matrix_c<T> vb;
	matrix_c<T> vx;
	unsigned char origine;
	
	bool get_object ( matrix_c<T>* pA, matrix_c<T>* base, T* np_x, T* n_b, T* n_x, T* n_A );
	bool get_world ( matrix_c<T>* pA, matrix_c<T>* base, T* np_x, T* n_b, T* n_x, T* n_A );
	bool set_object ( matrix_c<T>& pA, matrix_c<T>& pb, matrix_c<T>& px );
	bool set_world ( matrix_c<T>& pA, matrix_c<T>& pb, matrix_c<T>& px );	
	matrix_c<T> object_base ();
	matrix_c <T> world_vector ( int no );
	matrix_c<T> object_vector ( int no );
	void set_object_vector ( int no, matrix_c<T> vector );
	T object_norm ( int no );
	T object_projection ();
	void serialize ( std::fstream& fs, bool save );
	void serialize ( rstream_c& rs, bool save );
};

template<class T>
motion_3D_c<T>::motion_3D_c ()
{
	origine = '2';
	set_object_vector ( 1, {0, 0, 0});
	set_object_vector ( 2, {1, 0, 0});
	set_object_vector ( 3, {0, 1, 0});
}

template<class T>
void motion_3D_c<T>::serialize ( rstream_c& rs, bool save )
{
	vA.serialize ( rs, save );
	vb.serialize ( rs, save );
	vx.serialize ( rs, save );
	mo.serialize ( rs, save );
	if ( save == false ){
		rs >> origine;
	}
	else {
		rs << origine;
	}
}

template<class T>
void motion_3D_c<T>::serialize ( std::fstream& fs, bool save )
{
	vA.serialize ( fs, save );
	vb.serialize ( fs, save );
	vx.serialize ( fs, save );
	mo.serialize ( fs, save );
	if ( save == false ){
		fs >> origine;
	}
	else {
		fs << origine << std::endl;
	}
}

template<class T>
bool motion_3D_c<T>::set_object ( matrix_c<T>& vectorA, matrix_c<T>& vectorb, matrix_c<T>& vectorx )
{
	vA = vectorA;
	vb = vectorb;
	vx = vectorx;
	return true;
}

template<class T>
matrix_c<T> motion_3D_c<T>::object_base ()
{
	matrix_service_c<T> mxsrv;
	T npx;
	return mxsrv.ortho3 ( vb, vx,&npx, 0, 0 );
}

template<class T>
void  motion_3D_c<T>::set_object_vector ( int no, matrix_c<T> vector )
{
	switch ( no ){
	case 1:
		vA = vector; return;
	case 2:
		vb = vector; return;
	case 3:
		vx = vector; return;
	} 
}

template<class T>
matrix_c<T> motion_3D_c<T>::world_vector ( int no )
{
	matrix_c<T> m = object_base ();
	m = m.inverse ();
	switch ( no ){
	case 1:
		return -m*vA;
	case 2:
		return m.get_column ( 1 );
	case 3:
		return m.get_column ( 2 );
	} 
}

template<class T>
matrix_c<T> motion_3D_c<T>::object_vector ( int no )
{
	switch ( no ){
	case 1:
		return vA;
	case 2:
		return vb;
	case 3 :
		return vx; 
	default:
		return matrix_c<T>(3, 1 );
	}
}

template<class T>
T motion_3D_c<T>::object_norm ( int no )
{
	switch ( no ){
	case 1:
	    return sqrt ((~vA*vA ).l[0]);
	case 2:
	    return sqrt ((~vb*vb ).l[0]);
	case 3:
	    return sqrt ((~vx*vx ).l[0]);
	default:
	    return  1;
    }
}

template<class T>
T motion_3D_c<T>::object_projection ()
{
	matrix_service_c<T> mxsrv;
	T nx;
	mxsrv.ortho3 ( vb, vx, &nx, 0, 0 );
	return nx; 
}

template<class T>
bool motion_3D_c<T>::set_world ( matrix_c<T>& vectorA, matrix_c<T>& vectorb, matrix_c<T>& vectorx )
{
	return true;
}

template<class T>
bool motion_3D_c<T>::get_object ( matrix_c<T>* _vA, matrix_c<T>* base, T* np_x, T* n_b, T* n_x, T* n_A )
{ 
	matrix_service_c<T> mxsrv;
	*_vA = vA;
	*base = mxsrv.ortho3 ( vb, vx, np_x, n_b, n_x );
	if ( n_A ){
		*n_A = sqrt((~vA*vA ).l[0]);
	}
	return true;
}

template<class T>
bool motion_3D_c<T>::get_world ( matrix_c<T>* _vA, matrix_c<T>* base, T* np_x, T* n_b, T* n_x, T* n_A )
{ 
	return true;
}

#endif \\POSITION_H
