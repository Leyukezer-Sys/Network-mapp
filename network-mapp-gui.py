#!/usr/bin/env python3
"""
Scanner de Portas - Desktop App Otimizada com Tkinter/TTK (Dark Theme)
"""

import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import threading
import time
from datetime import datetime
from network_mapp import PortScanner, PortInfo, PortStatus
import queue
import os
from tkinter import font as tkfont
import sv_ttk

class DesktopScannerTkinter:
    def __init__(self):
        self.scanner = PortScanner()
        self.results = []
        self.scanning = False
        self.current_scan_thread = None
        self.progress_queue = queue.Queue()
        self.filtered_results = []
        self.last_scan_time = 0
        self.auto_refresh = False
        
        # Configuração da janela principal
        self.root = tk.Tk()
        self.root.title("🚀 Network Scanner")
        self.root.geometry("1000x800")
        
        # Aplicar tema dark
        sv_ttk.set_theme("dark")
        
        # Configurar estilo para tema dark
        self.style = ttk.Style()
        self.configure_styles()
        
        # Criar frames para as diferentes telas
        self.config_frame = ttk.Frame(self.root)
        self.progress_frame = ttk.Frame(self.root)
        self.results_frame = ttk.Frame(self.root)
        
        # Inicializar as telas
        self.create_config_screen()
        self.create_progress_screen()
        self.create_results_screen()
        
        # Mostrar a tela inicial
        self.show_screen("config")
        
    def configure_styles(self):
        """Configura estilos personalizados para o tema dark"""
        # Configurar cores do tema dark
        self.dark_bg = "#424242"
        self.dark_fg = "#ffffff"
        self.dark_entry = "#3c3f41"
        self.dark_button = "#290b3d"
        self.dark_hover = "#5e1861"
        self.dark_selected = "#4e6b9f"
        
        # Configurar estilo
        self.style.configure("TFrame", background=self.dark_bg)
        self.style.configure("TLabel", background=self.dark_bg, foreground=self.dark_fg)
        self.style.configure("TButton", background=self.dark_button, foreground=self.dark_fg)
        self.style.map("TButton", background=[("active", self.dark_hover)])
        self.style.configure("TEntry", fieldbackground=self.dark_entry, foreground=self.dark_fg)
        self.style.configure("TCombobox", fieldbackground=self.dark_entry, foreground=self.dark_fg)
        self.style.configure("TCheckbutton", background=self.dark_bg, foreground=self.dark_fg)
        self.style.configure("Horizontal.TProgressbar", background="#8737ac")
        self.style.configure("TScrollbar", background=self.dark_button)
        
        # Configurar a janela principal
        self.root.configure(bg=self.dark_bg)
        
    def create_config_screen(self):
        """Cria a tela de configuração"""
        # Título
        title_font = tkfont.Font(family="Helvetica", size=16, weight="bold")
        title_label = ttk.Label(self.config_frame, text="🌐 NETWORK PORT SCANNER", 
                               font=title_font)
        title_label.pack(pady=(20, 10))
        
        # Frame de configurações
        config_form_frame = ttk.Frame(self.config_frame)
        config_form_frame.pack(fill="both", expand=True, padx=20, pady=10)
        
        subtitle_font = tkfont.Font(family="Helvetica", size=14, weight="bold")
        ttk.Label(config_form_frame, text="⚙️ CONFIGURAÇÕES", 
                 font=subtitle_font).pack(pady=10)
        
        # Alvo
        target_frame = ttk.Frame(config_form_frame)
        target_frame.pack(fill="x", padx=20, pady=5)
        
        bold_font = tkfont.Font(family="Helvetica", weight="bold")
        ttk.Label(target_frame, text="Alvo/Host:", font=bold_font).pack(anchor="w")
        self.target_entry = ttk.Entry(target_frame)
        self.target_entry.pack(fill="x", pady=5)
        self.target_entry.insert(0, "127.0.0.1")
        
        # Portas
        ports_frame = ttk.Frame(config_form_frame)
        ports_frame.pack(fill="x", padx=20, pady=5)
        
        ttk.Label(ports_frame, text="Portas:", font=bold_font).pack(anchor="w")
        self.ports_entry = ttk.Entry(ports_frame)
        self.ports_entry.pack(fill="x", pady=5)
        self.ports_entry.insert(0, "1-1000")
        
        # Protocolos
        protocol_frame = ttk.Frame(config_form_frame)
        protocol_frame.pack(fill="x", padx=20, pady=5)
        
        ttk.Label(protocol_frame, text="Protocolos:", font=bold_font).pack(anchor="w")
        protocol_options = ttk.Frame(protocol_frame)
        protocol_options.pack(fill="x", pady=5)
        
        self.tcp_var = tk.BooleanVar(value=True)
        self.udp_var = tk.BooleanVar(value=False)
        ttk.Checkbutton(protocol_options, text="TCP", variable=self.tcp_var).pack(side="left", padx=10)
        ttk.Checkbutton(protocol_options, text="UDP", variable=self.udp_var).pack(side="left", padx=10)
        
        # Timeout
        timeout_frame = ttk.Frame(config_form_frame)
        timeout_frame.pack(fill="x", padx=20, pady=5)
        
        ttk.Label(timeout_frame, text="Timeout (segundos):", font=bold_font).pack(anchor="w")
        
        timeout_slider_frame = ttk.Frame(timeout_frame)
        timeout_slider_frame.pack(fill="x", pady=5)
        
        self.timeout_var = tk.DoubleVar(value=1.0)
        timeout_slider = ttk.Scale(timeout_slider_frame, from_=0.1, to=5.0, 
                                  variable=self.timeout_var, orient="horizontal", length=200)
        timeout_slider.pack(side="left", padx=(0, 10))
        self.timeout_label = ttk.Label(timeout_slider_frame, text="1.0")
        self.timeout_label.pack(side="left")
        
        # Threads
        threads_frame = ttk.Frame(config_form_frame)
        threads_frame.pack(fill="x", padx=20, pady=5)
        
        ttk.Label(threads_frame, text="Threads:", font=bold_font).pack(anchor="w")
        
        threads_slider_frame = ttk.Frame(threads_frame)
        threads_slider_frame.pack(fill="x", pady=5)
        
        self.threads_var = tk.IntVar(value=100)
        threads_slider = ttk.Scale(threads_slider_frame, from_=1, to=500, 
                                  variable=self.threads_var, orient="horizontal", length=200)
        threads_slider.pack(side="left", padx=(0, 10))
        self.threads_label = ttk.Label(threads_slider_frame, text="100")
        self.threads_label.pack(side="left")
        
        # Atualizar labels dos sliders
        def update_timeout_label(*args):
            self.timeout_label.configure(text=f"{self.timeout_var.get():.1f}")
            
        def update_threads_label(*args):
            self.threads_label.configure(text=str(self.threads_var.get()))
            
        self.timeout_var.trace('w', update_timeout_label)
        self.threads_var.trace('w', update_threads_label)
        
        # Botão de iniciar
        button_frame = ttk.Frame(config_form_frame)
        button_frame.pack(pady=20)
        
        self.start_button = ttk.Button(button_frame, text="🚀 Iniciar Varredura", 
                                      command=self.start_scan, width=20)
        self.start_button.pack()
        
    def create_progress_screen(self):
        """Cria a tela de progresso"""
        # Título
        title_font = tkfont.Font(family="Helvetica", size=16, weight="bold")
        title_label = ttk.Label(self.progress_frame, text="⏳ Varredura em Andamento", 
                               font=title_font)
        title_label.pack(pady=(40, 20))
        
        # Informações do scan
        self.scan_info_label = ttk.Label(self.progress_frame, text="")
        self.scan_info_label.pack(pady=10)
        
        # Progresso
        progress_container = ttk.Frame(self.progress_frame)
        progress_container.pack(pady=30)
        
        self.progress_bar = ttk.Progressbar(progress_container, length=400, mode='determinate')
        self.progress_bar.pack(pady=10)
        self.progress_bar['value'] = 0
        
        self.progress_label = ttk.Label(progress_container, text="0%", 
                                       font=tkfont.Font(weight="bold"))
        self.progress_label.pack()
        
        # Estatísticas
        stats_frame = ttk.Frame(self.progress_frame)
        stats_frame.pack(pady=20)
        
        self.stats_label = ttk.Label(stats_frame, text="Portas escaneadas: 0\nPortas abertas: 0")
        self.stats_label.pack()
        
        # Botão de cancelar
        self.cancel_button = ttk.Button(self.progress_frame, text="⏹️ Cancelar Varredura", 
                                       command=self.stop_scan, width=20)
        self.cancel_button.pack(pady=20)
        
    def create_results_screen(self):
        """Cria a tela de resultados"""
        # Cabeçalho
        header_frame = ttk.Frame(self.results_frame)
        header_frame.pack(fill="x", padx=20, pady=10)
        
        title_font = tkfont.Font(family="Helvetica", size=16, weight="bold")
        title_label = ttk.Label(header_frame, text="📊 Resultados da Varredura", 
                               font=title_font)
        title_label.pack(side="left")
        
        # Botões de ação (agora mais visíveis)
        action_buttons = ttk.Frame(header_frame)
        action_buttons.pack(side="right")
        
        self.export_button = ttk.Button(action_buttons, text="💾 Exportar CSV", 
                                       command=self.export_results, width=15)
        self.export_button.pack(side="left", padx=5)
        
        self.new_scan_button = ttk.Button(action_buttons, text="🔄 Nova Varredura", 
                                         command=lambda: self.show_screen("config"), width=15)
        self.new_scan_button.pack(side="left", padx=5)
        
        # Botão de monitoramento contínuo
        self.monitor_button = ttk.Button(action_buttons, text="🔍 Monitorar", 
                                        command=self.toggle_monitoring, width=12)
        self.monitor_button.pack(side="left", padx=5)
        
        # Filtros
        filter_frame = ttk.Frame(self.results_frame)
        filter_frame.pack(fill="x", padx=20, pady=10)
        
        bold_font = tkfont.Font(family="Helvetica", weight="bold")
        ttk.Label(filter_frame, text="Filtros:", font=bold_font).pack(anchor="w")
        
        filter_options = ttk.Frame(filter_frame)
        filter_options.pack(fill="x", pady=5)
        
        # Filtro por status
        ttk.Label(filter_options, text="Status:").pack(side="left", padx=(0, 5))
        self.status_filter = ttk.Combobox(filter_options, 
                                        values=["Todos", "Abertas", "Fechadas", "Filtradas", "Erro"],
                                        state="readonly", width=12)
        self.status_filter.pack(side="left", padx=5)
        self.status_filter.set("Abertas")
        self.status_filter.bind("<<ComboboxSelected>>", lambda e: self.apply_filters())
        
        # Filtro por protocolo
        ttk.Label(filter_options, text="Protocolo:").pack(side="left", padx=(20, 5))
        self.protocol_filter = ttk.Combobox(filter_options, values=["Todos", "TCP", "UDP"],
                                          state="readonly", width=8)
        self.protocol_filter.pack(side="left", padx=5)
        self.protocol_filter.set("Todos")
        self.protocol_filter.bind("<<ComboboxSelected>>", lambda e: self.apply_filters())
        
        # Botão limpar filtro
        self.clear_filter_button = ttk.Button(filter_options, text="Limpar Filtro", 
                                             command=self.clear_filters, width=12)
        self.clear_filter_button.pack(side="left", padx=10)
        
        # Tabela de resultados
        table_container = ttk.Frame(self.results_frame)
        table_container.pack(fill="both", expand=True, padx=20, pady=10)
        
        # Criar Treeview para exibir resultados
        columns = ("port", "protocol", "status", "service")
        self.tree = ttk.Treeview(table_container, columns=columns, show="headings", height=15)
        
        # Definir cabeçalhos
        self.tree.heading("port", text="Porta", command=lambda: self.sort_treeview("port", False))
        self.tree.heading("protocol", text="Protocolo", command=lambda: self.sort_treeview("protocol", False))
        self.tree.heading("status", text="Status", command=lambda: self.sort_treeview("status", False))
        self.tree.heading("service", text="Serviço", command=lambda: self.sort_treeview("service", False))
        
        # Definir largura das colunas
        self.tree.column("port", width=80, anchor="center")
        self.tree.column("protocol", width=80, anchor="center")
        self.tree.column("status", width=120, anchor="center")
        self.tree.column("service", width=200, anchor="w")
        
        # Adicionar scrollbars
        v_scrollbar = ttk.Scrollbar(table_container, orient="vertical", command=self.tree.yview)
        h_scrollbar = ttk.Scrollbar(table_container, orient="horizontal", command=self.tree.xview)
        self.tree.configure(yscrollcommand=v_scrollbar.set, xscrollcommand=h_scrollbar.set)
        
        # Layout da tabela
        self.tree.grid(row=0, column=0, sticky="nsew")
        v_scrollbar.grid(row=0, column=1, sticky="ns")
        h_scrollbar.grid(row=1, column=0, sticky="ew")
        
        table_container.grid_rowconfigure(0, weight=1)
        table_container.grid_columnconfigure(0, weight=1)
        
        # Configurar tags para cores
        self.tree.tag_configure("open", background="#2e7d32", foreground="white")
        self.tree.tag_configure("filtered", background="#ff9800", foreground="black")
        self.tree.tag_configure("closed", background="#f44336", foreground="white")
        self.tree.tag_configure("unknown", background="#9e9e9e", foreground="white")
        
        # Área de resumo
        summary_frame = ttk.Frame(self.results_frame)
        summary_frame.pack(fill="x", padx=20, pady=10)
        
        self.summary_label = ttk.Label(summary_frame, text="", font=bold_font)
        self.summary_label.pack(anchor="w")
        
        # Status do monitoramento
        self.monitor_status = ttk.Label(summary_frame, text="Monitoramento: Desativado", foreground="red")
        self.monitor_status.pack(anchor="w")
        
    def show_screen(self, screen_name):
        """Mostra a tela especificada esconde as outras"""
        self.config_frame.pack_forget()
        self.progress_frame.pack_forget()
        self.results_frame.pack_forget()
        
        if screen_name == "config":
            self.config_frame.pack(fill="both", expand=True)
        elif screen_name == "progress":
            self.progress_frame.pack(fill="both", expand=True)
        elif screen_name == "results":
            self.results_frame.pack(fill="both", expand=True)
            
            # Se estiver em modo de monitoramento, inicia a verificação
            if self.auto_refresh:
                self.start_monitoring()
    
    def start_scan(self):
        """Inicia a varredura de portas"""
        target = self.target_entry.get().strip()
        ports = self.ports_entry.get().strip()
        tcp = self.tcp_var.get()
        udp = self.udp_var.get()
        timeout = self.timeout_var.get()
        max_threads = self.threads_var.get()
        
        # Validações básicas
        if not target:
            messagebox.showerror("Erro", "Por favor, informe um alvo/host.")
            return
            
        if not ports:
            messagebox.showerror("Erro", "Por favor, informe as portas a serem escaneadas.")
            return
            
        if not tcp and not udp:
            messagebox.showerror("Erro", "Selecione pelo menos um protocolo (TCP ou UDP).")
            return
        
        # Prepara a tela de progresso
        self.show_screen("progress")
        self.scan_info_label.configure(
            text=f"Alvo: {target}\nPortas: {ports}\nProtocolos: {'TCP' if tcp else ''}{' UDP' if udp else ''}"
        )
        self.progress_bar['value'] = 0
        self.progress_label.configure(text="0%")
        self.stats_label.configure(text="Portas escaneadas: 0\nPortas abertas: 0")
        
        # Inicia a varredura em thread separada
        scan_thread = threading.Thread(target=self.run_scan, 
                                     args=(target, ports, tcp, udp, timeout, max_threads),
                                     daemon=True)
        scan_thread.start()
        
        # Inicia a verificação de progresso
        self.check_progress()
    
    def run_scan(self, target, ports, tcp, udp, timeout, max_threads):
        """Executa a varredura em uma thread separada"""
        try:
            self.scanning = True
            
            # Processa range de portas
            if '-' in ports:
                start_port, end_port = map(int, ports.split('-'))
                total_ports = end_port - start_port + 1
            elif ',' in ports:
                port_list = list(map(int, ports.split(',')))
                start_port, end_port = min(port_list), max(port_list)
                total_ports = len(port_list)
            else:
                start_port = end_port = int(ports)
                total_ports = 1
            
            # Configura callback para progresso
            def progress_callback(completed, total, open_ports):
                progress = int((completed / total) * 100) if total > 0 else 0
                self.progress_queue.put(("progress", progress))
                self.progress_queue.put(("stats", {"scanned": completed, "open": open_ports}))
            
            # Executa o scan com callback de progresso
            self.scanner.set_progress_callback(progress_callback)
            self.results = self.scanner.scan_ports(
                target=target,
                start_port=start_port,
                end_port=end_port,
                tcp=tcp,
                udp=udp,
                timeout=timeout,
                max_threads=max_threads
            )
            
            # Sinaliza conclusão
            self.progress_queue.put(("complete", None))
            
        except Exception as e:
            self.progress_queue.put(("error", str(e)))
    
    def check_progress(self):
        """Verifica o progresso da varredura periodicamente"""
        try:
            # Processa mensagens da fila
            while True:
                try:
                    msg_type, data = self.progress_queue.get_nowait()
                    
                    if msg_type == "progress":
                        self.update_progress(data)
                    elif msg_type == "stats":
                        self.update_stats(data)
                    elif msg_type == "complete":
                        self.scan_complete()
                        return
                    elif msg_type == "error":
                        self.scan_error(data)
                        return
                        
                except queue.Empty:
                    break
            
            # Continua verificando se ainda está escaneando
            if self.scanning:
                self.root.after(100, self.check_progress)
                
        except Exception as e:
            print(f"Erro ao verificar progresso: {e}")
    
    def update_progress(self, progress):
        """Atualiza a barra de progresso"""
        self.progress_bar['value'] = progress
        self.progress_label.configure(text=f"{progress}%")
    
    def update_stats(self, stats):
        """Atualiza as estatísticas"""
        self.stats_label.configure(
            text=f"Portas escaneadas: {stats['scanned']}\nPortas abertas: {stats['open']}"
        )
    
    def stop_scan(self):
        """Para a varredura em andamento"""
        self.scanner.stop_scan()
        self.scanning = False
        self.show_screen("config")
        messagebox.showinfo("Info", "Varredura cancelada pelo usuário.")
    
    def scan_complete(self):
        """Processa a conclusão da varredura"""
        self.scanning = False
        self.last_scan_time = time.time()
        self.show_screen("results")
        self.display_results()
        
        # Mostra resumo
        open_ports = [r for r in self.results if r.status in [PortStatus.OPEN, PortStatus.OPEN_UDP]]
        messagebox.showinfo('✅ Varredura Concluída', 
                           f'Portas abertas: {len(open_ports)}\nTotal escaneado: {len(self.results)}')
    
    def scan_error(self, error_msg):
        """Processa erros na varredura"""
        self.scanning = False
        self.show_screen("config")
        messagebox.showerror('Erro na varredura', error_msg)
    
    def display_results(self):
        """Exibe os resultados na tabela"""
        # Limpa resultados anteriores
        for item in self.tree.get_children():
            self.tree.delete(item)
        
        # Adiciona resultados à tabela
        for result in self.filtered_results:
            # Determina a tag baseada no status
            tag = ""
            if result.status == PortStatus.OPEN or result.status == PortStatus.OPEN_UDP:
                tag = "open"
            elif result.status == PortStatus.FILTERED:
                tag = "filtered"
            elif result.status == PortStatus.CLOSED:
                tag = "closed"
            else:
                tag = "unknown"
            
            # Insere na treeview
            self.tree.insert("", "end", values=(
                result.port, 
                result.protocol, 
                result.status.value, 
                result.service
            ), tags=(tag,))
        
        # Atualiza resumo
        open_ports = len([r for r in self.filtered_results if r.status in [PortStatus.OPEN, PortStatus.OPEN_UDP]])
        filtered_ports = len([r for r in self.filtered_results if r.status == PortStatus.FILTERED])
        self.summary_label.configure(
            text=f"Total: {len(self.filtered_results)} | Abertas: {open_ports} | Filtradas: {filtered_ports}"
        )
    
    def apply_filters(self):
        """Aplica os filtros selecionados aos resultados"""
        status_filter = self.status_filter.get()
        protocol_filter = self.protocol_filter.get()
        
        # Filtra os resultados
        self.filtered_results = []
        
        for result in self.results:
            # Filtro por status
            status_match = True
            if status_filter != "Todos":
                if status_filter == "Abertas" and result.status not in [PortStatus.OPEN, PortStatus.OPEN_UDP]:
                    status_match = False
                elif status_filter == "Fechadas" and result.status != PortStatus.CLOSED:
                    status_match = False
                elif status_filter == "Filtradas" and result.status != PortStatus.FILTERED:
                    status_match = False
                elif status_filter == "Erro" and result.status != PortStatus.UNKNOWN:
                    status_match = False
            
            # Filtro por protocolo
            protocol_match = True
            if protocol_filter != "Todos" and result.protocol != protocol_filter:
                protocol_match = False
            
            if status_match and protocol_match:
                self.filtered_results.append(result)
        
        # Atualiza a exibição
        self.display_results()
    
    def clear_filters(self):
        """Limpa todos os filtros"""
        self.status_filter.set("Todos")
        self.protocol_filter.set("Todos")
        self.filtered_results = self.results.copy()
        self.display_results()
    
    def sort_treeview(self, column, reverse):
        """Ordena a treeview pela coluna clicada"""
        # Obtém todos os itens da treeview
        items = [(self.tree.set(item, column), item) for item in self.tree.get_children("")]
        
        # Tenta converter para números se possível (para a coluna de porta)
        try:
            if column == "port":
                items = [(int(item[0]), item[1]) for item in items]
        except ValueError:
            pass  # Mantém como string se não for número
        
        # Ordena os itens
        items.sort(reverse=reverse)
        
        # Reorganiza os itens na treeview
        for index, (value, item) in enumerate(items):
            self.tree.move(item, "", index)
        
        # Alterna a direção da ordenação para o próximo clique
        self.tree.heading(column, command=lambda: self.sort_treeview(column, not reverse))
    
    def toggle_monitoring(self):
        """Ativa/desativa o monitoramento contínuo"""
        self.auto_refresh = not self.auto_refresh
        
        if self.auto_refresh:
            self.monitor_button.configure(text="⏹️ Parar Monitoramento")
            self.monitor_status.configure(text="Monitoramento: Ativado (30 segundos)", foreground="green")
            self.start_monitoring()
        else:
            self.monitor_button.configure(text="🔍 Monitorar")
            self.monitor_status.configure(text="Monitoramento: Desativado", foreground="red")
    
    def start_monitoring(self):
        """Inicia o monitoramento contínuo"""
        if not self.auto_refresh:
            return
            
        # Verifica se já passaram 30 segundos desde o último scan
        current_time = time.time()
        if current_time - self.last_scan_time >= 30:
            self.last_scan_time = current_time
            self.start_scan()
        else:
            # Atualiza o status com o tempo restante
            remaining = int(30 - (current_time - self.last_scan_time))
            self.monitor_status.configure(text=f"Monitoramento: Próximo scan em {remaining}s", foreground="orange")
            
        # Agenda a próxima verificação
        if self.auto_refresh:
            self.root.after(1000, self.start_monitoring)
    
    def export_results(self):
        """Exporta os resultados para CSV - Versão melhorada"""
        if not self.results:
            messagebox.showwarning("Aviso", "Não há resultados para exportar.")
            return
        
        # Cria um nome de arquivo padrão baseado no alvo e data/hora
        target = self.target_entry.get().replace('.', '_').replace(':', '_')
        default_filename = f"portscan_{target}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
        
        # Abre a caixa de diálogo para salvar arquivo
        filename = filedialog.asksaveasfilename(
            title="Salvar resultados como CSV",
            defaultextension=".csv",
            filetypes=[
                ("Arquivos CSV", "*.csv"),
                ("Todos os arquivos", "*.*")
            ],
            initialfile=default_filename
        )
        
        if filename:
            try:
                # Verifica se o diretório existe, se não, cria
                directory = os.path.dirname(filename)
                if directory and not os.path.exists(directory):
                    os.makedirs(directory)
                
                # Exporta os resultados
                self.scanner.export_results(self.results, filename)
                messagebox.showinfo("Sucesso", f"Resultados exportados para:\n{filename}")
            except Exception as e:
                messagebox.showerror("Erro", f"Falha ao exportar: {str(e)}")
    
    def run(self):
        """Executa a aplicação"""
        self.root.mainloop()

if __name__ == "__main__":
    app = DesktopScannerTkinter()
    app.run()