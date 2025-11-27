#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>

esat::SpriteHandle tank;
esat::SpriteHandle BOOM;

unsigned char fps=25; //Control de frames por segundo
double current_time, last_time;
int space[4] = {74/*derecha*/,140/*izquierda*/,10/*abajo*/, 205/*arriba*/};//cordenadas tank
const int velocidad = 10;

int Vel1 = 10, TankX1=0, TankY1=300, DirTank1 = 0;//tank
int DirBullet1 = 4, NumDisp1 = 0, BulletX1, BulletY1;//bullet
int ExplosionX1=0, ExplosionY1=0, Impact1 = 0;//explocion
int y = 0, j = 0;//animacion explocion

//en base a inpit (arrow) cambia direccion
//solo player---------------
void ControlDireccion(){
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Right)){
    DirTank1 = 0;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Left)){
    DirTank1 = 1;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Down)){
    DirTank1 = 2;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Up)){
    DirTank1 = 3;
  }
}

//en base a variable direccion modifica la pocicion de la imagen
//Todos
void movimiento(){
  switch(DirTank1){
    case 0://derecha
      TankX1+=Vel1;
    break;
    case 1://izquierda
      TankX1-=Vel1;
    break;
    case 2://arriba
      TankY1+=Vel1;
    break;
    case 3://abajo
      TankY1-=Vel1;
    break;
  }
}

//Cambia velocidad
//solo player---------------
void ControlVelocidad(){
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Keypad_1)){//si preciona 1 entonces velocidad -1
    Vel1-=1;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Keypad_2)){//si preciona 1 entonces velocidad +1
    Vel1+=1;
  }
}

//No puede ir en retor entonces velocidad no puede ir en negativo
//solo player---------------
void NoReverse(){
  if(Vel1 < 0){
    Vel1 = 0;
  }
}

//Si borde del mapa cambia la direccio a la direccion opuesta
//Todos
void Direccion(){
  if(TankX1<0){//si borde izquierda entonce nueva direccion derecha
    DirTank1=0;
  }
  if(TankX1>738){//si derecha nueva direccion izquierda
    DirTank1=1;
  }
  if(TankY1<0){//si arriba nueva direccion abajo
    DirTank1=2;
  }
  if(TankY1>546){//si abajo va pa ariiba
    DirTank1=3;
  }
}

//guarda datos del 
//Todos
void Disparo(){
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Space))
  DirBullet1 = DirTank1;
NumDisp1++;
BulletX1 = TankX1;
BulletY1 = TankY1;
}

//Todos
void MovDisp(){
  switch(DirBullet1){
    case 0://derecha
    BulletX1+=Vel1*1.5;
    break;
    case 1://izquierda
    BulletX1-=Vel1*1.5;
    break;
    case 2://arriba
    BulletY1+=Vel1*1.5;
    break;
    case 3://abajo
    BulletY1-=Vel1*1.5;
    break;
  }
}

//Todos
void VerificaCordsExplosion(){
  if(ExplosionX1<0){
    ExplosionX1 = 0;
    ExplosionY1 -= 30;//centra foto
  }
  if(ExplosionX1>739){
    ExplosionX1 = 739;
    ExplosionY1 -= 30;//centra foto
  }
  if(ExplosionY1<0){
    ExplosionY1 = 0;
  }
  if(ExplosionY1>519){
    ExplosionY1 = 519;
  }
}

//Todos
void explosion(){
  ExplosionX1=BulletX1;
  ExplosionY1=BulletY1;
  Impact1 = 1;
  VerificaCordsExplosion();
}

//Todos
void Impacto(){
  if(BulletX1<0){//impacto izquierda
    NumDisp1 = 0;
    explosion();
    DirBullet1 = 4;
    BulletX1 = 0;
  }
  else if(BulletX1>738){//impacto derecha
    NumDisp1 = 0;
    explosion();
    DirBullet1 = 4;
    BulletX1 = 0;
  }
  else if(BulletY1<0){//impacto arriba
    NumDisp1 = 0;
    explosion();
    DirBullet1 = 4;
    BulletY1 = 0;
  }
  else if(BulletY1>546){//impacto abajo
    NumDisp1 = 0;
    explosion();
    DirBullet1 = 4;
    BulletY1 = 0;
  }
}

// Todos
void AnimacionExlocion(){
  if(Impact1 > 0){
    //simulo un for a dos ciclos
    esat::DrawSprite(esat::SubSprite(BOOM, 65*j, 81*y, 65, 81), ExplosionX1, ExplosionY1);
    j++;//avanzo linea
    if(j%5 == 0){//avanzo columna
      y++;
      j=0;//reseteo columna
    }
    if(y == 5 && j%5 == 0){//termina y resetea variables 
      Impact1 = 0;
      y = 0;
      j = 0;
    }
  }
}

//Sistema
void ControlFPS(){
  do{
    current_time = esat::Time();
  }while((current_time-last_time)<=1000.0/fps);
}

int esat::main(int argc, char **argv) {
  esat::WindowInit(800,600);//dato importante todas la simajene c inicializan despes de windowsInit
  esat::SpriteHandle disparo[4] = {esat::SpriteFromFile("../img/disparo_03.png"), esat::SpriteFromFile("../img/disparo_09.png"), esat::SpriteFromFile("../img/disparo_06.png"), esat::SpriteFromFile("../img/disparo_12.png")};
  tank=esat::SpriteFromFile("../img/tanques.png");
  BOOM=esat::SpriteFromFile("../img/explosion.png");
	WindowSetMouseVisibility(true);
  
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    last_time = esat::Time(); 
    esat::DrawBegin();
    esat::DrawClear(0,0,0);
    
    //tank
    ControlDireccion();//las flechitas
    ControlVelocidad();//modifica velocidad si presionado 1 o 2
    NoReverse();//no velocidad negativa
    movimiento();//literalmente mueve
    Direccion();//cambia direccion si toca borde

    //disparo
    if(NumDisp1 < 1 && esat::IsSpecialKeyDown(esat::kSpecialKey_Space)){
      Disparo();//datos disparo y direccion
    }
    MovDisp();//mueve disparo
    Impacto();//choca borde
    if(NumDisp1 > 0){
      esat::DrawSprite(disparo[DirBullet1], BulletX1+20, BulletY1+10);
    }

    //explosion
    AnimacionExlocion();

    esat::DrawSprite(esat::SubSprite(tank , 0, space[DirTank1], 62, 51), TankX1, TankY1);//dibujp tan
    
    esat::DrawEnd();
    ControlFPS(); 
    esat::WindowFrame();
  }

  esat::SpriteRelease(tank);
  esat::SpriteRelease(BOOM);

  for(int i = 0; i<4; i++){
    esat::SpriteRelease(disparo[i]);
  }
  esat::WindowDestroy();
  return 0;
}