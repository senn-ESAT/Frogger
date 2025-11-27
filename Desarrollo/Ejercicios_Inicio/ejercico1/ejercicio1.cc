#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>

#include <stdio.h>

float puntos[4] = {10, 0, 790,0};
float puntos2[4] = {0,10,0,590};


int esat::main(int argc, char **argv) {
    esat::WindowInit(800,600);
	  WindowSetMouseVisibility(true);
  
    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
      esat::DrawBegin();
      esat::DrawClear(0,0,0);
      esat::DrawSetStrokeColor(239,120,8);
      //orizontal
      for(int i = 10; i<600; i+=10){
        puntos[1]=i;
        puntos[3]=i;
        esat::DrawSolidPath(puntos,2);
      }
      //vertical
      for(int i = 10; i<800; i+=10){
        puntos2[0]=i;
        puntos2[2]=i;
        esat::DrawSolidPath(puntos2,2);
      }
      esat::DrawEnd();  	
    	esat::WindowFrame();
      
    }
    esat::WindowDestroy();
    return 0;
}