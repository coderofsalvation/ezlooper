/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CLASSNAME : ezview
 *
 * author  : sQuezel (www.boompje.com  www.mannetje.org)
 * function: handles CONTROL-events
 * usage   : this class is used from within doEvent() in ezengine-class
 * notes   :
 */

#ifndef EZFMODCHANNEL_H
#define EZFMODCHANNEL_H

#include "fmod.h"
#include "fmod_errors.h"
#include <math.h>
#include <vector>
#include <map>
#include <string>
#include "../util/sine.h"
#include "ezlooperTypes.h"
#include "ezfx.h"

using namespace std;

class eztrack;
class ezlooper;
class ezchannel {
  // Associations
  // Attributes
	private:
    // pitch bend
    map<string, sine *> 	sines;
		bool initSines();		  
		bool									playing;
		unsigned int 					offset;		
		
	public:
		int 									setLoopPointCache[2];
		unsigned int 					position;
		int										loop; // 0 = loop off 1 = loop on 2 = loop on +microloops
		bool									pressedPlay;
 		bool									pressedStop;
 		bool									pressedRecord;
 		bool									pressedLock;
 		bool									needsLoad;
		bool 									loopPointsLoaded;
		map< string, ezfx *>	fx;		
    eztrack			       		*track;
    FMOD_SYSTEM       		*sys;
    FMOD_CHANNEL      		*channel;
    FMOD_CHANNEL      		*channelSoundFx;    
    FMOD_CHANNELGROUP 		*masterGroup;    		
    string								currentFile;

    // sine speed down/up utilities    
		float 							speed;	
		float								speedCache;	                   
    int									sineSpeedC;
    bool								speedDirection;
		
		ezchannel( FMOD_SYSTEM *sys, FMOD_CHANNELGROUP *masterGroup );
		~ezchannel();
		bool init();
		void initPlugin( string filename );
		void speedUp();
		void speedDown();
		bool isPlaying();

		void process( EZEvent e, int arg1 = 0);
		void doEvent( EZAudioEvent e, int arg1 = 0);
		void updatePosition();
		void updatePitch();
		void setPos( int offset );
		void setLoopPoint( int begin, int end );
		void toggleLoop();
		
};

#endif
