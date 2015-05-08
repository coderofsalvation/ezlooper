#include <cstdlib>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#ifdef DEBUG
#include "debug_new.h"
#endif
#include "ezlooper/ezlooper.h"

#include "debug_nogui.cpp"

using namespace std;

ezlooper *ez;
clock_t time_now[FRAMERATE_MAX];
clock_t movement_timer[FRAMERATE_MAX];

bool checkFPS( int framerate ){
  time_now[ framerate ] = clock();
  if( time_now[ framerate ] - movement_timer[ framerate ] > CLK_TCK/ framerate){
  	movement_timer[ framerate ] = time_now[ framerate ];	
  	return true;
  }else return false;
}

bool checkError( FMOD_RESULT r ){
	if( r != FMOD_OK )
		_debug( "FMOD error : (%d) %s", r, FMOD_ErrorString( r ) );	
	return ( r == FMOD_OK );
}

void selectMidi(){
	int driver;
	printf("\n");	
  if( ez->midiIn->ports.size() == 0 )
      return;
  _say("Select MIDI-IN device" );
  printf("---------------------------------------------------------\n");    
  for (int j=0; j < ez->midiIn->ports.size(); j++)
      printf("%d) %s\n", j + 1, ez->midiIn->ports[j].c_str() );
  do{
    int key = getch();
    if (key == 27)
    	return;
    driver = key - '1';
  } while (driver < 0 || driver >= ez->midiIn->ports.size());
  
  printf("\n");
	_say("initializing MIDI-IN device '%s'",ez->midiIn->ports[driver].c_str());
	ez->midiIn->addPort( driver );
	ez->settings->v.midiInDevice = driver;	
}

void selectSoundCards(){
	int driver,key;
		
	for( int i = 0; i < PLAYERS; i++ ){
		printf("\n");
	  _say("Select soundcard %s\n", (i==0) ? "A" : "B" );
	  printf("---------------------------------------------------------\n");    
	  for (int j=0; j < ez->audio->devices.size(); j++)
	      printf("%d) %s\n", j + 1, ez->audio->devices[j].c_str() );
	  do{
	    key = getch();
	    if (key == 27)
	    	return;
	    driver = key - '1';
	  } while (driver < 0 || driver >= ez->audio->devices.size());
	  printf("\n");
		_say("initializing soundcard '%s'",ez->audio->devices[driver].c_str());
		ez->audio->addSoundCard( driver );
		ez->settings->v.audioDevice[i] = driver;
	}	
}	

void selectFX(){
	printf("\n");
	int key = -1;
  _say("Do you want to use the VST audio fx? (y/n)");
  do{
    key = getch();
		ez->settings->v.usefx = (key == 'y');	    
  } while ( key == -1 );
  printf("\n");
}	

void drawInterface(){	
	ezStatus *s = ez->getStatus();
	if( s != NULL ){
		#ifndef DEBUG
		system("CLS");
		#endif
		//clrscr(); //LINUX
		//printf("\033[H\033[J");
		printf(":: EZ LOOPER LIVE LOOPSYSTEM v1.0 ::\n\n");
		for( int i = 0; i < PLAYERS; i++ ){
			int steps = 59;
			int speed = (int) ((float)(s->speed[i]+1.0f) * ((float)steps/2.0f));
			int speedL = speed;
			int speedR = steps - speedL; 			
			printf("%s[MARKERLOOP: %s]%s\n\n",
						  (s->selectedChannel == i)?">>>>>>":"      ",
							(s->loop[i])?"ON " : "OFF",
							(s->selectedChannel == i)?"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<":"-----------------------------------------------------");
			printf("      %s\n", 														 
														 s->filename[i].c_str() );
			printf("      [");
			for( int j = 0; j < speedL; j++ )	
				printf("-");													 
			printf("[%2f]",s->speed[i]);
			for( int j = 0; j < speedR; j++ )	
				printf("-");													 
			printf("]\n");
			if( s->bend[i] ){
				if( s->bendDir[i] )
				printf("      [                                           > > > > > > > > > > > > > ]\n");			
				else
				printf("      [ < < < < < < < < < < < < < < < < \n");				
			}else printf("\n");
			for( int j = 0; j < s->countLoopPoints[i]; j++ ){
				if( j == s->currentLoopPoint[i] )
				printf("      [%i] %s\n",j+1,ez->getLoopPoint( j, i ).c_str());	
				else if( j == s->nextLoopPoint[i] )
					printf(" !! -> %i  %s\n",j+1,ez->getLoopPoint( j, i ).c_str());					
				else
					printf("       %i  %s\n",j+1,ez->getLoopPoint( j, i ).c_str());									
			}
			if( i == 0 )											 
				printf("\n");
		}
	}
}

