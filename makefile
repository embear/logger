CC = gcc
LD = gcc
RM = rm
SED = sed
ECHO = echo
MKDIR = mkdir

SRCDIR := src
INCDIR := include
DEPDIR := dep
OBJDIR := obj

APP := logger
SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:%.c=%.o)))
DEPS := $(addprefix $(DEPDIR)/, $(notdir $(SRCS:%.c=%.d)))

CFLAGS := -g -Wall -I$(INCDIR)
LDFLAGS := 

all: $(APP)

print: 
	@$(ECHO) "APP:  $(APP)"
	@$(ECHO) "SRCS: $(SRCS)"
	@$(ECHO) "OBJS: $(OBJS)"
	@$(ECHO) "DEPS: $(DEPS)"

clean:
	@$(ECHO) "Cleaning"
	@$(RM) -rf $(APP) $(OBJS) $(DEPS)

$(APP): $(OBJS)
	@$(ECHO) "Linking $@"
	@$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@$(ECHO) "Compiling $@"
	@$(CC) $(CFLAGS) -c $< -o $@

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	@[ -d $(DEPDIR) ] || $(MKDIR) $(DEPDIR)
	@$(ECHO) "Updating dependencies for $<"
	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; $(SED) 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' < $@.$$$$ > $@; $(RM) -f $@.$$$$

-include $(DEPS)
