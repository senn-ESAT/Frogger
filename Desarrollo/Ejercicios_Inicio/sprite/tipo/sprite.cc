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
  Posicion puntos;
  Color color;
};

unsigned char fps=25; //Control de frames por segundo
double current_time,last_time;

int esat::main(int argc, char **argv) {
  Rectangulo rec = {{100, 200},{100, 170, 140}};

  float cords[10]={
    rec.puntos.x, rec.puntos.y, 
    rec.puntos.x*2, rec.puntos.y,
    rec.puntos.x*2, rec.puntos.y*2,
    rec.puntos.x, rec.puntos.y*2,
    rec.puntos.x, rec.puntos.y,
  };
  
	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    last_time = esat::Time(); 
    esat::DrawBegin();
    esat::DrawClear(0,0,0);

    esat::DrawSetFillColor(rec.color.r, rec.color.g, rec.color.b);

    esat::DrawSolidPath(cords, 5);

    esat::DrawEnd();
    esat::WindowFrame();
  }

  esat::WindowDestroy();
  return 0;
}