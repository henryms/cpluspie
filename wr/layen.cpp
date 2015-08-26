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
#include <cassert>
#include <unordered_set>
#include <functional>

#include <regex>
#include <iterator>
#include <iterator>
#include <locale>


using namespace std;


#include "debug.h"

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

#include "global.h"
#include "echo.h"
#include "object.h"
#include "line.h"
#include "spline.h"
#include "keyb.h"
#include "wr/font.h"
#include "wr/texel.h"
#include "wr/shared.h"
#include "layen.h"

uint32_t _get_utf322(string&s, string::iterator& c)
{
	uint32_t lc  = *c;
	c++;
	if ((lc&0x80 ) == 0 ){
		return lc;
	}
	if ((lc&0xE0 ) == 0xC0 ){
		long l2 = *c;
		c++;
		lc = lc & 0x1F;
		lc= lc << 6;
		l2 = l2 & 0x3F;
		lc = lc | l2;
		return lc;
	}
	if ((lc&0xF0 ) == 0xE0 ){
		long l2 = *c;
		c++;
		long l3 = *c;
		c++;
		lc = lc & 0x0F;
		lc= lc << 12;
		l2 = l2 & 0x3F;
		l2= l2 << 6;
		l3= l3 & 0x3F;
		lc = lc | l2 | l3;
		return lc;
	}
	if ( lc&0xF8 == 0xF0 ){
		long l2 = *c;
		c++;
		long l3 = *c;
		c++;
		long l4 = *c;
		c++;
		return 0;
	}
	echo << "err" << endl;
	return lc;
}

long _get_utf32 ( string& s, int& c )
{
	long lc = s.at(c);
	c++;
	if ((lc&0x80 ) == 0 ){
		return lc;
	}
	if ((lc&0xE0 ) == 0xC0 ){
		long l2 = s.at ( c );
		c++;
		lc = lc & 0x1F;
		lc= lc << 6;
		l2 = l2 & 0x3F;
		lc = lc | l2;
		return lc;
	}
	if ((lc&0xF0 ) == 0xE0 ){
		long l2 = s.at ( c );
		c++;
		long l3 = s.at ( c );
		c++;
		lc = lc & 0x0F;
		lc= lc << 12;
		l2 = l2 & 0x3F;
		l2= l2 << 6;
		l3= l3 & 0x3F;
		lc = lc | l2 | l3;
		return lc;
	}
	if ( lc&0xF8 == 0xF0 ){
		long l2 = s.at ( c );
		c++;
		long l3 = s.at ( c );
		c++;
		long l4 = s.at ( c );
		c++;
		return 0;
	}
	echo << "err" << endl;
	return lc;
}

void layen_c::dump ( string s, stringstream& ss ) 
{

	if ( s == "" ) {					
		auto tc = get_caret();						
		
		if ( tc == nullptr ) {
			return;
		}				
		auto it = tc->iterator();
		int 	post = 8,
			deep = 16;			
		for (int c = 1; it!=texels.begin() && c <= post; it--, c++ ){} 
		for ( int c = 1; it!= texels.end() && c <= deep; it++,c++ ) {			
			(*it)->dump( ss );
		}		
		if ( it == texels.end() ) {
			ss << "eof\n";
		}		
		return;
	}	
				
	int	c = 0,
		deep = 15; 	for (  ; c < deep  && c < texels.size() ; c++) {
		texels.at( c)-> dump(ss);
	}
	if ( texels.size()  <= deep ) {
		ss << "eof\n";
	}	
}

void layen_c::remove_rows (vector<texel_c*>* txs)
{
	if( txs == nullptr){
		txs = & texels;
	}
	vector<texel_c*> texs;
	for ( auto it=txs->begin() ; it != txs->end(); ) {
		if ( (*it)->is_row () ) {
			assert ( (*it)->reference == 1 );
			delete *it;
		}
		else {
			texs.push_back(*it);
		}
		it ++;
	}
	*txs = texs;
	invalide_caret();
}

void layen_c::clear_glyph_bitmap ()
{
	for ( auto x : texels ) {
		x->clear_glyph_bitmap ();
	}
}

void layen_c::clear_pixel_vectors ()
{
	for(auto x : shared_map.char_map){
		for(auto y : x.second){
			y.second->clear_pixel_vectors();
		}
	}
}

