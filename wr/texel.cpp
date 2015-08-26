#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "debug.h"

#include "symbol/keysym.h"
#include "symbol.h"
#include "library/wdslib.h"

#include "mstream.h"
#include "matrix.h"
#include "position.h"
#include "sens/retina.h"
#include "sens/spirit.h"
#include "global.h"
#include "font.h"
#include "texel.h"
#include "shared.h"
#include "layen.h"

void ex_entry_texel(stringstream&);

void row_representation_c::set(size_t _row)
{
	row = 0;		
	int pos = 1;				
	auto it = caret.layen->texels.begin();
	for(; it != caret.layen->texels.end(); it++){
		if((*it)->is_row()){
			row++;				
			if(row <= _row){				
				position = pos;
			}				
			else{				
				break;
			}
		}
		else if((*it)->is_text()){
			pos++;
		}
	}
	row_size = pos - position;
}

bool caret_representation_c::check()
{
	if ( ! d.caret_check){
		return true;
	}
	stringstream ss;
	auto it = caret.texels->begin();
	if ( it != caret.texels->end() &&  !(*it)->is_row()){
		cout << "err texs! not row at begin\n";
		return false;
	}
	int sposition{1};
	int srow_position{1};
	int srow{0};
	bool scaret{false};
	bool row_trigger{false};
	bool result = true;	
	for(;it != caret.texels->end();it++){
		if((*it)->is_text()){
			++sposition;
		}
		else if ( (*it)->is_row()){
			if( row_trigger){
				cout << "err texs! orphan row "<< '\n';
				cout << "at row: " << srow << '\n';
			}
			row_trigger = true;
			if ( scaret ){
				if( srow_position != row_position){
					cout << "err texs! false row_position \n";
					result = false;
					}
				if ( sposition - srow_position != row_size){
					cout << "err texs! false row_size\n";
					result = false;
				}
				if(srow != row){
					cout << "err texs! false row\n";
				cout << row << " instead of " <<  srow << '\n';

					result = false;
				}
				scaret = false;
			}
			srow_position = sposition;
			++srow;
			continue;	
		}
		else if ((*it)->is_caret()){
			if( it != iterator){
				cout << "caret check iterator err\n";	
				result = false;
			}
			if (sposition != position){
				cout << "caret check position err\n";	
				result = false;
			}
			scaret = true;
		}
		row_trigger = false;
	}	
	result = true;
	return result;
}

void caret_representation_c::set(bool force)
{
	if (valid && d.f1 == 0 && !force){
		return;
	}
	static int c = 0;
	if( caret.layen->signo == 12){
		mout << "caret info: " << c++ << '\n';
	}
	row = 0;
	int pos = 1;
	int trigger = 0;
	auto it = caret.layen->texels.begin();
	selector_position = 0;
	auto ie = caret.layen->texels.end();
	for (; it != ie; ++it){
		if((*it)->is_text()){
			++pos;
		}
		else if ( (*it)->is_row() ) {
			if ( trigger == 2 ) {
				break;
			}
			row++;
			row_position = pos;
		}		
		else if (  (*it)->is_caret () ) {
			column = pos - row_position + 1;
			iterator = it;
			position = pos;
			++trigger;
		}						
		else if ((*it)->is_selector()){
			if( ! (*it)->is_caret()){
				selector_position = pos;
				++trigger;
			}
			
		}
		/*
		else if ( (*it)->is_text() ) {
			pos++;				
		}
		*/
	}	
	row_size = pos - row_position;
	assert (trigger);
	assert(selector_position);
	if ( trigger ) {
		valid = true;
	}
}

void caret_representation_c::dump()
{
	if (1){
		int px =  position;
		cout 
		<< "row: " << row << '\n' 
		<< "row_position: " << row_position << '\n'
		<< "row_size: " << row_size << '\n'
		<< "position: " << px << '\n';
		echo << "position: " << px << '\n';
		echo << "row: " << row << '\n'; 
		echo << "row_position: " << row_position << '\n';
		echo << "row_size: " << row_size << '\n';
		echo << "scroll_up: " << scroll_up << '\n';
	}
}

void texel_caret_c::dump ( stringstream& ss ) 
{
	ss << "caret\n";
}

texel_caret_c::texel_caret_c():info(*this),row_info(*this)
{
	graphies = new graphies_c;
	in_selector_mode = false;
	column_memory = 1;
	trailing_caret = true;
}

