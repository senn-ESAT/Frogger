#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/time.h>
#include <esat/sprite.h>

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

/* Enums */
enum Direccion {
  ARRIBA,
  DERECHA,
  ABAJO,
  IZQUIERDA
};

enum Pantalla{
  INICIO_JUEGO,
  PETICION_CREDITOS,
  SELECCION_JUGADORES,
  MEDIADORA,
  JUEGO,
  GAME_OVER
};

/* STRUCTS */
struct PuntoCoord{
    float x = 0, y = 0;
};

struct Sprite{
    esat::SpriteHandle imagen;
    // tipoAnimacion -> El set de animaciones a usar (Fila en spritesheet);
    // indiceAnimacion -> Secuencia de la animación a usar (Columna en spritesheet);
    unsigned char tipoAnimacion = 0, indiceAnimacion = 0;
};

struct Colision{
    // P1 Tambien sirve como ubicación
    PuntoCoord P1;
    PuntoCoord P2;
};

struct Disparo{
    bool isDisparado = false;
    bool isJugador = false;
    Sprite sprite;
    Colision areaColision;
    Direccion direccion;
    unsigned char velocidad = 1;
};

struct Jugador{
    int puntuacion = 0;
    unsigned char vidas = 3;
    Sprite sprite;
    Direccion direccion = DERECHA;
    Colision areaColision;
    Disparo disparo;
    unsigned char velocidad = 2;
    bool isVisible = true;
};

struct Enemigo{
    Sprite sprite;
    Colision areaColision;
    Disparo disparo;
    int puntos = 0;
    bool isVisible = false;
};

struct Barrera{
  Colision areaColision;
  float barreraWidth, barreraHeight;
  unsigned char* barreraPixelData;
  unsigned char indice;
};

//Enemigo especial que sirve como bonus.
struct MysteryShip{
    bool isVisible = false;
    bool isMuriendo = false;
    int velocidad = 1; //Velocidad a la que se mueve.
    Sprite sprite;
    Direccion direccion;
    Colision areaColision;
    float tiempoAparicion = 25000.0f; //Aparece cada 25 segundos.
    float ultimaAparicion = 0;
    int puntuacion = 0;
    Colision impactadoEn; 
};
/* FIN STRUCTS */

/* GLOBALES */
// Constantes
//-- Tamaños ventanas
const int VENTANA_X = 588, VENTANA_Y = 720;
const int MARGEN_IZQ = 42, MARGEN_DER = VENTANA_X-MARGEN_IZQ;
const int MARGEN_SUP = 80, MARGEN_INF = VENTANA_Y-50;

//-- Fuentes
const unsigned char ALTURA_FUENTE = 19;

//-- FPS
const unsigned char FPS=25;

//-- UI

//-- Fin UI

//-- Enemigos
const int filasAliens=5, columnasAliens=11;
const int totalAliens = filasAliens*columnasAliens;

//-- Fin Enemigos
// -- Barrera
const int NUMERO_BARRERAS = 4;
// -- Fin Barrera
// Fin Constantes


// Variables
//-- Tiempos para FPS
double current_time,last_time;


//-- Declaración Temporizadores (&temp para función HacerCadaX)
//---- NOMENCLATURA VARIABLES TEMPORIZADOR -> temp_UsoTemporizador
//---- EJEMPLO 1: Temporizador para parpadeo de rotulo GAME OVER -> tempGameOver
//---- EJEMPLO 2: Temporizador para retraso en el movimiento de los enemigos -> tempDelayEnemigos

float 
// Declaración variables separado por comas excepto última
tempTextosInicioJuego = 0, 
tempTextosSeleccionJugadores = 0,
tempMovimientoEnemigos = 0,
tempDisparoEnemigos = 0,
tempAnimacionDisparoEnemigo = 0,
tempSpawnEnemigos = 0,
tempAnimacionJugador = 0,
tempPuntuacionEnemigoBonus = 0,
tempTextosGameOver = 0,
tempTextosMediadora = 0,
tempExplosionAlien = 0,
tempExplosionBalas = 0
;
//-- Fin Declaración Temporizadores (&temp para función HacerCadaX);


//-- UI
Pantalla pantallaActual = INICIO_JUEGO;

int opcionMenuPeticion = 1;

//-- Fin UI


//-- SpriteSheets
//Recursos/Imagenes/Sprites
esat::SpriteHandle Sheet_Enemigo1;
esat::SpriteHandle Sheet_Enemigo2;
esat::SpriteHandle Sheet_Enemigo3;
esat::SpriteHandle Sheet_Explosiones;
esat::SpriteHandle Sheet_Disparos;
esat::SpriteHandle DisparoPlayer;
esat::SpriteHandle SpriteJugador;
esat::SpriteHandle SpriteUFO;
esat::SpriteHandle SpriteExplosionUFO;
esat::SpriteHandle ExplosionJugador;
esat::SpriteHandle ExplosionedBordes;
esat::SpriteHandle basuras;

esat::SpriteHandle BarreraOnMemory[NUMERO_BARRERAS], SpriteObstaculo;
//-- Fin SpriteSheets

// -- Varaibles barreras
Barrera barrera[NUMERO_BARRERAS];
// -- Fin variables barreras

//-- Jugador
Jugador jugador_1;
Jugador jugador_2;
int highScore = 0;
unsigned char creditos = 1;
//-- Fin Jugador


//-- Enemigos
float inicio_x = 75, inicio_y = 350, separacion = 40, tiempo_ultimo_spawn = 0.0f, progreso_bajada = 0;
int indice_actual_mov = 0, indice_spawn_actual = 0, indiceAlienJugador = 0, velocidadEnemigo = 5, indiceMuriendo, aliensVivos = 0;
bool nivel_iniciando = true, bajando=false, muriendo = false, colisionBalas = false, colisionFromBarrera = false;
Enemigo aliens[totalAliens];
Direccion direccionEnemigos;
PuntoCoord impactosBalas;

//-- Fin Enemigos

//-- Enemigo bonus
MysteryShip enemigoBonus;
int numDisparos = 0;
//-- Fin enemigo bonus

// Fin Variables
/* FIN GLOBALES */

/* FUNCIONALIDADES */
// UTILS
void ControlFPS(){
    do{
        current_time = esat::Time();
    }while((current_time-last_time)<=1000.0/FPS);
}

//Pausa de los elementos necesarios del juego para la muerte del jugador y enemigos
void PausaMuerte(){
    velocidadEnemigo = 0;
}

//-- Randoms
    // Genera nueva semilla en base al 
    // ms del reloj del sistema
void GenerarSemillaAleatoria(){
    srand(time(NULL)); 
}
//Genera un número del 0 al límite indicado
int GenerarNumeroAleatorio(int limite){
    return (rand()%limite);
}
//-- Fin Randoms

//-- Arrays
int GetIndiceArray(int fila, int columnasTotales, int columna){
    return((fila*columnasTotales)+columna);
}

int GetFilaIndice(int indice, int columnasTotales){
    return(indice/columnasTotales);
}

int GetColumnaIndice(int indice, int columnasTotales){
    return(indice%columnasTotales);
}
//-- Fin Arrays

// Funcion booleana que devuelve true cada x segundos. 
// El parametro *temp debe ser una variable global que gestione las pausas de aquello 
// para lo que se utiliza el temporizador.
//
// AL TEMPORIZADOR QUE SE VA A USAR SE LE DEBE ASIGNAR
// EL VALOR esat::Time()/1000 UNA ÚNICA VEZ ANTES 
// DE EMPEZAR A HACER LA COMPROBACIÓN DE HacerCadaX()
//
// Ejemplo: Con la función DibujarInicioJuego(), la asignación del valor del 
// tiempo actual se hará una única vez, cada vez que se decida volver a dibujar la pantalla
// de inicio de juego (linea 614 + linea 659);
bool HacerCadaX(float *temp, float x){
    bool isAccesible = false;

    // Recupera el segundo actual de juego desde el punto de ejecución
    float tiempoActual = esat::Time()/1000.0f;

    // printf("LOG--- Valores funcion HacerCadaX()\n");
    // printf("LOG--- T.Actual    Temp    Contador    Limite\n");
    // printf("LOG--- %14.10f | %14.10f | %14.10f |%14.10f\n",tiempoActual,(*temp), tiempoActual - (*temp),x);

    isAccesible = tiempoActual - (*temp) > x;
    if(isAccesible){
        *temp = tiempoActual;
    }

    return isAccesible;
}

void Disparar(Disparo *disparo, Colision origen) {
    // Lógica del disparo
    float nuevaX, nuevaY;
    // medidas sprite balas enemigas
    int anchoSprite = 10, altoSprite = 18;
    if (disparo->isJugador) {
        disparo->direccion = ARRIBA;
        anchoSprite = esat::SpriteWidth(DisparoPlayer);
        altoSprite = esat::SpriteHeight(DisparoPlayer);
    } else {
        disparo->direccion = ABAJO;
    }
    
    nuevaX = origen.P1.x + ((origen.P2.x-origen.P1.x)*0.5);
    nuevaY = (*disparo).direccion == ARRIBA ? origen.P1.y : origen.P2.y;

    (*disparo).isDisparado = true;
    (*disparo).areaColision.P1 = {nuevaX, nuevaY};
    (*disparo).areaColision.P2 = {nuevaX + anchoSprite, nuevaY + altoSprite};
}

