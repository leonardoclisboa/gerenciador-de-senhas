#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fila.h"

void fila_inicializar(Fila *f) {
    if (f != NULL) {
        f->inicio = NULL;
        f->fim = NULL;
        f->tamanho = 0;
    }
}

int fila_esta_vazia(const Fila *f) {
    if (f == NULL) {
        return 1;
    }
    return (f->inicio == NULL);
}

int fila_tamanho(const Fila *f) {
    if (f == NULL) {
        return 0;
    }
    return f->tamanho;
}

int fila_enfileirar(Fila *f, Senha senha) {
    if (f == NULL) {
        return 0;
    }

    NodoFila *novo = (NodoFila *)malloc(sizeof(NodoFila));
    if (novo == NULL) {
        return 0; /* Falha de alocacao de memoria */
    }

    novo->senha = senha;
    novo->prox = NULL;

    if (f->fim == NULL) {
        /* Fila vazia: novo no e inicio e fim ao mesmo tempo */
        f->inicio = novo;
        f->fim = novo;
    } else {
        /* Adiciona no final e atualiza o ponteiro de fim */
        f->fim->prox = novo;
        f->fim = novo;
    }

    f->tamanho++;
    return 1;
}

int fila_desenfileirar(Fila *f, Senha *senha_removida) {
    if (f == NULL || fila_esta_vazia(f)) {
        return 0;
    }

    NodoFila *no_remover = f->inicio;
    if (senha_removida != NULL) {
        *senha_removida = no_remover->senha;
    }

    f->inicio = f->inicio->prox;
    if (f->inicio == NULL) {
        /* Fila tornou-se vazia */
        f->fim = NULL;
    }

    free(no_remover);
    f->tamanho--;
    return 1;
}

int fila_espiar(const Fila *f, Senha *senha_inicio) {
    if (f == NULL || fila_esta_vazia(f)) {
        return 0;
    }

    if (senha_inicio != NULL) {
        *senha_inicio = f->inicio->senha;
    }
    return 1;
}

void fila_destruir(Fila *f) {
    if (f == NULL) {
        return;
    }

    NodoFila *atual = f->inicio;
    while (atual != NULL) {
        NodoFila *temp = atual;
        atual = atual->prox;
        free(temp);
    }

    f->inicio = NULL;
    f->fim = NULL;
    f->tamanho = 0;
}

void fila_formatar_lista(const Fila *f, char *buffer, size_t buffer_size, int max_itens) {
    if (buffer == NULL || buffer_size == 0) {
        return;
    }

    buffer[0] = '\0';
    if (f == NULL || fila_esta_vazia(f)) {
        snprintf(buffer, buffer_size, "(vazia)");
        return;
    }

    NodoFila *atual = f->inicio;
    int contador = 0;
    size_t len = 0;

    while (atual != NULL && (max_itens <= 0 || contador < max_itens)) {
        char item[32];
        if (contador > 0) {
            snprintf(item, sizeof(item), ", %s", atual->senha.codigo);
        } else {
            snprintf(item, sizeof(item), "%s", atual->senha.codigo);
        }

        len = strlen(buffer);
        if (len + strlen(item) + 1 < buffer_size) {
            strncat(buffer, item, buffer_size - len - 1);
        } else {
            break;
        }

        atual = atual->prox;
        contador++;
    }

    if (atual != NULL) {
        len = strlen(buffer);
        if (len + 6 < buffer_size) {
            strncat(buffer, ", ...", buffer_size - len - 1);
        }
    }
}
