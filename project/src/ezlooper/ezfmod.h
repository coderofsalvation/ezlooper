/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CLASSNAME : ezview
 *
 * author  : sQuezel (www.boompje.com  www.mannetje.org)
 * function: handles CONTROL-events
 * usage   : this class is used from within doEvent() in ezengine-class
 * notes   :
 */

#ifndef EZFMOD_H
#define EZFMOD_H

#include "fmod.h"
#include "fmod_errors.h"
#include <math.h>
#include <vector>
#include <map>
#include <string>
#include "../util/sine.h"
#include "ezlooperTypes.h"

using namespace std;

class eztrack;
class ezlooper;
class ezchannel;
class ezfmod {
  // Associations
  // Attributes
	private:
		FMOD_SOUND						*soundfx;		
    FMOD_CHANNELGROUP 		*masterGroup[PLAYERS];
    FMOD_RESULT        		result;
    int                		key, count,numdrivers;
    unsigned int       		version;
    bool									shareSoundCard;
		bool									soundFxEnd;
		int 									soundFxChannel;    
		
		bool 	initFMOD( bool useConfig );				
		
  // Operations  
  public:		 			                                       
    FMOD_SYSTEM       		*sys[PLAYERS];
		vector<ezchannel *> channel;
    map<string,eztrack *>	track[PLAYERS];
    vector<string>				devices;    
		
		ezlooper *ez;
		bool									inited;

    ezfmod( ezlooper *ez );    
    ~ezfmod();
    bool init( bool useConfig );
		void initEffects();		
		void addSoundCard( int device );		
    bool load( string file, int selectedChannel );
		void process( EZEvent e, int arg1 = 0);
		void doEvent( EZAudioEvent e, int arg1 = 0 );
		void playSoundFx( int index, int channel );
		void stopSoundFx( );
		void createTracks( int player );		
		void destroyTracks( int player );
};

#endif
