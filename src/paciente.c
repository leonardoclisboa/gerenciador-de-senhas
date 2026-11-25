#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "paciente.h"

/*
 * Função utilitária para limpar o buffer de entrada do teclado (stdin).
 */
static void limpar_buffer_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta caracteres residuais */
    }
}

/*
 * Lê e valida um número inteiro dentro de um intervalo [min, max].
 */
static int ler_inteiro_validado(const char *prompt, int min, int max) {
    int valor;
    int valido = 0;

    do {
        printf("%s", prompt);
        if (scanf("%d", &valor) == 1) {
            limpar_buffer_entrada();
            if (valor >= min && valor <= max) {
                valido = 1;
            } else {
                printf("[ERRO] Valor fora do intervalo permitido (%d a %d). Tente novamente.\n", min, max);
            }
        } else {
            limpar_buffer_entrada();
            printf("[ERRO] Entrada invalida. Digite apenas numeros inteiros.\n");
        }
    } while (!valido);

    return valor;
}

/*
 * Lê e valida uma resposta Sim ('S') ou Nao ('N').
 */
static int ler_sim_nao(const char *prompt) {
    char resposta;
    int valido = 0;

    do {
        printf("%s", prompt);
        if (scanf(" %c", &resposta) == 1) {
            limpar_buffer_entrada();
            resposta = (char)toupper((unsigned char)resposta);
            if (resposta == 'S') {
                return 1;
            } else if (resposta == 'N') {
                return 0;
            } else {
                printf("[ERRO] Opcao invalida. Digite 'S' para Sim ou 'N' para Nao.\n");
            }
        } else {
            limpar_buffer_entrada();
            printf("[ERRO] Entrada invalida. Tente novamente.\n");
        }
    } while (!valido);

    return 0;
}

/*
 * Lê e valida o gênero do paciente ('M' ou 'F').
 */
static char ler_genero(const char *prompt) {
    char g;
    int valido = 0;

    do {
        printf("%s", prompt);
        if (scanf(" %c", &g) == 1) {
            limpar_buffer_entrada();
            g = (char)toupper((unsigned char)g);
            if (g == 'M' || g == 'F') {
                valido = 1;
            } else {
                printf("[ERRO] Genero invalido. Digite 'M' para Masculino ou 'F' para Feminino.\n");
            }
        } else {
            limpar_buffer_entrada();
            printf("[ERRO] Entrada invalida. Tente novamente.\n");
        }
    } while (!valido);

    return g;
}

const char* categoria_para_string(CategoriaSenha cat) {
    switch (cat) {
        case CAT_URGENTE:
            return "Urgente";
        case CAT_PREFERENCIAL:
            return "Preferencial";
        case CAT_NORMAL:
            return "Normal";
        default:
            return "Desconhecida";
    }
}

CategoriaSenha classificar_paciente(const DadosPaciente *paciente) {
    if (paciente == NULL) {
        return CAT_NORMAL;
    }

    /* Regra 1: Urgente - Pacientes com idade superior a 80 anos */
    if (paciente->idade > 80) {
        return CAT_URGENTE;
    }

    /* Regra 2: Preferencial - Condições prioritárias */
    /* Gestantes, lactantes, pessoas com necessidades especiais, pessoas obesas */
    if (paciente->gestante || paciente->lactante || 
        paciente->necessidadeEspecial || paciente->obesidade) {
        return CAT_PREFERENCIAL;
    }

    /* Idade preferencial por gênero: Mulheres >= 60 anos ou Homens >= 65 anos */
    if (paciente->genero == 'F' && paciente->idade >= 60) {
        return CAT_PREFERENCIAL;
    }
    if (paciente->genero == 'M' && paciente->idade >= 65) {
        return CAT_PREFERENCIAL;
    }

    /* Regra 3: Normal - Sem condições de prioridade */
    return CAT_NORMAL;
}

void cadastrar_paciente_interativo(DadosPaciente *paciente) {
    if (paciente == NULL) {
        return;
    }

    printf("\n--- FORMULARIO DE TRIAGEM DO PACIENTE ---\n");

    /* 1. Gênero */
    paciente->genero = ler_genero("Informe o genero do paciente (M/F): ");

    /* 2. Idade */
    paciente->idade = ler_inteiro_validado("Informe a idade do paciente (0 a 130 anos): ", 0, 130);

    /* 3 e 4. Gestante e Lactante (aplicável quando paciente for do gênero feminino) */
    if (paciente->genero == 'F') {
        paciente->gestante = ler_sim_nao("A paciente esta gestante? (S/N): ");
        paciente->lactante = ler_sim_nao("A paciente e lactante? (S/N): ");
    } else {
        paciente->gestante = 0;
        paciente->lactante = 0;
    }

    /* 5. Necessidade especial */
    paciente->necessidadeEspecial = ler_sim_nao("Apresenta necessidade especial? (S/N): ");

    /* 6. Obesidade */
    paciente->obesidade = ler_sim_nao("Possui condicao de obesidade? (S/N): ");
}

Senha criar_senha(CategoriaSenha categoria, int numero_sequencial, const DadosPaciente *paciente) {
    Senha s;
    memset(&s, 0, sizeof(Senha));

    s.categoria = categoria;
    s.numero = numero_sequencial;
    if (paciente != NULL) {
        s.paciente = *paciente;
    }

    /* Formatação sequencial independente: ex: U001, P002, N003 */
    snprintf(s.codigo, sizeof(s.codigo), "%c%03d", (char)categoria, numero_sequencial);

    return s;
}

void imprimir_detalhes_senha(const Senha *s) {
    if (s == NULL) {
        return;
    }

    printf("  Senha: %-6s | Categoria: %-12s | Idade: %3d anos | Genero: %c\n",
           s->codigo,
           categoria_para_string(s->categoria),
           s->paciente.idade,
           s->paciente.genero);

    printf("  Condicoes: Gestante: %s | Lactante: %s | Nec. Especial: %s | Obesidade: %s\n",
           s->paciente.gestante ? "Sim" : "Nao",
           s->paciente.lactante ? "Sim" : "Nao",
           s->paciente.necessidadeEspecial ? "Sim" : "Nao",
           s->paciente.obesidade ? "Sim" : "Nao");
}
