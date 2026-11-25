/*
 * ============================================================================
 * PROJETO: Sistema de Gerenciamento de Senhas de Atendimento Clinico
 * DISCIPLINA: Algoritmos e Programacao / Engenharia de Software
 * BIBLIOTECA UTILIZADA: Apenas <stdio.h>
 * ============================================================================
 */

#include <stdio.h>

#define MAX_FILA 100

// Estrutura para armazenar os dados do paciente e sua senha
typedef struct {
    char codigo[10];     // Ex: "U001", "P001", "N001"
    char categoria;      // 'U' = Urgente, 'P' = Preferencial, 'N' = Normal
    char genero;         // 'M' ou 'F'
    int idade;
    int gestante;        // 1 = Sim, 0 = Nao
    int lactante;        // 1 = Sim, 0 = Nao
    int especial;        // 1 = Sim, 0 = Nao
    int obeso;           // 1 = Sim, 0 = Nao
} Paciente;

// Filas para cada categoria de atendimento
Paciente filaUrgente[MAX_FILA];
int totalUrgente = 0;

Paciente filaPreferencial[MAX_FILA];
int totalPreferencial = 0;

Paciente filaNormal[MAX_FILA];
int totalNormal = 0;

// Contadores sequenciais para gerar as senhas
int contadorU = 1;
int contadorP = 1;
int contadorN = 1;

// Variavel para controlar quem esta sendo atendido no momento
int temPacienteEmAtendimento = 0;
Paciente pacienteAtual;

// Funcao propria para converter letras minusculas em maiusculas (sem ctype.h)
char paraMaiusculo(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    }
    return c;
}

// Funcao auxiliar para limpar o buffer do teclado
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Funcao para pausar e esperar o usuario apertar ENTER
void pausar() {
    printf("\nPressione [ENTER] para continuar...");
    limparBuffer();
}

// Funcao que classifica a prioridade com base nas regras do problema
char classificarPaciente(Paciente p) {
    // 1. Urgente: idade superior a 80 anos
    if (p.idade > 80) {
        return 'U';
    }

    // 2. Preferencial: gestante, lactante, necessidades especiais, obesidade
    if (p.gestante == 1 || p.lactante == 1 || p.especial == 1 || p.obeso == 1) {
        return 'P';
    }

    // Preferencial por idade: Mulheres com 60+ anos e Homens com 65+ anos
    if (p.genero == 'F' && p.idade >= 60) {
        return 'P';
    }
    if (p.genero == 'M' && p.idade >= 65) {
        return 'P';
    }

    // 3. Normal: demais casos
    return 'N';
}

// Funcao para ler uma resposta de Sim ou Nao (S/N)
int lerSimNao(char mensagem[]) {
    char resposta;
    while (1) {
        printf("%s (S/N): ", mensagem);
        scanf(" %c", &resposta);
        limparBuffer();
        resposta = paraMaiusculo(resposta);

        if (resposta == 'S') {
            return 1;
        } else if (resposta == 'N') {
            return 0;
        } else {
            printf("[Opcao invalida] Digite apenas 'S' para Sim ou 'N' para Nao.\n");
        }
    }
}

// Modulo 1 e 2: Cadastrar paciente, classificar e emitir a senha
void registrarNovaSenha() {
    Paciente p;

    printf("\n--- REGISTRO DE SENHA / TRIAGEM ---\n");

    // Validacao do Genero
    while (1) {
        printf("Informe o genero do paciente (M/F): ");
        scanf(" %c", &p.genero);
        limparBuffer();
        p.genero = paraMaiusculo(p.genero);

        if (p.genero == 'M' || p.genero == 'F') {
            break;
        }
        printf("[Entrada invalida] Digite apenas 'M' para Masculino ou 'F' para Feminino.\n");
    }

    // Validacao da Idade
    while (1) {
        printf("Informe a idade do paciente: ");
        if (scanf("%d", &p.idade) == 1 && p.idade >= 0 && p.idade <= 125) {
            limparBuffer();
            break;
        }
        limparBuffer();
        printf("[Entrada invalida] Idade deve ser um numero entre 0 e 125.\n");
    }

    // Gestante e Lactante (apenas se for do genero feminino)
    if (p.genero == 'F') {
        p.gestante = lerSimNao("A paciente esta gestante?");
        p.lactante = lerSimNao("A paciente e lactante?");
    } else {
        p.gestante = 0;
        p.lactante = 0;
    }

    // Necessidade especial e obesidade
    p.especial = lerSimNao("Possui necessidade especial?");
    p.obeso = lerSimNao("Possui condicao de obesidade?");

    // Classifica automaticamente
    p.categoria = classificarPaciente(p);

    // Gera o codigo sequencial e guarda na fila certa
    if (p.categoria == 'U') {
        if (totalUrgente < MAX_FILA) {
            sprintf(p.codigo, "U%03d", contadorU++);
            filaUrgente[totalUrgente] = p;
            totalUrgente++;
            printf("\n>> SENHA GERADA COM SUCESSO: [ %s ] (Categoria: URGENTE)\n", p.codigo);
        } else {
            printf("\n[Erro] A fila de Urgentes esta cheia!\n");
        }
    } else if (p.categoria == 'P') {
        if (totalPreferencial < MAX_FILA) {
            sprintf(p.codigo, "P%03d", contadorP++);
            filaPreferencial[totalPreferencial] = p;
            totalPreferencial++;
            printf("\n>> SENHA GERADA COM SUCESSO: [ %s ] (Categoria: PREFERENCIAL)\n", p.codigo);
        } else {
            printf("\n[Erro] A fila de Preferenciais esta cheia!\n");
        }
    } else {
        if (totalNormal < MAX_FILA) {
            sprintf(p.codigo, "N%03d", contadorN++);
            filaNormal[totalNormal] = p;
            totalNormal++;
            printf("\n>> SENHA GERADA COM SUCESSO: [ %s ] (Categoria: NORMAL)\n", p.codigo);
        } else {
            printf("\n[Erro] A fila de Normais esta cheia!\n");
        }
    }
}

