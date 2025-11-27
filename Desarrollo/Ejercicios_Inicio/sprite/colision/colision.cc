#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <stdio.h>

struct Player{
  float px, py;
  bool colision;
};

struct Obstaculos{
  float p1x, p1y, p2x, p2y;
  float puntos[10] = {0};
};

Player j = {50,500, false};

void DibujoPlayer(){
  float puntos[10] = {j.px, j.py, j.px+50, j.py, j.px+50, j.py+50, j.px, j.py+50, j.px,j.py};
  esat::DrawPath(puntos, 5);
  if(j.colision){
    esat::DrawSetFillColor(255,255,255);
    esat::DrawSolidPath(puntos, 5);
  }
}

void MovimientoPlayer(){
  if(!j.colision){
    if(esat::IsSpecialKeyPressed(esat::kSpecialKey_Up)){
      j.py-=1;
    }
    if(esat::IsSpecialKeyPressed(esat::kSpecialKey_Down)){
      j.py+=1;
    }
      if(esat::IsSpecialKeyPressed(esat::kSpecialKey_Left)){
      j.px-=1;
    }
    if(esat::IsSpecialKeyPressed(esat::kSpecialKey_Right)){
      j.px+=1;
    }
  }
}

void ControlColision(Obstaculos obj){
  //cordenadas jugador solo punto inicio
  for(int i = 0; i<50; i++){
    if(j.px+i > obj.p1x && j.px+i < obj.p2x){
      if(j.py+i > obj.p1y && j.py+i < obj.p2y){
        j.colision = true;
      }
    }
  }
}

void DibujoObs(Obstaculos O){
  float puntos[10] = {O.p1x, O.p1y, O.p2x, O.p1y, O.p2x, O.p2y, O.p1x, O.p2y, O.p1x, O.p1y};
  esat::DrawSetFillColor(200,20,20);
  esat::DrawSolidPath(puntos, 5);
  ControlColision(O);
}


int esat::main(int argc, char **argv) {
  Obstaculos 
  A = {50,50,400,150}, 
  B = {50,200,220,70}, 
  C = {500,400,600,500}, 
  D = {200,400,250,550};

	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);
  
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    esat::DrawBegin();
    esat::DrawClear(0,0,0);
    
    DrawSetStrokeColor(100,100,150);

    DibujoObs(A);
    DibujoObs(B);
    DibujoObs(C);
    DibujoObs(D);

    DibujoPlayer();
    MovimientoPlayer();

    esat::DrawEnd();
    esat::WindowFrame();
  }

  esat::WindowDestroy();
  return 0;
}