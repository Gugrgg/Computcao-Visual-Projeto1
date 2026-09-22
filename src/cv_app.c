// Projeto 1 (Proj1) - Computacao Visual - UPM FCI
// Modulo: estado geral da aplicacao e laco principal.

#include <SDL3_image/SDL_image.h>
#include "cv_app.h"

#define CV_OUTPUT_FILENAME "output_image.png"

// Layout da janela secundaria (valores em pixels).
enum
{
  CV_PANEL_MARGIN = 16,
  CV_HIST_X = 16,
  CV_HIST_Y = 40,
  CV_HIST_W = 384,
  CV_HIST_H = 200,
  CV_INFO_Y = 266,
  CV_INFO_LINE_HEIGHT = 21,
  CV_BUTTON_W = 384,
  CV_BUTTON_H = 40,
  CV_BUTTON1_Y = 380,
  CV_BUTTON2_Y = 428
};

static const SDL_Color CV_COLOR_TEXT = { 226, 232, 240, SDL_ALPHA_OPAQUE };
static const SDL_Color CV_COLOR_TITLE = { 130, 190, 255, SDL_ALPHA_OPAQUE };
static const SDL_Color CV_COLOR_DIM = { 148, 158, 178, SDL_ALPHA_OPAQUE };

/** Recalcula o histograma da imagem que esta sendo exibida. */
static void cv_app_refresh_histogram(CVApp *app)
{
  if (!cv_histogram_compute(&app->histogram, app->image.current))
    SDL_Log("*** Aviso: nao foi possivel recalcular o histograma.");
}

/** Ajusta o tamanho/posicao da janela principal conforme o modo de exibicao. */
static void cv_app_update_main_window(CVApp *app)
{
  const int width =
      app->showing_fixed_size ? CV_MAIN_WINDOW_WIDTH : app->image.width;
  const int height =
      app->showing_fixed_size ? CV_MAIN_WINDOW_HEIGHT : app->image.height;

  cv_window_resize_and_center(&app->main_window, width, height);

  // O texto do botao indica a acao do proximo clique.
  cv_button_set_label(&app->button_resolution, app->showing_fixed_size
                                                   ? "Resolução original"
                                                   : "1024x768");
  app->redraw_main = true;
  app->redraw_panel = true;
}

bool cv_app_initialize(CVApp *app, const char *image_path)
{
  if (!app || !image_path)
    return false;

  SDL_zerop(app);

  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_Log("*** Erro ao iniciar a SDL: %s", SDL_GetError());
    return false;
  }

  if (!TTF_Init())
  {
    SDL_Log("*** Erro ao iniciar a SDL_ttf: %s", SDL_GetError());
    return false;
  }

  SDL_Log("SDL %d.%d.%d | SDL_image %d.%d.%d | SDL_ttf %d.%d.%d",
          SDL_VERSIONNUM_MAJOR(SDL_GetVersion()),
          SDL_VERSIONNUM_MINOR(SDL_GetVersion()),
          SDL_VERSIONNUM_MICRO(SDL_GetVersion()),
          SDL_VERSIONNUM_MAJOR(IMG_Version()),
          SDL_VERSIONNUM_MINOR(IMG_Version()),
          SDL_VERSIONNUM_MICRO(IMG_Version()),
          SDL_VERSIONNUM_MAJOR(TTF_Version()),
          SDL_VERSIONNUM_MINOR(TTF_Version()),
          SDL_VERSIONNUM_MICRO(TTF_Version()));

  // Janela principal: 1024x768, centralizada no monitor principal.
  if (!cv_window_create(&app->main_window, "Proj1 - Processamento de imagens",
                        CV_MAIN_WINDOW_WIDTH, CV_MAIN_WINDOW_HEIGHT, 0))
    return false;

  // Janela secundaria: tamanho fixo, filha da principal, no canto (0, 0).
  if (!cv_window_create(&app->panel_window, "Histograma e controles",
                        CV_PANEL_WINDOW_WIDTH, CV_PANEL_WINDOW_HEIGHT, 0))
    return false;

  if (!SDL_SetWindowParent(app->panel_window.window, app->main_window.window))
    SDL_Log("*** Aviso: nao foi possivel definir a janela secundaria como "
            "filha da principal: %s",
            SDL_GetError());

  cv_window_place_top_left(&app->panel_window);

  // Fontes (o programa depende de texto, entao a falha aqui e fatal).
  app->font = cv_font_open(15.0f);
  app->font_small = cv_font_open(12.0f);
  app->font_title = cv_font_open(18.0f);

  if (!app->font || !app->font_small || !app->font_title)
    return false;

  if (!cv_image_load(&app->image, image_path, app->main_window.renderer))
    return false;

  cv_app_refresh_histogram(app);

  cv_button_init(&app->button_equalize, (float)CV_HIST_X, (float)CV_BUTTON1_Y,
                 (float)CV_BUTTON_W, (float)CV_BUTTON_H, "Equalizar");
  cv_button_init(&app->button_resolution, (float)CV_HIST_X, (float)CV_BUTTON2_Y,
                 (float)CV_BUTTON_W, (float)CV_BUTTON_H, "Resolução original");

  app->showing_equalized = false;
  app->showing_fixed_size = true;
  app->running = true;
  app->redraw_main = true;
  app->redraw_panel = true;

  // Centraliza a janela principal ja no tamanho inicial de 1024x768.
  cv_app_update_main_window(app);

  SDL_Log("Pronto. Tecla S: salvar %s | ESC: sair.", CV_OUTPUT_FILENAME);
  return true;
}