texel_caret_c::~texel_caret_c()
{
	assert(layen->caret !=nullptr);
	if ( layen->signo == 12){
		cout << "caret destruction\n";
	}
	layen->caret = nullptr;
}

void texel_caret_c::next_word ( )
{
	enum {
	word_part =1,  
	punctuation,
	space 
	};
	int trigger = 0,
	count = 0;
	for ( auto it = iterator(); it != texels->end(); it++ ) {
		if ( (*it)->is_text() ) {
			if ( (*it)->is_word_part () ) {
				if ( trigger != 0 && trigger != word_part ) {
					break;
				}
				trigger = word_part;
			}
			else if ( (*it)->is_punct() ) {
				if ( trigger != 0 && trigger != punctuation ) {
					break;
				}
				trigger = punctuation;
			}
			else {
				trigger = space;
			}
			count++;				
		}
	}
	to_right(count);
}

void texel_caret_c::next_word_end ()
{
	to_right(1);
	int trigger = 0,
		count = -1;
	auto it = iterator();
	for ( ; it != texels->end(); it++ ) {
		if ( (*it)->is_text()  ){
			if (  (*it)->is_word_part() ) {
				trigger = 1;
			}
			else  {
				if ( trigger == 1 ) {
					break;
				}
			}
			count++;
		}
	}
	if ( count > 0 ) {
		to_right(count);
	}
}

void texel_caret_c::previous_word ( )
{
	int trigger = 0,
		count = 0;
	auto it = iterator();
	for ( ; it != texels->begin(); it-- ) {
		if ( (*it)->is_text()  ){
			if (  (*it)->is_word_part() ) {
				trigger = 1;
			}
			else  {
				if ( trigger == 1 ) {
					break;
				}
			}
			count++;
		}
	}
	to_left(count);
}

bool texel_caret_c::is_line_end ()
{
	auto it = ++iterator();
	if ( it != texels->end() && (*it)->is_new_line()){
		return true;
	}
	return false;
}

bool texel_caret_c::is_eof ()
{
	if(++iterator() != texels->end()){
		return false;
	}
	return true;
}

size_t texel_caret_c::move_selector_to ( size_t position )
{
	texel_c* sel = nullptr;
 	auto it = texels->begin();
	for (; it != texels->end(); it++ ) {
		if ( (*it)->is_selector() && !(*it)->is_caret() ){
			sel = *it;
			it = texels->erase ( it );
			break;
		}
	}
	assert ( sel );
	int px = 1;
	for ( it = texels->begin(); it != texels->end(); it++ ) {
		if ( (*it)->is_text ()) {
			if ( px == position ) {
				break;
			}
			px++;
		}
	}
	texels->insert ( it, sel );
	info.valid = false;
	info.set();
}

size_t texel_caret_c::move_to2(size_t position)
{
//	assert(texels->front()->is_row());
	size_t pos = get_position ();	
	if ( pos > position){
		to_left ( pos - position );
	}
	else {
		to_right ( position - pos);
	}
}

size_t texel_caret_c::move_to ( size_t position )
{
	info.valid = false;
	size_t pos = get_position ();	
	if (texels->front()->is_row()){
		move_to2(position);
		return 0;
	}
	int delta= position-pos;	
	offset ( delta );
}

vector<texel_c*>::iterator texel_caret_c::iterator(int pos)
{
	if (pos == 0){
		if (info.valid)
			return info.iterator;
		for (auto it = texels->begin(); it != texels->end(); ++it){
			if ((*it)->is_caret()){
				info.iterator = it;
				return it;
			}
		}
	}
	else {
		for (auto it = texels->begin(); it != texels->end(); ++it){
			if ((*it)->is_text()){
				--pos;
				if (pos == 0)
					return it;
			}
		}
	}
	return texels->end();
}

size_t texel_caret_c::position()
{
	int c = 1;
	for (auto x : *texels){
		if (x->is_text()){
			++c;
		}
		else if (x->is_caret()){
			break;
		}
	}	
	return c;
}

size_t texel_caret_c::row()
{
	size_t row = 0;
	for (auto x : *texels){
		if (x->is_row())
			++row;
		else if (x == this)
			return row;
	}
}

size_t texel_caret_c::line()
{
	size_t line = 1;
	for (auto x : *texels){
		if (x == this)
			return line;	
		if (x->is_new_line())
			++line;	
	}	
}

