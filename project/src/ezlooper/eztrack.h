#ifndef EZTRACK
#define EZTRACK

#include "ezlooperTypes.h"
#include <vector>
#include <map>

class eztrack{
	private:
		int												currentMarker;
		
	public:
		string										filename;
		string										filenameFull;
		bool											setNextMarkerOnPlay;		
		bool 											mp3;
		int												loopPointCurrent;
		int												loopPointNext;
		int												nextMarker;
		bool											soundLoaded;
		FMOD_SOUND								*sound;
		map<int,marker *>					hiddenMarkers;		
		vector<marker *>					markers;
		vector<loopPoint *>				loopPoints;
		
		eztrack(string filename);
		~eztrack();	
		void cleanup();
		void getSyncPoints();
		void loadSound( FMOD_SYSTEM *sys );
		void setNextLoopPoint( int index, bool setNextMarkerOnPlay = false );
		int  getLength();
		void unload();
		int  getLoopIndex( int pos );
		
};

#endif
