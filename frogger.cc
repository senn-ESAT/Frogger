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
  PuntoCoord P1;  // P1 Tambien sirve como ubicación
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
  float arriba;
};

//-------------Generales-------------
struct SafeZone{
  Colision colision;
  int evento; //en plan 0 = agua, 1 = mosca, 2 = cocodrilo, 4 = rana/player
};

//-------------Objetos-------------
struct Autos{
  Colision colision;
  Sprite sprite;
  Direccion dir;
  int velocidad, ancho, espacio;
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
SafeZone fin[5];
Autos F1[5], F2[5], F3[5], F4[5], F5[5];
Tronco W1[5], W2[5], W3[5];
Tortuga T1[5], T2[5];

//Variables de sistema
const int FontSize = 20, SpritesHeight = 48;
const int ScreenX = 672, ScreenY = 768; //screen size
unsigned char fps=25; //Control de frames por segundo
double current_time, last_time;
float Points[10] = {0,0,ScreenX,0,ScreenX,ScreenY/2,0,ScreenY/2,0,0}; //zona azul
int TipoPantalla = 1, numPlayers = 1, highScore = 0, Nivel = 0, LastNivel = 0;

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

/********************************************DEBUG************************************************ */
void DibujarRectanguloColision(Colision colision, unsigned char r = 255, unsigned char g = 0, unsigned char b = 0) {
  esat::DrawSetStrokeColor(r, g, b);
  esat::DrawLine(colision.P1.x, colision.P1.y, colision.P2.x, colision.P1.y);
  esat::DrawLine(colision.P1.x, colision.P2.y, colision.P2.x, colision.P2.y);
  esat::DrawLine(colision.P1.x, colision.P1.y, colision.P1.x, colision.P2.y);
  esat::DrawLine(colision.P2.x, colision.P1.y, colision.P2.x, colision.P2.y);
}

void CalculoPuntos(Frog *Player){
  if(Player.arriba < Player.colision.P1.y){
    Player.score += 10;
  }
}

/*******************************************************
********************GAMEPLAY****************************
*******************************************************/

/*******************************************************
********************CONTROLES***************************
*******************************************************/

void InputsInGame(){
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Up)){
    Player1.colision.P1.y -= 48;
    Player1.colision.P2.y -= 48;
    CalculoPuntos(&Player1);
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Down)){
    Player1.colision.P1.y += 48;
    Player1.colision.P2.y += 48;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Right)){
    Player1.colision.P1.x += 48;
    Player1.colision.P2.x += 48;
  }
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Left)){
    Player1.colision.P1.x -= 48;
    Player1.colision.P2.x -= 48;
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
  DibujarRectanguloColision(Player1.colision, 0, 255, 255);
  esat::DrawSprite(Player1.sprite.img, Player1.colision.P1.x, Player1.colision.P1.y);
  if(numPlayers > 1){
    esat::DrawSprite(Player2.sprite.img, Player2.colision.P1.x, Player2.colision.P1.y);
  }
}

void DibujarMeta(float X){
  esat::DrawSprite(SpriteMeta, X, 72);
  esat::DrawSprite(SpritePastoVerde, X + esat::SpriteWidth(SpriteMeta), 72);
  esat::DrawSprite(SpritePastoVerde, X + (esat::SpriteWidth(SpriteMeta) + esat::SpriteWidth(SpritePastoVerde)), 72);
}

void DibujarPiso(){
  //pasto arriba
  int Anchura = esat::SpriteWidth(SpriteMeta) + (esat::SpriteWidth(SpritePastoVerde)*2);
  for(int i = 0; i<5; i++){
    if(fin[4].colision.P1.x == 0){
      fin[i].colision.P1.x = (Anchura*i) + 24;
      fin[i].colision.P1.y = 96;
      fin[i].colision.P2.x = fin[i].colision.P1.x + 48;
      fin[i].colision.P2.y = 144;
    }
    DibujarRectanguloColision(fin[i].colision, 0, 255, 255);
    DibujarMeta(Anchura*i);
  }

  //pasto violeta
  for(int i = 0; i<14; i++){
    esat::DrawSprite(SpritePasto,i*esat::SpriteWidth(SpritePasto), ScreenY/2);
    esat::DrawSprite(SpritePasto,i*esat::SpriteWidth(SpritePasto), ScreenY-96);
  }
}

void MoverObjeto(Autos *A){
  for (int i = 0; i < 5; i++) {
    if (A[i].dir == RIGHT) {
      A[i].colision.P1.x += A[i].velocidad;
      A[i].colision.P2.x += A[i].velocidad;
    } else { // LEFT
      A[i].colision.P1.x -= A[i].velocidad;
      A[i].colision.P2.x -= A[i].velocidad;
    }
  }
}