int texel_caret_c::line_length()
{
	auto it = iterator();
	int length{};
	for (;; --it){
		if ((*it)->is_new_line())
			break;
		if ((*it)->is_text())
			++length;
		if (it == texels->begin())
			break;	
	}
	for (++it = iterator(); it != texels->end(); ++it){
		if ((*it)->is_text())
			++length;
		if ((*it)->is_new_line())
			break;
	}	
	return length;
}

size_t texel_caret_c::position_in_line()
{
	int pil = 1;
	auto it = iterator();
	for (;; -- it){
		if ((*it)->is_new_line() || it == texels->begin())
			return pil;
		else if ((*it)->is_text())
			++pil;	
	}	
}

int texel_caret_c::scroll_up()
{
	return info.scroll_up;
}

//sophie michaud
void texel_caret_c::move_to_line(size_t line_, size_t pil_)
{
	auto ic = iterator(),
	it = ic;
	size_t cpil = position_in_line();
	int cline = line();	
	cout << "move to line: " << cline <<":" << cpil << endl;
//	return;

	if (cline > line_ || (cline == line_ && cpil > pil_)){
		for (int d = 0;; --it, --d){
			if ((*it)->is_new_line())
				--cline;
			if (cline == line_ -1 || it == texels->begin()){
				cpil = pil_;
				for (; it != texels->end(); ++it, ++d){
					if ((*it)->is_text()){
						if (--cpil == 0){
							move_from_to(ic, d);
							info.line = line_;
							info.position_in_line = pil_;
							return;
						}
					}
				}	
			}
		}
	}
	else if (cline != line_ ||  cpil < pil_){
		for (int d = 0; it != texels->end(); ++it, ++d){ 
			if (cline == line_){
				for (;; ++it, ++d){
					if (cpil == pil_){
						move_from_to(ic, d);
						info.line = line_;
						info.position_in_line = pil_;
						return;
					}	
					if((*it)->is_text())
						++cpil;
				}
			}	
			if ((*it)->is_new_line()){
				++cline;
				cpil = 1;	
			}
		}
	}
}

int texel_caret_c::scroll_up(int d)
{
	auto it = iterator();
	assert(it != texels->end());
	if ( d > info.scroll_up){	
		if (d > 0){
			int c = 0;
			for (;; --it){
				if (it == texels->begin())
					break;
	 			if((*it)->is_row()){
					++c;	
					if ( c >= d)
						break;
				}
			}
			info.scroll_up = min(c , d);
		}	
		else 
			info.scroll_up = d;
	}
	else if (d < info.scroll_up){
		if (d - (layen->frame_height-1) < 0){
			int c = 0;
			for (; it != texels->end(); ++it){
				if ((*it)->is_row()){
					--c;
					if (c < d -(layen->frame_height-1))
						break;
				}	
			}
			int dd = max(c + (layen->frame_height-2), d);
			if (dd < info.scroll_up){
				info.scroll_up = dd;	
			}
		} 
		else 
			info.scroll_up = d; 
	}
}

void texel_caret_c::set_to(size_t pos, int sd)
{
	move_to(pos);
	auto it = iterator(),
		iti = it;
	int c = 0;
	for (;it != texels->end(); --it){
		if ((*it)->is_new_line()){
			--c;
		}
		if (c < sd ){
			++it;
			break;
		}
		if ( it == texels->begin()){
			break;
		}
	}	
	auto itbeg = it;
	it = iti;
	c = 0;
	cout << "frame_height: " << layen->frame_height << '\n';
	for (; it != texels->end(); ++it){
		if ((*it)->is_new_line()){
			++c;	
		}
		if ( c >= layen->frame_height +sd){
			break;
		}
	}
	
	auto itend = it;	
	layen->font.set_rows(*layen, *texels, itbeg, itend);	
}

void texel_caret_c::move_from_to( vector<texel_c*>::iterator from, int to ) 
{
	assert ( from != texels->end());
	assert ( to);
	assert (from != texels->begin());
	
	auto it = from;
	auto caret = *from;
	texel_c* selector = nullptr;
	
	if ( (*(it-1))->is_selector()){
		selector = *(it-1);
		it = texels->erase(it-1,it+1);
	}
	else{
		it = texels->erase(it);
	}
	if(to > 0 ){
		--to;
	}	
	else if (selector){
		++to;
	}
	advance(it, to);
	if ( selector){
		info.iterator = ++texels->insert(it, {selector, caret});
		info.selector_position = info.position;
	}
	else if ((*(it-1))->is_selector()){
		info.iterator = texels->insert(--it, caret);						
	}
	else{
		info.iterator = texels->insert(it, caret);
	}
	info.check();
}

