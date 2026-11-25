#ifndef FILA_H
#define FILA_H

#include <stddef.h>
#include "paciente.h"

/*
 * ============================================================================
 * Módulo: fila.h
 * Responsabilidade: Implementação de estrutura de dados de Fila (Queue) FIFO
 *                   para armazenamento dinâmico e sequencial de senhas.
 * ============================================================================
 */

/* Nó da fila dinamicamente encadeada */
typedef struct NodoFila {
    Senha senha;
    struct NodoFila *prox;
} NodoFila;

/* Estrutura de controle da fila */
typedef struct {
    NodoFila *inicio; /* Ponteiro para o primeiro elemento (cabeça da fila) */
    NodoFila *fim;    /* Ponteiro para o último elemento (cauda da fila) */
    int tamanho;      /* Quantidade de elementos na fila */
} Fila;

/*
 * Inicializa a fila definindo ponteiros nulos e tamanho zero.
 */
void fila_inicializar(Fila *f);

/*
 * Retorna 1 se a fila estiver vazia, ou 0 caso contrário.
 */
int fila_esta_vazia(const Fila *f);

/*
 * Retorna a quantidade de elementos armazenados na fila.
 */
int fila_tamanho(const Fila *f);

/*
 * Insere uma nova senha no final da fila (FIFO).
 * Retorna 1 em caso de sucesso, ou 0 em caso de falha de alocação de memória.
 */
int fila_enfileirar(Fila *f, Senha senha);

/*
 * Remove a senha do início da fila e a armazena em *senha_removida.
 * Retorna 1 em caso de sucesso, ou 0 se a fila estiver vazia.
 */
int fila_desenfileirar(Fila *f, Senha *senha_removida);

/*
 * Consulta a senha do início da fila sem removê-la.
 * Retorna 1 em caso de sucesso, ou 0 se a fila estiver vazia.
 */
int fila_espiar(const Fila *f, Senha *senha_inicio);

/*
 * Libera toda a memória dinâmica alocada pelos nós da fila.
 */
void fila_destruir(Fila *f);

/*
 * Preenche um buffer de texto com a lista de códigos das senhas na fila.
 */
void fila_formatar_lista(const Fila *f, char *buffer, size_t buffer_size, int max_itens);

#endif /* FILA_H */
