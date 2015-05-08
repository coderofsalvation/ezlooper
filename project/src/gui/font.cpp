#include "font.h"
#include "EZgui.h"
#include "font_myriad.h"
#include "font_terminal.h"


font::font( string filename, unsigned int charsize, int charspace, FONT f )
{
	this->filename 	= filename;	
	this->charsize 	= charsize;
	this->texture 	= new image();
	this->charspace = charspace;
	if( f == NONE )
		EZgui::getInstance()->loadImage( filename.c_str(), texture );
	else{
		switch( f ){
			case MYRIAD: 		EZgui::getInstance()->loadImage( 	filename.c_str(), 
																												texture,
																												&font_myriad[0],
																												6362,
																												true ); break;
			case TERMINAL: 	EZgui::getInstance()->loadImage( 	filename.c_str(), 
																												texture,
																												&font_terminal[0],
																												1862,
																												true ); break;

		}	
	}			
	createChars();	
}

font::~font()
{
	if( this->texture != NULL )
		EZgui::getInstance()->destroyImage( texture );	
}

void font::createChars(){
	unsigned int xmax = texture->width / charsize;
	unsigned int ymax = texture->height / charsize;
	for( int y = 0; y < ymax; y++ ){
		for( int x = 0; x < xmax; x++ ){
			image *tmp = new image();
			tmp->width = tmp->height = charsize;
			// copy them pixels!
			for( int i = 0; i < charsize; i++ )
				for( int j = 0; j < charsize; j++ )
					tmp->data.push_back( texture->data[ ((x*charsize)+j) + ( ((y*charsize )+i) * texture->width ) ] );
			chars.push_back( tmp );					
		}	
	}		
}

void font::draw( string line, unsigned int x, unsigned int y, vector<Pixel> & surface ){
	if( chars.size() == 0 )
		return;
	unsigned int offset 	= x;
	const char *tokens = line.c_str();	
	for( int i = 0; i < line.size(); i++ ){
		unsigned char token = tokens[i]-33;
		switch( token ){ // most fontmaps dont support weird chars
			case 129: token = 117;	break;
			case 131:
			case 132:
			case 133: token = 93; break;
			case 136:
			case 137:
			case 138: token = 101; break;
			case 139:
			case 140:
			case 141: token = 105; break;
			case 147:
			case 148: token = 111; break;
			default: break;			
		}
		if( token < chars.size() )
			EZgui::getInstance()->drawImage( surface, x+(i*(charsize+charspace)), y, chars[ token ], 0 );//chars[ token ] );
	}			
}
