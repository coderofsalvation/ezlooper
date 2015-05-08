#include "ezfx.h"

ezfx::ezfx( FMOD_DSP *dsp, string filename, int index ){
	this->dsp = dsp;
	this->filename = filename;	
	this->index = index;
	initParams();
}

ezfx::~ezfx(){}

int ezfx::getDSPIndex(){
	return index;	
}

void ezfx::initParams(){
	if( filename.compare( "1_filter.dll" ) == 0 ){ // if filter

	}
	if( filename.compare( "2_pingpong.dll" ) == 0 ){ // if pingppong delay
		checkError( FMOD_DSP_SetParameter( dsp, 0, 0.3 ) );  	// time	
		checkError( FMOD_DSP_SetParameter( dsp, 1, 0.7 ) );  	// feedbacl
		checkError( FMOD_DSP_SetParameter( dsp, 4, 0.0 ) );  	// wet 0%
		checkError( FMOD_DSP_SetParameter( dsp, 5, 0.9 ) );  	// cutoff
		checkError( FMOD_DSP_SetParameter( dsp, 6, 0.3 ) );  	// resonance
		checkError( FMOD_DSP_SetParameter( dsp, 7, 0.2 ) );  	// filtertype
		checkError( FMOD_DSP_SetParameter( dsp, 8, 0.0 ) );  	// filterweeep
		checkError( FMOD_DSP_SetParameter( dsp, 9, 0.28 ) );  // saturation
	}	
}

void ezfx::moveAxis( EZJoystick e, int value ){
	if( filename.compare( "1_filter.dll" ) == 0 ){ // if filter
		switch( e ){
			case MOVE_TOTAL:{
			  //float fvalue = (1.0f/127.0f)*float(value);
				float fvalue = exp( value/(127.0f/0.69f) )-1.0;
			  float rvalue = ( value > 83 ) ? 0.33 : 0.0;
				checkError( FMOD_DSP_SetParameter( dsp, 0, fvalue ) );				  
				checkError( FMOD_DSP_SetParameter( dsp, 1, rvalue ) );				  				
				break;	
			}
			case MOVE_TOP:{}
			case MOVE_BOTTOM:{}
			case MOVE_LEFT:{}
			case MOVE_RIGHT:{}
			break;	
		}
	}
	if( filename.compare( "2_pingpong.dll" ) == 0 ){ // if pingppong delay
		switch( e ){
			case MOVE_TOP:{}
			case MOVE_BOTTOM:{}
			break;				

			case MOVE_TOTAL:{
				value += 5;
			  float fvalue = ((0.2f/127.0f)*float(value) );			  
				checkError( FMOD_DSP_SetParameter( dsp, 0, fvalue ) );  	// time
				break;	
			}
			case MOVE_LEFT:{
				if( value >= 63 ) return;
				value = 63-value;
			  float fvalue = (0.8f/63.0f)*float(value);
				if( value < 16 ){
					checkError( FMOD_DSP_SetParameter( dsp, 1, 0.0 ) );  	// feedbacl			  															
					checkError( FMOD_DSP_SetParameter( dsp, 4, 0.0 ) );  	// feedbacl			  																				
				}else{
					checkError( FMOD_DSP_SetParameter( dsp, 1, 0.7 ) );  	// feedbacl			  
					checkError( FMOD_DSP_SetParameter( dsp, 7, 0.0 ) );  	// filtertype			  
					checkError( FMOD_DSP_SetParameter( dsp, 9, 0.0 ) );  	// saturation								
					checkError( FMOD_DSP_SetParameter( dsp, 4, fvalue ) );// wet				  										
				}
				break;				
			}
			case MOVE_RIGHT:{
				if( value <= 63 ) return;
				value = value-63;
			  float fvalue = (1.0f/63.0f)*float(value);
				if( value < 16 ){
					checkError( FMOD_DSP_SetParameter( dsp, 1, 0.0 ) );  	// feedbacl			  															
					checkError( FMOD_DSP_SetParameter( dsp, 4, 0.0 ) );  	// feedbacl			  																				
				}else{
					checkError( FMOD_DSP_SetParameter( dsp, 1, 0.7 ) );  	// feedbacl			  			  
					checkError( FMOD_DSP_SetParameter( dsp, 7, 0.2 ) );  	// filtertype			  			  
					checkError( FMOD_DSP_SetParameter( dsp, 9, 0.18 ) );  // saturation				
					checkError( FMOD_DSP_SetParameter( dsp, 4, fvalue ) );// wet					
				}
				break;
			}
		}
	}	
}
