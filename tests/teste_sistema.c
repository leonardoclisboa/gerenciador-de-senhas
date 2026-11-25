#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/paciente.h"
#include "../src/fila.h"
#include "../src/atendimento.h"

static void test_classificacao_regras(void) {
    printf("[TESTE] Regras de Classificacao...\n");

    DadosPaciente p;

    /* 1. Idade > 80 deve ser URGENTE */
    memset(&p, 0, sizeof(p));
    p.genero = 'M';
    p.idade = 81;
    assert(classificar_paciente(&p) == CAT_URGENTE);

    p.genero = 'F';
    p.idade = 90;
    p.gestante = 0;
    assert(classificar_paciente(&p) == CAT_URGENTE);

    /* 2. Preferenciais:
     * - Mulher >= 60
     * - Homem >= 65
     * - Gestante
     * - Lactante
     * - Necessidade Especial
     * - Obesidade
     */
    memset(&p, 0, sizeof(p));
    p.genero = 'F';
    p.idade = 60;
    assert(classificar_paciente(&p) == CAT_PREFERENCIAL);

    memset(&p, 0, sizeof(p));
    p.genero = 'M';
    p.idade = 65;
    assert(classificar_paciente(&p) == CAT_PREFERENCIAL);

    memset(&p, 0, sizeof(p));
    p.genero = 'F';
    p.idade = 25;
    p.gestante = 1;
    assert(classificar_paciente(&p) == CAT_PREFERENCIAL);

    memset(&p, 0, sizeof(p));
    p.genero = 'F';
    p.idade = 28;
    p.lactante = 1;
    assert(classificar_paciente(&p) == CAT_PREFERENCIAL);

    memset(&p, 0, sizeof(p));
    p.genero = 'M';
    p.idade = 30;
    p.necessidadeEspecial = 1;
    assert(classificar_paciente(&p) == CAT_PREFERENCIAL);

    memset(&p, 0, sizeof(p));
    p.genero = 'M';
    p.idade = 40;
    p.obesidade = 1;
    assert(classificar_paciente(&p) == CAT_PREFERENCIAL);

    /* 3. Normais */
    memset(&p, 0, sizeof(p));
    p.genero = 'M';
    p.idade = 64; /* Homem com 64 nao e preferencial */
    assert(classificar_paciente(&p) == CAT_NORMAL);

    memset(&p, 0, sizeof(p));
    p.genero = 'F';
    p.idade = 59; /* Mulher com 59 sem condicoes nao e preferencial */
    assert(classificar_paciente(&p) == CAT_NORMAL);

    printf("  -> Regras de classificacao aprovadas com sucesso!\n");
}

static void test_geracao_e_contadores_independentes(void) {
    printf("[TESTE] Numeracao Sequencial Independente...\n");

    SistemaAtendimento s;
    sistema_inicializar(&s);

    DadosPaciente p_u, p_p, p_n;
    memset(&p_u, 0, sizeof(p_u));
    p_u.genero = 'M'; p_u.idade = 85; /* Urgente */

    memset(&p_p, 0, sizeof(p_p));
    p_p.genero = 'M'; p_p.idade = 70; /* Preferencial */

    memset(&p_n, 0, sizeof(p_n));
    p_n.genero = 'M'; p_n.idade = 30; /* Normal */

    /* Emissao alternada */
    Senha s1 = sistema_emitir_senha(&s, &p_n); // N001
    Senha s2 = sistema_emitir_senha(&s, &p_p); // P001
    Senha s3 = sistema_emitir_senha(&s, &p_u); // U001
    Senha s4 = sistema_emitir_senha(&s, &p_n); // N002
    Senha s5 = sistema_emitir_senha(&s, &p_p); // P002
    Senha s6 = sistema_emitir_senha(&s, &p_u); // U002

    assert(strcmp(s1.codigo, "N001") == 0);
    assert(strcmp(s2.codigo, "P001") == 0);
    assert(strcmp(s3.codigo, "U001") == 0);
    assert(strcmp(s4.codigo, "N002") == 0);
    assert(strcmp(s5.codigo, "P002") == 0);
    assert(strcmp(s6.codigo, "U002") == 0);

    printf("  -> Numeracao sequencial independente aprovada (N001, P001, U001, etc.)!\n");
    sistema_destruir(&s);
}

static void test_precedencia_e_fifo(void) {
    printf("[TESTE] Precedencia de Chamada (U > P > N) e FIFO...\n");

    SistemaAtendimento s;
    sistema_inicializar(&s);

    DadosPaciente p_u, p_p, p_n;
    memset(&p_u, 0, sizeof(p_u));
    p_u.genero = 'M'; p_u.idade = 85;

    memset(&p_p, 0, sizeof(p_p));
    p_p.genero = 'M'; p_p.idade = 70;

    memset(&p_n, 0, sizeof(p_n));
    p_n.genero = 'M'; p_n.idade = 30;

    /* Cadastra na ordem: N001, P001, U001, N002, P002, U002 */
    sistema_emitir_senha(&s, &p_n); // N001
    sistema_emitir_senha(&s, &p_p); // P001
    sistema_emitir_senha(&s, &p_u); // U001
    sistema_emitir_senha(&s, &p_n); // N002
    sistema_emitir_senha(&s, &p_p); // P002
    sistema_emitir_senha(&s, &p_u); // U002

    /* Ordem esperada de chamada:
     * 1. U001
     * 2. U002
     * 3. P001
     * 4. P002
     * 5. N001
     * 6. N002
     */
    const char *ordem_esperada[] = {
        "U001", "U002", "P001", "P002", "N001", "N002"
    };

    for (int i = 0; i < 6; i++) {
        Senha prox;
        assert(sistema_consultar_proxima_senha(&s, &prox) == 1);
        assert(strcmp(prox.codigo, ordem_esperada[i]) == 0);

        Senha chamada;
        assert(sistema_chamar_proximo(&s, &chamada) == 1);
        assert(strcmp(chamada.codigo, ordem_esperada[i]) == 0);

        /* Em atendimento */
        Senha atual;
        assert(sistema_obter_em_atendimento(&s, &atual) == 1);
        assert(strcmp(atual.codigo, ordem_esperada[i]) == 0);

        /* Nao pode chamar outro sem finalizar */
        Senha tentativa;
        assert(sistema_chamar_proximo(&s, &tentativa) == -1);

        /* Finaliza */
        Senha finalizada;
        assert(sistema_finalizar_atendimento(&s, &finalizada) == 1);
        assert(strcmp(finalizada.codigo, ordem_esperada[i]) == 0);
    }

    /* Filas devem estar vazias agora */
    Senha vazia;
    assert(sistema_consultar_proxima_senha(&s, &vazia) == 0);
    assert(sistema_chamar_proximo(&s, &vazia) == 0);

    /* Total de atendidos = 6 */
    assert(s.total_atendidos == 6);

    printf("  -> Precedencia e FIFO aprovados com 100%% de conformidade!\n");
    sistema_destruir(&s);
}

int main(void) {
    printf("=== EXECUTANDO TESTES AUTOMATIZADOS DO SISTEMA ===\n\n");
    test_classificacao_regras();
    test_geracao_e_contadores_independentes();
    test_precedencia_e_fifo();
    printf("\n>>> TODOS OS TESTES PASSARAM COM SUCESSO! <<<\n");
    return 0;
}
