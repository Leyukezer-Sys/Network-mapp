# 🌐 NetWork Mainn - Scanner de Portas

Um scanner de portas TCP/UDP moderno e intuitivo com interface.

## 📋 Pré-requisitos

Qt Framework Develop

## 🚀 Como Compilar e Executar

### Método 1: Terminal (Recomendado)
```bash
# Navegue até a pasta do projeto
cd network-mapp

# Compile o projeto
qmake6 port_scanner.pro  # Para Qt6
# ou
qmake port_scanner.pro   # Para Qt5

make

# Execute
./port_scanner
```

### Método 2: Qt Creator
1. Abra o Qt Creator
2. File → Open File or Project
3. Selecione o arquivo `port_scanner.pro`
4. Clique em "Configure Project"
5. Pressione Ctrl+R para executar

## 🎯 Como Usar

1. **Configuração**: 
   - Digite o endereço IP ou hostname do alvo
   - Defina o intervalo de portas a ser escaneado
   - Selecione os protocolos (TCP/UDP)

2. **Varredura**:
   - Clique em "🚀 Iniciar Varredura"
   - Acompanhe o progresso na barra de progresso

3. **Resultados**:
   - Use os filtros para visualizar portas por protocolo ou status
   - Exporte os resultados para CSV ou TXT
   - Clique nos cabeçalhos para ordenar os resultados

## 🎨 Características

- Interface com tema escuro
- Visualização em tempo real dos resultados
- Sistema de filtros avançado
- Exportação de resultados
- Detecção automática de alguns serviços
- Suporte a varreduras TCP e UDP

## 🔧 Solução de Problemas

### Erro: "qmake não encontrado"
```bash
# Instale o Qt development tools
sudo apt install qt6-base-dev qt6-tools-dev-tools
```

### Erro: Bibliotecas faltando
```bash
# Instale dependências comuns
sudo apt install libgl1-mesa-dev
```

### Erro de compilação
Certifique-se de que todas as dependências estão instaladas e tente:
```bash
make clean
qmake
make
```

## 📦 Estrutura do Projeto

```
port_scanner/
├── port_scanner.pro    # Arquivo de projeto
├── main.cpp            # Código principal
├── scanner_thread.h    # Cabeçalho da thread de scanner
├── scanner_thread.cpp  # Implementação da thread
└── README.md           # Este arquivo
```

---

**Desenvolvido com Qt Framework** - Versão compatível: Qt 5.12+ ou Qt 6.0+
