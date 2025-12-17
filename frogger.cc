#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum Direccion {
  UP,
  DOWN,
  RIGHT,
  LEFT
};

//-------------Generales-------------
struct PuntoCoord{
  float x = 0, y = 0;
};

struct Colision{
  PuntoCoord P1;                // P1 Tambien sirve como ubicación
  PuntoCoord P2;
};

struct Sprite{
  esat::SpriteHandle img;       //Imagen
  int IndiceY = 0, IndiceX = 0; //AnimacionTortugas spritesheet
};

//-------------Player-------------
struct Frog{
  Colision colision;            // Colision rana
  Sprite sprite;                // Sprite actual rana
  Direccion dir;                // Direccion rana
  PuntoCoord origen;            // Desde donde salta --> sirve para animacion del salto
  int vidas = 3, score;         // Las vidas y la puntuacion de la run
  float arriba;                 // Para calcular puntos en base a distancia
  bool jumping = false,         // Ppara animacion salto
       muriendo = false;        // es para que inicie animacion muerte y bloque los movimientos
};

//-------------Generales-------------
struct SafeZone{
  Colision colision;
  Sprite sprite;                //sprite de los eventos
  int evento = 0;               //en plan 0 = agua, 1 = mosca, 2 = cocodrilo, 4 = rana/player
  float inicioEvento;
};

//-------------Objetos-------------
struct Autos{
  Colision colision;
  Sprite sprite;
  Direccion dir;
  int velocidad, espacio;
};

struct Tronco{
  Colision colision;
  Direccion dir = RIGHT;        // Todos los troncos van a la derecha 
  int velocidad, no, espacio;   // No es numero objetos, ose la cantidad de sprites
};

struct Tortuga{
  Colision colision;
  Direccion dir = LEFT;         // Todas las tortugas van siempre a la izquierda
  int   velocidad,              // Velocidad de movimiento
        no,                     // Numero tortugas
        espacio,                // Espacion entre grupos de tortugas
        frame = 0,              // Frame de la animacion 0-2 nadando,
        frameSub = 2;           // 3-4 undiendose 5 abajo del agua
  double espera,                //todos estos doubles son para la animacion del undimiento
        hundimientoInicio,      //espera es cada cuanto se unden, en que segundo einicio el hundimiento
        AnimacionSub;           // sirve para calcular el frameSub
  bool  hunde = false,          // No todas se unden, esto sirve para decir si una tortuga se hunde 
        flota = true;           // Para ver si se puede saltar arriba de las tortugas o no
};

struct Cocodlio{
  Colision body;                //tiene dos colisiones protque n el cuerpo es seguro y en la cabza moris
  Colision head;
  Sprite sprite;
  int velocidad;
};

//-------------NPC-------------
struct Serpiente{
  Colision colision;
  Direccion direccion;
  int velocidad = 1;
};

struct Perro{
  Colision colision;
  Sprite sprite;
  Direccion direccion;
  int velocidad;
};

struct RanaNPC{
  Colision colision;
  Sprite sprite;
  int dir;
};

Frog Player1, Player2;                        //Jugadores
SafeZone fin[5];                              //Casas
Autos F1[5], F2[5], F3[5], F4[5], F5[5];      //Un array por cada fila de autos
Tronco M1[5], M2[5], M3[5];                   //Un array por cada fila de troncos(M de madera porque T va a las tortugas)
Tortuga T1[5], T2[5];                         //Un array por cada fila de columnas
//Porque 5? Porque me parece que nunca aparecen mas de 5 objetos a la vez por fila

//Variables de sistema y costantes
const int FontSize = 20, SpritesHeight = 48;  // La altuara de los sprites, sirve para calcular la cantidad de filas
const int ScreenX = 672, ScreenY = 768;       // Screen size
unsigned char fps=25;                         // Control de frames por AnimacionTortugas
double  current_time, last_time,              // Esto sirve para 
        TimeAnimacion, TimeMuerte,            // Gestion de tiempo para animacionse
        LevelTime;                            // Timer del nivel (la barra verde)

//Variables globales
float Points[10] = {0,0,ScreenX,0,ScreenX,ScreenY/2,0,ScreenY/2,0,0}; //zona azul
bool InGame = false;    // Si false es menu, si true es el juego
int TipoMenu = 0,       // 3 pantallas 
    numPlayers = 1,     // Numero de jugadores
    highScore = 0,      // El high score
    Nivel = 0,          // Nivel actual
    Creditos = 0,       // Los Creditos/coins
        TotalSeconds = 0,   // La cantidad de segundos totales
    SecondWidth,        // La cantidad de px que ocupa un segundo en proporcion al maximo de tiempo por nivel
    SecondPassed,       // Cantidad de segundos que pasaron desde el inicio del nivel
    ScoreList[5] = {0}; // Los top 5 high scores, inicializados a 0 para que c llenen
// Las tortugas las tengo que hacer retroceder en modo diferente
int UltimaT1 = 4, UltimaT2 = 4,
    PrimeraT1 = 0, PrimeraT2 = 0; 
//Sprite Handles
esat::SpriteHandle 
//Arrays
  Madera[3],
  Tortugas[5],
  Players[8],
  Muerte[7],
  Campero[2],
//UI
  SpriteLetras, SpriteNivel, SpriteVidas, SpriteTiempo,
//Extras
  SpriteMeta, SpritePastoVerde, SpritePasto, SpriteMosca, SpriteCocodriloCampero,
//Player 
  SpriteFrog, SpriteMuerte, RanaSafe,
//Flotantes
  SpriteTronco,
  SpriteTortuga,
  SpriteCocodlio,
//Veiculos
  SpriteCamion,
  SpriteAuto1,
  SpriteAuto2,
  SpriteAuto3,
  SpriteAuto4,
//Enemigos
  SpritePerro,
  SpriteSerpiente;

