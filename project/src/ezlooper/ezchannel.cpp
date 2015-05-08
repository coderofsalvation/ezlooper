#include "ezchannel.h"
#include "eztrack.h"
#include "ezlooper.h"

extern "C" {	
	/*
	FMOD_RESULT F_CALLBACK syncPoint(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, int command, unsigned int commanddata1, unsigned int commanddata2)
	{
		_debug("--------------------------- BOF callback");
	  
		ezlooper *e = ezlooper::Instance();
		for( int i = 0; i < e->audio->channel.size(); i++ ){
			ezchannel *c = e->audio->channel[ i ];
			
			if( (c->channel == channel && c->loop ) ){										
				loopPoint *current  		= c->track->loopPoints[ c->track->loopPointCurrent ];
				loopPoint *next  				= c->track->loopPoints[ c->track->loopPointNext ];								
				int distance 						= c->track->loopPointNext - c->track->loopPointCurrent;
			  map<int,marker *>::iterator hiddenMarker = c->track->hiddenMarkers.find( commanddata1 );
			  bool isLastMarker 			= (c->track->loopPointNext != c->track->loopPoints.size()-1 );

		    // lets see if this is not just a wakeup event
		    // REASON: sometimes wakeup syncpoints are passed, because FMOD does not do callbacks
		    // 					if there isn't one syncpoint given between a loopregion    
				char name[255];
				unsigned int offset;
				FMOD_SYNCPOINT *point;			    
				checkError( FMOD_Sound_GetSyncPoint( c->track->sound, commanddata1, &point) );				
				checkError( FMOD_Sound_GetSyncPointInfo( c->track->sound,point, name, 255,&offset, FMOD_TIMEUNIT_PCMBYTES) );
			  _debug("syncpoint[%i] '%s' callback!",commanddata1, name );			
				
				// loopend 'underscore' marker (if a markername starts with an underscore..)
				if( hiddenMarker != c->track->hiddenMarkers.end() && commanddata1 != 0 && c->loop != 2)
					return FMOD_OK;
				if( hiddenMarker != c->track->hiddenMarkers.end() && commanddata1 != 0 && c->loop == 2){ 
					int hiddenOffset 			= hiddenMarker->second->offset;
					bool beginPosIsHidden = false;										
					int nextHiddenMarker 	= -1;
				  map<int,marker *>::iterator iter;   
				  for( iter = c->track->hiddenMarkers.begin(); iter != c->track->hiddenMarkers.end(); ++iter ) {			
						if( iter->second->offset < c->setLoopPointCache[1] && iter->second->offset  > hiddenOffset )
							nextHiddenMarker = iter->second->offset;
						if( c->setLoopPointCache[0] == iter->second->offset )
							beginPosIsHidden = true;													
					}					
					if( !beginPosIsHidden )
						c->setLoopPoint( hiddenOffset, nextHiddenMarker != -1 ? nextHiddenMarker : c->setLoopPointCache[1] );										
					else
						c->setLoopPoint( c->setLoopPointCache[0], hiddenOffset  );						
					_debug(">> hidden looppoint : nextHiddenMarker = %i, beginPosIsHidden=%i",nextHiddenMarker, beginPosIsHidden);											
				}else if( distance != 0 ){
	        _debug("syncpoint 'setting next looppoint'");
					c->setLoopPoint( next->begin->offset, next->end->offset );	
					c->track->loopPointCurrent = c->track->loopPointNext;				
					if( distance > 1 || distance < -1 ){ // huge jump to next loop(other loops inbetween)
						c->setPos( next->begin->offset );	
	          _debug("syncpoint 'huge jump to next loop (other loops inbetween)'");
	        }
				} else if( current->begin->offset == c->setLoopPointCache[0] && current->end->offset == c->setLoopPointCache[1] ){
					c->setLoopPoint( current->begin->offset, current->end->offset );
					_debug("syncpoint 'setting current looppoint again'");
				}
				redraw( ALL, 23423 );
			}	
		}
		_debug("--------------------------- EOF callback");
		return FMOD_OK;	
	}	
	*/
	FMOD_RESULT F_CALLBACK syncPoint(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, int command, unsigned int commanddata1, unsigned int commanddata2)
	{
		_debug("--------------------------- BOF callback");
	  
		ezlooper *e = ezlooper::Instance();
		for( int i = 0; i < e->audio->channel.size(); i++ ){
			ezchannel *c = e->audio->channel[ i ];
			
			if( (c->channel == channel && c->loop ) ){										
				loopPoint *current  		= c->track->loopPoints[ c->track->loopPointCurrent ];
				loopPoint *next  				= c->track->loopPoints[ c->track->loopPointNext ];								
				int distance 						= c->track->loopPointNext - c->track->loopPointCurrent;
			  map<int,marker *>::iterator hiddenMarker = c->track->hiddenMarkers.find( commanddata1 );
			  bool isLastMarker 			= (c->track->loopPointNext != c->track->loopPoints.size()-1 );

		    // lets see if this is not just a wakeup event
		    // REASON: sometimes wakeup syncpoints are passed, because FMOD does not do callbacks
		    // 					if there isn't one syncpoint given between a loopregion    
				char name[255];
				unsigned int offset;
				FMOD_SYNCPOINT *point;			    
				checkError( FMOD_Sound_GetSyncPoint( c->track->sound, commanddata1, &point) );				
				checkError( FMOD_Sound_GetSyncPointInfo( c->track->sound,point, name, 255,&offset, FMOD_TIMEUNIT_PCMBYTES) );
			  _debug("syncpoint[%i] '%s' callback!",commanddata1, name );			
				
				/************************************************************************
         * Handle hidden markers				 
         */				
				// loopend 'underscore' marker (if a markername starts with an underscore..)
				if( hiddenMarker != c->track->hiddenMarkers.end() && commanddata1 != 0 && c->loop != 2)
					return FMOD_OK;
				if( hiddenMarker != c->track->hiddenMarkers.end() && commanddata1 != 0 && c->loop == 2){ 
					int hiddenOffset 			= hiddenMarker->second->offset;
					bool beginPosIsHidden = false;
					int nextHiddenMarker 	= -1;
				  map<int,marker *>::iterator iter;   
				  for( iter = c->track->hiddenMarkers.begin(); iter != c->track->hiddenMarkers.end(); ++iter ) {			
						if( iter->second->offset < c->setLoopPointCache[1] && iter->second->offset  > hiddenOffset )
							nextHiddenMarker = iter->second->offset;
						if( c->setLoopPointCache[0] == iter->second->offset )
							beginPosIsHidden = true;													
					}					
					if( !beginPosIsHidden )
						c->setLoopPoint( hiddenOffset, nextHiddenMarker != -1 ? nextHiddenMarker : c->setLoopPointCache[1] );										
					else
						c->setLoopPoint( c->setLoopPointCache[0], hiddenOffset  );						
					_debug(">> hidden looppoint : nextHiddenMarker = %i, beginPosIsHidden=%i",nextHiddenMarker, beginPosIsHidden);											
				}else if( distance != 0 ){
				/************************************************************************
         * Handle markers when next looppoint is not equal to current looppoint
				 */
	        _debug("syncpoint 'setting next looppoint'");
					c->setLoopPoint( next->begin->offset, next->end->offset );	
					c->track->loopPointCurrent = c->track->loopPointNext;				

					if( distance > 1 || distance < -1 ){ // huge jump to next loop(other loops inbetween)
						c->setPos( next->begin->offset );	
	          _debug("syncpoint 'huge jump to next loop (other loops inbetween)'");
	        }
				} else if( current->begin->offset == c->setLoopPointCache[0] && current->end->offset == c->setLoopPointCache[1] ){
					c->setLoopPoint( current->begin->offset, current->end->offset );
					_debug("syncpoint 'setting current looppoint again'");
				}
				redraw( ALL, 23423 );
			}	
		}
		_debug("--------------------------- EOF callback");
		return FMOD_OK;	
	}		
}

