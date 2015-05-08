#include "EZgui.h"
#include "../ezlooper/ezchannel.h"
#include "help.h"

#define GET_STATUS_FPS 5

/* name : Instance()
 * usage: call this to get the active EZgui ptr
 * notes: singleton design pattern
 */
EZgui* EZgui::pinstance = 0;// initialize pointer
EZgui* EZgui::getInstance () 
{
  if (pinstance == 0)  // is it the first call?
  {  
    pinstance = new EZgui; // create sole instance
  }
  return pinstance; // address of sole instance
}

EZgui::EZgui() : Listener(){
	width 				= 800;
	height 				= 600;
	done 					= false;
	showhelp 			= false;
	display 			= NULL;
	bg 						= NULL;
	stat 					= NULL;
	colorB.r 			= colorB.g = colorB.b = 0.0;
	colorF.r 			= -0.1; 
	colorF.g 			= -0.1; 
	colorF.b 			= -0.1;	
	colorFB.r 		= -0.3;
	colorFB.g 		= -0.3;
	colorFB.b 		= -0.3;	
	borderD.r 		= 0.2; 	
	borderD.g 		= 0.2; 	
	borderD.b 		= 0.2;			
	border.r 			= 0.5; 	
	border.g 			= 0.5; 	
	border.b 			= 0.5;		
	green.r 			= 0.5; 	
	green.g 			= 1.0; 	
	green.b 			= 0.5;			
	white.r 			= 1.0;
	white.g 			= 1.0;
	white.b 			= 1.0;
	
	ez = ezlooper::Instance();
	// layout margins
	loc["margin"] 			= 25;
	loc["spectrum_w"]		= 365;
	loc["spectrum_h"]		= 60;
	loc["scope_y"] 			= 23;
	loc["spectrum_A_x"]	= loc["margin"];
	loc["spectrum_A_y"]	= loc["margin"];	
	loc["spectrum_B_x"]	= 400 + (loc["margin"]/2);
	loc["spectrum_B_y"]	= loc["margin"];	
	loc["deck_A_x"]  		= loc["margin"];
	loc["deck_A_y"]  		= loc["spectrum_h"] - 13;	
	loc["deck_B_x"]			= loc["spectrum_B_x"] ;
	loc["deck_B_y"]			= loc["spectrum_h"] - 13;	
	loc["lp_margin_x1"]	= loc["spectrum_A_x"];
	loc["lp_margin_x2"]	= loc["spectrum_B_x"];
	loc["lp_margin_y"]	= loc["spectrum_h"] + loc["margin"] + 50;	
	loc["lp_width"]			= loc["spectrum_w"];
	loc["lp_height"]		= 34;	
}

EZgui::~EZgui(){
	_say("EZgui::destruct!");	
}

void EZgui::init(){
	bg 		= new image();
	help 	= new image();
	// setup display
	display = new Display( "EZ Looper - live loopsystem (F1 for help)", width, height, Output::Windowed );//Output::Fullscreen );
	display->listener( this );	
	pixels.resize( width * height );			
	

	// load fonts from disk
	//	fonts.push_back( new font( "data/font_terminal_8.gif", 8 ) );
	//	fonts.push_back( new font( "data/font_myriad_36.gif", 36, -20 ) );
	// load fonts from memory
	fonts.push_back( new font( "data/font_terminal_8.gif", 8, 0, TERMINAL ) );
	fonts.push_back( new font( "data/font_myriad_36.gif", 36, -20, MYRIAD ) );
	
	// load image
	if( !loadImage("ezlooper.gif", bg ) )
		_error("could not load/find background 'ezlooper.gif'");	
	else if( !((bg->width == 800) && (bg->height == 600 )) )
		_error("file 'ezlooper.gif' doesn't match 800x600 dimensions");	
	loadImage("help.gif", help, image_help, 74764, true );
		
}