void layen_c::clear_texel_map ()
{
	mout << "clear texel map\n";
	remove_rows();
	texel_caret_c* tc = get_caret ();
//	assert ( tc );
	int position;
	if(tc){
		position = tc->get_position ();
	}

	string s = text ( 1 , -1);
	for ( auto x : texels ) {
		x->clear_glyph_bitmap();
	}
	for ( auto x : texels ) {
		if (x->dec_reference() == 0 ){
			delete x;
		}
	}
	auto it = shared_map.char_map.begin();
	for (; it != shared_map.char_map.end();){
		auto it2 = it->second.begin();
		
		for(; it2 != it->second.end();){
			if(it2->second->dec_reference() == 0){
				delete it2->second;	
				it2 = it->second.erase(it2);
				continue;
			}
			++it2;
		}
		if(it->second.empty()){
			it = shared_map.char_map.erase(it);	
			continue;
		}
		++it;
	}
	texels.clear();
	set_text(s);
	if(tc){
		set_caret();
		get_caret()->move_to ( position );
	}
}

texel_caret_c* layen_c::get_caret ()
{
	if ( caret != nullptr){
		return caret;
	}
	for ( auto x : texels ) {
		if ( x->is_caret() ) {
			caret = static_cast<texel_caret_c*>(x);
			return caret;
		}
	}	
	return nullptr;
}

layen_c::layen_c():
	signo{0},
	scroll_right(0),
	scroll_down(0),
	first_invalide_row(1),
	last_invalide_row(-1),	
	x(0),
	y(0),
	penX(0),
	penY(0),
	is_checked(false),
	is_selected(false),
	is_collapsed(false),
	min_x(0),
	max_x(0),
	min_y(0),
	max_y(0),
	color(0x00),
	line_width(0),
	zoom(1),
	pi(nullptr),
	si(0),
	he(0),
	wi(0)
{
	face = font_c::system_face;
}

layen_c::~layen_c()
{
	for (auto x: texels) {
		if (x->dec_reference()==0 )
			delete x;
	}
	delete pi;
}

string layen_c::copy_text()
{
	auto& caret = *get_caret();
	auto it = caret.iterator();	
	bool is_ahead;
	auto is = caret.selector(&is_ahead);
	if(!is_ahead)
		swap(it, is);	
	string s;
	for(;it != is; ++it)
		if((*it)->is_text())
			s += utf32_to_utf8((*it)->get_char());
	return s;		
}

string layen_c::cut_text()
{
	string s = copy_text();
	erase_text();		
	return s;
}

void layen_c::erase_text()
{
	string s;
	replace_text(s);
}

void layen_c::replace_text(string& s)
{
	auto pcaret = get_caret();
	if(caret == nullptr){
		return;		
	}
	auto& caret = *pcaret;
	
	vector<texel_c*> txs;
	for(int c = 0; c < s.size();){
		uint32_t chr = _get_utf32(s, c);
		auto p = texel_char_c::create(chr, face, 0, shared_map);
		txs.push_back(p);
	}
	bool is_ahead;
	auto it = caret.iterator(),
	its = it; 
	its = caret.selector(&is_ahead);	
	if(not is_ahead)
		swap(it, its);
	auto itb = it;
	assert((*itb)->is_selector());

	for(int c = 10; itb != texels.begin(); --itb){
		if((*itb)->is_row()){
			--c;
			if(c == 0)
				break;
		}
	}	
	auto ite = its;
	for(int c = 10; ite != texels.end(); ++ite){
		if((*ite)->is_new_line()){
			--c;
			if(c == 0)
				break;
		}	
	}
	int scroll_count = 0;
	for(auto i = itb;; ++i){
		if((*i)->is_caret())
			break;	
		if((*i)->is_row())
			++scroll_count;
	}
	vector<texel_c*> txs0;
	auto i = itb;	
	for(;; ++i){
		if((*i)->is_row())
			delete *i;
		else 
			txs0.push_back(*i);
		if(i == it)
			break;
	}
	++i;
	for(; i != its; ++i)
		if(!(*i)->dec_reference())
			delete *i;
	for(;i != ite; ++i)
		if((*i)->is_row())
			delete *i;
		else
			txs.push_back(*i);
	txs.insert(txs.begin(), txs0.begin(), txs0.end());	
	font.set_rows(*this, &txs);		

	int sa = txs.size();
	int sb = 0;
	int scroll_countb = 0;
	for(i = itb; i != ite; ++i){
		++sb;	
	}
	for(i = txs.begin(); !(*i)->is_caret(); ++i)
		if((*i)->is_row())
			++scroll_countb;
	caret.info.scroll_up += scroll_countb -scroll_count;
	i = itb;
	auto i2 = txs.begin();
	for(; i != ite && i2 != txs.end();)
		*i++ = *i2++;	
//	cout << "sa:"<< sa << "  sb:"<< sb << endl;
	
	if (sa < sb){
//		assert(i != ite);
		i = texels.erase(i, next(i, sb - sa));
		--i;
//		cout << "sa < sb\n";
	}
	else{
		it = i = texels.insert(i, i2, txs.end());	
		cout << "sa >= sb\n";
		for(int c = 0;  c < sa-sb; ++c, ++it)
			if((*it)->is_caret()){
				caret.info.iterator = it;
				return;
			}	
	}
//	cout << "xxx\n";
	for(int c = 0; c < sa; ++c, --i){
		cout << c << endl;
		if((*i)->is_caret()){
			caret.info.iterator = i;
			cout << "caret found\n";
			return;
		}	
	}
	cout << "caret not found\n";
}

