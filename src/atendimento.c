#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atendimento.h"

void sistema_inicializar(SistemaAtendimento *s) {
    if (s == NULL) {
        return;
    }

    fila_inicializar(&s->fila_urgente);
    fila_inicializar(&s->fila_preferencial);
    fila_inicializar(&s->fila_normal);

    s->proximo_numero_urgente = 1;
    s->proximo_numero_preferencial = 1;
    s->proximo_numero_normal = 1;

    s->ha_atendimento_ativo = 0;
    memset(&s->senha_em_atendimento, 0, sizeof(Senha));

    s->total_atendidos = 0;
}

void sistema_destruir(SistemaAtendimento *s) {
    if (s == NULL) {
        return;
    }

    fila_destruir(&s->fila_urgente);
    fila_destruir(&s->fila_preferencial);
    fila_destruir(&s->fila_normal);
}

Senha sistema_emitir_senha(SistemaAtendimento *s, const DadosPaciente *paciente) {
    Senha nova_senha;
    memset(&nova_senha, 0, sizeof(Senha));

    if (s == NULL || paciente == NULL) {
        return nova_senha;
    }

    /* 1. Classificação automática baseada nas regras */
    CategoriaSenha cat = classificar_paciente(paciente);

    /* 2. Emissão de número sequencial independente por categoria */
    int num_seq = 1;
    switch (cat) {
        case CAT_URGENTE:
            num_seq = s->proximo_numero_urgente++;
            nova_senha = criar_senha(cat, num_seq, paciente);
            fila_enfileirar(&s->fila_urgente, nova_senha);
            break;

        case CAT_PREFERENCIAL:
            num_seq = s->proximo_numero_preferencial++;
            nova_senha = criar_senha(cat, num_seq, paciente);
            fila_enfileirar(&s->fila_preferencial, nova_senha);
            break;

        case CAT_NORMAL:
        default:
            num_seq = s->proximo_numero_normal++;
            nova_senha = criar_senha(cat, num_seq, paciente);
            fila_enfileirar(&s->fila_normal, nova_senha);
            break;
    }

    return nova_senha;
}

int sistema_consultar_proxima_senha(const SistemaAtendimento *s, Senha *proxima) {
    if (s == NULL) {
        return 0;
    }

    /* 1. Precedência Urgente */
    if (!fila_esta_vazia(&s->fila_urgente)) {
        return fila_espiar(&s->fila_urgente, proxima);
    }

    /* 2. Precedência Preferencial */
    if (!fila_esta_vazia(&s->fila_preferencial)) {
        return fila_espiar(&s->fila_preferencial, proxima);
    }

    /* 3. Precedência Normal */
    if (!fila_esta_vazia(&s->fila_normal)) {
        return fila_espiar(&s->fila_normal, proxima);
    }

    /* Nenhuma senha em espera */
    return 0;
}

int sistema_chamar_proximo(SistemaAtendimento *s, Senha *chamada) {
    if (s == NULL) {
        return 0;
    }

    /* Se já houver um atendimento em curso, não pode sobrescrever diretamente */
    if (s->ha_atendimento_ativo) {
        return -1; /* Indica atendimento em andamento */
    }

    Senha proxima;
    int removido = 0;

    /* Prioridade 1: Urgente */
    if (!fila_esta_vazia(&s->fila_urgente)) {
        removido = fila_desenfileirar(&s->fila_urgente, &proxima);
    }
    /* Prioridade 2: Preferencial */
    else if (!fila_esta_vazia(&s->fila_preferencial)) {
        removido = fila_desenfileirar(&s->fila_preferencial, &proxima);
    }
    /* Prioridade 3: Normal */
    else if (!fila_esta_vazia(&s->fila_normal)) {
        removido = fila_desenfileirar(&s->fila_normal, &proxima);
    }

    if (removido) {
        s->senha_em_atendimento = proxima;
        s->ha_atendimento_ativo = 1;
        if (chamada != NULL) {
            *chamada = proxima;
        }
        return 1;
    }

    return 0; /* Todas as filas vazias */
}

int sistema_obter_em_atendimento(const SistemaAtendimento *s, Senha *atual) {
    if (s == NULL || !s->ha_atendimento_ativo) {
        return 0;
    }

    if (atual != NULL) {
        *atual = s->senha_em_atendimento;
    }
    return 1;
}

int sistema_finalizar_atendimento(SistemaAtendimento *s, Senha *finalizada) {
    if (s == NULL || !s->ha_atendimento_ativo) {
        return 0;
    }

    if (finalizada != NULL) {
        *finalizada = s->senha_em_atendimento;
    }

    /* Registra no histórico de atendimentos finalizados */
    if (s->total_atendidos < MAX_HISTORICO) {
        s->historico_atendidos[s->total_atendidos++] = s->senha_em_atendimento;
    }

    /* Libera o status de atendimento ativo */
    s->ha_atendimento_ativo = 0;
    memset(&s->senha_em_atendimento, 0, sizeof(Senha));

    return 1;
}