void processKey( int key ){
	_debug("processKey( %i )",key,key);
	switch( key ){
		case 'q':  	// 'q' - QUIT
			ez->doEvent( EZ_QUIT ); 
			exit(0);
			break;
			
		case 'p':   // 'p' - PLAY
			ez->audio->doEvent( STOP );		
			ez->audio->doEvent( PLAY );
			break;
			
		case 's':   // 's' - STOP
			ez->audio->doEvent( STOP );
			break;

		case 80:    // down arrow
			ez->audio->doEvent( SPEEDDOWN );			
			break;		

		case 72:    // up arrow
 			ez->audio->doEvent( SPEEDUP );			
 			break;

		case 45:    // '-'
			ez->audio->doEvent( PITCHDOWN );
			break;

		case 43: 		// '+' 
			ez->audio->doEvent( PITCHUP );
			break;

    case 'r':
    case '*':
         ez->audio->doEvent( PITCHRESET );
         break;
    
    case '.':
         ez->nextFile(1);
         break;
    
    case ',':
         ez->nextFile(-1);
         break;
             
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':		
			_debug("pressed key marker %i",key-49);
			ez->setLoopPoint( ez->selectedChannel,key-49 );
			break;
			
		case 9:			// 'TAB' - toggle channel
			ez->toggleChannel();
			break;			
	}	
}

int main(int argc, char *argv[])
{			
	printf(":: EZ LOOPER LIVE LOOPSYSTEM v1.0 ::\n\n");
	ez = ezlooper::Instance();
	
	if( !ez->init() )
		_error("could not init ezlooper engine :(");
	if( !ez->settings->readSucces ){
		selectSoundCards();	
		selectMidi();						
		selectFX();
		ez->settings->write( FILE_SETTINGS );		
	}else{
		_say("found configuration file '%s', loading..",FILE_SETTINGS);
		for( int i = 0; i < PLAYERS; i++ )
			ez->audio->addSoundCard( ez->settings->v.audioDevice[i] );		
		ez->midiIn->addPort( ez->settings->v.midiInDevice );		
	}
  ez->audio->initSoundCards();
  /* set low priority for gui, other states are :
		REALTIME_PRIORITY_CLASS  
		HIGH_PRIORITY_CLASS  
		ABOVE_NORMAL_PRIORITY_CLASS  
		NORMAL_PRIORITY_CLASS  
		BELOW_NORMAL_PRIORITY_CLASS  
		IDLE_PRIORITY_CLASS 
	*/
	//SetPriorityClass(GetCurrentThread(), REALTIME_PRIORITY_CLASS);
	
	#ifdef DEBUG
	system("PAUSE");
	#endif

  do{
		if( kbhit() ){
	    int key = getch();
	    processKey( key );
	    if (key == 47 || key == 27){
				ez->doEvent( EZ_QUIT );				
			}
		} 
    ez->doEvent( EZ_UPDATE );    
	  if( checkFPS( FRAMERATE_CONSOLE) )
		 	drawInterface();    
  } while ( !ez->done );	
  
  delete ez;
  _popup("end!");
	#ifdef DEBUG
	check_leaks();
	#endif  
  return EXIT_SUCCESS;
}
