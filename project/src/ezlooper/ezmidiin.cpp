#include "ezlooper.h"
#include "ezmidiin.h"
#include "ezchannel.h"

#define MIDI_FPS 50

void mycallback( double deltatime, std::vector< unsigned char > *message, void *userData )
{
  unsigned int nBytes = message->size();
  for ( unsigned int i=0; i<nBytes; i++ )
    std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
  if ( nBytes > 0 )
    std::cout << "stamp = " << deltatime << '\n';
}

/* name : midi
 * usage: constructorish!
 * notes:
 */
ezmidiin::ezmidiin( ezlooper *ez ){
	this->ez = ez;
	this->iface = NULL;
	this->limitRingUp = false;
	this->limitRingDown = false;;	
	this->timerStop = 0;	
	this->soundFxBank = 0;
	this->enabled = false;		
}

ezmidiin::~ezmidiin(){
	if( iface != NULL ){
	  	iface->closePort();
	  	delete iface;
	}		
}

bool ezmidiin::checkFPS( int framerate ){
  clock_t now = clock();
  int diff;
  if( (diff = now - movement_timer) > CLK_TCK/ framerate){
		movement_timer = now;
  	return true;
  }else return false;
}

void ezmidiin::init( ){
  try{ 
    iface = new RtMidiIn();
  }catch ( RtError &error ){
    _error( "ezmidiin::init() ERROR %s",error.getMessage().c_str( ) );
	}
  // Check inputs.
  unsigned int nPorts = iface->getPortCount();
  std::string portName;
  for ( unsigned int i=0; i<nPorts; i++ ) {
    try {
      portName = iface->getPortName(i);
    }
    catch ( RtError &error ) {
	    _error( "ezmidiin::init() ERROR %s",error.getMessage().c_str( ) );
    }
    this->ports.push_back( portName );
  }
}

void ezmidiin::addPort( int device ){
	if( this->ports.size() == 0 )
		return;
	iface->openPort( device );
	_say("ezmidiin::init() opened '%s'",this->ports[ device ].c_str() );
  // Don't ignore sysex, timing, or active sensing messages.
  iface->ignoreTypes( true, true, true );
} 

/* name : process
 * usage: implementation of the superobject ezcontrol's process function
 * notes: see eztypes.h for event-types
 */
void ezmidiin::process( EZEvent e, int arg1 ) {
	std::vector<unsigned char> message;
	if( iface == NULL ) return;
  switch( e ){             
		case EZ_UPDATEMIDI:	{
			double stamp = iface->getMessage( &message );
			int    nBytes = message.size();										
			if( nBytes == 3 ){
				switch( message[0] ){
					case 144:	// MIDI NOTE EVENT
						if( enabled )
							processMidiNote( &message ); 
						break;
										
					case 176: // MIDI CONTROLLER
						if( enabled )
							processMidiCtrl( &message );
					break;
				}					
			}
	    //for ( int i=0; i < nBytes; i++ )
	    //  _debug( "[%f] Byte[%i] = %i ",stamp,i,(int)message[i]);											  			
		}
		break;
  }
}

void ezmidiin::processMidiNote( std::vector<unsigned char> *_message){	
	std::vector<unsigned char> message = *_message;	 
	ez = ezlooper::Instance();

	// hide soundbank of help screen if any
	if( showSoundFx != -1 ){		
		showSoundFx = -1;
		redraw(ALL,23423);
	}
	
	switch( message[1] ){ // MIDI NOTE NR
		case 0: {// STOP
			if( message[2] > 0 )
				ez->audio->doEvent( STOP, 0 );
			ez->audio->channel[ ez->selectedChannel]->pressedStop = (message[2] != 0);			
			break;
		}														          
			
		case 1:	// PLAY
			_debug("pressed play!");
			if( ez->audio->channel[ ez->selectedChannel ]->pressedLock ){
					ez->audio->doEvent( PITCHRESET );						
					return;	
			}			
			if( message[2] > 0){
				ez->audio->doEvent( STOP, 0 );		
				ez->audio->doEvent( PLAY, 0 );							
			}
			ez->audio->channel[ ez->selectedChannel]->pressedPlay = (message[2] != 0);			
			break;
						
		case 2:	// NEXTFILE
			if( message[2] > 0 ){
				if( ez->audio->channel[ ez->selectedChannel ]->isPlaying() )		
					_say("CHANNEL IS PLAYING!!! WAKEY WAKEEEEEY :)");
				else ez->nextFile( ez->audio->channel[ ez->selectedChannel ]->pressedLock ? -1 : 1 );									
			}
			break;

		case 38: /// SELECT TRACK A
			_debug("select deck A");
    	if( message[2] > 0 ){				
    		ez->selectedChannel = 0;
				if( ez->audio->channel[ ez->selectedChannel ]->pressedLock )
					ez->audio->channel[ ez->selectedChannel ]->toggleLoop();
				else ez->audio->stopSoundFx();
				redraw(ALL,2342);
			}
   		break;

		case 39: /// SELECT TRACK B
			_debug("select deck B");
    	if( message[2] > 0 ){
    		ez->selectedChannel = 1;
				if( ez->audio->channel[ ez->selectedChannel ]->pressedLock )
					ez->audio->channel[ ez->selectedChannel ]->toggleLoop();
				else ez->audio->stopSoundFx();
				redraw(ALL,2342);				
			}    		
   		break;

		case 60: // SET MARKERS DECK A
		case 61:
		case 62:
		case 63:
		case 64:
		case 65:
		case 66:						
		case 67:{
			int marker = (message[1] - 60);
			if( ez->audio->channel[0]->pressedLock )
				marker+= 8;			
			_debug("channel[0] setMarker( %i )", marker);
			ez->setLoopPoint( 0, marker );
			}
			break;

		case 68: // SET MARKERS DECK B
		case 69:
		case 70:			
		case 71:			
		case 72:			
		case 73:
		case 74:
		case 75:{
			int marker = (message[1] - 68);
			if( ez->audio->channel[1]->pressedLock )
				marker+= 8;
			_debug("channel[1] setMarker( %i )", marker);
			ez->setLoopPoint( 1, marker );
			}
			break;
			
		case 80: // PRESS LOCK/SHIFT
			for( int i = 0; i < PLAYERS; i++ )
				ez->audio->channel[ i ]->pressedLock = (message[2] != 0);
			break;
		
		case 40:
		case 41:
		case 42:
		case 43:{
			if( message[2] > 0 ){
				int index = message[1] - 40;
				if( ez->audio->channel[ 0 ]->pressedLock ){ // select bank
					soundFxBank = index;
					_debug("BANK %i",index+1);
					showSoundFx = index;
					return;
				}				
				ez->audio->playSoundFx( index + (4*soundFxBank), ez->selectedChannel );
			}
			break;	
		}		
				
	}
}