void sistema_exibir_painel(const SistemaAtendimento *s) {
    if (s == NULL) {
        return;
    }

    int qtd_urg = fila_tamanho(&s->fila_urgente);
    int qtd_pref = fila_tamanho(&s->fila_preferencial);
    int qtd_norm = fila_tamanho(&s->fila_normal);
    int qtd_total = qtd_urg + qtd_pref + qtd_norm;

    char lista_urg[128], lista_pref[128], lista_norm[128];
    fila_formatar_lista(&s->fila_urgente, lista_urg, sizeof(lista_urg), 5);
    fila_formatar_lista(&s->fila_preferencial, lista_pref, sizeof(lista_pref), 5);
    fila_formatar_lista(&s->fila_normal, lista_norm, sizeof(lista_norm), 5);

    Senha proxima_geral;
    int tem_proxima = sistema_consultar_proxima_senha(s, &proxima_geral);

    printf("\n======================================================================\n");
    printf("                    PAINEL DE CHAMADA CLINICA                         \n");
    printf("======================================================================\n");

    /* 1. Senha atualmente chamada e categoria */
    if (s->ha_atendimento_ativo) {
        printf("  >> SENHA ATUALMENTE CHAMADA: [ %s ]\n", s->senha_em_atendimento.codigo);
        printf("  >> CATEGORIA:               %s\n", categoria_para_string(s->senha_em_atendimento.categoria));
        printf("  >> DETALHES DO PACIENTE:    Idade: %d anos | Genero: %c\n",
               s->senha_em_atendimento.paciente.idade,
               s->senha_em_atendimento.paciente.genero);
    } else {
        printf("  >> SENHA ATUALMENTE CHAMADA: [ NENHUMA NO MOMENTO ]\n");
        printf("  >> STATUS:                  Aguardando chamada de paciente\n");
    }
    printf("----------------------------------------------------------------------\n");

    /* 2. Próxima senha a ser chamada */
    if (tem_proxima) {
        printf("  * PROXIMA DA FILA:          %s (%s)\n",
               proxima_geral.codigo,
               categoria_para_string(proxima_geral.categoria));
    } else {
        printf("  * PROXIMA DA FILA:          Nao ha pacientes aguardando\n");
    }
    printf("----------------------------------------------------------------------\n");

    /* 3. Próximas senhas aguardando chamada por categoria */
    printf("  FILAS DE ESPERA (ordem de atendimento FIFO por categoria):\n");
    printf("  [U] Urgentes:     %-35s (Aguardando: %d)\n", lista_urg, qtd_urg);
    printf("  [P] Preferencias: %-35s (Aguardando: %d)\n", lista_pref, qtd_pref);
    printf("  [N] Normais:      %-35s (Aguardando: %d)\n", lista_norm, qtd_norm);
    printf("----------------------------------------------------------------------\n");

    /* 4. Quantidade de pacientes em espera por categoria e total */
    printf("  RESUMO DE ESPERA:\n");
    printf("  - Urgentes:     %d paciente(s)\n", qtd_urg);
    printf("  - Preferenciais:%d paciente(s)\n", qtd_pref);
    printf("  - Normais:      %d paciente(s)\n", qtd_norm);
    printf("  - TOTAL NA FILA:%d paciente(s)\n", qtd_total);
    printf("======================================================================\n\n");
}

void sistema_exibir_relatorio(const SistemaAtendimento *s) {
    if (s == NULL) {
        return;
    }

    printf("\n======================================================================\n");
    printf("              RELATORIO GERAL DO SISTEMA DE ATENDIMENTO               \n");
    printf("======================================================================\n");

    printf("ESTATISTICAS DE EMISSAO:\n");
    printf("  - Senhas Urgentes emitidas:      %d\n", s->proximo_numero_urgente - 1);
    printf("  - Senhas Preferenciais emitidas: %d\n", s->proximo_numero_preferencial - 1);
    printf("  - Senhas Normais emitidas:       %d\n", s->proximo_numero_normal - 1);
    printf("  - Total emitido:                 %d\n",
           (s->proximo_numero_urgente - 1) + 
           (s->proximo_numero_preferencial - 1) + 
           (s->proximo_numero_normal - 1));

    printf("\nESTADO ATUAL:\n");
    printf("  - Pacientes em espera:           %d\n",
           fila_tamanho(&s->fila_urgente) + 
           fila_tamanho(&s->fila_preferencial) + 
           fila_tamanho(&s->fila_normal));
    printf("  - Paciente em atendimento:       %s\n",
           s->ha_atendimento_ativo ? s->senha_em_atendimento.codigo : "Nenhum");
    printf("  - Total de atendimentos concluidos: %d\n", s->total_atendidos);

    if (s->total_atendidos > 0) {
        printf("\nHISTORICO DE SENHAS CONCLUIDAS (nao retornam a fila):\n");
        for (int i = 0; i < s->total_atendidos; i++) {
            printf("  [%02d] Senha: %-5s | Categoria: %-12s | Idade: %3d | Genero: %c\n",
                   i + 1,
                   s->historico_atendidos[i].codigo,
                   categoria_para_string(s->historico_atendidos[i].categoria),
                   s->historico_atendidos[i].paciente.idade,
                   s->historico_atendidos[i].paciente.genero);
        }
    }
    printf("======================================================================\n\n");
}
