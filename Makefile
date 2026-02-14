# Makefile para automação do Projeto SCADA
# Autor: Você

#Compilar apenas (pra ver se tem erro)
build:
	pio run

#Enviar para a placa (Upload)
upload:
	pio run --target upload

#Limpar a sujeira (Clean)
clean:
	pio run --target clean

#Monitor Serial
monitor:
	pio device monitor

#Compila, Envia e já abre o Monitor
run:
	pio run --target upload && pio device monitor

#Apagar a memória da placa (Erase Flash)
erase:
	pio run --target erase

#RESET TOTAL: Limpa PC, Zera Placa, Grava e Monitora
tudo:
	pio run --target clean && pio run --target erase && pio run --target upload && pio device monitor

# 7. Menu de ajuda (se você digitar apenas 'make')
help:
	@echo "------------------------------------------------"
	@echo "COMANDOS DISPONIVEIS:"
	@echo "  make build    -> Compila o codigo"
	@echo "  make upload   -> Grava na placa"
	@echo "  make monitor  -> Abre o Serial Monitor"
	@echo "  make run      -> Grava e abre o Monitor (Recomendado)"
	@echo "  make erase    -> Apaga toda a memoria da placa"
	@echo "  make clean    -> Limpa arquivos temporarios"
	@echo "  make tudo     -> Reset total: Limpa PC, Zera Placa, Grava e Monitora"
	@echo "------------------------------------------------"