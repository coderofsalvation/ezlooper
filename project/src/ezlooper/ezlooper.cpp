#include "ezlooper.h"

#include "ezfmod.h"
#include "ezchannel.h"

int showSoundFx = -1;

/* name : Instance()
 * usage: call this to get the active ezlooper ptr
 * notes: singleton design pattern
 */
ezlooper* ezlooper::pinstance = 0;// initialize pointer
ezlooper* ezlooper::Instance () 
{        
  if (pinstance == 0)  // is it the first call?
  {  
    pinstance = new ezlooper(); // create sole instance
  }
  return pinstance; // address of sole instance
}

/* name : ezlooper()
 * usage: don't...singleton handles this
 * notes: don't use this..use Instance() instead
 */
ezlooper::ezlooper(){
	this->done 						= false;
	this->settings  			= NULL;
	this->audio						= NULL;
	this->dir							= "";
	this->selectedChannel = 0;
	for( int i = 0; i < PLAYERS; i++ )
		this->selectedFile[i] = 0;
	this->status  				= NULL;	
  _say( "ezlooper started :]" );
  
}

ezlooper::~ezlooper(){
	if( this->audio != NULL )
		delete this->audio;		
	if( this->midiIn != NULL )
		delete midiIn;	
	if( this->status != NULL )
		delete status;
	if( this->ini 	 != NULL )
		delete this->ini;		
}

/* name : init(..)
 * usage:
 * notes: if "ezlooper.cfg"-file is written, classes will be inited with its values
 */
bool ezlooper::init(){
	// read ini file into settings obj
	this->settings 	= new ezsettings();	
	this->ini 		 	= new CIniFile();
	this->read();	

  // init engines
	this->status 		= new ezStatus();
	this->audio 		= new ezfmod( this );		
	this->midiIn 		= new ezmidiin( this );		
	this->midiIn->init( );
	
	if( !settings->readSucces ){
		_popup("could not read 'ezlooper.ini'");
	}else{
		for( int i = 0; i < PLAYERS; i++ )
			audio->addSoundCard( settings->v.audioDevice[i] );		
		midiIn->addPort( settings->v.midiInDevice );				
	}
	this->audio->init( this->settings->readSucces );		

  // scan directory for files if any
	for( int i = 0; i < PLAYERS; i++ )	
		this->scanAllFiles( i );	
		
	// scan for effects and load them
	for( int i = 0; i < PLAYERS; i++ )	
		this->scanFiles( FILE_DIR_VST, 	i, (vector<string> *)&(this->vst),		"*.dll");	
  audio->initEffects();

	toggleChannel();

  // start receiving midi
  midiIn->enable( true );
  return true;
}

void ezlooper::cleanup(){
	this->done = true;
	if( this->status  != NULL ) delete this->status;
	if( this->audio		!= NULL ) delete this->audio;	
	if( this->midiIn  != NULL ) delete this->midiIn;
	if( this->ini     != NULL ) delete this->ini;
}

void ezlooper::doEvent( EZEvent e, int value ){
	switch( e ){
		case EZ_UPDATE: 		{
												if( this->audio != NULL )
													this->audio->process( e, value );
												break;
												}
		case EZ_UPDATEMIDI:	{											
												this->midiIn->process( e, value );  	
											  break;
												}
		case EZ_QUIT: 			this->cleanup(); 
		                    this->done = true;
												break;				
		default: break;	
	}		
}

