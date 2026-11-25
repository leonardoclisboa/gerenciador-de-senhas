#ifndef MENU_H
#define MENU_H

#include "atendimento.h"

/*
 * ============================================================================
 * Módulo: menu.h
 * Responsabilidade: Interface de interação com o usuário via terminal,
 *                   apresentação de menu de opções, captura e validação de
 *                   comandos e chamadas para os módulos de atendimento e triagem.
 * ============================================================================
 */

/*
 * Executa o loop principal do menu interativo do sistema.
 */
void executar_menu_principal(SistemaAtendimento *sistema);

/*
 * Pausa a execução e aguarda o pressionamento de Enter pelo usuário.
 */
void pausar(void);

#endif /* MENU_H */
