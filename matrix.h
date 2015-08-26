#ifndef MATRIX_H
#define MATRIX_H
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>


#define FT float


#include "mstream.h"
void permutationset (
int n, 
std::vector<std::vector<int>>* set, 
std::vector<int>* sign, 
bool out = false );


class rstream_c;

template<class T> class matrix_c;

template<class T> T operator <<(T&, matrix_c<T>&);

template<class T>
class field_c
{
	public:
	field_c ( T d ){v = d;};
	field_c (){};
	T v;
	field_c ( matrix_c<T> mx );
	operator T () const {return v;};
	matrix_c<T> operator *(matrix_c<T> mx );
};

template<class T>
class ctl_c
{
	public :
	matrix_c<T>* mx;
	int i;
	ctl_c ( T ){};
	ctl_c (){};
	T& operator =(T d ){mx->l[i-1]= d;};
	operator double (){return mx->l[i-1];}
	T& operator [](int j ){return mx->get ( i, j );};
};

template<class T>
class matrix_c
{
public:
	matrix_c ();
	matrix_c ( const matrix_c<T>&);
	matrix_c ( int i, int j );
	matrix_c ( std::initializer_list<T> list );
	matrix_c ( std::initializer_list<matrix_c<T>> list );
	T get ( int index );
	T& get ( int i, int j );
	matrix_c<T> get_column ( int index );
	matrix_c<T> get_row ( int index, int jb = 1, int je = 0 );
	void set_column ( int index, matrix_c<T> mx );
	std::vector<T> l;


	matrix_c<T> operator+(matrix_c<T> ma );
	matrix_c<T> operator-(matrix_c<T> ma );
	matrix_c<T> operator~(void );
	matrix_c<T> operator*(matrix_c<T> ma );
	matrix_c<T> operator*(T );
	matrix_c<T> operator!(void ){return *this;};

	T operator|(matrix_c<T>& ma );
	T operator||(matrix_c<T>& ma );

	ctl_c<T> la;
	ctl_c<T>& operator[](int i )
	{
	    la.mx = this;
	    la.i = i;
	    return la;
	}
	T& operator ()(int i1, int i2 )
	{
		return get ( i1, i2 );
	};
	T& operator ()()
	{
		return get ( 1, 1 );
	};	
	matrix_c<T> comatrix ();
	matrix_c<T> inverse ();		

	matrix_c<T> normal ( int i, std::vector<T>* norms = 0 ); //normalize the i column
	matrix_c<T> GS_ortho (); //Gram-Schmidt orthogonalisation
	matrix_c<T> surface ();
	matrix_c<T> ortho ( std::vector<T>* norms = 0 );  //construct an orthogonal matrix 
	T cofactor ( int i, int j );
	T minor_ ( int i, int j );
	T det ();
	void out ( int tab = 1 );
	void out ( int tab, std::stringstream&);
	int m; //row numbers
	int n; //column numbers	
public:
	friend T operator<< <>(T&, matrix_c<T>&);
	friend matrix_c<T> operator/  (T, matrix_c<T>&)
	{
		matrix_c& mx;
		return mx;
	};	
	friend matrix_c<T> operator*  (T f, matrix_c<T> mx ){
		matrix_c<T>& mxdum = mx;
		typename std::vector<T>::iterator it = mxdum.l.begin ();
		for(;it!= mxdum.l.end ();it++){
			*it*= f;
		}; 
		return mxdum;
	};	
	friend matrix_c<T> operator/  (matrix_c<T>&, matrix_c<T>&)
	{
		matrix_c mx;
		return mx;
	};	
	void serialize ( std::fstream& f, bool save );
	void serialize ( rstream_c& rs, bool save );
};


template<class T>
matrix_c<T>::matrix_c ( const matrix_c<T>& matrix )
{
	m = matrix.m;
	n = matrix.n;
	l = matrix.l;
}


template<class T>
void matrix_c<T>::serialize ( std::fstream& f, bool save )
{
	if ( save == false ){
		f >> m;
		f >> n;
		l.resize ( m*n );
		for ( int cn = 1; cn <= n ; cn++){
			for ( int cm = 1; cm <= m ; cm++){
				T r;			
				f >> r;
				l[(cm-1 )*n+cn-1]= r;	//	get ( cm, cn );
			}
		}
	}
	else{
		f << m << std::endl;
		f << n << std::endl;
		for ( int cn = 1; cn <= n ; cn++){
			for ( int cm = 1; cm <= m ; cm++){
				f << get ( cm, cn ) << std::endl;
			}
		}
	}
}