void SumarCreditos(int cantidad = 1){
    if (creditos<99){
        creditos = creditos+cantidad >= 99 ? 99 : creditos+cantidad;

    }
}


void DibujarIcono (int player) {
    if (player != 1) {
        opcionMenuPeticion = 2;
    } else {
        opcionMenuPeticion = 1;
    }
}

void DesplazarEnMenu (Direccion direccion) {
    if (direccion == ABAJO) {
        DibujarIcono(2);
    } else {
        DibujarIcono(1);
    }
}

// FIN UTILS

// Sprites

void Basura(){
    esat::SpriteRelease(basuras);
}

void CargarSpriteSheet(){ //literalmente carga las imagenes
    Sheet_Enemigo1 = esat::SpriteFromFile("./Recursos/Imagenes/Sprites/sheet_enemigo1.png");
    Sheet_Enemigo2 = esat::SpriteFromFile("./Recursos/Imagenes/Sprites/sheet_enemigo2.png");
    Sheet_Enemigo3 = esat::SpriteFromFile("./Recursos/Imagenes/Sprites/sheet_enemigo3.png");
    Sheet_Explosiones = esat::SpriteFromFile("./Recursos/Imagenes/Sprites/sheet_explosion.png");
    Sheet_Disparos = esat::SpriteFromFile("./Recursos/Imagenes/Sprites/sheet_disparos.png");
    DisparoPlayer = esat::SpriteFromFile("./Recursos/Imagenes/Sprites/DisparoPlayer.png");
    SpriteJugador = esat::SpriteFromFile("./Recursos/Imagenes/Sprites/player.png");
    ExplosionJugador = esat::SpriteFromFile("./Recursos/Imagenes/Sprites/ExplosionPlayer.png");
    SpriteUFO = esat::SpriteFromFile("./Recursos/Imagenes/Sprites/UFO.png");
    SpriteExplosionUFO = esat::SpriteFromFile("./Recursos/Imagenes/Sprites/ExplosionUFO.png");
    SpriteObstaculo = esat::SpriteFromFile("./Recursos/Imagenes/Sprites/obstaculo.png");
    ExplosionedBordes = esat::SpriteFromFile("./Recursos/Imagenes/Sprites/Explosiones_Bordes.png");
}

esat::SpriteHandle getDisparoAlien(Sprite AlienDisparo){
    return esat::SubSprite(Sheet_Disparos, (AlienDisparo.indiceAnimacion%4)*(esat::SpriteWidth(Sheet_Disparos)/4), AlienDisparo.tipoAnimacion*esat::SpriteHeight(Sheet_Disparos)/3, esat::SpriteWidth(Sheet_Disparos)/4, esat::SpriteHeight(Sheet_Disparos)/3);
}

//para las explosiones es igual solo que no hay autoincrement del inidice animacion. spriteSheet = SheeyExplociones y sprite es el sprite de
esat::SpriteHandle GetSpriteFrame(esat::SpriteHandle spriteSheet, Sprite sprite){
    //el /2 del width es porque por el momento todos los Sprite sheets son de 2 imagenes. Lo alto no importa en nuestro caso
    basuras = esat::SubSprite(spriteSheet,(sprite.indiceAnimacion%2)*(esat::SpriteWidth(spriteSheet)/2) ,0 ,esat::SpriteWidth(spriteSheet)/2, esat::SpriteHeight(spriteSheet));
    return basuras;
}

void AvanzarSpriteFrame(esat::SpriteHandle spriteSheet, Sprite *sprite){
    (*sprite).indiceAnimacion++;
    esat::SpriteRelease((*sprite).imagen);
    (*sprite).imagen = GetSpriteFrame(spriteSheet, *sprite);
    Basura();
}

void LiberarSprites(){
    esat::SpriteRelease(Sheet_Enemigo1);
    esat::SpriteRelease(Sheet_Enemigo2);
    esat::SpriteRelease(Sheet_Enemigo3);
    esat::SpriteRelease(Sheet_Explosiones);
    esat::SpriteRelease(Sheet_Disparos);
    esat::SpriteRelease(DisparoPlayer);
    esat::SpriteRelease(SpriteJugador);
    esat::SpriteRelease(ExplosionJugador);
    esat::SpriteRelease(SpriteUFO);
    esat::SpriteRelease(SpriteExplosionUFO);
    esat::SpriteRelease(SpriteObstaculo);
    esat::SpriteRelease(ExplosionedBordes);
    
    //TO_DO LIBERAR SPRITES ALIENS Y DISPAROS
    //TO_DO LIBERAR SPRITES JUGADORES Y DISPAROS JUGADOR
    //TO_DO LIBERAR SPRITES ENEMIGO BONUS
    //TO_DO LIBERAR SPRITES BARRERAS
}
// Fin Sprites

// Colisiones
// Funcion para detectar colision con el borde izquierdo
bool DetectarColisionBordeIzquierdo(Colision colision) {
    return colision.P1.x <= MARGEN_IZQ;
}
// Funcion para detectar colision con el borde derecho
bool DetectarColisionBordeDerecho(Colision colision) {
    return colision.P2.x >= MARGEN_DER;
}
// Funcion para detectar colision con el borde superior
bool DetectarColisionBordeSuperior(Colision colision) {
    return colision.P1.y <= MARGEN_SUP;
}
// Funcion para detectar colision con el borde inferior
bool DetectarColisionBordeInferior(Colision colision) {
    return colision.P2.y >= MARGEN_INF;
}
/*** Funciones para detectar colision con los bordes ***/
bool DetectarColisionBordes(Colision colision) {
    return (DetectarColisionBordeIzquierdo(colision)) ||
           (DetectarColisionBordeDerecho(colision)) ||
           (DetectarColisionBordeSuperior(colision)) ||
           (DetectarColisionBordeInferior(colision));
}

/*** Funcion para detectar colision entre dos objetos ***/
bool DetectarColision(Colision a, Colision b) {
  return (a.P2.x >= b.P1.x) &&
         (a.P1.x <= b.P2.x) &&
         (a.P2.y >= b.P1.y) &&
         (a.P1.y <= b.P2.y);
}
// Funcion para detectar colision en el lado izquierdo del objeto a con el objeto b
bool DetectarColisionIzquierda(Colision a, Colision b) {
  if (a.P2.x >= b.P1.x && a.P1.x < b.P1.x) {
    if (a.P2.y >= b.P1.y && a.P1.y <= b.P2.y) {
        return true;
    }
  }
  return false;
}
// Funcion para detectar colision en el lado derecho del objeto a con el objeto b
bool DetectarColisionDerecha(Colision a, Colision b) {
  if (a.P1.x <= b.P2.x && a.P2.x > b.P2.x) {
    if (a.P2.y >= b.P1.y && a.P1.y <= b.P2.y) {
        return true;
    }
  }
  return false;
}
// Funcion para detectar colision en el lado superior del objeto a con el objeto b
bool DetectarColisionSuperior(Colision a, Colision b) {
  if (a.P2.y >= b.P1.y && a.P1.y < b.P1.y) {
    if (a.P2.x >= b.P1.x && a.P1.x <= b.P2.x) {
        return true;
    }
  }
  return false;
}
// Funcion para detectar colision en el lado inferior del objeto a con el objeto b
bool DetectarColisionInferior(Colision a, Colision b) {
  if (a.P1.y <= b.P2.y && a.P2.y > b.P2.y) {
    if (a.P2.x >= b.P1.x && a.P1.x <= b.P2.x) {
        return true;
    }
  }
  return false;
}

// Funcion para detectar colision entre enemigo y bala del jugador

bool DetectarColisionEnemigoBala(Enemigo enemy, Disparo bullet) {
    if (bullet.isJugador && bullet.isDisparado) {
        return DetectarColision(enemy.areaColision, bullet.areaColision);
    }
    return false;
}

// Funcion para detectar colision entre enemigo especial y bala del jugador.
bool DetectarColisionMisteryShipBala(MysteryShip enemy, Disparo bullet) {
  if (bullet.isJugador && bullet.isDisparado) {
    return DetectarColision(enemy.areaColision, bullet.areaColision);
  }
  return false;
}

// // Funcion para detectar colision entre enemigo y jugador
bool DetectarColisionEnemigoJugador(Enemigo enemy, Jugador player) {
    return DetectarColision(enemy.areaColision, player.areaColision);
}

// // Funcion para detectar colision entre jugador y bala del enemigo
// bool DetectPlayerEnemyBulletCollision(Jugador player, Disparo bullet) {
//     if (!bullet.isJugador && bullet.isDisparado) {
//         return DetectarColision(player.areaColision, bullet.areaColision);
//     }
//     return false;
// }

