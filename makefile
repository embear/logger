SILENT ?= @

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
LIBDIR := lib

INCDIRS := 
INCDIRS += h

APP := logger
LIB := logger

APPSRCS := $(SRCDIR)/main.c
LIBSRCS := $(SRCDIR)/logger.c
APPOBJS := $(addprefix $(OBJDIR)/, $(notdir $(APPSRCS:%.c=%.o)))
LIBOBJS := $(addprefix $(OBJDIR)/, $(notdir $(LIBSRCS:%.c=%.o)))
LIBNAME := $(LIBDIR)/lib$(LIB).a
DEPS := $(addprefix $(DEPDIR)/, $(notdir $(LIBSRCS:%.c=%.d)) $(notdir $(LIBSRCS:%.c=%.d)))

CFLAGS :=
CFLAGS += -g
CFLAGS += -DLOGGER_ENABLE
CFLAGS += -DLOGGER_COLORS
CFLAGS += -DLOGGER_FORMAT_SIMPLE
#CFLAGS += -DLOGGER_FORMAT_FULL
#CFLAGS += -DLOGGER_FORMAT_FILE
#CFLAGS += -DLOGGER_FORMAT_FUNCTION
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
INCLUDE := -I$(INCDIR)

LDFLAGS := 
LDFLAGS += -L$(LIBDIR) -l$(LIB)

.PHONY: all print clean distclean

all: $(APP)

print: 
	$(SILENT)$(ECHO) "APP:     $(APP)"
	$(SILENT)$(ECHO) "LIB:     $(LIB)"
	$(SILENT)$(ECHO) "APPSRCS: $(APPSRCS)"
	$(SILENT)$(ECHO) "LIBSRCS: $(LIBSRCS)"
	$(SILENT)$(ECHO) "APPOBJS: $(APPOBJS)"
	$(SILENT)$(ECHO) "LIBOBJS: $(LIBOBJS)"
	$(SILENT)$(ECHO) "INCLUDE:  $(INCLUDE)"
	$(SILENT)$(ECHO) "DEPS:    $(DEPS)"

clean:
	$(SILENT)$(ECHO) "Cleaning"
	$(SILENT)$(RM) -rf $(APP) $(LIBNAME) $(APPOBJS) $(LIBOBJS) $(DEPS)

distclean:
	$(SILENT)$(ECHO) "Dist Cleaning"
	$(SILENT)$(RM) -rf $(APP) $(LIBDIR) $(OBJDIR) $(DEPDIR) logfile

$(APP): $(APPOBJS) $(LIBNAME)
	$(SILENT)$(ECHO) "Linking $@"
	$(SILENT)$(LD) -o $@ $(APPOBJS) $(LDFLAGS)

$(LIBNAME): $(LIBOBJS)
	$(SILENT)[ -d $(LIBDIR) ] || $(MKDIR) $(LIBDIR)
	$(SILENT)$(ECHO) "Archiving $@"
	$(SILENT)ar r $@ $(LIBOBJS) >/dev/null 2>&1

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(SILENT)[ -d $(OBJDIR) ] || $(MKDIR) $(OBJDIR)
	$(SILENT)$(ECHO) "Compiling $@"
	$(SILENT)$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	$(SILENT)[ -d $(DEPDIR) ] || $(MKDIR) $(DEPDIR)
	$(SILENT)$(ECHO) "Updating dependencies for $<"
	$(SILENT)$(CC) -MM $(CFLAGS) $(INCLUDE) $< > $@.$$$$; $(SED) 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' < $@.$$$$ > $@; $(RM) -f $@.$$$$

-include $(DEPS)