template<class T>
void matrix_c<T>::serialize ( rstream_c& rs, bool save )
{
	float rr;
	if ( save == false ){
		rs >> m;
		rs >> n;
		l.resize ( m*n );
		for ( int cn = 1; cn <= n ; cn++){
			for ( int cm = 1; cm <= m ; cm++){
				T r;			
				rs >> r;
				l[(cm-1 )*n+cn-1]= r;	//	get ( cm, cn );
			}
		}
	}
	else{
		rs << m;
		rs << n;
		for ( int cn = 1; cn <= n ; cn++){
			for ( int cm = 1; cm <= m ; cm++){
				rs << get ( cm, cn );
			}
		}
	}
}

template<class T>
field_c<T>::field_c ( matrix_c<T> mx )
{
	v = mx[1];
}

template<class T>
matrix_c<T> field_c<T>::operator*(matrix_c<T> mx )
{
	typename std::vector<T>::iterator it = mx.l.begin ();
	for(;it!= mx.l.end ();it++){
		*it = v**it;
	}
	return mx;
}

template<class T>
class matrix_service_c
{
public :
	matrix_service_c (){};
	matrix_c<T> ortho3 ( matrix_c<T> ptb, matrix_c<T> ptx, T* nxy, T* nx, T* ny );
	matrix_c<T> rotate3 ( T A, T B, T C );
	matrix_c<T> vectorProduct3 ( matrix_c<T> vecA, matrix_c<T> vecB );
	matrix_c<T> vx3 ( matrix_c<T> vb, T rayon );
};

template <class T> 
matrix_c<T> matrix_service_c<T>::vx3 ( matrix_c<T> vb, T r )
{
	matrix_c<T> m={1, 0, 0};
	if ( vb[1]== 0 && vb[2]== 0 && vb[3]== 0 ){
		return r*m;
	}
	else if ( abs ( vb[1]) <= abs ( vb[2])){
		if ( abs ( vb[1]) <= abs ( vb[3])){
			m = {1, 0, 0};
		}
		else {
			m = {0, 0, 1};
		}
	}
	else if( abs ( vb[2]) <= abs ( vb[3])){
		m= {0, 1, 0};
	}
	else{
		m = {0, 0, 1};
	}
	T tt;
	ortho3 ( vb, m,&tt, 0, 0 );
	matrix_c<T> mm = r*m;
	return mm;
}

template <class T>
matrix_c<T> matrix_service_c<T>::vectorProduct3 ( matrix_c<T> vecA, matrix_c<T> vecB )
{
	matrix_c<T> mx={vecA, vecB, matrix_c<T>(3, 1 )};
	mx = mx.GS_ortho ();
	return mx.get_column ( 3 ); 
}

template<class T>
matrix_c<T> matrix_service_c<T>::rotate3 ( T A, T B, T C )
{
	//f = S;
	matrix_c<T> mx ( 3, 3 );
	double pi = 3.1415926535;
	double a, b, c, D;
	a=(pi*A )/180;
	b=(pi*B )/180;
	c=(pi*C )/180;
	/*
	|+cos ( c ) -sin ( c ) 0| |1	  0        0  | |+cos ( a ) 0  +sin ( a )| 	
	|+sin ( c ) +cos ( c ) 0|X|0 +cos ( b ) -sin ( b )|X|     0  1       0 |
	|   0       0    1| |0 +sin ( b ) +cos ( b )| |-sin ( a ) 0  +cos ( a )| 
	*/
	/*
	|+cos ( c ) -sin ( c ) 0| |+cos ( b ) 0 +sin ( b )| |1    0      0    | 	
	|+sin ( c ) +cos ( c ) 0|X|     0  1      0 |X|0 +cos ( a ) -sin ( a )|
	|  0       0     1| |-sin ( b ) 0 +cos ( b )| |0 +sin ( a ) +cos ( a )|  
	*/

	mx.l[0]= cos ( b )*cos ( c )+sin ( b )*sin ( a )*sin ( c );
	mx.l[1]=-cos ( b )*sin ( c )+sin ( b )*sin ( a )*cos ( c );
	mx.l[2]= sin ( b )*cos ( a );
	mx.l[3]= cos ( a )*sin ( c );
	mx.l[4]= cos ( a )*cos ( c );
	mx.l[5]=-sin ( a );
	mx.l[6]=-sin ( b )*cos ( c )+cos ( b )*sin ( c )*sin ( a );
	mx.l[7]= sin ( b )*sin ( c )+cos ( b )*sin ( a )*cos ( c );
	mx.l[8]= cos ( b )*cos ( a );
	
	return mx;
	
	/*@
	a11 = cos ( b )*cos ( c )+sin ( b )*sin ( a )*sin ( c );
	a12=-cos ( b )*sin ( c )+sin ( b )*sin ( a )*cos ( c );
	a13 = sin ( b )*cos ( a );
	a21 = cos ( a )*sin ( c );
	a22 = cos ( a )*cos ( c );
	a23=-sin ( a );
	a31=-sin ( b )*cos ( c )+cos ( b )*sin ( c )*sin ( a );
	a32 = sin ( b )*sin ( c )+cos ( b )*sin ( a )*cos ( c );
	a33 = cos ( b )*cos ( a );
	@*/

	/*
	a11 = cos ( a )*cos ( c )+sin ( a )*sin ( b )*sin ( c );
	a12=-cos ( a )*sin ( c )+sin ( a )*sin ( b )*cos ( c );
	a13 = sin ( a )*cos ( b );
	a21 = cos ( b )*sin ( c );
	a22 = cos ( b )*cos ( c );
	a23=-sin ( b );
	a31=-sin ( a )*cos ( c )+cos ( a )*sin ( c )*sin ( b );
	a32 = sin ( a )*sin ( c )+cos ( a )*sin ( b )*cos ( c );
	a33 = cos ( a )*cos ( b );
*/
}

