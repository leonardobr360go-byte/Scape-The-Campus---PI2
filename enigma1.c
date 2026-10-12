#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro_primitives.h>
#include "enigma1.h"
#include "config.h"

/* Mostrar o peso de cada chave (128 64 32 ...) embaixo dela? Ajuda a
 * jogar. Ponha 0 para deixar o enigma mais difícil. */
#define E1_MOSTRAR_PESOS 1

/* ---- Layout do painel (coordenadas virtuais) ---- */
#define PX 304.0f
#define PY 110.0f
#define PW 800.0f
#define PH 570.0f
#define CENTRO_X (PX + PW / 2.0f)

#define CH_LARG   56.0f
#define CH_ALT   110.0f
#define CH_ESPACO 80.0f
#define CH_TOPO  (PY + 300.0f)

#define BTN_X1 (CENTRO_X - 100.0f)
#define BTN_X2 (CENTRO_X + 100.0f)
#define BTN_Y1 (PY + 475.0f)
#define BTN_Y2 (PY + 520.0f)

/* ============================ LÓGICA ============================ */

void enigma1_iniciar(Enigma1 *e)
{
    int i;
    e->a = 10 + rand() % 111;          /* 10..120 */
    e->b = 10 + rand() % 111;          /* 10..120 -> soma máxima 240 (cabe em 8 bits) */
    e->alvo = e->a + e->b;
    for (i = 0; i < E1_BITS; i++) e->chaves[i] = false;
    e->resolvido = false;
    e->tentativas = 0;
    e->feedback = 0;
    e->bonus_primeira = false;
    e->tempo_feedback = 0.0f;
}

int enigma1_valor_atual(const Enigma1 *e)
{
    int i, valor = 0;
    for (i = 0; i < E1_BITS; i++) {
        /* chave 0 é o bit 7 (peso 128), chave 7 é o bit 0 (peso 1) */
        if (e->chaves[i])
            valor += 1 << (E1_BITS - 1 - i);
    }
    return valor;
}

void enigma1_alternar_chave(Enigma1 *e, int i)
{
    if (e->resolvido) return;                  /* RN05: resolvido fica inativo */
    if (i < 0 || i >= E1_BITS) return;
    e->chaves[i] = !e->chaves[i];
    e->feedback = 0;                           /* some a mensagem de erro antiga */
}

static ResultadoE1 confirmar(Enigma1 *e)
{
    if (e->resolvido) return E1_NADA;

    if (enigma1_valor_atual(e) == e->alvo) {
        e->resolvido = true;
        e->feedback = 1;
        e->tempo_feedback = 1.8f;
        e->bonus_primeira = (e->tentativas == 0);
        return e->bonus_primeira ? E1_ACERTOU_PRIMEIRA : E1_ACERTOU;
    }

    e->tentativas++;
    e->feedback = -1;
    e->tempo_feedback = 1.5f;
    return E1_ERROU;
}

/* ============================ ENTRADA ============================ */

static bool dentro(float x, float y, float x1, float y1, float x2, float y2)
{
    return x >= x1 && x <= x2 && y >= y1 && y <= y2;
}

static float centro_chave(int i)
{
    return CENTRO_X + ((float)i - (E1_BITS - 1) / 2.0f) * CH_ESPACO;
}

ResultadoE1 enigma1_clique(Enigma1 *e, float mx, float my)
{
    int i;

    if (dentro(mx, my, BTN_X1, BTN_Y1, BTN_X2, BTN_Y2))
        return confirmar(e);

    for (i = 0; i < E1_BITS; i++) {
        float cx = centro_chave(i);
        if (dentro(mx, my, cx - CH_LARG / 2, CH_TOPO, cx + CH_LARG / 2, CH_TOPO + CH_ALT)) {
            enigma1_alternar_chave(e, i);
            break;
        }
    }
    return E1_NADA;
}

ResultadoE1 enigma1_tecla(Enigma1 *e, int keycode)
{
    if (keycode == ALLEGRO_KEY_ESCAPE) return E1_FECHAR;
    if (keycode == ALLEGRO_KEY_ENTER || keycode == ALLEGRO_KEY_PAD_ENTER) return confirmar(e);

    /* Teclas 1..8 alternam as chaves da esquerda para a direita */
    if (keycode >= ALLEGRO_KEY_1 && keycode < ALLEGRO_KEY_1 + E1_BITS)
        enigma1_alternar_chave(e, keycode - ALLEGRO_KEY_1);

    return E1_NADA;
}

bool enigma1_atualizar(Enigma1 *e)
{
    if (e->tempo_feedback > 0.0f) {
        e->tempo_feedback -= DT;
        if (e->tempo_feedback <= 0.0f) {
            e->tempo_feedback = 0.0f;
            e->feedback = 0;
            if (e->resolvido) return true;     /* acertou: fecha o painel */
        }
    }
    return false;
}

/* ============================ DESENHO ============================ */