ezchannel::ezchannel(	FMOD_SYSTEM *sys, FMOD_CHANNELGROUP *masterGroup ){
	this->sys = sys;		
	this->track = NULL;
	this->channel = NULL;
	this->channelSoundFx = NULL;
	this->masterGroup = masterGroup;
	this->sineSpeedC	= 0;
	this->playing = false;
	this->needsLoad 		= false;
	this->pressedPlay		= false;
	this->pressedStop		= false;
	this->pressedRecord = false;
	this->pressedLock		= false;
	this->loop					= 0;
	this->offset 				= 0;	
	this->loopPointsLoaded = false;
	speed = -0.1f;
	
}

/* destructor
 *
 * NOTE: tracks are deleted and unloaded by ezfmod-class!
 */
ezchannel::~ezchannel(){
  int num;
  if( this->channel != NULL )
		checkError( FMOD_Channel_Stop( this->channel ) );  
	map<string, ezfx *>::iterator iter;   
  for( iter = fx.begin(); iter != fx.end(); iter++ ){ 
		ezfx *tmp = (ezfx *)iter->second;
  	checkError( FMOD_System_UnloadPlugin(sys, FMOD_PLUGINTYPE_OUTPUT, tmp->getDSPIndex() ) );
	}	
	if( this->sys != NULL ){
	  checkError( FMOD_System_Close(sys) );
	  checkError( FMOD_System_Release(sys) );	
	  sys = NULL;
	}	  
}

