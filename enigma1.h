#ifndef ENIGMA1_H
#define ENIGMA1_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#define E1_BITS 8   /* 8 chaves = 1 byte (0 a 255) */

/* O que aconteceu depois de uma ação do jogador. Quem decide pontos é o jogo,
 * o enigma só informa o resultado. */
typedef enum {
    E1_NADA,
    E1_ACERTOU_PRIMEIRA,
    E1_ACERTOU,
    E1_ERROU,
    E1_FECHAR
} ResultadoE1;

typedef struct {
    int  a, b;                 /* o cálculo mostrado: a + b */
    int  alvo;                 /* resposta correta em decimal (a + b) */
    bool chaves[E1_BITS];      /* chaves[0] = bit MAIS significativo (esquerda) */
    bool resolvido;
    int  tentativas;           /* quantos erros já cometeu */
    int  feedback;             /* 0 = nada, 1 = acerto, -1 = erro */
    bool bonus_primeira;
    float tempo_feedback;      /* segundos restantes da mensagem */
} Enigma1;

/* ---- Lógica (não desenha nada; fácil de testar) ---- */
void enigma1_iniciar(Enigma1 *e);                 /* sorteia um novo cálculo */
int  enigma1_valor_atual(const Enigma1 *e);       /* decimal formado pelas chaves */
void enigma1_alternar_chave(Enigma1 *e, int i);

/* ---- Entrada ---- */
ResultadoE1 enigma1_clique(Enigma1 *e, float mx, float my);   /* coords virtuais */
ResultadoE1 enigma1_tecla(Enigma1 *e, int keycode);

/* Chame todo frame. Retorna true quando o painel deve fechar sozinho. */
bool enigma1_atualizar(Enigma1 *e);

/* ---- Desenho ---- */
void enigma1_desenhar(const Enigma1 *e, const ALLEGRO_FONT *fonte_p,
                      const ALLEGRO_FONT *fonte_m, const ALLEGRO_FONT *fonte_g);

#endif
