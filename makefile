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


CFLAGS := -g -Wall -DLOGGER_ENABLE -DLOGGER_COLORS
#CFLAGS := -g -Wall
LINTFLAGS := -nullstate -varuse 
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
