#ifndef EZFX_H
#define EZFX_H

#include "ezlooperTypes.h"
#include <fmod.h>
#include <string>
#include <math.h>

using namespace std;

class ezfx{
	
	private:
		string		filename;
		int				index;		

	public:	
		FMOD_DSP	*dsp;		
		ezfx( FMOD_DSP *dsp, string filename, int index );
		~ezfx();
		int getDSPIndex();
		void moveAxis( EZJoystick e, int value );
		void initParams();
};

#endif
