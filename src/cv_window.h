// Projeto 1 (Proj1) - Computacao Visual - UPM FCI
// Modulo: janela + renderizador.
#ifndef CV_WINDOW_H
#define CV_WINDOW_H

#include <stdbool.h>
#include <SDL3/SDL.h>

/**
 * Agrupa uma janela e o renderizador associado a ela, evitando ponteiros
 * soltos espalhados pelo programa. Cada janela do programa tem o seu proprio
 * renderizador.
 */
typedef struct CVWindow
{
  SDL_Window *window;
  SDL_Renderer *renderer;
} CVWindow;

bool cv_window_create(CVWindow *win, const char *title, int width, int height,
                      SDL_WindowFlags flags);
void cv_window_destroy(CVWindow *win);

/**
 * Redimensiona a janela e a posiciona centralizada no monitor principal.
 * Excecao exigida pelo enunciado: se a janela for maior do que a resolucao
 * atual do monitor principal, o canto superior esquerdo vai para (0, 0).
 */
void cv_window_resize_and_center(CVWindow *win, int width, int height);

/**
 * Posiciona a janela no canto superior esquerdo da tela, compensando a
 * espessura da borda/barra de titulo para que ela nao fique fora da area
 * visivel (mesma tecnica usada no exemplo 04 da disciplina).
 */
void cv_window_place_top_left(CVWindow *win);

#endif // CV_WINDOW_H
