#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>

#include <stdio.h>

float puntos[4] = {0, 300, 800,300};
float puntos2[4] = {400,0,400,600};


int esat::main(int argc, char **argv) {
    esat::WindowInit(800,600);
	  WindowSetMouseVisibility(true);
  
    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
      esat::DrawBegin();
      esat::DrawClear(0,0,0);
      esat::DrawSetStrokeColor(239,120,8);
      //orizontal

        esat::DrawSolidPath(puntos,2);
      
      //vertical

        esat::DrawSolidPath(puntos2,2);
      
      esat::DrawEnd();  	
    	esat::WindowFrame();
      
    }
    esat::WindowDestroy();
    return 0;
}