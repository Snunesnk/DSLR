# Makefile

# Compilateur
CXX = g++

# Options de compilation
CXXFLAGS = -std=c++20 -Iinc -Wall -Wextra

# Liste des programmes à générer
PROGRAMS = describe histogram scatter_plot pair_plot logreg_train logreg_predict

# Génération des noms des fichiers objets
OBJECTS = $(PROGRAMS:%=%.o)

# Règle de construction de tous les programmes
all: $(PROGRAMS)

# Règle générique pour la construction d'un programme
%: src/%.cpp src/utils.cpp src/calculate.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

# Nettoyage des fichiers objets et exécutables
clean:
	rm -rf $(PROGRAMS:%=%.*) 

# Règle pour nettoyer et reconstruire
re: clean all

