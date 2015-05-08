#include "ezlooper.h"
#include "eztrack.h"
#include "ezfmod.h" 		
#include "ezchannel.h"
        
/* name : fmod
 * usage: constructorish!
 * notes:
 */
ezfmod::ezfmod( ezlooper *ez ) {
	this->ez 						= ez;
	this->inited				= false;
	this->soundfx				= NULL;
	for( int i = 0; i < PLAYERS; i++ ){
		this->sys[i] = NULL;
		this->masterGroup[i] = NULL;
	}
}

/* destructor
 *
 * NOTE: mastergroups are deleted by channels
 */
ezfmod::~ezfmod(){
  _debug("exit");
	for( int i = 0; i < PLAYERS; i++ )
		destroyTracks(i);
	if( this->soundfx != NULL )
		checkError( FMOD_Sound_Release( this->soundfx ) );
	for( int i = 0; i < channel.size(); i++ )
		delete channel[i];	
}

bool ezfmod::init( bool useConfig = false ){
	_debug("ezfmod::init()");	
	bool ok = true;	
	ok &= this->initFMOD( useConfig );	
	this->inited = ok;		
	return ok;
}


bool ezfmod::initFMOD( bool useConfig = false ){
	bool ok;
	_debug("ezfmod::initFMOD()");	   
	checkError( FMOD_System_Create( &(this->sys[0]) ) );
	checkError( FMOD_System_Init( sys[0] ,2, FMOD_INIT_NORMAL,0) );	
	checkError( FMOD_System_GetVersion( sys[0], &(this->version) ) );
	if(version < FMOD_VERSION)
    _error("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);	  
  // get audio drivers
	checkError( FMOD_System_GetNumDrivers(sys[0], &(this->numdrivers))	);		
	_debug("found %i audiodrivers", this->numdrivers );
	for( int i = 0; i < numdrivers; i++ ){
		char name[255];
		//FMOD_System_GetDriverInfo( sys[0], i, name, 255,NULL);
		checkError( FMOD_System_GetDriverName(sys[0], i, name, 255) );
		devices.push_back( name );
	}		
	checkError( FMOD_System_Close( sys[0] ) );
	checkError( FMOD_System_Release( sys[0] ) );
	_debug("ezfmod::initFMOD()");	
}

void ezfmod::initEffects(){
  // init the soundcards/channels/vst's
	for( int i = 0; i < channel.size(); i++ ){
		this->channel[i]->init();
		if( ez->settings->v.usefx )
	   	for( int j = 0; j < ez->vst[ i ].size(); j++ )
				channel[i]->initPlugin( ez->vst[ i ][ j ] );			
	}				                                              	
}
                                             
void ezfmod::addSoundCard( int device ){
	int ddevice;
	ezchannel *c;
	int id = 0;	
	bool initSystem = true;

	if( this->channel.size() == PLAYERS ){
		_say("ezfmod::addSoundCard() already 2 soundcards assigned!");
		return;
	}
	if( this->channel.size() == 1 ){
		FMOD_System_GetDriver( this->channel[0]->sys, &ddevice );
		this->shareSoundCard = (ddevice == device);
		id = (shareSoundCard) ? 0 : 1;		
		initSystem = ( ddevice != device );
	}
	_debug("sysid = %i & device = %i && initSystem = %i",id,device,initSystem);
	if( initSystem ){
		checkError( FMOD_System_Create( &(sys[id]) ) );		
		checkError( FMOD_System_SetDriver(sys[id], device ) );
		checkError( FMOD_System_SetDSPBufferSize( sys[id], ez->settings->v.bufferlength, ez->settings->v.numbuffers) );
		//checkError( FMOD_System_SetOutput( sys[id], FMOD_OUTPUTTYPE_DSOUND ) );
		checkError( FMOD_System_SetSoftwareFormat( sys[id], 44100,  FMOD_SOUND_FORMAT_PCM16, 2, 0, FMOD_DSP_RESAMPLER_LINEAR ) );		
 		checkError( FMOD_System_Init( sys[id] ,2, FMOD_INIT_NORMAL,0) );
		checkError( FMOD_System_GetMasterChannelGroup(sys[id], &(this->masterGroup[id]) ) );  			
	}	
	this->channel.push_back( new ezchannel( sys[id], masterGroup[id] ) );	
}

void ezfmod::createTracks( int player ){
	destroyTracks( player );
  if( ez->files[player].size() > 0 ){
		for( int i = 0; i < ez->files[player].size(); i++ ){
			eztrack *t = new eztrack( ez->files[player][i] );
			t->filenameFull = ez->file_dir[player];
			t->filenameFull.append("\\");
			t->filenameFull.append( ez->files[player][i] );
			this->track[player][ ez->files[player][i] ] = t;
		}
	}
}

void ezfmod::destroyTracks( int player ){
	map<string, eztrack *>::iterator iter;   
	if( track[player].size() != 0){
	  for( iter = track[ player ].begin(); iter != track[ player ].end(); ++iter ){
			eztrack *t = (eztrack *)iter->second;
			if( t != NULL ){
				t->unload();
				delete t;
			}
		}
		track[player].clear();
	}else _debug("destroyTracks(): no tracks in player");
}


void ezfmod::process( EZEvent e, int arg1 ){	
	for( int i = 0; i < channel.size(); i++ ){
		channel[i]->process( e, arg1 );	
	}													
}

void ezfmod::doEvent( EZAudioEvent e, int arg1 ){
	#ifdef DEBUG
	//_say("ezfmod::doEvent(%i,%i)",e,arg1);
	#endif
	channel[ ez->selectedChannel ]->doEvent( e, arg1 );			
}

bool ezfmod::load( string file, int selectedChannel ){
	if( file.size() == 0 )
		return false;
	ezchannel *c = this->channel[ selectedChannel ];
	eztrack *t = this->track[ selectedChannel ][ file ];
	if( t == NULL ){
		_popup("could not find ezfmod::track[ %i ][ '%s' ]",selectedChannel, file.c_str());		
		return false;		
	}
	if( c->isPlaying() )
		c->doEvent( STOP );		
	c->loopPointsLoaded = false;	
	if( c->track != NULL )
		c->track->unload();			
	c->speed = 0.0;
	c->track = t;		
	c->currentFile = t->filename;	
	return true;	
}



void ezfmod::playSoundFx( int index, int channel ){
	if( index >= ez->filesfx[0].size() )
		return;
	soundFxChannel = channel;
	string file = FILE_DIR_FX;
	file.append("/");
	file.append( ez->filesfx[0][ index ] );
	if( this->soundfx != NULL )
		checkError( FMOD_Sound_Release(soundfx) );
	checkError( FMOD_System_CreateSound( this->channel[ channel ]->sys , file.c_str(), FMOD_SOFTWARE | FMOD_ACCURATETIME | FMOD_LOOP_OFF, 0, &(this->soundfx) ) );
	checkError( FMOD_System_PlaySound( this->channel[ channel ]->sys, FMOD_CHANNEL_FREE, this->soundfx, false, &(this->channel[ channel ]->channelSoundFx)) );		
	checkError( FMOD_Channel_SetPaused( this->channel[ channel ]->channelSoundFx, false) );
	soundFxEnd = false;	
}

void ezfmod::stopSoundFx( ){
//	if( soundFxEnd )
//		return;
	//checkError( FMOD_Channel_Stop( this->channelSoundFx ) );	
	unsigned int length;
	checkError( FMOD_Sound_GetLength( this->soundfx, &length, FMOD_TIMEUNIT_PCMBYTES) );	
	unsigned int offset = (length/4) * 3;
	checkError( FMOD_Channel_SetPosition( channel[ soundFxChannel ]->channelSoundFx, offset, FMOD_TIMEUNIT_PCMBYTES) );	
//	soundFxEnd = true;
}
