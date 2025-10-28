CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
TARGET = gppc
SRCDIR = src
OBJDIR = obj
INCLUDES = -I$(SRCDIR)

SRCS = $(wildcard $(SRCDIR)/*.cpp)

OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "-> [Linker] Erstelle Programm: $@"
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "-> [Make] Fertig: $(TARGET) wurde erstellt."

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR) # Erstelle den obj-Ordner, falls er nicht existiert
	@echo "-> [Compiler] Kompiliere: $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "-> [Make] Räume auf..."
	rm -rf $(OBJDIR) $(TARGET)
	@echo "-> [Make] Aufgeräumt."

.PHONY: all clean