void RetrocederAutos(Autos *A){
  if (A[0].dir == RIGHT) {
    for (int i = 0; i < 5; i++) { // [     >]
      A[i].colision.P1.x -= A[i].espacio;
      A[i].colision.P2.x -= A[i].espacio;
    }
  } 
  else { // LEFT
    for (int i = 0; i < 5; i++) { // [<      ]
      A[i].colision.P1.x += A[i].espacio;
      A[i].colision.P2.x = A[i].colision.P1.x+A[i].ancho;
    }
  }
}

void NoSalirDePantalla(){
  if(F1[0].colision.P1.x < (0-F1[0].ancho)){ //si el de la izquierda sale de pantalla por la dimension de su sprite
    RetrocederAutos(F1);
  }
  if(F2[4].colision.P1.x > ScreenX){  //si sale a la derecha
    RetrocederAutos(F2);
  }
  if(F3[0].colision.P1.x < (0-F3[0].ancho)){
    RetrocederAutos(F3);
  }
  if(F4[4].colision.P1.x > ScreenX){
    RetrocederAutos(F4);
  }
  if(F5[0].colision.P1.x <  (0-F5[0].ancho)){
    RetrocederAutos(F5);
  }
}

void DibujarVeiculos(){
  MoverObjeto(F1);
  MoverObjeto(F2);
  MoverObjeto(F3);
  MoverObjeto(F4);
  MoverObjeto(F5);
  for(int i = 0; i < 5; i++){
    esat::DrawSprite(F1[i].sprite.img, F1[0].colision.P1.x+(i*F1[i].espacio), F1[i].colision.P1.y); 
    esat::DrawSprite(F2[i].sprite.img, F2[0].colision.P1.x+(i*F2[i].espacio), F2[i].colision.P1.y);
    esat::DrawSprite(F3[i].sprite.img, F3[0].colision.P1.x+(i*F3[i].espacio), F3[i].colision.P1.y);
    esat::DrawSprite(F4[i].sprite.img, F4[0].colision.P1.x+(i*F4[i].espacio), F4[i].colision.P1.y);
    esat::DrawSprite(F5[i].sprite.img, F5[0].colision.P1.x+(i*F5[i].espacio), F5[i].colision.P1.y);
    DibujarRectanguloColision(F1[i].colision, 0, 255, 255);
    DibujarRectanguloColision(F2[i].colision, 0, 255, 255);
    DibujarRectanguloColision(F3[i].colision, 0, 255, 255);
    DibujarRectanguloColision(F4[i].colision, 0, 255, 255);
    DibujarRectanguloColision(F5[i].colision, 0, 255, 255);
  }
  NoSalirDePantalla();  //controla si el auto se sale de la pantalla
}

void DibujarFlotantes(){

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
    esat::DrawSprite(SpriteVidas, i*esat::SpriteWidth(SpriteVidas), ScreenY-48);
  }
  if(numPlayers > 1){
    for(int i = 0; i < Player2.vidas; i++){
      esat::DrawSprite(SpriteVidas, i*esat::SpriteWidth(SpriteVidas), (ScreenY-24)+esat::SpriteHeight(SpriteVidas));
    }
  }
}

