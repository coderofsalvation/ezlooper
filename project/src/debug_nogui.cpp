#include <cstdarg>


// DEBUG FUNCS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/* name : _say( str, args )
 * usage: global debug function
 * notes: pass printf(..)-like format strings
 */
void _say( char *msg, ... ){
  // do some funky formatstring stuff!
  va_list ap;
  const char *ptr = msg;
  char result[255];
  va_start(ap, msg);
    vsprintf( result, ptr, ap);	
  va_end(ap);	    
  std::string saystr = std::string("ezlooper> ");
  saystr.append( result );
  saystr.append( "\r\n" );
  // write to screen!  
	printf(saystr.c_str());
}


void _popup( char *msg, ... ){	
  // do some funky formatstring stuff!
  va_list ap;
  const char *ptr = msg;
  char result[255];
  va_start(ap, msg);
    vsprintf( result, ptr, ap);	
  va_end(ap);	    
  MessageBox(NULL,result,"EZ Looper message",MB_OK ); 
}

void _error( char *msg, ... ){
  // do some funky formatstring stuff!
  va_list ap;
  const char *ptr = msg;
  char result[255];
  va_start(ap, msg);
    vsprintf( result, ptr, ap);	
  va_end(ap);	    
  MessageBox(NULL,result,"error",MB_OK );
  PostQuitMessage (0);  
}

#ifdef DEBUG

void _debug( char *msg, ... ){
  // do some funky formatstring stuff!
  va_list ap;
  const char *ptr = msg;
  char result[255];
  va_start(ap, msg);
    vsprintf( result, ptr, ap);	
  va_end(ap);	    
  std::string saystr = std::string("ezlooper> ");
  saystr.append( result );
  saystr.append( "\r\n" );
  #ifdef DEBUG  
  // write to screen!  
	printf(saystr.c_str());
  #endif
}

#else
void _debug( char *msg, ... ){	}
#endif
