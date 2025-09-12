#include "scanner_thread.cpp"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Scroll.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_draw.H>

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <string>
#include <sstream>
#include <iomanip>

// Estrutura para informações da porta
struct PortInfo
{
    int portNumber;
    std::string protocol;
    int status;
    std::string service;
};

// Thread de scanner (simplificada para exemplo)
class ScannerThread
{
public:
    ScannerThread() : stopFlag(false) {}

    void setParameters(const std::string &target, int startPort, int endPort,
                       bool scanTCP, bool scanUDP, int timeout)
    {
        this->target = target;
        this->startPort = startPort;
        this->endPort = endPort;
        this->scanTCP = scanTCP;
        this->scanUDP = scanUDP;
        this->timeout = timeout;
    }

    void start()
    {
        stopFlag = false;
        thread = std::thread(&ScannerThread::run, this);
    }

    void stopScan()
    {
        stopFlag = true;
    }

    void wait()
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    bool isRunning()
    {
        return thread.joinable() && !stopFlag;
    }

    // Sinais (simulados com callbacks)
    std::function<void(const PortInfo &)> portScannedCallback;
    std::function<void(int)> progressUpdateCallback;
    std::function<void()> scanFinishedCallback;
    std::function<void(const std::string &)> resultReadyCallback;

private:
    void run()
    {
        int totalPorts = endPort - startPort + 1;
        int scannedPorts = 0;

        for (int port = startPort; port <= endPort && !stopFlag; port++)
        {
            // Simulação de escaneamento
            PortInfo info;
            info.portNumber = port;
            info.protocol = scanTCP ? "TCP" : "UDP";

            // Simula diferentes status
            if (port % 10 == 0)
            {
                info.status = 0; // ABERTA
                info.service = "HTTP";
            }
            else if (port % 5 == 0)
            {
                info.status = 2; // FILTRADA
                info.service = "Unknown";
            }
            else
            {
                info.status = 1; // FECHADA
                info.service = "Unknown";
            }

            // Chama callback
            if (portScannedCallback)
            {
                portScannedCallback(info);
            }

            // Atualiza progresso
            scannedPorts++;
            int progress = (scannedPorts * 100) / totalPorts;
            if (progressUpdateCallback)
            {
                progressUpdateCallback(progress);
            }

            // Pequeno delay para simulação
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (scanFinishedCallback)
        {
            scanFinishedCallback();
        }
    }

    std::thread thread;
    std::atomic<bool> stopFlag;
    std::string target;
    int startPort, endPort;
    bool scanTCP, scanUDP;
    int timeout;
};

// Tabela personalizada para exibir resultados
class ResultsTable : public Fl_Table
{
private:
    std::vector<std::vector<std::string>> data;
    std::vector<int> colWidths;

public:
    ResultsTable(int X, int Y, int W, int H, const char *L = 0) : Fl_Table(X, Y, W, H, L)
    {
        cols(4);
        col_header(1);
        col_resize(1);
        col_width_all(80);

        // Configura larguras das colunas
        col_width(0, 80);  // Porta
        col_width(1, 100); // Protocolo
        col_width(2, 120); // Status
        col_width(3, 200); // Serviço

        // Cabeçalhos
        col_header(1);
        headers.push_back("Porta");
        headers.push_back("Protocolo");
        headers.push_back("Status");
        headers.push_back("Serviço");

        end();
    }

    void addRow(const std::vector<std::string> &row)
    {
        data.push_back(row);
        rows(data.size());
        redraw();
    }

    void clear()
    {
        data.clear();
        rows(0);
        redraw();
    }

