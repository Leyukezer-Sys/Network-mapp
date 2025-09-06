#include "scanner_thread.h"
#include <QMap>

ScannerThread::ScannerThread(QObject *parent)
    : QThread(parent), stopRequested(false) {
    qDebug() << "ScannerThread criado";
}

ScannerThread::~ScannerThread() {
    qDebug() << "ScannerThread destruído";
    stopScan();
    wait(); // Espera a thread terminar
}

void ScannerThread::setParameters(const QString &target, int startPort, int endPort,
                                  bool tcpSelected, bool udpSelected, int timeout) {
    this->target = target;
    this->startPort = startPort;
    this->endPort = endPort;
    this->tcpSelected = tcpSelected;
    this->udpSelected = udpSelected;
    this->timeout = timeout;
}

void ScannerThread::stopScan() {
    stopRequested = true;
    qDebug() << "Parando varredura...";
}

QString ScannerThread::getServiceName(int port, const QString &protocol) {
    // Mapa de serviços comuns (mantido igual)
    static QMap<int, QString> commonServices;
    if (commonServices.isEmpty()) {
        commonServices[20] = "FTP Data";
        commonServices[21] = "FTP Control";
        commonServices[22] = "SSH";
        commonServices[23] = "Telnet";
        commonServices[25] = "SMTP";
        commonServices[53] = "DNS";
        commonServices[80] = "HTTP";
        commonServices[110] = "POP3";
        commonServices[143] = "IMAP";
        commonServices[443] = "HTTPS";
        commonServices[465] = "SMTPS";
        commonServices[587] = "SMTP Submission";
        commonServices[993] = "IMAPS";
        commonServices[995] = "POP3S";
        commonServices[3389] = "RDP";
    }

    return commonServices.value(port, "Unknown");
}

void ScannerThread::run() {
    qDebug() << "Iniciando execução da thread...";
    stopRequested = false;
    int totalPorts = endPort - startPort + 1;

    if (totalPorts <= 0) {
        emit resultReady("Erro: Intervalo de portas inválido.");
        emit scanFinished();
        return;
    }

    int totalScans = (tcpSelected ? totalPorts : 0) + (udpSelected ? totalPorts : 0);
    if (totalScans == 0) {
        emit resultReady("Erro: Nenhum protocolo selecionado.");
        emit scanFinished();
        return;
    }

    int scannedPorts = 0;

    // Resolve o endereço de destino
    QHostInfo hostInfo = QHostInfo::fromName(target);
    if (hostInfo.addresses().isEmpty()) {
        emit resultReady("Erro: Não foi possível resolver o endereço " + target);
        emit scanFinished();
        return;
    }

    QHostAddress address = hostInfo.addresses().first();
    QString ipString = address.toString();
    emit resultReady("Iniciando varredura em: " + ipString + "\n");

    // Varredura TCP
    if (tcpSelected) {
        emit resultReady("\n[+] Varredura TCP iniciada...\n");

        for (int port = startPort; port <= endPort && !stopRequested; port++) {
            PortInfo portInfo;
            portInfo.portNumber = port;
            portInfo.protocol = "TCP";
            portInfo.service = getServiceName(port, "TCP");

            QTcpSocket socket;
            socket.connectToHost(ipString, port);

            if (socket.waitForConnected(timeout)) {
                portInfo.status = STATUS_OPEN;
                socket.disconnectFromHost();
            } else {
                if (socket.error() == QTcpSocket::ConnectionRefusedError) {
                    portInfo.status = STATUS_CLOSED;
                } else {
                    portInfo.status = STATUS_FILTERED;
                }
            }

            emit portScanned(portInfo);

            scannedPorts++;
            int progress = (scannedPorts * 100) / totalScans;
            emit progressUpdate(progress);
        }
    }

    // Varredura UDP
    if (udpSelected && !stopRequested) {
        emit resultReady("\n[+] Varredura UDP iniciada...\n");

        for (int port = startPort; port <= endPort && !stopRequested; port++) {
            PortInfo portInfo;
            portInfo.portNumber = port;
            portInfo.protocol = "UDP";
            portInfo.service = getServiceName(port, "UDP");

            QUdpSocket udpSocket;
            QByteArray data = "UDP_SCAN";

            // Envia um pacote UDP
            udpSocket.writeDatagram(data, address, port);

            // Espera por uma resposta
            if (udpSocket.waitForReadyRead(timeout)) {
                portInfo.status = STATUS_OPEN_UDP;
            } else {
                portInfo.status = STATUS_FILTERED;
            }

            emit portScanned(portInfo);

            scannedPorts++;
            int progress = (scannedPorts * 100) / totalScans;
            emit progressUpdate(progress);
        }
    }

    if (!stopRequested) {
        emit resultReady("\n[+] Varredura concluída!\n");
    } else {
        emit resultReady("\n[+] Varredura interrompida!\n");
    }

    emit progressUpdate(100);
    emit scanFinished();
    qDebug() << "Thread finalizada";
}
