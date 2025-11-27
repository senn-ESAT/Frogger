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
  PuntoCoord P1;// P1 Tambien sirve como ubicación
  PuntoCoord P2;
};

struct Sprite{
  esat::SpriteHandle img;   //imagen
  int IndiceY = 0, IndiceX = 0; //indice spritesheet
};

//-------------Player-------------
struct Frog{
  Colision colision;
  Sprite sprite;
  Direccion dir;
  int vidas = 3, score;
};

//-------------Generales-------------
struct SafeZone{
  Colision colision;
  Sprite sprite;
  int evento; //en plan 0 = agua, 1 = mosca, 2 = cocodrilo, 4 = rana/player
};

//-------------Objetos-------------
struct Autos{
  Colision colision;
  Sprite sprite;
  Direccion dir;
  int velocidad;
};

struct Tronco{
  Colision colision;
  Sprite sprite;
  int velocidad;
};

struct Tortuga{
  Colision colision;
  Sprite sprite;
  int velocidad;
  bool flota = true; //en plan esta abajo del agua?
};

struct Cocodlio{
  Colision body;
  Colision head;
  Sprite sprite;
  int velocidad;
};

//-------------NPC-------------
struct Serpiente{
  Colision colision;
  Sprite sprite;
  Direccion direccion;
  int velocidad;
};

struct Perro{
  Colision colision;
  Sprite sprite;
  Direccion direccion; // o --> o <--
  int velocidad;
};

struct Mosca{
  Colision colision;
  esat::SpriteHandle sprite;
  int Puntos;
};

struct RanaNPC{
  Colision colision;
  Sprite sprite;
  int dir; //direccion -> 0 = arriba, 1=derecha, 2 = abajo, 3 = izquierda
};

Frog Player1, Player2;

//Variables de sistema
const int FontSize = 20, Margen = 100;
const float ScreenX = 672, ScreenY = 768; //screen size
unsigned char fps=25; //Control de frames por segundo
double current_time, last_time;
float Points[10] = {0,0,ScreenX,0,ScreenX,ScreenY/2,0,ScreenY/2,0,0}; //zona azul
int TipoPantalla = 1, numPlayers = 1, highScore = 00000;

//Sprite Handles
//UI
esat::SpriteHandle SpriteLetras;
esat::SpriteHandle SpriteNivel;
esat::SpriteHandle SpriteVidas;
esat::SpriteHandle SpriteTiempo;
//Extras
esat::SpriteHandle SpriteMeta;
esat::SpriteHandle SpritePastoVerde;
esat::SpriteHandle SpritePasto;
esat::SpriteHandle SpriteMosca;
esat::SpriteHandle SpriteCocodriloCampero;
//Player
esat::SpriteHandle SpriteFrog;
esat::SpriteHandle SpriteMuerte;
//Flotantes
esat::SpriteHandle SpriteTronco;
esat::SpriteHandle SpriteTortuga;
esat::SpriteHandle SpriteCocodlio;
//Veiculos
esat::SpriteHandle SpriteCamion;
esat::SpriteHandle SpriteAuto1;
esat::SpriteHandle SpriteAuto2;
esat::SpriteHandle SpriteAuto3;
esat::SpriteHandle SpriteAuto4;
//Enemigos
esat::SpriteHandle SpritePerro;
esat::SpriteHandle SpriteSerpiente;

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
}

/*******************************************************
********************Colisiones**************************
*******************************************************/

/*******************************************************
********************GAMEPLAy****************************
*******************************************************/

/*******************************************************
********************CONTROLES***************************
*******************************************************/

void InputsInGame(){
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Up)){
    Player1.colision.P1.y -= 48;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Down)){
    Player1.colision.P1.y += 48;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Right)){
    Player1.colision.P1.x += 48;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Left)){
    Player1.colision.P1.x -= 48;
  }
}

void InputsInMenu(){
  //TO-DO
}

void DetectarInput(){
  if(TipoPantalla == 1){
    InputsInGame();
  }
  else{
    InputsInMenu();
  }
}

/*******************************************************
********************DIJUBOS*****************************
*******************************************************/

void DibujarJugador(){
  esat::DrawSprite(Player1.sprite.img, Player1.colision.P1.x, Player1.colision.P1.y);
  if(numPlayers>1){
    //TO-DO dibujar player 2
  }
}

void DibujarMeta(float X){
  esat::DrawSprite(SpriteMeta, X, 72);
  esat::DrawSprite(SpritePastoVerde,X + esat::SpriteWidth(SpriteMeta), 72);
    esat::DrawSprite(SpritePastoVerde,X + (esat::SpriteWidth(SpriteMeta)+esat::SpriteWidth(SpritePastoVerde)), 72);
}

void DibujarPiso(){
  //pasto arriba
  int Anchura = esat::SpriteWidth(SpriteMeta)+(esat::SpriteWidth(SpritePastoVerde)*2);
  for(int i = 0; i<5; i++){
    DibujarMeta(Anchura*i);
  }

  //pasto centro
  for(int i = 0; i<14; i++){
    esat::DrawSprite(SpritePasto,i*esat::SpriteWidth(SpritePasto), ScreenY/2);
    esat::DrawSprite(SpritePasto,i*esat::SpriteWidth(SpritePasto), ScreenY-96);
  }
}