void EZgui::render(){
  unsigned int index = 0;
	if( display == NULL )
		return;
	
	if( checkFPS( GET_STATUS_FPS ) || redrawAll || redrawBG || this->stat == NULL)
		this->stat = ez->getStatus();

    
	// draw background
	if( bg != NULL && (redrawAll || redrawBG ))
	 	drawImage( pixels, 0, 0, bg );
	// draw spectrums
	drawSpectrums( stat->playing[0], 
								 stat->playing[1],
								 false,
								 false,
								 stat->selectedChannel );
								 
	// draw locators
	drawLocators(); 

	if( redrawAll || redrawLooppoints ){		
		string title[2];

		//_debug("redrawALL || redrawLooppoints playing %i/%i", stat->playing[0], stat->playing[1]);		
			
	 	for( int i = 0; i < PLAYERS; i++ )
			title[i] = stat->filename[i].substr(0,22);
		// draw title 
		fonts[1]->draw( title[0], loc["deck_A_x"]+5, loc["deck_A_y"]+40, pixels);  	
		fonts[1]->draw( title[1], loc["deck_B_x"]+5, loc["deck_B_y"]+40, pixels);  		
		
		// draw looppoints
		for( int i = 0; i < PLAYERS; i++ ){
			if( ez->file_dir[i].size() == 0 ){
				switch( i ){
					case 0: fonts[0]->draw( "PRESS F2 TO PICK DIRECTORY", loc["deck_A_x"]+11, loc["deck_A_y"]+15, pixels); break;
					case 1: fonts[0]->draw( "PRESS F3 TO PICK DIRECTORY", loc["deck_B_x"]+11, loc["deck_B_y"]+15, pixels); break;
				}
				continue;
			}
			float speed = stat->speed[i]+1.0f;
			bool highlight = (stat->selectedChannel == i );

			// THE CODE BELOW can cause a crash when a new track is loaded
			if( stat->countLoopPoints[i] != -1 ){
				for( int j = 0; j < stat->countLoopPoints[i]; j++ ){
				 	int x = ( i == 0 ) ? loc["lp_margin_x1"] : loc["lp_margin_x2"];
				 	int y = loc["lp_margin_y"] + (j * (loc["lp_height"]+10));
				 	int lpwidth = loc["lp_width"];
				 	int lpheight = loc["lp_height"];	
				 	if( ez->audio->channel[i]->loopPointsLoaded ){
						if( j == stat->currentLoopPoint[i] )
							drawLoopPoint( j+1, ez->getLoopPoint( j, i ).c_str(), LOOPPOINT_SELECTED, x, y, lpwidth, lpheight, highlight );	
						else if( j == stat->nextLoopPoint[i] )
							drawLoopPoint( j+1, ez->getLoopPoint( j, i ).c_str(), LOOPPOINT_NEXT, 		x, y, lpwidth, lpheight, highlight );	
						else
							drawLoopPoint( j+1, ez->getLoopPoint( j, i ).c_str(), LOOPPOINT_NORMAL, 	x, y, lpwidth, lpheight, highlight );	
					}	
				}			
			}				
		}
	}
	// draw pitch  	
	for( int i = 0; i < PLAYERS; i++ ){
		if( stat->filename[i].size() > 0 ){
			char msg[255];
			if( this->stat->bend[ i ] || (ez->audio->channel[ ez->selectedChannel ]->pressedLock && this->stat->playing[i] )){
				sprintf(msg,"PITCH %f %s",this->stat->speed[i], (stat->bend[i]) ? (stat->bendDir[i] ? "+" : "-") : " " );
				fonts[1]->draw( msg, ((i == 0 ) ? loc["deck_A_x"] : loc["deck_B_x"] )+5, loc["deck_A_y"]+5, pixels);  	
			}
		}             
	} 

	
	// draw loop on /off
	string loop[2];
 	for( int i = 0; i < PLAYERS; i++ ){
		char 		msg[255];
		string loopmsg;
		switch( stat->loop[i] ){
			case 0: loopmsg = "OFF";  break;			
			case 1: loopmsg = "ON";   break;	
			case 2: loopmsg = "ON++"; break;				
		}
		sprintf(msg,"LOOP: %s", loopmsg.c_str() );
		loop[i] = msg;
	}	
	fonts[0]->draw( loop[0], 	loc["deck_A_x"]+280,loc["deck_A_y"]+15, pixels);		
	fonts[0]->draw( loop[1], 	loc["deck_B_x"]+280,loc["deck_B_y"]+15, pixels);				
	
	// show soundfx if needed
	if( showSoundFx != -1 )
		drawSoundFx();		
	
	// draw help if needed
	if( help != NULL && showhelp )
	 	drawImage( pixels, 23, 133, help );

	//_say("All/bg/looppoints = %i/%i/%i\n",redrawAll,redrawBG,redrawLooppoints);
	redrawAll = redrawBG = redrawLooppoints = false;
	
	display->update( pixels ); 		
	done = !display->open();
	flash = checkFPS( 19 );
}
  