void LoadSprites(){
  //UI
  SpriteLetras = esat::SpriteFromFile("./Assets/img/SheetLetras.png");
  SpriteNivel = esat::SpriteFromFile("./Assets/img/Nivel.png");
  SpriteVidas = esat::SpriteFromFile("./Assets/img/Vidas.png");
  SpriteTiempo = esat::SpriteFromFile("./Assets/img/SheetTime.png");
  //Extras
  SpriteMeta = esat::SpriteFromFile("./Assets/img/lagunito.png");
  SpritePastoVerde = esat::SpriteFromFile("./Assets/img/piso.png");
  SpritePasto = esat::SpriteFromFile("./Assets/img/Pasto.png");
  SpriteMosca = esat::SpriteFromFile("./Assets/img/Mosca.png");
  SpriteCocodriloCampero = esat::SpriteFromFile("./Assets/img/SheetCampero.png");
  //Player
  SpriteFrog = esat::SpriteFromFile("./Assets/img/SheetRana.png");
  SpriteMuerte = esat::SpriteFromFile("./Assets/img/SheetMuerte.png");
  RanaSafe = esat::SpriteFromFile("./Assets/img/RanaSafe.png");
  //Flotantes
  SpriteTronco = esat::SpriteFromFile("./Assets/img/SheetTronco.png");
  SpriteTortuga = esat::SpriteFromFile("./Assets/img/SheetTortuga.png");
  SpriteCocodlio = esat::SpriteFromFile("./Assets/img/SheetCocodrilo.png");
  //Veiculos
  SpriteCamion = esat::SpriteFromFile("./Assets/img/Camion.png");
  SpriteAuto1 = esat::SpriteFromFile("./Assets/img/Auro1.png");
  SpriteAuto2 = esat::SpriteFromFile("./Assets/img/Auto2.png");
  SpriteAuto3 = esat::SpriteFromFile("./Assets/img/Auto3.png");
  SpriteAuto4 = esat::SpriteFromFile("./Assets/img/Auto4.png");
  //Enemigos
  SpritePerro = esat::SpriteFromFile("./Assets/img/SheetPerro.png");
  SpriteSerpiente = esat::SpriteFromFile("./Assets/img/SheetSerpiente.png");

  for(int i = 0; i < 8; i++){ //sprites madera son 3 y sprtes tortuga son 5
    if(i<2){
      Campero[i] = esat::SubSprite(SpriteCocodriloCampero,48*i,0,48,48);   //48*i es lo ancho de cada sprite
    }
    if(i<3){
      Madera[i] = esat::SubSprite(SpriteTronco,48*i,0,48,48);
    }
    if(i < 5){
      Tortugas[i] = esat::SubSprite(SpriteTortuga,48*i,0,48,48);
    }
    if(i<7){ 
      Muerte[i] = esat::SubSprite(SpriteMuerte, 48*i, 0, 48, 48);
    }
    Players[i] = esat::SubSprite(SpriteFrog, 48*i, 0, 48, 48);
  } 
}

/****************************************************************************************************
*****************************************INICIALIZACION**********************************************
****************************************************************************************************/

void InicializarJugadores(){
  //todos los datos de
  Player1.colision.P1.x = (ScreenX/2)-24;
  Player1.colision.P1.y = (ScreenY-(SpritesHeight*2))+1;
  Player1.sprite.img = Players[1];

  //Mismo spawn para ambs jugadores
  if(numPlayers == 2){
    Player2 = Player1;
    Player1.colision.P1.x /= 2;
    Player2.colision.P1.x = Player1.colision.P1.x*3;
    Player2.colision.P2.x = Player2.colision.P1.x+48;
    Player2.colision.P2.y = Player2.colision.P1.y + SpritesHeight -3;
    Player2.origen = Player2.colision.P1;
    Player2.arriba = Player2.colision.P1.y;
  }
  Player1.colision.P2.x = Player1.colision.P1.x+48;
  Player1.colision.P2.y = Player1.colision.P1.y + SpritesHeight -3;
  Player1.origen = Player1.colision.P1;
  Player1.arriba = Player1.colision.P1.y;
}

void InicializarAutos(){
  for(int i = 0; i < 5; i++) {
    //el clculo de SprtieHeight*n es solo para calcular la fila ya que los sprites son altos 48
    //La fila 1 osea es SpriteHeight*1 es el footer
    
    F1[i].sprite.img = SpriteAuto1;                             //carga sprite de auto ya que auto tiene un sprite
    F1[i].dir = LEFT;                                           //direccion a la que se mueve
    F1[i].colision.P1.y = ScreenY-(SpritesHeight*3);            //aparece en la terzera fila desde abajo
    F1[i].colision.P2.y = F1[i].colision.P1.y + SpritesHeight;  //largo
    
    F2[i].sprite.img = SpriteAuto2;
    F2[i].dir = RIGHT;
    F2[i].colision.P1.y = ScreenY-(SpritesHeight*4);
    F2[i].colision.P2.y = F2[i].colision.P1.y + SpritesHeight;
    
    F3[i].sprite.img = SpriteAuto3;
    F3[i].dir = LEFT;
    F3[i].colision.P1.y = ScreenY-(SpritesHeight*5);
    F3[i].colision.P2.y = F3[i].colision.P1.y + SpritesHeight;
    
    F4[i].sprite.img = SpriteAuto4;
    F4[i].dir = RIGHT;
    F4[i].colision.P1.y = ScreenY-(SpritesHeight*6);
    F4[i].colision.P2.y = F4[i].colision.P1.y + SpritesHeight;
    
    F5[i].sprite.img = SpriteCamion;
    F5[i].dir = LEFT;
    F5[i].colision.P1.y = ScreenY-(SpritesHeight*7);
    F5[i].colision.P2.y = F5[i].colision.P1.y + SpritesHeight;
  }
}

void InicializarFlotantes(){
  for(int i = 0; i<5; i++){
    M1[i].no = 3;                                             //no es el numero de sprites
    M1[i].colision.P1.y = ScreenY-(SpritesHeight*10);         //posision en el eje y 
    M1[i].colision.P2.y = M1[i].colision.P1.y+SpritesHeight;  //largo
    
    M2[i].no = 6;
    M2[i].colision.P1.y = ScreenY-(SpritesHeight*11);
    M2[i].colision.P2.y = M2[i].colision.P1.y+SpritesHeight;
    
    M3[i].no = 4;
    M3[i].colision.P1.y = ScreenY-(SpritesHeight*13);
    M3[i].colision.P2.y = M3[i].colision.P1.y+SpritesHeight;
    
    T1[i].no = 3;
    T1[i].colision.P1.y = ScreenY-(SpritesHeight*9);
    T1[i].colision.P2.y = T1[i].colision.P1.y+SpritesHeight;
    
    T2[i].no = 2;
    T2[i].colision.P1.y = ScreenY-(SpritesHeight*12);
    T2[i].colision.P2.y = T2[i].colision.P1.y+SpritesHeight;
  }
}

Colision ResetPosicionRandom(Colision PosicionY){
  Colision coli;
  coli.P1.x = rand()%(ScreenX/2);
  coli.P1.y = PosicionY.P1.y;
  coli.P2.x = coli.P1.x + 48;
  coli.P2.y = PosicionY.P2.y;
  return coli;
}

int ResetEspacio(){
  int newSpacing;
  newSpacing = 192 + (rand() % (48*3)); //el +192 es el ancho minimo + random de 0 a 3 veces el ancho de la imagen
  return newSpacing;
}

int ResetVelocidad(){
  int newSpeed;
  newSpeed = (Nivel/3) + 3 + rand()%5; //minimo 3 maximo 8 (3+4=8) + 1/3 por cada nivel
  return newSpeed;  
}