void ezmidiin::processMidiCtrl( std::vector<unsigned char> *_message){
	ez = ezlooper::Instance();	
	std::vector<unsigned char> message = *_message;	
	if( ez->settings->v.usefx ){
		switch( message[1] ){	// controller number
			case 1:	 // LEFT Y-AXIS
					ez->audio->channel[0]->fx["1_filter.dll"]->moveAxis( MOVE_TOTAL, message[2] );
				break;
	
			case 2:{ // LEFT Y-AXIS
				for( int i = 0; i < PLAYERS; i++ )
					ez->audio->channel[i]->fx["1_filter.dll"]->moveAxis( MOVE_TOTAL, message[2] );
				break;
			}
				
			case 3:	 // MIDDLE Y-AXIS
				ez->audio->channel[1]->fx["1_filter.dll"]->moveAxis( MOVE_TOTAL, message[2] );
				break;
	
			case 7:	 // LEFT X-AXIS
				ez->audio->channel[0]->fx["2_pingpong.dll"]->moveAxis( MOVE_RIGHT, 	message[2] );
				ez->audio->channel[0]->fx["2_pingpong.dll"]->moveAxis( MOVE_LEFT, 	message[2] );				
				break;
	
	 		case 8:{ // MIDDLE X-AXIS
					for( int i = 0; i < PLAYERS; i++ ){
						ez->audio->channel[i]->fx["2_pingpong.dll"]->moveAxis( MOVE_RIGHT, 	message[2] );
						ez->audio->channel[i]->fx["2_pingpong.dll"]->moveAxis( MOVE_LEFT, 	message[2] );				
					}
				}
				break;
	
			case 9:	 // RIGHT X-AXIS
					ez->audio->channel[1]->fx["2_pingpong.dll"]->moveAxis( MOVE_RIGHT, 	message[2] );
					ez->audio->channel[1]->fx["2_pingpong.dll"]->moveAxis( MOVE_LEFT, 	message[2] );				
				break;
	
		
			case 10: // X-FADER
				for( int i = 0; i < PLAYERS; i++ )
					ez->audio->channel[i]->fx["2_pingpong.dll"]->moveAxis( MOVE_TOTAL, 	message[2] );		
				break;	
		}			
	}		
	switch( message[1] ){
		case 12: // LEFT WHEEL
			if( ez->audio->channel[0]->isPlaying() )									
				doPitchChange( 0, message[1], _message );			
			break;
			
		case 13: // RIGHT WHEEL 
			if( ez->audio->channel[1]->isPlaying() )									
				doPitchChange( 1, message[1], _message );
			break;
	}	
}

void ezmidiin::doPitchChange( int channel, int ctrlNum, std::vector<unsigned char> *_message ){
	std::vector<unsigned char> message = *_message;	
	ezchannel *c = ez->audio->channel[ channel ];
	// speed up/down record with sine																									
	if( !c->pressedPlay && 	!c->pressedStop){
		if( message[2] > this->ctrlCache[ctrlNum] || message[2] == 127 )
			c->speedUp();	
		if( message[2] < this->ctrlCache[ctrlNum] || message[2] == 0 )
			c->speedDown();																											
	}																						
	// set pitch of record
	if( c->pressedLock && !c->pressedPlay &&  !c->pressedStop){
		float speedNew;
	  if( message[2] > this->ctrlCache[ctrlNum] )
	  	speedNew = (SPEED_MAXPITCH/127);																										  
	  if( message[2] < this->ctrlCache[ctrlNum] )
	  	speedNew = -(SPEED_MAXPITCH/127);
		if( speedNew > (SPEED_MAXPITCH) ||
				speedNew < -(SPEED_MAXPITCH) )
					return;																										  	
  	c->speed += speedNew/SPEED_MAXPITCHCHANGE;
	}
	this->ctrlCache[ ctrlNum ] = message[2];	
}

void ezmidiin::enable( bool state ){
	this->enabled = state;	
}