void DestruirPixelesBarrera(int centroX, int centroY, int indiceBarrera){
    bool pixelDestruido = false;
    
    // Explosion principal
    int minXExplosion = centroX - 2;
    int maxXExplosion = centroX + 2;
    int minYExplosion = centroY - 12;
    int maxYExplosion = centroY + 12;
    
    // Destrucción extra
    int minXExtendido = centroX - 4;
    int maxXExtendido = centroX + 4;
    int minYExtendido = centroY - 20;
    int maxYExtendido = centroY + 20;
    
    // Loop para la explosione
    for (int i = minYExtendido; i <= maxYExtendido; i++) {
        for (int j = minXExtendido; j <= maxXExtendido; j++) {
            if (j >= 0 && j < barrera[indiceBarrera].barreraWidth && i >= 0 && i < barrera[indiceBarrera].barreraHeight) {
                int pixelIndex = (i * barrera[indiceBarrera].barreraWidth + j) * 4;

                // Revisamos si el pixel ya está destruido
                if (barrera[indiceBarrera].barreraPixelData[pixelIndex + 3] > 0) {
                    bool destruirPixel = false;
                    
                    bool dentroExplosionPrincipal = (j >= minXExplosion && j <= maxXExplosion) && (i >= minYExplosion && i <= maxYExplosion);
                    bool dentroExplosionExtendido = (j >= minXExtendido && j <= maxXExtendido) && (i >= minYExtendido && i <= maxYExtendido);
                    
                    // Explosion principal
                    if (dentroExplosionPrincipal) {
                        destruirPixel = true;
                    }
                    // Destrucción extra
                    else if (dentroExplosionExtendido) {
                        int randomValue = GenerarNumeroAleatorio(100);
                        if (randomValue < 30) {
                            destruirPixel = true;
                        }
                    }
                    
                    if (destruirPixel) {
                        barrera[indiceBarrera].barreraPixelData[pixelIndex] = 0;     // R
                        barrera[indiceBarrera].barreraPixelData[pixelIndex + 1] = 0; // G
                        barrera[indiceBarrera].barreraPixelData[pixelIndex + 2] = 0; // B
                        barrera[indiceBarrera].barreraPixelData[pixelIndex + 3] = 0; // A
                        pixelDestruido = true;
                    }
                }
            }
        }
    }

    if (pixelDestruido) {
        esat::SpriteUpdateFromMemory(BarreraOnMemory[indiceBarrera], barrera[indiceBarrera].barreraPixelData);
    }
}

void ColisionPixelPerfect(Barrera barrera, Disparo *disparo){
    if (disparo->isDisparado){
        unsigned char pixelColor[4];
        int barreraLocalX, barreraLocalY;
        bool esBalaJugador = disparo->isJugador || disparo->direccion == ARRIBA;
        int anchoBala = esBalaJugador ? 4 : 10;
        
        if (esBalaJugador) {
            barreraLocalY = disparo->areaColision.P1.y - barrera.areaColision.P1.y;
        } else {
            barreraLocalY = disparo->areaColision.P2.y - barrera.areaColision.P1.y;
        }
        for (int offsetX = 0; offsetX < anchoBala; offsetX++) {
            barreraLocalX = (disparo->areaColision.P1.x + offsetX) - barrera.areaColision.P1.x;
            
            if (barreraLocalX >= 0 && barreraLocalX < barrera.barreraWidth && barreraLocalY >= 0 && barreraLocalY < barrera.barreraHeight && disparo->isDisparado) {
                
                esat::SpriteGetPixel(BarreraOnMemory[barrera.indice], 
                    barreraLocalX, 
                    barreraLocalY, 
                    pixelColor
                );

                if (pixelColor[0] > 0 || pixelColor[1] > 0 || pixelColor[2] > 0 || pixelColor[3] > 0){
                    DestruirPixelesBarrera(barreraLocalX, barreraLocalY, barrera.indice);
                    // Destruir la bala
                    disparo->isDisparado = false;
                }
            }
        }
    }
}

// Funcion para destruir la barrera cuando los enemigos pasan por ella
void ColisionEnemigoBarrera(Enemigo enemigo, int numeroBarrera){
    // Calculamos la colicion 
    float overlapX1 = (enemigo.areaColision.P1.x > barrera[numeroBarrera].areaColision.P1.x) ? enemigo.areaColision.P1.x : barrera[numeroBarrera].areaColision.P1.x;
    float overlapY1 = (enemigo.areaColision.P1.y > barrera[numeroBarrera].areaColision.P1.y) ? enemigo.areaColision.P1.y : barrera[numeroBarrera].areaColision.P1.y;
    float overlapX2 = (enemigo.areaColision.P2.x < barrera[numeroBarrera].areaColision.P2.x) ? enemigo.areaColision.P2.x : barrera[numeroBarrera].areaColision.P2.x;
    float overlapY2 = (enemigo.areaColision.P2.y < barrera[numeroBarrera].areaColision.P2.y) ? enemigo.areaColision.P2.y : barrera[numeroBarrera].areaColision.P2.y;

    // Creamos una nueva colicion con los puntos
    Colision colisionLocal = {
        overlapX1 - barrera[numeroBarrera].areaColision.P1.x,
        overlapY1 - barrera[numeroBarrera].areaColision.P1.y,
        overlapX2 - barrera[numeroBarrera].areaColision.P1.x,
        overlapY2 - barrera[numeroBarrera].areaColision.P1.y
    };
    
    if (colisionLocal.P1.x < 0) colisionLocal.P1.x = 0;
    if (colisionLocal.P1.y < 0) colisionLocal.P1.y = 0;
    if (colisionLocal.P2.x > barrera[numeroBarrera].barreraWidth) colisionLocal.P2.x = barrera[numeroBarrera].barreraWidth;
    if (colisionLocal.P2.y > barrera[numeroBarrera].barreraHeight) colisionLocal.P2.y = barrera[numeroBarrera].barreraHeight;

    bool pixelDestruido = false;

    // Destruimos esta area
    for (int i = colisionLocal.P1.y; i < colisionLocal.P2.y; i++) {
        for (int j = colisionLocal.P1.x; j < colisionLocal.P2.x; j++) {
            int pixelIndex = (i * barrera[numeroBarrera].barreraWidth + j) * 4;
            
            if (barrera[numeroBarrera].barreraPixelData[pixelIndex + 3] > 0) {
                barrera[numeroBarrera].barreraPixelData[pixelIndex] = 0;     // R
                barrera[numeroBarrera].barreraPixelData[pixelIndex + 1] = 0; // G
                barrera[numeroBarrera].barreraPixelData[pixelIndex + 2] = 0; // B
                barrera[numeroBarrera].barreraPixelData[pixelIndex + 3] = 0; // A
                pixelDestruido = true;
            }
        }
    }

    if (pixelDestruido) {
        esat::SpriteUpdateFromMemory(BarreraOnMemory[numeroBarrera], barrera[numeroBarrera].barreraPixelData);
    }
}

// Mueve colisión pasada por parametro. El P1 de la colisión indica también la ubicación
void MoverColision(Colision *colision, Direccion dir, int desplazamiento){
    switch(dir){
        case ARRIBA:
            (*colision).P1.y-=desplazamiento;
            (*colision).P2.y-=desplazamiento;
        break;
        case DERECHA:
            (*colision).P1.x+=desplazamiento;
            (*colision).P2.x+=desplazamiento;
        break;
        case ABAJO:
            (*colision).P1.y+=desplazamiento;
            (*colision).P2.y+=desplazamiento;
        break;
        case IZQUIERDA:
            (*colision).P1.x-=desplazamiento;
            (*colision).P2.x-=desplazamiento;
        break;
    }
}

// For Debug
void DibujarRectanguloColision(Colision colision, unsigned char r = 255, unsigned char g = 0, unsigned char b = 0) {
  esat::DrawSetStrokeColor(r, g, b);
  esat::DrawLine(colision.P1.x, colision.P1.y, colision.P2.x, colision.P1.y);
  esat::DrawLine(colision.P1.x, colision.P2.y, colision.P2.x, colision.P2.y);
  esat::DrawLine(colision.P1.x, colision.P1.y, colision.P1.x, colision.P2.y);
  esat::DrawLine(colision.P2.x, colision.P1.y, colision.P2.x, colision.P2.y);
}
void DibujarTodosLosColiders() {
  DibujarRectanguloColision(jugador_1.areaColision, 0, 255, 0);
  
  if (jugador_1.disparo.isDisparado) {
      DibujarRectanguloColision(jugador_1.disparo.areaColision, 0, 255, 255);
  }
  
  for (int i = 0; i < totalAliens; i++) {
      if (aliens[i].isVisible) {
          DibujarRectanguloColision(aliens[i].areaColision, 255, 0, 0);
      }
        if (aliens[i].disparo.isDisparado) {
                DibujarRectanguloColision(aliens[i].disparo.areaColision, 255, 255, 0);
        }
  }

  for (int i = 0; i < NUMERO_BARRERAS; i++){
    DibujarRectanguloColision(barrera[i].areaColision, 0, 0, 255);
  }
}
// FIN COLISIONES

// Enemigos

void CargarEnemigo(Enemigo &E, float x, float y){

    float w = (float)esat::SpriteWidth(E.sprite.imagen);
    float h = (float)esat::SpriteHeight(E.sprite.imagen);

    E.areaColision.P1.x = x;
    E.areaColision.P1.y = y;

    E.areaColision.P2.x = x + w;
    E.areaColision.P2.y = y + h;
}

