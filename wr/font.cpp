#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <list>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H


#include "debug.h"

#include "symbol/keysym.h"

#include "library/wdslib.h"
#include "matrix.h"
#include "position.h"

#include "mstream.h"
#include "sens/retina.h"
#include "sens/spirit.h"

#include "global.h"
#include "texel.h"
#include "shared.h"
#include "font.h"
#include "layen.h"

using namespace std;

FT_Library library = 0;

FT_Face font_c::system_face {};

unsigned long font_c::type = 2;

float z;

void bitmap_c::set_box(int w, int r, uint32_t color)
{
	delete buffer;
	width= w;
	rows = r;
	buffer = new uint32_t[w*r];
	auto p = buffer;
	for ( int i = 0; i < w*r; i++ ) {
		*p = 0x0ffffff;
		p++;
	}
	p = buffer;	
	for ( int i = 0; i < r; i++){
		*p = 0;
		p+=w-1;
		*p = 0;
		p++;
	}
	auto q = buffer;
	p = q+(r-1)*w;
	for ( int i = 0; i < w; i++) {
		*(p++)=0;
		*(q++)=0;
	}
}

void bitmap_c::set (int w, int r ){
	delete buffer;
	buffer = new uint32_t[w*r];
	memset (buffer, 0x0, r*w*4 );
	width= w;
	rows = r;
}

glyph_ft2_c::glyph_ft2_c()
{
	bitmapGlyph = nullptr;
}

void glyph_ft2_c::dump(stringstream& ss)
{
	FT_BitmapGlyph & b= bitmapGlyph;
	ss 
	<< "root.advance.x: " << b->root.advance.x
	<< "root.advance.y: " << b->root.advance.y;
	echo << ss.str() << '\n';
}

glyph_ft2_c::~glyph_ft2_c()
{
//	cout << "glyph destruction\n";
}

FT_Face font_c::set_face(string name, size_t size, float f )
{
	FT_Face face{};
	if ( name.substr(0,1) !="/"){
		name = get_lambs_path()+"/fonts/"+name;
	}
	
	int error = FT_New_Face(library, name.c_str (), 0, &face );
	if ( error ) {
		cout << "free type: err  new face " <<  name<< '\n';
		return nullptr;
	}
	else{
		mout << "ok new face\n";
	}
	error = FT_Set_Char_Size ( face, 0, size*64*f, 0, 92 );
	if ( error ){
	    cout << "free type: err size\n";
		return nullptr;
	}
	return face;
}

int font_c::getY ( string c )
{
	return 20;
}

int font_c::test ( stringstream& ss , int sel)
{
	cout << "test\n";
	return 0;
/*	
	sel = 1;
	if ( sel == 1 ) {
		char chr = 'p';
		FT_UInt glyph_index;
		glyph_index = FT_Get_Char_Index ( face, chr );
		glyph_ft2_c  glyph1;
		glyph1.create_bitmap_glyph (face,glyph_index );
		glyph_ft2_c  glyph2;
		glyph2.create_bitmap_glyph (face,glyph_index);
		ss 
		<< "glyph1: " << glyph1.bitmapGlyph << endl
		<< "bitmap: " << (void*)glyph1.bitmapGlyph->bitmap.buffer << endl
		<< "glyph2: " << glyph2.bitmapGlyph << endl
		<< "bitmap: " << (void*)glyph2.bitmapGlyph->bitmap.buffer << endl
		;
	}
	sel = 2;
	if ( sel == 2 ) {
		string fontpath = get_lambs_path ();
		echo << "lambs path: " << fontpath << endl;
		fontpath+="/fonts/Vera.ttf";
		FT_Face face1, face2;
	 	FT_New_Face ( library, fontpath.c_str (), 0, &face1 );
//		fontpath = fontpath + "/fonts/VeraMono.ttf";
	 	FT_New_Face ( library, fontpath.c_str (), 0, &face2 );
		echo << "face1: " << face1 << endl;
		echo << "face2: " << face2 << endl;
	} 
*/
}

matrix_c<FT> font_c::get_text_size ( string str )
{
	cout << "get_text_size\n";
	matrix_c<FT> size ( 3, 1 );
//	matrix_c<FT> size { 0, 0 };
	vector<texel_c*> texels;
	layen->set_text( str,&texels );
	for ( auto x : texels ) {
		if ( x->is_tab() ) {
//			size[1][1]+= 40*z;
			size[1][1]+= 40;
		}
		else { 
			size [1][1]+= x->advance_x();
		}
	}					
	for(auto x : texels){
		if ( x->dec_reference() == 0) {
			delete x;
		}
	}	
	
	return size;
}

