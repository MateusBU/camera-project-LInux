# Variáveis do compilador e flags
CXX = g++
CXXFLAGS = -Wall -O2 $(shell pkg-config --cflags opencv4)
LIBS = $(shell pkg-config --libs opencv4)

# Nome do executável final e do arquivo-fonte
TARGET = timelapse
SRC = main.cpp

# Regra principal (padrão ao rodar apenas 'make')
all: $(TARGET)

# Regra para compilar o executável
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

# Regra para limpar os arquivos gerados
clean:
	rm -f $(TARGET)

.PHONY: all clean