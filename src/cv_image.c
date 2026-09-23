// Projeto 1 (Proj1) - Computacao Visual - UPM FCI
// Modulo: carregamento, conversao, equalizacao e gravacao da imagem.

#include <SDL3_image/SDL_image.h>
#include "cv_image.h"
#include "cv_histogram.h"

// Coeficientes de luminancia indicados no enunciado.
#define CV_LUMA_R 0.2125
#define CV_LUMA_G 0.7154
#define CV_LUMA_B 0.0721

/**
 * Verifica se todos os pixels tem R = G = B. Em caso afirmativo, a imagem ja
 * esta em escala de cinza.
 */
static bool cv_surface_is_grayscale(SDL_Surface *surface)
{
  const SDL_PixelFormatDetails *format =
      SDL_GetPixelFormatDetails(surface->format);
  if (!format)
    return false;

  if (!SDL_LockSurface(surface))
    return false;

  bool is_gray = true;

  for (int y = 0; y < surface->h && is_gray; ++y)
  {
    const Uint32 *row =
        (const Uint32 *)((const Uint8 *)surface->pixels + (size_t)y * surface->pitch);

    for (int x = 0; x < surface->w; ++x)
    {
      Uint8 r = 0;
      Uint8 g = 0;
      Uint8 b = 0;
      SDL_GetRGB(row[x], format, NULL, &r, &g, &b);

      if (r != g || g != b)
      {
        is_gray = false;
        break;
      }
    }
  }

  SDL_UnlockSurface(surface);
  return is_gray;
}

/**
 * Converte a superficie para escala de cinza, no lugar, aplicando
 * Y = 0.2125*R + 0.7154*G + 0.0721*B. O canal alfa e preservado.
 */
static bool cv_surface_to_grayscale(SDL_Surface *surface)
{
  const SDL_PixelFormatDetails *format =
      SDL_GetPixelFormatDetails(surface->format);
  if (!format)
    return false;

  if (!SDL_LockSurface(surface))
    return false;

  for (int y = 0; y < surface->h; ++y)
  {
    Uint32 *row = (Uint32 *)((Uint8 *)surface->pixels + (size_t)y * surface->pitch);

    for (int x = 0; x < surface->w; ++x)
    {
      Uint8 r = 0;
      Uint8 g = 0;
      Uint8 b = 0;
      Uint8 a = 0;
      SDL_GetRGBA(row[x], format, NULL, &r, &g, &b, &a);

      double luma = CV_LUMA_R * (double)r + CV_LUMA_G * (double)g +
                    CV_LUMA_B * (double)b;

      if (luma < 0.0)
        luma = 0.0;
      if (luma > 255.0)
        luma = 255.0;

      const Uint8 y_value = (Uint8)(luma + 0.5); // arredondamento

      row[x] = SDL_MapRGBA(format, NULL, y_value, y_value, y_value, a);
    }
  }

  SDL_UnlockSurface(surface);
  return true;
}

bool cv_image_load(CVImage *image, const char *filename, SDL_Renderer *renderer)
{
  if (!image || !filename || !renderer)
  {
    SDL_Log("*** Erro: parametros invalidos em cv_image_load().");
    return false;
  }

  SDL_zerop(image);

  // IMG_Load trata tanto arquivo inexistente quanto formato invalido; a causa
  // exata vem em SDL_GetError().
  SDL_Surface *loaded = IMG_Load(filename);
  if (!loaded)
  {
    SDL_Log("*** Erro ao carregar a imagem \"%s\": %s", filename,
            SDL_GetError());
    SDL_Log("    Verifique se o caminho esta correto e se o arquivo e uma "
            "imagem em formato suportado (PNG, JPEG, BMP, GIF, TGA, etc.).");
    return false;
  }

  // Convertemos para RGBA32 para que o acesso aos pixels independa do formato
  // original do arquivo.
  SDL_Surface *rgba = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_RGBA32);
  SDL_DestroySurface(loaded);

  if (!rgba)
  {
    SDL_Log("*** Erro ao converter a imagem para RGBA32: %s", SDL_GetError());
    return false;
  }

  image->width = rgba->w;
  image->height = rgba->h;

  // Item 2 do enunciado: informar se a imagem de entrada e colorida ou ja esta
  // em escala de cinza.
  image->source_was_color = !cv_surface_is_grayscale(rgba);

  if (image->source_was_color)
  {
    SDL_Log("A imagem de entrada e COLORIDA. Convertendo para escala de cinza "
            "(Y = 0.2125*R + 0.7154*G + 0.0721*B)...");

    if (!cv_surface_to_grayscale(rgba))
    {
      SDL_Log("*** Erro ao converter a imagem para escala de cinza: %s",
              SDL_GetError());
      SDL_DestroySurface(rgba);
      return false;
    }
  }
  else
  {
    SDL_Log("A imagem de entrada ja esta em ESCALA DE CINZA.");
  }

  image->gray = rgba;

  if (!cv_image_show(image, image->gray, renderer))
  {
    cv_image_destroy(image);
    return false;
  }

  SDL_Log("Imagem carregada: \"%s\" (%dx%d pixels).", filename, image->width,
          image->height);
  return true;
}