int font_c::my_draw2 ( graphy_c & glyph, layen_c& layen )
{
	if ( glyph.bitmap.buffer==nullptr ) {
		return 0;
	}
	if ( ! glyph.is_vectorized ) {
		int	
		width = glyph.bitmap.width,
		rows = glyph.bitmap.rows,
		ix = glyph.left,
		iy = glyph.top,
		t = -layen.wi*iy + ix,
		p = 0;
		for ( int j = 0 ; j < rows ; j++){
			for ( int i = 0;  i < width ; i++){
				uint32_t col = glyph.bitmap.buffer[p+i];
				if ( (col&0xffffff) < 0xffffff ){
					glyph.pixels.push_back ( pixel_c ( t+i, 0 , col ));
				}
			}
			p += width;
			t += layen.wi;
		}
		glyph.is_vectorized = true;
	}
	if ( glyph.pixels.empty() ) {
		return 0;
	}
	int cc = (-layen.y + layen.penY)*layen.wi+layen.penX;
	/*
	if ( z != 1 ){
		ix /= z;
		iy /= z;
	}
	*/
	uint32_t colo = 0;
	if ( layen.is_checked ){
		colo = 0xff0000;
	}
	else if ( layen.is_selected ){
		colo = 0xff;
	}	
	if ( 0<= glyph.pixels.front().x+cc 
		&& glyph.pixels.back().x+cc < layen.si ) {
		for ( auto& x : glyph.pixels ) {
			*(layen.pi + cc + x.x ) = x.color | colo;
		}
	}
	else {
		for ( auto& x : glyph.pixels ) {
			int c = cc + x.x;
			if ( 0<= c && c < layen.si ) {
				*(layen.pi+c ) = x.color | colo;
			}
		}
	}
	return 0;
}

int font_c::done () 
{
	FT_Done_Face ( system_face );
	FT_Done_FreeType ( library );
}

int font_c::init ()
{
	int error = FT_Init_FreeType (&library );
	if ( error ) {
		cout << "free type: err init\n";
		return 1;
	}
	else {
		mout << "initialise freetype\n";
	}
	return 0;
}


int font_c::get_text_size ( vector<texel_c*>& texels, size_t inz, size_t inz_e ){
	int x = 0;
	for (; inz < inz_e; inz++){
		texel_c& tex = *texels.at ( inz );
		tex.construct();
		if ( tex.is_tab () ) {
		//	layen.penX += 40-(layen.penX%40 );
			x += 40*z;
		}
		else if (! tex.is_caret() ){
			x += tex.advance_x();
		}
	}
	return x;
}

/*
c , nm , 'w'	;change_mode visual
c , nm , 'i'	;change_mode select
*/

void font_c::resolve_elastic_tabs2(layen_c& layen, list<list<tab>>& tabs)
{
	int ptexs = -1;
	int max_size = 0;
	
//	if ( layen.signo == 1 ) {
	if (0) {
		echo <<"set elastic: " << tabs.size() << '\n';
		int c = 3;
		for (auto x: tabs){
			echo << "size: " << x.size() << '\n';
			for (auto y: x){
				echo<< y.ctexs << " " << y.size <<'\n';
			}
			if ( --c == 0 ) {
				break;
			}
		}
	}	
	int c = 3;
	while ( ! tabs.empty()){
		for (auto it = tabs.begin(); it !=tabs.end();){
			if (it->empty()){
				it = tabs.erase(it);
				continue;
			}		
			auto ita = it;
			for ( ;it !=tabs.end(); it++){
				if (it->empty()){
					break;
				}
				if ( ptexs != -1 && ptexs != it->front().ctexs ){
					break;
				}
				max_size = max(max_size, it->front().size);
				ptexs = it->front().ctexs;
			}
			for ( ; ita != it; ita++ ){
				texel_tab_c* t = (texel_tab_c*)*(ita->front().it);
				t->size = 30 + max_size -ita->front().size;
//				if ( layen.signo ==1 && c){
				if (0){
					echo << max_size<< ":" << ita->front().size<<'\n';
					c--;
				}
				ita->pop_front();
			}
			ptexs = -1;
			max_size = 0;
		}
	}
}


void font_c::set_elastic_tabs(layen_c&layen, vector<texel_c*>& texs)
{
	list<list<tab>> tabs;
	int 
	ctexs = 0,
	size = 0;
	tabs.push_front(list<tab>());
	auto it = tabs.begin();
	for (auto i = texs.begin(); i!= texs.end();i++){
		auto x = *i;
		if ( x->is_new_line()){
			it = tabs.insert(++it, list<tab>());
			ctexs = 0;
			size = 0;
		}
		else if ( x->is_tab()){
			it->push_back(tab(size, ctexs, i));
			size = 0;
			ctexs = 0;
		}
		else if ( x->is_text()){
			ctexs++;
			size += x->advance_x();
		}
	}
	resolve_elastic_tabs2(layen, tabs);
} 

void font_c::set_elastic_tabs(layen_c& layen)
{
	list<list<tab>> tabs;
	int 
	ctexs = 0,
	size = 0;
	tabs.push_front(list<tab>());
	auto it = tabs.begin();
	for (auto i = layen.texels.begin(); i!= layen.texels.end();i++){
		auto x = *i;
		if ( x->is_new_line()){
			it = tabs.insert(++it, list<tab>());
			ctexs = 0;
			size = 0;
		}
		else if ( x->is_tab()){
			it->push_back(tab(size, ctexs, i));
			size = 0;
			ctexs = 0;
		}
		else if ( x->is_text()){
			ctexs++;
			size += x->advance_x();
		}
	}
	resolve_elastic_tabs2(layen, tabs);
}

