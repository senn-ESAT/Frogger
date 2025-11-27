#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>

esat::SpriteHandle imagen;

unsigned char fps=15; //Control de frames por segundo
double current_time,last_time;

int esat::main(int argc, char **argv) {
  int space[] = {15,184,394,594,769,949,1136,1310};
  int i = 0, j = 0;

	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);
  imagen=esat::SpriteFromFile("../img/hojasprites.png");

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    last_time = esat::Time(); 

    esat::DrawBegin();
    esat::DrawClear(0,0,0);
    esat::DrawSprite(esat::SubSprite(imagen ,space[i], 5, 165, 310), j, 10);
    esat::DrawEnd();
    i++;
    j+= 25;
    if(j>800)
      j = -100;
    if(i>7)//++i%=7;
      i=0;

    //Control fps 
    do{
    	current_time = esat::Time();
    }while((current_time-last_time)<=1000.0/fps);
    esat::WindowFrame();
  }

  esat::SpriteRelease(imagen);
  esat::WindowDestroy();
  return 0;
}
/*
15 5, 
184 5, 
394 5, 
594 3, 
769 5, 
949 5, 
1136 5, 
1310 5.

165x310
*/