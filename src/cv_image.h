// Projeto 1 (Proj1) - Computacao Visual - UPM FCI
// Modulo: carregamento, conversao, equalizacao e gravacao da imagem.
#ifndef CV_IMAGE_H
#define CV_IMAGE_H

#include <stdbool.h>
#include <SDL3/SDL.h>

/**
 * Imagem do programa.
 *
 * `gray` guarda a imagem em escala de cinza e NUNCA e modificada depois de
 * criada: e ela que permite voltar ao original sem recarregar o arquivo.
 * `equalized` guarda o resultado da equalizacao (criada sob demanda).
 * `current` apenas aponta para uma das duas (nao e dona da memoria) e
 * `texture` e a textura correspondente a `current`.
 */
typedef struct CVImage
{
  SDL_Surface *gray;
  SDL_Surface *equalized;
  SDL_Surface *current;
  SDL_Texture *texture;
  int width;
  int height;
  bool source_was_color;
} CVImage;

/**
 * Carrega o arquivo, informa se a origem era colorida ou ja estava em escala
 * de cinza, converte para escala de cinza e prepara a textura inicial.
 */
bool cv_image_load(CVImage *image, const char *filename, SDL_Renderer *renderer);

void cv_image_destroy(CVImage *image);

/** Cria (uma unica vez) a versao equalizada a partir de `gray`. */
bool cv_image_build_equalized(CVImage *image);

/** Troca a superficie exibida e recria a textura, sem recarregar o arquivo. */
bool cv_image_show(CVImage *image, SDL_Surface *surface, SDL_Renderer *renderer);

/**
 * Salva a imagem atual em PNG, na resolucao (out_width x out_height) que esta
 * sendo exibida. Informa por `existed` se o arquivo ja existia antes.
 */
bool cv_image_save_png(const CVImage *image, int out_width, int out_height,
                       const char *path, bool *existed);

#endif // CV_IMAGE_H