void ResetAutos(Autos *A){
  A[0].espacio = ResetEspacio();
  A[0].colision = ResetPosicionRandom(A[0].colision);

  if(esat::SpriteWidth(A[0].sprite.img) < 50){
    A[0].velocidad = ResetVelocidad();
  }
  else{//Si es el camion
    A[0].velocidad = 3;
    A[0].colision.P2.x += 48;
    A[0].espacio += 48;
  }

  for(int i = 1; i<5; i++){ //i = 1 porque los datos estan en 0
    A[i].velocidad = A[0].velocidad;
    A[i].espacio = A[0].espacio;
    A[i].colision.P1.x = A[i-1].colision.P1.x + (A[i].espacio);
    A[i].colision.P2.x = A[i].colision.P1.x + esat::SpriteWidth(A[i].sprite.img);
  }
}

void ResetTroncos(Tronco *M){
  M[0].espacio = (48*M[0].no)+48+(rand()%(48*3));     //un tronco puede ser ancho 6 sprites entonces de 4 a 1 sprite mas ancho del tronco
  M[0].colision = ResetPosicionRandom(M[0].colision);

  M[0].velocidad = ResetVelocidad();
  M[0].colision.P2.x = M[0].colision.P1.x + 48*M[0].no;    //lo ancho de el tronco depende de el numero de sprites

  for(int i = 1; i<5; i++){ //i = 1 porque los datos estan en 0
    M[i].velocidad = M[0].velocidad;
    M[i].espacio = M[0].espacio;
    M[i].colision.P1.x = M[i-1].colision.P1.x + (M[i].espacio);
    M[i].colision.P2.x = M[i].colision.P1.x + 48*M[i].no;
  }
}

void ResetTortugas(Tortuga *T){
  int nada;

  T[0].espacio = ResetEspacio() + 48; //el minimo de posible de ResetEspacio es 3 sprites
  T[0].colision = ResetPosicionRandom(T[0].colision);

  T[0].velocidad = ResetVelocidad();
  T[0].colision.P2.x = T[0].colision.P1.x + 48*T[0].no;
  
  for(int i = 1; i<5; i++){ //i = 1 porque los datos estan en 0
    T[i].velocidad = T[0].velocidad;
    T[i].espacio = T[0].espacio;
    T[i].colision.P1.x = T[i-1].colision.P1.x + (T[i].espacio);
    T[i].colision.P2.x = T[i].colision.P1.x + 48*T[i].no;
    T[i].frame = 0;
    //ahora a ver si se hunde
    nada = rand()%100;    //numero random entre 0 y 100
    if(nada > 90-Nivel){        //posibilidad de 10% de que se hunde +1% de posibilidad por cada nivel
      T[i].hunde = true;

      double espera = rand()%4000; //espera entre 0 y 4000 milisegundos (4 segundo)
      T[i].espera = espera;
      T[i].hundimientoInicio = esat::Time();
    }
  }
}

void ResetPlayer(Frog *P){
  P->vidas = 3 + (P->score/20000);
  P->score = 0;
  P->arriba = 0;
}

void ResetTime(){
  TotalSeconds = 25 + rand()%15;  //de 25 a 40 segundos x nivel
  LevelTime = esat::Time();
  SecondWidth = 400/TotalSeconds;
  SecondPassed = 0; 
}

void CambiarValoresNivel(){
    //Cambio valores de la velocidad y el espacio de los objetos que se muvene,  y las coliciones tambien
    //Los objetos que c mueven son los autos, Los troncos y las tortugas
    ResetAutos(F1);
    ResetAutos(F2);
    ResetAutos(F3);
    ResetAutos(F4);
    ResetAutos(F5);

    ResetTroncos(M1);
    ResetTroncos(M2);
    ResetTroncos(M3);

    ResetTortugas(T1);
    ResetTortugas(T2);

    ResetTime();
    
    int Anchura = esat::SpriteWidth(SpriteMeta) + (esat::SpriteWidth(SpritePastoVerde)*2);

    for(int i = 0; i < 5; i++){
      fin[i].evento = 0;
      if(fin[4].colision.P1.x == 0){
        fin[i].colision.P1.x = (Anchura*i) + 24;
        fin[i].colision.P1.y = 96;
        fin[i].colision.P2.x = fin[i].colision.P1.x + 48;
        fin[i].colision.P2.y = 144;
      }
    }
}

void RespawnPlayer(){
  Player1.colision.P1.x = (ScreenX/2)-24;
  Player1.colision.P1.y = (ScreenY-(SpritesHeight*2))+1;
  Player1.sprite.img = Players[1];
  Player1.colision.P2.x = Player1.colision.P1.x+48;
  Player1.colision.P2.y = Player1.colision.P1.y + SpritesHeight - 3;
  Player1.origen = Player1.colision.P1;
  Player1.dir=UP;
  Player1.arriba = Player1.colision.P1.y;
}


/*******************************************************
********************Colisiones**************************
*******************************************************/

bool DetectarColision(Colision a, Colision b) {
  return (a.P2.x >= b.P1.x) &&
         (a.P1.x <= b.P2.x) &&
         (a.P2.y >= b.P1.y) &&
         (a.P1.y <= b.P2.y);
}

void Death(bool *mu, int *hp, float *A, float y){
  *mu = true;
  *hp -= 1;
  *A = y;
  TimeMuerte = esat::Time();
}

void ColisionPlayerMeta(Frog *P){
  for(int i = 0; i < 5; i++){
    if(DetectarColision(P->colision, fin[i].colision)){ //si colision player-meta entra
      //en plan 0 = agua, 1 = mosca, 2 = cocodrilo, 3 = rana/player
      switch(fin[i].evento){  //si esta vacio
        case 0:
          P->score += 200;
          RespawnPlayer();
          fin[i].evento = 3;
          ResetTime();
        break;
        case 1: //si mosca
          P->score += 400;
          RespawnPlayer();
          fin[i].evento = 3;
          ResetTime();
        break;
        case 2: // si cocodrilomuerto
          Death(&P->muriendo, &P->vidas, &P->arriba, P->colision.P1.y);
          ResetTime();
        break;
        case 3: //si esta ocupado rana retrocede
          P->colision.P1.y += 48;
          P->colision.P2.y += 48;
          P->origen = P->colision.P1;
        break;
      }
    }
  }
}


void ColisionPlayerAuto(Frog *P){
  for(int i = 0; i < 5; i++){   
    if(P->muriendo == false){
      if( DetectarColision(P->colision, F1[i].colision) || 
          DetectarColision(P->colision, F2[i].colision) ||
          DetectarColision(P->colision, F3[i].colision) ||
          DetectarColision(P->colision, F4[i].colision) ||
          DetectarColision(P->colision, F5[i].colision) ){
        Death(&P->muriendo, &P->vidas, &P->arriba, P->colision.P1.y);
        break;
      }
    }
  }
}

void MoverPlayer(Colision *coli, PuntoCoord *orig, int vel, Direccion dir){
  if(dir == RIGHT){
    coli->P1.x += vel;
    coli->P2.x += vel;
    orig->x += vel;
  }
  else{
    coli->P1.x -= vel;
    coli->P2.x -= vel;
    orig->x -= vel;
  }
}

