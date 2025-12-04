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
  int arriba;
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
  int velocidad, espacio;
};

struct Tronco{
  Colision colision;
  int velocidad, no, espacio;  //no es numero objetos, ose la cantidad de sprites
};

struct Tortuga{
  Colision colision;
  Sprite sprite;
  int velocidad, no, espacio;
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
  int dir;
};

Frog Player1, Player2;
Colision fin[5];
Autos F1[5], F2[5], F3[5], F4[5], F5[5];
Tronco M1[5], M2[5], M3[5];
Tortuga T1[5], T2[5];

//Variables de sistema
const int FontSize = 20, SpritesHeight = 48;
const int ScreenX = 672, ScreenY = 768; //screen size
unsigned char fps=25; //Control de frames por segundo
double current_time, last_time;
float Points[10] = {0,0,ScreenX,0,ScreenX,ScreenY/2,0,ScreenY/2,0,0}; //zona azul
int TipoPantalla = 0, TipoMenu = 0, numPlayers = 1, highScore = 0, Nivel = 0, LastNivel = 0, creditos = 0, ScoreList[5] = {0};

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
  if(Player->arriba < Player->colision.P1.y){
    Player->score += 10;
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
  if(TipoMenu > 0 && esat::IsSpecialKeyDown(esat::kSpecialKey_Left))
    TipoMenu--;
  if(TipoMenu < 2 && esat::IsSpecialKeyDown(esat::kSpecialKey_Right))
    TipoMenu++;
  if(TipoMenu == 1 && esat::IsSpecialKeyDown(esat::kSpecialKey_Up) && numPlayers > 1)
    numPlayers--;
  if(TipoMenu == 1 && esat::IsSpecialKeyDown(esat::kSpecialKey_Down) && numPlayers < 2)
    numPlayers++;
  if(creditos < 99 && esat::IsSpecialKeyDown(esat::kSpecialKey_Keypad_1))
    creditos++;
  if(TipoMenu == 1 && creditos > 0 && esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
    TipoPantalla = 1;
    creditos--;
  }
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
********************DIJUBOS MENU************************
*******************************************************/

void DrawFrogger(){
  //TO-DO y tambien cambiar el sprite
}

void DubujarMenu(){
  //TO-DO animacion inicial de la ranas que FROGGER
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
      //TO-DO puntos de ScoreList
      esat::DrawText((ScreenX/2)-180, SpritesHeight*15, "KONAMI  a  1981");
    break;
  }
}

/*******************************************************
*****************DIJUBOS GAMEPLAY***********************
*******************************************************/

void DibujarJugador(){
  DibujarRectanguloColision(Player1.colision, 0, 255, 255);
  esat::DrawSprite(Player1.sprite.img, Player1.colision.P1.x, Player1.colision.P1.y);
  if(numPlayers > 1)
    esat::DrawSprite(Player2.sprite.img, Player2.colision.P1.x, Player2.colision.P1.y);
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
    if(fin[4].P1.x == 0){
      fin[i].P1.x = (Anchura*i) + 24;
      fin[i].P1.y = 96;
      fin[i].P2.x = fin[i].P1.x + 48;
      fin[i].P2.y = 144;
    }
    DibujarRectanguloColision(fin[i], 0, 255, 255);
    DibujarMeta(Anchura*i);
  }

  //pasto violeta
  for(int i = 0; i<14; i++){
    esat::DrawSprite(SpritePasto,i*esat::SpriteWidth(SpritePasto), ScreenY/2);
    esat::DrawSprite(SpritePasto,i*esat::SpriteWidth(SpritePasto), ScreenY-96);
  }
}

void MoverObjeto(Colision *coli, Direccion dire, int speed){
  if (dire == RIGHT) {
    coli->P1.x += speed;
    coli->P2.x += speed;
  } else { // LEFT
    coli->P1.x -= speed;
    coli->P2.x -= speed;
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
      A[i].colision.P2.x += A[i].espacio;
    }
  }
}

