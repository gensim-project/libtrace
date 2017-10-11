
LIB_SOURCES=${shell find lib/|grep '.cpp$$'}
LIB_OBJS=$(LIB_SOURCES:%.cpp=build/%.o)
LIB_LIBDIR=dist/lib

TOOL_SOURCES=${shell find tools/|grep '.cpp$$'}
TOOL_OBJS=$(TOOL_SOURCES:%.cpp=build/%.o)
TOOLS=$(TOOL_SOURCES:tools/%.cpp=dist/bin/%)

ALL_SOURCES=$(LIB_SOURCES) $(TOOL_SOURCES)
ALL_OBJS=$(LIB_OBJS) $(TOOL_OBJS)

INCDIRS=-I./inc/
LIB=dist/lib/libtrace.a
LIBS=$(LIB) -lncurses

DEBUGFLAGS=-g
ifeq ($(NDEBUG),1)
	DEBUGFLAGS=-DNDEBUG
endif
CPPFLAGS=$(INCDIRS) $(DEBUGFLAGS) -fPIC -std=c++11 -pthread -D_GLIBCXX_USE_NANOSLEEP -Wextra -Werror=uninitialized -Werror=unused-value -Werror=return-type

ifeq ($(FUZZ),1)
	CXX=afl-g++
	CPPFLAGS += -DFUZZ=1 -O3
endif

ifeq ($(OPT),1)
	CPPFLAGS += -O3 -DMULTITHREAD=1
endif

all: $(TOOLS) $(LIB)

tests : $(TOOLS) .FORCE
	make CPPFLAGS="$(CPPFLAGS)" -C tests

dist/bin/% : build/tools/%.o $(LIB)
	@mkdir -p `dirname $@`
	@echo " [LD] => $@"
	@$(CXX) $< $(LIBS) -o $@ -pthread
	

SUFFIXES:.o .cpp 
.PHONY: print grammars .FORCE

$(ALL_OBJS):build/%.o:%.cpp
	@echo " [CC] $< => $@"
	@mkdir -p `dirname $@`
	@$(CXX) $(CPPFLAGS) -MD -c $< -o $@

-include $(ALL_SOURCES:%.cpp=build/%.d)

$(LIB) : $(LIB_OBJS)
	@mkdir -p dist/lib
	@echo " [AR] => $@"
	@$(AR) crs $@ $(LIB_OBJS)
	
clean:
	rm -rf build/ dist/
