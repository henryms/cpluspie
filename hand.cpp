#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_set>
#include <functional>

#include <regex>
#include <iterator>
#include <iterator>

#include "standard.h"

#include "symbol/keysym.h"
#include "symbol.h"

#include "library/wdslib.h"

#include "gate.h"
#include "matrix.h"
#include "position.h"
#include "mstream.h"
#include "sens/retina.h"
#include "sens/spirit.h"
#include "message.h"
#include "cash.h"
#include "regexp.h"

#include "global.h"
#include "object.h"
#include "line.h"
#include "spline.h"
#include "keyb.h"
#include "wr/font.h"
#include "wr/texel.h"
#include "wr/shared.h"
#include "wr/layen.h"
#include "edit.h"

#include "land.h"
#include "mouse.h"
#include "eyes.h"
#include "hand.h"
#include "home.h"
#include "bookmarks.h"
#include "lamb.h"
#include "comtab4.h"
#include "edit_ct.h"
#include "bookmarks.h"
#include "image.h"
#include "make.h"
#include "shell.h"



extern land_c land;

void hand_c::draw ()
{
	retina_c* retina = impression_clients.retina;
	if ( retina == nullptr ) {
		return;
	}
	
	memory_c* memory;
	retina->get_memory ( ui, & impression_clients,  &memory  );
	retina->erase_memories ( ui );
	if (off){
		return;
	}
	matrix_c <FT> ptv = pointer;
	ptv = land.rasterize ( ptv );
	
	int x = ptv[1];
	int y = ptv[2];
	for ( int c=-10; c <= 10 ; c++){
		retina->set_pixel (*memory, x+c, y, 0, 0x00 ); 
		if ( c == 0 ){
			continue;
		}
		retina->set_pixel ( *memory, x, y+c, 0, 0x00 );
	}	
}

void hand_c::draw ( lamb_c& lamb )
{
	draw ();
}


hand_c::hand_c ():pointer ( 3, 1 )
{ui = die();}

hand_c::~hand_c()
{
	if ( impression_clients.retina != 0 ) {
		impression_clients.retina->unregister ( ui );				
	}
	cout << "hand destruction\n";
}




