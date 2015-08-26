#ifndef FONT_H
#define FONT_H

#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

class pixel_c;
class editor_c;
class texel_c;
class texel_caret_c;
class layen_c;
class texel_selector_c;

using namespace std;

/*
typedef struct FT_GlyphRec
{
FT_GlyphRec
Ft_Library			library;
const FT_Glyph_Class*	clazz;
FT_Glyph_Format		format;	
FT_Vector			advance;
}	FT_GlyphRec;
typedef struct FT_BitmapGlyphRec
{
FT_GlyphRec	root;
FT_Int		left;
FT_Int		top;
FT_Bitmap	bitmap;
}	FT_BitmapGlyphRec
typdef struct FT_Bitmap
{
int			rows;
int			width;
int			pitch;
unsigned char*	buffer;
short			num_grays;
char				pixel_mode;
char				palette_mode;
void*			palette;	
}	FT_Bitmap
*/

class bitmap_c
{
public:
	void set_box( int,int, uint32_t);
	void set (int, int);
	int rows{};
	int width{};
	uint32_t* buffer{nullptr}; 
};

class graphy_c 
{
public:
	bool is_vectorized{};
	vector<pixel_c> pixels;
	bitmap_c bitmap;
	int left{};
	int top{};
};

class graphies_c
{
public:
	FT_Face face{};
	size_t size{13};
	graphy_c graphy;
	graphy_c trace;
	int reference{1};
	int dec_reference(){return --reference;}
};

class glyph_ft2_c 
{
public:
	glyph_ft2_c ();
	~glyph_ft2_c ();
	FT_BitmapGlyph bitmapGlyph;
	void dump( stringstream& ss);
};

struct tab{
	tab(int _size, int _ctexs, vector<texel_c*>::iterator _it):
	size{_size},ctexs{_ctexs},it{_it}{};
	int size{};
	int ctexs{};
	vector<texel_c*>::iterator it;		
};

class font_c 
{
	public :
	font_c ();
	virtual ~font_c ();
	static int init ();
	static int done ();
	static unsigned long type;
	layen_c* layen;
	void set_rows ( layen_c& layen, vector<texel_c*>* texs = nullptr );
	void set_rows(layen_c& layen, vector<texel_c*>&, vector<texel_c*>::iterator begin, vector<texel_c*>::iterator end);
	void resolve_elastic_tabs(int max_size, list<tab>& tabs);
	void resolve_elastic_tabs2(layen_c& layen, list<list<tab>>& tabs);
	void set_elastic_tabs(layen_c& layen, vector<texel_c*>&);
	void set_elastic_tabs( layen_c& layen );
	virtual void engrave ( layen_c& layen );
	virtual unsigned long get_type (){return type;};
	matrix_c<FT> get_text_size ( string s );
	int get_text_size ( vector<texel_c*>&, size_t, size_t );
	static FT_Face set_face ( string name, size_t size, float f );
	int my_draw2( graphy_c& glyph, layen_c& layen );
	int getY(string c);
	int tab_width;
	int cell;
	
	void dump (stringstream& ss );	
	int test ( stringstream& ss, int sel);	
	static FT_Face system_face;
};


#endif
