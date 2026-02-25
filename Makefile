# Makefile Multi-Placas (Uno e ESP32)

# --- LIMPEZA DE ARQUIVOS (No Computador) ---
clean:
	pio run --target clean

#----------------------------------------------- COMANDOS PARA O ARDUINO UNO ----------------------------------------------------------------------------

# Grava e abre o Monitor
run_uno:
	pio run -e uno --target upload && pio device monitor -e uno

# Só Grava
upload_uno:
	pio run -e uno --target upload

# Só Monitora
monitor_uno:
	pio device monitor -e uno

# APAGA A MEMÓRIA FLASH (Zera o Arduino)
erase_uno:
	pio run -e uno --target erase

#----------------------------------------------- COMANDOS PARA O ESP32 ---------------------------------------------------------------------------------

# ======================================================
# COMANDOS ESP32 (Wemos D1 R32)
# ======================================================
# Grava e Monitora
run_esp:
	pio run -e wemos_d1_uno32 --target upload && pio device monitor -e wemos_d1_uno32

# Só Grava
upload_esp:
	pio run -e wemos_d1_uno32 --target upload

# Só Monitora
monitor_esp:
	pio device monitor -e wemos_d1_uno32

# APAGA A MEMÓRIA FLASH (Zera o ESP32 - Ótimo para limpar o Core Dump)
erase_esp:
	pio run -e wemos_d1_uno32 --target erase

# ======================================================
# MENU DE AJUDA
# ======================================================
help:
	@echo "------------------------------------------------"
	@echo "COMANDOS DISPONIVEIS:"
	@echo "  make run_uno    -> Grava e Monitora o ARDUINO UNO"
	@echo "  make erase_uno  -> Apaga a memoria do ARDUINO UNO"
	@echo ""
	@echo "  make run_esp    -> Grava e Monitora o ESP32 (Wemos D1 R32)"
	@echo "  make upload_esp -> Apenas Grava o ESP32"
	@echo "  make erase_esp  -> Apaga a memoria flash do ESP32"
	@echo ""
	@echo "  make clean      -> Limpa arquivos temporarios do PC"
	@echo "------------------------------------------------"