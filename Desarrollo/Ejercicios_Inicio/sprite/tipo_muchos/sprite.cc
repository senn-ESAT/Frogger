#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>

struct Color{
  int r, g, b;
};

struct Posicion{
  float x, y;
};

struct Rectangulo{
  Posicion p1;
  Color color;
};

unsigned char fps=25; //Control de frames por segundo
double current_time,last_time;

int esat::main(int argc, char **argv) {
  Rectangulo rec[2] = {
    {{100, 200},{100, 170, 140}},
    {{120, 180},{100, 170, 140}}
  };
  
	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);
  
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    last_time = esat::Time(); 
    esat::DrawBegin();
    esat::DrawClear(0,0,0);

    for(int i=0; i<2; i++){
      float cords[10] = {rec[i].p1.x, rec[i].p1.y, rec[i].p1.x*2, rec[i].p1.y, rec[i].p1.x*2, rec[i].p1.y*2, rec[i].p1.x, rec[i].p1.y*2, rec[i].p1.x, rec[i].p1.y};

      esat::DrawSetFillColor(rec[i].color.r, rec[i].color.g, rec[i].color.b);
      esat::DrawSolidPath(cords, 5);
    }

    esat::DrawEnd();
    esat::WindowFrame();
  }

  esat::WindowDestroy();
  return 0;
}