void cv_app_shutdown(CVApp *app)
{
  if (!app)
    return;

  cv_image_destroy(&app->image);

  if (app->font_title)
  {
    TTF_CloseFont(app->font_title);
    app->font_title = NULL;
  }
  if (app->font_small)
  {
    TTF_CloseFont(app->font_small);
    app->font_small = NULL;
  }
  if (app->font)
  {
    TTF_CloseFont(app->font);
    app->font = NULL;
  }

  cv_window_destroy(&app->panel_window);
  cv_window_destroy(&app->main_window);

  TTF_Quit();
  SDL_Quit();
}

/** Acao do primeiro botao: alterna entre imagem equalizada e original. */
static void cv_app_toggle_equalization(CVApp *app)
{
  if (!app->showing_equalized)
  {
    if (!cv_image_build_equalized(&app->image))
      return;

    if (!cv_image_show(&app->image, app->image.equalized,
                       app->main_window.renderer))
      return;

    app->showing_equalized = true;
    cv_button_set_label(&app->button_equalize, "Ver original");
  }
  else
  {
    // Voltamos para a superficie original em escala de cinza, que nunca foi
    // modificada: nao e preciso recarregar o arquivo.
    if (!cv_image_show(&app->image, app->image.gray, app->main_window.renderer))
      return;

    app->showing_equalized = false;
    cv_button_set_label(&app->button_equalize, "Equalizar");
  }

  cv_app_refresh_histogram(app);
  app->redraw_main = true;
  app->redraw_panel = true;
}

/** Acao da tecla S: salva a imagem exibida na janela principal. */
static void cv_app_save_image(CVApp *app)
{
  int width = 0;
  int height = 0;
  SDL_GetWindowSize(app->main_window.window, &width, &height);

  bool existed = false;
  if (cv_image_save_png(&app->image, width, height, CV_OUTPUT_FILENAME,
                        &existed))
  {
    SDL_Log("Arquivo %s %s (%dx%d pixels).", CV_OUTPUT_FILENAME,
            existed ? "sobrescrito" : "criado", width, height);
  }
}

static void cv_app_handle_event(CVApp *app, const SDL_Event *event)
{
  const SDL_WindowID panel_id = SDL_GetWindowID(app->panel_window.window);
  const SDL_WindowID main_id = SDL_GetWindowID(app->main_window.window);

  switch (event->type)
  {
  case SDL_EVENT_QUIT:
    app->running = false;
    return;

  case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    // Fechar qualquer uma das janelas encerra o programa.
    app->running = false;
    return;

  case SDL_EVENT_WINDOW_EXPOSED:
    if (event->window.windowID == main_id)
      app->redraw_main = true;
    else if (event->window.windowID == panel_id)
      app->redraw_panel = true;
    return;

  case SDL_EVENT_KEY_DOWN:
    if (event->key.repeat)
      return;

    if (event->key.key == SDLK_S)
      cv_app_save_image(app);
    else if (event->key.key == SDLK_ESCAPE)
      app->running = false;
    return;

  default:
    break;
  }

  // Os botoes so respondem a eventos de mouse da janela secundaria.
  const bool is_panel_mouse_event =
      (event->type == SDL_EVENT_MOUSE_MOTION &&
       event->motion.windowID == panel_id) ||
      ((event->type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
        event->type == SDL_EVENT_MOUSE_BUTTON_UP) &&
       event->button.windowID == panel_id) ||
      (event->type == SDL_EVENT_WINDOW_MOUSE_LEAVE &&
       event->window.windowID == panel_id);

  if (!is_panel_mouse_event)
    return;

  const CVButtonState previous_equalize = app->button_equalize.state;
  const CVButtonState previous_resolution = app->button_resolution.state;

  const bool equalize_clicked =
      cv_button_handle_event(&app->button_equalize, event);
  const bool resolution_clicked =
      cv_button_handle_event(&app->button_resolution, event);

  // Redesenha a janela secundaria quando o estado visual de algum botao muda.
  if (previous_equalize != app->button_equalize.state ||
      previous_resolution != app->button_resolution.state)
    app->redraw_panel = true;

  if (equalize_clicked)
    cv_app_toggle_equalization(app);

  if (resolution_clicked)
  {
    app->showing_fixed_size = !app->showing_fixed_size;
    cv_app_update_main_window(app);
  }
}

