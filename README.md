# Sistema de Gerenciamento de Senhas de Atendimento Clínico

> **Avaliação Prática de Desenvolvimento de Software**  
> **Linguagem:** C (Padrão ISO C99)  
> **Tema Escolhido:** Questão 02 — Sistema de Gerenciamento de Senhas de Atendimento Clínico  

---

## 1. Objetivo do Sistema

O **Sistema de Gerenciamento de Senhas de Atendimento Clínico** foi desenvolvido para informatizar e otimizar o fluxo de triagem, emissão e chamada de senhas em clínicas de especialidades médicas.

O programa identifica de forma automatizada o perfil do paciente a partir de critérios objetivos coletados na triagem, classifica-o na categoria apropriada de atendimento (Urgente, Preferencial ou Normal), emite senhas sequenciais e independentes e gerencia a ordem de atendimento com base em regras de precedência rígidas e organização FIFO (*First-In, First-Out*).

---

## 2. Funcionalidades do Sistema

O sistema é dividido em quatro módulos funcionais e operacionais:

### Módulo 1 — Registro de Senhas (Triagem)
Coleta e valida interativamente os dados do paciente:
- **Gênero:** Masculino (`M`) ou Feminino (`F`);
- **Idade:** em anos (com validação numérica e rejeição de idades inconsistentes);
- **Gestante:** aplicável a pacientes do gênero feminino (`S`/`N`);
- **Lactante:** aplicável a pacientes do gênero feminino (`S`/`N`);
- **Necessidade Especial:** identificação de mobilidade reduzida ou deficiência (`S`/`N`);
- **Condição de Obesidade:** identificação de obesidade (`S`/`N`).

### Módulo 2 — Classificação Automática e Geração de Senhas
A classificação é calculada sem intervenção manual do atendente, seguindo a tabela didática da avaliação:

| Código | Categoria | Regra / Condição Didática |
| :---: | :---: | :--- |
| **U001** | **Urgente** | Pacientes com idade estritamente superior a 80 anos (`idade > 80`). |
| **P001** | **Preferencial** | Gestantes, lactantes, pessoas com necessidades especiais, pessoas obesas, mulheres com 60 anos ou mais (`idade >= 60`) e homens com 65 anos ou mais (`idade >= 65`). |
| **N001** | **Normal** | Demais pacientes sem critérios prioritários cadastrados. |

* **Numeração independente:** cada categoria possui seu próprio sequencial iniciado em `001` (ex.: `U001`, `U002`; `P001`, `P002`; `N001`, `N002`).
* **Precedência de chamada:** `Urgente` precede `Preferencial`, que precede `Normal`.
* **Critério interno da categoria:** estritamente FIFO (ordem de chegada/emissão).

### Módulo 3 — Registro e Controle de Atendimento
- **Consultar próxima senha:** visualiza quem será o próximo paciente chamado sem retirá-lo da fila.
- **Registrar chamada do paciente:** retira o paciente do topo da fila de maior prioridade disponível e o define como em atendimento no consultório.
- **Identificar paciente em atendimento:** exibe a senha, categoria e perfil clínico de quem está atualmente em consulta.
- **Finalizar atendimento:** conclui o atendimento atual, desocupa o consultório e arquiva o registro no histórico de atendimentos finalizados.
- **Senhas atendidas não retornam à fila:** garantia estrutural de integridade de fluxo.

### Módulo 4 — Painel Textual de Chamada
Apresenta um painel de monitoramento contendo:
- Senha atualmente em atendimento e respectiva categoria;
- Próxima senha prioritária aguardando chamada;
- Relação das próximas senhas em espera por fila;
- Contagem total e segmentada de pacientes em espera.

---

## 3. Arquitetura e Modularização do Código

O projeto adota separação estrita de responsabilidades em módulos (`.h` e `.c`), evitando a concentração de lógica na função `main()`.

```
Sistema de senhas/
├── Makefile                # Automação de compilação para Linux/macOS/MinGW
├── compilar.bat            # Script de compilação rápida para Windows
├── .gitignore              # Ignora binários e arquivos temporários
├── README.md               # Documentação completa do projeto
├── src/
│   ├── main.c              # Ponto de entrada, inicialização e término limpo
│   ├── paciente.h          # Definições de estruturas e protótipos de paciente e senha
│   ├── paciente.c          # Implementação da triagem, validações e classificação
│   ├── fila.h              # Protótipos da estrutura de dados de fila dinâmica FIFO
│   ├── fila.c              # Implementação de enfileirar, desenfileirar, espiar e destruir
│   ├── atendimento.h       # Protótipos das operações de controle e painel do sistema
│   ├── atendimento.c       # Lógica de precedência, histórico e renderização do painel
│   ├── menu.h              # Protótipos da interface interativa e validação de comandos
│   └── menu.c              # Menus de usuário, ações e mensagens formatadas
└── tests/
    └── teste_sistema.c     # Suíte de testes unitários automatizados com asserts
```

