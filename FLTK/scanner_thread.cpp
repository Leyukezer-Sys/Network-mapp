#include "scanner_thread.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <netdb.h>
#include <chrono>
#include <thread>

ScannerThread::ScannerThread() : stopRequested(false) {
    std::cout << "ScannerThread criado" << std::endl;
}

ScannerThread::~ScannerThread() {
    std::cout << "ScannerThread destruído" << std::endl;
    stopScan();
    wait();
}

void ScannerThread::setParameters(const std::string &target, int startPort, int endPort,
                                 bool tcpSelected, bool udpSelected, int timeout) {
    this->target = target;
    this->startPort = startPort;
    this->endPort = endPort;
    this->tcpSelected = tcpSelected;
    this->udpSelected = udpSelected;
    this->timeout = timeout;
}

void ScannerThread::start() {
    stopRequested = false;
    thread = std::thread(&ScannerThread::run, this);
}

void ScannerThread::stopScan() {
    stopRequested = true;
    std::cout << "Parando varredura..." << std::endl;
}

void ScannerThread::wait() {
    if (thread.joinable()) {
        thread.join();
    }
}

bool ScannerThread::isRunning() {
    return thread.joinable() && !stopRequested;
}

std::string ScannerThread::getServiceName(int port, const std::string &protocol) {
    // Mapa de serviços comuns
    static std::map<int, std::string> commonServices;
    if (commonServices.empty()) {
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

    auto it = commonServices.find(port);
    if (it != commonServices.end()) {
        return it->second;
    }
    return "Unknown";
}

void ScannerThread::run() {
    std::cout << "Iniciando execução da thread..." << std::endl;
    stopRequested = false;
    int totalPorts = endPort - startPort + 1;

    if (totalPorts <= 0) {
        if (resultReadyCallback) {
            resultReadyCallback("Erro: Intervalo de portas inválido.");
        }
        if (scanFinishedCallback) {
            scanFinishedCallback();
        }
        return;
    }

    int totalScans = (tcpSelected ? totalPorts : 0) + (udpSelected ? totalPorts : 0);
    if (totalScans == 0) {
        if (resultReadyCallback) {
            resultReadyCallback("Erro: Nenhum protocolo selecionado.");
        }
        if (scanFinishedCallback) {
            scanFinishedCallback();
        }
        return;
    }

    int scannedPorts = 0;

    // Resolve o endereço de destino
    struct hostent *host = gethostbyname(target.c_str());
    if (host == nullptr) {
        if (resultReadyCallback) {
            resultReadyCallback("Erro: Não foi possível resolver o endereço " + target);
        }
        if (scanFinishedCallback) {
            scanFinishedCallback();
        }
        return;
    }

    std::string ipString = inet_ntoa(*(struct in_addr*)host->h_addr);
    if (resultReadyCallback) {
        resultReadyCallback("Iniciando varredura em: " + ipString + "\n");
    }

    // Varredura TCP
    if (tcpSelected) {
        if (resultReadyCallback) {
            resultReadyCallback("\n[+] Varredura TCP iniciada...\n");
        }

        int cont = 0;

        for (int port = startPort; port <= endPort && !stopRequested; port++) {
            PortInfo portInfo;
            portInfo.portNumber = port;
            portInfo.protocol = "TCP";
            portInfo.service = getServiceName(port, "TCP");

            // Cria socket TCP
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) {
                portInfo.status = STATUS_FILTERED;
            } else {
                // Configura timeout
                struct timeval tv;
                tv.tv_sec = timeout / 1000;
                tv.tv_usec = (timeout % 1000) * 1000;
                setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
                setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

                // Configura endereço
                struct sockaddr_in server_addr;
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(port);
                server_addr.sin_addr.s_addr = inet_addr(ipString.c_str());

                // Tenta conectar
                if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) {
                    portInfo.status = STATUS_OPEN;
                    cont++;
                } else {
                    if (errno == ECONNREFUSED) {
                        portInfo.status = STATUS_CLOSED;
                    } else {
                        portInfo.status = STATUS_FILTERED;
                    }
                }

                close(sock);
            }

            if (portScannedCallback) {
                portScannedCallback(portInfo);
            }

            scannedPorts++;
            int progress = (scannedPorts * 100) / totalScans;
            if (progressUpdateCallback) {
                progressUpdateCallback(progress);
            }

            // Pequena pausa para não sobrecarregar o sistema
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (cont <= 0 && resultReadyCallback) {
            resultReadyCallback("\n[**] Nenhuma Porta Aberta encontrada...\n");
        }
    }

    // Varredura UDP
    if (udpSelected && !stopRequested) {
        if (resultReadyCallback) {
            resultReadyCallback("\n[+] Varredura UDP iniciada...\n");
        }

        for (int port = startPort; port <= endPort && !stopRequested; port++) {
            PortInfo portInfo;
            portInfo.portNumber = port;
            portInfo.protocol = "UDP";
            portInfo.service = getServiceName(port, "UDP");

            // Cria socket UDP
            int sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock < 0) {
                portInfo.status = STATUS_FILTERED;
            } else {
                // Configura timeout
                struct timeval tv;
                tv.tv_sec = timeout / 1000;
                tv.tv_usec = (timeout % 1000) * 1000;
                setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

                // Configura endereço
                struct sockaddr_in server_addr;
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(port);
                server_addr.sin_addr.s_addr = inet_addr(ipString.c_str());

                // Envia dados (pacote vazio)
                sendto(sock, nullptr, 0, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

                // Verifica se há resposta (muito básico - detecção UDP é complexa)
                char buffer[1];
                fd_set readfds;
                FD_ZERO(&readfds);
                FD_SET(sock, &readfds);

                tv.tv_sec = timeout / 1000;
                tv.tv_usec = (timeout % 1000) * 1000;

                int activity = select(sock + 1, &readfds, nullptr, nullptr, &tv);
                if (activity > 0 && FD_ISSET(sock, &readfds)) {
                    portInfo.status = STATUS_OPEN_UDP;
                } else {
                    portInfo.status = STATUS_FILTERED;
                }

                close(sock);
            }

            if (portScannedCallback) {
                portScannedCallback(portInfo);
            }

            scannedPorts++;
            int progress = (scannedPorts * 100) / totalScans;
            if (progressUpdateCallback) {
                progressUpdateCallback(progress);
            }

            // Pequena pausa
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    if (!stopRequested && resultReadyCallback) {
        resultReadyCallback("\n[+] Varredura concluída!\n");
    } else if (resultReadyCallback) {
        resultReadyCallback("\n[+] Varredura interrompida!\n");
    }

    if (progressUpdateCallback) {
        progressUpdateCallback(100);
    }
    
    if (scanFinishedCallback) {
        scanFinishedCallback();
    }
    
    std::cout << "Thread finalizada" << std::endl;
}