/** Desenha a janela principal: apenas a imagem, ocupando toda a janela. */
static void cv_app_render_main(CVApp *app)
{
  SDL_Renderer *renderer = app->main_window.renderer;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  if (app->image.texture)
  {
    int width = 0;
    int height = 0;
    SDL_GetWindowSize(app->main_window.window, &width, &height);

    const SDL_FRect destination = { 0.0f, 0.0f, (float)width, (float)height };
    SDL_RenderTexture(renderer, app->image.texture, NULL, &destination);
  }

  SDL_RenderPresent(renderer);
}

/** Desenha a janela secundaria: histograma, informacoes e botoes. */
static void cv_app_render_panel(CVApp *app)
{
  SDL_Renderer *renderer = app->panel_window.renderer;

  SDL_SetRenderDrawColor(renderer, 17, 19, 24, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  cv_text_draw(renderer, app->font_title, (float)CV_PANEL_MARGIN, 10.0f,
               CV_COLOR_TITLE, "Histograma");

  const SDL_FRect histogram_area = { (float)CV_HIST_X, (float)CV_HIST_Y,
                                     (float)CV_HIST_W, (float)CV_HIST_H };
  cv_histogram_draw(&app->histogram, renderer, &histogram_area);

  // Rotulos do eixo de intensidades.
  const float labels_y = (float)(CV_HIST_Y + CV_HIST_H + 4);
  cv_text_draw(renderer, app->font_small, (float)CV_HIST_X, labels_y,
               CV_COLOR_DIM, "0");
  cv_text_draw(renderer, app->font_small,
               (float)CV_HIST_X + (float)CV_HIST_W * 0.5f - 8.0f, labels_y,
               CV_COLOR_DIM, "128");
  cv_text_draw(renderer, app->font_small,
               (float)(CV_HIST_X + CV_HIST_W) - 22.0f, labels_y, CV_COLOR_DIM,
               "255");

  // Bloco de informacoes exigido pelo item 4 do enunciado.
  float y = (float)CV_INFO_Y;
  const float x = (float)CV_PANEL_MARGIN;
  const float line = (float)CV_INFO_LINE_HEIGHT;

  cv_text_draw(renderer, app->font, x, y, CV_COLOR_TEXT, "Entrada: %s",
               app->image.source_was_color ? "colorida (convertida para cinza)"
                                           : "já em escala de cinza");
  y += line;

  cv_text_draw(renderer, app->font, x, y, CV_COLOR_TEXT,
               "Resolução original: %d x %d", app->image.width,
               app->image.height);
  y += line;

  cv_text_draw(renderer, app->font, x, y, CV_COLOR_TEXT, "Exibindo: %s em %s",
               app->showing_equalized ? "equalizada" : "original (cinza)",
               app->showing_fixed_size ? "1024x768" : "resolução original");
  y += line;

  cv_text_draw(renderer, app->font, x, y, CV_COLOR_TEXT,
               "Média de intensidade: %.2f  (imagem %s)", app->histogram.mean,
               cv_histogram_brightness_label(&app->histogram));
  y += line;

  cv_text_draw(renderer, app->font, x, y, CV_COLOR_TEXT,
               "Desvio padrão: %.2f  (contraste %s)", app->histogram.stddev,
               cv_histogram_contrast_label(&app->histogram));

  cv_button_draw(&app->button_equalize, renderer, app->font);
  cv_button_draw(&app->button_resolution, renderer, app->font);

  cv_text_draw(renderer, app->font_small, x,
               (float)(CV_BUTTON2_Y + CV_BUTTON_H + 12), CV_COLOR_DIM,
               "Tecla S: salvar %s   |   ESC: sair", CV_OUTPUT_FILENAME);

  SDL_RenderPresent(renderer);
}

void cv_app_run(CVApp *app)
{
  if (!app)
    return;

  while (app->running)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
      cv_app_handle_event(app, &event);

    if (app->redraw_main)
    {
      cv_app_render_main(app);
      app->redraw_main = false;
    }

    if (app->redraw_panel)
    {
      cv_app_render_panel(app);
      app->redraw_panel = false;
    }

    // Evita consumo desnecessario de CPU quando nada muda na tela.
    SDL_Delay(16);
  }
}