void CambioDireccionEnemigos(){
    //direccionEnemigos = direccionEnemigos == DERECHA ? IZQUIERDA : DERECHA;
    
    //IFs SEPARADOS PARA EVITAR ELSE -> Direccion = ABAJO
    if(direccionEnemigos == DERECHA){
        direccionEnemigos = IZQUIERDA;
    }else if (direccionEnemigos == IZQUIERDA){
        direccionEnemigos = DERECHA;
    }
    
    bajando=true;
}

void MoverEnemigo(int velocidad){
    if(indice_actual_mov < totalAliens){
        if(indice_actual_mov <= 21){
            //AvanzarSpriteFrame(Sheet_Enemigo3,&aliens[indice_actual_mov].sprite);
        }else{ 
            if(indice_actual_mov > 21 && indice_actual_mov <= 43){  
                //AvanzarSpriteFrame(Sheet_Enemigo2,&aliens[indice_actual_mov].sprite);
            }else{
                //AvanzarSpriteFrame(Sheet_Enemigo1,&aliens[indice_actual_mov].sprite);
            }
        }

        if (direccionEnemigos == IZQUIERDA) {
            MoverColision(&aliens[indice_actual_mov].areaColision, IZQUIERDA, velocidad);
        }else{
            MoverColision(&aliens[indice_actual_mov].areaColision, DERECHA, velocidad);
        }

        if(bajando){
            MoverColision(&aliens[indice_actual_mov].areaColision, ABAJO, esat::SpriteHeight(aliens[indice_actual_mov].sprite.imagen));
        }
        indice_actual_mov++;
    }else{
        indice_actual_mov = 0;
        bajando=false;
    }
}

void SpawnAliensSecuencialmente(){

    // Espera un poco entre cada aparición
    if (HacerCadaX(&tempSpawnEnemigos, 0.05f) && indice_spawn_actual < totalAliens) {

        if(indice_spawn_actual <= 21){
            esat::SpriteRelease(aliens[indice_spawn_actual].sprite.imagen);
            aliens[indice_spawn_actual].sprite.imagen = GetSpriteFrame(Sheet_Enemigo3,aliens[indice_spawn_actual].sprite);
            Basura();
            aliens[indice_spawn_actual].puntos = 10;
        }else if(indice_spawn_actual > 21 && indice_spawn_actual <= 43){
            esat::SpriteRelease(aliens[indice_spawn_actual].sprite.imagen);
            aliens[indice_spawn_actual].sprite.imagen = GetSpriteFrame(Sheet_Enemigo2,aliens[indice_spawn_actual].sprite);
            Basura();
            aliens[indice_spawn_actual].puntos = 20;
        }else{
            esat::SpriteRelease(aliens[indice_spawn_actual].sprite.imagen);
            aliens[indice_spawn_actual].sprite.imagen = GetSpriteFrame(Sheet_Enemigo1,aliens[indice_spawn_actual].sprite);
            Basura();
            aliens[indice_spawn_actual].puntos = 30;
        }
        aliens[indice_spawn_actual].isVisible = true;
        
        float w = esat::SpriteWidth(aliens[indice_spawn_actual].sprite.imagen);
        float h = esat::SpriteHeight(aliens[indice_spawn_actual].sprite.imagen);
        aliens[indice_spawn_actual].areaColision.P2.x = aliens[indice_spawn_actual].areaColision.P1.x + w;
        aliens[indice_spawn_actual].areaColision.P2.y = aliens[indice_spawn_actual].areaColision.P1.y + h;
        aliens[indice_spawn_actual].disparo.sprite.tipoAnimacion = rand() %3;

        indice_spawn_actual++;
    }

    // Cuando todos están cargados, empezamos el movimiento
    if (indice_spawn_actual >= totalAliens){
        nivel_iniciando = false;
        enemigoBonus.ultimaAparicion = esat::Time();
    }
}

void CargaParteEnemiga(){
    aliensVivos = 0;
    for (int f = 0; f < filasAliens; f++){
        for (int c = 0; c < columnasAliens; c++){
            int i = GetIndiceArray(f,columnasAliens,c);
            float x = inicio_x + c * separacion;
            float y = inicio_y - f * separacion;

            // Guardamos posición, pero no cargamos sprite todavía
            aliens[i].areaColision.P1.x = x;
            aliens[i].areaColision.P1.y = y;
            aliens[i].isVisible = false;
            aliens[i].disparo.isDisparado = false;
            aliens[i].sprite.imagen = GetSpriteFrame(Sheet_Enemigo1,aliens[i].sprite);
            Basura();
            aliensVivos++;
        }
    }

    indice_spawn_actual = 0;
    direccionEnemigos = DERECHA;
    progreso_bajada = 0;
    indice_actual_mov = 0;
    
    nivel_iniciando = true;
    bajando=false;
    tempMovimientoEnemigos = esat::Time();
}


bool IsDisparoEnemigoPosible(int fila, int columna){
    if((fila == 0 || !aliens[GetIndiceArray(fila-1,columnasAliens,columna)].isVisible) && !aliens[GetIndiceArray(fila,columnasAliens,columna)].disparo.isDisparado){
        return true;
    }else{
        return false;
    }
}

void ExplotarEnemigo(){

    velocidadEnemigo = 0;
    aliens[indiceMuriendo].sprite.tipoAnimacion = 0; 
    aliens[indiceMuriendo].sprite.indiceAnimacion = 0; 
    int c = ((esat::Time()/1000.0f) - tempExplosionAlien)*10;

    if(HacerCadaX(&tempExplosionAlien, 0.5f) && muriendo){
        velocidadEnemigo=5;
        muriendo = false;
        aliensVivos--;
    }else{
        esat::DrawSprite(GetSpriteFrame(Sheet_Explosiones,aliens[indiceMuriendo].sprite),  aliens[indiceMuriendo].areaColision.P1.x, aliens[indiceMuriendo].areaColision.P1.y);
        Basura();
    }    
}

// Enemigo bonus
//Función para crear un enemigo especial, incluye valores por defecto para crear el genérico fácilmente. 
MysteryShip GenerarMisteryShip(
    bool isVisible = false, 
    bool isMuriendo = false,
    int velocidad = 1, 
    Sprite sprite = {SpriteUFO}, 
    Direccion direccion = IZQUIERDA, 
    Colision colision = {{(MARGEN_DER-48), (MARGEN_SUP+21)},{(MARGEN_DER), (MARGEN_SUP+42)}}
){
    MysteryShip mysteryShip = {
    isVisible,
    isMuriendo,
    velocidad,
    sprite,
    direccion,
    colision,
    };
    return mysteryShip;
}

//Función para inicializar el enemigo bonus por defecto.
void inicializarEnemigoBonus(){
    printf("inicializarEnemigo\n");
    enemigoBonus = GenerarMisteryShip();
    enemigoBonus.ultimaAparicion = esat::Time();
}

// Barreras

void GenerarBarreras(){
    float next = 118;
    for (int i = 0; i < NUMERO_BARRERAS; i++){
        barrera[i].barreraWidth = esat::SpriteWidth(SpriteObstaculo);
        barrera[i].barreraHeight = esat::SpriteHeight(SpriteObstaculo);
        barrera[i].areaColision.P1.x = next;
        barrera[i].areaColision.P1.y = 550;
        barrera[i].areaColision.P2.x = next + barrera[i].barreraWidth;
        barrera[i].areaColision.P2.y = 550 + barrera[i].barreraHeight;
        barrera[i].indice = i;
        next += 100;
    }
    
    for (int i = 0; i < NUMERO_BARRERAS; i++){
        // Pixel data en memoria (x4 por RGBA)
        int pixelDataSize =  barrera[i].barreraWidth *  barrera[i].barreraHeight * 4;

        // Reservamos memoria
        barrera[i].barreraPixelData = (unsigned char*)malloc(pixelDataSize);
        if (barrera[i].barreraPixelData == NULL) {
            printf("Error cargar imagen en momoria :(\n");
        }

        // Copiar los pixeles de sprite en memoria
        unsigned char color[4];
        for (int y = 0; y < barrera[i].barreraHeight; y++) {
            for (int x = 0; x < barrera[i].barreraWidth; x++) {
                esat::SpriteGetPixel(SpriteObstaculo, x, y, color);
                int pixelIndex = (y * barrera[i].barreraWidth + x) * 4;
                // Cambiar color a verde
                barrera[i].barreraPixelData[pixelIndex] = 0;            // R
                barrera[i].barreraPixelData[pixelIndex + 1] = color[1]; // G 
                barrera[i].barreraPixelData[pixelIndex + 2] = 0;        // B
                barrera[i].barreraPixelData[pixelIndex + 3] = color[3]; // A
            }
        }
    }

    // Crear el sprite a partir de memoria
    for (int i = 0; i < NUMERO_BARRERAS; i++){
        BarreraOnMemory[i] = esat::SpriteFromMemory( barrera[i].barreraWidth,  barrera[i].barreraHeight,  barrera[i].barreraPixelData);
    }
}

