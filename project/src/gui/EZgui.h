#ifndef EZGUI_H
#define EZGUI_H

#include <vector>
#include "../ezlooper/ezlooper.h"
#include "../ezlooper/ezlooperTypes.h"
#include "EZguiTypes.h"
#include "PixelToaster.h"
#include "GifDecoder.h"
#include "font.h"
#include "spectrum.h"

using namespace PixelToaster;

class EZgui : Listener{
	protected:
		EZgui();
		EZgui( const EZgui&);
		EZgui& operator = (const EZgui &);

	void onKeyDown( DisplayInterface & display, Key key );
	void onKeyPressed( DisplayInterface & display, Key key );
	void onKeyUp( DisplayInterface & display, Key key );
	const char * getKeyString( Key key );

	private:
    static EZgui 			*pinstance;				// ptr to singleton obj
		ezlooper					*ez;              // ptr to ezlooper engine
		vector<Pixel> 		pixels;           // framebuffer of screen
		vector<font *>   	fonts;            // fontscontainer 
		image 						*bg;              // backgroundimage container
		image 						*help;            // helpimage container		
		map<string,int>		loc;           		// container of layout specific margins
		ezStatus					*stat;						// pointer to current statistics of ezlooper engine
		int 							width; 						// width of the window
		int 							height;           // height of the window
		bool							flash;            // indicator for flashing gfx
		bool 							showhelp;

		Pixel 						colorB; 					// color used for backgrounds
		Pixel 						colorF;           // color used for dimmed/faded backgrounds
		Pixel							colorFB;					// like colorF but slightly darker
		Pixel 						border;
		Pixel 						borderD; 					// same as border color only darker
		Pixel 						green;
		Pixel 						white;
		

	public:
		Display 					*display;
		bool done;    		
		~EZgui();
		static EZgui*	getInstance();		
		
		void init();	
		void render();
		
		bool loadImage( const char filename[], image *img, unsigned char buf[] = NULL, int bufsize = 0, bool usebuf = false	);		
		void drawImage( vector<Pixel> & surface, unsigned int x, unsigned y, image *img, int transparentIndex = -1 );		
		void destroyImage( image *img );

		void drawBlock( 	vector<PixelToaster::Pixel> &surface, 
											int surfaceWidth,
											int surfaceHeight,
											int x, 
											int y, 
											int width, 
											int height,
											Pixel color,
											bool fadeout );	
		void drawLoopPoint( int index, const char *name, LOOPPOINT l, int x, int y, int lpwidth, int lpheight, bool highlight );
		void uppercase(char *lc);
		int sgn (long a);
		void line( int x1, int y1, int x2, int y2, Pixel color );
		void drawRect( int x1, int y1, int x2, int y2, Pixel color );
		void drawLocators();
		void drawSpectrums( bool drawA, bool drawB, bool fadeA, bool fadeB, int highlight );
		void drawSoundFx();
		
};

#endif