bool EZgui::loadImage( const char filename[], image *img, unsigned char *buf, int bufsize, bool usebuf )
{
	FILE* file;
	long i, j;
	unsigned char* buffer;
	unsigned char *image;
	unsigned int index = 0;	
	unsigned int pixelIndex = 0;
	CGifDecoder decoder;
	if( !usebuf ){
		if( file = fopen( filename, "rb") ){
			//	read gif file
			fseek(file, 0, 0x2);
			i = ftell(file);
			buffer = new unsigned char[i];
			fseek(file, 0, 0x0);
			fread(buffer, i, 1, file);
			fclose(file);
			//	decode 
			j = decoder.Open(buffer, i);
			decoder.Decode();
			image = decoder.GetImageBuffer();
		}else return false;
	}else{
		j = decoder.Open(buf, bufsize);
		decoder.Decode();
		image = decoder.GetImageBuffer();		
	}
	img->width  = decoder.GetWidth();
	img->height = decoder.GetHeight(); 	
	_debug("file = %s height = %i width = %i", filename, img->height,img->width );
	img->data.resize( img->width * img->height );
	img->data.clear();
	for( int i = 0; i < (img->width * img->height); i++ ){
		Pixel *p = new Pixel();
		p->b = image[index*3+0] * 1.0f / 255.0f;
		p->g = image[index*3+1] * 1.0f / 255.0f;
		p->r = image[index*3+2] * 1.0f / 255.0f;
		img->data.push_back( p );
		++index;
	}		
	decoder.Close();
	if( !usebuf )
		delete[] buffer;	
	return true;
}

void EZgui::drawImage( vector<Pixel> & surface, unsigned int x, unsigned y, image *img, int transparentIndex )
{
	if( img == NULL || img->data.size() == 0 )
		return;
	unsigned int 	xjump,yjump,index;
	bool 					transparent = ( transparentIndex >= 0 );
	float 				transValue = transparent ?	img->data[ transparentIndex ]->r + 
																						img->data[ transparentIndex ]->g +
																						img->data[ transparentIndex ]->b : -1.0f;
	index = x + (y*display->width());
	xjump = ( width > img->width ) 		? width  - img->width  : 0;
	yjump = ( height > img->height )	? height - img->height : 0;
	
	for ( int y = 0; y < img->height; y++ ){
		for ( int x = 0; x < img->width; x++ ){
			Pixel &pixel = surface[index];	
			int ptr = x+(y*img->width);
			if( transValue != (	img->data[ ptr ]->r + 
													img->data[ ptr ]->b + 
													img->data[ ptr ]->g ) ){
				pixel.b = img->data[ ptr ]->b;
				pixel.g = img->data[ ptr ]->g;
				pixel.r = img->data[ ptr ]->r;
			}
	    index++;
		}
		index += xjump;
		if( index > surface.size()-1 )
			break;
	}
}