void DibujarJuego(){
  DibujarPiso();
  DibujarJugador();
  DibujarVeiculos();
  DibujarFlotantes();
}

  // UI
void DibujarCabecera(){
  char ScoreP1[5] = {0}, ScoreP2[5] = {0}, highScoreChars[5] = {0};
  //itoa de int a char
  itoa(Player1.score +100000, ScoreP1, 10);
  itoa(Player2.score +100000, ScoreP2, 10);
  itoa(highScore +100000, highScoreChars, 10);

  esat::DrawSetFillColor(255,255,255);
  
  esat::DrawText((ScreenX/2)-230, 23, "1-UP   HI-SCORE");
  if(numPlayers > 1)
    esat::DrawText((ScreenX/2)+140, 25, "2-UP");
  
  esat::DrawSetFillColor(255,0,0);
  esat::DrawText((ScreenX/2)-250, 43, ScoreP1+1);
  esat::DrawText((ScreenX/2)-60, 43, highScoreChars+1);
  if(numPlayers > 1)
  esat::DrawText((ScreenX/2)+140, 25, ScoreP2+1);
}

//esto c llama al game over
void GuardarHighScore() {
    //Comprueba solo el jugador 1 al no tener modo 2 jugadores implementado
    if (Player1.score > highScore) {
      highScore = Player1.score;
    }
    if (Player2.score > highScore) {
      highScore = Player2.score;
    }
}




void DibujarPantalla(){
  //TO-DO if in game (juego) o menu (animacion)
  switch(TipoPantalla){
    case 0:
    //TO-DO DubujarMenu();
    break;
    case 1:
    DibujarJuego();
    break;
    case 2:
    //TO-DO DibujarCreditos();
    break;
  }
}

//Pie de pagina
void DibujarVidas(){
  for(int i = 0; i < Player1.vidas; i++){
    esat::DrawSprite(SpriteVidas, i*esat::SpriteWidth(SpriteVidas), Screeny-48);
  }
  if(numPlayers > 1){
    for(int i = 0; i < Player2.vidas; i++){
      esat::DrawSprite(SpriteVidas, i*esat::SpriteWidth(SpriteVidas), (Screeny-48)+esat::SpriteHeight(SpriteVidas));
    }
  }
}

void DibujarPie(){
  if(TipoPantalla == 1){
    DibujarVidas();
    DibujarNivel();
    TibujarTime();
  }
  else{
    //TO-DO display creditos
  }
}

void Dispaly(){
  DibujarCabecera();
  DibujarPantalla();
  DibujarPie();
}

/*******************************************************
********************Sistema*****************************
*******************************************************/

void InicializarJugadores(){
  Player1.colision.P1.x = (ScreenX/2)-24;
  Player1.colision.P1.y = ScreenY-96;
  Player1.sprite.img = esat::SubSprite(SpriteFrog, 0,0,48,48);
  Player1.colision.P2.x = Player1.colision.P1.x + esat::SpriteWidth(Player1.sprite.img);
  Player1.colision.P2.y = Player1.colision.P1.y + esat::SpriteHeight(Player1.sprite.img);
}

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
  //UI
  esat::SpriteRelease(SpriteLetras);
  esat::SpriteRelease(SpriteNivel);
  esat::SpriteRelease(SpriteVidas);
  esat::SpriteRelease(SpriteTiempo);
  //Extras
  esat::SpriteRelease(SpriteMeta);
  esat::SpriteRelease(SpritePastoVerde);
  esat::SpriteRelease(SpritePasto);
  esat::SpriteRelease(SpriteMosca);
  esat::SpriteRelease(SpriteCocodriloCampero);
  //Player
  esat::SpriteRelease(SpriteFrog);
  esat::SpriteRelease(SpriteMuerte);
  //Flotantes
  esat::SpriteRelease(SpriteTronco);
  esat::SpriteRelease(SpriteTortuga);
  esat::SpriteRelease(SpriteCocodlio);
  //Veiculos
  esat::SpriteRelease(SpriteCamion);
  esat::SpriteRelease(SpriteAuto1);
  esat::SpriteRelease(SpriteAuto2);
  esat::SpriteRelease(SpriteAuto3);
  esat::SpriteRelease(SpriteAuto4);
  //Enemigos
  esat::SpriteRelease(SpritePerro);
  esat::SpriteRelease(SpriteSerpiente);
}

int esat::main(int argc, char **argv) {
  srand(time(NULL));
  esat::WindowInit(ScreenX, ScreenY);
  WindowSetMouseVisibility(true);

  LoadSprites();
  InicializarJugadores();
  //fuente
  esat::DrawSetTextFont("./Assets/font/arcade-legacy.ttf");
  esat::DrawSetTextSize(FontSize);
  
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)){
    last_time = esat::Time();
    esat::DrawBegin();

    DibujarFondo();

    DetectarInput();

    Dispaly();

    esat::DrawEnd();
    ControlFPS();
    esat::WindowFrame();
  }
  ReleaseOfSprites();
  esat::WindowDestroy();
  return 0;
}