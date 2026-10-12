#include <stddef.h>
#include "jogador.h"
#include "config.h"
#include "recursos.h"

bool jogador_carregar(Jogador *j)
{
    j->sheet = carregar_bitmap("jogador.png", false);
    return j->sheet != NULL;
}

void jogador_destruir(Jogador *j)
{
    if (j->sheet) { al_destroy_bitmap(j->sheet); j->sheet = NULL; }
}

void jogador_parar(Jogador *j)
{
    j->movendo = false;
    j->frame = 0;
    j->tempo_anim = 0.0f;
}

void jogador_posicionar(Jogador *j, float x, float y)
{
    j->x = x;
    j->y = y;
    j->dir = DIR_BAIXO;
    jogador_parar(j);
}

void jogador_atualizar(Jogador *j, const bool *teclas, const Mapa *mapa)
{
    float dx = 0.0f, dy = 0.0f;
    float passo, nx, ny;

    if (teclas[ALLEGRO_KEY_W] || teclas[ALLEGRO_KEY_UP])    dy -= 1.0f;
    if (teclas[ALLEGRO_KEY_S] || teclas[ALLEGRO_KEY_DOWN])  dy += 1.0f;
    if (teclas[ALLEGRO_KEY_A] || teclas[ALLEGRO_KEY_LEFT])  dx -= 1.0f;
    if (teclas[ALLEGRO_KEY_D] || teclas[ALLEGRO_KEY_RIGHT]) dx += 1.0f;

    j->movendo = (dx != 0.0f || dy != 0.0f);

    if (j->movendo) {
        /* Diagonal não pode ser mais rápida: divide por raiz de 2 (~0.7071) */
        if (dx != 0.0f && dy != 0.0f) {
            dx *= 0.7071f;
            dy *= 0.7071f;
        }

        /* Escolhe para onde o boneco "olha" */
        if (dx != 0.0f) j->dir = (dx < 0.0f) ? DIR_ESQUERDA : DIR_DIREITA;
        else            j->dir = (dy < 0.0f) ? DIR_CIMA : DIR_BAIXO;

        /* Colisão por eixo: testa X e Y separados. Assim, ao bater numa parede
         * o jogador DESLIZA nela em vez de ficar travado. */
        passo = VELOCIDADE_JOGADOR * DT;

        nx = j->x + dx * passo;
        if (!mapa_colide(mapa, nx, j->y)) j->x = nx;

        ny = j->y + dy * passo;
        if (!mapa_colide(mapa, j->x, ny)) j->y = ny;

        /* Animação: frames 1..6 em loop */
        j->tempo_anim += DT;
        if (j->tempo_anim >= TEMPO_POR_FRAME_ANIM) {
            j->tempo_anim -= TEMPO_POR_FRAME_ANIM;
            j->frame++;
            if (j->frame >= SPRITE_FRAMES) j->frame = 1;
        }
        if (j->frame == 0) j->frame = 1;
    } else {
        j->frame = 0;
        j->tempo_anim = 0.0f;
    }
}

void jogador_desenhar(const Jogador *j)
{
    float larg = SPRITE_LARGURA * ESCALA_JOGADOR;
    float alt  = SPRITE_ALTURA  * ESCALA_JOGADOR;

    /* O ponto (x,y) são os pés = meio da borda de baixo do sprite */
    al_draw_scaled_bitmap(j->sheet,
                          (float)(j->frame * SPRITE_LARGURA), (float)((int)j->dir * SPRITE_ALTURA),
                          (float)SPRITE_LARGURA, (float)SPRITE_ALTURA,
                          j->x - larg / 2.0f, j->y - alt,
                          larg, alt, 0);
}