void NoSalirDePantalla(){
  if(F1[0].colision.P1.x < -48){ //si el de la izquierda sale de pantalla por la dimension de su sprite
    RetrocederAutos(F1);
  }
  if(F2[4].colision.P1.x > ScreenX){  //si sale a la derecha
    RetrocederAutos(F2);
  }
  if(F3[0].colision.P1.x < -48){
    RetrocederAutos(F3);
  }
  if(F4[4].colision.P1.x > ScreenX){
    RetrocederAutos(F4);
  }
  if(F5[0].colision.P1.x < -48){
    RetrocederAutos(F5);
  }
}

void DibujarVeiculos(){
  for(int i = 0; i < 5; i++){
    MoverObjeto(&F1[i].colision, F1[i].dir, F1[i].velocidad);
    MoverObjeto(&F2[i].colision, F2[i].dir, F2[i].velocidad);
    MoverObjeto(&F3[i].colision, F3[i].dir, F3[i].velocidad);
    MoverObjeto(&F4[i].colision, F4[i].dir, F4[i].velocidad);
    MoverObjeto(&F5[i].colision, F5[i].dir, F5[i].velocidad);

    esat::DrawSprite(F1[i].sprite.img, F1[i].colision.P1.x, F1[i].colision.P1.y); 
    esat::DrawSprite(F2[i].sprite.img, F2[i].colision.P1.x, F2[i].colision.P1.y);
    esat::DrawSprite(F3[i].sprite.img, F3[i].colision.P1.x, F3[i].colision.P1.y);
    esat::DrawSprite(F4[i].sprite.img, F4[i].colision.P1.x, F4[i].colision.P1.y);
    esat::DrawSprite(F5[i].sprite.img, F5[i].colision.P1.x, F5[i].colision.P1.y);
    
    DibujarRectanguloColision(F1[i].colision, 255, 0, 0);
    printf("-%f-%d-\n", F1[0].colision.P1.x, i);
    DibujarRectanguloColision(F2[i].colision, 255, 0, 0);
    DibujarRectanguloColision(F3[i].colision, 255, 0, 0);
    DibujarRectanguloColision(F4[i].colision, 255, 0, 0);
    DibujarRectanguloColision(F5[i].colision, 255, 0, 0);
  }
  NoSalirDePantalla();  //controla si el auto se sale de la pantalla
}

