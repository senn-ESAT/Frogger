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
  int dir, puntuacion; //direccion -> 0 = arriba, 1=derecha, 2 = abajo, 3 = izquierda
  int vidas = 3;
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
  D
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
const int FontSize = 19, Margen = 100;
const float ScreenX = 672, ScreenY = 768; //screen size
unsigned char fps=25; //Control de frames por segundo
double current_time, last_time;
float Points[10] = {0,0,ScreenX,0,ScreenX,ScreenY/2,0,ScreenY/2,0,0}; //zona azul
int TipoPantalla = 1, numPlayers, highScore = 0;; 

//Sprite Handles
//UI
esat::SpriteHandle SpriteLetras;
esat::SpriteHandle SpriteNivel;
esat::SpriteHandle SpriteVidas;
esat::SpriteHandle SpriteTiempo;
//Extras
esat::SpriteHandle SpriteMeta;
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


//Rotacion Sprites
//si tengo tiempo
// esat::SpriteTransform Rotacion;
// RotacionImagen(DERECHA, esat::SpriteHeight(prueba), esat::SpriteWidth(prueba));
// esat::DrawSprite(prueba, Rotacion);
// RotacionImagen(IZQUIERDA, esat::SpriteHeight(prueba), esat::SpriteWidth(prueba));
// esat::DrawSprite(prueba, Rotacion);
// RotacionImagen(ARRIBA, esat::SpriteHeight(prueba), esat::SpriteWidth(prueba));
// esat::DrawSprite(prueba, Rotacion);
// RotacionImagen(ABAJO, esat::SpriteHeight(prueba), esat::SpriteWidth(prueba));
// esat::DrawSprite(prueba, Rotacion);
// void RotacionImagen(Direccion dir, int alto, int ancho){
//   switch(dir){
//     case ARRIBA:
//       Rotacion.angle = 0;
//     break;
//     case DERECHA:
//       Rotacion.x += ancho;
//       Rotacion.angle = 1.5708;
//     break;
//     case ABAJO:
//       Rotacion.y -= alto;
//       Rotacion.angle = 3.14159;
//     break;
//     case IZQUIERDA:
//       Rotacion.y -= alto;
//       Rotacion.angle = 4.71239;
//     break;  
//   }
// }



void LoadSprites(){
  //UI
  SpriteLetras = esat::SpriteFromFile("./Assets/img/SheetLetras.png");
  SpriteNivel = esat::SpriteFromFile("./Assets/img/Nivel.png");
  SpriteVidas = esat::SpriteFromFile("./Assets/img/Vidas.png");
  SpriteTiempo = esat::SpriteFromFile("./Assets/img/SheetTime.png");
  //Extras
  SpriteMeta = esat::SpriteFromFile("./Assets/img/Meta.png");
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
    Player1.colision.P1.y += 48;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Down)){
    Player1.colision.P1.y -= 48;
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
  if(numPlayers>2){
    //TO-DO dibujar player 2
  }
}

void DibujarPiso(){

}


  // UI
void DibujarCabecera(){
  char puntuacionJ1[5], puntuacionJ2[5], highScoreChars[5];
  int c = ((esat::Time()/100.0f) - tempTextosSeleccionJugadores);

  itoa(Player1.puntuacion + 10000, puntuacionJ1, 10);
  itoa(Player2.puntuacion + 10000, puntuacionJ2, 10);
  itoa(highScore + 10000, highScoreChars, 10);
    
  esat::DrawText(Margen+6,FontSize+10,"1-UP   HI-SCORE");

  esat::DrawText(Margen+6,(FontSize+10)*2, puntuacionJ1+1);
    
  esat::DrawText((ScreenX*0.5)-FontSize*2,(FontSize+10)*2, highScoreChars+1);
  esat::DrawText((100-6)-(FontSize*4),(FontSize+10)*2,puntuacionJ2+1);
}
  void DrawSetTextFont(const char *name);
  void DrawSetTextSize(float size);
  void DrawSetTextBlur(float blur_radius);
  void DrawText(float x, float y, const char *text);

void GuardarHighScore() {
    //Comprueba solo el jugador 1 al no tener modo 2 jugadores implementado
    if (Player1.puntuacion > highScore) {
      highScore = Player1.puntuacion;
    }
    if (Player2.puntuacion > highScore) {
      highScore = Player2.puntuacion;
    }
}


void DibujarPie(){
  if(TipoPantalla == 1){
    //TO-DO dibujar vidas, nivel y TIME
  }
  else{
    //TO-DO display creditos
  }
}

void DibujarJuego(){
  DibujarPiso();
  DibujarJugador();
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


void Dispaly(){
  DibujarCabecera();
  DibujarPantalla();
  DibujarPie();
}

/*******************************************************
********************Sistema*****************************
*******************************************************/

void InicializarJugadores(){
  Player1.colision.P1.x = ScreenX/2;
  Player1.colision.P1.y = ScreenY-96;
  Player1.sprite.img = esat::SubSprite(SpriteFrog, 0,0,36,39);
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
  esat::DrawSetTextFont("Desarrollo/Frogger/Assets/font/arcade-legacy.ttf");
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