#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "PixelToaster.h"
#include "EZguiTypes.h"
#include <fmod.h>
#include <vector>
#include <math.h>

using namespace std;

class spectrum{
	protected:
		
	private:
	
	public:
		static void drawSpectrum( vector<PixelToaster::Pixel> &surface, 
															int surfaceWidth,
															int surfaceHeight,
															int x, 
															int y, 
															int width, 
															int height, 
															FMOD_CHANNEL *channel, 
															bool uselog,
															bool highlight);
		spectrum();
		~spectrum();			
};

#endif