// Modulo 3.1: Consultar a proxima senha sem chamar
void consultarProximaSenha() {
    printf("\n--- CONSULTAR PROXIMA SENHA ---\n");

    // Regra de precedencia: Urgente > Preferencial > Normal
    if (totalUrgente > 0) {
        printf("A proxima senha a ser chamada e: [ %s ] (Categoria: URGENTE)\n", filaUrgente[0].codigo);
        printf("Paciente com %d anos | Genero: %c\n", filaUrgente[0].idade, filaUrgente[0].genero);
    } else if (totalPreferencial > 0) {
        printf("A proxima senha a ser chamada e: [ %s ] (Categoria: PREFERENCIAL)\n", filaPreferencial[0].codigo);
        printf("Paciente com %d anos | Genero: %c\n", filaPreferencial[0].idade, filaPreferencial[0].genero);
    } else if (totalNormal > 0) {
        printf("A proxima senha a ser chamada e: [ %s ] (Categoria: NORMAL)\n", filaNormal[0].codigo);
        printf("Paciente com %d anos | Genero: %c\n", filaNormal[0].idade, filaNormal[0].genero);
    } else {
        printf("Nao ha pacientes aguardando em nenhuma das filas.\n");
    }
}

// Modulo 3.2: Registrar chamada de paciente (desenfileira da fila)
void chamarPaciente() {
    int i;
    printf("\n--- REGISTRAR CHAMADA DE PACIENTE ---\n");

    if (temPacienteEmAtendimento == 1) {
        printf("[AVISO] Ja existe um paciente em atendimento: Senha [ %s ]\n", pacienteAtual.codigo);
        printf("Finalize o atendimento atual antes de chamar outro paciente.\n");
        return;
    }

    // 1. Tenta chamar da fila Urgente
    if (totalUrgente > 0) {
        pacienteAtual = filaUrgente[0];
        temPacienteEmAtendimento = 1;

        // Desloca os outros pacientes da fila para a esquerda (FIFO)
        for (i = 0; i < totalUrgente - 1; i++) {
            filaUrgente[i] = filaUrgente[i + 1];
        }
        totalUrgente--;

        printf(">> CHAMANDO AGORA: Senha [ %s ] (URGENTE)\n", pacienteAtual.codigo);
        printf("Paciente: Genero %c, Idade %d anos\n", pacienteAtual.genero, pacienteAtual.idade);
        return;
    }

    // 2. Tenta chamar da fila Preferencial
    if (totalPreferencial > 0) {
        pacienteAtual = filaPreferencial[0];
        temPacienteEmAtendimento = 1;

        for (i = 0; i < totalPreferencial - 1; i++) {
            filaPreferencial[i] = filaPreferencial[i + 1];
        }
        totalPreferencial--;

        printf(">> CHAMANDO AGORA: Senha [ %s ] (PREFERENCIAL)\n", pacienteAtual.codigo);
        printf("Paciente: Genero %c, Idade %d anos\n", pacienteAtual.genero, pacienteAtual.idade);
        return;
    }

    // 3. Tenta chamar da fila Normal
    if (totalNormal > 0) {
        pacienteAtual = filaNormal[0];
        temPacienteEmAtendimento = 1;

        for (i = 0; i < totalNormal - 1; i++) {
            filaNormal[i] = filaNormal[i + 1];
        }
        totalNormal--;

        printf(">> CHAMANDO AGORA: Senha [ %s ] (NORMAL)\n", pacienteAtual.codigo);
        printf("Paciente: Genero %c, Idade %d anos\n", pacienteAtual.genero, pacienteAtual.idade);
        return;
    }

    printf("Nao ha pacientes aguardando chamada no momento.\n");
}

