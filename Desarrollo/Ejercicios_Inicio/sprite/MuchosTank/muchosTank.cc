#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

esat::SpriteHandle tanque;
esat::SpriteHandle BOOM;

unsigned char fps=25; //Control de frames por segundo
double current_time, last_time;
int space[4] = {74/*derecha*/,140/*izquierda*/,10/*abajo*/, 205/*arriba*/};//cordenadas tank
const int nt = 10;

struct Bullet{
  int DirBullet1 = 4, NumDisp1 = 0, BulletX1, BulletY1;//bullet
};

struct Explocion{
  int ExplosionX1=0, ExplosionY1=0, Impact1 = 0;//explocion
  int y = 0, j = 0;//animacion explocion
};

struct Tank{
  int Vel1 = 10, TankX1=0, TankY1=300, DirTank1 = 0;//tank
  Explocion explocion; 
  Bullet bullet; 
};


// tank 0 = jugador
Tank tank[nt+1] = {};

//en base a inpit (arrow) cambia direccion
//solo player---------------
void ControlDireccion(){
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Right)){
    tank[0].DirTank1 = 0;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Left)){
    tank[0].DirTank1 = 1;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Down)){
    tank[0].DirTank1 = 2;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Up)){
    tank[0].DirTank1 = 3;
  }
}

//en base a variable direccion modifica la pocicion de la imagen
//Todos
void movimiento(int j){
  switch(tank[j].DirTank1){
    case 0://derecha
      tank[j].TankX1+=tank[j].Vel1;
    break;
    case 1://izquierda
      tank[j].TankX1-=tank[j].Vel1;
    break;
    case 2://arriba
      tank[j].TankY1+=tank[j].Vel1;
    break;
    case 3://abajo
      tank[j].TankY1-=tank[j].Vel1;
    break;
  }
}

//Cambia velocidad
//solo player---------------
void ControlVelocidad(){
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Keypad_1)){//si preciona 1 entonces velocidad -1
    tank[0].Vel1-=1;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Keypad_2)){//si preciona 1 entonces velocidad +1
    tank[0].Vel1+=1;
  }
}

//No puede ir en retor entonces velocidad no puede ir en negativo
//solo player---------------
void NoReverse(){
  if(tank[0].Vel1 < 0){
    tank[0].Vel1 = 0;
  }
}

//Si borde del mapa cambia la direccio a la direccion opuesta
//Todos
void Direccion(int j){
  if(tank[j].TankX1<0){//si borde izquierda entonce nueva direccion derecha
    tank[j].DirTank1=0;
  }
  if(tank[j].TankX1>738){//si derecha nueva direccion izquierda
    tank[j].DirTank1=1;
  }
  if(tank[j].TankY1<0){//si arriba nueva direccion abajo
    tank[j].DirTank1=2;
  }
  if(tank[j].TankY1>546){//si abajo va pa ariiba
    tank[j].DirTank1=3;
  }
}

//guarda datos del 
//Todos
void Disparo(int j){
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Space)){
    tank[0].bullet.DirBullet1 = tank[0].DirTank1;
    tank[0].bullet.NumDisp1++;
    tank[0].bullet.BulletX1 = tank[0].TankX1;
    tank[0].bullet.BulletY1 = tank[0].TankY1;
  }
  else if(j > 0 && tank[j].bullet.NumDisp1<1){
    tank[j].bullet.DirBullet1 = tank[j].DirTank1;
    tank[j].bullet.NumDisp1++;
    tank[j].bullet.BulletX1 = tank[j].TankX1;
    tank[j].bullet.BulletY1 = tank[j].TankY1;
  }
}

//Todos
void MovDisp(int j){
  switch(tank[j].bullet.DirBullet1){
    case 0://derecha
    tank[j].bullet.BulletX1+=tank[j].Vel1*1.5;
    break;
    case 1://izquierda
    tank[j].bullet.BulletX1-=tank[j].Vel1*1.5;
    break;
    case 2://arriba
    tank[j].bullet.BulletY1+=tank[j].Vel1*1.5;
    break;
    case 3://abajo
    tank[j].bullet.BulletY1-=tank[j].Vel1*1.5;
    break;
  }
}

//Todos
void VerificaCordsExplosion(int a){
  if(tank[a].explocion.ExplosionX1<0){
    tank[a].explocion.ExplosionX1 = 0;
    tank[a].explocion.ExplosionY1 -= 30;//centra foto
  }
  if(tank[a].explocion.ExplosionX1>739){
    tank[a].explocion.ExplosionX1 = 739;
    tank[a].explocion.ExplosionY1 -= 30;//centra foto
  }
  if(tank[a].explocion.ExplosionY1<0){
    tank[a].explocion.ExplosionY1 = 0;
  }
  if(tank[a].explocion.ExplosionY1>519){
    tank[a].explocion.ExplosionY1 = 519;
  }
}

