#include "stdio.h"
#include "ezsettings.h"

ezsettings::ezsettings(){
	this->readSucces = false;
}

ezsettings::~ezsettings(){
}

bool ezsettings::read(char *filename){
	FILE *inFile = NULL;
	if( (inFile = fopen(filename, "r")) == NULL)	
 		this->readSucces = false;	
	else{
		fread(&v,sizeof(v),1,inFile);	
		fclose(inFile);
		this->readSucces = true;								
	}		
	return this->readSucces;
}


bool ezsettings::write(char *filename){
FILE *outFile;

	  /* open the file we are writing to */
	  if(!(outFile = fopen(filename, "w")))
	     return false;

	  /* use fwrite to write binary data to the file */
	  fwrite(&v,sizeof(v),1,outFile);
	  
	  fclose(outFile);
	  return true;
}

void ezsettings::print(){
// not implemented
}