string layen_c::text( int a , int b)
{
	string s;
	int c = 1;
	for ( auto t :texels ) {
		if ( c >= b && b != -1 ) {
			break;
		} 
		if ( t->is_text() ) {
			if ( c >= a ){		
				s+= utf32_to_utf8 ( t->get_char() );
			}
			c++;
		}
	}
	return s;
}

void layen_c::set_text2(string& s, vector<texel_c*>* pt)
{
	if ( pt == nullptr ) 
		pt = &texels;
	
	for ( auto x : *pt ) 
		if ( x->dec_reference() == 0 )
			delete x;
	pt->clear ();
	
	auto ie = s.end();
	array<texel_c*, 65536> a;
	a.fill(nullptr);
	texel_c* p;
	for (auto i = s.begin(); i != ie;){
		long chr = _get_utf322(s, i);
		if(chr >= 0x0f)
			if(a[chr] != nullptr){
				p = a[chr];	
				++p->reference;
			}
			else
				p = a[chr] = texel_char_c::create(chr, face, 0, shared_map);
		else
			p = texel_char_c::create(chr, face, 0, shared_map);
		pt->push_back(p);
	}
}


void layen_c::set_text(string& s, vector<texel_c*>* pt) 
{
	if(s.size() > 10000){
		set_text2(s, pt);
		return;
	}
	if ( pt == nullptr ) 
		pt = &texels;
	for ( auto x : *pt ) 
		if ( x->dec_reference() == 0 )
			delete x;
	pt->clear ();
/*	
	auto ie = s.end();
	for (auto i = s.begin(); i != ie;){
		long chr = _get_utf322(s, i);
		texel_c* p = texel_char_c::create(chr, face, 0, shared_map);
		pt->push_back(p);
	}
*/	
//	cout << "huu\n";		
	for(int c = 0; c<s.size ();){
		long chr = _get_utf32(s, c);
		texel_c* p = texel_char_c::create(chr, face, 0, shared_map);
		pt->push_back(p);
	}
	
}

int find_next_bracket(vector<texel_c*>& texels, vector<texel_c*>::iterator& it, size_t& size, bool skeep_return)
{
	int result = 0;
	size = 0;
	vector<texel_c*>::iterator it2;
	int comment = 0;
	for (;it!= texels.end ();it++){
		unsigned long ch=(*it )->get_char ();
		if ( comment == 0 ){
			if ( ch == '/'){
				comment = 1;
			}
		}
		else if (comment == 1 ){
			if ( ch == '/'){
				comment = 2;
			}
			else if ( ch == '*'){
				comment = 4;
			}
			else {
				comment = 0;
			}
		}
		else if ( comment == 2 ){
			if ( ch == '\n'){
				comment = 0;
			}
		}
		else if (comment == 3 ){
			if ( ch == '/'){
				comment = 0;
			}
			else {
				comment = 4;
			}
		}
		else if ( comment == 4 ){
			if ( ch == '*'){
				comment = 3;
			}
		}
		if ( comment == 0 && ch=='{'){
			it2 = it-1;
			if ( it!= texels.begin () && (*it2 )->get_char () =='\n'){
				size = 2;
				it = it2;
			}
			else{
				size = 1;
			}
			result = 1;
			break;
		}
		if ( comment == 00 && ch=='}'){
			it2 = it+1;
			if ( it2!= texels.end () && (*it2 )->get_char () =='\n'){
				size = 2;
			}
			else{
				size = 1;
			}					
			result=-1;
			break;
		}
	}
	return result;
}

