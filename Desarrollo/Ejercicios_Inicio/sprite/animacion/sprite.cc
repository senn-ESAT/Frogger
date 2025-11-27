#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <stdio.h>

esat::SpriteHandle imagen;
esat::SpriteHandle imagen2;
int esat::main(int argc, char **argv) {
  int space[8] = {15,184,394,594,769,949,1136,1310};
  int i = 0;
	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);
  imagen=esat::SpriteFromFile("../img/hojasprites.png");
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    esat::DrawBegin();
    esat::DrawClear(0,0,0);
    imagen2=esat::SubSprite(imagen ,space[i], 5, 165, 310);
    esat::DrawSprite(imagen2, 10, 10);
    esat::DrawEnd();
    esat::WindowFrame();
    i++;
    if(i>7)
    i=0;
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