void cv_image_destroy(CVImage *image)
{
  if (!image)
    return;

  if (image->texture)
  {
    SDL_DestroyTexture(image->texture);
    image->texture = NULL;
  }

  if (image->equalized)
  {
    SDL_DestroySurface(image->equalized);
    image->equalized = NULL;
  }

  if (image->gray)
  {
    SDL_DestroySurface(image->gray);
    image->gray = NULL;
  }

  // `current` e apenas uma referencia para `gray` ou `equalized`.
  image->current = NULL;
  image->width = 0;
  image->height = 0;
}

bool cv_image_show(CVImage *image, SDL_Surface *surface, SDL_Renderer *renderer)
{
  if (!image || !surface || !renderer)
  {
    SDL_Log("*** Erro: parametros invalidos em cv_image_show().");
    return false;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture)
  {
    SDL_Log("*** Erro ao criar a textura da imagem: %s", SDL_GetError());
    return false;
  }

  // Só destruimos a textura anterior depois de criar a nova com sucesso.
  if (image->texture)
    SDL_DestroyTexture(image->texture);

  image->texture = texture;
  image->current = surface;

  SDL_SetTextureScaleMode(image->texture, SDL_SCALEMODE_LINEAR);
  return true;
}

bool cv_image_build_equalized(CVImage *image)
{
  if (!image || !image->gray)
  {
    SDL_Log("*** Erro: imagem invalida em cv_image_build_equalized().");
    return false;
  }

  // A versao equalizada e calculada uma unica vez e reaproveitada.
  if (image->equalized)
    return true;

  CVHistogram hist;
  if (!cv_histogram_compute(&hist, image->gray))
    return false;

  // Equalizacao classica (GONZALES; WOODS, cap. 3):
  //   s_k = round( (L - 1) * soma_{j=0}^{k} p_r(r_j) ),  p_r(r_j) = n_j / n
  Uint8 lookup[CV_HISTOGRAM_LEVELS];
  Uint64 accumulated = 0;

  for (int k = 0; k < CV_HISTOGRAM_LEVELS; ++k)
  {
    accumulated += hist.counts[k];

    const double cdf = (double)accumulated / (double)hist.total;
    double level = cdf * (double)(CV_HISTOGRAM_LEVELS - 1);

    if (level < 0.0)
      level = 0.0;
    if (level > 255.0)
      level = 255.0;

    lookup[k] = (Uint8)(level + 0.5);
  }

  SDL_Surface *equalized =
      SDL_ConvertSurface(image->gray, SDL_PIXELFORMAT_RGBA32);
  if (!equalized)
  {
    SDL_Log("*** Erro ao duplicar a superficie para equalizacao: %s",
            SDL_GetError());
    return false;
  }

  const SDL_PixelFormatDetails *format =
      SDL_GetPixelFormatDetails(equalized->format);
  if (!format || !SDL_LockSurface(equalized))
  {
    SDL_Log("*** Erro ao acessar os pixels da superficie equalizada: %s",
            SDL_GetError());
    SDL_DestroySurface(equalized);
    return false;
  }

  for (int y = 0; y < equalized->h; ++y)
  {
    Uint32 *row =
        (Uint32 *)((Uint8 *)equalized->pixels + (size_t)y * equalized->pitch);

    for (int x = 0; x < equalized->w; ++x)
    {
      Uint8 r = 0;
      Uint8 g = 0;
      Uint8 b = 0;
      Uint8 a = 0;
      SDL_GetRGBA(row[x], format, NULL, &r, &g, &b, &a);

      const Uint8 s = lookup[r]; // imagem em cinza: R = G = B
      row[x] = SDL_MapRGBA(format, NULL, s, s, s, a);
    }
  }

  SDL_UnlockSurface(equalized);

  image->equalized = equalized;
  SDL_Log("Histograma equalizado (transformacao calculada a partir da CDF).");
  return true;
}

bool cv_image_save_png(const CVImage *image, int out_width, int out_height,
                       const char *path, bool *existed)
{
  if (!image || !image->current || !path)
  {
    SDL_Log("*** Erro: parametros invalidos em cv_image_save_png().");
    return false;
  }

  // Descobrimos antes se o arquivo ja existia, para informar corretamente se
  // ele foi criado ou sobrescrito.
  SDL_PathInfo info;
  const bool already_exists = SDL_GetPathInfo(path, &info);

  if (existed)
    *existed = already_exists;

  SDL_Surface *to_save = image->current;
  SDL_Surface *scaled = NULL;

  // Se a janela principal estiver exibindo a imagem em outra resolucao,
  // salvamos exatamente o que esta sendo exibido.
  if (out_width != image->current->w || out_height != image->current->h)
  {
    scaled = SDL_ScaleSurface(image->current, out_width, out_height,
                              SDL_SCALEMODE_LINEAR);
    if (!scaled)
    {
      SDL_Log("*** Erro ao redimensionar a imagem para salvar: %s",
              SDL_GetError());
      return false;
    }
    to_save = scaled;
  }

  const bool ok = IMG_SavePNG(to_save, path);

  if (scaled)
    SDL_DestroySurface(scaled);

  if (!ok)
  {
    SDL_Log("*** Erro ao salvar \"%s\": %s", path, SDL_GetError());
    return false;
  }

  return true;
}
