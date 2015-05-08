#include "eztrack.h"

eztrack::eztrack(string filename){
	this->filename 				= filename;
	this->filenameFull		= "no filename set";
	this->sound 					= NULL;
	this->soundLoaded 		= false;
	this->loopPointCurrent= 0;
	this->loopPointNext		= 0;
	this->setNextMarkerOnPlay = true;
}

eztrack::~eztrack(){
	this->cleanup();	
}

void eztrack::cleanup(){
	if( markers.size() > 0)
		for( int i = 0; i < markers.size(); i++ )
			delete markers[i];	
	map<int, marker *>::iterator iter;
	for( iter = hiddenMarkers.begin(); iter != hiddenMarkers.end(); ++iter )			
		delete iter->second;	
			
	if( loopPoints.size() > 0 )
		for( int i = 0; i < loopPoints.size(); i++ )
			delete loopPoints[i];	
	markers.clear();
	hiddenMarkers.clear();
	loopPoints.clear();		
}

void eztrack::getSyncPoints(){
	if( mp3 ) return;
	if( !soundLoaded ){
		_popup("can't get syncpoints when sound is not loaded!" );
		return;
	}else cleanup();		

	// sync points
	int syncpoints;	
	FMOD_Sound_GetNumSyncPoints( sound, &syncpoints);
	_debug("sound	is %i PCMBYTES long, found %i syncpoints", getLength(), syncpoints );

	// create marker (0) 'begin'
	marker *m = new marker();
	m->name = "begin";
	m->offset = 0;
	markers.push_back( m );

	for( int i = 0; i < syncpoints; i++ ){
		char name[255];
		unsigned int offset;
		FMOD_SYNCPOINT *point;	
		if ( checkError( FMOD_Sound_GetSyncPoint( sound, i, &point) ) ){
			checkError( FMOD_Sound_GetSyncPointInfo(sound,point, name, 255,&offset, FMOD_TIMEUNIT_PCMBYTES) );				
			if( name[0] != '_' ){
			  // if beginmarker is set overwrite offset 0
				if( offset == 0 ){
					markers[0]->index = i;
					markers[0]->name 	= name;
					markers[0]->offset= offset;
					_debug("found marker %i - '%s (%i)'",i,m->name.c_str(), offset);				
				}else{	
				  // create marker
					m = new marker();
					m->index = i;
					m->name = name;
					m->offset = offset;
					markers.push_back( m );		
					_debug("found marker %i - '%s (%i)'",i,m->name.c_str(), offset);				
				}			
			}else{
				
				// create (hidden) underscore marker key = offset, item = percentage of length
				m = new marker();
				m->index = i;
				m->name = name;
				m->offset = offset;
				int length = static_cast<int>( (100.0f / (float)getLength()) * float(offset) );				
				m->percent = length;
				hiddenMarkers[ i ] = m;
				_debug("found marker %i (%i)- '%s' (hidden) (%i%)",i,offset, name, length);						
				//marker *m = markers[ markers.size()-1];
			}
		}
	}

	// create marker (0) 'end'
	FMOD_SYNCPOINT *point;
	checkError( FMOD_Sound_AddSyncPoint( sound, getLength()-2000, FMOD_TIMEUNIT_PCMBYTES, "end", &point ) );	
	_debug("gelukt?");
	m = new marker();
	m->index = markers.size();
	m->name = "end";
	m->offset = getLength()-2;
	markers.push_back( m );

	checkError( FMOD_Sound_GetNumSyncPoints( sound, &syncpoints) );
	_debug("sound	is %i PCMBYTES long, found %i syncpoints", getLength(), syncpoints );
	

	// determine length		
	_debug("marker size = %i",markers.size());
	if( markers.size() > 0 ){
		for( int i = 0; i < markers.size()-1; i++ ){
			loopPoint *p = new loopPoint();
			p->begin = markers[i];
			p->end = markers[ i+1];
			p->length =  static_cast<int>( (100.0f / (float)getLength()) * ((float)p->end->offset - (float)p->begin->offset) );
			if( loopPoints.size() < MAX_LOOPPOINTS )			
				loopPoints.push_back( p );
			else{
				_popup("track exceeds maximum looppoints, ignoring exceeding looppoints" );						
				break;
			}
			_debug("adding looppoint [%i] %i-%i = %i \% of track", i, p->begin->offset, p->end->offset, p->length );
			if( i > 500 ){
				_popup("strange behaviour took place...strange things may happen :(");
				return;
			}
		}		
	}
	for( int i = 0; i < loopPoints.size(); i++ ){	
		_debug(	"eztrack::loopPoint (%i) ['%s'-@'%s'] = %i - %i",
					i,
					loopPoints[ i ]->begin->name.c_str(),
					loopPoints[ i ]->end->name.c_str(),
					loopPoints[ i ]->begin->offset,
					loopPoints[ i ]->end->offset);
	}

}

void eztrack::loadSound( FMOD_SYSTEM *sys ){
	this->unload();
  // create sound
	mp3 = ( filename.find( ".mp3" ) != string::npos );
	if( !mp3 )                                                            
	  checkError( FMOD_System_CreateSound( sys, filenameFull.c_str(), FMOD_ACCURATETIME | FMOD_SOFTWARE |FMOD_ACCURATETIME, 0, &(this->sound) ) );	
	else		                                                                                            
		checkError( FMOD_System_CreateSound( sys, filenameFull.c_str(), FMOD_CREATESTREAM | FMOD_SOFTWARE |FMOD_ACCURATETIME, 0, &(this->sound) ) );			
  checkError( FMOD_Sound_SetLoopCount( sound, -1 ) );
  checkError( FMOD_Sound_SetMode( sound, FMOD_LOOP_NORMAL ) );
	soundLoaded = true;  
  _debug("sound is loaded(%s)",(soundLoaded)?"ja":"nee");	
}

int eztrack::getLength(){
	if( sound != NULL ){
		unsigned int length;
		checkError( FMOD_Sound_GetLength( sound, &length, FMOD_TIMEUNIT_PCMBYTES ) );		              
		return length;
	}	
	return 0;
}

void eztrack::setNextLoopPoint( int index, bool setNextMarkerOnPlay ){	
	if( index < 0 )
		loopPointNext = 0;		
	else if( index >= loopPoints.size() )
		loopPointNext = loopPoints.size()-1;
	else loopPointNext = index;	
	
	// if we are not playing
	if( setNextMarkerOnPlay ){
		this->setNextMarkerOnPlay = setNextMarkerOnPlay;	
		this->loopPointCurrent = loopPointNext;
	}
	_debug("eztrack::setNextLoopPoint( %i ,%i ) loopPointNex = %i",index, setNextMarkerOnPlay, loopPointNext );	
}

void eztrack::unload(){
	if( this->sound != NULL && soundLoaded )
		checkError( FMOD_Sound_Release(sound) );	
	soundLoaded = false;
	sound = NULL;		
}

int eztrack::getLoopIndex( int pos ){
	for( int i = 0; i < loopPoints.size(); i++ ){	
		if( loopPoints[ i ]->begin->offset < pos && loopPoints[ i ]->end->offset > pos )
			return i;
	}		
	return 0;
}