void EZgui::drawBlock( 	vector<PixelToaster::Pixel> &surface, 
												int surfaceWidth,
												int surfaceHeight,
												int x, 
												int y, 
												int width, 
												int height,
												Pixel color,
												bool fadeout )
{
	for( int i = 0; i < height; i++ ){
		for( int j = 0; j < width; j++ ){    
			PixelToaster::Pixel &p = surface[ (x+j) + ((y+i)*surfaceWidth) ];
			if( fadeout ){ // dim color to black
				// limit the colors, so they don't distort
				if(p.r + color.r < 0.0 ) color.r = -p.r; 
				if(p.r + color.r > 1.0 ) color.r = 1.0-p.r;
				if(p.g + color.g < 0.0 ) color.g = -p.g; 
				if(p.g + color.g > 1.0 ) color.g = 1.0-p.g;
				if(p.b + color.b < 0.0 ) color.b = -p.b; 
				if(p.b + color.b > 1.0 ) color.b = 1.0-p.b;
				// apply the fade
				p.r += color.r; p.g += color.g; p.b += color.b;								
			}else{
				p.r = color.r; 
				p.b = color.b; 
				p.g = color.g; 		
			}
		}
	}
}

void EZgui::onKeyDown( DisplayInterface & display, Key key )
{
	switch( key ){
		case Key::F1:
			showhelp = !showhelp;
			redraw(ALL,2342);
			break;		
		
		case Key::F2:
			ez->selectedChannel = 0;
			ez->settings->v.dir[0] = ez->pickDir();	
			ez->file_dir[0] 			 = ez->settings->v.dir[0]; 
			ez->save();
			ez->scanAllFiles(0);
			redrawAll = true;
			break;

		case Key::F3:
			ez->selectedChannel = 1;			
			ez->settings->v.dir[1] = ez->pickDir();
			ez->file_dir[1] 			 = ez->settings->v.dir[1]; 
			ez->save();
			ez->scanAllFiles(1);
			redrawAll = true;			
			break;
					
		case Key::Enter:   // 'p' - PLAY
			ez->audio->doEvent( PLAY );
			break;
			
		case Key::Space:   // 's' - STOP
			ez->audio->doEvent( STOP );
			break;

		case Key::Down:    // down arrow
			ez->audio->doEvent( SPEEDDOWN );			
			break;		

		case Key::Up:    // up arrow
 			ez->audio->doEvent( SPEEDUP );			
 			break;

		case Key::Subtract:    // '-'
			ez->audio->doEvent( PITCHDOWN );
 			ez->audio->doEvent( SPEEDDOWN );			
			break;

		case Key::Add: 		// '+' 
			ez->audio->doEvent( PITCHUP );
 			ez->audio->doEvent( SPEEDUP );			
			break;
		
		case Key::L:
			ez->audio->channel[ ez->selectedChannel ]->toggleLoop();
			redraw(ALL,2342);
			break;

    case Key::Multiply:
         ez->audio->doEvent( PITCHRESET );
         break;
    
    case Key::Period:{
         ez->nextFile(1);
				}
         break;
    
    case Key::Comma:
         ez->nextFile(-1);
         break;
             
		case Key::One:
		case Key::Two:
		case Key::Three:
		case Key::Four:
		case Key::Five:
		case Key::Six:
		case Key::Seven:
		case Key::Eight:
		case Key::Nine:		
			_debug("pressed key marker %i",key-49);
			ez->setLoopPoint( ez->selectedChannel,key-49 );
			break;
			
		case Key::Tab:		// 'TAB' - toggle channel
			ez->toggleChannel();
			break;
			   
		case Key::O:{
			/*
			HWND h = CreateWindow("STATIC", 
                             "", 
                             WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 
                             5,                         
                             500,
                             200, 
                             500, 
                             NULL, //parent window  
                             NULL,     
                             0, 
                             0);
			_popup("dsp = %i", ez->audio->channel[ ez->selectedChannel ]->fx["1_filter.dll"]->dsp != NULL );
			FMOD_DSP_ShowConfigDialog( ez->audio->channel[ ez->selectedChannel ]->fx["1_filter.dll"]->dsp, h, true );	
			*/
		}break;			   
			   
	  /* SOUNDFX KEYS! */			
		case Key::Q:
			ez->audio->playSoundFx( 0, ez->selectedChannel );						
			break;
		case Key::W:
			ez->audio->playSoundFx( 1, ez->selectedChannel );									
			break;
		case Key::E:
			ez->audio->playSoundFx( 2, ez->selectedChannel );									
			break;
		case Key::R:
			ez->audio->playSoundFx( 3, ez->selectedChannel );									
			break;						
		case Key::A:
			ez->audio->playSoundFx( 4, ez->selectedChannel );						
			break;						
		case Key::S:
			ez->audio->playSoundFx( 5, ez->selectedChannel );									
			break;						
		case Key::D:
			ez->audio->playSoundFx( 6, ez->selectedChannel );									
			break;						
		case Key::F:
			ez->audio->playSoundFx( 7, ez->selectedChannel );									
			break;						
		case Key::Z:
			ez->audio->playSoundFx( 8, ez->selectedChannel );						
			break;						
		case Key::X:
			ez->audio->playSoundFx( 9, ez->selectedChannel );									
			break;						
		case Key::C:
			ez->audio->playSoundFx( 10, ez->selectedChannel );									
			break;						
		case Key::V:
			ez->audio->playSoundFx( 11, ez->selectedChannel );									
			break;						
			
	}	
}

