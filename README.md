# Sistema de Gerenciamento de Senhas de Atendimento Clínico

**Disciplina:** Algoritmos e Programação / Engenharia de Software (2º Semestre)  
**Linguagem:** C  
**Atividade:** Questão 02 — Sistema de Gerenciamento de Senhas Clínicas  

---

## 1. Objetivo do Sistema

O objetivo deste projeto é informatizar o processo de triagem e controle de filas de atendimento em uma clínica médica. O programa cadastra os pacientes, analisa seus dados de prioridade e gera senhas sequenciais automáticas. 

O sistema organiza o atendimento através de três filas distintas (**Urgente**, **Preferencial** e **Normal**), respeitando a precedência de chamada e a regra FIFO (*First-In, First-Out* - o primeiro que chega é o primeiro a ser atendido dentro de cada fila).

---

## 2. Funcionalidades e Regras de Negócio

### Categorias de Atendimento e Prioridade

| Código | Categoria | Regra de Classificação | Ordem de Chamada |
| :---: | :---: | :--- | :---: |
| **U001** | **Urgente** | Pacientes com idade maior que 80 anos | 1ª prioridade |
| **P001** | **Preferencial** | Gestantes, lactantes, pessoas com deficiência/necessidade especial, pessoas obesas, mulheres com 60+ anos e homens com 65+ anos | 2ª prioridade |
| **N001** | **Normal** | Pacientes sem nenhuma das condições de prioridade acima | 3ª prioridade |

* **Numeração Independente:** As senhas contam de forma separada para cada categoria (ex: `U001`, `U002`, `P001`, `P002`, `N001`, `N002`).
* **Precedência:** Se houver alguém na fila de urgência, ele é chamado primeiro. Se não houver urgentes, chama os preferenciais. Se não houver preferenciais, chama os normais.
* **Senhas atendidas não retornam à fila:** Assim que o atendimento é concluído, o paciente é liberado e a fila é atualizada.

---

## 3. Estrutura do Código e Funções Implementadas

O código foi todo implementado no arquivo `main.c`, utilizando `struct`, vetores para simular as filas e funções específicas para não concentrar a lógica dentro da `main()`:

* `limparBuffer()`: Limpa o buffer de entrada do teclado (`stdin`) para evitar problemas de leitura com `scanf`.
* `pausar()`: Dá uma pausa na tela para que o usuário possa ler as mensagens antes de voltar ao menu.
* `classificarPaciente(Paciente p)`: Recebe os dados do paciente e retorna a categoria correspondente (`'U'`, `'P'` ou `'N'`).
* `lerSimNao(char mensagem[])`: Valida a leitura de opções Sim/Não (`S`/`N`).
* `registrarNovaSenha()`: Realiza a triagem, valida os dados (gênero, idade, condições especiais), gera a senha e enfileira o paciente.
* `consultarProximaSenha()`: Exibe quem é o próximo paciente da fila sem retirá-lo da fila.
* `chamarPaciente()`: Chama o próximo paciente respeitando a prioridade (`Urgente > Preferencial > Normal`), coloca-o em atendimento e reorganiza a fila.
* `identificarEmAtendimento()`: Mostra os dados do paciente que está atualmente no consultório.
* `finalizarAtendimento()`: Conclui a consulta do paciente atual e libera o consultório.
* `exibirPainel()`: Exibe o painel textual com a senha atual, as filas de espera e o total de pessoas aguardando.
* `main()`: Contém o menu interativo com repetição `do...while` e seleção `switch...case`.

---

## 4. Como Compilar e Executar

### Usando o GCC no Windows, Linux ou macOS:
Abra o terminal na pasta do projeto e execute:

```bash
gcc main.c -o sistema_senhas
```

Para rodar o programa:

* **Windows:**
  ```cmd
  .\sistema_senhas.exe
  ```
* **Linux / Mac:**
  ```bash
  ./sistema_senhas
  ```

---

## 5. Exemplo de Utilização

### Exemplo de Cadastro e Emissão:
1. Cadastrar homem de 85 anos:
   - Sistema gera automaticamente a senha: **`U001`** (Urgente).
2. Cadastrar mulher de 25 anos, gestante:
   - Sistema gera automaticamente a senha: **`P001`** (Preferencial).
3. Cadastrar homem de 30 anos sem comorbidades:
   - Sistema gera automaticamente a senha: **`N001`** (Normal).

### Exemplo do Painel de Chamada (Opção 6):
```text
=======================================================
              PAINEL DE CHAMADA CLINICA                
=======================================================
 SENHA CHAMADA ATUALMENTE: [ Nenhuma no momento ]
-------------------------------------------------------
 FILAS DE ESPERA:
  [U] Urgentes (1): U001 
  [P] Preferenciais (1): P001 
  [N] Normais (1): N001 
-------------------------------------------------------
 TOTAL DE PACIENTES AGUARDANDO: 3
=======================================================
```

### Exemplo de Chamada (Opção 3):
Ao solicitar a chamada, a primeira senha chamada será **`U001`**. Após finalizar seu atendimento, a próxima será **`P001`**, e por último **`N001`**.