void ezlooper::read(){
	if( ini != NULL ){	
		ini->Path( FILE_SETTINGS );		
		ini->ReadFile();
		settings->v.midiInDevice 			= ini->GetValueI( "settings", "midiInDevice" );
		settings->v.audioDevice[0] 		= ini->GetValueI( "settings", "audioDevice1");
		settings->v.audioDevice[1]  	= ini->GetValueI( "settings", "audioDevice2" );		
		settings->v.usefx  						= ini->GetValueI( "settings", "usefx");		
		settings->v.dir[0] 						= ini->GetValue( "settings", "dir1" );
		settings->v.dir[1]				 		= ini->GetValue( "settings", "dir2");
		settings->v.bufferlength 			= ini->GetValueI( "settings", "bufferlength" );
		settings->v.numbuffers				= ini->GetValueI( "settings", "numbuffers" );		
		settings->v.useheadphones			= ini->GetValueI( "settings", "useheadphones" );		
		if( ini->GetValueI( "settings", "showhelp" ) == 0 )
			_popup("Welcome to your first session!\r\n\r\n To make your mixman dm2 work, load the 'ezlooper.dm2'-into the dm2midi.exe application.\r\n\r\nAnd always remember..you can press 'F1' for help anytime!\n\n\nWARNING: markers werken niet 100%, gebruik loop on/off!");
		file_dir[0] 									= settings->v.dir[0];
		file_dir[1] 									= settings->v.dir[1];		
		settings->readSucces = true;
	}	
		
}

void ezlooper::save(){
	if( ini != NULL ){	
		ini->Path( FILE_SETTINGS );		
		ini->SetValueI( "settings", "midiInDevice", settings->v.midiInDevice );
		ini->SetValueI( "settings", "audioDevice1", settings->v.audioDevice[0] );
		ini->SetValueI( "settings", "audioDevice2", settings->v.audioDevice[1] );		
		ini->SetValueI( "settings", "usefx", 				settings->v.usefx );		
		ini->SetValueI( "settings", "showhelp", 		1 );
		ini->SetValueI( "settings", "bufferlength", settings->v.bufferlength );
		ini->SetValueI( "settings", "numbuffers", 	settings->v.numbuffers );		
		ini->SetValueI( "settings", "useheadphones", 	settings->v.useheadphones );				
		if( settings->v.dir[0].size() )
			ini->SetValue( "settings", "dir1", 				settings->v.dir[0].c_str() );
		if( settings->v.dir[1].size() )
			ini->SetValue( "settings", "dir2", 				settings->v.dir[1].c_str() );		
		ini->WriteFile();		
	}	
}

void ezlooper::scanAllFiles( int channel ){	
	audio->destroyTracks( channel );
	files[channel].clear();
	this->scanFiles( file_dir[channel], 		channel, (vector<string> *)&(this->files), 	"*.wav" );
	this->scanFiles( file_dir[channel], 		channel, (vector<string> *)&(this->files), 	"*.mp3" );
	if( channel == 0 ){		
		this->scanFiles( FILE_DIR_FX,					channel, (vector<string> *)&(this->filesfx),"*.wav" );						
		this->scanFiles( FILE_DIR_FX,					channel, (vector<string> *)&(this->filesfx),"*.mp3" );									
	}	
	audio->createTracks( channel );

	// select first track
  if( files[channel].size() > 0 ){
		audio->channel[channel]->currentFile = files[channel][0];		
		this->selectedFile[channel] = 0;		
	}      
	             
	if( files[channel].size() > 0 )	
		if( audio->channel[ channel ]->currentFile.size() > 0 )
	  	audio->load( audio->channel[ channel ]->currentFile, channel );
	selectedChannel = channel;

	redraw( ALL, 1231);
	for( int j = 0; j < PLAYERS; j++ )
		for( int i = 0; i < files[j].size(); i++ )
			_debug("channel[%i][%i] = %s",j,i,files[j][i].c_str());
	for( int j = 0; j < PLAYERS; j++ )
		for( int i = 0; i < vst[j].size(); i++ )
			_debug("vst[%i][%i] = %s",channel,i,vst[j][i].c_str());			

}


void ezlooper::scanFiles( string dir, int channel, vector<string> *files, string wildcard )
{
	char            search_path[457];
	WIN32_FIND_DATA aDt;
	string          fulldir = string();
	char msg[255];

	fulldir.append(dir);
	if (fulldir[fulldir.size() - 1] != '\\')
		fulldir.append("\\");
	if( wildcard.size() == 0 )
		sprintf(search_path, "%s\\*", fulldir.c_str());
	else
		sprintf(search_path, "%s\\%s", fulldir.c_str(), wildcard.c_str());	
	
	
	// SEARCH!
	HANDLE  aSearch = FindFirstFile(search_path, &aDt);
	if (aSearch != INVALID_HANDLE_VALUE) {
		string  file = string();
		if (aDt.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
			file = aDt.cFileName;			
			files[channel].push_back( aDt.cFileName);
		}else{
			// do something with dirs?
		}

		// process the first file here. aDt.cFileName contains the relative
		// file name
		while (FindNextFile(aSearch, &aDt))
		{
			string  file = string();
			// process the next file here. aDt.cFileName contains the
			// relative file name
			if (aDt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// do something with dirs?
			}
			else {
				file = aDt.cFileName;
				files[channel].push_back( aDt.cFileName);
			}
		}

		FindClose(aSearch);
	}	
}