void ColisionesPlayerFlotantes(Frog *P){
  int velocidad;
  if(P->muriendo == false){   //si ya esta muriendo entonces no entra
    if(P->colision.P1.y < (ScreenY/2) && P->colision.P1.y > (ScreenY-(SpritesHeight*14))+1){   //si esta en el rio
      bool flotando = false;
    
      for(int i = 0; i < 5; i++){//si hay colision almenos 1 de los que flotan y 
        if(DetectarColision(P->colision, M1[i].colision)){
          flotando = true;    //si fltoa entonces flotando es true y sale del bucle
          MoverPlayer(&P->colision, &P->origen, M1[i].velocidad, M1[i].dir);
          break;
        }   
        if(DetectarColision(P->colision, M2[i].colision)){
          flotando = true;    //si fltoa entonces flotando es true y sale del bucle
          MoverPlayer(&P->colision, &P->origen, M2[i].velocidad, M2[i].dir);
          break;
        }  
        if(DetectarColision(P->colision, M3[i].colision)){
          flotando = true;    //si fltoa entonces flotando es true y sale del bucle
          MoverPlayer(&P->colision, &P->origen, M3[i].velocidad, M3[i].dir);
          break;
        }
        if(DetectarColision(P->colision, T1[i].colision) && T1[i].frameSub != 5){
          flotando = true;    //si fltoa entonces flotando es true y sale del bucle
          MoverPlayer(&P->colision, &P->origen, T1[i].velocidad, T1[i].dir);
          break;
        }
        if(DetectarColision(P->colision, T2[i].colision) && T2[i].frameSub != 5){
          flotando = true;    //si fltoa entonces flotando es true y sale del bucle
          MoverPlayer(&P->colision, &P->origen, T2[i].velocidad, T2[i].dir);
          break;
        }
      }

      //si no flota muere
      if(flotando == false){
        Death(&P->muriendo, &P->vidas, &P->arriba, P->colision.P1.y);
      }
    }
  }
}

void ColisionBordes(Frog *P){
  if(P->muriendo == false){
    if(P->colision.P2.x < 0){
      Death(&P->muriendo, &P->vidas, &P->arriba, P->colision.P1.y);
    }
    if(P->colision.P1.x > ScreenX){
      Death(&P->muriendo, &P->vidas, &P->arriba, P->colision.P1.y);
    }
  }
  if(P->colision.P1.y > (ScreenY-(SpritesHeight*2))+1){
    P->colision.P1.y -= 48;
    P->colision.P2.y -= 48;
    P->origen = P->colision.P1;
  }
  if(P->colision.P1.y < (ScreenY-(SpritesHeight*13))+1){
    P->colision.P1.y += 48;
    P->colision.P2.y += 48;
    P->origen = P->colision.P1;
  }
}

void DetectarColisionesPlayers(){
  //calculo colisiones jugadores
  if(InGame){
    ColisionPlayerMeta(&Player1);
    ColisionPlayerAuto(&Player1);
    ColisionesPlayerFlotantes(&Player1);
    ColisionBordes(&Player1);
  
    if(numPlayers > 1){
      ColisionPlayerAuto(&Player2);
      ColisionesPlayerFlotantes(&Player2);
    }
  }
}

/********************************************DEBUG**************************************************/
void DibujarRectanguloColision(Colision colision, unsigned char r = 255, unsigned char g = 0, unsigned char b = 0) {
  esat::DrawSetStrokeColor(r, g, b);
  esat::DrawLine(colision.P1.x, colision.P1.y, colision.P2.x, colision.P1.y);
  esat::DrawLine(colision.P1.x, colision.P2.y, colision.P2.x, colision.P2.y);
  esat::DrawLine(colision.P1.x, colision.P1.y, colision.P1.x, colision.P2.y);
  esat::DrawLine(colision.P2.x, colision.P1.y, colision.P2.x, colision.P2.y);
}

void CalculoPuntos(Frog *Player){
  if(Player->arriba > Player->colision.P1.y){
    Player->score += 10;
    Player->arriba = Player->colision.P1.y;
  }
}

/**************************************************************************************************************
**********************************************GAMEPLAY*********************************************************
**************************************************************************************************************/

/**************************************************************************************************************
**********************************************CONTROLES********************************************************
**************************************************************************************************************/

void InputsInGame(){
  //jugador 1 se mueve con las flechas
  //player.jumping sirve para la animacion del salto
  if(Player1.jumping == false){
    if(esat::IsSpecialKeyDown(esat::kSpecialKey_Up)){     //si va pa arriba mueve, cambia direccion y calcula si puntos por distancia
      Player1.origen.y = Player1.colision.P1.y;           //salva posicion del origen para hacer la animacion del salto
      Player1.colision.P1.y -= 48;
      Player1.colision.P2.y -= 48;
      Player1.dir = UP;
      CalculoPuntos(&Player1);
      Player1.jumping = true;
    }
    if(esat::IsSpecialKeyDown(esat::kSpecialKey_Down)){   //mueve para abajo y cambia dir para abajo
      Player1.origen.y = Player1.colision.P1.y;
  
      Player1.colision.P1.y += 48;
      Player1.colision.P2.y += 48;
      Player1.dir = DOWN;
      Player1.jumping = true;
    }
    if(esat::IsSpecialKeyDown(esat::kSpecialKey_Right)){  //mueve y cambia dir para derecha
      Player1.origen.x = Player1.colision.P1.x;
  
      Player1.colision.P1.x += 48;
      Player1.colision.P2.x += 48;
      Player1.dir = RIGHT;
      Player1.jumping = true;
    }
    if(esat::IsSpecialKeyDown(esat::kSpecialKey_Left)){   // mueve y cakmbia dir para izquierda 
      Player1.origen.x = Player1.colision.P1.x;
  
      Player1.colision.P1.x -= 48;
      Player1.colision.P2.x -= 48;
      Player1.dir = LEFT;
      Player1.jumping = true;
    }
  }
}

void InputsInMenu(){
  if(TipoMenu > 0 && esat::IsSpecialKeyDown(esat::kSpecialKey_Left))
    TipoMenu--;
  if(TipoMenu < 2 && esat::IsSpecialKeyDown(esat::kSpecialKey_Right))
    TipoMenu++;
  if(TipoMenu == 1 && esat::IsSpecialKeyDown(esat::kSpecialKey_Up))
    numPlayers = 1;
  if(TipoMenu == 1 && esat::IsSpecialKeyDown(esat::kSpecialKey_Down))
    numPlayers = 2;
  if(Creditos < 99 && esat::IsSpecialKeyDown(esat::kSpecialKey_Keypad_1))
    Creditos++;
  if(TipoMenu == 1 && Creditos > 0 && esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
    InicializarJugadores();   // inicializa players 
    CambiarValoresNivel();    // jenera valores random para el primer nivel
    InGame = true;
    Creditos--;
  }
}

void DetectarInput(){
  if(InGame == true){
    InputsInGame();
  }
  else{
    InputsInMenu();
  }
}