void DibujarFlotantes(){
  //TO-DO
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

void CalculoScorePlayer(int PTS){
  int NewScore = 0;
  bool cambio = false;
  for(int i = 0; i < 5; i++){
    if(cambio = false){
      if(PTS > ScoreList[i]){
        int NewScore = i;
        cambio = true;
      }
    }
    else{
      ScoreList[i+1] = ScoreList[i];
      if(i == NewScore){
        ScoreList[NewScore] = PTS;
      }
    }
  }
}

//esto c llama al game over
void GuardarScore() {
  CalculoScorePlayer(Player1.score);
  if(numPlayers>1)
    CalculoScorePlayer(Player2.score);
}

void DibujarPantalla(){
  //TO-DO if in game (juego) o menu (animacion)
  switch(TipoPantalla){
    case 0:
    DubujarMenu();
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

void DibujarCreditos(){
  //itoa de int a char
  char coins[2];
  itoa(creditos+100, coins, 10);

  esat::DrawSetFillColor(144,213,255);
  
  esat::DrawText(ScreenX-180, ScreenY-40, "CREDIT");
  esat::DrawText(ScreenX-50, ScreenY-40, coins+1);
}

void DibujarPie(){
  if(TipoPantalla == 1){
    DibujarVidas();
    //TO-DO DibujarNivel();
    //TO-DO TibujarTime();
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
    /*
    el clculo de SprtieHeight*n es solo para calcular la fila
    La fila 1 osea es SpriteHeight*1 es el footer, y 
    */
    F1[i].sprite.img = SpriteAuto1;
    F1[i].dir = LEFT;
    F1[i].colision.P1.y = ScreenY-(SpritesHeight*3);
    F1[i].colision.P2.y = F1[i].colision.P1.y + SpritesHeight;

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
    M1[i].no = 3;
    M1[i].colision.P1.y = ScreenY-(SpritesHeight*10);
    M1[i].colision.P2.y = M1[i].colision.P1.y+SpritesHeight;

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

void DibujarFondo(){
  esat::DrawClear(0,0,0);
  esat::DrawSetFillColor(0,4,74);
  esat::DrawSolidPath(Points,5);
}

Colision ResetPosicionRandom(){
  Colision coli;
  coli.P1.x = rand()%(ScreenX/2);
  coli.P2.x = coli.P1.x + 48;
  return coli;
}

int ResetEspacio(){
  int newSpacing;
  newSpacing = 96 + (rand() % (48*6)); //el +96 al inicio es para que no esten pegados + random de 0 a 3 veces el ancho de la imagen dando un maximo de espacio de 4 sprites entre autos

  return newSpacing;
}

void ResetVelocidad(int *speed){
  int newSpeed;
  newSpeed = Nivel + 3 + rand()%5; //minimo 3 maximo 8 (5+3=8) + 1 por cada nivel
  *speed = newSpeed;  
}

void ResetAutos(Autos *A, int vel){
  for(int i = 1; i<5; i++){
    A[i].velocidad=vel;
    A[i].espacio = A[0].espacio;
    A[i].colision.P1.x = A[i-1].colision.P1.x+F1[i].espacio;
    A[i].colision.P2 = {A[i].colision.P1.x+esat::SpriteWidth(A[i].sprite.img), A[i].colision.P1.y + SpritesHeight};
  }
}

void ResetFlotantes(){

}

//Que horror
//Lo odio
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
void CambiarValoresNivel(){
  if(Nivel >= LastNivel){
    int vF1, vF2, vF3, vF4, vM1, vM2, vM3, vT1, vT2, esp;
    ResetVelocidad(&vF1);
    ResetVelocidad(&vF2);
    ResetVelocidad(&vF3);
    ResetVelocidad(&vF4);
    ResetVelocidad(&vM1);
    ResetVelocidad(&vM2);
    ResetVelocidad(&vM3);
    ResetVelocidad(&vT1);
    ResetVelocidad(&vT2);

    //Cambio valores de la velocidad y el espacio de los objetos que se muvene,  y las coliciones tambien
    //Los objetos que c mueven son los autos, Los troncos y las tortugas
    F1[0].colision = ResetPosicionRandom();
    F2[0].colision = ResetPosicionRandom();
    F3[0].colision = ResetPosicionRandom();
    F4[0].colision = ResetPosicionRandom();
    F5[0].colision = ResetPosicionRandom();
    
    M1[0].colision=ResetPosicionRandom();
    M2[0].colision=ResetPosicionRandom();
    M3[0].colision=ResetPosicionRandom();

    T1[0].colision=ResetPosicionRandom();
    T2[0].colision=ResetPosicionRandom();

    F1[0].espacio=ResetEspacio();
    F2[0].espacio=ResetEspacio();
    F3[0].espacio=ResetEspacio();
    F4[0].espacio=ResetEspacio();

    M1[0].espacio=ResetEspacio();
    M2[0].espacio=ResetEspacio();
    M3[0].espacio=ResetEspacio();

    T1[0].espacio=ResetEspacio();
    T2[0].espacio=ResetEspacio();

    ResetAutos(F1, vF1);
    ResetAutos(F2, vF2);
    ResetAutos(F3, vF3);
    ResetAutos(F4, vF4);
    ResetAutos(F5, 3);

    // ResetFlotantes(M1, vM1);
    // ResetFlotantes(M2,vM2);
    // ResetFlotantes(M3, vM3);

    // ResetFlotantes(T1, vT1);
    // ResetFlotantes(T2, vT2);

    esp = (esat::SpriteWidth(F5[0].sprite.img)*2) + (rand() % (esat::SpriteWidth(F5[0].sprite.img)*2));
    for(int i = 0; i<5; i++){
      F5[i].espacio = esp;
    }
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

    DibujarFondo(); //dibuja el background

    DetectarInput();

    Display(); //Dibuja todos los objetos que se mueven

    esat::DrawEnd();
    ControlFPS();
    esat::WindowFrame();
  }
  ReleaseOfSprites();
  esat::WindowDestroy();
  return 0;
}