void DibujarPie(){
  if(TipoPantalla == 1){
    DibujarVidas();
    //TO-DO DibujarNivel();
    //TO-DO TibujarTime();
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
  Player1.colision.P1.y = ScreenY-(SpritesHeight*2);
  Player1.sprite.img = esat::SubSprite(SpriteFrog, 0,0,48,48);
  Player1.colision.P2.x = Player1.colision.P1.x + esat::SpriteWidth(Player1.sprite.img);
  Player1.colision.P2.y = Player1.colision.P1.y + esat::SpriteHeight(Player1.sprite.img);
}

void InicializarAutos(){
  for(int i = 0; i < 5; i++) {
    F1[i].sprite.img = SpriteAuto1;
    F1[i].ancho = esat::SpriteWidth(F1[i].sprite.img);
    F1[i].dir = LEFT;
    F1[i].colision.P1.y = ScreenY-(SpritesHeight*3);
    F1[i].colision.P2.y = F1[i].colision.P1.y + SpritesHeight;

    F2[i].sprite.img = SpriteAuto2;
    F2[i].ancho = esat::SpriteWidth(F2[i].sprite.img);
    F2[i].dir = RIGHT;
    F2[i].colision.P1.y = ScreenY-(SpritesHeight*4);
    F2[i].colision.P2.y = F2[i].colision.P1.y + SpritesHeight;

    F3[i].sprite.img = SpriteAuto3;
    F3[i].ancho = esat::SpriteWidth(F3[i].sprite.img);
    F3[i].dir = LEFT;
    F3[i].colision.P1.y = ScreenY-(SpritesHeight*5);
    F3[i].colision.P2.y = F3[i].colision.P1.y + SpritesHeight;

    F4[i].sprite.img = SpriteAuto4;
    F4[i].ancho = esat::SpriteWidth(F4[i].sprite.img);
    F4[i].dir = RIGHT;
    F4[i].colision.P1.y = ScreenY-(SpritesHeight*6);
    F4[i].colision.P2.y = F4[i].colision.P1.y + SpritesHeight;

    F5[i].sprite.img = SpriteCamion;
    F5[i].ancho = esat::SpriteWidth(F5[i].sprite.img);
    F5[i].dir = LEFT;
    F5[i].colision.P1.y = ScreenY-(SpritesHeight*7);
    F5[i].colision.P2.y = F5[i].colision.P1.y + SpritesHeight;
  }
}

void InicializarFlotantes(){
  
}

void DibujarFondo(){
  esat::DrawClear(0,0,0);
  esat::DrawSetFillColor(0,4,74);
  esat::DrawSolidPath(Points,5);
}

void ResetPosicionRandom(){
  F1[0].colision.P1.x = rand()%(ScreenX);
  F2[0].colision.P1.x = rand()%(ScreenX);
  F3[0].colision.P1.x = rand()%(ScreenX);
  F4[0].colision.P1.x = rand()%(ScreenX);
  F5[0].colision.P1.x = rand()%(ScreenX);

  F1[0].colision.P2.x = F1[0].colision.P1.x + F1[0].ancho;
  F2[0].colision.P2.x = F2[0].colision.P1.x + F2[0].ancho;
  F3[0].colision.P2.x = F3[0].colision.P1.x + F3[0].ancho;
  F4[0].colision.P2.x = F4[0].colision.P1.x + F4[0].ancho;
  F5[0].colision.P2.x = F5[0].colision.P1.x + F5[0].ancho;
}

void ResetEspacio(Autos *A){
  int newSpacing;
  newSpacing = (A[1].ancho*3) + (rand() % (A[1].ancho*6)); //el +A*2 l inicio es para que no esten pegados + random de 0 a 3 veces el ancho de la imagen dando un maximo de espacio de 4 sprites entre autos
  for(int i = 0; i<5; i++){
    A[i].espacio = newSpacing;
  }
}

void ResetVelocidad(Autos *A){
  int newSpeed;
  newSpeed = 3 + rand() % 5; //minimo 3 maximo 8 (5+3=8)
  for(int i = 0; i<5; i++){
    A[i].velocidad = newSpeed;
  }
}

void CambiarValoresNivel(){
  if(Nivel >= LastNivel){

    ResetPosicionRandom();
    
    ResetEspacio(F1);
    ResetEspacio(F2);
    ResetEspacio(F3);
    ResetEspacio(F4);

    ResetVelocidad(F1);
    ResetVelocidad(F2);
    ResetVelocidad(F3);
    ResetVelocidad(F4);
    
    int esp = (F5[0].ancho*2) + (rand() % (F5[0].ancho*2));
    for(int i = 0; i<5; i++){
      F5[i].espacio = esp;
      F5[i].velocidad = 3;
      if(i>=1){
        F1[i].colision.P1.x = F1[i-1].colision.P1.x+F1[i].espacio;
        F2[i].colision.P1.x = F2[i-1].colision.P1.x+F2[i].espacio;
        F3[i].colision.P1.x = F3[i-1].colision.P1.x+F3[i].espacio;
        F4[i].colision.P1.x = F4[i-1].colision.P1.x+F4[i].espacio;
        F5[i].colision.P1.x = F5[i-1].colision.P1.x+F5[i].espacio;

        F1[i].colision.P2 = {F1[i].colision.P1.x+F1[i].ancho, F1[i].colision.P1.y + SpritesHeight};
        F2[i].colision.P2 = {F2[i].colision.P1.x+F2[i].ancho, F2[i].colision.P1.y + SpritesHeight};
        F3[i].colision.P2 = {F3[i].colision.P1.x+F3[i].ancho, F3[i].colision.P1.y + SpritesHeight};
        F4[i].colision.P2 = {F4[i].colision.P1.x+F4[i].ancho, F4[i].colision.P1.y + SpritesHeight};
        F5[i].colision.P2 = {F5[i].colision.P1.x+F5[i].ancho, F5[i].colision.P1.y + SpritesHeight};
      }
    }
    //parece que el F5 no cambia de velocidad
    LastNivel++;
  }
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
  printf("x: %d Y: %d \n\n", ScreenX, ScreenY);
  srand(time(NULL));
  esat::WindowInit(ScreenX, ScreenY);
  WindowSetMouseVisibility(true);

  LoadSprites();
  InicializarJugadores();
  InicializarAutos();
  InicializarFlotantes();
  //fuente
  esat::DrawSetTextFont("./Assets/font/arcade-legacy.ttf");
  esat::DrawSetTextSize(FontSize);
  
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)){
    last_time = esat::Time();
    esat::DrawBegin();
    CambiarValoresNivel();

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