void layen_c::invalide_caret()
{
	auto caret = get_caret();
	if(caret){
		caret->info.valid = false;
	}
}

void layen_c::collapse ()
{
	static list<pair<size_t, size_t>> collapse_list;
	texel_caret_c* tc =  get_caret ();
	if ( !is_collapsed ){
		is_collapsed = true;
		collapse_list.clear ();
		collapse_texels.clear ();
		
		size_t row = tc->row();
		size_t position = tc->position();
		remove_rows();
		
		tc->move_to(1);
		invalide_caret();		
		vector<texel_c*>::iterator it, itb;
		it = itb = texels.begin ();
		int bracket = 0, prev_bracket = 0;
		pair<size_t, size_t> collapse_block;
		size_t size;
		for (;it!= texels.end ();){
			prev_bracket = bracket;
			bracket+= find_next_bracket ( texels, it, size, false );
			if ((it == texels.end () && bracket == 0 )  || 
			(bracket == 1 && prev_bracket == 0 )){
				if ( itb!= texels.begin ()){
					collapse_block.second = tc->get_position ( itb-1 );
					collapse_list.push_back ( collapse_block );
				}
				for (;it!= itb;itb++){
					collapse_texels.push_back (*itb );
				}
				collapse_block.first = tc->get_position ( it );
				invalide_caret();		
			}
			for (;size>0;--size ){
				it++;
			}
			if ((it == texels.end () && bracket!= 0 ) ||
			(bracket == 0 )){
				itb = it;
			}
		}
		auto itl = collapse_list.begin ();
		size_t pos = position;
		for (;itl!= collapse_list.end ();itl++){
			if ( itl->second > position ){
				if ( itl->first < position ){
					pos-= position-itl->first;
				}
				break;
			}
			pos-= itl->second-itl->first+1;
		}
		vector<texel_c*> texba;
		texba = texels;
		texels = collapse_texels;
		collapse_texels = texba;
		tc->move_to ( pos );
		invalide_caret();
		size_t row2 = tc->row();
//		dump_collapse ( collapse_list );
	}
	else{
		is_collapsed = false;
		invalide_caret();
		size_t row = tc->row(),
		pos = tc->position();
		list<pair<size_t, size_t>>::iterator itl;
		itl = collapse_list.begin ();
		for (;itl!= collapse_list.end ();itl++){
			if ( itl->first >pos ){
				break;
			}
			pos+= itl->second-itl->first+1;
		}
		texels = collapse_texels;
		invalide_caret();
		tc->move_to ( pos );
		invalide_caret();
		size_t row2 = tc->row();
	}
		
}

void layen_c::resize ( int _width, int _height, int frame_height_ ) 
{
	frame_height = frame_height_;
	delete pi;
	wi = _width;
	he  = _height;
	si = wi*he;
	pi = new uint32_t [_width*_height];
	for ( int c = 0 ; c < si; c++ ) {
		*pi = 0x00ffffff;
	}
}

void layen_c::delete_caret ()
{
	for ( auto it=texels.begin() ; it != texels.end() ; ) {
		if ( (*it)->is_caret () ) {
			assert((*it)->reference == 1);
			delete *it;
			it = texels.erase(it );
			continue;
		}
		if  ( (*it)->is_selector() ) {
			assert((*it)->reference == 1);
			delete *it;
			it = texels.erase(it);
			continue;
		}
		it++;
	}	
	caret = nullptr;
}

void layen_c::set_caret ()
{
	delete_caret();
	auto p = new texel_caret_c;
	p->texels =&texels;
	p->layen = this;	
	texels.insert( texels.begin(), p );
	texels.insert( texels.begin(), new texel_selector_c);
	p->info.selector_position = 1;
	caret = p;
}
