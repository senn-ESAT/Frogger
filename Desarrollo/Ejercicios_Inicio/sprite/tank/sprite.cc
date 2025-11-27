#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>

esat::SpriteHandle img;

unsigned char fps=25; //Control de frames por segundo
double current_time,last_time;
int space[] = {74,140};

int esat::main(int argc, char **argv) {
  int j = 0, i = 0;

	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);
  img=esat::SpriteFromFile("../img/tanques.png");

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    last_time = esat::Time(); 
    
    esat::DrawBegin();
    esat::DrawClear(0,0,0);
    if(i%2 == 0){
      j+= 10;
    }
    else{
      j-= 10;
    }
    esat::DrawSprite(esat::SubSprite(img ,0, space[i%2], 62, 45), j, 255);
    esat::DrawEnd();
    if(j>738){
      i++;
    }
    if(j<0){
      i++;
    }

    //Control fps 
    do{
    	current_time = esat::Time();
    }while((current_time-last_time)<=1000.0/fps);
    esat::WindowFrame();
  }

  esat::SpriteRelease(img);
  esat::WindowDestroy();
  return 0;
}