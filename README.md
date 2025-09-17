# 🚀 Network Port Scanner - GUI

Uma aplicação desktop para varredura de portas de rede com interface gráfica intuitiva, desenvolvida em Python com PySimpleGUI.

## 📋 Descrição

O Network Port Scanner é uma ferramenta gráfica que permite escanear portas TCP e UDP em um endereço de rede especificado, exibindo os resultados de forma organizada e permitindo exportar os dados para análise.

## ✨ Funcionalidades

- 🔍 Varredura de portas TCP e UDP
- 🎯 Interface gráfica intuitiva
- ⚡ Varredura multi-threaded para maior velocidade
- 📊 Visualização de resultados em tabela
- 💾 Exportação de resultados para CSV
- 📈 Barra de progresso em tempo real
- 🧹 Limpeza de resultados
- ⏹️ Capacidade de parar varredura em andamento

## 🛠️ Pré-requisitos

- Python 3.6 ou superior

## 📦 Instalação

1. Clone ou baixe o repositório

2. baixe o sv_ttk

```bash
pip install sv-ttk
```

## 🚀 Como Usar

### Executando a Aplicação

Execute o arquivo principal:

```bash
python network-mapp-gui.py
```

### Interface Gráfica

A interface é dividida em várias seções:

1. **Configurações**:

   - **Alvo**: Digite o endereço IP ou hostname a ser escaneado
   - **Portas**: Especifique as portas (ex: "1-1000", "80,443,8080" ou "22")
   - **Protocolos**: Selecione TCP e/ou UDP
   - **Timeout**: Ajuste o tempo de espera para cada porta
   - **Threads**: Defina o número de threads paralelas

2. **Controles**:

   - 🚀 **Iniciar Varredura**: Inicia o scan
   - ⏹️ **Parar**: Interrompe a varredura em andamento
   - 💾 **Exportar**: Salva os resultados em CSV
   - 🧹 **Limpar**: Limpa os resultados atuais

3. **Resultados**:
   - Tabela com portas, protocolos, status e serviços
   - Log de atividades com timestamps

### Executando uma Varredura

1. Preencha o endereço de destino
2. Especifique o intervalo de portas desejado
3. Selecione os protocolos (TCP/UDP)
4. Ajuste timeout e threads conforme necessário
5. Clique em "🚀 Iniciar Varredura"
6. Aguarde a conclusão ou clique em "⏹️ Parar" para interromper

### Exportando Resultados

Após a varredura, clique em "💾 Exportar" para salvar os resultados em um arquivo CSV. Será solicitado o local e nome do arquivo.

## 📁 Estrutura do Projeto

```
network-scanner/
├── network-mapp-gui.py    # Aplicação principal com interface gráfica
├── network-mapp.py        # Módulo de varredura de portas (deve estar no mesmo diretório)
└── README.md              # Este arquivo
```

## ⚠️ Notas Importantes

- A ferramenta é para uso ético e educacional
- Sempre obtenha permissão antes de escanear redes
- Varreduras em redes não autorizadas podem ser ilegais
- A precisão dos resultados depende das configurações de rede e firewall

---

**Desenvolvido com Python e PySimpleGUI**
