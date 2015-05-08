#include <string>
#include <sstream>
#include "ezlooperTypes.h"

using namespace std;

class ezsettings{

	private:

	public:
		bool readSucces;
		
		struct vars{
			bool usefx;
			int audioDevice[PLAYERS];
			int midiInDevice;
      string dir[PLAYERS];
			int bufferlength;
			int numbuffers;
			int useheadphones;			           	
			//char value[255];
			//bool value;
		}v;	

		ezsettings();
		~ezsettings();
		bool read(char *filename);
		bool write(char *filename);
		void print();
};


