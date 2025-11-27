#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>

#include <esat_extra/soloud/soloud.h>
#include <esat_extra/soloud/soloud_wav.h>



#include <stdio.h>



unsigned char fps=25;



double current_time,last_time;



//Control de audio 
//SoLoud::Soloud soloud;
//SoLoud::Wav ;




int esat::main(int argc, char **argv) {

	//soloud.init();
	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);
  
    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    	last_time = esat::Time(); 
    	esat::DrawBegin();
    	esat::DrawClear(0,0,0);


    	esat::DrawEnd();
    	


    	//Control fps 
    	do{
    		current_time = esat::Time();
    	}while((current_time-last_time)<=1000.0/fps);
    	esat::WindowFrame();
  }
  
  
  esat::WindowDestroy();
  //soloud.deinit();
  
  return 0;
    
}