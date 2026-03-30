# Makefile for WIZZ MANIA Project

.PHONY: all clean build-server build-client build-tests help

# Variables
QT_PATH = C:\Qt\6.10.1\mingw_64
MINGW_BIN = C:\Qt\Tools\mingw1120_64\bin
CXX = $(MINGW_BIN)\g++
QMAKE = $(QT_PATH)\bin\qmake
CXXFLAGS = -std=c++17 -O2 -Wall
LIBS = -lws2_32 -lpthread

# Directories
SRC_DIR = src
BIN_DIR = bin
BUILD_TEMP = build_temp

help:
	@echo "╔════════════════════════════════════════╗"
	@echo "║   WIZZ MANIA - Build System            ║"
	@echo "╠════════════════════════════════════════╣"
	@echo "║  make build-server   : Compile serveur ║"
	@echo "║  make build-tests    : Compile tests   ║"
	@echo "║  make build-client   : Compile client  ║"
	@echo "║  make all            : Build tout      ║"
	@echo "║  make clean          : Clean fichiers  ║"
	@echo "║  make run-server     : Lancer serveur  ║"
	@echo "║  make run-tests      : Lancer tests    ║"
	@echo "╚════════════════════════════════════════╝"

# Build Server
build-server:
	@echo "[*] Compilation du serveur..."
	@$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/serveur_tcp.exe $(SRC_DIR)/server/server.cpp $(LIBS) -D_WINSOCK_DEPRECATED_NO_WARNINGS
	@echo "[✓] Serveur compilé: $(BIN_DIR)/serveur_tcp.exe"

# Build Tests
build-tests:
	@echo "[*] Compilation des tests..."
	@$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/tests_exec.exe $(SRC_DIR)/tests/integration_tests.cpp $(LIBS) -D_WINSOCK_DEPRECATED_NO_WARNINGS
	@echo "[✓] Tests compilés: $(BIN_DIR)/tests_exec.exe"

# Build Client
build-client:
	@echo "[*] Compilation du client Qt..."
	@cd $(SRC_DIR)/client/qt_client && \
	$(QMAKE) WizzClient.pro && \
	mingw32-make && \
	cd ../../../
	@echo "[✓] Client Qt compilé"

# Build All
all: build-server build-tests
	@echo ""
	@echo "╔════════════════════════════════════════╗"
	@echo "║  ✅ Compilation terminée avec succès! ║"
	@echo "╚════════════════════════════════════════╝"

# Run Server
run-server: build-server
	@echo "[*] Lancement du serveur..."
	@$(BIN_DIR)/serveur_tcp.exe

# Run Tests
run-tests: build-tests
	@echo "[*] Lancement des tests..."
	@$(BIN_DIR)/tests_exec.exe

# Clean
clean:
	@echo "[*] Nettoyage..."
	@del /Q $(BIN_DIR)/*.exe 2>nul || true
	@echo "[✓] Fichiers nettoyés"

.DEFAULT_GOAL := help
