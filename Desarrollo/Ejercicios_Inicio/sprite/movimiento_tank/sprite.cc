#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>

esat::SpriteHandle img;

unsigned char fps=25; //Control de frames por segundo
double current_time,last_time;
int space[] = {74/*derecha*/,140/*izquierda*/,10/*abajo*/, 205/*arriba*/};
int velocidad = 10, px=0, py=300, direccion = 0;
//en base a inpit (arrow) cambia direccion
void ControlDireccion(){
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Right)){
    direccion = 0;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Left)){
    direccion = 1;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Down)){
    direccion = 2;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Up)){
    direccion = 3;
  }
}
//en base a variable direccion modifica la pocicion de la imagen
void movimiento(){
  switch(direccion){
    case 0://derecha
      px+=velocidad;
    break;
    case 1://izquierda
      px-=velocidad;
    break;
    case 2://arriba
      py+=velocidad;
    break;
    case 3://abajo
      py-=velocidad;
    break;
  }
}

//cambia velocidad
void ControlVelocidad(){
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Keypad_1)){//si preciona 1 entonces velocidad -1
    velocidad-=1;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Keypad_2)){//si preciona 1 entonces velocidad +1
    velocidad+=1;
  }
}

//no puede ir en retor entonces velocidad no puede ir en negativo
void NoReverse(){
  if(velocidad < 0){
    velocidad = 0;
  }
}

void ControlFPS(){
  do{
    current_time = esat::Time();
  }while((current_time-last_time)<=1000.0/fps);
}

//si borde del mapa cambia la direccio a la direccion opuesta
void Direccion(){
  if(px<0){//si borde izquierda entonce nueva direccion derecha
    direccion=0;
  }
  if(px>738){//si derecha nueva direccion izquierda
    direccion=1;
  }
  if(py<0){//si arriba nueva direccion abajo
    direccion=2;
  }
  if(py>546){//si abajo va pa ariiba
    direccion=3;
  }
}

int esat::main(int argc, char **argv) {

	esat::WindowInit(800,600);
	WindowSetMouseVisibility(true);
  img=esat::SpriteFromFile("../img/tanques.png");

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    last_time = esat::Time(); 
    esat::DrawBegin();
    esat::DrawClear(0,0,0);

    ControlDireccion();
    ControlVelocidad();
    NoReverse();

    movimiento();
    Direccion();
    esat::DrawSprite(esat::SubSprite(img ,0, space[direccion], 62, 45), px, py);
    esat::DrawEnd();

    ControlFPS(); 
    esat::WindowFrame();
  }

  esat::SpriteRelease(img);
  esat::WindowDestroy();
  return 0;
}