void EZgui::onKeyPressed( DisplayInterface & display, Key key )
{
    //_popup( "onKeyPressed: key=%s\n", getKeyString(key) );
    
}

void EZgui::onKeyUp( DisplayInterface & display, Key key )
{
    //_popup( "onKeyUp: key=%s\n", getKeyString(key) );
}

void EZgui::destroyImage( image *img ){
	_popup("EZgui::destroy image!");
	// to be implemented
}

const char * EZgui::getKeyString( Key key )
{
    switch (key)
    {
        case Key::Enter: return "Enter";
        case Key::BackSpace: return "BackSpace";
        case Key::Tab: return "Tab";
        case Key::Cancel: return "Cancel";
        case Key::Clear: return "Clear";
        case Key::Shift: return "Shift";
        case Key::Control: return "Control";
        case Key::Alt: return "Alt";
        case Key::Pause: return "Pause";
        case Key::CapsLock: return "CapsLock";
        case Key::Escape: return "Escape";
        case Key::Space: return "Space";
        case Key::PageUp: return "PageUp";
        case Key::PageDown: return "PageDown";
        case Key::End: return "End";
        case Key::Home: return "Home";
        case Key::Left: return "Left";
        case Key::Up: return "Up";
        case Key::Right: return "Right";
        case Key::Down: return "Down";
        case Key::Comma: return "Comma";
        case Key::Period: return "Period";
        case Key::Slash: return "Slash";
        case Key::Zero: return "Zero";
        case Key::One: return "One";
        case Key::Two: return "Two";
        case Key::Three: return "Three";
        case Key::Four: return "Four";
        case Key::Five: return "Five";
        case Key::Six: return "Six";
        case Key::Seven: return "Seven";
        case Key::Eight: return "Eight";
        case Key::Nine: return "Nine";
        case Key::SemiColon: return "SemiColon";
        case Key::Equals: return "Equals";
        case Key::A: return "A";
        case Key::B: return "B";
        case Key::C: return "C";
        case Key::D: return "D";
        case Key::E: return "E";
        case Key::F: return "F";
        case Key::G: return "G";
        case Key::H: return "H";
        case Key::I: return "I";
        case Key::J: return "J";
        case Key::K: return "K";
        case Key::L: return "L";
        case Key::M: return "M";
        case Key::N: return "N";
        case Key::O: return "O";
        case Key::P: return "P";
        case Key::Q: return "Q";
        case Key::R: return "R";
        case Key::S: return "S";
        case Key::T: return "T";
        case Key::U: return "U";
        case Key::V: return "V";
        case Key::W: return "W";
        case Key::X: return "X";
        case Key::Y: return "Y";
        case Key::Z: return "Z";
        case Key::OpenBracket: return "OpenBracket";
        case Key::BackSlash: return "BackSlash";
        case Key::CloseBracket: return "CloseBracket";
        case Key::NumPad0: return "NumPad0";
        case Key::NumPad1: return "NumPad1";
        case Key::NumPad2: return "NumPad2";
        case Key::NumPad3: return "NumPad3";
        case Key::NumPad4: return "NumPad4";
        case Key::NumPad5: return "NumPad5";
        case Key::NumPad6: return "NumPad6";
        case Key::NumPad7: return "NumPad7";
        case Key::NumPad8: return "NumPad8";
        case Key::NumPad9: return "NumPad9";
        case Key::Multiply: return "Multiply";
        case Key::Add: return "Add";
        case Key::Separator: return "Separator";
        case Key::Subtract: return "Subtract";
        case Key::Decimal: return "Decimal";
        case Key::Divide: return "Divide";
        case Key::F1: return "F1";
        case Key::F2: return "F2";
        case Key::F3: return "F3";
        case Key::F4: return "F4";
        case Key::F5: return "F5";
        case Key::F6: return "F6";
        case Key::F7: return "F7";
        case Key::F8: return "F8";
        case Key::F9: return "F9";
        case Key::F10: return "F10";
        case Key::F11: return "F11";
        case Key::F12: return "F12";
        case Key::Delete: return "Delete";
        case Key::NumLock: return "NumLock";
        case Key::ScrollLock: return "ScrollLock";
        case Key::PrintScreen: return "PrintScreen";
        case Key::Insert: return "Insert";
        case Key::Help: return "Help";
        case Key::Meta: return "Meta";
        case Key::BackQuote: return "BackQuote";
        case Key::Quote: return "Quote";
        case Key::Final: return "Final";
        case Key::Convert: return "Convert";
        case Key::NonConvert: return "NonConvert";
        case Key::Accept: return "Accept";
        case Key::ModeChange: return "ModeChange";
        case Key::Kana: return "Kana";
        case Key::Kanji: return "Kanji";
        default: return "Undefined";
    }
}