    void draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) override
    {
        static char s[40];

        switch (context)
        {
        case CONTEXT_STARTPAGE:
            fl_font(FL_HELVETICA, 12);
            return;

        case CONTEXT_COL_HEADER:
            fl_push_clip(X, Y, W, H);
            {
                fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, col_header_color());
                fl_color(FL_BLACK);
                if (C < headers.size())
                {
                    fl_draw(headers[C].c_str(), X, Y, W, H, FL_ALIGN_CENTER);
                }
            }
            fl_pop_clip();
            return;

        case CONTEXT_CELL:
        {
            fl_push_clip(X, Y, W, H);
            {
                // Cor de fundo
                Fl_Color bgcolor = FL_WHITE;
                if (R < data.size() && C < data[R].size())
                {
                    if (data[R][2] == "ABERTA")
                    {
                        bgcolor = fl_rgb_color(200, 255, 200); // Verde claro
                    }
                    else if (data[R][2] == "FILTRADA")
                    {
                        bgcolor = fl_rgb_color(255, 255, 200); // Amarelo claro
                    }
                    else if (data[R][2] == "FECHADA")
                    {
                        bgcolor = fl_rgb_color(255, 200, 200); // Vermelho claro
                    }
                }

                fl_color(bgcolor);
                fl_rectf(X, Y, W, H);

                // Texto
                fl_color(FL_BLACK);
                if (R < data.size() && C < data[R].size())
                {
                    fl_draw(data[R][C].c_str(), X, Y, W, H, FL_ALIGN_LEFT);
                }

                // Borda
                fl_color(FL_LIGHT2);
                fl_rect(X, Y, W, H);
            }
            fl_pop_clip();
            return;
        }

        default:
            return;
        }
    }

private:
    std::vector<std::string> headers;
};

// Classe principal da janela
class MainWindow
{
private:
    Fl_Window *window;
    Fl_Group *configGroup;
    Fl_Group *progressGroup;
    Fl_Group *resultsGroup;

    // Controles da tela de configuração
    Fl_Input *targetInput;
    Fl_Int_Input *startPortInput;
    Fl_Int_Input *endPortInput;
    Fl_Check_Button *tcpCheck;
    Fl_Check_Button *udpCheck;
    Fl_Button *scanButton;

    // Controles da tela de progresso
    Fl_Progress *progressBar;
    Fl_Box *progressLabel;
    Fl_Button *stopButton;

    // Controles da tela de resultados
    ResultsTable *resultsTable;
    Fl_Text_Display *logDisplay;
    Fl_Text_Buffer *logBuffer;
    Fl_Choice *protocolFilter;
    Fl_Choice *statusFilter;
    Fl_Button *newScanButton;
    Fl_Button *exportButton;
    Fl_Button *refreshButton;
    Fl_Box *statusLabel;

    ScannerThread scannerThread;
    std::vector<PortInfo> portResults;

public:
    MainWindow()
    {
        // Cria janela principal
        window = new Fl_Window(1000, 700, "NetWork Mainn - Scanner de Portas");
        window->begin();

        // Configura o estilo básico
        Fl::set_font(FL_HELVETICA, "Arial");

        // Cria os grupos para as diferentes telas
        configGroup = new Fl_Group(0, 0, 1000, 700);
        progressGroup = new Fl_Group(0, 0, 1000, 700);
        resultsGroup = new Fl_Group(0, 0, 1000, 700);

        setupConfigScreen();
        setupProgressScreen();
        setupResultsScreen();

        // Mostra a tela de configuração inicialmente
        configGroup->show();
        progressGroup->hide();
        resultsGroup->hide();

        window->end();
        window->resizable(window);

        // Configura callbacks da thread
        scannerThread.portScannedCallback = [this](const PortInfo &info)
        {
            // Usar Fl::awake para thread safety
            Fl::awake([](void *data)
                      {
        auto self = static_cast<MainWindow*>(data);
        // Processar informação da porta
        self->onPortScanned(info); }, this);
        };

        scannerThread.progressUpdateCallback = [this](int progress)
        {
            Fl::awake([](void *data)
                      {
        auto self = static_cast<MainWindow*>(data);
        self->onProgressUpdate(progress); }, this);
        };

        scannerThread.scanFinishedCallback = [this]()
        {
            Fl::awake([](void *data)
                      {
        auto self = static_cast<MainWindow*>(data);
        self->onScanFinished(); }, this);
        };

        scannerThread.resultReadyCallback = [this](const std::string &result)
        {
            Fl::awake([](void *data)
                      {
        auto self = static_cast<MainWindow*>(data);
        self->onResultReady(result); }, this);
        };
    }

