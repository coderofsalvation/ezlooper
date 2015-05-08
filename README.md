## Screenshots

<img alt="" src="screenshot/ezlooper_1.gif"/>
<br>
<img alt="" src="screenshot/ezlooper_2.gif"/>
<br>
<img alt="" src="screenshot/ezlooper_3.gif"/>
<br>
<img alt="" src="screenshot/ezlooper_4.gif"/>
<br>

## Installation

Just grab one of the installers.
Press F1 in the application to understand its workings.
Make looppoints using an capable wave editor (like wavosaur)

## what is it?

EZ Looper is an application build for dj's and livemusicians, originally intended to control it with the MIDIMAN DM2 controller (still works). Its build in 100% pure C++. Basically its just a djplayer with :

> NOTE: seems to work on linux (wine) too

- controllable by keyboard or a midicontroller 
- loopregions inside can be triggered (uses WAV markers) (beta).
- internal VST effects : lowpass/highpass filter + 2x dubdelay
- plays mp3/wav files
- jog/pitch tracks like any other cdplayer in the clubscene.
- GIF as background, so you can style the app to your own taste

## Why another djplayer?

First of all, I wasn't satisfied with the software which came with my mixman dm2 controller. When a producer/dj designs his liveset, sometimes sticking with minimalistic stuff can make things more pleasant. After trying zillions of dj-players, I concluded that it was better to redirect this energy towards building a minimalistic player. I wanted to create a simple loop tool, but it evolved into this since the looppoints arent really stable.

## Focus

The main focus is that you should be able to dj with your eyes closed. A lot of djplayers/sequencers have complex interfaces with tiny fonts, dangerous complex MIDI-options. The workflow should be : get on stage,start your laptop+program, close your laptop.

## Building the source

It was compiled with DevCpp (see projectfiles in /prj).
But it should compile eventually in any mingw compiler.

## Note

Chances for updates are very rare since I kind of abanded the idea which started this app.
