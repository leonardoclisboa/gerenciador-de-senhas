@echo off
echo ========================================================
echo Compilando Sistema de Gerenciamento de Senhas Clinicas
echo ========================================================
gcc -Wall -Wextra -std=c99 -pedantic src\main.c src\paciente.c src\fila.c src\atendimento.c src\menu.c -o sistema_senhas.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilacao concluida com sucesso!
    echo Execute: sistema_senhas.exe
) else (
    echo.
    echo [ERRO] Ocorreu um erro durante a compilacao.
)