    ~MainWindow()
    {
        scannerThread.stopScan();
        scannerThread.wait();
    }

    void show()
    {
        window->show();
    }

    int run()
    {
        return Fl::run();
    }

private:
    int progressValue;

    void setupConfigScreen()
    {
        configGroup->begin();

        // Título
        Fl_Box *title = new Fl_Box(0, 20, 1000, 40, "🌐 Scanner de Portas TCP/UDP");
        title->align(FL_ALIGN_CENTER);
        title->labelfont(FL_HELVETICA_BOLD);
        title->labelsize(16);

        Fl_Box *subtitle = new Fl_Box(0, 60, 1000, 30, "⚡ Configure os parâmetros da varredura de portas");
        subtitle->align(FL_ALIGN_CENTER);
        subtitle->labelsize(12);

        // Grupo de configurações
        Fl_Group *settingsGroup = new Fl_Group(100, 120, 800, 200, "🔧 Configurações de Varredura");
        settingsGroup->box(FL_ENGRAVED_BOX);
        settingsGroup->align(FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        settingsGroup->labelfont(FL_HELVETICA_BOLD);
        settingsGroup->begin();

        // Campos de entrada
        Fl_Box *targetLabel = new Fl_Box(120, 160, 200, 30, "🌍 Endereço IP/Host:");
        targetLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        targetInput = new Fl_Input(350, 160, 300, 30);
        targetInput->value("127.0.0.1");

        Fl_Box *startPortLabel = new Fl_Box(120, 200, 200, 30, "🚪 Porta inicial:");
        startPortLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        startPortInput = new Fl_Int_Input(350, 200, 100, 30);
        startPortInput->value("1");

        Fl_Box *endPortLabel = new Fl_Box(120, 240, 200, 30, "🚪 Porta final:");
        endPortLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        endPortInput = new Fl_Int_Input(350, 240, 100, 30);
        endPortInput->value("100");

        // Checkboxes
        tcpCheck = new Fl_Check_Button(500, 200, 100, 30, "🔗 TCP");
        tcpCheck->value(1);

        udpCheck = new Fl_Check_Button(500, 240, 100, 30, "📡 UDP");

        settingsGroup->end();

        // Botão de scan
        scanButton = new Fl_Button(400, 350, 200, 50, "🚀 Iniciar Varredura");
        scanButton->callback(scanButtonCallback, this);

        configGroup->end();
    }

    void setupProgressScreen()
    {
        progressGroup->begin();

        // Título
        Fl_Box *title = new Fl_Box(0, 20, 1000, 40, "🔍 Varredura em andamento...");
        title->align(FL_ALIGN_CENTER);
        title->labelfont(FL_HELVETICA_BOLD);
        title->labelsize(16);

        // Barra de progresso
        progressBar = new Fl_Progress(200, 100, 600, 30);
        progressBar->minimum(0);
        progressBar->maximum(100);
        progressBar->value(0);

        progressLabel = new Fl_Box(0, 140, 1000, 30, "Progresso: 0%");
        progressLabel->align(FL_ALIGN_CENTER);

        // Botão de parar
        stopButton = new Fl_Button(400, 200, 200, 40, "⏹️ Parar Varredura");
        stopButton->callback(stopButtonCallback, this);

        progressGroup->end();
    }

    void setupResultsScreen()
    {
        resultsGroup->begin();

        // Título
        Fl_Box *title = new Fl_Box(0, 20, 1000, 40, "📊 Resultados da Varredura");
        title->align(FL_ALIGN_CENTER);
        title->labelfont(FL_HELVETICA_BOLD);
        title->labelsize(16);

        // Grupo de filtros
        Fl_Group *filterGroup = new Fl_Group(50, 70, 900, 50, "🔧 Filtros");
        filterGroup->box(FL_ENGRAVED_BOX);
        filterGroup->align(FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        filterGroup->begin();

        Fl_Box *protocolLabel = new Fl_Box(60, 85, 80, 30, "🌐 Protocolo:");
        protocolLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        protocolFilter = new Fl_Choice(150, 85, 150, 30);
        protocolFilter->add("Todos");
        protocolFilter->add("TCP");
        protocolFilter->add("UDP");
        protocolFilter->value(0);
        protocolFilter->callback(filterCallback, this);

        Fl_Box *statusLabel = new Fl_Box(320, 85, 80, 30, "📊 Status:");
        statusLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        statusFilter = new Fl_Choice(410, 85, 150, 30);
        statusFilter->add("Todos");
        statusFilter->add("Aberta");
        statusFilter->add("Fechada");
        statusFilter->add("Filtrada");
        statusFilter->value(0);
        statusFilter->callback(filterCallback, this);

        refreshButton = new Fl_Button(700, 85, 150, 30, "🔄 Atualizar Tabela");
        refreshButton->callback(refreshCallback, this);

        filterGroup->end();

        // Tabela de resultados
        resultsTable = new ResultsTable(50, 130, 900, 350);

        // Área de log
        logBuffer = new Fl_Text_Buffer();
        logDisplay = new Fl_Text_Display(50, 500, 900, 100);
        logDisplay->buffer(logBuffer);
        logDisplay->textfont(FL_COURIER);

        // Status label
        statusLabel = new Fl_Box(50, 610, 900, 30, "✅ Pronto para escanear.");
        statusLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        // Botões
        newScanButton = new Fl_Button(200, 650, 200, 30, "🔄 Nova Varredura");
        newScanButton->callback(newScanCallback, this);

        exportButton = new Fl_Button(500, 650, 200, 30, "💾 Exportar Resultados");
        exportButton->callback(exportCallback, this);

        resultsGroup->end();
    }

    void showScreen(Fl_Group *screen)
    {
        configGroup->hide();
        progressGroup->hide();
        resultsGroup->hide();
        screen->show();
        window->redraw();
    }

    void onScanButton()
    {
        // Validação dos campos
        std::string target = targetInput->value();
        if (target.empty())
        {
            fl_alert("Por favor, informe um alvo para escanear.");
            return;
        }

        int startPort = atoi(startPortInput->value());
        int endPort = atoi(endPortInput->value());
        if (startPort <= 0 || endPort <= 0 || startPort > endPort || endPort > 65535)
        {
            fl_alert("Intervalo de portas inválido. Use valores entre 1 e 65535.");
            return;
        }

        if (tcpCheck->value() == 0 && udpCheck->value() == 0)
        {
            fl_alert("Selecione pelo menos um protocolo (TCP ou UDP) para escanear.");
            return;
        }

        // Limpa resultados anteriores
        portResults.clear();
        resultsTable->clear();
        logBuffer->text("");

        // Configura a thread
        scannerThread.setParameters(target, startPort, endPort,
                                    tcpCheck->value(), udpCheck->value(), 1000);

        // Muda para a tela de progresso
        showScreen(progressGroup);
        scanButton->label("⏸ Parar Varredura");

        // Inicia a thread
        scannerThread.start();
    }

    void onStopButton()
    {
        scannerThread.stopScan();
        scanButton->label("🚀 Iniciar Varredura");
        statusLabel->label("Varredura interrompida pelo usuário.");
        showScreen(configGroup);
    }

    void onPortScanned(const PortInfo &portInfo)
    {
        // Converte status para texto
        std::string statusText;
        switch (portInfo.status)
        {
        case 0:
            statusText = "ABERTA";
            break;
        case 1:
            statusText = "FECHADA";
            break;
        case 2:
            statusText = "FILTRADA";
            break;
        default:
            statusText = "DESCONHECIDO";
        }

        // Adiciona à tabela
        resultsTable->addRow({std::to_string(portInfo.portNumber),
                              portInfo.protocol,
                              statusText,
                              portInfo.service});

        // Adiciona ao log
        std::string logEntry = "Porta " + std::to_string(portInfo.portNumber) +
                               "/" + portInfo.protocol + ": " + statusText + "\n";
        logBuffer->append(logEntry.c_str());
    }

    void onProgressUpdate(int progress)
    {
        progressBar->value(progress);
        char label[50];
        snprintf(label, sizeof(label), "Progresso: %d%%", progress);
        progressLabel->copy_label(label);
    }

    void onScanFinished()
    {
        scanButton->label("🚀 Iniciar Varredura");

        char status[100];
        snprintf(status, sizeof(status), "Varredura concluída. %lu portas escaneadas.", portResults.size());
        statusLabel->copy_label(status);

        // Muda para a tela de resultados
        showScreen(resultsGroup);

        // Mostra tutorial
        fl_message("TUTORIAL DA TELA DE RESULTADOS:\n\n"
                   "1. FILTROS: Use os menus suspensos para filtrar por protocolo (TCP/UDP) e status das portas\n"
                   "2. CORES: Portas abertas (verde), fechadas (vermelho), filtradas (amarelo)\n"
                   "3. EXPORTAR: Clique em 'Exportar Resultados' para salvar em CSV ou TXT\n"
                   "4. NOVA VARREDURA: Clique em 'Nova Varredura' para voltar à tela inicial\n"
                   "5. ATUALIZAR: Use o botão 'Atualizar Tabela' caso os resultados não apareçam automaticamente\n\n"
                   "Dica: Para uma análise rápida, filtre apenas por portas 'ABERTAS'");
    }

    void onFilterChanged()
    {
        // Implementação simplificada de filtro
        // Na prática, você precisaria filtrar os dados na tabela
        resultsTable->redraw();
    }

    void onRefreshTable()
    {
        resultsTable->redraw();
        statusLabel->label("Tabela atualizada.");
    }

    void onNewScan()
    {
        showScreen(configGroup);
    }

    void onExport()
    {
        Fl_Native_File_Chooser chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
        chooser.title("Exportar Resultados");
        chooser.filter("CSV Files\t*.csv\nText Files\t*.txt");

        if (chooser.show() == 0)
        {
            std::string filename = chooser.filename();
            std::ofstream file(filename);

            if (file.is_open())
            {
                file << "Porta,Protocolo,Status,Serviço\n";

                for (const auto &info : portResults)
                {
                    std::string statusText;
                    switch (info.status)
                    {
                    case 0:
                        statusText = "ABERTA";
                        break;
                    case 1:
                        statusText = "FECHADA";
                        break;
                    case 2:
                        statusText = "FILTRADA";
                        break;
                    default:
                        statusText = "DESCONHECIDO";
                    }

                    file << info.portNumber << ","
                         << info.protocol << ","
                         << statusText << ","
                         << info.service << "\n";
                }

                file.close();
                fl_message("Resultados exportados com sucesso para: %s", filename.c_str());
            }
            else
            {
                fl_alert("Não foi possível criar o arquivo: %s", filename.c_str());
            }
        }
    }

    // Callbacks estáticos
    static void scanButtonCallback(Fl_Widget *w, void *data)
    {
        MainWindow *win = static_cast<MainWindow *>(data);
        win->onScanButton();
    }

    static void stopButtonCallback(Fl_Widget *w, void *data)
    {
        MainWindow *win = static_cast<MainWindow *>(data);
        win->onStopButton();
    }

    static void filterCallback(Fl_Widget *w, void *data)
    {
        MainWindow *win = static_cast<MainWindow *>(data);
        win->onFilterChanged();
    }

    static void refreshCallback(Fl_Widget *w, void *data)
    {
        MainWindow *win = static_cast<MainWindow *>(data);
        win->onRefreshTable();
    }

    static void newScanCallback(Fl_Widget *w, void *data)
    {
        MainWindow *win = static_cast<MainWindow *>(data);
        win->onNewScan();
    }

    static void exportCallback(Fl_Widget *w, void *data)
    {
        MainWindow *win = static_cast<MainWindow *>(data);
        win->onExport();
    }
};

int main(int argc, char *argv[])
{
    MainWindow win;
    win.show();
    return win.run();
}