void ExplotarJugador(){
    //Recupera el contador actual de tempAnimacionJugador en decimas de segundo
    int c = ((esat::Time()/1000.0f) - tempAnimacionJugador)*10;

    if(HacerCadaX(&tempAnimacionJugador, 4)){
        if(jugador_1.vidas > 0 && DetectarColision(aliens[indiceAlienJugador].areaColision,jugador_1.areaColision)){
            GenerarBarreras();
            CargaParteEnemiga();
            inicializarEnemigoBonus();
            jugador_1.sprite.imagen = SpriteJugador;
            jugador_1.isVisible = true;
        }else{
            jugador_1.isVisible = true;
            velocidadEnemigo = 5;
        }
            
    }else{
        if(c <= 20){
            esat::DrawSprite(
                esat::SubSprite(
                    ExplosionJugador, 
                    (c%2)*(esat::SpriteWidth(ExplosionJugador)/2), 
                    0, 
                    esat::SpriteWidth(ExplosionJugador)/2, 
                    esat::SpriteHeight(ExplosionJugador)
                ), 
            jugador_1.areaColision.P1.x, jugador_1.areaColision.P1.y);
        }
    }
}

void DibujarColisionBorde(){
    int y = 0;
    if(impactosBalas.y < 200){
        y = 1;
    }
    esat::DrawSprite(
        esat::SubSprite(ExplosionedBordes,
        y*esat::SpriteWidth(ExplosionedBordes)/2,
        0,
        esat::SpriteWidth(ExplosionedBordes)/2,
        esat::SpriteHeight(ExplosionedBordes)),
    impactosBalas.x-10, impactosBalas.y-(y*22));
}

void DibujarColisionDisparo() {
    if(colisionBalas){
        if(HacerCadaX(&tempExplosionBalas, 0.5f)){
            colisionBalas = false;
        }
        else{
            if(!colisionFromBarrera){
                esat::DrawSprite(
                    esat::SubSprite(Sheet_Explosiones,
                    esat::SpriteWidth(Sheet_Explosiones)/2,
                    0,
                    esat::SpriteWidth(Sheet_Explosiones)/2,
                    esat::SpriteHeight(Sheet_Explosiones)),
                impactosBalas.x-15, impactosBalas.y-10);
            }
            else{
                DibujarColisionBorde();
            }
        }
    }
}

//Función para comprobar si se debe mostrar el enemigo bonus.
bool seDebeMostrarEnemigoBonus(MysteryShip *enemigoBonus){
    //Si el flag para mostrarse está en el estado correcto devuelve directamente true.
    if((*enemigoBonus).isVisible == true){
        return true;
    }

    float ahora = esat::Time();
    //Si no comprueba si se puede cambiar de estado el flag.
    if(ahora-(*enemigoBonus).ultimaAparicion >= (*enemigoBonus).tiempoAparicion && !(*enemigoBonus).isVisible){
        //Si el tiempo de generación es 0 y no se está mostrando, cambiar flag de estado.
        (*enemigoBonus).isVisible = true;
        (*enemigoBonus).ultimaAparicion = ahora;
        return true;
    }

    //Si no se cumple ninguna de las condiciones, no se mostrará.
    return false;
}

//Función para resetear enemigo bonus.
void resetEnemigoBonus(MysteryShip *enemigoBonus,  bool isVisible, Direccion direccion){
    (*enemigoBonus).isVisible = isVisible;
    (*enemigoBonus).direccion = direccion;
}

//Función para mover enemigo bonus.
void moverEnemigoBonus(MysteryShip *enemigoBonus) {
    MoverColision(&(*enemigoBonus).areaColision, (*enemigoBonus).direccion, (*enemigoBonus).velocidad);
    if(DetectarColisionBordeIzquierdo((*enemigoBonus).areaColision)){
        (*enemigoBonus).areaColision.P1.x = MARGEN_IZQ;
        (*enemigoBonus).areaColision.P2.x = MARGEN_IZQ+esat::SpriteWidth(SpriteUFO);
        (*enemigoBonus).areaColision.P2.y = (*enemigoBonus).areaColision.P1.y+21;
        resetEnemigoBonus(&(*enemigoBonus), false, DERECHA);
    }
    if(DetectarColisionBordeDerecho((*enemigoBonus).areaColision)){
        (*enemigoBonus).areaColision.P1.x = MARGEN_DER-esat::SpriteWidth(SpriteUFO);
        (*enemigoBonus).areaColision.P2.x = MARGEN_DER;
        (*enemigoBonus).areaColision.P2.y = (*enemigoBonus).areaColision.P1.y+21;
        resetEnemigoBonus(&(*enemigoBonus), false, IZQUIERDA);
    }
}

//Función para imprimir un enemigo bonus en pantalla.
void mostrarEnemigoBonus(MysteryShip *enemigoBonus){
    //Comprueba que el flag sea el correcto para mostrarse.
    if((*enemigoBonus).isVisible){
        esat::DrawSprite((*enemigoBonus).sprite.imagen, (*enemigoBonus).areaColision.P1.x, (*enemigoBonus).areaColision.P1.y);   
        moverEnemigoBonus(enemigoBonus);    
    }
}

//Función para comprobar si una bala ha impactado en el enemigo bonus.
bool enemigoBonusEsDado(Disparo disparo){
    bool impactado = false;
    enemigoBonus.impactadoEn.P1.x = enemigoBonus.areaColision.P1.x;
    enemigoBonus.impactadoEn.P1.y = enemigoBonus.areaColision.P1.y;
    if(enemigoBonus.isVisible && DetectarColisionMisteryShipBala(enemigoBonus, disparo)){
        impactado = true;
        enemigoBonus.isMuriendo = true;
        tempPuntuacionEnemigoBonus = esat::Time()/1000.0f;
        if(enemigoBonus.direccion == DERECHA){
            enemigoBonus.areaColision.P1.x = MARGEN_DER-esat::SpriteWidth(enemigoBonus.sprite.imagen);
            enemigoBonus.areaColision.P2.x = MARGEN_DER;
            resetEnemigoBonus(&enemigoBonus, false, IZQUIERDA);
        }else{
            enemigoBonus.areaColision.P1.x = MARGEN_IZQ;
            enemigoBonus.areaColision.P2.x = MARGEN_IZQ+esat::SpriteWidth(enemigoBonus.sprite.imagen);
            resetEnemigoBonus(&enemigoBonus, false, DERECHA);
        }
    }
    return impactado;
}

//Función para sumar la puntuación del enemigo bonus.
int obtenerPuntuacionEnemigoBonus(int numDisparos){
//La puntuación del enemigo bonus se obtiene mediante el número de disparos.
    //Si es múltiplo de 15 devuelve la máxima puntuación (300).
    if(numDisparos % 15 == 0){
        return 300;
    }else if(numDisparos % 15 == 1){
        //Si el múltiplo de 15 devuelve de resto 1 la puntuación será de 100.
        return 100;
    }else if(numDisparos % 15 == 2){
        //Si el múltiplo de 15 devuelve de resto 2 la puntuación será de 150. (Esto imita al arcade japonés).
        return 150;
    }else{
        //Por defecto la puntuación será de 50.
        return 50;
    }
}

void ExplotarEnemigoBonus(){
    int c = (esat::Time()/1000.0f) - tempPuntuacionEnemigoBonus;
    printf("Contador enemigo bonus %d \n", c);
    if(HacerCadaX(&tempPuntuacionEnemigoBonus, 2)){
        enemigoBonus.sprite = {SpriteUFO};
        enemigoBonus.isMuriendo = false;  
        enemigoBonus.ultimaAparicion = esat::Time();
    }else{
        if(enemigoBonus.isMuriendo){
            if(c >= 1){
                char puntuacionEnemigoBonus[3];
                 esat::DrawSetFillColor(255,0,0);
                 esat::DrawText(
                     enemigoBonus.impactadoEn.P1.x,
                     (enemigoBonus.areaColision.P1.y+10.5), 
                     itoa(enemigoBonus.puntuacion, puntuacionEnemigoBonus, 10)
                );
                 esat::DrawSetFillColor(255,255,255);
            }else{
                esat::DrawSprite(
                    SpriteExplosionUFO,
                    enemigoBonus.impactadoEn.P1.x, 
                    enemigoBonus.impactadoEn.P1.y
                );
            }
        }
    }
}

//Función para dibujar el enemigo bonus en pantalla.
void DibujarEnemigoBonus(){
    if(seDebeMostrarEnemigoBonus(&enemigoBonus)){
        mostrarEnemigoBonus(&enemigoBonus);
    }
}

// Fin enemigo bonus

// Fin Enemigos
// Jugador
void InicializarJugador() {
    //Instanciación de disparo por defecto de jugadores
    Disparo disparoDefault;
    disparoDefault.isDisparado = false;
    disparoDefault.isJugador = true;
    disparoDefault.sprite = {DisparoPlayer,0,0};
    disparoDefault.areaColision = {{0,0},{0,0}};
    disparoDefault.direccion = ARRIBA;
    disparoDefault.velocidad = 5;
    
    //Calculo y asignación de posición jugador (J1 en modo 1 jugador)
    float areaColisionP1X = VENTANA_X / 2;
    float areaColisionP2Y = MARGEN_INF - (esat::SpriteHeight(SpriteJugador) * 2);
    float areaColisionP1Y = areaColisionP2Y - (esat::SpriteHeight(SpriteJugador));
    float areaColisionP2X = (VENTANA_X / 2) + esat::SpriteWidth(SpriteJugador);

    jugador_1.sprite = {SpriteJugador,0,0};
    jugador_1.areaColision = {{areaColisionP1X,areaColisionP1Y},{areaColisionP2X,areaColisionP2Y}};
    jugador_1.disparo = disparoDefault;
    jugador_1.vidas = 3;


    //Calculo y asignación de posición jugador (J2, ACTUALMENTE SOLO NECESARIO PARA LA INTERFAZ)
    jugador_2.sprite = {SpriteJugador, 0, 0};
    jugador_2.areaColision = {{areaColisionP1X, areaColisionP1Y}, {areaColisionP2X , areaColisionP2Y}};
    jugador_2.disparo = disparoDefault;
}

