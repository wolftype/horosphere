#MAKEFILE FOR ALLOSPHERE PROJECTS WITH VERSOR 2.0

CXX = clang++ -std=c++11 -arch x86_64 -O3 -ftemplate-depth-1200

ALLOPATH = $(HOME)/code/AlloSystem/
VSRPATH = $(HOME)/code/vsr2.0/
include $(ALLOPATH)Makefile.common

LDFLAGS += -L$(VSRPATH)/build/lib/ -L$(ALLOPATH)build/lib/ -Iinclude/ -I$(VSRPATH) -I$(VSRPATH)ext/gfx/ -I$(ALLOPATH)build/include/   
LDFLAGS += -lvsr -lallocore -lalloGLV -lalloutil -lGamma -lGLV -llo

MODULES = allocore alloGLV alloutil ../Gamma ../GLV

#ALLMODULES = $(addsuffix .Makefile.link, $(MODULES))
MODULELINKS =  $(addprefix $(ALLOPATH),  $(addsuffix /Makefile.link, $(MODULES) ))

-include $(MODULELINKS)

#link EVERYTHING twice to avoid undefined symbols
LDFLAGS += -lfreeimage
LDFLAGS2 = $(LDFLAGS)# $(LDFLAGS)
CXXFLAGS += $(CPPFLAGS)


EXEC_TARGETS = demos/*.cpp 

ALLOSPHERE = 0  
ifeq ($(ALLOSPHERE),1)
CXXFLAGS += -D__allosphere__
endif

dir:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)
#	@mkdir -p $(PCH_DIR)
#   @mkdir -p $(LIB_DIR)

printG:
	@echo $(CXX) $(CXXFLAGS) $(LDFLAGS2)

printDir:
	@echo $(BUILD_DIR) $(OBJ_DIR) $(BIN_DIR) $(DESTDIR)


FORCE:
.PRECIOUS: $(EXEC_TARGETS)
$(EXEC_TARGETS): dir FORCE
	@echo Building $@ using $<
	@echo $(CXX) $(CXXFLAGS) -H $@ -o $(BIN_DIR)$(*F) $(LDFLAGS2)
	$(CXX) $(CXXFLAGS) $@ -o $(BIN_DIR)$(*F) $(LDFLAGS2)
	@cd $(BIN_DIR) && ./$(*F)

