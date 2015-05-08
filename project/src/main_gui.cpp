#ifdef DEBUG
//#include "debug_new.h"
#endif

#include <cstdlib>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "ezlooper/ezlooper.h"
#include "PixelToaster.h"
#include "gui/EZgui.h"
#include "gui/resource.h"
#include "debug_gui.cpp"

using namespace std;

HANDLE threads[2];
DWORD genericThread;
HWND hwnd;
ezlooper *ez							= NULL;
EZgui		 *gui 						= NULL;
bool 		 redrawAll 				= true;	// indicator for redraw all gfx
bool 		 redrawBG 				= true; // indicator to redraw the background image
bool 		 redrawLooppoints = true; // indicator to redraw Spectrum gfx 
int 		 cacheId				 	= -1;		// cache indicating caller of redraw of everything
bool 		 bp 							= false;// breakpoint indicator
clock_t time_now[FRAMERATE_MAX];
clock_t movement_timer[FRAMERATE_MAX];

DWORD WINAPI thread_midi(LPVOID iValue){
  EZgui *g = EZgui::getInstance();
  ezlooper *e = ezlooper::Instance();
  while( !g->done ){
		e->doEvent( EZ_UPDATEMIDI );
    Sleep(5);
  }
   ExitThread( NULL );
}

DWORD WINAPI thread_audio(LPVOID iValue){
  EZgui *g = EZgui::getInstance();
  ezlooper *e = ezlooper::Instance();
  while( !g->done ){
    ez->doEvent( EZ_UPDATE );    
    Sleep(10);
  }
  ExitThread( NULL );
}

bool checkFPS( int framerate ){
  time_now[ framerate ] = clock();
  int diff = time_now[ framerate ] - movement_timer[ framerate ];
  if( (diff > CLK_TCK/ framerate ) || (diff < 0) ){
  	movement_timer[ framerate ] = time_now[ framerate ];	
//  	_say("framerate = %i diff=%i \n", framerate, diff );
  	return true;
  }else return false;
}

void	redraw( REDRAW_TYPE t, int id ){
	switch( t ){
		case ALL: 				redrawAll 				= true; 	break;	
		case BACKGROUND: 	redrawBG 					= true; 	break;			
		case LOOPPOINTS:	redrawLooppoints	= true; 	break;					
	}	
	cacheId = id;	
}

bool checkError( FMOD_RESULT r ){
	if( r != FMOD_OK )
		_debug( "FMOD error : (%d) %s", r, FMOD_ErrorString( r ) );	
	return ( r == FMOD_OK );
}

BOOL CALLBACK ConfigDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

    switch(Message)
    {
        case WM_INITDIALOG:{
					HWND midi 	= GetDlgItem( hwnd, IDC_MIDI );
					HWND audio1 = GetDlgItem( hwnd, IDC_AUDIO_1 );
					HWND audio2 = GetDlgItem( hwnd, IDC_AUDIO_2 );										
					// make midi listbox
					ezmidiin *m = new ezmidiin( NULL );		
					m->init( );									
				  if( m->ports.size() != 0 ){
						int j = 0;
					  for (; j < m->ports.size(); j++)
							SendMessage(midi, CB_ADDSTRING, j, (LPARAM)m->ports[j].c_str() );							
						SendMessage(midi, CB_SETITEMDATA, j, (LPARAM) j);															
						SendDlgItemMessage(hwnd, IDC_MIDI, 		CB_SETCURSEL, 0, 0);	// important! else empty combox				
					}
					// make audio listboxes
					ezfmod *e = new ezfmod( NULL );
					e->init(false);
					int j = 0;
				  for (; j < e->devices.size(); j++){
						SendMessage(audio1, CB_ADDSTRING, j, (LPARAM)e->devices[j].c_str() );							
						SendMessage(audio2, CB_ADDSTRING, j, (LPARAM)e->devices[j].c_str() );														
					}
					SendMessage(audio1, CB_SETITEMDATA, j, (LPARAM) j);															
					SendMessage(audio2, CB_SETITEMDATA, j, (LPARAM) j);																					
					SendDlgItemMessage(hwnd, IDC_AUDIO_1, CB_SETCURSEL, 0, 0);	// important! else empty combox				
					SendDlgItemMessage(hwnd, IDC_AUDIO_2, CB_SETCURSEL, 0, 0);	// important! else empty combox				
					delete m;							
					delete e;
	        return TRUE;
				}
        break;
        
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDC_BTN1:{
										// save dialog settings
										HWND midi 			= GetDlgItem( hwnd, IDC_MIDI );
										HWND audio1 		= GetDlgItem( hwnd, IDC_AUDIO_1 );
										HWND audio2 		= GetDlgItem( hwnd, IDC_AUDIO_2 );																																																	
										HWND headphones = GetDlgItem( hwnd, IDC_CHK1 );										
										CIniFile *ini = new CIniFile();
										ini->Path( FILE_SETTINGS );		
										ini->SetValueI( "settings", "midiInDevice", SendMessage( midi, 				CB_GETCURSEL, 0, 0) );
										ini->SetValueI( "settings", "audioDevice1", SendMessage( audio1, 			CB_GETCURSEL, 0, 0) );
										ini->SetValueI( "settings", "audioDevice2", SendMessage( audio2, 			CB_GETCURSEL, 0, 0) );		
										ini->SetValueI( "settings", "useheadphones",SendMessage( headphones, 	BM_GETSTATE, 0, 0) == BST_CHECKED );												
										ini->SetValueI( "settings", "usefx", 				1 );		
										ini->SetValueI( "settings", "showhelp", 		0 );
										ini->SetValueI( "settings", "bufferlength", 1024 );
										ini->SetValueI( "settings", "numbuffers", 4 );															
										ini->WriteFile();		
										delete ini;
                    EndDialog(hwnd, IDOK);
                }break;
                
                case IDCANCEL:
                    EndDialog(hwnd, IDCANCEL);
                break;
            }
        break;
        default:
            return FALSE;
    }
    return TRUE;
}

void popupConfig(){
	int ret = DialogBox(GetModuleHandle(NULL),MAKEINTRESOURCE(IDC_DLG), hwnd, ConfigDlgProc);
  if(ret == IDOK){

  }
  else if(ret == -1){

  }	
}

using namespace PixelToaster;

int main()
{			
  const int width = 800;
  const int height = 600;
	ez = ezlooper::Instance();

	// popup config dialog if needed
	FILE *f = fopen("ezlooper.ini","r");
	if( f == NULL )
		popupConfig();
	else fclose(f);
	if( !ez->init() )
		_error("could not init ezlooper engine :(");
  gui = EZgui::getInstance();
	gui->init();		

  // start threads
	if( ez->midiIn->ports.size() > 0 )
		threads[0] = CreateThread(NULL,0,thread_midi,  &threads[0],0,&genericThread);	
	threads[1] = CreateThread(NULL,0,thread_audio, &threads[1],0,&genericThread);		
  do{
		gui->render();   
		Sleep(30);
  } while ( !gui->done );
  ez->save();
  ez->done = true;
	WaitForMultipleObjects( 2, threads, true, INFINITE );
  
  delete ez;
  delete gui;
	#ifdef DEBUG
	//check_leaks();
	#endif  

  return EXIT_SUCCESS;
}