template<class T>
matrix_c<T> matrix_service_c<T>::ortho3 ( matrix_c<T> ptb, matrix_c<T> ptx, T* nxb, T* nb, T* nx )
{
	matrix_c<T> mx={ptb, ptx};
	std::vector<T> norms;
	mx = mx.ortho(&norms );
	if ( nxb ){
		*nxb = norms.at ( 1 );
	}
	if ( nb ){
		*nb = norms.at ( 0 );
	}		
	if ( nx ){
		*nx=(~ptb*ptb ).l[0];
	}
	return mx;
}    
/*
class dum
{
    public:
	//	friend const double operator=(const double&, const dum&);
};
*/

template<class T>
matrix_c<T> matrix_c<T>::GS_ortho ()
{
	matrix_c<T> mx;
	typename std::vector<matrix_c<T>> setzn, setz;
	typename std::vector<matrix_c<T>>::iterator itzn, itz;
	assert ( setz.size () == 0 );
	itz = setz.begin ();
	matrix_c<T>x, z, nz;
	for ( int i = 1;i<= n;i++){ 
		x = get_column ( i );
		z = x;
		for ( itzn = setzn.begin ();itzn!= setzn.end ();itzn++){
			nz=*itzn;
			z = z-(~nz*x ).l[0]*nz;	
		}
		setz.push_back ( z );
		z = z.normal ( 0 );
		setzn.push_back ( z );
	}
	matrix_c<T> mr;
	int c = 1;
	for ( itz = setz.begin ();itz!= setz.end ();itz++){
		mr.set_column ( c,*itz );
		c++;
	}
	return mr;
}

template<class T>
matrix_c<T> matrix_c<T>::surface ()
{
	matrix_c<T> mx;
	int j;
	for ( j = 1;j<= n;j++){
		mx.set_column ( j, get_column ( j ));
	}
	mx.set_column ( j,{{0}});
	for ( int i = 1;i<= m;i++){
		mx.get ( i, j ) = mx.cofactor ( i, j );	
	}
	return mx;
}

template<class T>
matrix_c<T> matrix_c<T>::ortho ( std::vector<T>* norms )
{
	matrix_c<T> mx;
	int j = n + 1;
	mx = GS_ortho ();
	mx = mx.normal ( 0, norms );
	mx.set_column ( j, {{0}} );
	for ( int i = 1; i<= m; i++){
		mx.get ( i, j ) = mx.cofactor ( i, j );	
	}
	return mx;
}

template<class T>
matrix_c<T> matrix_c<T>::normal ( int i, std::vector<T>* norms )
{
	matrix_c<T> mx, mc;
	for ( int i = 1; i<= n; i++){
		mc = get_column ( i );
		T n = (~mc*mc ).l[0];
		n = sqrt ( n );
		assert ( n != 0 );
		if ( norms != 0 ){
			norms->push_back ( n );
		}
		mc = 1/n*mc;
		mx.set_column ( i, mc );
	}
	return mx;
}

