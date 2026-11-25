#ifndef ATENDIMENTO_H
#define ATENDIMENTO_H

#include "paciente.h"
#include "fila.h"

/*
 * ============================================================================
 * Módulo: atendimento.h
 * Responsabilidade: Gerenciamento do fluxo de atendimento clínico, controle
 *                   das 3 filas (Urgente, Preferencial, Normal), precedência
 *                   de chamadas, senha em atendimento e histórico.
 * ============================================================================
 */

#define MAX_HISTORICO 500

typedef struct {
    /* Filas por categoria de prioridade */
    Fila fila_urgente;
    Fila fila_preferencial;
    Fila fila_normal;

    /* Contadores sequenciais independentes */
    int proximo_numero_urgente;
    int proximo_numero_preferencial;
    int proximo_numero_normal;

    /* Estado do atendimento atual */
    int ha_atendimento_ativo;
    Senha senha_em_atendimento;

    /* Histórico de atendimentos concluídos */
    Senha historico_atendidos[MAX_HISTORICO];
    int total_atendidos;
} SistemaAtendimento;

/*
 * Inicializa o sistema de atendimento, filas e contadores.
 */
void sistema_inicializar(SistemaAtendimento *s);

/*
 * Libera os recursos alocados pelas filas do sistema.
 */
void sistema_destruir(SistemaAtendimento *s);

/*
 * Cadastra um paciente, classifica sua prioridade, gera a senha sequencial
 * correspondente e a insere na fila apropriada.
 * Retorna a senha emitida.
 */
Senha sistema_emitir_senha(SistemaAtendimento *s, const DadosPaciente *paciente);

/*
 * Consulta qual será a próxima senha a ser chamada respeitando a precedência:
 * Urgente > Preferencial > Normal.
 * Não altera o estado das filas. Retorna 1 se houver senha, ou 0 se todas vazias.
 */
int sistema_consultar_proxima_senha(const SistemaAtendimento *s, Senha *proxima);

/*
 * Registra a chamada do próximo paciente:
 * - Desenfileira a próxima senha de maior prioridade.
 * - Define-a como senha em atendimento atual.
 * Se já houver alguém em atendimento, retorna código específico (ex: -1)
 * para que o operador decida ou finalize o atendimento anterior.
 * Retorna: 1 em caso de sucesso, 0 se filas vazias, -1 se já existe atendimento em curso.
 */
int sistema_chamar_proximo(SistemaAtendimento *s, Senha *chamada);

/*
 * Identifica a senha atualmente em atendimento.
 * Retorna 1 se há atendimento em curso, ou 0 caso contrário.
 */
int sistema_obter_em_atendimento(const SistemaAtendimento *s, Senha *atual);

/*
 * Finaliza o atendimento atual, registrando a senha no histórico de atendidos
 * e liberando o consultório/atendente para novas chamadas.
 * Retorna 1 se finalizou com sucesso, ou 0 se não havia atendimento em curso.
 */
int sistema_finalizar_atendimento(SistemaAtendimento *s, Senha *finalizada);

/*
 * Exibe o painel textual de chamada com:
 * - Senha atualmente chamada e categoria;
 * - Próximas senhas aguardando chamada;
 * - Quantidade de pacientes em espera por categoria.
 */
void sistema_exibir_painel(const SistemaAtendimento *s);

/*
 * Exibe relatório completo de status das filas e histórico de atendimentos.
 */
void sistema_exibir_relatorio(const SistemaAtendimento *s);

#endif /* ATENDIMENTO_H */
