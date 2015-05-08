#include "spectrum.h"

spectrum::spectrum()
{}

spectrum::~spectrum()
{}

void spectrum::drawSpectrum( 	vector<PixelToaster::Pixel> &surface, 
															int surfaceWidth,
															int surfaceHeight,
															int x, 
															int y, 
															int width, 
															int height, 
															FMOD_CHANNEL *channel, 
															bool uselog,
															bool highlight )
{
  FMOD_RESULT result;
  int count, count2, numchannels;
  float xoff, step;
  float *src;
  float OscBuffer[width];
//	_debug("drawing spec %i, %i, %i, %i", x,y,width,height );
//  result = FMOD_System_GetSpectrum( sys, spectrum[0], numvalues, 0, FMOD_DSP_FFT_WINDOW_TRIANGLE);    // returns an array of 512 floats 
//  if( !checkError( FMOD_System_GetWaveData( sys, OscBuffer, width, 0 ) ) )
//	  return;
		if( !checkError( FMOD_Channel_GetWaveData( channel, OscBuffer, width, 0 ) ) )
			return;
    /*
        xoff is the x position that is scaled lookup of the dsp block according to the graphical
        window size.
    */
    xoff = 0;
    step = 1;
    src = OscBuffer;
    for (count=0; count < width; count++)
    {
        int x1, y1, y2;

        #define calcoffset2(_x, _y) (surfaceWidth * _y) + _x

        x1  = (int)xoff;
        y1  = (int)((src[x1]           + 1.0f) / 2.0f * (float)height);
        y2 = (int)((src[x1+(int)step] 	+ 1.0f) / 2.0f * (float)height);

        y1  = y1  < 0 ? 0 : y1  >= height ? height-1 : y1;
        y2 = y2 < 0 ? 0 : y2 >= height ? height-1 : y2;

        if (y1 > y2)
        {
            int tmp = y1;
            y1 = y2;
            y2 = tmp;
        }

        for (count2=y1; count2<= y2; count2++)
        {
		        PixelToaster::Pixel &p = surface[ calcoffset2(count+x, count2) + (y*surfaceWidth) ];
            p.r = p.g = p.b = highlight ? 0.33 : 0.07;            
        }

        xoff += step;
    }
}
