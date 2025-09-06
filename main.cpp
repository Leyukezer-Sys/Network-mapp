#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QProgressBar>
#include <QComboBox>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTableView>
#include <QTimer>
#include <QStackedWidget>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QFontDatabase>

#include "scanner_thread.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent), scannerThread(nullptr) {
        loadCustomFonts();
        setupUI();
        setupTable();
        setupFilters();
        applyUserStyle();

        // Inicia na tela de configuração
        stackedWidget->setCurrentIndex(0);
    }

    ~MainWindow() {
        if (scannerThread) {
            if (scannerThread->isRunning()) {
                scannerThread->stopScan();
                scannerThread->wait();
            }
            delete scannerThread;
        }
    }

private:
    void loadCustomFonts() {
        // Tenta carregar fontes personalizadas
        int fontId = QFontDatabase::addApplicationFont(":/fonts/Orbitron-Regular.ttf");
        if (fontId != -1) {
            QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
            if (!fontFamilies.isEmpty()) {
                userFont = QFont(fontFamilies.at(0), 10);
            }
        }

        // Fallback para fontes padrão do sistema se as customizadas não estiverem disponíveis
        if (userFont.family().isEmpty()) {
            userFont = QFont("Arial", 10);
            userFont.setBold(true);
        }
    }

    void applyUserStyle() {
        // Estilo CSS futurista
        QString styleSheet = R"(
            QMainWindow {
                background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                    stop: 0 #0c0c0c, stop: 1 #1a1a2e);
            }

            QWidget {
                color: #e6e6e6;
                background: transparent;
            }

            QLabel {
                color: #00ffff;
                font-weight: bold;
            }

            QPushButton {
                background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                    stop: 0 #4a4a4a, stop: 1 #2a2a2a);
                border: 2px solid #00ffff;
                border-radius: 15px;
                color: #00ffff;
                padding: 8px 16px;
                font-weight: bold;
                min-height: 30px;
            }

            QPushButton:hover {
                background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                    stop: 0 #5a5a5a, stop: 1 #3a3a3a);
                border: 2px solid #00ffcc;
                color: #00ffcc;
            }

            QPushButton:pressed {
                background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                    stop: 0 #3a3a3a, stop: 1 #1a1a1a);
            }

            QPushButton:disabled {
                background: #333333;
                border: 2px solid #666666;
                color: #999999;
            }

            QLineEdit, QTextEdit, QComboBox {
                background: #2a2a2a;
                border: 2px solid #00ffff;
                border-radius: 8px;
                padding: 6px;
                color: #ffffff;
                selection-background-color: #00ffff;
                selection-color: #000000;
            }

            QLineEdit:focus, QTextEdit:focus, QComboBox:focus {
                border: 2px solid #00ffcc;
            }

            QGroupBox {
                font-weight: bold;
                border: 2px solid #00ffff;
                border-radius: 10px;
                margin-top: 1ex;
                padding-top: 10px;
                background: rgba(42, 42, 42, 150);
            }

            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top center;
                padding: 0 5px;
                color: #00ffff;
            }

            QProgressBar {
                border: 2px solid #00ffff;
                border-radius: 8px;
                text-align: center;
                background: #2a2a2a;
            }

            QProgressBar::chunk {
                background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                    stop: 0 #00ffff, stop: 1 #00bfff);
                border-radius: 6px;
            }

            QTableView {
                background: #2a2a2a;
                border: 2px solid #00ffff;
                border-radius: 8px;
                gridline-color: #444444;
                color: #ffffff;
                selection-background-color: #00ffff;
                selection-color: #000000;
            }

            QTableView::item:selected {
                background: #00ffff;
                color: #000000;
            }

            QHeaderView::section {
                background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                    stop: 0 #4a4a4a, stop: 1 #2a2a2a);
                color: #00ffff;
                padding: 4px;
                border: 1px solid #444444;
                font-weight: bold;
            }

            QCheckBox {
                color: #00ffff;
                spacing: 5px;
            }

            QCheckBox::indicator {
                width: 16px;
                height: 16px;
                border: 2px solid #00ffff;
                border-radius: 4px;
                background: #2a2a2a;
            }

            QCheckBox::indicator:checked {
                background: #00ffff;
                border: 2px solid #00ffcc;
            }

            QCheckBox::indicator:hover {
                border: 2px solid #00ffcc;
            }

            QComboBox::drop-down {
                subcontrol-origin: padding;
                subcontrol-position: top right;
                width: 20px;
                border-left: 1px solid #00ffff;
                border-top-right-radius: 8px;
                border-bottom-right-radius: 8px;
            }

            QComboBox::down-arrow {
                width: 12px;
                height: 12px;
                border-left: 4px solid transparent;
                border-right: 4px solid transparent;
                border-top: 6px solid #00ffff;
            }
        )";

        this->setStyleSheet(styleSheet);

        // Aplica a fonte futurista
        QApplication::setFont(userFont);

        // Configurações específicas para títulos
        QFont titleFont = userFont;
        titleFont.setPointSize(16);
        titleFont.setBold(true);

        // Aplica fontes específicas para diferentes elementos
        QList<QLabel*> labels = findChildren<QLabel*>();
        foreach (QLabel *label, labels) {
            if (label->text().contains("Scanner", Qt::CaseInsensitive) ||
                label->text().contains("Resultados", Qt::CaseInsensitive)) {
                label->setFont(titleFont);
            }
        }
    }