void texel_caret_c::right(vector<texel_c*>::iterator it)
{
	auto ic = iterator();	
	if (it == texels->end())
		;//--it;	
	if (it == ic || it == ic+1)
		return;
	if ((*(ic-1))->is_selector()){
		auto selector = *(ic-1);
		for (auto i = ic+1; it != i; ++i){
			if ((*i)->is_row())
				++info.scroll_up;
			*(i-2) = *i;
		}	
		*(it-1) = this;
		*(it-2) = selector;
		info.iterator = it-1;
	}
	else{
		for (auto i = ic+1; i != it; ++i){
			if ((*i)->is_row())
				++info.scroll_up;
			*(i-1) = *i;	
		}	
		*(it-1) = this;		
		if ((*(it-2))->is_selector()){
			swap(*(it-1), *(it-2));
			info.iterator = it-2;	
		}
		else 
			info.iterator = it-1; 
	}
}

void texel_caret_c::left(vector<texel_c*>::iterator it)
{
	auto ic = iterator();
	if (ic == texels->begin())
		return;
	if ((*(ic-1))->is_selector()){
		auto selector = *(ic-1);
		for (auto i = ic-2;; --i){
			if ((*(i))->is_row())
				--info.scroll_up;
			*(i+2) = *i;	
			if (i == it)
				break;
		}		
		*it = selector;
		*++it = this;
		info.iterator = it;
	}
	else{
		for (auto i = ic-1;; --i){
			if ((*(i))->is_row())
				--info.scroll_up;
			*(i+1) = *i;
			if (i == it)
				break;	
		}	
		*it = this;
		if (it != texels->begin() && (*(it-1))->is_selector()){
			swap(*it, *(it-1));
			info.iterator = it -1;
		}
		else
			info.iterator = it;
	}	
}


void texel_caret_c::to_file_begin()
{
	auto it = texels->begin();
	for (; it != texels->end() && !(*it)->is_text(); ++it)
		if ( it == iterator())
			return;
	left(it);		

//	left(++texels->begin());
	
}

void texel_caret_c::to_file_end()
{
	right(texels->end());
	return;
	auto i = texels->begin();
	advance(i, 3);
	right(i);
	return;
	right(++texels->begin());
}

void texel_caret_c::to_line_end()
{
	auto it = iterator();
	for (; it != texels->end(); ++it){
		if ((*it)->is_new_line())
			break;				
	}	
	right(it);
}

void texel_caret_c::to_line_begin()
{
	auto it = iterator();
	for (;; --it){
		if ((*it)->is_new_line()){
			++it;
			break;
		}
		if ( it == texels->begin())
			break;
	}	
	for (; it != texels->end() && !(*it)->is_text(); ++it)
		if ( it == iterator())
			return;
	left(it);		
}

void texel_caret_c::to_right(int d)
{
	assert(texels->front()->is_row());
	assert(info.iterator != texels->end());	
//	info.set();
	
	if (d ==0){
		return;
	}	
	
	int cx = 0;
	auto it = info.iterator;
	for(; it != texels->end();it++, cx++){
		if ((*it)->is_text()){
			--d;
			if (d == -1){
				break;
			}		
			++info.position;
		}
		else if ( (*it)->is_row()){
			info.row_position = info.position;
			info.row++;
			++info.scroll_up;
		}
	}
	info.row_size = info.position - info.row_position;
	for (; it != texels->end() &&  !(*it)->is_row(); it++){
		if ((*it)->is_text()){
			info.row_size ++;
		}
	}
	move_from_to(info.iterator, cx);
//	assert(info.check());
}

void texel_caret_c::to_left(int d)
{
	assert(info.iterator != texels->begin());
	assert(texels->front()->is_row());
	if ( d == 0 ){
		return;
	}
	info.set();
		
	int cx = 0;
	size_t last_row_position = info.row_position + info.row_size;
	auto it = info.iterator;
	for (;;){
		it--, cx++;	
		if ( (*it)->is_text()){
			info.position--;
			d--;
			if ( d == 0 ){
				info.row_position = info.position;
				for (; !(*--it)->is_row();){
					if ( (*it)->is_text()){
						info.row_position--;
					}
				} 
				info.row_size = last_row_position - info.row_position;
				break;
			}
		}
		else if ((*it)->is_row()){
		 	if ( it == texels->begin()){
				for(; it != texels->end() && (!(*it)->is_text()); ++it, --cx);
				break;
			}
			last_row_position = info.position;						
			info.row--;
			--info.scroll_up;
		}		
	}	
	move_from_to(info.iterator, -cx);
	assert(info.check());
}

