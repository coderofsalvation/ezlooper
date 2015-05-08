#include "sine.h"

#ifdef WINDOWS
  #include <windows.h>
#endif

sine::sine( ) {

}

sine::~sine(){

}

void sine::setSine( float amplitude, int type, int slices ){
	this->sinetable.reserve( slices );
  switch( type ){
    case SINE_FULL:{
      float degree = 360 / (float)slices; 
      for( float i= 0.0 ; i < 360 ; i+=degree)
         this->sinetable.push_back( float(sin( i * (PI/180) ) * amplitude) );
      break;
    }
    case SINE_RISE:{
      float degree = 90 / (float)slices;
      for( int i= 1; i < slices; i++)
        this->sinetable.push_back( float(sin( (i * degree) / (150/PI) ) * amplitude) );
      this->sinetable.push_back( float(amplitude) );
      break;
    }
    case SINE_FALL:{
      float degree = 90 / (float)slices;
      for( float i = 0.0; i < 90.0; i+=degree)
        this->sinetable.push_back( float( sin( (i+90) * (PI/180) ) * amplitude) );
      this->sinetable[ this->sinetable.size()-1 ] = 0.0;
			break;
		}
		
    case SINE_MOUNTAIN:{
      float degree = 180 / (float)slices; 
      for( int i= 1 ; i < slices ; i++)
         this->sinetable.push_back( float(sin( (i * degree) * (PI/180) ) * amplitude) );
      break;
 
    }
    case SINE_BOUNCE:{
      float degree = 180 / (float)slices; 
      for( float i= 0.0 ; i < 180.0 ; i+=degree)
        this->sinetable.push_back(float(sin( (i+90) * (PI/180) ) * amplitude) );
      break;

    }
  }  
}

float sine::getSine(int slice){
  if( slice < this->sinetable.size() )return this->sinetable[ slice ];
  else return this->sinetable[ this->sinetable.size()-1 ];
}


