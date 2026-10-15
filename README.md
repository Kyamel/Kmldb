# Kmldb

Projeto acadêmico em C para a disciplina de Algoritmos e Estruturas de Dados 2 (AEDS2). Simula um cadastro de academia em arquivos binários, com menus para clientes, funcionários, exercícios e treinos. Usa tabelas com índice hash e inclui rotinas experimentais de ordenação externa.

## Requisitos

- Compilador C99 (GCC ou Clang; validação feita em Linux)
- CMake 3.10 ou superior

## Compilar e executar

A partir da raiz do repositório:

```sh
cmake -S . -B build
cmake --build build
./build/Bin/Debug/programa
```

O programa cria `data/`, `data/particions/` e `data/testes/` no **diretório de trabalho atual**. Por isso, execute-o a partir da raiz do repositório se quiser manter os arquivos em `./data`. Esses arquivos e os logs são ignorados pelo Git.

O menu principal oferece cadastro, pesquisa, atualização e exclusão de cada entidade. Escolha `9` para encerrar. As chaves primárias são atribuídas automaticamente nos cadastros normais; treinos usam as chaves de cliente e exercício já cadastrados. A opção `Debug` contém geradores de dados e rotinas de ordenação do trabalho acadêmico.

## Usando Nix

Se você usa Nix com flakes habilitados, pode entrar no ambiente de desenvolvimento com `nix develop`. Dentro dele, execute os comandos CMake acima. Para compilar e rodar o teste de regressão em uma derivação isolada, use `nix build` e execute `./result/bin/programa` a partir de um diretório onde você pode criar `data/`. O flake aceita Linux x86_64 e aarch64.

## Organização do código

| Caminho | Responsabilidade |
| --- | --- |
| `src/main.c` | Entrada da aplicação CLI |
| `src/view/cli/` | Menus e entrada de dados |
| `src/controler/` | Encaminhamento das operações de cadastro |
| `src/kmldb/` | Formato do banco binário e operações hash |
| `src/model/` | Estruturas de cliente, funcionário, exercício e treino; rotinas de ordenação |
| `src/utils/` | Funções auxiliares, geração de dados e log |
| `src/testes/`, `src/teste2.c` | Código de testes e experimentos acadêmicos |

Cada tipo de registro é salvo em seu próprio arquivo `.dat` dentro de `data/`. O formato grava estruturas C diretamente; portanto, **não há garantia de compatibilidade entre sistemas, compiladores ou versões das estruturas**. Faça cópia dos dados antes de atualizar o programa. O projeto é didático e não oferece transações, controle de acesso nem proteção para dados pessoais. Use apenas dados fictícios.

## Estado do projeto

A interface de linha de comando é a interface disponível neste repositório. As opções de ordenação e geração de bases pertencem aos experimentos da disciplina e devem ser usadas com cópias dos arquivos de dados. Há um teste de regressão do índice hash. Após compilar, execute `ctest --test-dir build --output-on-failure`.

## Licença

Distribuído sob a **GNU General Public License versão 3** (GPL-3.0-only). Consulte o texto completo em [LICENSE](LICENSE). Ao redistribuir o programa ou versões modificadas, siga os termos dessa licença, incluindo a disponibilização do código-fonte correspondente quando aplicável.
