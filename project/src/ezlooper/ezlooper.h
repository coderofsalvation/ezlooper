/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CLASSNAME : ezlooper
 *
 * author: sQuezel (www.boompje.com  www.mannetje.org)
 * function: core engine, wrapper of MVC design
 * usage: with care
 * notes: use singleton
 *
 * FUTURE : vst support, config midi-events thru blopscript, gui?
 *
 */
#ifndef EZLOOPER_H
#define EZLOOPER_H

#include <windows.h>
#include <shlobj.h>
#include <vector>
#include <map>
#include "ezlooperTypes.h"
#include "ezsettings.h"
#include "ezfmod.h"
#include "ezmidiin.h"
#include "eztrack.h"
#include "iniFile.h"

class ezcontrol;
class ezfmod;
class ezmidiin;
class ezlooper {
  // Attributes
  public:
		
		string															dir;
		int																	selectedChannel;
		vector<string>											files[PLAYERS];		
		vector<string>											filesfx[1];				
		vector<string>											vst[PLAYERS];				
		string 															file_dir[PLAYERS];
		int																	selectedFile[PLAYERS];
		ezsettings													*settings;
		ezmidiin														*midiIn;		
		ezfmod															*audio;							
    static ezlooper*        						Instance();    
		~ezlooper();
    bool 																done;
    
    
  // Operations
  protected:
    ezlooper();
    ezlooper(const ezlooper&);
    ezlooper& operator = (const ezlooper &);
  
  private:
		CIniFile 														*ini;		
    static  														ezlooper *pinstance;
		ezStatus														*status;				
    

  // Associations
  // Attributes
  /* Operations ***************************************************************/
  public:
  bool 		init();              
  void 		initPlugins();
  void 		cleanup();
	void 		save();
	void 		read();
  void 		doEvent( EZEvent e, int value = 0);

  string 	pickDir();
	void 		scanAllFiles( int channel );
  void 		scanFiles( string dir, int channel, vector<string> *files, string wilcard = "" );
  void 		nextFile( int direction );
  void 		toggleChannel();
  void 		setLoopPoint( int channel, int marker );
	string 	getLoopPoint( int nr, int channel ); 
  ezStatus *  getStatus();
};

#endif