void MoverBalas() {
    int c = tempAnimacionDisparoEnemigo;
    if (jugador_1.disparo.isDisparado) {

        jugador_1.disparo.areaColision.P1.y -= jugador_1.disparo.velocidad;
        jugador_1.disparo.areaColision.P2.y -= jugador_1.disparo.velocidad;

        esat::DrawSprite(
            DisparoPlayer,
            jugador_1.disparo.areaColision.P1.x, 
            jugador_1.disparo.areaColision.P1.y
        );
    }
    
    for (int i = 0; i < totalAliens; ++i) {
        if (aliens[i].disparo.isDisparado) {
            aliens[i].disparo.areaColision.P1.y += aliens[i].disparo.velocidad;
            aliens[i].disparo.areaColision.P2.y += aliens[i].disparo.velocidad;
            
            esat::DrawSprite(
                getDisparoAlien(aliens[i].disparo.sprite),
                aliens[i].disparo.areaColision.P1.x,
                aliens[i].disparo.areaColision.P1.y
            );

            if(HacerCadaX(&tempAnimacionDisparoEnemigo,0.25f)){
                aliens[i].disparo.sprite.indiceAnimacion++;
            }
        }
    }
}

void MoverJugador(Jugador *jugador) {
    MoverColision(&(*jugador).areaColision, (*jugador).direccion, (*jugador).velocidad);
    if(DetectarColisionBordeIzquierdo((*jugador).areaColision)){
        (*jugador).areaColision.P1.x = MARGEN_IZQ;
        (*jugador).areaColision.P2.x = MARGEN_IZQ+esat::SpriteWidth((*jugador).sprite.imagen);
    }
    if(DetectarColisionBordeDerecho((*jugador).areaColision)){
        (*jugador).areaColision.P1.x = MARGEN_DER-esat::SpriteWidth((*jugador).sprite.imagen);
        (*jugador).areaColision.P2.x = MARGEN_DER;
    }
}

void DibujarJugador() {
    if (jugador_1.isVisible) {
        esat::DrawSprite(SpriteJugador, jugador_1.areaColision.P1.x, jugador_1.areaColision.P1.y);
    }
}
// Fin jugador

//REINICIO JUEGO
void ReinicioJuego(){
    GenerarBarreras();
    CargaParteEnemiga();
    InicializarJugador();
    inicializarEnemigoBonus();
}

void DibujarBarreras(){
    for (int i = 0; i < NUMERO_BARRERAS; i++){
        esat::DrawSprite(BarreraOnMemory[i],
            barrera[i].areaColision.P1.x,
            barrera[i].areaColision.P1.y
        );
    }
}

void DibujarLineaAbajo(){
    // TO_DO printear cuando se inicie el juego solo
    // @jhony
    esat::DrawSetStrokeColor(0,255,0);
    esat::DrawLine(MARGEN_IZQ,MARGEN_INF,MARGEN_DER,MARGEN_INF);
    // BARRERAS DE ZONA DE GAMEPLAY | TEMPORAL A ELIMINAR
}

// Fin Barreras

// UI
void DibujarCabecera(){
    char puntuacionJ1[5], puntuacionJ2[5], highScoreChars[5];
    // Contador de tiempo dividido entre 100 para recuperar las decimas de segundo
    // De tal manera que en 3 segundos, la animación del parpadeo se pueda ejecutar 30 veces
    // 15 veces oculto y 15 mostrando el texto
    int c = ((esat::Time()/100.0f) - tempTextosSeleccionJugadores);

    itoa(jugador_1.puntuacion + 10000, puntuacionJ1, 10);
    itoa(jugador_2.puntuacion + 10000, puntuacionJ2, 10);
    itoa(highScore + 10000, highScoreChars, 10);
    
    esat::DrawText(MARGEN_IZQ+6,ALTURA_FUENTE+10,"SCORE<1> HI-SCORE SCORE<2>");

    if (pantallaActual == SELECCION_JUGADORES) {
        if ( c % 2 == 0) {  
            esat::DrawText(MARGEN_IZQ+6,(ALTURA_FUENTE+10)*2, puntuacionJ1+1);
        }
    } else {
        esat::DrawText(MARGEN_IZQ+6,(ALTURA_FUENTE+10)*2, puntuacionJ1+1);
    }
    esat::DrawText((VENTANA_X*0.5)-ALTURA_FUENTE*2,(ALTURA_FUENTE+10)*2, highScoreChars+1);
    esat::DrawText((MARGEN_DER-6)-(ALTURA_FUENTE*4),(ALTURA_FUENTE+10)*2,puntuacionJ2+1);
}

void GuardarHighScore() {
    //Comprueba solo el jugador 1 al no tener modo 2 jugadores implementado
    if (jugador_1.puntuacion > highScore) {
        highScore = jugador_1.puntuacion;
    }
}

void RestarVida(){
    jugador_1.vidas--;
    if(jugador_1.vidas <= 0){
        tempTextosGameOver = esat::Time()/1000;
    }
}

void DibujarEnemigos(){
    int indiceEnemigosDisparo[columnasAliens];
    int columnasDisparables[columnasAliens];
    int totalColumnasDisparables = 0;
    int enemigoDisparador;

    if (nivel_iniciando) {
        SpawnAliensSecuencialmente();
        //Imprime por pantalla los enemigos visibles
        for (int j = 0; j < columnasAliens; j++) {
            for(int f = 0; f < filasAliens; f++){
                if(aliens[GetIndiceArray(f,columnasAliens,j)].isVisible){
                    esat::DrawSprite(
                        aliens[GetIndiceArray(f,columnasAliens,j)].sprite.imagen,
                        aliens[GetIndiceArray(f,columnasAliens,j)].areaColision.P1.x,
                        aliens[GetIndiceArray(f,columnasAliens,j)].areaColision.P1.y);
                }
            }
            indiceEnemigosDisparo[j] = -1;
            columnasDisparables[j] = -1;
        }
    } 
    else {
        if(aliensVivos > 0){
            //Imprime por pantalla los enemigos visibles, asigna cuales de ellos pueden disparar
            totalColumnasDisparables = 0;
            for (int f = 0; f < filasAliens; f++) {
                for(int j = 0; j < columnasAliens; j++){
                    if(aliens[GetIndiceArray(f,columnasAliens,j)].isVisible){
                        esat::DrawSprite(
                            aliens[GetIndiceArray(f,columnasAliens,j)].sprite.imagen,
                            aliens[GetIndiceArray(f,columnasAliens,j)].areaColision.P1.x,
                            aliens[GetIndiceArray(f,columnasAliens,j)].areaColision.P1.y);
                        if(IsDisparoEnemigoPosible(f,j)){
                            indiceEnemigosDisparo[j] = GetIndiceArray(f,columnasAliens,j);
                            columnasDisparables[totalColumnasDisparables] = j;
                            totalColumnasDisparables++;
                        }
                    }
                }
            }

            // Mueve a los enemigos secuencialmente
            while(!aliens[indice_actual_mov].isVisible && indice_actual_mov < totalAliens){
                indice_actual_mov++;
            };
            if(!velocidadEnemigo == 0){
                MoverEnemigo(velocidadEnemigo); // ya se mueven normalmente
            }
            if(indice_actual_mov == 0){
                bajando = false; 
            }

            // Selecciona a uno de los posibles enemigos disparadores y dispara
            if(HacerCadaX(&tempDisparoEnemigos, 1.5f) && jugador_1.isVisible && totalColumnasDisparables > 0){
                enemigoDisparador = GenerarNumeroAleatorio(totalColumnasDisparables);
                Disparar(
                    &aliens[indiceEnemigosDisparo[columnasDisparables[enemigoDisparador]]].disparo,
                    aliens[indiceEnemigosDisparo[columnasDisparables[enemigoDisparador]]].areaColision
                );
            }
        }else{
            esat::Sleep(1000);
            GenerarBarreras();
            CargaParteEnemiga();
            inicializarEnemigoBonus();
        }
    }
}