void enigma1_desenhar(const Enigma1 *e, const ALLEGRO_FONT *fonte_p,
                      const ALLEGRO_FONT *fonte_m, const ALLEGRO_FONT *fonte_g)
{
    ALLEGRO_COLOR ciano   = al_map_rgb(0, 255, 200);
    ALLEGRO_COLOR branco  = al_map_rgb(235, 245, 255);
    ALLEGRO_COLOR cinza   = al_map_rgb(120, 140, 160);
    ALLEGRO_COLOR borda   = ciano;
    char bin[E1_BITS + 1];
    char peso[8];
    int i;

    if (e->feedback == 1)  borda = al_map_rgb(60, 255, 100);
    if (e->feedback == -1) borda = al_map_rgb(255, 70, 70);

    /* Fundo escurecido + painel */
    al_draw_filled_rectangle(0, 0, LARGURA_VIRTUAL, ALTURA_VIRTUAL, al_map_rgba(0, 0, 0, 170));
    al_draw_filled_rectangle(PX, PY, PX + PW, PY + PH, al_map_rgb(12, 22, 38));
    al_draw_rectangle(PX, PY, PX + PW, PY + PH, borda, 4);

    al_draw_text(fonte_m, ciano, CENTRO_X, PY + 18, ALLEGRO_ALIGN_CENTRE, "ENIGMA 1 - DECODIFICADOR ALU");
    al_draw_text(fonte_p, cinza, CENTRO_X, PY + 62, ALLEGRO_ALIGN_CENTRE,
                 "Calcule o resultado e monte-o em BINARIO com as chaves");
    al_draw_textf(fonte_g, branco, CENTRO_X, PY + 95, ALLEGRO_ALIGN_CENTRE, "%d + %d = ?", e->a, e->b);

    /* Visor com o binário atual */
    {
        int v = enigma1_valor_atual(e);
        for (i = 0; i < E1_BITS; i++)
            bin[i] = (v & (1 << (E1_BITS - 1 - i))) ? '1' : '0';
        bin[E1_BITS] = '\0';
    }
    al_draw_filled_rectangle(CENTRO_X - 200, PY + 175, CENTRO_X + 200, PY + 245, al_map_rgb(4, 10, 18));
    al_draw_rectangle(CENTRO_X - 200, PY + 175, CENTRO_X + 200, PY + 245, ciano, 2);
    al_draw_text(fonte_g, ciano, CENTRO_X, PY + 184, ALLEGRO_ALIGN_CENTRE, bin);

    /* Chaves */
    for (i = 0; i < E1_BITS; i++) {
        float cx = centro_chave(i);
        float x1 = cx - CH_LARG / 2, x2 = cx + CH_LARG / 2;
        float y1 = CH_TOPO,          y2 = CH_TOPO + CH_ALT;
        bool ligada = e->chaves[i];

        al_draw_text(fonte_m, ligada ? al_map_rgb(60, 255, 160) : cinza,
                     cx, PY + 262, ALLEGRO_ALIGN_CENTRE, ligada ? "1" : "0");

        al_draw_filled_rounded_rectangle(x1, y1, x2, y2, 8, 8, al_map_rgb(30, 45, 65));
        al_draw_rounded_rectangle(x1, y1, x2, y2, 8, 8, al_map_rgb(90, 130, 160), 2);

        /* A "alavanca": metade de cima = ligada, metade de baixo = desligada */
        if (ligada)
            al_draw_filled_rounded_rectangle(x1 + 6, y1 + 6, x2 - 6, y1 + CH_ALT / 2, 6, 6, al_map_rgb(0, 230, 140));
        else
            al_draw_filled_rounded_rectangle(x1 + 6, y1 + CH_ALT / 2, x2 - 6, y2 - 6, 6, 6, al_map_rgb(70, 85, 105));

#if E1_MOSTRAR_PESOS
        snprintf(peso, sizeof(peso), "%d", 1 << (E1_BITS - 1 - i));
        al_draw_text(fonte_p, cinza, cx, y2 + 8, ALLEGRO_ALIGN_CENTRE, peso);
#endif
    }

    /* Feedback */
    if (e->feedback == 1) {
        if (e->bonus_primeira)
            al_draw_textf(fonte_m, al_map_rgb(60, 255, 100), CENTRO_X, PY + 438, ALLEGRO_ALIGN_CENTRE,
                          "ALU DECODIFICADA!  +%d", BONUS_PRIMEIRA);
        else
            al_draw_text(fonte_m, al_map_rgb(60, 255, 100), CENTRO_X, PY + 438, ALLEGRO_ALIGN_CENTRE,
                         "ALU DECODIFICADA!");
    } else if (e->feedback == -1) {
        al_draw_textf(fonte_m, al_map_rgb(255, 70, 70), CENTRO_X, PY + 438, ALLEGRO_ALIGN_CENTRE,
                      "RESULTADO INCORRETO  -%d", PENALIDADE_ERRO);
    }

    /* Botão confirmar */
    al_draw_filled_rectangle(BTN_X1, BTN_Y1, BTN_X2, BTN_Y2,
                             e->resolvido ? al_map_rgb(40, 60, 50) : al_map_rgb(0, 120, 100));
    al_draw_rectangle(BTN_X1, BTN_Y1, BTN_X2, BTN_Y2, ciano, 2);
    al_draw_text(fonte_m, branco, CENTRO_X, BTN_Y1 + 6, ALLEGRO_ALIGN_CENTRE, "CONFIRMAR");

    al_draw_text(fonte_p, cinza, CENTRO_X, PY + PH - 30, ALLEGRO_ALIGN_CENTRE,
                 "Clique nas chaves (ou teclas 1-8)  |  ENTER confirma  |  ESC fecha");
}