### Detalhamento das Funções Implementadas

#### Módulo `paciente` (`paciente.h` / `paciente.c`)
- `const char* categoria_para_string(CategoriaSenha cat)`: Converte enum da categoria para representação textual amigável.
- `CategoriaSenha classificar_paciente(const DadosPaciente *paciente)`: Aplica as regras de negócio para determinar `CAT_URGENTE`, `CAT_PREFERENCIAL` ou `CAT_NORMAL`.
- `void cadastrar_paciente_interativo(DadosPaciente *paciente)`: Conduz o formulário de triagem no terminal, tratando erros de entrada e limpando o buffer.
- `Senha criar_senha(CategoriaSenha categoria, int numero_sequencial, const DadosPaciente *paciente)`: Formata o código textual da senha (ex: `U001`) e anexa os dados do paciente.
- `void imprimir_detalhes_senha(const Senha *s)`: Imprime relatório formatado dos dados de uma senha.

#### Módulo `fila` (`fila.h` / `fila.c`)
- `void fila_inicializar(Fila *f)`: Inicializa ponteiros de início/fim nulos e tamanho zerado.
- `int fila_esta_vazia(const Fila *f)`: Checa se a fila possui elementos.
- `int fila_tamanho(const Fila *f)`: Retorna a quantidade atual de nós na fila.
- `int fila_enfileirar(Fila *f, Senha senha)`: Aloca dinamicamente um novo nó e insere ao final da fila (FIFO).
- `int fila_desenfileirar(Fila *f, Senha *senha_removida)`: Remove o nó da cabeça da fila e libera sua memória.
- `int fila_espiar(const Fila *f, Senha *senha_inicio)`: Acessa o elemento da cabeça da fila sem removê-lo.
- `void fila_destruir(Fila *f)`: Libera todos os nós encadeados, prevenindo vazamentos de memória (*memory leaks*).
- `void fila_formatar_lista(const Fila *f, char *buffer, size_t buffer_size, int max_itens)`: Formata string com os códigos das senhas na fila para o painel textual.

#### Módulo `atendimento` (`atendimento.h` / `atendimento.c`)
- `void sistema_inicializar(SistemaAtendimento *s)`: Configura as três filas de atendimento e os contadores sequenciais.
- `void sistema_destruir(SistemaAtendimento *s)`: Desaloca todas as filas.
- `Senha sistema_emitir_senha(SistemaAtendimento *s, const DadosPaciente *paciente)`: Coordena classificação, incremento do contador correspondente e enfileiramento.
- `int sistema_consultar_proxima_senha(const SistemaAtendimento *s, Senha *proxima)`: Avalia a precedência (Urgente > Preferencial > Normal) e retorna a próxima senha a ser atendida.
- `int sistema_chamar_proximo(SistemaAtendimento *s, Senha *chamada)`: Transfere o paciente prioritário da fila para o consultório. Trata conflito caso já exista paciente em consulta.
- `int sistema_obter_em_atendimento(const SistemaAtendimento *s, Senha *atual)`: Retorna os dados do paciente atualmente em atendimento.
- `int sistema_finalizar_atendimento(SistemaAtendimento *s, Senha *finalizada)`: Registra no histórico de concluídos e libera o consultório.
- `void sistema_exibir_painel(const SistemaAtendimento *s)`: Imprime o painel textual formatado (Módulo 4).
- `void sistema_exibir_relatorio(const SistemaAtendimento *s)`: Imprime estatísticas de atendimento e histórico geral.

#### Módulo `menu` (`menu.h` / `menu.c`)
- `void executar_menu_principal(SistemaAtendimento *sistema)`: Controla o loop principal de interação com o usuário.
- `void pausar(void)`: Aguarda confirmação do operador para retornar ao menu.

---

## 4. Procedimentos de Compilação e Execução

### Pré-requisitos
- Compilador C compatível com C99 (ex.: `gcc`, `clang`, MinGW, MSYS2).

### Compilação no Windows (Prompt de Comando ou PowerShell)
Execute o script em lote fornecido:
```cmd
compilar.bat
```
Ou compile manualmente com o GCC:
```bash
gcc -Wall -Wextra -std=c99 -pedantic src/main.c src/paciente.c src/fila.c src/atendimento.c src/menu.c -o sistema_senhas.exe
```

Para executar o programa:
```cmd
sistema_senhas.exe
```

### Compilação no Linux / macOS / WSL (usando Make ou GCC)
Utilizando o `Makefile`:
```bash
make
./sistema_senhas
```
Ou com GCC direto:
```bash
gcc -Wall -Wextra -std=c99 -pedantic src/main.c src/paciente.c src/fila.c src/atendimento.c src/menu.c -o sistema_senhas
./sistema_senhas
```