void DibujarPie(){
    char creditArray[3], vidasArray[2];

    // Muestra la vida
    if(pantallaActual != INICIO_JUEGO){
        itoa(jugador_1.vidas, vidasArray, 10);
        esat::DrawText(MARGEN_IZQ+6,MARGEN_INF+ALTURA_FUENTE, vidasArray);

        if(jugador_1.vidas == 3){
            esat::DrawSprite(SpriteJugador, MARGEN_IZQ+40, MARGEN_INF+5);
            esat::DrawSprite(SpriteJugador, MARGEN_IZQ+85, MARGEN_INF+5);
        }else if (jugador_1.vidas == 2){
            esat::DrawSprite(SpriteJugador, MARGEN_IZQ+40, MARGEN_INF+5);
        }
    }

    // Muestra los créditos
    itoa(creditos + 100, creditArray, 10);

    esat::DrawText((MARGEN_DER+12)-(ALTURA_FUENTE*2), MARGEN_INF+ALTURA_FUENTE, creditArray+1);
    esat::DrawText(((MARGEN_DER+12)-(ALTURA_FUENTE*2))-(ALTURA_FUENTE*7), MARGEN_INF+ALTURA_FUENTE, "CREDIT");
}

//Todo lo relacionado con el dibujado de la pantall central
void DibujarInicioJuego(){
    int c;
    float textMargenIzquierda, spriteMargenIzquierdaUFO;
    if(HacerCadaX(&tempTextosInicioJuego,8)){
        //PASADOS 8 SEGUNDOS CAMBIA A PANTALLA DE ----
        pantallaActual=PETICION_CREDITOS;
    }else{
        //PROTOTIPO APARICIONES DE TEXTO
        c = ((esat::Time()/1000.0f) - tempTextosInicioJuego);

        textMargenIzquierda = MARGEN_IZQ + 190;
        spriteMargenIzquierdaUFO = textMargenIzquierda-esat::SpriteWidth(SpriteUFO)-20;

        if (c >= 0.5) {
            esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*2)),(ALTURA_FUENTE+100)*2, "PLAY");
        }

        if (c >= 1) {
            esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*7)),(ALTURA_FUENTE+130)*2, "SPACE INVADERS");
        }
        if (c >= 2) {
            esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*10)),(ALTURA_FUENTE+200)*2, "*SCORE ADVANCE TABLE*");
            esat::DrawSprite(SpriteUFO, spriteMargenIzquierdaUFO, (ALTURA_FUENTE+210)*2 );
            esat::DrawSprite(
                GetSpriteFrame(Sheet_Enemigo1,{0,0,0}), 
                //Calculo que centra el sprite al UFO
                (spriteMargenIzquierdaUFO+(esat::SpriteWidth(SpriteUFO)*0.5)) - (esat::SpriteWidth(GetSpriteFrame(Sheet_Enemigo1,{0,0,0}))*0.5), 
                (ALTURA_FUENTE+230)*2
            );
            Basura();
            esat::DrawSprite(
                GetSpriteFrame(Sheet_Enemigo2,{0,0,0}), 
                (spriteMargenIzquierdaUFO+(esat::SpriteWidth(SpriteUFO)*0.5)) - (esat::SpriteWidth(GetSpriteFrame(Sheet_Enemigo2,{0,0,0}))*0.5), 
                (ALTURA_FUENTE+250)*2
            );
            Basura();
            esat::DrawSprite(
                GetSpriteFrame(Sheet_Enemigo3,{0,0,0}), 
                (spriteMargenIzquierdaUFO+(esat::SpriteWidth(SpriteUFO)*0.5)) - (esat::SpriteWidth(GetSpriteFrame(Sheet_Enemigo3,{0,0,0}))*0.5), 
                (ALTURA_FUENTE+270)*2
            );
            Basura();
        }

        if (c >= 3) {
            esat::DrawSetFillColor(255, 0, 0);
            esat::DrawSprite(SpriteUFO, spriteMargenIzquierdaUFO, (ALTURA_FUENTE+210)*2 );
            esat::DrawText(textMargenIzquierda,(ALTURA_FUENTE+220)*2, "= ? MYSTERY");
        }

        esat::DrawSetFillColor(255, 255, 255);

        if (c >= 4) {
            esat::DrawText(textMargenIzquierda,(ALTURA_FUENTE+240)*2, "= 30 POINTS");
        } 
        
        if (c >= 5) {
            esat::DrawText(textMargenIzquierda,(ALTURA_FUENTE+260)*2, "= 20 POINTS");
        }

        if (c >= 6) {
            esat::DrawText(textMargenIzquierda,(ALTURA_FUENTE+280)*2, "= 10 POINTS");
        }
    }
}

void DibujarPeticionCreditos(){
    esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*5)),(ALTURA_FUENTE+130)*2, "INSERT COIN");
    esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*8)),(ALTURA_FUENTE+170)*2, "<1 OR 2 PLAYERS>");

    if(opcionMenuPeticion == 1){
        esat::DrawSetFillColor(0, 255, 0);
        esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*12)),(ALTURA_FUENTE+200)*2, ">> ");
        esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*8)),(ALTURA_FUENTE+200)*2, "*1 PLAYER  1 COIN");

        esat::DrawSetFillColor(255, 255, 255);
        esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*8)),(ALTURA_FUENTE+230)*2, "*2 PLAYERS 2 COINS");
    }else{
        esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*8)),(ALTURA_FUENTE+200)*2, "*1 PLAYER  1 COIN");
        
        esat::DrawSetFillColor(0, 255, 0);
        esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*12)),(ALTURA_FUENTE+230)*2, ">> ");
        esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*8)),(ALTURA_FUENTE+230)*2, "*2 PLAYERS 2 COINS");
        esat::DrawSetFillColor(255, 255, 255);
    }
    
    esat::DrawSetFillColor(255, 255, 255);
}

void DibujarSeleccionJugadores(){
    
    esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*7)),(ALTURA_FUENTE+130)*2, "PLAY PLAYER <1>");
    jugador_1.puntuacion = 0;
    
    if (HacerCadaX(&tempTextosSeleccionJugadores, 3)) {
        tempTextosMediadora = esat::Time()/1000.0f;
        pantallaActual = MEDIADORA;
    }
}

void DibujarMediadora(){

    esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*2)),(ALTURA_FUENTE+130)*2, "PUSH");
    esat::DrawText(((VENTANA_X*0.5)-(ALTURA_FUENTE*10)),(ALTURA_FUENTE+160)*2, "1 OR 2 PLAYERS BUTTON");

}

void DibujarMuertes(){
    if(jugador_1.isVisible == false){
        ExplotarJugador();
    }

    if(!enemigoBonus.isVisible && enemigoBonus.isMuriendo){
        ExplotarEnemigoBonus();
    }

    if(muriendo == true){
        ExplotarEnemigo();
    }
}

void DibujarGameOver() {
    // Dibujar game over
    int c = ((esat::Time()/100.0f) - tempTextosGameOver);
    if(jugador_1.vidas == 0 ){
        GuardarHighScore();
        
        if(HacerCadaX(&tempTextosGameOver, 4)){
            pantallaActual = PETICION_CREDITOS;
        }else{

            c = ((esat::Time()/300.0f) - tempTextosGameOver);
            if ( c % 2 == 0) {  
                esat::DrawSetFillColor(255, 0, 0);
                esat::DrawText((VENTANA_X*0.5)-ALTURA_FUENTE*6,(ALTURA_FUENTE+40)*2,  "GAME OVER !!");
                esat::DrawSetFillColor(255, 255, 255);            
            }            
        }
    }
}

void DibujarJuego(){
    // DibujarTodosLosColiders();
    DibujarJugador();
    DibujarEnemigos();
    DibujarBarreras();
    DibujarLineaAbajo();
    DibujarEnemigoBonus();
    DibujarMuertes();
    DibujarGameOver();
    DibujarColisionDisparo();
    // Mover dentro del juego
    MoverBalas();
}

void DibujarPantalla(){
    //Ejecutará el dibujado de una función u otra en función de la pantalla actual
    switch(pantallaActual){
        case INICIO_JUEGO:
            DibujarInicioJuego();
        break;
        case PETICION_CREDITOS:
            DibujarPeticionCreditos();
        break;
        case SELECCION_JUGADORES:
            DibujarSeleccionJugadores();
        break;
        case MEDIADORA:
            DibujarMediadora();
        break;
        case JUEGO:
            DibujarJuego();
        break;
    }
}
//FIN Todo lo relacionado con el dibujado de la pantall central

void DibujarEntorno(){
    DibujarCabecera();
    DibujarPie();
    DibujarPantalla();
}

void RomperBarrera(Barrera barrera, Disparo *disparo) {
    // Verificamos las coliciones de las balas enemigas con las barreras
    ColisionPixelPerfect(barrera, disparo);
}