template<class T>
void matrix_c<T>::set_column ( int j, matrix_c<T> mx ){
	int nn = n;
	int mm = m; 
	if ( n<j ){
		n = j;
	}
	if ( m<mx.m ){
		m = mx.m;
	}
	l.resize ( m*n );
	for ( int i = mm*nn;i>1;i--){
		T v = l[i-1];
		l[i-1]= 0;
		l[i+(i-1 )/nn*(n-nn )-1]= v;
	}	
	for ( int i = 1;i<= m;i++ ){ 
		T v;
		if ( i<= mx.m ){
			v = mx.get ( i, 1 );
		}
		else{
			v = 0;
		}
		l[n*(i-1 )+j-1]= v;
	}
}

template<class T>
matrix_c<T> matrix_c<T>::get_row ( int i, int jb, int je )
{	
	matrix_c<T> mx;
	mx.m = 1;
	if ( je == 0 ){
		je = n;
	}
	mx.n = je-jb+1; 
	for ( int j = jb; j<= je; j++){
		mx.l[j-jb]= get ( i, j );	
	}
	return mx;
}

template<class T>
matrix_c<T> matrix_c<T>::get_column ( int j )
{
	matrix_c<T> mx;
	mx.n = 1, mx.m = m;
	mx.l.resize ( mx.m );
	for ( int i = 1;i<= m;i++ ){ 
		mx.l[i-1]= get ( i, j );
	}
	return mx;
}

template<class T>
void matrix_c<T>::out ( int tab )
{
	tab = 16;
	std::string str;
	std::stringstream ss;
	for ( int i = 1;i<= m;i++){
		for ( int j = 1;j<= n;j++){
			ss << get ( i, j );
			str = ss.str (); 
			ss.str("" );
			ss.clear ();
			std::cout << get ( i, j );
			if ( j<n ){
				for ( int t = 1;t<(tab-str.length ());t++){
					 std::cout << ' ';
				}
			} 
		}
		std::cout << std::endl;
	}
}

template<class T>
void matrix_c<T>::out ( int tab, std::stringstream& sss ){
	tab = 16;
	std::string str;
	std::stringstream ss;
	for ( int i = 1; i<= m; i++){
		for ( int j = 1; j <= n; j++){
			ss << get ( i, j );
			str = ss.str (); 
			ss.str("" );
			ss.clear ();
			sss << get ( i, j );
			//std::cout << get ( i, j );
			if ( j<n ){
				//for ( int t = 1;t<(tab-str.length ());t++) std::cout << ' ';
				for ( int t = 1;t<(tab-str.length ());t++) sss << ' ';
			} 
		}
		sss << std::endl;
		//std::cout << std::endl;
	}
}

template<class T>
matrix_c<T> matrix_c<T>::inverse ()
{
	matrix_c<T> mx = comatrix ();
	mx=~mx;
	T d = det ();
	(1/d )*mx;
	return mx;
}

template<class T>
matrix_c<T> matrix_c<T>::comatrix ()
{
	matrix_c<T> mx;
	mx.l.resize ( m*n );
	mx.m = m, mx.n = n;
	for ( int i = 1;i<= m;i++){
		for ( int j = 1;j<= n;j++){
			mx.l[(i-1 )*n+j-1]= cofactor ( i, j );
		}
	}
	return mx;
}

template<class T>
T matrix_c<T>::cofactor ( int i, int j )
{
	int sign=(i+j )%2 == 0?1:-1;
	return sign*minor_ ( i, j );	
}

template<class T>
T matrix_c<T>::minor_ ( int i, int j )
{
	typename std::vector<T>::iterator itf, its;
	matrix_c<T> mx=*this;
	itf = its = mx.l.begin ();
	itf+=(i-1 )*n;
	its = itf+n;
	mx.l.erase ( itf, its );
	mx.m--;
	for ( int k=(mx.m-1 )*mx.n+j-1;k>= 0;k-= mx.n ){
		mx.l.erase ( mx.l.begin ()+k );
	};
	mx.n--;
	T dum = mx.det ();
	return dum; 
}

template<class T> 
T matrix_c<T>::det ()
{
	if ( m!= n ) return 0;
	std::vector<std::vector<int>> set;
	std::vector<int> sign;	
	permutationset ( m,&set,&sign, 0 );
	std::vector<std::vector<int>>::iterator iter;
	std::vector<int>::iterator its;
	iter = set.begin ();
	its = sign.begin ();
	T det = 0;
	for(;iter!= set.end ();iter++, its++){
		T fac = 1;
		std::vector<int> nu=*iter;
		for ( int i = 0;i<m;i++){
			fac*= l[i*n+nu[i]-1];
		}
		det+= fac**its;		
	}	
	return det;
}

