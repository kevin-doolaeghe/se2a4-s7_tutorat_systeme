# Compiler
CC          = gcc

# Executable name
TARGET      = output

# Paths
SRCDIR      = src
OBJDIR      = build
PATHLIBH    = includes
PATHLIBA    = includes

# Libraries to link
LIBS        = $(addprefix -l, $(patsubst lib%, %, $(basename $(notdir $(wildcard $(PATHLIBA)/*.a))))) -lusb-1.0 -lm

# Compiler and linker flags
INCLUDES    = -I$(PATHLIBH)
LDFLAGS     = -L$(PATHLIBA) $(LIBS)
CFLAGS      = -Wall -Wextra -g -O0 $(INCLUDES)

EXEC        = $(addprefix $(OBJDIR)/, $(TARGET))

CFILES      = $(notdir $(wildcard $(addprefix $(SRCDIR)/, *c)))
SRC         = $(addprefix $(SRCDIR)/, $(CFILES))

OBJ         = $(addprefix $(OBJDIR)/, $(CFILES:.c=.o))

#####################################################

all : build

build : prepare $(EXEC)

rebuild : clean build

#####################################################

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(EXEC) : $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

#####################################################

prepare :
	mkdir -p $(OBJDIR)

format :
	clang-format -i $(SRCDIR)/*.c

#####################################################

clean :
	rm -fr $(OBJDIR)/*.o

mrproper :
	rm -fr $(OBJDIR)

.PHONY : clean mrproper prepare
