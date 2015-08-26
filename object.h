#ifndef OBJECT_H
#define OBJECT_H

using namespace std;

class lamb_c;
class keyboard_c;
struct message;
class message_c;
class surface_c;
class map_memory_c;
class map_memory2_c;
class object_c: public message_c 
{
	public:
	object_c ();
	virtual ~object_c ();
	virtual object_c* duplicate ();
	virtual void shift ( matrix_c<FT>&);
	virtual void serialize ( fstream& file, bool bsave );
	virtual bool find ( lamb_c &lamb, matrix_c<FT>);
	virtual void draw_transcient ( lamb_c &lamb ){};
	virtual void draw () {};
	virtual unsigned long get_type (){};
	virtual int edit ( lamb_c& lamb, keyboard_c& keyb ){ return 0;};
	virtual void move ( int type, int x, int y, int z ){};
	long state;
	motion_3D_c<FT> motion;
	matrix_c<FT> to_object ( object_c&, matrix_c<FT>&);
	matrix_c<FT> from_object ( object_c&, matrix_c<FT>&);
	virtual void shift ( int bf, int lr, int du ){};
	virtual void shift3 ( matrix_c<FT>&){};
	virtual void button ( int, int ){};
	virtual void change_movement ( int para1, int para2 ){};
	virtual void invalide_visual ( int level ){};
	virtual int command2 ( unsigned short, string&, string&){};
	virtual int set_point ( lamb_c& lamb ){return false;}
	virtual void object_to_local ( object_c* o, matrix_c < FT >& lbase, matrix_c < FT >& lvA );
	virtual void coocked_to_local ( object_c* o, matrix_c < FT > & lbase, matrix_c <FT >& lvA, int& flag);
	virtual void quit ();	
	
	impression_clients_c impression_clients;
	
	int object_id;
	object_c* parent;
};

class sensor_c: virtual public object_c
{
	int tactil_edition ();
	int semantic_edition ();
};

class motor_c: virtual public object_c
{
};

class both_c: public sensor_c, public motor_c
{
};
#endif