bool ezchannel::init(){
 	bool ok = true;
	ok &= this->initSines();	
	return ok;
}

void ezchannel::initPlugin( string filename ){
	#ifdef NOFX
		return;
	#endif
  FMOD_PLUGINTYPE plugintype;
  int index;
  int nParams;
  int num;
	FMOD_DSP *dsp = NULL;
//	checkError( FMOD_System_SetPluginPath( sys, FILE_DIR_VST ) );
	string filenameFull = FILE_DIR_VST;
	filenameFull.append("/");
	filenameFull.append(filename);
	if( !	checkError( FMOD_System_LoadPlugin( sys, filenameFull.c_str(), &plugintype, &index) ) ){
		_popup("error loading plugin '%s'",filenameFull.c_str());
		return;
	}	
	checkError( FMOD_System_CreateDSPByIndex( sys, index, &dsp) );
	checkError( FMOD_DSP_GetNumParameters( dsp, &nParams ) );
	for( int i = 0; i < nParams; i++ ){
		char name[255],label[255],description[255];
		float min,max;
		checkError( FMOD_DSP_GetParameterInfo( dsp, i, name, label, description, 250, &min, &max) );		
		_debug("%i ['%s'] <%f-%f>	%s", i, filename.c_str(), min, max, name );
	}
	checkError( FMOD_System_AddDSP( sys, dsp ) );
	checkError( FMOD_DSP_SetActive( dsp, true ) );	

	fx[ filename ] = new ezfx( dsp, filename, fx.size() );
}


bool ezchannel::initSines(){
	this->sines["speed"] 		= new sine( );
	this->sines["speed"]->setSine( SPEED_PITCHBEND, SINE_FALL, SPEED_FALL_SLICES );
}

void ezchannel::process( EZEvent e, int arg1 ){	
	switch( e ){
		case EZ_UPDATE:{
			if( !playing )
				return;
			checkError( FMOD_System_Update(sys) );
			updatePitch();
			updatePosition();
		}break;										
		
		default: break;
	}	
}

void ezchannel::updatePosition(){	
	if( this->track == NULL || this->channel == NULL | !playing )  // optimize tweak
		return;                 					
	
	loopPoint *current 	= NULL;
	loopPoint *next			= NULL;
	bool 			hack 			= false;
	unsigned int pos;

	checkError( FMOD_Channel_GetPosition( this->channel, &pos, FMOD_TIMEUNIT_PCMBYTES ) );				
	this->position 	= (100.0/track->getLength()) * offset;
	this->offset 		= pos;
/*	
	if( track->loopPoints.size() > 0 ){
 		current	= track->loopPoints[ track->loopPointCurrent ];	
   	next 		= track->loopPoints[ track->loopPointNext ];											
 		hack		= (track->loopPointCurrent != 0 && track->loopPointCurrent != track->loopPoints.size()-1 );
	}
	// tight getposition calls
	if( (current->end->offset - offset) < PCM_LOOKAHEAD && hack ){
		checkError( FMOD_Channel_GetPosition( this->channel, &pos, FMOD_TIMEUNIT_PCMBYTES ) );				
		if( pos >= current->end->offset-5000 ){
			if( current != next ){ 
				track->loopPointCurrent = track->loopPointNext;			
				setLoopPoint( next->begin->offset, next->end->offset );
				setPos( next->begin->offset );
			}
		}
		this->offset = pos;
		_debug("scanning fast [%i] bytes till end: %i",offset,(current->end->offset - offset));		
	}else if( checkFPS( FRAMERATE_LOOKAHEAD ) ){
		// normal getposition call (intervalled for performance)
		checkError( FMOD_Channel_GetPosition( this->channel, &pos, FMOD_TIMEUNIT_PCMBYTES ) );		
		_debug("scanning normal [%i] lookahead(%i)",pos, PCM_LOOKAHEAD );
	}		
*/	
}

void ezchannel::updatePitch(){
	if( (speed == speedCache && sineSpeedC == 0) || !playing) // optimize tweak
		return;
	float pitch, x;
	x = speed + 1.0;
	if( sineSpeedC > 0 ){
		_debug("ezchannel::process() x = %f & getSine( %i ) = %f",x,sineSpeedC,this->sines["speed"]->getSine( sineSpeedC ));											
		x += (this->speedDirection ) ? this->sines["speed"]->getSine( sineSpeedC ) :
																		-this->sines["speed"]->getSine( sineSpeedC );
		if( checkFPS( FRAMERATE_PITCHBEND ) ){																		
			sineSpeedC = ( sineSpeedC > SPEED_FALL_SLICES ) ? 0 : sineSpeedC + 1;
		}
	}      
	pitch = 0.75 + ( x * (0.5/2) );		
	speedCache = speed;
	checkError( FMOD_ChannelGroup_SetPitch(masterGroup, pitch ) );
	
}

