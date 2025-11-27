#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <stdio.h>

float ope[10] = {1,1,0,1,0,0,1,0,1,1};

int esat::main(int argc, char **argv) {
  esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);
  
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    float puntos[10] = {50,30,750,30,750,570,50,570,50,30};
    esat::DrawBegin();
    esat::DrawClear(0,0,0);
    esat::DrawSetStrokeColor(239,120,8);

    //i bucle quantas veces
    for(int i = 0; i<27; i++){
      esat::DrawPath(puntos,5);
      //j bucle pociciones
      for(int j = 0; j<10;j++){
        //ope 1 == + ope 0 == -
        if(ope[j] == 1){
          puntos[j] += 10;
        }
        else{
          puntos[j] -= 10;
        }
      }
    }
    esat::DrawEnd();
    esat::WindowFrame();
  }
  esat::WindowDestroy();
  return 0;
}