/***********************************************************************************************************
**********************************************DIJUBOS MENU**************************************************
***********************************************************************************************************/

void DrawFrogger(){
  //TO-DO y tambien cambiar el sprite
}

void DubujarMenu(){
  switch(TipoMenu){
    case 0:
      DrawFrogger();

      esat::DrawSetFillColor(255,255,255);
      esat::DrawText((ScreenX/2)-120, SpritesHeight*6, "-POINT TABLE-");
      esat::DrawText((ScreenX/2)-200, SpritesHeight*15, "KONAMI  a  1981");
      
      esat::DrawSetFillColor(255,255,0);
      esat::DrawText((ScreenX/2)-300, SpritesHeight*7, "10 PTS FORRR EACH STEP");
      esat::DrawText((ScreenX/2)-300, (SpritesHeight*8)+20, "50 PTS FOR EVERY FROG");
      esat::DrawText((ScreenX/2)-300, (SpritesHeight*10)+20, "1000 PTS BY SAVING FROGS");
      esat::DrawText((ScreenX/2)-300, (SpritesHeight*12)+20, "PLUS BONUS");

      esat::DrawSetFillColor(255,0,0);
      esat::DrawText((ScreenX/2)-300, SpritesHeight*9, "ARRIVED HOME SAFELY");
      esat::DrawText((ScreenX/2)-300, SpritesHeight*11, "INTO FIVE HOMES");
      esat::DrawText((ScreenX/2)-300, SpritesHeight*13, "10 PTS X REMAINING SECOND");
    break;
    case 1:
      int PlayerSelection;
      esat::DrawSetFillColor(255,255,255);
      esat::DrawText((ScreenX/2)-50, SpritesHeight*5, "PUSH");
      esat::DrawText((ScreenX/2)-100, SpritesHeight*9, "ONE PLAYER");
      esat::DrawText((ScreenX/2)-110, (SpritesHeight*10)+20, "TWO PLAYERS");
      if(numPlayers == 1){
        PlayerSelection = SpritesHeight*9;
      }
      else{
        PlayerSelection = (SpritesHeight*10)+20;
      }
      esat::DrawText((ScreenX/2)-150, PlayerSelection, "->");

      esat::DrawSetFillColor(255,192,203);
      esat::DrawText((ScreenX/2)-120, (SpritesHeight*7), "START BUTTON");

      esat::DrawSetFillColor(255,0,0);
      esat::DrawText((ScreenX/2)-260, SpritesHeight*12, "ONE EXTRA FROGG 20000 PTS");
    break;
    case 2:
      DrawFrogger();
      esat::DrawSetFillColor(255,255,0);
      esat::DrawText((ScreenX/2)-160, SpritesHeight*7, "SCORE RANKING");

      esat::DrawSetFillColor(255,255,255);

      esat::DrawText((ScreenX/2)-200, SpritesHeight*9,  "1 ST         PTS");
      esat::DrawText((ScreenX/2)-200, SpritesHeight*10, "2 ND         PTS");
      esat::DrawText((ScreenX/2)-200, SpritesHeight*11, "3 RD         PTS");
      esat::DrawText((ScreenX/2)-200, SpritesHeight*12, "4 TH         PTS");
      esat::DrawText((ScreenX/2)-200, SpritesHeight*13, "5 TH         PTS");

      char  ScoreHistory1[5] = {0}, 
            ScoreHistory2[5] = {0}, 
            ScoreHistory3[5] = {0}, 
            ScoreHistory4[5] = {0}, 
            ScoreHistory5[5] = {0};

      //itoa de int a char
      itoa(ScoreList[0] +100000, ScoreHistory1, 10);
      itoa(ScoreList[1] +100000, ScoreHistory2, 10);
      itoa(ScoreList[2] +100000, ScoreHistory3, 10);
      itoa(ScoreList[3] +100000, ScoreHistory4, 10);
      itoa(ScoreList[4] +100000, ScoreHistory5, 10);

      esat::DrawText((ScreenX/2)-80, SpritesHeight*9,  ScoreHistory1 + 1);
      esat::DrawText((ScreenX/2)-80, SpritesHeight*10, ScoreHistory2 + 1);
      esat::DrawText((ScreenX/2)-80, SpritesHeight*11, ScoreHistory3 + 1);
      esat::DrawText((ScreenX/2)-80, SpritesHeight*12, ScoreHistory4 + 1);
      esat::DrawText((ScreenX/2)-80, SpritesHeight*13, ScoreHistory5 + 1);


      esat::DrawText((ScreenX/2)-180, SpritesHeight*15, "KONAMI  a  1981");
    break;
  }
}

/**************************************************************************************************************
**********************************************DIBUJOS**********************************************************
**************************************************************************************************************/

void AnimacionMuerte(Frog *P){
  int indiceMuerte = 0;
  float TimerFrames = esat::Time() - TimeMuerte;
  if(TimerFrames < 2000){
    if(TimerFrames < 300){indiceMuerte = 0;}
    else if(TimerFrames < 600){indiceMuerte = 1;}
    else if(TimerFrames < 900){indiceMuerte = 2;}
    else if(TimerFrames < 1200){indiceMuerte = 3;}
    else if(TimerFrames < 1500){indiceMuerte = 4;}
    else if(TimerFrames < 1800){indiceMuerte = 5;}
    else{indiceMuerte = 6;}
  }
  esat::DrawSprite(Muerte[indiceMuerte], P->colision.P1.x, P->colision.P1.y);
  if(TimerFrames > 2100){
    RespawnPlayer();
    P->muriendo = false;
    ResetTime();
  }
}

