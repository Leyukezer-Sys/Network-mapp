#!/usr/bin/env python3
"""
Scanner de Portas TCP/UDP - Versão Cross-Platform
Funciona em Linux e Windows sem dependências externas
"""

import socket
import threading
import time
import sys
import argparse
from enum import Enum
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass
import ipaddress
import csv
from datetime import datetime
import os
from pathlib import Path

class PortStatus(Enum):
    OPEN = "ABERTA"
    CLOSED = "FECHADA"
    FILTERED = "FILTRADA"
    OPEN_UDP = "ABERTA (UDP)"
    UNKNOWN = "DESCONHECIDO"

@dataclass
class PortInfo:
    port: int
    protocol: str
    status: PortStatus
    service: str

class PortScanner:
    def __init__(self):
        self.stop_requested = False
        self.common_services = self._load_common_services()
        self.progress_callback = None
        
    def set_progress_callback(self, callback):
        """Define uma função de callback para progresso"""
        self.progress_callback = callback
        
    def _load_common_services(self) -> Dict[int, str]:
        """Carrega os serviços comuns das portas"""
        return {
            20: "FTP Data",
            21: "FTP Control",
            22: "SSH",
            23: "Telnet",
            25: "SMTP",
            53: "DNS",
            80: "HTTP",
            110: "POP3",
            143: "IMAP",
            443: "HTTPS",
            465: "SMTPS",
            587: "SMTP Submission",
            993: "IMAPS",
            995: "POP3S",
            3389: "RDP"
        }
    
    def get_service_name(self, port: int, protocol: str) -> str:
        """Retorna o nome do serviço para uma porta"""
        return self.common_services.get(port, "Unknown")
    
    def resolve_host(self, target: str) -> Optional[str]:
        """Resolve um hostname para IP"""
        try:
            if self.is_valid_ip(target):
                return target
            return socket.gethostbyname(target)
        except (socket.gaierror, socket.herror):
            return None
    
    def is_valid_ip(self, ip: str) -> bool:
        """Verifica se é um IP válido"""
        try:
            ipaddress.ip_address(ip)
            return True
        except ValueError:
            return False
    
    def scan_tcp_port(self, target: str, port: int, timeout: float = 1.0) -> PortStatus:
        """Escaneia uma porta TCP"""
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.settimeout(timeout)
                result = sock.connect_ex((target, port))
                
                if result == 0:
                    return PortStatus.OPEN
                elif result == 10061:  # Connection refused (Windows)
                    return PortStatus.CLOSED
                elif result == 111:    # Connection refused (Linux)
                    return PortStatus.CLOSED
                else:
                    return PortStatus.FILTERED
        except socket.timeout:
            return PortStatus.FILTERED
        except Exception:
            return PortStatus.UNKNOWN
    
    def scan_udp_port(self, target: str, port: int, timeout: float = 1.0) -> PortStatus:
        """Escaneia uma porta UDP (método básico)"""
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
                sock.settimeout(timeout)
                
                # Envia dados para a porta UDP
                sock.sendto(b"SCAN", (target, port))
                
                try:
                    # Tenta receber resposta
                    data, addr = sock.recvfrom(1024)
                    if data:
                        return PortStatus.OPEN_UDP
                except socket.timeout:
                    # Timeout pode indicar porta filtrada ou aberta sem resposta
                    return PortStatus.FILTERED
                
        except Exception:
            return PortStatus.UNKNOWN
        
        return PortStatus.FILTERED
    
    def scan_ports(self, target: str, start_port: int, end_port: int, 
                  tcp: bool = True, udp: bool = False, 
                  timeout: float = 1.0, max_threads: int = 100) -> List[PortInfo]:
        """Escaneia um range de portas"""
        self.stop_requested = False
        results = []
        lock = threading.Lock()
        
        # Resolve o host
        resolved_ip = self.resolve_host(target)
        if not resolved_ip:
            print(f"Erro: Não foi possível resolver {target}")
            return results
        
        # Cria fila de trabalho
        port_queue = []
        if tcp:
            port_queue.extend([(port, "TCP") for port in range(start_port, end_port + 1)])
        if udp:
            port_queue.extend([(port, "UDP") for port in range(start_port, end_port + 1)])
        
        total_ports = len(port_queue)
        scanned_ports = 0
        open_ports = 0
        
        def worker():
            nonlocal scanned_ports, open_ports
            while not self.stop_requested:
                with lock:
                    if not port_queue:
                        break
                    port, protocol = port_queue.pop(0)
                
                try:
                    if protocol == "TCP":
                        status = self.scan_tcp_port(resolved_ip, port, timeout)
                    else:
                        status = self.scan_udp_port(resolved_ip, port, timeout)
                    
                    service = self.get_service_name(port, protocol)
                    port_info = PortInfo(port, protocol, status, service)
                    
                    with lock:
                        results.append(port_info)
                        scanned_ports += 1
                        if status in [PortStatus.OPEN, PortStatus.OPEN_UDP]:
                            open_ports += 1
                            
                        # Chama callback de progresso se definido
                        if self.progress_callback:
                            self.progress_callback(scanned_ports, total_ports, open_ports)
                            
                except Exception as e:
                    print(f"Erro ao escanear porta {port}/{protocol}: {e}")
                    with lock:
                        scanned_ports += 1
        
        # Cria threads
        threads = []
        for _ in range(min(max_threads, len(port_queue))):
            thread = threading.Thread(target=worker)
            thread.daemon = True
            thread.start()
            threads.append(thread)
        
        # Aguarda todas as threads
        for thread in threads:
            thread.join()
        
        return results
    
    def display_result(self, port_info: PortInfo, progress: int, 
                      scanned: int, total: int):
        """Exibe o resultado de uma porta escaneada"""
        status_color = {
            PortStatus.OPEN: "\033[92m",      # Verde
            PortStatus.OPEN_UDP: "\033[92m",  # Verde
            PortStatus.CLOSED: "\033[91m",    # Vermelho
            PortStatus.FILTERED: "\033[93m",  # Amarelo
            PortStatus.UNKNOWN: "\033[90m"    # Cinza
        }
        
        color = status_color.get(port_info.status, "\033[0m")
        reset = "\033[0m"
        
        if port_info.status in [PortStatus.OPEN, PortStatus.OPEN_UDP]:
            print(f"{color}✅ {port_info.port:5d}/{port_info.protocol:3s} - "
                  f"{port_info.status.value:15s} - {port_info.service}{reset}")
        elif port_info.status == PortStatus.FILTERED:
            print(f"{color}🛡️  {port_info.port:5d}/{port_info.protocol:3s} - "
                  f"{port_info.status.value:15s}{reset}")
        
        # Mostra progresso a cada 10 portas ou quando terminar
        if scanned % 10 == 0 or scanned == total:
            print(f"📊 Progresso: {progress}% ({scanned}/{total} portas)")
    
    def stop_scan(self):
        """Para o escaneamento"""
        self.stop_requested = True
    
    def export_results(self, results: List[PortInfo], filename: str):
        """Exporta resultados para CSV"""
        try:
            # Cria o diretório se não existir
            directory = os.path.dirname(filename)
            if directory and not os.path.exists(directory):
                os.makedirs(directory)
            
            with open(filename, 'w', newline='', encoding='utf-8') as csvfile:
                writer = csv.writer(csvfile)
                writer.writerow(['Porta', 'Protocolo', 'Status', 'Serviço'])
                
                for result in results:
                    writer.writerow([
                        result.port,
                        result.protocol,
                        result.status.value,
                        result.service
                    ])
            
            print(f"💾 Resultados exportados para: {filename}")
            
        except Exception as e:
            print(f"Erro ao exportar resultados: {e}")
    
    def print_summary(self, results: List[PortInfo]):
        """Imprime resumo do scan"""
        open_ports = [r for r in results if r.status in [PortStatus.OPEN, PortStatus.OPEN_UDP]]
        filtered_ports = [r for r in results if r.status == PortStatus.FILTERED]
        closed_ports = [r for r in results if r.status == PortStatus.CLOSED]
        
        print("\n" + "="*50)
        print("📋 RESUMO DA VARREDURA")
        print("="*50)
        print(f"✅ Portas abertas: {len(open_ports)}")
        print(f"🛡️  Portas filtradas: {len(filtered_ports)}")
        print(f"❌ Portas fechadas: {len(closed_ports)}")
        print(f"📊 Total escaneado: {len(results)} portas")
        
        if open_ports:
            print("\n🔓 PORTAS ABERTAS:")
            for port in open_ports:
                print(f"   {port.port:5d}/{port.protocol:3s} - {port.service}")