void EZgui::drawSpectrums( bool drawA, bool drawB, bool fadeA, bool fadeB, int highlight ){
	if( drawA )
		drawBlock( 	pixels, width, height, 
								loc["spectrum_A_x"],
								loc["spectrum_A_y"],
								loc["spectrum_w"],
								loc["spectrum_h"],
								fadeA ? colorF : colorB,
								fadeA );	
	if( drawB )									
		drawBlock( 	pixels, width, height, 
								loc["spectrum_B_x"],
								loc["spectrum_B_y"],
								loc["spectrum_w"],
								loc["spectrum_h"],
								fadeB ? colorF : colorB,
								fadeB );	
	
	if( drawA )								
		spectrum::drawSpectrum( pixels, 
														width, height, 
														loc["spectrum_A_x"],
														loc["scope_y"],
														loc["spectrum_w"],
														loc["spectrum_h"]-3,
														ez->audio->channel[0]->channel,
														false,
														highlight == 0 );
	if( drawB )																						
		spectrum::drawSpectrum( pixels, 
														width, height, 
														loc["spectrum_B_x"],
														loc["scope_y"],
														loc["spectrum_w"],
														loc["spectrum_h"]-3,
														ez->audio->channel[1]->channel,
														false,
														highlight == 1 );
}

void EZgui::drawSoundFx(){
	int xoffset = 200;
	int yoffset = loc["lp_margin_y"]-30;
	int height 	= 260;
	drawBlock( 	pixels, width, height, 
							loc["spectrum_A_x"]+xoffset, 
							loc["spectrum_A_y"]+yoffset, 
							loc["spectrum_w"],													
							height-1,
							colorB,
							false );		
	drawRect( loc["spectrum_A_x"]+xoffset, 
						loc["spectrum_A_y"]+yoffset, 
						loc["spectrum_A_x"] + xoffset+ loc["spectrum_w"],													
						loc["spectrum_A_y"] + yoffset + height,
						border );
	fonts[1]->draw( "SELECT SAMPLEBANK", loc["spectrum_A_x"]+12+xoffset, loc["spectrum_A_y"]+yoffset+12, pixels);  																
	char msg[255];						
	for( int i = 0; i < ez->filesfx[0].size() && i < 16; i++ ){	
		bool selected = ( (showSoundFx == 0 ) && ( i >= 0 && i 	< 4 ) ||
											(showSoundFx == 1 ) && ( i >= 4 && i 	< 8 ) ||
											(showSoundFx == 2 ) && ( i >= 8 && i 	< 12 ) ||		
											(showSoundFx == 3 ) && ( i >= 12 && i < 16 ) );
		sprintf(msg,"%s %i %s", selected ? ">>" : "  ", (i%4)+1, ez->filesfx[0][i].c_str() );
		fonts[0]->draw( msg, loc["spectrum_A_x"]+7+xoffset, loc["spectrum_A_y"]+(yoffset+57+(i*12)), pixels);  										
	}	
}