void EventosJuego(){
    // EVENTOS JUGADOR
    // Disparo Jugador - Enemigos
    if (jugador_1.disparo.isDisparado) {
        for (int i = 0; i < totalAliens; ++i) {
            if (aliens[i].isVisible && DetectarColisionEnemigoBala(aliens[i], jugador_1.disparo)) {
                printf("EVENTO -> Enemigo - Bala jugador\n");
                muriendo = true;
                indiceMuriendo = i;
                aliens[i].isVisible = false;
                tempExplosionAlien = esat::Time()/1000;

                // PausarAccionesJuego();
                jugador_1.disparo.isDisparado = false;
                jugador_1.puntuacion += aliens[i].puntos;
            }
        }

        // Disparo Jugador - Enemigo Bonus
        if(enemigoBonus.isVisible && enemigoBonusEsDado(jugador_1.disparo)){
            printf("EVENTO -> Bonus - Bala Jugador\n");
            enemigoBonus.puntuacion = obtenerPuntuacionEnemigoBonus(numDisparos);
            jugador_1.puntuacion += enemigoBonus.puntuacion;
        }

        // Disparo Jugador - Borde Superior
        if(DetectarColisionBordeSuperior(jugador_1.disparo.areaColision)){
            printf("EVENTO -> Bala Jugador - BordeSuperior\n");
            tempExplosionBalas = esat::Time()/1000;
            colisionBalas = true;
            colisionFromBarrera = true;
            impactosBalas = jugador_1.disparo.areaColision.P1;
            jugador_1.disparo.isDisparado = false;
        }
    }

    // EVENTOS ENEMIGOS
    for(int i=0; i < totalAliens; i++){
        if(aliens[i].isVisible){
            // Enemigo - Bordes Laterales
            if((DetectarColisionBordeDerecho((aliens[i]).areaColision) || DetectarColisionBordeIzquierdo((aliens[i]).areaColision)) && !bajando){
                if(indice_actual_mov == 0){
                    CambioDireccionEnemigos();
                }
            }
            
            // Enemigo - Jugador
            if(DetectarColisionEnemigoJugador((aliens[i]),jugador_1) && jugador_1.isVisible && jugador_1.vidas > 0){
                printf("EVENTO -> Enemigo - Jugador\n");
                //Al no tener modo 2 jugadores por el momento. 
                //El restado de vidas se aplica únicamente al jugador 1
                RestarVida();
                jugador_1.isVisible = false;
                tempAnimacionJugador = esat::Time()/1000;
                velocidadEnemigo = 0;
                indiceAlienJugador = i;
            }

            // Enemigo - Borde Inferior
            if((DetectarColisionBordeInferior((aliens[i]).areaColision))){
                printf("EVENTO -> Enemigo - Borde Inferior\n");
                RestarVida();
                if(jugador_1.vidas > 0){
                    esat::Sleep(1000);
                    GenerarBarreras();
                    CargaParteEnemiga();
                    inicializarEnemigoBonus();
                }
            }
        }

        // Bala Enemigo - Jugador
        if(aliens[i].disparo.isDisparado){
            if(DetectarColision(aliens[i].disparo.areaColision,jugador_1.areaColision) && jugador_1.isVisible){
                printf("EVENTO -> Bala Enemigo - Jugador\n");
                RestarVida();
                jugador_1.isVisible = false;
                tempAnimacionJugador = esat::Time()/1000;
                aliens[i].disparo.isDisparado = false;
                velocidadEnemigo = 0;
            }
            // Bala Enemigo - BordeInferior
            if(DetectarColisionBordeInferior(aliens[i].disparo.areaColision)){
                printf("EVENTO -> Bala Enemigo - BordeInferior\n");
                tempExplosionBalas = esat::Time()/1000;
                colisionBalas = true;
                colisionFromBarrera = true;
                impactosBalas = aliens[i].disparo.areaColision.P1;
                aliens[i].disparo.isDisparado = false;
            }
        }

        // Bala Enemigo - Bala Jugador
        if(jugador_1.disparo.isDisparado && aliens[i].disparo.isDisparado){
            if(DetectarColision(aliens[i].disparo.areaColision,jugador_1.disparo.areaColision)){
                printf("EVENTO -> Bala Enemigo - Bala jugador\n");
                // Explosión en area de colisión
                tempExplosionBalas = esat::Time()/1000;
                colisionBalas = true;
                colisionFromBarrera = false;
                impactosBalas = jugador_1.disparo.areaColision.P1;
                jugador_1.disparo.isDisparado = false;
                aliens[i].disparo.isDisparado = false;
            }
        }

        // Verificamos colisiones entre enemigos y barreras
        for (int j = 0; j < NUMERO_BARRERAS; j++){
            if (aliens[i].isVisible && DetectarColision(barrera[j].areaColision, aliens[i].areaColision)){
                printf("EVENTO -> Barrera - Enemigo\n");
                ColisionEnemigoBarrera(aliens[i], j);
            }

            // Verificamos las colisiones de las balas enemigas con las barreras
            if (aliens[i].isVisible && aliens[i].disparo.isDisparado && DetectarColision(barrera[j].areaColision, aliens[i].disparo.areaColision)){
                printf("EVENTO -> Barrera - Bala Enemigo\n");
                // RomperBarrera tambien borra la bala
                RomperBarrera(barrera[j], &aliens[i].disparo);
            }
        }
        
    }

    // Verificamos las colisiones de las balas del jugador con las barreras
    if (jugador_1.disparo.isDisparado) {
        for (int i = 0; i < NUMERO_BARRERAS; i++){
            if (DetectarColision(barrera[i].areaColision, jugador_1.disparo.areaColision)){
                printf("EVENTO -> Barrera - Bala Jugador\n");
                // RomperBarrera tambien borra la bala
                RomperBarrera(barrera[i], &jugador_1.disparo);
            }
        }
    }

}

void DetectarControles() {

    if(pantallaActual==PETICION_CREDITOS){
        if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Up)){
            DesplazarEnMenu(ARRIBA);
        }

        if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Down)) {
            DesplazarEnMenu(ABAJO);
        }
        //SumarCreditos + 1
        if (esat::IsSpecialKeyDown(esat::kSpecialKey_F1)){
            SumarCreditos();
        }

        if (esat::IsSpecialKeyDown(esat::kSpecialKey_Enter) && creditos > 0 && opcionMenuPeticion == 1) {
            tempTextosSeleccionJugadores = esat::Time()/1000.0f;
            creditos--;
            pantallaActual=SELECCION_JUGADORES;
        }
    }

    if(pantallaActual==MEDIADORA){
        if (esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
            ReinicioJuego();
            pantallaActual = JUEGO;
        }
    }

    if(pantallaActual==JUEGO){
        if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Right) && jugador_1.isVisible) {
            jugador_1.direccion = DERECHA;
            MoverJugador(&jugador_1);
        } 
        if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Left)  && jugador_1.isVisible) {
            jugador_1.direccion = IZQUIERDA;
            MoverJugador(&jugador_1);
        }
        // El jugador podrá disparar con el SpaceBar siempre que:
        //      No haya un disparo suyo activo
        //      El juego ya se haya iniciado
        //      El jugador no esté muerto
        //      No haya un enemigo muiriendose
        if (esat::IsSpecialKeyDown(esat::kSpecialKey_Space) && !jugador_1.disparo.isDisparado && jugador_1.isVisible && !nivel_iniciando && !muriendo) {
            Disparar(&jugador_1.disparo, jugador_1.areaColision);
            numDisparos++;
        }
        //PROTOTIPADO
        if (esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && !nivel_iniciando){
            printf("\nMATAR Enemigos\n");
            for(int i = 0 ; i < totalAliens; i++){
                if(i < totalAliens-1){
                    aliens[i].isVisible=false;
                }
            }
            aliensVivos = 1;
        }
        if (esat::IsSpecialKeyDown(esat::kSpecialKey_Enter) && !nivel_iniciando){
            printf("\nREVIVIR TODOS\n");
            for(int i = 0 ; i < filasAliens; i++){
                for(int j = 0 ; j < columnasAliens; j++){
                    aliens[GetIndiceArray(i,columnasAliens,j)].isVisible=true;
                }
            }
        }
        if (esat::IsSpecialKeyDown(esat::kSpecialKey_Down)){
            RestarVida();
        }
    }

    
    if (esat::IsKeyDown('1')){
        tempTextosInicioJuego = esat::Time()/1000.0f;
        pantallaActual=INICIO_JUEGO;
    }
    if (esat::IsKeyDown('2')){
        pantallaActual=PETICION_CREDITOS;
    }
    if (esat::IsKeyDown('3')){
        tempTextosSeleccionJugadores = esat::Time()/1000.0f;
        pantallaActual=SELECCION_JUGADORES;
    }
    if (esat::IsKeyDown('4')){
        pantallaActual=MEDIADORA;
    }
    if (esat::IsKeyDown('5')){
        ReinicioJuego();
        pantallaActual=JUEGO;
    }
}
/* FIN FUNCIONALIDADES*/

int esat::main(int argc, char **argv) {
    GenerarSemillaAleatoria();

    esat::WindowInit(VENTANA_X,VENTANA_Y);
    WindowSetMouseVisibility(true);

    //Declaración de la fuente
    esat::DrawSetTextFont("./Recursos/Fuentes/fuenteSI.ttf");
    esat::DrawSetTextSize(19);
    esat::DrawSetFillColor(255,255,255);

    CargarSpriteSheet();
    CargaParteEnemiga();
    GenerarBarreras();

    // inicializar jugadores
    InicializarJugador();
    
    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
        last_time = esat::Time(); 

        //Inicio dibujado de pantalla
        esat::DrawBegin();
        esat::DrawClear(0,0,0);

        DetectarControles();
        EventosJuego();
        DibujarEntorno();

        esat::DrawEnd();      
        esat::WindowFrame();
        //Fin dibujado de pantalla
        
        // ControlFPS();
    }

    LiberarSprites();
    esat::WindowDestroy();
    return 0;  
}

