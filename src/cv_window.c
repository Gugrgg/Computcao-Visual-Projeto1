// Projeto 1 (Proj1) - Computacao Visual - UPM FCI
// Modulo: janela + renderizador.

#include "cv_window.h"

bool cv_window_create(CVWindow *win, const char *title, int width, int height,
                      SDL_WindowFlags flags)
{
  if (!win)
  {
    SDL_Log("*** Erro: CVWindow invalida (win == NULL).");
    return false;
  }

  win->window = NULL;
  win->renderer = NULL;

  if (!SDL_CreateWindowAndRenderer(title, width, height, flags, &win->window,
                                   &win->renderer))
  {
    SDL_Log("*** Erro ao criar janela \"%s\": %s", title, SDL_GetError());
    return false;
  }

  return true;
}

void cv_window_destroy(CVWindow *win)
{
  if (!win)
    return;

  if (win->renderer)
  {
    SDL_DestroyRenderer(win->renderer);
    win->renderer = NULL;
  }

  if (win->window)
  {
    SDL_DestroyWindow(win->window);
    win->window = NULL;
  }
}

void cv_window_resize_and_center(CVWindow *win, int width, int height)
{
  if (!win || !win->window)
    return;

  SDL_SetWindowSize(win->window, width, height);

  SDL_DisplayID display = SDL_GetPrimaryDisplay();
  SDL_Rect bounds = { 0, 0, 0, 0 };

  if (!SDL_GetDisplayBounds(display, &bounds))
  {
    SDL_Log("*** Aviso: nao foi possivel obter a resolucao do monitor "
            "principal (%s). A janela sera apenas centralizada.",
            SDL_GetError());
    SDL_SetWindowPosition(win->window, SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);
    SDL_SyncWindow(win->window);
    return;
  }

  // Regra do enunciado (item 6): a janela fica centralizada no monitor
  // principal, exceto quando for maior do que a resolucao atual da tela; nesse
  // caso, o canto superior esquerdo vai para (0, 0).
  if (width > bounds.w || height > bounds.h)
  {
    SDL_Log("\tJanela (%dx%d) excede a resolucao do monitor principal (%dx%d): "
            "posicionando em (0, 0).",
            width, height, bounds.w, bounds.h);
    SDL_SetWindowPosition(win->window, bounds.x, bounds.y);
  }
  else
  {
    SDL_SetWindowPosition(win->window, SDL_WINDOWPOS_CENTERED_DISPLAY(display),
                          SDL_WINDOWPOS_CENTERED_DISPLAY(display));
  }

  SDL_SyncWindow(win->window);
}

void cv_window_place_top_left(CVWindow *win)
{
  if (!win || !win->window)
    return;

  // Posicionar exatamente em (0, 0) faria a barra de titulo ficar fora da area
  // visivel da tela, porque a posicao informada a SDL e a da area de cliente.
  // Compensamos a espessura da borda superior e esquerda (mesma abordagem do
  // exemplo 04 da disciplina).
  int top = 0;
  int left = 0;
  SDL_GetWindowBordersSize(win->window, &top, &left, NULL, NULL);

  SDL_SetWindowPosition(win->window, left, top);
  SDL_SyncWindow(win->window);
}