// Modulo 3.3: Identificar a senha em atendimento
void identificarEmAtendimento() {
    printf("\n--- PACIENTE EM ATENDIMENTO ---\n");

    if (temPacienteEmAtendimento == 1) {
        printf("Senha em atendimento: [ %s ]\n", pacienteAtual.codigo);
        printf("Categoria: ");
        if (pacienteAtual.categoria == 'U') printf("Urgente\n");
        else if (pacienteAtual.categoria == 'P') printf("Preferencial\n");
        else printf("Normal\n");

        printf("Genero: %c | Idade: %d anos\n", pacienteAtual.genero, pacienteAtual.idade);
    } else {
        printf("Nenhum paciente esta sendo atendido no momento.\n");
    }
}

// Modulo 3.4 e 3.5: Finalizar atendimento e atualizar fila
void finalizarAtendimento() {
    printf("\n--- FINALIZAR ATENDIMENTO ---\n");

    if (temPacienteEmAtendimento == 1) {
        printf("Atendimento da senha [ %s ] finalizado com sucesso!\n", pacienteAtual.codigo);
        printf("O paciente foi liberado e o consultorio esta livre.\n");
        temPacienteEmAtendimento = 0;
    } else {
        printf("[Aviso] Nao ha nenhum atendimento em andamento para finalizar.\n");
    }
}

// Modulo 4: Painel textual de chamada
void exibirPainel() {
    int i;

    printf("\n=======================================================\n");
    printf("              PAINEL DE CHAMADA CLINICA                \n");
    printf("=======================================================\n");

    // Senha atualmente chamada
    if (temPacienteEmAtendimento == 1) {
        printf(" SENHA CHAMADA ATUALMENTE: [ %s ]\n", pacienteAtual.codigo);
        printf(" CATEGORIA: ");
        if (pacienteAtual.categoria == 'U') printf("Urgente\n");
        else if (pacienteAtual.categoria == 'P') printf("Preferencial\n");
        else printf("Normal\n");
    } else {
        printf(" SENHA CHAMADA ATUALMENTE: [ Nenhuma no momento ]\n");
    }
    printf("-------------------------------------------------------\n");

    // Proximas senhas aguardando
    printf(" FILAS DE ESPERA:\n");

    // Urgentes
    printf("  [U] Urgentes (%d): ", totalUrgente);
    if (totalUrgente == 0) {
        printf("(Vazia)\n");
    } else {
        for (i = 0; i < totalUrgente; i++) {
            printf("%s ", filaUrgente[i].codigo);
        }
        printf("\n");
    }

    // Preferenciais
    printf("  [P] Preferenciais (%d): ", totalPreferencial);
    if (totalPreferencial == 0) {
        printf("(Vazia)\n");
    } else {
        for (i = 0; i < totalPreferencial; i++) {
            printf("%s ", filaPreferencial[i].codigo);
        }
        printf("\n");
    }

    // Normais
    printf("  [N] Normais (%d): ", totalNormal);
    if (totalNormal == 0) {
        printf("(Vazia)\n");
    } else {
        for (i = 0; i < totalNormal; i++) {
            printf("%s ", filaNormal[i].codigo);
        }
        printf("\n");
    }

    printf("-------------------------------------------------------\n");
    printf(" TOTAL DE PACIENTES AGUARDANDO: %d\n", totalUrgente + totalPreferencial + totalNormal);
    printf("=======================================================\n");
}

// Funcao Principal (main): apenas exibe o menu e chama as funcoes
int main() {
    int opcao;

    do {
        printf("\n=======================================================\n");
        printf("        CLINICA MEDICA - SISTEMA DE SENHAS             \n");
        printf("=======================================================\n");
        printf(" 1 - Cadastrar paciente e emitir senha\n");
        printf(" 2 - Consultar proxima senha a ser chamada\n");
        printf(" 3 - Registrar chamada de paciente\n");
        printf(" 4 - Identificar senha em atendimento\n");
        printf(" 5 - Finalizar atendimento atual\n");
        printf(" 6 - Exibir painel de chamada\n");
        printf(" 0 - Sair do programa\n");
        printf("=======================================================\n");
        printf("Digite a opcao desejada: ");

        if (scanf("%d", &opcao) != 1) {
            limparBuffer();
            printf("[Erro] Opcao invalida. Digite um numero de 0 a 6.\n");
            continue;
        }
        limparBuffer();

        switch (opcao) {
            case 1:
                registrarNovaSenha();
                pausar();
                break;
            case 2:
                consultarProximaSenha();
                pausar();
                break;
            case 3:
                chamarPaciente();
                pausar();
                break;
            case 4:
                identificarEmAtendimento();
                pausar();
                break;
            case 5:
                finalizarAtendimento();
                pausar();
                break;
            case 6:
                exibirPainel();
                pausar();
                break;
            case 0:
                printf("\nEncerrando o sistema... Ate logo!\n");
                break;
            default:
                printf("[Erro] Opcao invalida! Escolha entre 0 e 6.\n");
                pausar();
                break;
        }

    } while (opcao != 0);

    return 0;
}