void ezlooper::nextFile( int direction ){
	int channel = this->selectedChannel;
	int index = this->selectedFile[ channel ];
	ezchannel *c = this->audio->channel[ this->selectedChannel ];
	if( direction == -1)		
		this->selectedFile[channel] = ( index == 0 ) ? 	(this->files[channel].size()-1) : 
																										selectedFile[channel]-1;
	if( direction == 1)		
		this->selectedFile[channel] = (this->selectedFile[channel] + 1 ) % (this->files[channel].size());		
	audio->load( this->files[ channel ][ this->selectedFile[channel] ], channel );
	redraw( ALL, 1231);
}

void ezlooper::toggleChannel(){
	this->selectedChannel = (this->selectedChannel + 1) % PLAYERS;	
	redraw( ALL, 2423 );		
}

void ezlooper::setLoopPoint( int channel, int marker ){	
	ezchannel *c = audio->channel[ channel ];
	selectedChannel = channel;	
	if( c->track != NULL && c->track->soundLoaded && c->loop ){
		c->track->setNextLoopPoint( marker, !c->isPlaying() );
		// if we are playing, shift the endlooppoint a bit to the right,
		// so the endlooppoint marker will do the 'onMarker' callback
  	if( c->isPlaying() )
			c->setLoopPoint( 	c->track->loopPoints[ c->track->loopPointCurrent ]->begin->offset,
												c->track->loopPoints[ c->track->loopPointCurrent ]->end->offset + PCM_LOOKAHEAD
												);
	}
	redraw( ALL, 23423 );		
}

string ezlooper::getLoopPoint( int nr, int channel ){
	return this->audio->channel[channel]->track->markers[nr]->name;
}

ezStatus * ezlooper::getStatus(){	

	this->status->selectedChannel = this->selectedChannel;
	for( int i = 0; i < PLAYERS; i++ ){
		eztrack *t 												= this->audio->channel[i]->track;

		if( this->audio->channel[i] != NULL && this->audio->channel[i]->track != NULL )
			this->status->filename[i] = this->audio->channel[i]->track->filename;
		else this->status->filename[i] = "no file loaded";		

		this->status->speed[i] 						= this->audio->channel[i]->speed;
		this->status->bend[i] 						= (this->audio->channel[i]->sineSpeedC > i);
		this->status->bendDir[i] 					= this->audio->channel[i]->speedDirection;
		this->status->countLoopPoints[i] 	= (t == NULL) ? -1 : t->loopPoints.size();
		this->status->currentLoopPoint[i] = (t == NULL) ? 0 : t->loopPointCurrent;
		this->status->nextLoopPoint[i] 		= (t == NULL) ? 0 : t->loopPointNext;
		this->status->loop[i] 						= this->audio->channel[i]->loop;
		this->status->playing[i] 					= this->audio->channel[i]->isPlaying();		
		this->status->loopLocation[i] 		= this->audio->channel[i]->position;
	}
	return this->status;	
}

string ezlooper::pickDir()
{
  BROWSEINFO  bi = { 0 };
  bi.lpszTitle = "Pick a Directory";
  TCHAR path[MAX_PATH];
  LPITEMIDLIST  pidl = SHBrowseForFolder(&bi);
  if (pidl != 0) {

    // get the name of the folder    
    SHGetPathFromIDList(pidl, path);

    // free memory used
    IMalloc *imalloc = 0;
    if (SUCCEEDED(SHGetMalloc(&imalloc))) {
      imalloc->Free(pidl);
      imalloc->Release();
    }
  }
	return path;
}
