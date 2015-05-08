/** 
 * File:        font.h
 * Date:        Sat Nov  1 21:54:23 2008
 *
 * description 
 * 
 * Changelog:
 * 	[Sat Nov  1 21:54:23 2008] 
 *		first sketch from scratch
 *
 * @todo description
 *
 * Usage example: 
 * <code>  
 *   // some code
 * </code>
 */
#ifndef FONT_H 
#define FONT_H
 
#include "PixelToaster.h"
#include "EZguiTypes.h"
#include <string>

class font {
  protected:

  private:
		std::string				filename;
		unsigned int			charsize;
		image							*texture;
		int 							charspace;
		vector<image *>   chars;
		    
  public:
    font( std::string filename, unsigned int charsize, int charspace = 0, FONT f = NONE );
    ~font();   
    void createChars();
		void draw( string line, unsigned int x, unsigned int y, vector<Pixel> & surface );
};

#endif

