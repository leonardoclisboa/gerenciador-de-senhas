#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "menu.h"

/*
 * Limpa o buffer de entrada do teclado.
 */
static void limpar_buffer_menu(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta caracteres */
    }
}

void pausar(void) {
    printf("\nPressione [ENTER] para continuar...");
    fflush(stdout);
    limpar_buffer_menu();
}

/*
 * Lê uma opção numérica inteira no menu entre min e max.
 */
static int ler_opcao_menu(int min, int max) {
    int op;
    int lido;

    while (1) {
        printf("Escolha uma opcao (%d-%d): ", min, max);
        lido = scanf("%d", &op);
        limpar_buffer_menu();

        if (lido == 1 && op >= min && op <= max) {
            return op;
        }
        printf("[ERRO] Opcao invalida! Digite um numero entre %d e %d.\n", min, max);
    }
}

/*
 * Exibe a justificativa da classificação clínica.
 */
static void explicar_classificacao(const DadosPaciente *p, CategoriaSenha cat) {
    printf("Motivo da classificacao: ");
    if (cat == CAT_URGENTE) {
        printf("Idade (%d anos) superior a 80 anos (Prioridade Urgente).\n", p->idade);
    } else if (cat == CAT_PREFERENCIAL) {
        if (p->gestante) printf("Paciente gestante; ");
        if (p->lactante) printf("Paciente lactante; ");
        if (p->necessidadeEspecial) printf("Pessoa com necessidade especial; ");
        if (p->obesidade) printf("Condicao de obesidade; ");
        if (p->genero == 'F' && p->idade >= 60) printf("Mulher com 60 anos ou mais (%d anos); ", p->idade);
        if (p->genero == 'M' && p->idade >= 65) printf("Homem com 65 anos ou mais (%d anos); ", p->idade);
        printf("(Prioridade Preferencial).\n");
    } else {
        printf("Nenhuma condicao de prioridade cadastrada (Fila Normal).\n");
    }
}

/*
 * Ação 1: Emitir nova senha através de triagem.
 */
static void acao_emitir_senha(SistemaAtendimento *s) {
    DadosPaciente paciente;
    cadastrar_paciente_interativo(&paciente);

    Senha emitida = sistema_emitir_senha(s, &paciente);

    printf("\n=======================================================\n");
    printf("               SENHA EMITIDA COM SUCESSO!              \n");
    printf("=======================================================\n");
    printf("  >> SENHA GERADA:   [ %s ]\n", emitida.codigo);
    printf("  >> CATEGORIA:      %s\n", categoria_para_string(emitida.categoria));
    printf("  >> ");
    explicar_classificacao(&paciente, emitida.categoria);
    printf("  >> Detalhes: Genero %c | Idade: %d anos\n", paciente.genero, paciente.idade);
    printf("=======================================================\n");
}

/*
 * Ação 2: Consultar a próxima senha da fila sem alterar estado.
 */
static void acao_consultar_proxima(const SistemaAtendimento *s) {
    Senha proxima;
    printf("\n--- CONSULTA DE PROXIMA SENHA ---\n");

    if (sistema_consultar_proxima_senha(s, &proxima)) {
        printf("A proxima senha a ser chamada e:\n");
        printf("  >> Codigo:    [ %s ]\n", proxima.codigo);
        printf("  >> Categoria: %s\n", categoria_para_string(proxima.categoria));
        printf("  >> Idade:     %d anos | Genero: %c\n", proxima.paciente.idade, proxima.paciente.genero);
    } else {
        printf("[INFO] Nao ha nenhuma senha aguardando na fila no momento.\n");
    }
}

/*
 * Ação 3: Registrar chamada do próximo paciente.
 */
