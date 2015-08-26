#ifndef HAND_C
#define HAND_C

class hand_c : public object_c
{
public:
	hand_c();
	matrix_c <FT> pointer;	
	void draw ( lamb_c& lamb );
	void draw ( );
	uint64_t ui;
	~hand_c();
	bool off{0};
};

#endif