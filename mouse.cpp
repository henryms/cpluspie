#include <cstdlib>
#include <cstdio>

#include <chrono>
#include "mouse.h"


using namespace std;

void mouse_c::momentum(int &x, int acc, int decc)
{
}

void mouse_c::inertia(int *px, int* py, int *pz)
{
	
	if(px != 0){
		momentum(*px, 0, 0);
	}
	if(py != 0){
		momentum(*py, 0, 0);
	}	
	using namespace chrono;
	steady_clock::time_point t1 = steady_clock::now();
//	land.impress(*this);

	steady_clock::time_point t2 = steady_clock::now();

	//duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	//echo << "It took me " << time_span.count() << " seconds." << endl;

}