//Todos
void explosion(int y){
  tank[y].explocion.ExplosionX1=tank[y].bullet.BulletX1;
  tank[y].explocion.ExplosionY1=tank[y].bullet.BulletY1;
  tank[y].explocion.Impact1 = 1;
  VerificaCordsExplosion(y);
}

//Todos
void Impacto(int j){
  if(tank[j].bullet.BulletX1<0){//impacto izquierda
    tank[j].bullet.NumDisp1 = 0;
    explosion(j);
    tank[j].bullet.DirBullet1 = 4;
    tank[j].bullet.BulletX1 = 0;
  }
  else if(tank[j].bullet.BulletX1>738){//impacto derecha
    tank[j].bullet.NumDisp1 = 0;
    explosion(j);
    tank[j].bullet.DirBullet1 = 4;
    tank[j].bullet.BulletX1 = 0;
  }
  else if(tank[j].bullet.BulletY1<0){//impacto arriba
    tank[j].bullet.NumDisp1 = 0;
    explosion(j);
    tank[j].bullet.DirBullet1 = 4;
    tank[j].bullet.BulletY1 = 0;
  }
  else if(tank[j].bullet.BulletY1>546){//impacto abajo
    tank[j].bullet.NumDisp1 = 0;
    explosion(j);
    tank[j].bullet.DirBullet1 = 4;
    tank[j].bullet.BulletY1 = 0;
  }
}

// Todos
void AnimacionExlocion(int a){
  if(tank[a].explocion.Impact1 > 0){
    //simulo un for a dos ciclos
    esat::DrawSprite(esat::SubSprite(BOOM, 65*tank[a].explocion.j, 81*tank[a].explocion.y, 65, 81), tank[a].explocion.ExplosionX1, tank[a].explocion.ExplosionY1);
    tank[a].explocion.j++;//avanzo linea
    if(tank[a].explocion.j%5 == 0){//avanzo columna
      tank[a].explocion.y++;
      tank[a].explocion.j=0;//reseteo columna
    }
    if(tank[a].explocion.y == 5 && tank[a].explocion.j%5 == 0){//termina y resetea variables 
      tank[a].explocion.Impact1 = 0;
      tank[a].explocion.y = 0;
      tank[a].explocion.j = 0;
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
  tanque=esat::SpriteFromFile("../img/tanques.png");
  BOOM=esat::SpriteFromFile("../img/explosion.png");
	WindowSetMouseVisibility(true);
  srand(time(NULL));

  for(int i=1; i<nt+1; i++){
    tank[i].TankX1 = (rand() % 800);
    tank[i].TankY1 = (rand() % 600);
    tank[i].DirTank1 = (rand() % 4);
  }
  tank[0].TankX1 = 400;
  tank[0].TankY1 = 300; 
  
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    last_time = esat::Time(); 
    esat::DrawBegin();
    esat::DrawClear(0,0,0);

    ControlDireccion();//las flechitas
    //tank
    ControlVelocidad();//modifica velocidad si presionado 1 o 
    NoReverse();//no velocidad negativa
    for(int i=0; i<nt+1; i++){
      movimiento(i);//literalmente mueve
      Direccion(i);//cambia direccion si toca borde
      //disparo
      if(tank[i].bullet.NumDisp1 < 1){
        Disparo(i);//datos disparo y direccion
      }
      MovDisp(i);//mueve disparo
      Impacto(i);//choca borde
      AnimacionExlocion(i);
      if(tank[i].bullet.NumDisp1 > 0){
        esat::DrawSprite(disparo[tank[i].bullet.DirBullet1], tank[i].bullet.BulletX1+20, tank[i].bullet.BulletY1+10);
        //explosion

      }
      esat::DrawSprite(esat::SubSprite(tanque, 0, space[tank[i].DirTank1], 62, 51),tank[i].TankX1, tank[i].TankY1);//dibujp tan
    }
    
    esat::DrawEnd();
    ControlFPS(); 
    esat::WindowFrame();
  }

  esat::SpriteRelease(tanque);
  esat::SpriteRelease(BOOM);

  for(int i = 0; i<4; i++){
    esat::SpriteRelease(disparo[i]);
  }
  esat::WindowDestroy();
  return 0;
}