private slots:
    void onScanButtonClicked() {
        if (scannerThread && scannerThread->isRunning()) {
            // Se já está escaneando, para a varredura
            scannerThread->stopScan();
            scanButton->setText("🚀 Iniciar Varredura");
            stackedWidget->setCurrentIndex(0);
            return;
        }

        // Validação dos campos
        QString target = targetEdit->text();
        if (target.isEmpty()) {
            QMessageBox::warning(this, "Erro", "Por favor, informe um alvo para escanear.");
            return;
        }

        int startPort = startPortEdit->text().toInt();
        int endPort = endPortEdit->text().toInt();
        if (startPort <= 0 || endPort <= 0 || startPort > endPort || endPort > 65535) {
            QMessageBox::warning(this, "Erro", "Intervalo de portas inválido. Use valores entre 1 and 65535.");
            return;
        }

        if (!tcpCheckBox->isChecked() && !udpCheckBox->isChecked()) {
            QMessageBox::warning(this, "Erro", "Selecione pelo menos um protocolo (TCP ou UDP) para escanear.");
            return;
        }

        // Limpa a tabela antes de uma nova varredura
        model->removeRows(0, model->rowCount());

        // Prepara e inicia a thread de escaneamento
        if (scannerThread) {
            scannerThread->stopScan();
            scannerThread->wait();
            delete scannerThread;
        }

        scannerThread = new ScannerThread(this);
        scannerThread->setParameters(target, startPort, endPort,
                                     tcpCheckBox->isChecked(),
                                     udpCheckBox->isChecked(),
                                     1000); // Timeout de 1 segundo

        connect(scannerThread, &ScannerThread::portScanned, this, &MainWindow::onPortScanned);
        connect(scannerThread, &ScannerThread::progressUpdate, this, &MainWindow::onProgressUpdate);
        connect(scannerThread, &ScannerThread::scanFinished, this, &MainWindow::onScanFinished);
        connect(scannerThread, &ScannerThread::resultReady, this, &MainWindow::onResultReady);

        // Muda para a tela de progresso
        stackedWidget->setCurrentIndex(1);
        scanButton->setText("⏸ Parar Varredura");
        scannerThread->start();
    }

    void onProgressUpdate(int value) {
        progressBar->setValue(value);
        progressLabel->setText("Progresso: " + QString::number(value) + "%");
    }

    void onStopButtonClicked() {
        if (scannerThread && scannerThread->isRunning()) {
            scannerThread->stopScan();
            scanButton->setText("🚀 Iniciar Varredura");
            statusLabel->setText("Varredura interrompida pelo usuário.");
            stackedWidget->setCurrentIndex(0);
        }
    }

    void onClearButtonClicked() {
        // Limpa a tabela e volta para a tela de configuração
        model->removeRows(0, model->rowCount());
        stackedWidget->setCurrentIndex(0);
    }

    void onPortScanned(const PortInfo &portInfo) {
        // Adiciona os dados à tabela
        QList<QStandardItem*> rowItems;

        // Porta
        rowItems << new QStandardItem(QString::number(portInfo.portNumber));

        // Protocolo
        rowItems << new QStandardItem(portInfo.protocol);

        // Status
        QString statusText;
        QBrush statusColor;
        switch(portInfo.status) {
        case STATUS_OPEN:
            statusText = "ABERTA";
            statusColor = QBrush(QColor(0, 200, 0)); // Verde
            break;
        case STATUS_OPEN_UDP:
            statusText = "ABERTA (UDP)";
            statusColor = QBrush(QColor(0, 150, 0)); // Verde escuro
            break;
        case STATUS_CLOSED:
            statusText = "FECHADA";
            statusColor = QBrush(QColor(200, 0, 0)); // Vermelho
            break;
        case STATUS_FILTERED:
            statusText = "FILTRADA";
            statusColor = QBrush(QColor(200, 200, 0)); // Amarelo
            break;
        default:
            statusText = "DESCONHECIDO";
            statusColor = QBrush(Qt::gray);
        }

        QStandardItem *statusItem = new QStandardItem(statusText);
        statusItem->setBackground(statusColor);
        rowItems << statusItem;

        // Serviço
        rowItems << new QStandardItem(portInfo.service);

        model->appendRow(rowItems);
    }

    void onScanFinished() {
        scanButton->setText("🚀 Iniciar Varredura");
        statusLabel->setText("Varredura concluída. " + QString::number(model->rowCount()) + " portas escaneadas.");

        // Muda para a tela de resultados após um pequeno delay
        QTimer::singleShot(100, this, [this]() {
            stackedWidget->setCurrentIndex(2);
            showTutorial();
        });
    }

    void onResultReady(const QString &result) {
        resultsEdit->append(result);
    }

    void onFilterChanged() {
        QString protocolFilter = protocolFilterCombo->currentText();
        QString statusFilter = statusFilterCombo->currentText();

        // Filtra os dados na tabela
        for (int row = 0; row < model->rowCount(); ++row) {
            bool protocolMatch = (protocolFilter == "Todos") ||
                                 (model->item(row, 1)->text() == protocolFilter);
            bool statusMatch = (statusFilter == "Todos") ||
                               (model->item(row, 2)->text().contains(statusFilter, Qt::CaseInsensitive));

            tableView->setRowHidden(row, !(protocolMatch && statusMatch));
        }
    }

    void onExportButtonClicked() {
        QString fileName = QFileDialog::getSaveFileName(this, "Exportar Resultados", "", "CSV Files (*.csv);;Text Files (*.txt)");

        if (fileName.isEmpty()) {
            return;
        }

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Erro", "Não foi possível criar o arquivo: " + fileName);
            return;
        }

        QTextStream out(&file);

        // Cabeçalho
        out << "Porta,Protocolo,Status,Serviço\n";

        // Dados
        for (int row = 0; row < model->rowCount(); ++row) {
            if (!tableView->isRowHidden(row)) {
                out << model->item(row, 0)->text() << ","
                    << model->item(row, 1)->text() << ","
                    << model->item(row, 2)->text() << ","
                    << model->item(row, 3)->text() << "\n";
            }
        }

        file.close();
        QMessageBox::information(this, "Exportar", "Resultados exportados com sucesso para: " + fileName);
    }

    void showTutorial() {
        QMessageBox::information(this, "Tutorial - Tela de Resultados",
                                 "TUTORIAL DA TELA DE RESULTADOS:\n\n"
                                 "1. FILTROS: Use os menus suspensos para filtrar por protocolo (TCP/UDP) e status das portas\n"
                                 "2. ORDENAÇÃO: Clique nos cabeçalhos das colunas para ordenar os resultados\n"
                                 "3. CORES: Portas abertas (verde), fechadas (vermelho), filtradas (amarelo)\n"
                                 "4. EXPORTAR: Clique em 'Exportar Resultados' para salvar em CSV or TXT\n"
                                 "5. NOVA VARREDURA: Clique em 'Nova Varredura' para voltar à tela inicial\n"
                                 "6. ATUALIZAR: Use o botão 'Atualizar Tabela' caso os resultados não apareçam automaticamente\n\n"
                                 "Dica: Para uma análise rápida, filtre apenas por portas 'ABERTAS'");
    }

    // Função para atualizar a tabela manualmente
    void onRefreshTable() {
        // Força a atualização da visualização da tabela
        tableView->viewport()->update();

        // Reaplica os filtros atuais
        onFilterChanged();

        // Atualiza o status
        statusLabel->setText("Tabela atualizada. " + QString::number(model->rowCount()) + " portas escaneadas.");

        qDebug() << "Tabela atualizada manualmente";
    }

