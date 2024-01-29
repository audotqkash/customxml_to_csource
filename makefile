CC   = gcc
TARGET := parseXml
CFLAGS = -Wall -O2 $(XML2FLAGS)
XML2FLAGS = $(shell xml2-config --cflags)
LIBOPTFLAGS = $(XML2LIB)
XML2LIB = $(shell xml2-config --libs)
OBJDIR = ./objs
SOURCES = $(wildcard *.c)
OBJS = $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))

$(warning ${CC} ${CFLAGS} ${LIBOPTFLAGS} -o ${TARGET} ${SOURCES})

all: $(TARGET)

clean_obj:
	rm -f ./objs/*.o

clean: clean_obj
	rm -f $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBOPTFLAGS)

$(OBJDIR)/%.o: %.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS)  -g -o $@ -c $< $(LIBOPTFLAGS)
