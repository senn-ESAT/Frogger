#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <stdio.h>

esat::SpriteHandle imagen;

int esat::main(int argc, char **argv) {

	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);
  imagen=esat::SpriteFromFile("../img/imagen.png");
  
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    esat::DrawBegin();
    esat::DrawClear(0,0,0);
    esat::DrawSprite(imagen, 10, 10);
    esat::DrawEnd();
    esat::WindowFrame();
  }

  esat::SpriteRelease(imagen);
  esat::WindowDestroy();
  return 0;
}