void texel_caret_c::to_row_up(int n)
{
	assert (n >0);
	info.set();
	
	auto it = info.iterator;
	int cx = 0;	
	info.row_position = info.row_position + info.row_size;
	for (;; --it, ++cx){
		if ( (*it)->is_text()){
			--info.position;
		}
		else if ((*it)->is_row()){
			--n;
			if( n == -1 || it == texels->begin()){
				info.row_size = info.row_position - info.position; 
				info.row_position = info.position;
				break;
			}
			--info.scroll_up;
			--info.row;
			info.row_position = info.position;
		}
	}
	info.position = info.row_position + info.row_size -1;
	if ( info.row_position + column_memory -1 < info.position){
		info.position = info.row_position + column_memory -1;
	}
	for (;it != texels->end() && !(*it)->is_text();it++, cx--);
	for(int c = info.position - info.row_position; c > 0; it++, cx--){
		if ( (*it)->is_text()){
			c--;
		}
	}
	move_from_to(info.iterator, -cx);
	assert(info.check());
}

void texel_caret_c::to_row_down(int d)
{
	info.set();
	int cx = 0;	
	auto it = info.iterator;
	for (;; ++it, ++cx){
		if (it == texels->end()){
			info.row_size = info.position - info.row_position;
			break;
		}
		else if ( (*it)->is_row()){
			--d;
			if ( d == -1) {
				info.row_size = info.position - info.row_position;
				if (info.row_size != 1 ){
					info.position -= 1;
				}
				break;
			}
			info.row_position = info.position;
			++info.scroll_up;
			++info.row;
		}
		else if ((*it)->is_text()){
			++info.position;
		}
	}
	if ( layen->signo == 12 ) {
//		info.dump();
	}
	
	if ( info.row_position + column_memory <= info.position){
		info.position = info.row_position + column_memory -1;
	}
	cx -= info.row_position + info.row_size - info.position;

	move_from_to(info.iterator, cx);
	assert(info.check());
}

size_t texel_caret_c::offset(int delta)
{
	auto it = iterator();
	if ( it == texels->end() ){
		return 0;
	}
	texel_c* sel = nullptr;
	it = texels->erase ( it );
	
	if ( it != texels->begin() && (* (it-1))->is_selector () ) {
		sel = *--it;
		it = texels->erase (it );
	}
	if ( delta<0 ){
		if ( it != texels->begin() ) {
			--it;
		}
		for (;it!= texels->begin ();it--){
			if ( (*it)->is_text() ) {
				delta++;
				if ( delta == 0 ) {
					break;
				}
			}
		}
		for (;  it != texels->end() && ! (*it)->is_text(); ) {
			it++;
		}
	}
	else{
		for (;it !=texels->end(); it++ ) {
			if ( (*it)->is_text() ) {
				delta--;
				if ( delta < 0 ) {
					break;
				}
			}
		}
	}
	if ( sel != nullptr ) {
		it = texels->insert ( it,this );
		texels->insert ( it, sel );
		info.valid = false;
		
		return 0;
	}
	if ( it != texels->begin() && (*(it-1))->is_selector () ) {
		it--;
	}
	info.valid = false;
	it = texels->insert ( it,this );
	info.valid = false;
}

void texel_caret_c::swap_selector()
{
	if (is_selector_binded())
		return;
	bool is_leading;
	auto is = selector(&is_leading),
	ic = iterator(),
	it = ic;
	if (is_leading){
		for (; it != is; ++it)
			if ((*it)->is_row())
				++info.scroll_up;
	}
	else{
		for (; it != is; --it)
			if ((*it)->is_row())
				--info.scroll_up;
	}			
	swap(*is, *ic);	
	info.iterator = is;
}

void texel_caret_c::unbind_selector ()
{
	auto it = iterator();
	if ( it != texels->end() &&  (*(it-1))->is_selector() ) {
		swap(*(it-1),*it);
		--info.iterator;
	}
}

void texel_caret_c::bind_selector()
{
	auto it = iterator();
	if( is_selector_binded()){
		return;
	}
	bool is_leading;	
	auto is = selector(&is_leading);	
	if(is_leading){
		rotate(it, is, is+1);
		++info.iterator;
	}
	else
		rotate(is, is+1, it);
}

