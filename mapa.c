#include <stdio.h>
#include <allegro5/allegro_primitives.h>
#include "mapa.h"
#include "config.h"
#include "recursos.h"

static void add_obstaculo(Mapa *m, float x1, float y1, float x2, float y2)
{
    if (m->n_obstaculos < MAX_OBSTACULOS) {
        Retangulo r = { x1, y1, x2, y2 };
        m->obstaculos[m->n_obstaculos++] = r;
    }
}

static void add_interagivel(Mapa *m, IdInteracao id, const char *rotulo,
                            float x1, float y1, float x2, float y2)
{
    if (m->n_interagiveis < MAX_INTERAGIVEIS) {
        Interagivel it;
        it.id = id;
        it.rotulo = rotulo;
        it.zona.x1 = x1; it.zona.y1 = y1; it.zona.x2 = x2; it.zona.y2 = y2;
        m->interagiveis[m->n_interagiveis++] = it;
    }
}

bool mapa_carregar_fase1(Mapa *m)
{
    m->n_obstaculos = 0;
    m->n_interagiveis = 0;

    m->fundo = carregar_bitmap("fase1.png", true);
    if (m->fundo == NULL) return false;

    /* ---------- OBSTÁCULOS (coordenadas na imagem 1408x768) ----------
     * Se o personagem estiver atravessando algo ou travando em lugar vazio,
     * ajuste os números aqui. Aperte F1 no jogo para VER esses retângulos. */
    add_obstaculo(m,    0,   0, 1408, 230);   /* bancadas de cima (parede norte) */
    add_obstaculo(m,    0,   0,  190, 768);   /* bancadas da esquerda            */
    add_obstaculo(m,    0, 595, 1408, 768);   /* bancadas de baixo               */
    add_obstaculo(m, 1335,   0, 1408, 768);   /* parede da direita               */
    add_obstaculo(m,  470, 330,  900, 500);   /* mesa do Enigma 1                */
    add_obstaculo(m,  948, 225, 1225, 382);   /* painel de RAM (Enigma 2)        */
    add_obstaculo(m, 1225,   0, 1335, 385);   /* parede ao redor da porta        */
    add_obstaculo(m, 1090, 490, 1335, 768);   /* computador do Enigma 3          */

    /* ---------- ZONAS DE INTERAÇÃO (a tecla E funciona com os PÉS aqui) ---------- */
    add_interagivel(m, INT_ENIGMA1, "Decodificador ALU",     430, 240,  940, 595);
    add_interagivel(m, INT_ENIGMA2, "Alocacao de Registradores", 930, 382, 1222, 455);
    add_interagivel(m, INT_ENIGMA3, "Terminal de Algoritmos", 1030, 455, 1335, 595);
    add_interagivel(m, INT_PORTA,   "Porta",                 1225, 385, 1335, 455);

    return true;
}

void mapa_destruir(Mapa *m)
{
    if (m->fundo) { al_destroy_bitmap(m->fundo); m->fundo = NULL; }
}

static bool retangulos_colidem(const Retangulo *a, const Retangulo *b)
{
    return a->x1 < b->x2 && a->x2 > b->x1 && a->y1 < b->y2 && a->y2 > b->y1;
}

bool mapa_colide(const Mapa *m, float x, float y)
{
    Retangulo pes;
    int i;
    pes.x1 = x - HITBOX_LARGURA / 2.0f;
    pes.x2 = x + HITBOX_LARGURA / 2.0f;
    pes.y1 = y - HITBOX_ALTURA;
    pes.y2 = y;

    for (i = 0; i < m->n_obstaculos; i++)
        if (retangulos_colidem(&pes, &m->obstaculos[i]))
            return true;
    return false;
}

const Interagivel *mapa_interacao_em(const Mapa *m, float x, float y)
{
    int i;
    for (i = 0; i < m->n_interagiveis; i++) {
        const Retangulo *z = &m->interagiveis[i].zona;
        if (x >= z->x1 && x <= z->x2 && y >= z->y1 && y <= z->y2)
            return &m->interagiveis[i];
    }
    return NULL;
}

void mapa_desenhar(const Mapa *m, bool debug, const ALLEGRO_FONT *fonte)
{
    int i;
    al_draw_scaled_bitmap(m->fundo,
                          0, 0, (float)al_get_bitmap_width(m->fundo), (float)al_get_bitmap_height(m->fundo),
                          0, 0, (float)LARGURA_VIRTUAL, (float)ALTURA_VIRTUAL, 0);

    if (!debug) return;

    for (i = 0; i < m->n_obstaculos; i++) {
        const Retangulo *r = &m->obstaculos[i];
        al_draw_filled_rectangle(r->x1, r->y1, r->x2, r->y2, al_map_rgba(120, 0, 0, 90));
        al_draw_rectangle(r->x1, r->y1, r->x2, r->y2, al_map_rgb(255, 60, 60), 2);
    }
    for (i = 0; i < m->n_interagiveis; i++) {
        const Interagivel *it = &m->interagiveis[i];
        al_draw_rectangle(it->zona.x1, it->zona.y1, it->zona.x2, it->zona.y2, al_map_rgb(60, 255, 60), 2);
        al_draw_text(fonte, al_map_rgb(60, 255, 60), it->zona.x1 + 4, it->zona.y1 + 4, 0, it->rotulo);
    }
}
