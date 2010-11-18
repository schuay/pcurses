CXX  = g++
LINK = g++
CXXFLAGS = -pedantic -Wall -pipe -W -D_REENTRANT
LDFLAGS = -m64 -Wl,--hash-style=gnu
LIBS = -lalpm -lncursesw
RM = rm
INSTALL = install
SOURCES = package.cpp \
		  main.cpp \
		  cursesframe.cpp \
		  curseslistbox.cpp \
		  program.cpp \
		  config.cpp \
		  filter.cpp \
		  attributeinfo.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = pcurses

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINK) $(LDFLAGS) $(LIBS) $(OBJECTS) -o $@

%.o: %.c
	$(CXX) $(CFLAGS) $< -o $@

clean:
	$(RM) -f $(TARGET) $(OBJECTS)
