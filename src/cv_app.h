// Projeto 1 (Proj1) - Computacao Visual - UPM FCI
// Modulo: estado geral da aplicacao e laco principal.
#ifndef CV_APP_H
#define CV_APP_H

#include <stdbool.h>
#include "cv_window.h"
#include "cv_image.h"
#include "cv_histogram.h"
#include "cv_ui.h"

enum
{
  CV_MAIN_WINDOW_WIDTH = 1024,
  CV_MAIN_WINDOW_HEIGHT = 768,
  CV_PANEL_WINDOW_WIDTH = 416,
  CV_PANEL_WINDOW_HEIGHT = 520
};

typedef struct CVApp
{
  CVWindow main_window;  // exibe a imagem sendo processada
  CVWindow panel_window; // janela filha: histograma, informacoes e botoes

  CVImage image;
  CVHistogram histogram;

  TTF_Font *font;
  TTF_Font *font_small;
  TTF_Font *font_title;

  CVButton button_equalize;
  CVButton button_resolution;

  bool showing_equalized; // false = original em cinza, true = equalizada
  bool showing_fixed_size; // true = 1024x768, false = resolucao original

  bool running;
  bool redraw_main;
  bool redraw_panel;
} CVApp;

bool cv_app_initialize(CVApp *app, const char *image_path);
void cv_app_run(CVApp *app);
void cv_app_shutdown(CVApp *app);

#endif // CV_APP_H