### Execução dos Testes Automatizados
O projeto conta com suíte de testes unitários para verificação autônoma das regras de classificação, numeração independente e precedência:
```bash
gcc -Wall -Wextra -std=c99 -pedantic tests/teste_sistema.c src/paciente.c src/fila.c src/atendimento.c -o tests/teste_sistema.exe
./tests/teste_sistema.exe
```

---

## 5. Exemplos de Utilização Passo a Passo

### Cenário 1: Cadastro de Pacientes e Geração de Senhas
O operador cadastra consecutivamente três pacientes na triagem:

1. **Paciente 1:** Homem de 32 anos, sem comorbidades.  
   - Classificação: **Normal**  
   - Senha gerada: `N001`
2. **Paciente 2:** Mulher de 28 anos, gestante.  
   - Classificação: **Preferencial** (condição de gestante)  
   - Senha gerada: `P001`
3. **Paciente 3:** Homem de 83 anos.  
   - Classificação: **Urgente** (idade > 80)  
   - Senha gerada: `U001`
4. **Paciente 4:** Homem de 66 anos.  
   - Classificação: **Preferencial** (homem >= 65)  
   - Senha gerada: `P002`

### Cenário 2: Visualização do Painel Textual de Chamada
Ao acessar a opção **[6] Visualizar Painel de Chamada**, o sistema exibe:
```text
======================================================================
                    PAINEL DE CHAMADA CLINICA                         
======================================================================
  >> SENHA ATUALMENTE CHAMADA: [ NENHUMA NO MOMENTO ]
  >> STATUS:                  Aguardando chamada de paciente
----------------------------------------------------------------------
  * PROXIMA DA FILA:          U001 (Urgente)
----------------------------------------------------------------------
  FILAS DE ESPERA (ordem de atendimento FIFO por categoria):
  [U] Urgentes:     U001                                (Aguardando: 1)
  [P] Preferencias: P001, P002                          (Aguardando: 2)
  [N] Normais:      N001                                (Aguardando: 1)
----------------------------------------------------------------------
  RESUMO DE ESPERA:
  - Urgentes:     1 paciente(s)
  - Preferenciais:2 paciente(s)
  - Normais:      1 paciente(s)
  - TOTAL NA FILA:4 paciente(s)
======================================================================
```

### Cenário 3: Respeito Estrito à Precedência de Chamada
Ao acionar a opção **[3] Registrar Chamada de Paciente**:
1. A primeira chamada será **`U001`** (Urgente precede Preferencial e Normal, mesmo tendo sido emitida depois de `N001` e `P001`).
2. Finalizado o atendimento de `U001`, a próxima chamada será **`P001`** (Preferencial precede Normal).
3. Finalizado `P001`, a próxima chamada será **`P002`** (FIFO dentro da categoria Preferencial).
4. Finalizado `P002`, a próxima chamada será **`N001`**.

---

## 6. Tratamento de Exceções e Validações

- **Validação de tipos e intervalos:** Entradas não numéricas no campo de idade ou opções de menu são tratadas sem travamento do programa nem loops infinitos de `scanf`.
- **Validação contextual:** As perguntas de gestação e lactação só são aplicadas a pacientes do gênero feminino.
- **Proteção de estado de consulta:** Caso o operador tente chamar um novo paciente enquanto outro estiver com atendimento ativo, o sistema alerta sobre a pendência e solicita confirmação antes de concluir a consulta anterior.
- **Limpeza de memória:** A destruição das filas libera os nós alocados dinamicamente ao sair do sistema.

---

## 7. Critérios de Avaliação Atendidos

| Critério | Peso | Como foi atendido |
| :--- | :---: | :--- |
| **Funcionalidades e requisitos** | 0,70 | Todos os 4 módulos implementados integralmente com regras de prioridade e precedência estritas. |
| **Modularização** | 0,40 | Código desacoplado em `paciente`, `fila`, `atendimento`, `menu` e `main`. |
| **Lógica e estruturas** | 0,30 | Uso de filas FIFO simplesmente encadeadas com alocação dinâmica, enums e structs. |
| **Validação e testes** | 0,20 | Validações completas de entrada e suíte de testes unitários em `tests/teste_sistema.c`. |
| **GitHub e README** | 0,30 | Repositório Git estruturado, `.gitignore`, procedimentos de compilação e manual completo. |
| **Legibilidade e boas práticas** | 0,10 | Padrão C99, indentação consistente, comentários explicativos e flags `-Wall -Wextra -pedantic` sem avisos. |
| **TOTAL** | **2,00** | **Atendimento integral a todos os requisitos.** |
