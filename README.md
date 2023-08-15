# Malloc-eiro: Gerenciador de memória dinâmica
<img src="./assets/school_furyou_tsuppari.png" alt="drawing" width="200"/>
<p align="center"></p>

- [💡 Ideia do projeto](#💡-ideia-do-projeto)
- [🔨 Instalação](#🔨-instalação)
    - [📦 Pré requisitos](#📦-pré-requisitos)
        - [Gerais](#gerais)
        - [Específicos de Windows](#específicos-de-windows)
        - [Instalação no Linux](#instalação-no-linux)
        - [Instalação no Windows](#instalação-no-windows)
    - [🚨 Observações](#🚨-observações)
- [🚀 Exemplo de funcionamento](#🚀-exemplo-de-funcionamento)
- [📄 Referências](#📄-referências)
- [👥 Contribuidores](#👥-contribuidores)

# 💡 Ideia do projeto

O projeto consiste no desenvolvimento de uma biblioteca básica de gerenciamento de memória dinâmica. 

O gerenciamento de memória lida diretamente com endereços físicos utilizando o algoritmo "buddy allocator", de maneira semelhante ao gerenciador de memória dinâmica interno do Linux. Tal algoritmo possui 3 etapas principais:
- Gerenciamento de blocos livres;
- Alocação de páginas;
- Liberação de páginas.

Os detalhes de cada etapa podem ser vistos nas seções 1.2.1 à 1.2.3 do [relatório do projeto](./doc/PCS%203732%20-%20Relatório%20-%20Malloc-eiro_%20Gerenciador%20de%20memória%20dinâmica%20.pdf).


Para isso, foram desenvolvidos os seguintes arquivos:
- `malloc.h` e `malloc.c`: Implementação das funções para gerenciamento de memória;
- `main.c`: Contem a função utilizada para testar o alocador; 
- `sizes.h`: Macros com os parâmetros da memória (tamanho de uma palavra, espaço alocado para heap, tamanho de bloco, etc.);
- `boot.s`: Implementação do vetor de interrupções e da seção .text;
- `kernel.ld`: Definição dos segmentos de text, data, bss e reserva de espaço para a pilha.


# 🔨 Instalação
## 📦 Pré requisitos
### Gerais

Os pré-requisitos gerais para a utilização do projeto são:
- make;
- arm-none-eabi e seus programas, gcc, ld e as;
- qemu, mais especificamente, o qemu-system-arm;
- gdb (gdb-multiarch).

### Específicos de Windows

Caso esteja utilizando o Windows, deve-se instalar a seguinte dependência adicional:
- cygwin ou mingw

### Instalação no Linux
Para instalar as dependências no Debian ou no Ubuntu, rode os seguintes comandos:

~~~bash
sudo apt update
sudo apt install binutils-arm-none-eabi gcc-arm-none-eabi
sudo apt install gdb-multiarch qemu-system-arm make
~~~

### Instalação no Windows
Para instalar as dependências no Windows 10, rode os seguintes comandos:

Siga esse guia de instalação para o [Msys2](https://www.msys2.org/) ou instale outro ambiesnte semelhante

Em algum dos terminais baixados(mingw64, mingw32, msys), rode os seguintes comandos para instalação, se utilizar um processador Intel ou AMD:

~~~bash
pacman -Suy
pacman -S mingw-w64-x86_64-arm-none-eabi-toolchain
pacman -S mingw-w64-x86_64-gdb-multiarch
pacman -S mingw-w64-x86_64-qemu
pacman -S make
~~~


## 🚨 Observações

No Makefile, alterar a seguinte linha de flags:

```
LDOPTS = -L/mingw64/lib/gcc/arm-none-eabi/12.2.0/ -L/mingw64/arm-none-eabi/lib
```

Esses caminhos devem ser alterados com os diretórios dos arquivos libgcc.a e libg.a, instalados pelo arm-none-eabi.

Por exemplo, em um Ubuntu com o package manager apt-get, eles estarão no seguinte caminho:

```
LDOPTS = -L/usr/lib/gcc/arm-none-eabi/<VERSION_NUMBER>/ -L/usr/lib/arm-none-eabi/lib
```

# 🚀 Exemplo de funcionamento
Alguns testes que exemplificam o funcionamento podem ser vistos com mais detalhe no seguinte vídeo disponível no [YouTube](https://www.youtube.com/watch?v=cmFaDghkKq8), com um backup também sendo disponível no Google Drive no [seguinte link](https://drive.google.com/file/d/1reY1DfBkVRnffd3z2AoPIyTrQeIOiicI/view?usp=sharing) a partir do minuto [6:35](https://youtu.be/cmFaDghkKq8?t=395).


# 📄 Referências
Gorman M. Chapter 6  Physical Page Allocation. Kernel. Disponível em: <https://www.kernel.org/doc/gorman/html/understand/understand009.html> Acesso em: 27 de Jul. de 2023.

Kenneth C. Knowlton. A fast storage allocator. Communications of the ACM, 8(10):623–624, 1965.

D. Knuth. The Art of Computer Programming, Fundamental Algorithms, Volume 1. Addison-Wesley, Reading, Mass., 1968.

# 👥 Contribuidores

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="https://github.com/Edu-Hiroshi"><img src="https://avatars.githubusercontent.com/u/97803912?s=400&u=14625cf4c91606d4787d983fd2692ee4db47ff4e&v=4" width="100px;" alt=""/><br /><sub><b>Eduardo Hiroshi Ito<br/>11806868</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/GabrielCout"><img src="https://avatars.githubusercontent.com/u/64551976?v=4" width="100px;" alt=""/><br /><sub><b>Gabriel Coutinho Ribeiro<br />11803437</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/RicardoHonda"><img src="https://avatars.githubusercontent.com/u/62343088?v=4?s=100" width="100px;" alt=""/><br /><sub><b>Ricardo Tamay Honda <br/> 11803778</b></sub></a><br /></td>
  </tr>
</table>