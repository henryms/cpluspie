#ifndef RETINA_H
#define RETINA_H

using namespace std;

#include <map>

class tstream_c;
class retina_c;

class object_c;

class zlist_c
{
	public:
	list < pair < float, uint32_t >  >  zpix;
	uint32_t *frame_xy;
};

class localization_c
{
	public:
	float zoom;
	int width;
	int height;
	matrix_c <FT> mx;
	motion_3D_c <FT> motion;
};

class surface_c
{
	public:
	surface_c ( );
	void initialize ( int x, int y, uint32_t color );
	vector < vector < zlist_c > > v;
	
	uint32_t *frame;
	bool is_valide;
	
	matrix_c<FT> world_to_view ( matrix_c<FT>& pt );
	
	float zoom;
	int width;
	int height;

	matrix_c <FT> vA;
	matrix_c < FT> mx;
	motion_3D_c < FT> motion;
	
	~surface_c ();
};

class memory_c
{
	public:
	memory_c ();
	memory_c (const memory_c& );
	memory_c ( list < surface_c >::iterator _surface );
	vector < pair < zlist_c*, list < pair < float, uint32_t > >::iterator> > l;
	list <surface_c >::iterator surface;
	int valide;
	void disapear ();
};

class retina_c;
class spirit_c;

class impression_clients_c
{
	public:
	impression_clients_c(){};
	retina_c* retina{};
	spirit_c* spirit{};
	bool has_focus{};
	bool is_motor{};
	bool is_selected{};
};

class memory_client_c
{
	public:
	memory_client_c();
	list<memory_c> memories;
	impression_clients_c* object; 
};

class retina_c 
{
	public:
	retina_c ();
	~retina_c();
	char* smi;
	char* smo;
	rstream_c ssi;
	rstream_c sso;
	list < surface_c > surfaces;
	
	map <uint64_t ,list< memory_client_c>::iterator > handle_to_list;	
	
	list < memory_client_c > memory_clients;
	void set_pixel (memory_c& , int x, int y, float z, long color);
	
	void enregister ( uint64_t ui );
	void enregister ( uint64_t ui, impression_clients_c* );
	
	void unregister ( uint64_t ui);
	
	
	void erase_memories ( uint64_t ui );
	void get_memory ( uint64_t ui, impression_clients_c*, memory_c** );		
	
	void stream_in2 ();
	void stream_out();
	int height;
	int width;
	object_c* owner;
	int test ( int , string);
	
	void focus ( object_c* object ) ;
	void motor ( object_c* object );
	void select ( object_c*, int state );	
	
	bool is_invalide;
};

#endif
