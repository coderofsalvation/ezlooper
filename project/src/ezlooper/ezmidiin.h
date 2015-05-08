/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CLASSNAME : ezcontrol
 *
 * author  : sQuezel (www.boompje.com  www.mannetje.org)
 * function: handles CONTROL-events
 * usage   : this class is used from within doEvent() in ezengine-class
 * notes   :
 */

#ifndef EZCONTROLMIDIIN_H
#define EZCONTROLMIDIIN_H

#ifdef WIN32
#include <windows.h>
#endif
#include <string>
#include <vector>
#include "../util/RtMidi/RtMidi.h"
#include <time.h>

using namespace std;

class ezlooper;
class ezmidiin{
  // Associations
  // Attributes
  private:         
		bool 	  enabled;
		ezlooper *ez;
		int 		ctrlCache[127];
		bool 		limitRingUp;
		bool 		limitRingDown;
		int  		timerStop;
		int  		soundFxBank;	
		clock_t movement_timer;
		bool 		checkFPS( int framerate );
  
  // Operations  
  public: 
		vector<string>		ports;
		RtMidiIn		*iface;
    ezmidiin( ezlooper *ez );
    ~ezmidiin();    
    void process( EZEvent e, int arg1 );
    void init(  );
		void addPort( int device );
		void enable( bool state );		
		
		void processMidiNote( std::vector<unsigned char> *_message);
		void processMidiCtrl( std::vector<unsigned char> *_message);
		void doPitchChange( int channel, int ctrlNum, std::vector<unsigned char> *_message );
		void doMarker( int channel, int marker );
};

#endif