bool texel_caret_c::is_selector_binded ()
{
	auto it = iterator();
	if ( it!=texels->end() ){
		it--;
		if ( it!=texels->begin() &&  (*it)->is_selector () ) {
			return true;
		}
	}
	return false;
}

size_t texel_caret_c::get_row ( int No, size_t* pos, size_t* size )
{
	if ( ! layen->texels.front()->is_row()) {
		layen->font.set_rows(*layen);
	}	
	assert(No == 0);	
	info.set();
	if ( pos!= 0 ){
		*pos = info.row_position;
	}
	if ( size!= 0 ){
		*size = info.row_size;
	}
	size_t row = info.row;
	return row;
}

size_t texel_caret_c::get_line (  size_t* pos, size_t* si )
{
	auto it = texels->begin();
	size_t count = 1;
	size_t position = 1;
	size_t  line_position = 1;
	size_t size = 0;
	bool last_line = false;
	for (;it!= texels->end ();it++){
		if ( (*it)->is_caret () ){
			last_line = true;
			continue;
		}
		else if ( (*it)->is_new_line () ){
			if ( last_line == true ){
				break;
			}
			size = 0;
			position++;
			line_position = position;
			count++;
		}
		else if ( (*it)->is_text () ) {
			size++;
			position++;
		}
	}
	if ( last_line == false ){
		line_position = 0;
		count = 0;
		size = 0;
	}
	if ( pos!= 0 ){
		*pos = line_position;
	}
	if ( si!= 0 ){
		*si = size;
	}
	return count;
}


size_t texel_caret_c::get_position ( vector<texel_c*>::iterator it2 )
{
	auto it = texels->begin ();
	size_t c = 1;
	for (;it!= texels->end ();it++){
		if ( it2 == it ){
			return c;
		}
		if (  ( *it )->is_text ()  ) {
			c++;
		}
	}
}	

void texel_caret_c::move_to_next_of_sort ( string s )
{
	auto it = iterator();
	if ( it == texels->end() ) {
		return;
	}
	int d = 0;
	for ( ; it!=texels->end() ; it++ ) {
		if ( (*it)->is_text() ) {
			d++;
		}
		if ( (*it)->get_char() == s.at(0)) {
			break;
		}
	}
	to_right(d-1);
}

size_t texel_caret_c::get_position ()
{
	if ( !info.valid){
	
		info.set();
	}
	return info.position;
}

size_t texel_caret_c::get_selector_position () 
{
	size_t c = 1;
	for ( auto t : *texels ){
		if ( t->is_text () ) {
			c++;
		}		
		else	if ( t->is_selector() && ! t->is_caret() ){
			return c;
		}
	}
	return 0;
}

vector<texel_c*>::iterator texel_caret_c::selector(bool* leading)
{
	auto it = iterator(),
	itb = it++;
	bool leads = false;	
	int block = 100;
	for (;it != texels->end() or itb != texels->begin();){
		if(it != texels->end()){
			for (int c = block; c > 0 && it != texels->end(); --c, ++it){
				if ((*it)->is_selector()){
					if (leading){
						*leading = true;
					}	
					return it;
				}
			}
		}
		if (itb != texels->begin()){
			for (int c = block;;){
				if ((*itb)->is_selector() && !(*itb)->is_caret()){
					if (leading){
						*leading = false;
					}
					return itb;
				}
				if (texels->begin()== itb || --c == 0){
					break;
				}
				--itb;
			}
		}
	}
	return texels->end();
}

size_t texel_caret_c::get_line_position (int line)
{
	if ( line == 0 ) {
		size_t position = get_position ();
		auto it = iterator ();
		for (;it!= texels->begin ();){
			it--;
			if ((*it )->is_new_line() ){
				break;
			}
			if ( (*it)->is_text() ) {
				position--;
			}
		}
		return position;
	}
	else {
		auto position = 1,
		line_position = 1,
		c_line = 1;
		for ( auto x : *texels ) {
			if ( c_line == line){
				break;
			}  
			if ( x->is_text() ){
				position++;
			}
			if ( x->is_new_line()){
				line_position = position;
				c_line++;
			}
		}
		return line_position;
	}
}

int texel_caret_c::get_leading_tabs ()
{
	int tabs = 0;
	for (auto it = iterator();; --it){
		if ((*it)->is_new_line() || it == texels->begin())
			return tabs;
		if ((*it)->is_row())
			continue;
		if ((*it)->is_tab())
			++tabs;
		else 
			tabs = 0;
	}
}

