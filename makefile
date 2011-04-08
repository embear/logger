CC = gcc
LD = gcc
RM = rm
SED = sed
ECHO = echo
MKDIR = mkdir
LINT = splint

SRCDIR := src
INCDIR := include
DEPDIR := dep
OBJDIR := obj

APP := logger
SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:%.c=%.o)))
DEPS := $(addprefix $(DEPDIR)/, $(notdir $(SRCS:%.c=%.d)))


CFLAGS :=
CFLAGS += -g
CFLAGS += -DLOGGER_ENABLE
CFLAGS += -DLOGGER_COLORS
CFLAGS += -O1
CFLAGS += -Wall
CFLAGS += -Werror=address
CFLAGS += -Werror=format
CFLAGS += -Werror=format-extra-args
CFLAGS += -Werror=implicit-function-declaration
CFLAGS += -Werror=implicit-int
CFLAGS += -Werror=missing-braces
CFLAGS += -Werror=missing-declarations
CFLAGS += -Werror=missing-prototypes
CFLAGS += -Werror=overflow
CFLAGS += -Werror=parentheses
CFLAGS += -Werror=return-type
CFLAGS += -Werror=sign-compare
CFLAGS += -Werror=switch
CFLAGS += -Werror=undef
CFLAGS += -Werror=unused-value
CFLAGS += -Wextra
CFLAGS += -Wno-comments
CFLAGS += -Wno-error=ignored-qualifiers
CFLAGS += -Wno-error=variadic-macros
CFLAGS += -Wno-missing-field-initializers
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wuninitialized
CFLAGS += -fdiagnostics-show-option
CFLAGS += -fmessage-length=0
LINTFLAGS := -nullstate -varuse -preproc
INCLUDE := -I$(INCDIR)
LDFLAGS := 

.PHONY: all print clean lint

all: $(APP)

print: 
	@$(ECHO) "APP:  $(APP)"
	@$(ECHO) "SRCS: $(SRCS)"
	@$(ECHO) "OBJS: $(OBJS)"
	@$(ECHO) "DEPS: $(DEPS)"

clean:
	@$(ECHO) "Cleaning"
	@$(RM) -rf $(APP) $(OBJS) $(DEPS)

lint:
	$(LINT) $(LINTFLAGS) $(INCLUDE) $(SRCS)

$(APP): $(OBJS)
	@$(ECHO) "Linking $@"
	@$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@$(ECHO) "Compiling $@"
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	@[ -d $(DEPDIR) ] || $(MKDIR) $(DEPDIR)
	@$(ECHO) "Updating dependencies for $<"
	@$(CC) -MM $(CFLAGS) $(INCLUDE) $< > $@.$$$$; $(SED) 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' < $@.$$$$ > $@; $(RM) -f $@.$$$$

-include $(DEPS)