void DibujarJugador(){
  DibujarRectanguloColision(Player1.colision, 0, 255, 255);
  if(Player1.muriendo == true){
    AnimacionMuerte(&Player1);
  }
  else if(Player1.jumping){
    switch(Player1.dir){
      case RIGHT:
        Player1.origen.x += 12;   //12 es 1/4 de 48 entonces en 4 frames completa el salto
        if(Player1.colision.P1.x > Player1.origen.x){
          esat::DrawSprite(Players[7], Player1.origen.x, Player1.origen.y);
        }
        else{
          Player1.sprite.img = Players[6];  //mira a la derecha
          esat::DrawSprite(Player1.sprite.img, Player1.colision.P1.x, Player1.colision.P1.y);
          Player1.jumping = false;
        }
      break;
      case LEFT:
        Player1.origen.x -= 12;   //12 es 1/4 de 48 entonces en 4 frames completa el salto
        if(Player1.colision.P1.x < Player1.origen.x){
          esat::DrawSprite(Players[3], Player1.origen.x, Player1.origen.y);
        }
        else{
          Player1.sprite.img = Players[2];
          esat::DrawSprite(Player1.sprite.img, Player1.colision.P1.x, Player1.colision.P1.y);
          Player1.jumping = false;
        }
      break;      
      case UP:
        Player1.origen.y -= 12;   //12 es 1/4 de 48 entonces en 4 frames completa el salto
        if(Player1.colision.P1.y < Player1.origen.y){
          esat::DrawSprite(Players[1], Player1.origen.x, Player1.origen.y);
        }
        else{
          Player1.sprite.img = Players[0];
          esat::DrawSprite(Player1.sprite.img, Player1.colision.P1.x, Player1.colision.P1.y);
          Player1.jumping = false;
        }
      break;      
      case DOWN:
        Player1.origen.y += 12;   //12 es 1/4 de 48 entonces en 4 frames completa el salto
        if(Player1.colision.P1.y > Player1.origen.y){
          esat::DrawSprite(Players[5], Player1.origen.x, Player1.origen.y);
        }
        else{
          Player1.sprite.img = Players[4];
          esat::DrawSprite(Player1.sprite.img, Player1.colision.P1.x, Player1.colision.P1.y);
          Player1.jumping = false;
        }
      break;
    }
  }
  else{
    esat::DrawSprite(Player1.sprite.img, Player1.colision.P1.x, Player1.colision.P1.y);
  }
  if(numPlayers > 1)
    esat::DrawSprite(Player2.sprite.img, Player2.colision.P1.x, Player2.colision.P1.y);
    DibujarRectanguloColision(Player2.colision, 0, 255, 255);
}

void DibujarMeta(float X){
  esat::DrawSprite(SpriteMeta, X, 72);
  esat::DrawSprite(SpritePastoVerde, X + esat::SpriteWidth(SpriteMeta), 72);
  esat::DrawSprite(SpritePastoVerde, X + (esat::SpriteWidth(SpriteMeta) + esat::SpriteWidth(SpritePastoVerde)), 72);
}

void DibujarPiso(){
  //pasto arriba
  int Anchura = esat::SpriteWidth(SpriteMeta) + (esat::SpriteWidth(SpritePastoVerde)*2);
  int ranasEnMeta = 0;

  for(int i = 0; i<5; i++){
    DibujarMeta(Anchura*i);
    DibujarRectanguloColision(fin[i].colision, 0, 255, 255);
    
    if(fin[i].inicioEvento + 3000 < esat::Time() && fin[i].evento == 2){  //si pasaron 3 segundos y el evento es cocodrilo
      fin[i].evento = 0;  //vuelve a vacio
    }
    if(fin[i].evento == 3){
      esat::DrawSprite(RanaSafe, fin[i].colision.P1.x, fin[i].colision.P1.y);
      ranasEnMeta += 1;
      if(ranasEnMeta == 5){
        Nivel++;
        CambiarValoresNivel();
      }
    }
  }

  //pasto violeta
  for(int i = 0; i<14; i++){
    esat::DrawSprite(SpritePasto,i*esat::SpriteWidth(SpritePasto), ScreenY/2);
    esat::DrawSprite(SpritePasto,i*esat::SpriteWidth(SpritePasto), ScreenY-96);
  }
}

void MoverObjeto(Colision *coli, Direccion dire, int speed){
  if(dire == RIGHT){
    coli->P1.x += speed;
    coli->P2.x += speed;
  }else{ // LEFT
    coli->P1.x -= speed;
    coli->P2.x -= speed;
  }
}

void RetrocederColisiones(Colision *coli, Direccion dire, int esp){
  if (dire == RIGHT) {
    coli->P1.x -= esp;
    coli->P2.x -= esp;
  } 
  else { // LEFT
    coli->P1.x += esp;
    coli->P2.x += esp;
  }
}

//la verdad que es horriblemente larga y fea esta funcion
void NoSalirDePantalla(){
  //TO-DO si hay tiempo hacer un for adentro de cada if y paso solo la colision asi puedo reutilisar la misma funcion tanto para flotantes que para autos
  //este codigo controla si los objetos que se mueven salen o no de la pantalla
  //si si llama funcion de retroceder
  if(F1[0].colision.P2.x < 0){ //si el de la izquierda sale de pantalla por la dimension de su sprite
    for(int i = 0; i<5; i++){
      RetrocederColisiones(&F1[i].colision, F1[i].dir, F1[i].espacio);
    }
  }
  if(F2[4].colision.P1.x > ScreenX){  //si sale a la derecha
    for(int i = 0; i<5; i++){
      RetrocederColisiones(&F2[i].colision, F2[i].dir, F2[i].espacio);
    }
  }
  if(F3[0].colision.P2.x < 0){
    for(int i = 0; i<5; i++){
      RetrocederColisiones(&F3[i].colision, F3[i].dir, F3[i].espacio);
    }
  }
  if(F4[4].colision.P1.x > ScreenX){
    for(int i = 0; i<5; i++){
      RetrocederColisiones(&F4[i].colision, F4[i].dir, F4[i].espacio);
    }
  }
  if(F5[0].colision.P2.x < 0){
    for(int i = 0; i<5; i++){
      RetrocederColisiones(&F5[i].colision, F5[i].dir, F5[i].espacio);
    }
  }
  //ahora el rio
  if(M1[4].colision.P1.x > ScreenX){
    for(int i = 0; i<5; i++){
      RetrocederColisiones(&M1[i].colision, M1[i].dir, M1[i].espacio);
    }
  }
  if(M2[4].colision.P1.x > ScreenX){
    for(int i = 0; i<5; i++){
      RetrocederColisiones(&M2[i].colision, M2[i].dir, M2[i].espacio);
    }
  }
  if(M3[4].colision.P1.x > ScreenX){
    for(int i = 0; i<5; i++){
      RetrocederColisiones(&M3[i].colision, M3[i].dir, M3[i].espacio);
    }
  }
  //esto lleva la tortga que sale atras de la tortuga 
  //Ahora que lo pienso es un mejor sistema, no me hace falta un bucle
  if(T1[PrimeraT1].colision.P2.x < 0){
    T1[PrimeraT1].colision.P1.x = T1[UltimaT1].colision.P1.x + T1[UltimaT1].espacio;
    T1[PrimeraT1].colision.P2.x = T1[UltimaT1].colision.P2.x + T1[UltimaT1].espacio;
    
    if(PrimeraT1 < 4){
      PrimeraT1++;
    }
    else{
      PrimeraT1 = 0;
    }
    if(UltimaT1 < 4){
      UltimaT1++;
    }
    else{
      UltimaT1 = 0;
    }
  }
  if(T2[PrimeraT2].colision.P2.x < 0){
    T2[PrimeraT2].colision.P1.x = T2[UltimaT2].colision.P1.x + T2[UltimaT2].espacio;
    T2[PrimeraT2].colision.P2.x = T2[UltimaT2].colision.P2.x + T2[UltimaT2].espacio;
    
    if(PrimeraT2 < 4){
      PrimeraT2++;
    }
    else{
      PrimeraT2 = 0;
    }
    if(UltimaT2 < 4){
      UltimaT2++;
    }
    else{
      UltimaT2 = 0;
    }
  }
}