void texel_caret_c::set_column_memory ()
{
	info.set();
	column_memory = info.position - info.row_position + 1;
}


size_t texel_caret_c::get_last_position ()
{
	size_t numerator = 1;
	auto it = texels->begin ();
	for (;it!= texels->end ();it++){
		if ( (*it )->is_text () ) {
			numerator++;
		}
	}
	return numerator;
}

size_t texel_caret_c::get_last_line ()
{
	size_t numerator = 1;
	auto it = texels->begin ();
	for (; it!= texels->end (); it++){
		if ( (*it)->is_new_line() ) {
			numerator++;	
		}
	}	
	return numerator;
}

size_t texel_caret_c::get_last_row (size_t till)
{
	size_t row = 0;
	if (till == 0 || till < info.row){
		for (auto x : *texels){
			if( x->is_row()){
				++row;
			}
		}
	}
	else {
		row = info.row;
		auto it = info.iterator;
		for (; it != texels->end();it++){
			if (row == till){
				break;
			}
			if ( (*it)->is_row()){
				row++;
			}
		}
	}
	return row;
}

void texel_caret_c::construct ()
{
	graphies->graphy.bitmap.set(14,4);	
	graphies->graphy.left = 0;
	graphies->graphy.top = -1;
}

void texel_caret_c::draw ( layen_c& layen )
{
	layen.font.my_draw2 ( graphies->graphy, layen );
}

void texel_caret_c::inc_line ()
{
	if(texels->empty() || !texels->front()->is_row()){
		cout << "inc line error\n";
		return;
	}
	to_row_down(1);
}

void texel_caret_c::dec_line ()
{
	if(texels->empty() || !texels->front()->is_row()){
		cout << "dec line error\n";
		return;
	}

	to_row_up(1);
}

int texel_caret_c::advance_x ()
{
	return adv_x;
}

int texel_caret_c::advance_y ()
{
	return adv_y;
}


bool texel_collapse_c::is_collapse ()
{
	return true;
}

texel_color_c::texel_color_c ()
{
	count = 2;
	color = 0xff0000;
}

texel_selector_c::texel_selector_c ()
{
	graphies = new graphies_c();
}

void texel_selector_c::dump ( stringstream& ss)
{
	ss << "sel\n";
}

texel_tab_c::texel_tab_c ()
{
	graphies = new graphies_c();
}

texel_tab_c::~texel_tab_c()
{
}

void texel_tab_c::construct()
{
	graphies->graphy.bitmap.set(3,3);
	graphies->graphy.left = 3;
	graphies->graphy.top = 3;
}

void texel_tab_c::dump(stringstream& ss )
{
	ss << "tab\n";
}

void texel_tab_c::draw(layen_c& layen)
{
	if ( layen.trace == 1 ) {
		layen.font.my_draw2 ( graphies->graphy, layen );
	}
}

int texel_tab_c::advance_x ()
{
	return size;
}

int texel_tab_c::advance_y ()
{
	return 0;
}

texel_newline_c::texel_newline_c ()
{
	graphies = new graphies_c();
}

texel_newline_c::~texel_newline_c()
{
}

void texel_newline_c::construct()
{
	graphies->trace.left = 0;
	graphies->trace.top = 20;
	graphies->trace.bitmap.set_box(10,20,0);
}

void texel_newline_c::draw(layen_c& layen)
{
	if ( layen.trace == 1 ) {
		layen.font.my_draw2 ( graphies->trace, layen );
	}
}

void texel_newline_c::dump (stringstream& ss )
{
	ss << "nl\n";
}

texel_row_c::texel_row_c(size_t size, shared_c& shared)
{
	graphies = shared.clone_row_graphies(1);
}

texel_row_c::texel_row_c():
last_row ( false ),
class_symbol ( class_row)
{
	graphies = new graphies_c;
}

texel_row_c::~texel_row_c()
{
}

void texel_row_c::dump ( stringstream& ss ) 
{
	ss << "row\n";
}

void texel_row_c::draw (layen_c& layen )
{
	if ( layen.trace == 1 ) {
		layen.font.my_draw2 ( graphies->graphy, layen );	
	}
}

void texel_row_c::construct()
{
	graphies->graphy.bitmap.set(1,10);
	graphies->graphy.left = 1;
	graphies->graphy.top = 10;
}

