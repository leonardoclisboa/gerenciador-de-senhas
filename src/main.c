/*
 * ============================================================================
 * Projeto: Sistema de Gerenciamento de Senhas de Atendimento Clinico
 * Disciplina: Avaliacao de Desenvolvimento de Software
 * Questao: 02 - Sistema de Senhas Clinicas
 * Linguagem: C (Padrao C99)
 *
 * Descricao:
 *   Modulo principal de entrada da aplicacao.
 *   Responsavel unicamente pela inicializacao do sistema, invocacao do loop
 *   do menu interativo e liberacao controlada de recursos ao termino.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "atendimento.h"
#include "menu.h"

int main(void) {
    /* Define encoding para melhor compatibilidade com acentuacao no console */
    #ifdef _WIN32
        /* Opcional: configuracao de console Windows se necessario */
    #endif

    printf("Iniciando Sistema de Gerenciamento de Senhas...\n");

    /* Inicializacao do sistema e de suas estruturas de dados */
    SistemaAtendimento sistema;
    sistema_inicializar(&sistema);

    /* Executa o loop do menu interativo */
    executar_menu_principal(&sistema);

    /* Liberacao de recursos e memoria dinamica */
    sistema_destruir(&sistema);

    return EXIT_SUCCESS;
}
