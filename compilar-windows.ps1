param(
    [string]$GccPath,
    [string]$SdlDir,
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$MakeArgs
)

$ErrorActionPreference = 'Stop'
$localTools = Join-Path $PSScriptRoot '../.toolchain'
if (!$GccPath) {
    $localGcc = Join-Path $localTools 'mingw64/bin/gcc.exe'
    if (Test-Path $localGcc) { $GccPath = $localGcc }
    else {
        $installedGcc = Get-Command gcc.exe -ErrorAction SilentlyContinue
        if ($installedGcc) { $GccPath = $installedGcc.Source }
    }
}
if (!$GccPath -or !(Test-Path $GccPath)) {
    throw 'Instale GCC 15.1.0/MinGW-w64 e informe -GccPath C:/dev/mingw64/bin/gcc.exe. Consulte README.md.'
}
if (!$SdlDir) {
    if ($env:SDL_DIR) { $SdlDir = $env:SDL_DIR }
    else { $SdlDir = Join-Path $localTools 'SDL3' }
}
if (!(Test-Path (Join-Path $SdlDir 'include/SDL3/SDL.h'))) {
    throw 'Instale as tres bibliotecas SDL e informe -SdlDir C:/dev/SDL3. Consulte README.md.'
}

# Os caminhos curtos contornam limitacoes do GCC/ld com acentos e espacos.
$fileSystem = New-Object -ComObject Scripting.FileSystemObject
$compilerRoot = Split-Path (Split-Path (Resolve-Path $GccPath).Path -Parent) -Parent
$compilerRoot = $fileSystem.GetFolder($compilerRoot).ShortPath.Replace('\', '/')
$SdlDir = $fileSystem.GetFolder((Resolve-Path $SdlDir).Path).ShortPath.Replace('\', '/')
if ($compilerRoot -match '[^\x00-\x7F]|\s') {
    throw 'Coloque o compilador em um caminho sem espacos ou acentos, por exemplo C:/dev/mingw64.'
}
$compilerBin = "$compilerRoot/bin"
$gcc = "$compilerBin/gcc.exe"
$make = "$compilerBin/mingw32-make.exe"
if (!(Test-Path $make)) { throw 'mingw32-make.exe nao encontrado na pasta bin do compilador.' }
$previousPath = $env:PATH
Push-Location $PSScriptRoot
try {
    $env:PATH = "$compilerBin;$previousPath"
    $version = & $gcc -dumpfullversion
    if ($LASTEXITCODE -ne 0 -or $version -ne '15.1.0') { throw "Esperado GCC 15.1.0; encontrado: $version" }
    & $gcc --version | Select-Object -First 1
    $compilerCommand = "gcc -B$compilerRoot/lib/gcc/x86_64-w64-mingw32/15.1.0/ -B$compilerRoot/x86_64-w64-mingw32/lib/"
    & $make "CC=$compilerCommand" 'SHELL=cmd.exe' "SDL_DIR=$SdlDir" @MakeArgs
    if ($LASTEXITCODE -ne 0) { throw "Falha na compilacao (codigo $LASTEXITCODE)." }
}
finally { $env:PATH = $previousPath; Pop-Location }