void font_c::set_rows(
layen_c& layen, 
vector<texel_c*>& texs,
vector<texel_c*>::iterator begin, 
vector<texel_c*>::iterator end
)
{
if(0){
	vector<texel_c*>ts;
	int c = 0,
	prev_caret_row;
	
}

else{
	vector<texel_c*> ts{begin, end};
	cout << "ts size:" << ts.size() << endl;
	int c = 0,
	prev_caret_row;
	for (auto it = ts.begin(); it != ts.end();){
		if ((*it)->is_row()){
			++c;
			it = ts.erase(it);
			continue;
		}
		if ( (*it)->is_caret()){
			prev_caret_row = c;
		}	
		it++;
	}	
	set_rows(layen, &ts);
	auto& caret = *layen.get_caret();
	begin = texs.erase(begin, end);
	auto it = texs.insert(begin, ts.begin(), ts.end());
	c = 0;
	for (; it != texs.end(); ++it){
		if ((*it)->is_row()){
			++c;
		}
		if ((*it)->is_caret()){
			caret.info.iterator = it;
			caret.info.scroll_up += c-prev_caret_row;
			break;
		}
	}	
}
	return;
}

void font_c::set_rows ( layen_c& layen, vector<texel_c*>* texss ) 
{
	static int c = 0;	
	vector<texel_c*>* t;
	if ( texss ){
		t = texss;
	}
	else {
		t = &layen.texels;
	}
	vector<texel_c*>& texels = *t;
	if ( texels.empty () ) {
		return;
	}
	if ( texels.front()->is_row() ) {
		return;
	}
	if ( layen.signo == 12 ) {
		cout << "set rows: " << ++c << '\n';
	}
	
	for ( auto x: texels){
		x->construct();
	}
	
//	set_elastic_tabs(layen);
	set_elastic_tabs(layen, texels);
	vector <texel_c*> texs;
	texs.push_back(new texel_row_c);
	texs.back()->construct();
	int penX =  0;
	int penY = 0;	
	int line_width = layen.line_width;
	int c_x = 0;
	int c_w = 0;
	int c_column = 0;
	int c_n = 0;
	int texels_size = texels.size();
	for (int n = 0; n < texels_size;){
		texel_c& tex = *texels[n];
		if (tex.is_new_line()){
			for (; c_n <= n; ++c_n){
				texs.push_back(texels[c_n]);
			}
			++n;
//			assert(c_n == n);
			texs.push_back(new texel_row_c(1,layen.shared_map ));
			texs.back()->construct();
			c_x = 0;
			penX = 0;
			c_column = 0;
			continue;
		}
		if (! tex.is_caret()){
			c_x += tex.advance_x();
			c_column ++;
		}
		if (c_x + penX  >= line_width && c_column != 0){
			texs.push_back(new texel_row_c(1,layen.shared_map));	
			texs.back()->construct();
			for (; c_n <= n; ++c_n){
				texs.push_back(texels[c_n]);
			}
			++n;
//			assert(c_n == n);
			penX = c_x;
			c_x = 0;
			c_column = 0;
		}
		else {
			if (! tex.is_word_part()){
				penX += c_x;
				c_x = 0;
				for (; c_n <= n; ++c_n){
					texs.push_back( texels[c_n]);
				}
				++n;
			}
			else {
				++n;
				if (n == texels_size){
					for (;c_n < n; ++c_n) {
						texs.push_back(texels[c_n]);
					}
				}
			}
		}
	}
	texels = texs;	
}

void font_c::engrave ( layen_c& layen )
{
	layen_c& ge = layen;
	z = layen.font_zoom;
	auto& texels = ge.texels;
	ge.is_checked = false;	
	auto it = texels.begin();
	if (layen.caret){
		ge.is_selected = true;
		texel_caret_c& caret = *layen.caret;
		ge.min_y = ge.penY = 0;
		ge.max_y = cell*z*(layen.frame_height);
		auto i = caret.iterator(); 
		int scroll_up = caret.info.scroll_up;
		for (--i; i != texels.begin();--i){
			if ((*i)->is_selector()){
				ge.is_selected = false;
			}
			else if ( (*i)->is_row()){
				if (scroll_up == 0){
					break;
				}
				--scroll_up;	
			}
		}
		it = i;
		if (ge.is_selected){
			bool is_selector_leading;
			caret.selector(&is_selector_leading);
			ge.is_selected = !is_selector_leading;
		}
	}
	
	for (; it != texels.end(); it++){
		if ( ge.penY >= ge.max_y ){
			break;
		}
		texel_c& tex = **it;
		if ( tex.is_row() ) {
			ge.penX = 0;
			ge.penY += cell*z;
			continue;
		}
		if ( tex.is_selector() ) {
			ge.is_selected = ! ge.is_selected;
		}
		if ( ge.penY < ge.min_y ) {
			continue;
		}

		tex.draw ( ge );
		ge.penX += tex.advance_x();
		ge.penY += tex.advance_y();
	}
}
//             
font_c::font_c ()
{
}

font_c::~font_c () 
{
}