texel_char_c* texel_char_c::create(uint32_t chr,FT_Face face, size_t size, shared_c& shared)
{
	auto p = (texel_char_c*)shared.clone(chr, size);
	p->graphies->face = face;
	return p;
//	return (texel_char_c*) shared.clone ( chr, size );
}

texel_char_c::texel_char_c ( unsigned int ui )
{
	graphies = new graphies_c();
	character = ui;
}

texel_char_c::~texel_char_c ()
{
	clear_glyph_bitmap();
}

void texel_char_c::dump (stringstream& ss )
{
	ss << "char:" << (char)character << '\n';
}

void texel_char_c::draw ( layen_c& layen )
{
	layen.font.my_draw2 (graphies->graphy, layen );
	if ( layen.is_selected ) {
//		layen.font.my_draw2 (graphies->trace, layen );
	}
}

int texel_char_c::advance_x ()
{
	return adv_x;
}

int texel_char_c::advance_y ()
{
	return adv_y;
}

bool texel_char_c::is_word_part ()
{
	if ( isalnum ( character ) ){
		return true;
	}
	return false;
}

bool texel_char_c::is_punct ()
{
	return  ispunct(character );
}

void texel_char_c::construct ()
{
	FT_Face face = graphies->face;
	if ( graphies->graphy.bitmap.buffer == nullptr ) {
		FT_UInt glyph_index = FT_Get_Char_Index ( face, character );
		int error = FT_Load_Glyph ( face, glyph_index, FT_LOAD_DEFAULT );
//		FT_UInt glyph_index = FT_Get_Char_Index ( face, character );
//		int error = FT_Load_Glyph ( face, glyph_index, FT_LOAD_DEFAULT );
		if ( error ){
			cout << "free type: err load glyph\n";
			cout << "glyph_index: " << glyph_index << '\n';
			return;
		}
		if ( glyph_index == 0) {
	//		cout << "glyph_index: " << glyph_index << '\n';
		}
//		FT_GlyphSlot slot = face->glyph;
		FT_GlyphSlot slot = face->glyph;
		error = FT_Render_Glyph ( slot, FT_RENDER_MODE_NORMAL );
		if ( error ){
			cout << "free type: err render\n";
			return;
		}
		FT_Glyph ftglyph;
		FT_Get_Glyph ( slot,&ftglyph );
		FT_Glyph_To_Bitmap (&ftglyph, FT_RENDER_MODE_NORMAL, 0, 1 );
	
		FT_BitmapGlyph ftbmp = ( FT_BitmapGlyph ) ftglyph;
		
		auto &graphy = graphies->graphy;
		graphy.left = ftbmp->left;	
		graphy.top = ftbmp->top;	
		int rows = graphy.bitmap.rows = ftbmp->bitmap.rows;	
		int width = graphy.bitmap.width = ftbmp->bitmap.width;	
		adv_x = slot->advance.x >> 6;
		adv_y = slot->advance.y >> 6;
	
		
		unsigned char* p = ftbmp->bitmap.buffer;
		int si = rows*width;
		uint32_t* u = graphy.bitmap.buffer = new uint32_t[si]; 
		
		static gray_8_rgb_c grays;
		for (int i = 0; i < si; i++)
			*u++ = grays.table[*p++];
		
		FT_Done_Glyph ( (FT_Glyph) ftbmp);
		
	}
	if ( graphies->trace.bitmap.buffer == nullptr ) {
		graphies->trace.bitmap.set_box(adv_x, 20, 0);
		graphies->trace.top=20;
	}	
}

texel_c::texel_c ()
{
//	graphies = new graphies_c();
}

void texel_c::clear_glyph_bitmap ()
{
	if (graphies ==nullptr){
		return;
	}
	assert (graphies);

	clear_pixel_vectors();

	delete graphies->graphy.bitmap.buffer;
	graphies->graphy.bitmap.buffer = nullptr;
	delete graphies->trace.bitmap.buffer;
	graphies->trace.bitmap.buffer = nullptr;
}

void texel_c::clear_pixel_vectors()
{
	if ( graphies == nullptr){
		return;
	}
	graphies->graphy.is_vectorized = false;
	graphies->graphy.pixels.clear();
	graphies->trace.is_vectorized = false;
	graphies->trace.pixels.clear();
}

texel_c::~texel_c ()
{

	if ( graphies != nullptr && graphies->dec_reference() == 0 ){
		delete graphies;
	}
//	cout << "texel destruction\n";
}

void ex_entry_texel ( stringstream& ss )
{
	echo << "testing texel\n";
}