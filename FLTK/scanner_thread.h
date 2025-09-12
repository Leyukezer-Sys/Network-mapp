#ifndef SCANNER_THREAD_H
#define SCANNER_THREAD_H

#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <map>

// Status das portas
#define STATUS_OPEN 0
#define STATUS_OPEN_UDP 1
#define STATUS_CLOSED 2
#define STATUS_FILTERED 3

// Estrutura para informações da porta
struct PortInfo {
    int portNumber;
    std::string protocol;
    int status;
    std::string service;
};

class ScannerThread {
public:
    ScannerThread();
    ~ScannerThread();

    void setParameters(const std::string &target, int startPort, int endPort,
                      bool tcpSelected, bool udpSelected, int timeout);

    void start();
    void stopScan();
    void wait();
    bool isRunning();

    // Callbacks (substituem os sinais do Qt)
    std::function<void(const PortInfo&)> portScannedCallback;
    std::function<void(int)> progressUpdateCallback;
    std::function<void()> scanFinishedCallback;
    std::function<void(const std::string&)> resultReadyCallback;

private:
    void run();
    std::string getServiceName(int port, const std::string &protocol);

    std::thread thread;
    std::atomic<bool> stopRequested;
    
    std::string target;
    int startPort;
    int endPort;
    bool tcpSelected;
    bool udpSelected;
    int timeout;
};

#endif // SCANNER_THREAD_H