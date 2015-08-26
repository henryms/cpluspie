#ifndef LAYEN_H
#define LAYEN_H


class layen_c
{
public:
	layen_c ();
	~layen_c();
	
	void set_rows();
	void remove_rows(vector<texel_c*>* txs = nullptr);
	int first_invalide_row;
	int last_invalide_row;

	int x;
	int y;
	int penX;
	int penY;
	
	vector<texel_c*> texels;
	vector<texel_c*> collapse_texels;
	shared_c shared_map;	
	FT_Face face{};	
	float font_zoom{1};	
	bool is_checked;
	bool is_selected;
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	uint32_t color;
	bool show_cursor{true};
	int line_width;
	uint32_t trace{};
	void delete_caret ();
	void set_caret ();
	texel_caret_c* caret{nullptr};
	void resize(int width, int height, int frqame_height_);
	texel_caret_c* get_caret();
	void invalide_caret();
	void clear_glyph_bitmap ();
	void clear_texel_map ();
	void clear_pixel_vectors ();
	
	void dump(string s, stringstream& ss);
	
	bool is_collapsed;
	void collapse( );
	
	int scroll_right;
	int scroll_down;
	int frame_height;	
	string text( int from = 1, int to = -1);
	string copy_text();
	void replace_text(string& txt);
	string cut_text();
	void erase_text();
	void set_text2 ( string& s, vector<texel_c*>* texels = nullptr);
	void set_text ( string& s, vector<texel_c*>* texels = nullptr);
	float zoom;
	font_c font;
	uint32_t* pi;
	int wi;
	int he;
	int si;
	int signo;
};

#endif