void EZgui::drawLocators(){	
	if( stat == NULL )
		return;																									
	int locatorHeight = loc["spectrum_h"];	
	drawBlock( 	pixels, width, height, 
							loc["spectrum_A_x"], 
							loc["spectrum_A_y"], 
							loc["spectrum_w"],													
							locatorHeight-1,
							colorF,
							true );		

	drawBlock( 	pixels, width, height, 
							loc["spectrum_B_x"], 
							loc["spectrum_B_y"], 
							loc["spectrum_w"],													
							locatorHeight-1,
							colorF,
							true );																																																											
	drawRect( loc["spectrum_A_x"], 
						loc["spectrum_A_y"], 
						loc["spectrum_A_x"] + loc["spectrum_w"],													
						loc["spectrum_A_y"] + locatorHeight,
						border );
	drawRect( loc["spectrum_B_x"], 
						loc["spectrum_B_y"], 
						loc["spectrum_B_x"] + loc["spectrum_w"],													
						loc["spectrum_B_y"] + locatorHeight,
						border );			
	for( int i = 0; i < PLAYERS; i++ ){
		eztrack *t = ez->audio->channel[ i ]->track;
		if( t == NULL || !t->soundLoaded	)
			return;
		int currentLoopPoint = stat->currentLoopPoint[i];
		int xloc;
		int x = xloc = ( i == 0 ) ? loc["spectrum_A_x"] : loc["spectrum_B_x"];
		int y = ( i == 0 ) ? loc["spectrum_A_y"] : loc["spectrum_B_y"];			
		// draw markers
		fonts[0]->draw( "1", x+3, y+2, pixels);
		for( int j = 0; j < stat->countLoopPoints[i]; j++ ){			
			char msg[5];			
			sprintf(msg, "%i", j+2 );
			if( j == currentLoopPoint )
				drawBlock( 	pixels, width, height, 
										x+1, 
										y+1, 
										((loc["spectrum_w"]/100)* t->loopPoints[ j ]->length)-1,													
										(locatorHeight/2)-1,
										borderD,
										false );				
			x += (loc["spectrum_w"]/100) * t->loopPoints[ j ]->length;
			if( j != stat->countLoopPoints[i]-1){ // don't draw last marker
				line( x, y+1, x, y + locatorHeight/2, green );  						
				fonts[0]->draw( msg, x+3, y+2, pixels);  			
			}
		}
		x = xloc = ( i == 0 ) ? loc["spectrum_A_x"] : loc["spectrum_B_x"];
		// draw hidden markers *TODO*!!!!
	  map<int, marker *>::iterator iter;   
	  for( iter = t->hiddenMarkers.begin(); iter != t->hiddenMarkers.end(); ++iter ) {			
			x = (loc["spectrum_w"]/100) * iter->second->percent;			
			line( xloc + x, y+1, xloc + x, y + locatorHeight/3, border );  									
		}
		// draw position
		xloc += (loc["spectrum_w"]/100) * stat->loopLocation[i];
		if( flash )
			line( xloc, y+1, xloc, y + locatorHeight-1, border );  								
	}						
							
}