static void acao_chamar_proximo(SistemaAtendimento *s) {
    Senha chamada;
    printf("\n--- REGISTRO DE CHAMADA DE PACIENTE ---\n");

    int resultado = sistema_chamar_proximo(s, &chamada);

    if (resultado == 1) {
        printf("=======================================================\n");
        printf("  *** CHAMANDO PACIENTE: SENHA [ %s ] ***\n", chamada.codigo);
        printf("=======================================================\n");
        printf("  >> Categoria: %s\n", categoria_para_string(chamada.categoria));
        printf("  >> Paciente:  Idade %d anos | Genero: %c\n",
               chamada.paciente.idade, chamada.paciente.genero);
        printf("  >> Status:    Em atendimento no consultorio.\n");
    } else if (resultado == -1) {
        Senha atual;
        sistema_obter_em_atendimento(s, &atual);
        printf("[ATENCAO] Ja existe um paciente em atendimento no momento!\n");
        printf("  >> Senha em atendimento atual: [ %s ] (%s)\n",
               atual.codigo, categoria_para_string(atual.categoria));
        printf("Deseja finalizar o atendimento atual e chamar o proximo? (S/N): ");

        char resp;
        if (scanf(" %c", &resp) == 1) {
            limpar_buffer_menu();
            resp = (char)toupper((unsigned char)resp);
            if (resp == 'S') {
                Senha fin;
                sistema_finalizar_atendimento(s, &fin);
                printf("[SUCESSO] Atendimento da senha %s finalizado.\n", fin.codigo);

                /* Tenta chamar novamente */
                resultado = sistema_chamar_proximo(s, &chamada);
                if (resultado == 1) {
                    printf("=======================================================\n");
                    printf("  *** CHAMANDO PACIENTE: SENHA [ %s ] ***\n", chamada.codigo);
                    printf("=======================================================\n");
                    printf("  >> Categoria: %s\n", categoria_para_string(chamada.categoria));
                    printf("  >> Paciente:  Idade %d anos | Genero: %c\n",
                           chamada.paciente.idade, chamada.paciente.genero);
                } else {
                    printf("[INFO] Filas de espera estao vazias agora.\n");
                }
            } else {
                printf("[INFO] Chamada cancelada. O paciente atual continua em atendimento.\n");
            }
        } else {
            limpar_buffer_menu();
        }
    } else {
        printf("[INFO] Nao ha pacientes em espera em nenhuma das filas.\n");
    }
}

/*
 * Ação 4: Identificar a senha atualmente em atendimento.
 */
static void acao_identificar_em_atendimento(const SistemaAtendimento *s) {
    Senha atual;
    printf("\n--- IDENTIFICACAO DE ATENDIMENTO EM CURSO ---\n");

    if (sistema_obter_em_atendimento(s, &atual)) {
        printf("Paciente atualmente em atendimento:\n");
        imprimir_detalhes_senha(&atual);
    } else {
        printf("[INFO] No momento, nenhum paciente esta sendo atendido.\n");
    }
}

/*
 * Ação 5: Finalizar atendimento atual.
 */
static void acao_finalizar_atendimento(SistemaAtendimento *s) {
    Senha finalizada;
    printf("\n--- FINALIZACAO DE ATENDIMENTO ---\n");

    if (sistema_finalizar_atendimento(s, &finalizada)) {
        printf("=======================================================\n");
        printf("[SUCESSO] Atendimento da senha [ %s ] FINALIZADO!\n", finalizada.codigo);
        printf("  >> Categoria: %s\n", categoria_para_string(finalizada.categoria));
        printf("  >> O paciente foi retirado do consultorio e registrado no historico.\n");
        printf("  >> Senha nao retornara a fila.\n");
        printf("=======================================================\n");
    } else {
        printf("[AVISO] Nao ha nenhum atendimento em andamento para ser finalizado.\n");
    }
}

void executar_menu_principal(SistemaAtendimento *sistema) {
    int opcao;

    do {
        printf("\n=======================================================\n");
        printf("     SISTEMA DE GERENCIAMENTO DE SENHAS CLINICAS       \n");
        printf("=======================================================\n");
        printf(" [1] Registrar e Emitir Nova Senha (Triagem)\n");
        printf(" [2] Consultar Proxima Senha a Ser Chamada\n");
        printf(" [3] Registrar Chamada de Paciente (Chamar Proximo)\n");
        printf(" [4] Identificar Senha em Atendimento\n");
        printf(" [5] Finalizar Atendimento Atual\n");
        printf(" [6] Visualizar Painel de Chamada (Mural Textual)\n");
        printf(" [7] Relatorio Geral e Historico de Atendimentos\n");
        printf(" [0] Sair do Sistema\n");
        printf("=======================================================\n");

        opcao = ler_opcao_menu(0, 7);

        switch (opcao) {
            case 1:
                acao_emitir_senha(sistema);
                pausar();
                break;
            case 2:
                acao_consultar_proxima(sistema);
                pausar();
                break;
            case 3:
                acao_chamar_proximo(sistema);
                pausar();
                break;
            case 4:
                acao_identificar_em_atendimento(sistema);
                pausar();
                break;
            case 5:
                acao_finalizar_atendimento(sistema);
                pausar();
                break;
            case 6:
                sistema_exibir_painel(sistema);
                pausar();
                break;
            case 7:
                sistema_exibir_relatorio(sistema);
                pausar();
                break;
            case 0:
                printf("\nEncerrando o Sistema de Gerenciamento de Senhas...\n");
                printf("Obrigado por utilizar o sistema!\n\n");
                break;
            default:
                break;
        }

    } while (opcao != 0);
}
