// Projeto 1 (Proj1) - Computacao Visual - UPM FCI
// Integrantes:
// Gustavo Galhardo Rodrigues - RA 10403091
// Guilherme Rainho Geraldo - RA 10418251
// Gabriel Vieira de Sousa - RA 10410264
// Guilherme Gomes Arantes Teles - RA 10364065
// Ponto de entrada: valida os argumentos de linha de comando e executa o
// programa de processamento de imagens.
//
// Uso: imgproc <caminho_da_imagem.ext>

#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "cv_app.h"

static void print_usage(const char *program_name)
{
  SDL_Log("Uso: %s <caminho_da_imagem.ext>",
          program_name ? program_name : "imgproc");
  SDL_Log("Exemplo: %s images/teste_colorida_baixo_contraste.png",
          program_name ? program_name : "imgproc");
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    SDL_Log("*** Erro: o programa espera exatamente um argumento (o caminho "
            "do arquivo de imagem). Recebidos: %d.",
            argc - 1);
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  CVApp app;

  if (!cv_app_initialize(&app, argv[1]))
  {
    cv_app_shutdown(&app);
    return EXIT_FAILURE;
  }

  cv_app_run(&app);
  cv_app_shutdown(&app);

  return EXIT_SUCCESS;
}