void EZgui::drawLoopPoint( int index, const char *name, LOOPPOINT l, int x, int y, int lpwidth, int lpheight, bool highlight )
{
	if( name == NULL )
		return;
	char label[255];
	sprintf(label,"%i.  %s",index, name );
	uppercase( label );
	switch( l ){
		case LOOPPOINT_NORMAL:{
			Pixel h = border;
			h.r = h.b = h.g = -h.r;
			drawBlock( 	pixels, width, height, x, y, lpwidth, lpheight, highlight ? h : colorFB, highlight ? false : true  );
			break;
		}			
		
		case LOOPPOINT_SELECTED:{
			drawBlock( 	pixels, width, height, x, y, lpwidth, lpheight, colorB, false  );
			for( int i = 0; i < 3; i++ )
				line( x+5+(i*3), y+5, x+5+(i*3), y+(lpheight-5), green );
			break;
		}

		case LOOPPOINT_NEXT:{
			drawBlock( 	pixels, width, height, x, y, lpwidth, lpheight, colorB, false  );
			for( int i = 0; i < 3; i++ )
				line( x+5+(i*3), y+5, x+5+(i*3), y+(lpheight-5), borderD );
			break;
		}			
	}
	if( highlight )
		drawRect( x, y, x+lpwidth, y+lpheight, colorB);
	fonts[0]->draw( label, x+20, y+11, pixels);  		
}

void EZgui::uppercase(char *lc) {
    for(;*lc;lc++)
        *lc = (unsigned char)toupper((unsigned char)*lc);
}

int EZgui::sgn (long a) {
  if (a > 0) return +1;
  else if (a < 0) return -1;
  else return 0;
}

void EZgui::drawRect( int x1, int y1, int x2, int y2, Pixel color ){
	line( x1, y1, x2, y1, color );	
	line( x2, y1, x2, y2, color );	
	line( x2, y2, x1, y2, color );		
	line( x1, y2, x1, y1, color );			
}
		
void EZgui::line( int x1, int y1, int x2, int y2, Pixel color ) {
  long u,s,v,d1x,d1y,d2x,d2y,m,n;
  int  i;
  u   = x2-x1;
  v   = y2-y1;
  d1x = sgn(u);
  d1y = sgn(v);
  d2x = sgn(u);
  d2y = 0;
  m   = abs(u);
  n   = abs(v);
  if (m<=n) {
    d2x = 0;
    d2y = sgn(v);
    m   = abs(v);
    n   = abs(u);
  }
  s = (int)(m / 2);
  for (i=0;i<round(m);i++) {
    //    Putpixel(a,b,col);
    Pixel &p = pixels[ x1 + (y1*width) ];
    p.r = color.r;
    p.g = color.g;
    p.b = color.b;
    s += n;
    if (s >= m) {
      s -= m;
      x1 += d1x;
      y1 += d1y;
    }
    else {
      x1 += d2x;
      y1+= d2y;
    }
  }
}
