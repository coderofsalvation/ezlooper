#ifndef EZLOOPERTYPES_H
#define EZLOOPERTYPES_H

#include <string>
#include <fmod.h>

using namespace std;

#define	PLAYERS											2

#define FILE_SETTINGS								"ezlooper.ini"
#define FILE_DIR_A									"A"
#define FILE_DIR_B									"B"
#define	FILE_DIR_VST								"VST"
#define	FILE_DIR_FX									"FX"

#define MAX_LOOPPOINTS 							11

#define SPEED_FALL_SLICES						8
#define SPEED_MAXPITCH							0.5
#define SPEED_MAXPITCHCHANGE				1.5
#define SPEED_PITCHBEND							0.15
#define PCM_LOOKAHEAD 							150000
#define DELAY_STOP									1000

#define FRAMERATE_MAX								50
#define FRAMERATE_CONSOLE 					2
#define FRAMERATE_GUI								2
#define FRAMERATE_LOOKAHEAD					4
#define FRAMERATE_PITCHBEND					40
                               
// global functions
extern string pickDir();
extern void 	_say( 	char *msg, ... );
extern void 	_debug( 	char *msg, ... );
extern void 	_error( char *msg, ... );
extern void 	_popup( char *msg, ... );  
extern bool   checkError( FMOD_RESULT r );
extern bool 	checkFPS( int framerate );
extern int 		showSoundFx;

// GUI function <-------------------------- this does not belong with EZ looper engine
extern bool 	bp;
#ifndef REDRAWTYPES
#define REDRAWTYPES
enum REDRAW_TYPE{
	ALL,
	BACKGROUND,
	LOOPPOINTS
};
#endif
extern void	redraw( REDRAW_TYPE t, int id );

// <-------------------------------------- this does not belong with EZ looper engine

// ezos controller
enum EZEvent
{
	EZ_UPDATE,
	EZ_UPDATEMIDI,
	EZ_UPDATEGUI,
	EZ_LOADTRACK,	
	EZ_QUIT,
	EZ_KEYDOWN
};

// audio events
enum EZAudioEvent{
	PLAY,
	STOP,
	SPEEDUP,
	SPEEDDOWN,
	PITCHUP,
	PITCHDOWN,
  PITCHRESET	
};

// joystick event mode
enum EZJoystick{
	MOVE_TOP,
	MOVE_BOTTOM,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_TOTAL
};

// audio events
enum EZAudioState{
	PLAYING	
};

class marker{
	public:
		int index;
		string name;
		int offset;
		int percent;
		int wakeUpOffset;	
};

class loopPoint{
	public:
		marker *begin;
		marker *end;
		int length; // percentage of 100%
		int location; // percentage of 100%		
		
};

class ezStatus{
	public:
		int		 selectedChannel;
		string filename[PLAYERS];
		float  speed[PLAYERS];
		bool   bend[PLAYERS];
		bool   bendDir[PLAYERS];
		int		 countLoopPoints[PLAYERS];
		int		 currentLoopPoint[PLAYERS];
		int		 nextLoopPoint[PLAYERS];
		int  	 loop[PLAYERS];
		int    loopLocation[PLAYERS];
		bool   playing[PLAYERS];		
};

#endif
