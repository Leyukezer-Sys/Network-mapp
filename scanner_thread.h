#ifndef SCANNER_THREAD_H
#define SCANNER_THREAD_H

#include <QThread>
#include <QObject>
#include <QtNetwork>
#include <QHostInfo>
#include <QDebug>

// Enum para status das portas
enum PortStatus {
    STATUS_OPEN,
    STATUS_CLOSED,
    STATUS_FILTERED,
    STATUS_OPEN_UDP,
    STATUS_UNKNOWN
};

// Estrutura para armazenar informações da porta
struct PortInfo {
    int portNumber;
    QString protocol;
    PortStatus status;
    QString service;
};

class ScannerThread : public QThread {
    Q_OBJECT
public:
    explicit ScannerThread(QObject *parent = nullptr);
    ~ScannerThread();

    void setParameters(const QString &target, int startPort, int endPort,
                       bool tcpSelected, bool udpSelected, int timeout);
    void stopScan();

signals:
    void portScanned(const PortInfo &portInfo);
    void progressUpdate(int value);
    void scanFinished();
    void resultReady(const QString &result);

protected:
    void run() override;

private:
    QString target;
    int startPort;
    int endPort;
    bool tcpSelected;
    bool udpSelected;
    int timeout;
    volatile bool stopRequested;

    QString getServiceName(int port, const QString &protocol);
};

#endif // SCANNER_THREAD_H
