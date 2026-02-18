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

#----------------------------------------------- COMANDOS PARA O ESP8266 ---------------------------------------------------------------------------------

# Grava e abre o Monitor
run_esp:
	pio run -e esp8266 --target upload && pio device monitor -e esp8266

# Só Grava
upload_esp:
	pio run -e esp8266 --target upload

# Só Monitora
monitor_esp:
	pio device monitor -e esp8266

# APAGA A MEMÓRIA FLASH (Zera o ESP8266)
erase_esp:
	pio run -e esp8266 --target erase

# ======================================================
# MENU DE AJUDA
# ======================================================
help:
	@echo "------------------------------------------------"
	@echo "COMANDOS DISPONIVEIS:"
	@echo "  make run_uno    -> Grava e Monitora o ARDUINO"
	@echo "  make erase_uno  -> Apaga a memoria do ARDUINO"
	@echo ""
	@echo "  make run_esp    -> Grava e Monitora o ESP8266"
	@echo "  make erase_esp  -> Apaga a memoria do ESP8266"
	@echo ""
	@echo "  make clean      -> Limpa arquivos temporarios do PC"
	@echo "------------------------------------------------"