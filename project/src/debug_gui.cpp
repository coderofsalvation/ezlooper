#include <cstdarg>


// DEBUG FUNCS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/* name : _say( str, args )
 * usage: global debug function
 * notes: pass printf(..)-like format strings
 */
void _say( char *msg, ... ){
  // do some funky formatstring stuff!
  // do some funky formatstring stuff!
  va_list ap;
  const char *ptr = msg;
  char result[255];
  va_start(ap, msg);
    vsprintf( result, ptr, ap);	
  va_end(ap);	    
  printf("%s",result);//MessageBox(NULL,result,"EZ Looper message",MB_OK ); 
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
	exit(0);  
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
	printf("debug> %s\n", result );  
}

#else
void _debug( char *msg, ... ){	}
#endif