void DibujarVeiculos(){
  for(int i = 0; i < 5; i++){
    //Mueve la coluision de los autos antes de dibujarlos
    MoverObjeto(&F1[i].colision, F1[i].dir, F1[i].velocidad);
    MoverObjeto(&F2[i].colision, F2[i].dir, F2[i].velocidad);
    MoverObjeto(&F3[i].colision, F3[i].dir, F3[i].velocidad);
    MoverObjeto(&F4[i].colision, F4[i].dir, F4[i].velocidad);
    MoverObjeto(&F5[i].colision, F5[i].dir, F5[i].velocidad);
    //dibuja los autos
    esat::DrawSprite(F1[i].sprite.img, F1[i].colision.P1.x, F1[i].colision.P1.y); 
    esat::DrawSprite(F2[i].sprite.img, F2[i].colision.P1.x, F2[i].colision.P1.y);
    esat::DrawSprite(F3[i].sprite.img, F3[i].colision.P1.x, F3[i].colision.P1.y);
    esat::DrawSprite(F4[i].sprite.img, F4[i].colision.P1.x, F4[i].colision.P1.y);
    esat::DrawSprite(F5[i].sprite.img, F5[i].colision.P1.x, F5[i].colision.P1.y);
    
    DibujarRectanguloColision(F1[i].colision, 255, 0, 0);
    DibujarRectanguloColision(F2[i].colision, 255, 0, 0);
    DibujarRectanguloColision(F3[i].colision, 255, 0, 0);
    DibujarRectanguloColision(F4[i].colision, 255, 0, 0);
    DibujarRectanguloColision(F5[i].colision, 255, 0, 0);
  }
}

void DibujarMadera(int b, Colision coli, int n){
  if(b == 0){
    esat::DrawSprite(Madera[0], coli.P1.x, coli.P1.y);
  }
  else if(b == n-1){
    esat::DrawSprite(Madera[2], coli.P1.x + (48*b), coli.P1.y);
  }
  else{
    if(b < n){
      esat::DrawSprite(Madera[1], coli.P1.x + (48*b), coli.P1.y);
    }
    else{}
  }
}

void AnimacionTortuga(Tortuga *T, int b){
  if(T->hunde){                //si se puede hundir
    if(T->flota == false){     //si esta hundiendose
      double tiempoHundimiento = (esat::Time() - T->hundimientoInicio) - T->espera; //Time va a seguir creciendo hasta que el if sea verdad
      if(tiempoHundimiento < 2000){   //2 segundos
        if(tiempoHundimiento < 333 || tiempoHundimiento > 1666){   //frame 3 es el primero y el ultimo
          T->frameSub = 3;
          esat::DrawSprite(Tortugas[T->frameSub], T->colision.P1.x + (48*b), T->colision.P1.y);
        }
        else if(666 > tiempoHundimiento && tiempoHundimiento < 999 || tiempoHundimiento > 1333 && tiempoHundimiento < 1666){  //frame 4 es el segundo y el penultimo
          T->frameSub = 4;   //reinica el temporizador
          esat::DrawSprite(Tortugas[T->frameSub], T->colision.P1.x + (48*b), T->colision.P1.y);
        }
        else{   //quedan solo entre 999 y 1333 que es el frame 
          T->frameSub = 5;   //en este caso simplemente no dibuja
        }
      }
      else{
        esat::DrawSprite(Tortugas[T->frame], T->colision.P1.x + (48*b), T->colision.P1.y);
        T->flota = true;                       // No esta mas abajo del agua
        T->hundimientoInicio = esat::Time();   // Reinicia el temporizador
      }
    }
    else{
      if(T->espera + T->hundimientoInicio < esat::Time()){  //resien sale de el if de arriba asi que seguro es mas y por consequencia falso
        T->flota = false;

      }
      esat::DrawSprite(Tortugas[T->frame], T->colision.P1.x + (48*b), T->colision.P1.y);
    }
  }
  else{
    esat::DrawSprite(Tortugas[T->frame], T->colision.P1.x + (48*b), T->colision.P1.y);
  }
}

void DibujarFlotantes(){
  for(int i = 0; i < 5; i++){
    //mueve la colsion de todos
    MoverObjeto(&T1[i].colision, T1[i].dir, T1[i].velocidad);
    MoverObjeto(&T2[i].colision, T2[i].dir, T2[i].velocidad);
    MoverObjeto(&M1[i].colision, M1[i].dir, M1[i].velocidad);
    MoverObjeto(&M2[i].colision, M2[i].dir, M2[i].velocidad);
    MoverObjeto(&M3[i].colision, M3[i].dir, M3[i].velocidad);
    
    for(int j = 0; j < M2[0].no; j++){    //M2 porque tiene el sprite mas ancho
      // si es el primer bucle entonces si o si es el primer chunk de el tronco
      DibujarMadera(j, M1[i].colision, M1[i].no);
      DibujarMadera(j, M2[i].colision, M2[i].no);
      DibujarMadera(j, M3[i].colision, M3[i].no);
    }
    
    // cambia frame de nadar 3 veces por segundo
    if(TimeAnimacion+333 < esat::Time()){
      for(int j = 0; j < 5; j++){
        T1[j].frame++;                  //aumento el frame de todos
        T2[j].frame++;
        if(T1[j].frame > 2){            // frames nadando van de posicion 0 a la 2
          T1[j].frame = 0;              // Por lo tanto si se pasa pongo el frame al 0 para reiniciar
          T2[j].frame = 0;
        }
      }
      TimeAnimacion = esat::Time();     //reinica el temporizador
    }

    for(int j = 0; j < 3; j++){         // Las tortugas son 2 o 3
      if(j<2){
        AnimacionTortuga(&T2[i], j);
      }
      AnimacionTortuga(&T1[i], j);
    }

    DibujarRectanguloColision(M1[i].colision, 255, 0, 0);
    DibujarRectanguloColision(M2[i].colision, 255, 0, 0);
    DibujarRectanguloColision(M3[i].colision, 255, 0, 0);
    DibujarRectanguloColision(T1[i].colision, 255, 0, 0);
    DibujarRectanguloColision(T2[i].colision, 255, 0, 0);
  }
}

void CalculoScorePlayer(int PTS) {
    for (int i = 0; i < 5; i++) {
        if (PTS > ScoreList[i]) {
            // reduce la posicion de 1 por cada inferior
            for (int j = 4; j > i; j--) {
                ScoreList[j] = ScoreList[j - 1];
            }
            // Mete un nuevo 
            ScoreList[i] = PTS;
            break;
        }
    }
}

//esto c llama al game over
void GuardarScore() {
  CalculoScorePlayer(Player1.score);
  if(numPlayers>1)
    CalculoScorePlayer(Player2.score);
}