template<class T> T operator<<(T& a, matrix_c<T>& mx ){
	if ( mx.m!= mx.n ){
		return 0;
	}
	std::vector<std::vector<int>> set;
	std::vector<int> sign;	
	permutationset ( mx.m,&set,&sign );
	std::vector<std::vector<int>>::iterator iter;
	std::vector<int>::iterator its;
	iter = set.begin ();
	its = sign.begin ();
	T det = 0;
	for(;iter!= set.end ();iter++, its++){
		T fac = 1;
		std::vector<int> nu=*iter;
		for ( int i = 0;i<mx.m;i++){
			fac*= mx.l[i*mx.m+nu[i]];
		}
		det+= fac**its;		
	}	
	return det;
}

template<class T> 
matrix_c<T> matrix_c<T>::operator~(void )
{
	matrix_c<T> mx;
	mx.l.resize ( m*n );
	mx.m = n;
	mx.n = m;
	for ( int i = 0;i<m;i++){
		for ( int j = 0;j<n;j++)
		mx.l[j*m+i]= l[i*n+j];	
	}
	return mx;
}

template<class T>
matrix_c<T>::matrix_c ()
{
	m = 1, n = 1;
	l.resize ( 1 );
}


template<class T>
matrix_c<T>::matrix_c ( int i, int j )
{
	l.resize ( i*j );
	m = i;
	n = j;
}

template<class T>
matrix_c<T>::matrix_c ( std::initializer_list<T> list ){
	m = list.size ();
	n = 1;
	l.resize ( 0 );
	typename std::initializer_list<T>::iterator it;
	for ( it = list.begin ();it!= list.end ();it++){
		l.push_back(*it );	
	}
}

template<class T>
matrix_c<T>::matrix_c ( std::initializer_list<matrix_c<T>> list )
{
	typename std::initializer_list<matrix_c<T>>::iterator it = list.begin ();
	matrix_c<T> mx=*it;
	m = list.size ();
	n = mx.l.size ();
	l.resize ( 0 );
	int column = false;
	if ( mx.n == 1 ) 
	column = true;
	for ( it = list.begin ();it!= list.end ();it++){
		mx=*it;
		typename std::vector<T>::iterator itf;
		itf = mx.l.begin ();
		for(;itf!= mx.l.end ();itf++){
			l.push_back(*itf );
		}
	}
	if ( column ){
		*this=~*this;
	}
}

template<class T>
T matrix_c<T>::operator||(matrix_c<T> & sec )
{
	matrix_c<T> mx=~*this*sec;
	return sqrt ( mx.l[0]);
}

template<class T>
T matrix_c<T>::operator | (matrix_c<T> & sec )
{
	matrix_c<T> mx=~*this*sec;
	return mx.l[0];
}

template<class T>
matrix_c<T> matrix_c<T>::operator*(T e )
{
	typename std::vector<T>::iterator it;
	it = l.begin ();
	for(;it!= l.end ();it++){
		*it=*it*(e );
	}
	return *this;
}

template<class T>
matrix_c<T> matrix_c<T>::operator*(matrix_c<T> sec )
{
	matrix_c<T> mx;
	mx.l.resize ( m*sec.n );
	mx.m = m;
	mx.n = sec.n;
	if ( n!= sec.m ) {
		return mx;
	}
	for ( int i1 = 1;i1<= sec.n;i1++){
		for ( int i2 = 1;i2<= m;i2++){
			T a = 0;
			for ( int i3 = 1;i3<= sec.m;i3++){
			a+= get ( i2, i3 )*sec.get ( i3, i1 );
			}
			mx.get ( i2, i1 ) = a	;
		}	
	}
	return mx;
}

template<class T>
matrix_c<T> matrix_c<T>::operator-(matrix_c<T> sec )
{
	matrix_c<T> res;
	res.m = m, res.n = n;
	res.l.resize ( 0 );
	typename std::vector<T>::iterator itf, its;
	for ( itf = l.begin (), its = sec.l.begin ();itf!= l.end ();itf++, its++){
		res.l.push_back(*itf-*its );
	}
	return res;
}


template<class T>
matrix_c<T> matrix_c<T>::operator+(matrix_c<T> sec )
{
	static matrix_c<T> res;
	res.l.resize ( 0 );	
	res.m = m, res.n = n;
	typename std::vector<T>::iterator itf, its;
	for ( itf = l.begin (), its = sec.l.begin ();itf!= l.end ();itf++, its++){
		res.l.push_back(*itf+*its );
	}
	return res;
}

template<class T>
T& matrix_c<T>::get ( int i, int j )
{
	return l[(i-1 )*n+j-1];
}

template<class T>
T matrix_c<T>::get ( int index )
{
	return l[index-1];
}

#endif
