CXX  = g++
LINK = g++
CXXFLAGS = -pedantic -Wall -pipe
LDFLAGS = -m64 -Wl,--hash-style=gnu
LIBS = -lalpm -lncursesw
RM = rm
INSTALL_PROGRAM = install -Dm755
SOURCES = package.cpp \
		  main.cpp \
		  cursesframe.cpp \
		  curseslistbox.cpp \
		  program.cpp \
		  config.cpp \
		  filter.cpp \
		  attributeinfo.cpp \
		  history.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = pcurses
bindir = /usr/bin

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINK) $(LDFLAGS) $(LIBS) $(OBJECTS) -o $@

%.o: %.c
	$(CXX) $(CFLAGS) $< -o $@

install:
	$(INSTALL_PROGRAM) $(TARGET) $(DESTDIR)/$(bindir)/$(TARGET)

clean:
	$(RM) -f $(TARGET) $(OBJECTS)