void GameOver(){
  if(Player1.vidas < 0){
    esat::DrawSetFillColor(255,0,0);
    esat::DrawText((ScreenX/2)-120, ScreenY/2, "-GAME OVER-");
    float TimerMuerte = esat::Time() - TimeMuerte;
    if(TimerMuerte > 2000){
      InGame = false;
      TipoMenu = 2;
      GuardarScore();
      ResetPlayer(&Player1);
      Nivel = 0;
      UltimaT1 = 4;
      UltimaT2 = 4;
      PrimeraT1 = 0;
      PrimeraT2 = 0;
    }
  }
}

void DibujarJuego(){
  NoSalirDePantalla();  //controla si el autooflotantes se salgan de la pantalla
  DibujarPiso();
  DibujarVeiculos();
  DibujarFlotantes();
  DibujarJugador();
  GameOver();
}

  // UI
void DibujarCabecera(){
  char ScoreP1[5] = {0}, ScoreP2[5] = {0}, highScoreChars[5] = {0};
  //itoa de int a char
  itoa(Player1.score +100000, ScoreP1, 10);
  itoa(Player2.score +100000, ScoreP2, 10);
  itoa(ScoreList[0] +100000, highScoreChars, 10);

  esat::DrawSetFillColor(255,255,255);
  
  esat::DrawText((ScreenX/2)-230, 23, "1-UP   HI-SCORE");
  if(numPlayers > 1)
    esat::DrawText((ScreenX/2)+140, 25, "2-UP");
  
  esat::DrawSetFillColor(255,0,0);
  esat::DrawText((ScreenX/2)-250, 43, ScoreP1+1);
  esat::DrawText((ScreenX/2)-60, 43, highScoreChars+1);
  if(numPlayers > 1)
  esat::DrawText((ScreenX/2)+140, 43, ScoreP2+1);
}

void DibujarPantalla(){
  if(InGame){
    DibujarJuego();
  }
  else{
    DubujarMenu();
  }

}

//Pie de pagina
void DibujarVidas(){
  for(int i = 0; i < Player1.vidas; i++){
    esat::DrawSprite(SpriteVidas, i*esat::SpriteWidth(SpriteVidas), ScreenY-48);
  }
  if(numPlayers > 1){
    for(int i = 0; i < Player2.vidas; i++){
      esat::DrawSprite(SpriteVidas, i*esat::SpriteWidth(SpriteVidas), (ScreenY-24)+esat::SpriteHeight(SpriteVidas));
    }
  }
}

void DibujarNivel(){
  for(int i = 0; i < Nivel; i++){
    esat::DrawSprite(SpriteNivel, (ScreenX - 48) - (i*esat::SpriteWidth(SpriteNivel)), ScreenY-48);
  }
}

void TibujarTime(){
  // LevelTime es los segundos
  if(Player1.muriendo == false){
    if((LevelTime + (1000*SecondPassed) + 1000) < esat::Time()){
      SecondPassed += 1;
    }
  }

  if(Player1.muriendo == false){
    if(TotalSeconds < SecondPassed){
      Death(&Player1.muriendo, &Player1.vidas, &Player1.arriba, Player1.colision.P1.y);
    }
  }

  float TimeSpace[10] = {(float)((ScreenX - 500) + (SecondWidth*SecondPassed)), (float)(ScreenY - 24),// cordenadas barra tiempo
                      (float)(ScreenX - 100), (float)(ScreenY - 24),
                      (float)(ScreenX - 100) , (float)(ScreenY - 5),
                      (float)((ScreenX - 500) + (SecondWidth*SecondPassed)), (float)(ScreenY - 5),
                      (float)((ScreenX - 500) + (SecondWidth*SecondPassed)), (float)(ScreenY - 24)
                      };

    
  esat::DrawSetFillColor(0, 255, 0);
  esat::DrawSolidPath(TimeSpace, 5);

  esat::DrawSetFillColor(255, 255, 0);
  esat::DrawText(ScreenX - 90, ScreenY - 5, "TIME");
}

void DibujarCreditos(){
  //itoa de int a char
  char coins[2];
  itoa(Creditos+100, coins, 10);

  esat::DrawSetFillColor(144,213,255);
  
  esat::DrawText(ScreenX-180, ScreenY-40, "CREDIT");
  esat::DrawText(ScreenX-50, ScreenY-40, coins+1);
}

void DibujarPie(){
  if(InGame){
    DibujarVidas();
    DibujarNivel();
    TibujarTime();
  }
  else{
    DibujarCreditos();
  }
}

void Display(){
  DibujarCabecera();
  DibujarPantalla();
  DibujarPie();
}

/**************************************************************************************************************
**********************************************SISTEMA**********************************************************
**************************************************************************************************************/

void DibujarFondo(){
  esat::DrawClear(0,0,0);
  esat::DrawSetFillColor(0,4,74);
  esat::DrawSolidPath(Points,5);
}



void ControlFPS(){
  do{
    current_time = esat::Time();
  }while((current_time-last_time)<=1000.0/fps);
}

void ReleaseOfSprites(){
  esat::SpriteRelease(SpriteLetras);
  esat::SpriteRelease(SpriteNivel);
  esat::SpriteRelease(SpriteVidas);
  esat::SpriteRelease(SpriteTiempo);
  esat::SpriteRelease(SpriteMeta);
  esat::SpriteRelease(SpritePastoVerde);
  esat::SpriteRelease(SpritePasto);
  esat::SpriteRelease(SpriteMosca);
  esat::SpriteRelease(SpriteCocodriloCampero);
  esat::SpriteRelease(SpriteCocodlio);
  esat::SpriteRelease(SpriteCamion);
  esat::SpriteRelease(SpriteAuto1);
  esat::SpriteRelease(SpriteAuto2);
  esat::SpriteRelease(SpriteAuto3);
  esat::SpriteRelease(SpriteAuto4);
  esat::SpriteRelease(SpritePerro);
  esat::SpriteRelease(SpriteSerpiente);
}

void ReleaseSpriteSheets(){
  esat::SpriteRelease(SpriteFrog);
  esat::SpriteRelease(SpriteMuerte);
  esat::SpriteRelease(SpriteTronco);
  esat::SpriteRelease(SpriteTortuga);
}

int esat::main(int argc, char **argv) {
  srand(time(NULL));
  esat::WindowInit(ScreenX, ScreenY);
  WindowSetMouseVisibility(true);

  LoadSprites();
  ReleaseSpriteSheets();
  InicializarAutos();
  InicializarFlotantes();
  //fuente
  esat::DrawSetTextFont("./Assets/font/arcade-legacy.ttf");
  esat::DrawSetTextSize(FontSize);
  

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)){
    last_time = esat::Time();
    esat::DrawBegin();
    
    DibujarFondo();       //dibuja el background
    
    DetectarInput();
    
    Display();            //Dibuja todos los objetos que se mueven
    
    DetectarColisionesPlayers();

    esat::DrawEnd();
    ControlFPS();
    esat::WindowFrame();
  }
  ReleaseOfSprites();
  esat::WindowDestroy();
  return 0;
}