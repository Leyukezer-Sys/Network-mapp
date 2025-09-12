# NetWork Scanner - Scanner de Portas TCP/UDP

NetWork Scanner é uma aplicação para varredura de portas TCP e UDP com interface gráfica moderna e funcionalidades avançadas.

## 📋 Funcionalidades

- 🔍 Varredura de portas TCP e UDP
- 🎯 Interface gráfica moderna com tema futurista
- 📊 Visualização de resultados em tabela com cores para diferentes status
- 🔧 Filtros por protocolo e status das portas
- 💾 Exportação de resultados para CSV e TXT
- 📱 Controle de tamanho da fonte com atalhos de teclado
- ⚡ Multiplataforma (Linux e Windows)

## 🚀 Instalação no Linux

### Dependências

```bash
# Ubuntu/Debian
sudo apt-get install build-essential libfltk1.3-dev libx11-dev libxft-dev

# Fedora
sudo dnf install gcc-c++ fltk-devel libX11-devel libXft-devel

# Arch Linux
sudo pacman -S base-devel fltk
```

### Compilação e Execução

1. Clone ou baixe os arquivos do projeto:

```bash
git clone <url-do-repositorio>
cd network-scanner/FLTK/
```

2. Compile o projeto:

```bash
make
```

3. Execute o aplicativo:

```bash
./network-scanner
```

### Compilação Manual

Se preferir compilar manualmente:

```bash
g++ -std=c++11 -pthread main.cpp scanner_thread.cpp -lfltk -o network-scanner
```

## 🪟 Instalação no Windows

### Pré-requisitos

1. **Instalar o MSYS2** (recomendado):

   - Baixe em: https://www.msys2.org/
   - Siga as instruções de instalação

2. **Instalar dependências no MSYS2**:
   Abra o terminal MSYS2 MinGW 64-bit e execute:

```bash
pacman -S --needed base-devel mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-fltk
```

### Compilação no Windows

1. Abra o terminal MSYS2 MinGW 64-bit
2. Navegue até a pasta do projeto
3. Compile o projeto:

```bash
g++ -std=c++11 -I/mingw64/include -L/mingw64/lib main.cpp scanner_thread.cpp -lfltk -lws2_32 -o network-scanner.exe
```

4. Execute o aplicativo:

```bash
./network-scanner.exe
```

### Compilação com Visual Studio (Alternativa)

1. Instale o FLTK para Windows:

   - Baixe de: https://www.fltk.org/software.php
   - Extraia e adicione os caminhos de include e lib ao Visual Studio

2. Crie um novo projeto no Visual Studio
3. Adicione os arquivos `main.cpp` e `scanner_thread.cpp` ao projeto
4. Configure as dependências:

   - Vá em Project Properties → C/C++ → General → Additional Include Directories
   - Adicione o caminho para a pasta include do FLTK
   - Vá em Linker → Input → Additional Dependencies
   - Adicione `fltk.lib`, `ws2_32.lib`

5. Compile e execute

## 🎮 Como Usar

1. **Configuração**:

   - Informe o endereço IP ou hostname do alvo
   - Defina o intervalo de portas (inicial e final)
   - Selecione os protocolos (TCP e/ou UDP)

2. **Varredura**:

   - Clique em "Iniciar Varredura" para começar
   - Acompanhe o progresso na barra de status
   - Use "Parar Varredura" para interromper

3. **Resultados**:
   - Visualize as portas abertas, fechadas e filtradas
   - Use os filtros para refinar os resultados
   - Exporte os resultados com o botão "Exportar Resultados"

## ⌨️ Atalhos de Teclado

- `Ctrl++`: Aumentar tamanho da fonte
- `Ctrl+-`: Diminuir tamanho da fonte
- `Ctrl+0`: Resetar tamanho da fonte padrão

## 🐛 Solução de Problemas

### Erros Comuns no Linux

**Problema**: Erro de linker com FLTK
**Solução**:

```bash
sudo apt-get install libfltk1.3-dev
```

**Problema**: Erro de permissão para criar socket
**Solução**: Execute o programa como usuário normal (não requer privilégios elevados)

### Erros Comuns no Windows

**Problema**: Não encontra bibliotecas FLTK
**Solução**: Verifique se o caminho das bibliotecas está correto no Visual Studio

**Problema**: Erro de Winsock
**Solução**: Certifique-se de que `-lws2_32` está incluído na linha de comando

## 📦 Estrutura do Projeto

```
network-scanner/
├── main.cpp          # Código principal da interface
├── scanner_thread.h  # Cabeçalho do scanner
├── scanner_thread.cpp # Implementação do scanner
├── Makefile         # Arquivo de compilação (Linux)
└── README.md        # Este arquivo
```

## 🔧 Personalização

### Modificando o Tema

O tema futurista pode ser personalizado editando as cores no código fonte. Procure pela função `applyFuturisticStyle()` no arquivo `main.cpp`.

### Adicionando Novos Serviços

Para adicionar novos serviços à detecção automática, edite a função `getServiceName()` no arquivo `scanner_thread.cpp`.

## 📄 Licença

Este projeto é distribuído sob a licença MIT. Veja o arquivo LICENSE para detalhes.

## 🤝 Contribuições

Contribuições são bem-vindas! Sinta-se à vontade para:

- Reportar bugs
- Sugerir novas funcionalidades
- Enviar pull requests

## 📞 Suporte

Em caso de problemas ou dúvidas:

1. Verifique a seção de Solução de Problemas acima
2. Consulte as issues do projeto
3. Crie uma nova issue se necessário

---

**Nota**: Este software é destinado para fins educacionais e de testes de rede legítimos. Sempre obtenha permissão adequada antes de escanear qualquer rede que não seja sua.
