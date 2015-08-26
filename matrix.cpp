#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <iostream>
#include <list>
#include <vector>
#include <sstream>

#include "gate.h"

#include "mstream.h"
#include "matrix.h"



/*
* the set of all the permutation of 1 to n
* the sign set is the 1 or -1 factor to compute the determinant
*/
/*
double sqrt ( matrix_c<float> mx ){
   return sqrt ( mx.l[0]);
};

double sqrt ( matrix_c<double> mx ){
   return sqrt ( mx.l[0]);
};
*/

void permutationset ( int n, std::vector<std::vector<int>>* set, std::vector<int>* sign, bool out ){
	if ( n<= 0 ) return;
	std::vector<int> per (n );
	for ( int i = 0;i<n;i++)per[i]= i+1;
	int k, l, swap, s = 0;
	for(;;){
		set->push_back ( per );
		sign->push_back ( s%2 == 0?1:-1 );
		for ( k = n-1;k>0 && per[k-1]>per[k];k--);	
		if ( k == 0 ) if(!out )return;else break;
		for ( l = n-1;l>= k && per[k-1]>per[l];l--);
		if ( l>= k ){
			swap = per[k-1], per[k-1]= per[l], per[l]= swap, s++;
			for ( l = n-1;k<l;k++, l--){
				swap = per[k], per[k]= per[l], per[l]= swap, s++;
			}
		}
	}
	out = true;
	if ( true ){	
		for ( int i = 0;i<set->size ();i++){
			for ( int j = 0;j<n;j++){
				std::cout << (*set )[i][j] ;			
			}	
			std::cout << ' ' << (*sign )[i] <<std::endl;
		}
	}
}