private:
    void setupUI() {
        setWindowTitle("NetWork Mainn - Scanner");
        setMinimumSize(1000, 700);

        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        // Widget empilhado para múltiplas telas
        stackedWidget = new QStackedWidget(this);
        mainLayout->addWidget(stackedWidget);

        // TELA 1: Configuração de varredura
        QWidget *configWidget = new QWidget();
        QVBoxLayout *configLayout = new QVBoxLayout(configWidget);

        QLabel *titleLabel = new QLabel("🌐 Scanner de Portas TCP/UDP");
        titleLabel->setAlignment(Qt::AlignCenter);
        QFont titleFont = userFont;
        titleFont.setPointSize(16);
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        configLayout->addWidget(titleLabel);

        QLabel *subtitleLabel = new QLabel("⚡ Configure os parâmetros da varredura de portas");
        subtitleLabel->setAlignment(Qt::AlignCenter);
        subtitleLabel->setStyleSheet("color: #00ffcc; font-size: 12px;");
        configLayout->addWidget(subtitleLabel);

        configLayout->addSpacing(20);

        // Painel de configuração
        QGroupBox *configGroup = new QGroupBox("🔧 Configurações de Varredura", this);
        QGridLayout *configGridLayout = new QGridLayout(configGroup);

        configGridLayout->addWidget(new QLabel("🌍 Endereço IP/Host:"), 0, 0);
        targetEdit = new QLineEdit("127.0.0.1", this);
        configGridLayout->addWidget(targetEdit, 0, 1);

        configGridLayout->addWidget(new QLabel("🚪 Porta inicial:"), 1, 0);
        startPortEdit = new QLineEdit("1", this);
        configGridLayout->addWidget(startPortEdit, 1, 1);

        configGridLayout->addWidget(new QLabel("🚪 Porta final:"), 2, 0);
        endPortEdit = new QLineEdit("100", this);
        configGridLayout->addWidget(endPortEdit, 2, 1);

        tcpCheckBox = new QCheckBox("🔗 TCP", this);
        tcpCheckBox->setChecked(true);
        configGridLayout->addWidget(tcpCheckBox, 0, 2);

        udpCheckBox = new QCheckBox("📡 UDP", this);
        configGridLayout->addWidget(udpCheckBox, 1, 2);

        configLayout->addWidget(configGroup);

        configLayout->addStretch();

        scanButton = new QPushButton("🚀 Iniciar Varredura", this);
        scanButton->setMinimumHeight(50);
        scanButton->setStyleSheet(R"(
            QPushButton {
                background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                    stop: 0 #00ffff, stop: 1 #00bfff);
                border: 2px solid #00ffff;
                border-radius: 20px;
                color: #000000;
                font-weight: bold;
                font-size: 14px;
            }
            QPushButton:hover {
                background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                    stop: 0 #00ffcc, stop: 1 #0099cc);
            }
        )");
        connect(scanButton, &QPushButton::clicked, this, &MainWindow::onScanButtonClicked);
        configLayout->addWidget(scanButton);

        stackedWidget->addWidget(configWidget);

        // TELA 2: Progresso da varredura
        QWidget *progressWidget = new QWidget();
        QVBoxLayout *progressLayout = new QVBoxLayout(progressWidget);

        QLabel *progressTitle = new QLabel("🔍 Varredura em andamento...");
        progressTitle->setAlignment(Qt::AlignCenter);
        progressTitle->setFont(titleFont);
        progressLayout->addWidget(progressTitle);

        progressLayout->addSpacing(40);

        progressBar = new QProgressBar(this);
        progressBar->setValue(0);
        progressBar->setMinimumHeight(30);
        progressBar->setTextVisible(false);
        progressLayout->addWidget(progressBar);

        progressLabel = new QLabel("Progresso: 0%");
        progressLabel->setAlignment(Qt::AlignCenter);
        progressLayout->addWidget(progressLabel);

        progressLayout->addStretch();

        QPushButton *stopButton = new QPushButton("🟥️ Parar Varredura", this);
        connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopButtonClicked);
        progressLayout->addWidget(stopButton);

        stackedWidget->addWidget(progressWidget);

        // TELA 3: Resultados
        QWidget *resultsWidget = new QWidget();
        QVBoxLayout *resultsLayout = new QVBoxLayout(resultsWidget);

        QLabel *resultsTitle = new QLabel("📊 Resultados da Varredura");
        resultsTitle->setAlignment(Qt::AlignCenter);
        resultsTitle->setFont(titleFont);
        resultsLayout->addWidget(resultsTitle);

        // Painel de filtros
        QGroupBox *filterGroup = new QGroupBox("🔧 Filtros", this);
        QHBoxLayout *filterLayout = new QHBoxLayout(filterGroup);

        filterLayout->addWidget(new QLabel("🌐 Protocolo:"));
        protocolFilterCombo = new QComboBox(this);
        protocolFilterCombo->addItem("Todos");
        protocolFilterCombo->addItem("TCP");
        protocolFilterCombo->addItem("UDP");
        connect(protocolFilterCombo, &QComboBox::currentTextChanged, this, &MainWindow::onFilterChanged);
        filterLayout->addWidget(protocolFilterCombo);

        filterLayout->addWidget(new QLabel(" Status:"));
        statusFilterCombo = new QComboBox(this);
        statusFilterCombo->addItem("Todos");
        statusFilterCombo->addItem("Aberta");
        statusFilterCombo->addItem("Fechada");
        statusFilterCombo->addItem("Filtrada");
        connect(statusFilterCombo, &QComboBox::currentTextChanged, this, &MainWindow::onFilterChanged);
        filterLayout->addWidget(statusFilterCombo);

        filterLayout->addStretch();

        // Botão de refresh/atualizar tabela (adicionado à direita)
        QPushButton *refreshButton = new QPushButton("🔄 Atualizar Tabela", this);
        refreshButton->setToolTip("Força a atualização da visualização da tabela");
        connect(refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshTable);
        filterLayout->addWidget(refreshButton);

        resultsLayout->addWidget(filterGroup);

        // Tabela de resultados
        tableView = new QTableView(this);
        resultsLayout->addWidget(tableView);

        // Área de log (adicionada)
        resultsEdit = new QTextEdit(this);
        resultsEdit->setMaximumHeight(100);
        resultsEdit->setReadOnly(true);
        resultsLayout->addWidget(resultsEdit);

        // Barra de status
        statusLabel = new QLabel("✅ Pronto para escanear.", this);
        resultsLayout->addWidget(statusLabel);

        // Botões de ação
        QHBoxLayout *buttonLayout = new QHBoxLayout();

        QPushButton *newScanButton = new QPushButton("♻ Nova Varredura", this);
        connect(newScanButton, &QPushButton::clicked, this, &MainWindow::onClearButtonClicked);
        buttonLayout->addWidget(newScanButton);

        QPushButton *exportButton = new QPushButton("💾 Exportar Resultados", this);
        connect(exportButton, &QPushButton::clicked, this, &MainWindow::onExportButtonClicked);
        buttonLayout->addWidget(exportButton);

        resultsLayout->addLayout(buttonLayout);

        stackedWidget->addWidget(resultsWidget);

        setCentralWidget(centralWidget);
    }

    void setupTable() {
        model = new QStandardItemModel(0, 4, this);
        model->setHeaderData(0, Qt::Horizontal, "Porta");
        model->setHeaderData(1, Qt::Horizontal, "Protocolo");
        model->setHeaderData(2, Qt::Horizontal, "Status");
        model->setHeaderData(3, Qt::Horizontal, "Serviço");

        tableView->setModel(model);
        tableView->setSortingEnabled(true);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    void setupFilters() {
        // Conecta os filtros (já conectado no setupUI com a nova sintaxe)
    }

    // UI elements
    QStackedWidget *stackedWidget;
    QLineEdit *targetEdit;
    QLineEdit *startPortEdit;
    QLineEdit *endPortEdit;
    QCheckBox *tcpCheckBox;
    QCheckBox *udpCheckBox;
    QTextEdit *resultsEdit;
    QProgressBar *progressBar;
    QPushButton *scanButton;
    QLabel *statusLabel;
    QLabel *progressLabel;
    QComboBox *protocolFilterCombo;
    QComboBox *statusFilterCombo;
    QTableView *tableView;
    QStandardItemModel *model;

    ScannerThread *scannerThread;
    QFont userFont;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"