def get_default_output_filename(target: str) -> str:
    """Gera um nome de arquivo padrão baseado no target e data/hora"""
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    safe_target = target.replace(".", "_").replace(":", "_")
    return f"portscan_{safe_target}_{timestamp}.csv"

def main():
    """Função principal"""
    parser = argparse.ArgumentParser(description='Scanner de Portas TCP/UDP')
    parser.add_argument('target', help='Alvo (IP ou hostname)')
    parser.add_argument('-p', '--ports', default='1-1000', 
                       help='Range de portas (ex: 1-1000, 80,443)')
    parser.add_argument('-t', '--tcp', action='store_true', default=True,
                       help='Escanear portas TCP (padrão)')
    parser.add_argument('-u', '--udp', action='store_true',
                       help='Escanear portas UDP')
    parser.add_argument('-T', '--timeout', type=float, default=1.0,
                       help='Timeout em segundos (padrão: 1.0)')
    parser.add_argument('--threads', type=int, default=100,
                       help='Número máximo de threads (padrão: 100)')
    parser.add_argument('-o', '--output',
                       help='Arquivo para exportar resultados (CSV). Pode incluir caminho completo.')
    parser.add_argument('--output-dir', default='.',
                       help='Diretório para salvar o arquivo de resultados (padrão: diretório atual)')
    parser.add_argument('--no-tcp', action='store_true',
                       help='Não escanear TCP')
    
    args = parser.parse_args()
    
    # Processa range de portas
    if '-' in args.ports:
        start_port, end_port = map(int, args.ports.split('-'))
    elif ',' in args.ports:
        # Para múltiplas portas específicas
        ports = list(map(int, args.ports.split(',')))
        start_port, end_port = min(ports), max(ports)
    else:
        start_port = end_port = int(args.ports)
    
    # Configura protocolos
    tcp_scan = args.tcp and not args.no_tcp
    udp_scan = args.udp
    
    if not tcp_scan and not udp_scan:
        print("Erro: Selecione pelo menos um protocolo (TCP ou UDP)")
        return
    
    # Determina o caminho do arquivo de saída
    if args.output:
        output_file = args.output
    else:
        # Gera nome padrão no diretório especificado
        default_name = get_default_output_filename(args.target)
        output_file = os.path.join(args.output_dir, default_name)
    
    # Verifica se o diretório de saída existe
    output_dir = os.path.dirname(output_file) or args.output_dir
    if not os.path.exists(output_dir):
        try:
            os.makedirs(output_dir)
            print(f"📁 Diretório criado: {output_dir}")
        except OSError as e:
            print(f"Erro: Não foi possível criar o diretório {output_dir}: {e}")
            print("Usando diretório atual...")
            output_file = get_default_output_filename(args.target)
    
    # Cria e executa scanner
    scanner = PortScanner()
    
    try:
        print("🚀 Iniciando scanner de portas...")
        print(f"📁 Resultados serão salvos em: {output_file}")
        print("⚠️  Pressione Ctrl+C para interromper")
        print("-" * 50)
        
        results = scanner.scan_ports(
            target=args.target,
            start_port=start_port,
            end_port=end_port,
            tcp=tcp_scan,
            udp=udp_scan,
            timeout=args.timeout,
            max_threads=args.threads
        )
        
        # Mostra resumo
        scanner.print_summary(results)
        
        # Exporta resultados
        scanner.export_results(results, output_file)
            
    except KeyboardInterrupt:
        print("\n\n⏹️  Varredura interrompida pelo usuário")
        scanner.stop_scan()
        
        # Exporta resultados parciais se interrompido
        if results:
            partial_file = output_file.replace(".csv", "_partial.csv")
            print(f"💾 Salvando resultados parciais em: {partial_file}")
            scanner.export_results(results, partial_file)
            
    except Exception as e:
        print(f"Erro durante a varredura: {e}")
    
    print("\n🎯 Varredura concluída!")

if __name__ == "__main__":
    main()