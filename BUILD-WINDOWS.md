# Compilação verificada no Windows

Compilado em 22/09/2026 no Windows 10 Home 22H2 (build 19045), para x86-64,
com GCC **15.1.0**, WinLibs / MinGW-w64 UCRT, usando C17,
`-Wall -Wextra -Wpedantic -O2`. Compilação sem erros ou avisos.

O executável é `imgproc.exe`. Mantenha as três DLLs e a pasta `assets`
junto dele. Para executar, abra um terminal nesta pasta:

```powershell
.\imgproc.exe images/teste_colorida_baixo_contraste.png
```

Para recompilar usando o compilador portátil preparado nesta máquina:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\compilar-windows.ps1
```

O script usa o Makefile, exige GCC 15.1.0 e não altera o PATH permanente.
Na máquina de preparação, encontra automaticamente `../.toolchain`.
Em outra máquina, instale as dependências indicadas abaixo e informe os caminhos:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\compilar-windows.ps1 -GccPath C:/dev/mingw64/bin/gcc.exe -SdlDir C:/dev/SDL3
```

Para testar, acrescente `-MakeArgs test`. A pasta `.toolchain` não deve ser enviada ao GitHub.
O script usa caminhos curtos para contornar limitações do compilador com acentos.
Se indisponíveis, instale o compilador em um caminho sem espaços ou acentos.

Dependências obtidas dos distribuidores:

- [GCC 15.1.0 / WinLibs](https://github.com/brechtsanders/winlibs_mingw/releases/tag/15.1.0posix-12.0.0-ucrt-r1)
- [SDL 3.4.16](https://github.com/libsdl-org/SDL/releases/tag/release-3.4.16)
- [SDL_image 3.4.6](https://github.com/libsdl-org/SDL_image/releases/tag/release-3.4.6)
- [SDL_ttf 3.2.2](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-3.2.2)

O teste existente de processamento foi compilado e executado nas três imagens
incluídas (PNG em cinza, PNG colorido e JPEG colorido). Os três terminaram com
`OK`, gerando imagens em cinza, equalizadas e redimensionadas para 1024x768.
O teste agora usa caminhos portáveis em `build/out_*.png`, verifica falhas e libera os recursos também nos caminhos de erro. O alvo `make test` executa as três imagens.

O carregamento do executável e das DLLs foi verificado sem o compilador no PATH.
Os testes de processamento usam o driver de vídeo `dummy`; os controles da
interface gráfica não foram testados manualmente nesta verificação.
Os registros estão em `build/build-windows.log` e `build/test-*.log`.

A auditoria de integração confirmou a janela filha, os estados dos botões, a troca de resolução e a gravação/sobrescrita com o driver Windows, usando janelas ocultas e eventos simulados. A compilação no WSL Ubuntu 26.04/GCC 15.2.0 ainda precisa ser validada.
