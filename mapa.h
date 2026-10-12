#ifndef MAPA_H
#define MAPA_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

/* Retângulo em coordenadas do cenário (1408x768) */
typedef struct {
    float x1, y1, x2, y2;
} Retangulo;

/* Cada objeto com o qual o jogador pode interagir apertando E */
typedef enum {
    INT_ENIGMA1,
    INT_ENIGMA2,
    INT_ENIGMA3,
    INT_PORTA
} IdInteracao;

typedef struct {
    IdInteracao id;
    Retangulo   zona;      /* se os PÉS do jogador estiverem aqui, pode apertar E */
    const char *rotulo;    /* texto mostrado no aviso "[E] ..." */
} Interagivel;

#define MAX_OBSTACULOS  16
#define MAX_INTERAGIVEIS 8

typedef struct {
    ALLEGRO_BITMAP *fundo;
    Retangulo   obstaculos[MAX_OBSTACULOS];
    int         n_obstaculos;
    Interagivel interagiveis[MAX_INTERAGIVEIS];
    int         n_interagiveis;
} Mapa;

bool mapa_carregar_fase1(Mapa *m);
void mapa_destruir(Mapa *m);

/* true se a hitbox dos pés, com os pés em (x,y), bate em algum obstáculo */
bool mapa_colide(const Mapa *m, float x, float y);

/* Retorna o interagível sob os pés em (x,y), ou NULL se não houver */
const Interagivel *mapa_interacao_em(const Mapa *m, float x, float y);

/* debug = true desenha obstáculos (vermelho) e zonas de interação (verde) */
void mapa_desenhar(const Mapa *m, bool debug, const ALLEGRO_FONT *fonte);

#endif
