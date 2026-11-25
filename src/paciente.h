#ifndef PACIENTE_H
#define PACIENTE_H

/*
 * ============================================================================
 * Módulo: paciente.h
 * Responsabilidade: Definição dos tipos de dados de paciente, categoria de senha,
 *                   funções de leitura, validação e classificação de prioridade.
 * ============================================================================
 */

/* Categorias de atendimento do sistema */
typedef enum {
    CAT_NORMAL = 'N',
    CAT_PREFERENCIAL = 'P',
    CAT_URGENTE = 'U'
} CategoriaSenha;

/* Estrutura com os dados do paciente coletados na triagem */
typedef struct {
    char genero;              /* 'M' (Masculino) ou 'F' (Feminino) */
    int idade;                /* Idade em anos (>= 0) */
    int gestante;             /* 1 = Sim, 0 = Não (aplicável a mulheres) */
    int lactante;             /* 1 = Sim, 0 = Não (aplicável a mulheres) */
    int necessidadeEspecial;  /* 1 = Sim, 0 = Não */
    int obesidade;            /* 1 = Sim, 0 = Não */
} DadosPaciente;

/* Estrutura que representa uma senha emitida */
typedef struct {
    char codigo[16];          /* Ex: "U001", "P002", "N003" */
    CategoriaSenha categoria; /* Categoria da senha */
    int numero;               /* Número sequencial na categoria */
    DadosPaciente paciente;   /* Cópia dos dados do paciente associado */
} Senha;

/*
 * Retorna o nome por extenso da categoria ('Urgente', 'Preferencial', 'Normal').
 */
const char* categoria_para_string(CategoriaSenha cat);

/*
 * Classifica o paciente conforme as regras do Módulo 2:
 * 1. Urgente (U): Pacientes com idade superior a 80 anos (> 80).
 * 2. Preferencial (P): Gestantes, lactantes, pessoas com necessidades especiais,
 *    pessoas obesas, mulheres com 60 anos ou mais e homens com 65 anos ou mais.
 * 3. Normal (N): Pacientes sem condições de prioridade cadastradas.
 */
CategoriaSenha classificar_paciente(const DadosPaciente *paciente);

/*
 * Realiza o cadastro interativo do paciente com validação rigorosa de entradas.
 */
void cadastrar_paciente_interativo(DadosPaciente *paciente);

/*
 * Formata e gera uma nova senha para a categoria e número sequencial especificados.
 */
Senha criar_senha(CategoriaSenha categoria, int numero_sequencial, const DadosPaciente *paciente);

/*
 * Imprime os detalhes de um paciente e de sua senha.
 */
void imprimir_detalhes_senha(const Senha *s);

#endif /* PACIENTE_H */