void ezchannel::doEvent( EZAudioEvent e, int arg1 ){
	#ifdef DEBUG
	//_say("ezchannel::doEvent(%i,%i)",e,arg1);
	#endif
	switch( e ){
		case PLAY:		
			if( this->isPlaying() )
				checkError( FMOD_Channel_SetPaused( channel, false) );			
			if( this->track != NULL && !this->track->soundLoaded ){
				_say("loading track '%s'",track->filename.c_str());
  			loopPointsLoaded = false;
				track->loadSound( sys );	
				if( !track->mp3 )
					track->getSyncPoints();
				loopPointsLoaded = (track->loopPoints.size() > 1 );			
					
				loop = ( track->loopPoints.size() > 1 ) ? 1 : 0;				
				checkError( FMOD_System_PlaySound(sys, FMOD_CHANNEL_REUSE , track->sound, false, &(this->channel)) );								
			}
			if( this->track != NULL && this->track->soundLoaded ){
				if( !track->mp3 ){
					loopPoint *current 	= track->loopPoints[ track->loopPointCurrent ];
					if( track->setNextMarkerOnPlay ){
						track->loopPointCurrent = track->loopPointNext;
						track->setNextMarkerOnPlay = false;
					} 
					setPos( current->begin->offset+1 );								
					setLoopPoint( current->begin->offset, current->end->offset );				
					_debug("pressed play, playing %i-%i",	current->begin->offset, current->end->offset );														
				}else setPos( 1 );
			}   
			checkError( FMOD_Channel_SetPaused( channel, false) );
			this->playing = true;				
			redraw( ALL, PLAY );
			break; 	

		case STOP:		
			if( this->isPlaying() ){
				//checkError( FMOD_Channel_Stop( this->channel ) ); // this destroys the channel handle
				checkError( FMOD_Channel_SetPaused( channel, true) );				
				checkError( FMOD_Channel_SetPosition( this->channel, 0, FMOD_TIMEUNIT_PCMBYTES) );
				this->playing = false;
				redraw( ALL, STOP );								
			}
			break;
			
		case SPEEDUP:
			speedUp();
			break;
			
		case SPEEDDOWN:
			speedDown();
			break;
			
		case PITCHUP:
			speed += 0.005;
			break;
			
		case PITCHDOWN:
			speed -= 0.005;
			break;
	
    case PITCHRESET:
      speed = 0.0;		
      break;
	}	
}

           
bool ezchannel::isPlaying(){ 
	return this->playing;
}					 
					                        
void ezchannel::speedUp(){	
	if( sineSpeedC != 0 ) return;	
	speedDirection = true;
	sineSpeedC = 1;
}

void ezchannel::speedDown(){
	if( sineSpeedC != 0 ) return;
	speedDirection = false;
	sineSpeedC = 1;
}

void ezchannel::setPos( int offset ){
	checkError( FMOD_Channel_SetPosition( channel, offset, FMOD_TIMEUNIT_PCMBYTES ) );						
}

void ezchannel::setLoopPoint( int begin, int end ){
	setLoopPointCache[0] = begin;
	setLoopPointCache[1] = end;	
	checkError( FMOD_Channel_SetLoopPoints( channel, 
																				begin, 
																				FMOD_TIMEUNIT_PCMBYTES, 
																				end, 
																				FMOD_TIMEUNIT_PCMBYTES) );	
	_debug("setting looppoint! %i-%i",begin,end);																				
	if( !track->mp3 )
		checkError( FMOD_Channel_SetCallback( channel, FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT, &syncPoint, 0 ) );			
	
}


void ezchannel::toggleLoop(){
	loop = (loop + 1) % 2;
	_debug("toggleLoop = %i",loop);
	
	if( track == NULL || !track->soundLoaded )
		return;

	// this is when we're really doing some audio	
	if( track->loopPoints.size() > 0 ){	
		switch( loop ){
			case 0:
			{
				setLoopPoint( track->loopPoints[0]->begin->offset, 
											track->loopPoints[ track->loopPoints.size()-1 ]->end->offset );
				break;											
			}			
			
			case 1:
		  case 2:
			{
				unsigned int pos;
				checkError( FMOD_Channel_GetPosition( this->channel, &pos, FMOD_TIMEUNIT_PCMBYTES ) );			
				int index = track->getLoopIndex( pos );
				if( index < track->loopPoints.size() ){
					loopPoint *l 						= track->loopPoints[ index ];
					track->loopPointCurrent = track->loopPointNext = index;
					setLoopPoint( l->begin->offset, l->end->offset );
				}					 					
				_debug("toggleLoop() guessing looppoint = %i", index );
				break;
			}
		}
	}else loop = 0;	
}
