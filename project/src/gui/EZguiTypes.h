#ifndef EZGUI_TYPES_H
#define EZGUI_TYPES_H

#include <vector>
#include "PixelToaster.h"
#include <fmod.h>

using namespace PixelToaster;

enum LOOPPOINT{
	LOOPPOINT_NORMAL,
	LOOPPOINT_SELECTED,
	LOOPPOINT_NEXT
};

#ifndef REDRAWTYPES
#define REDRAWTYPES
enum REDRAW_TYPE{
	ALL,                                                          
	BACKGROUND,
	LOOPPOINTS
};
#endif

enum FONT{
	NONE,
	MYRIAD,
	TERMINAL	
};

class image{
	public:
		vector<Pixel *> data;
		unsigned int width;
		unsigned int height;		
};

extern void 	_say( 	char *msg, ... );
extern void 	_debug( 	char *msg, ... );
extern void 	_error( char *msg, ... );
extern void 	_popup( char *msg, ... );  
extern bool   checkError( FMOD_RESULT r );
extern bool checkFPS( int framerate );

extern bool bp;
extern bool redrawAll;						// indicator for redraw all gfx
extern bool redrawBG ; 			// indicator to redraw Spectrum gfx 
extern bool redrawLooppoints; 		// indicator to redraw Spectrum gfx 
extern void	redraw( REDRAW_TYPE t, int id );
extern int showSoundFx;


#endif
