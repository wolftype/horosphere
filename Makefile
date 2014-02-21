#MAKEFILE FOR ALLOSPHERE PROJECTS WITH VERSOR 2.0
# LINUX or MAC

PORT = 22
HOST = 192.168.0.15
NAME = main


#LIBRARIES MUST BE IN /code/
ALLOPATH = $(HOME)/code/AlloSystem/
VSRPATH = $(HOME)/code/versor/
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

#ifeq ($(PLATFORM), linux)
CXXFLAGS += -std=c++0x -ftemplate-depth-1200
#else ifeq ($(PLATFORM), macosx)
CXX = clang++ -std=c++11 -arch x86_64 -O3 -ftemplate-depth-1200
#endif


dir:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

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

run:
	@cd $(BIN_DIR) && ./$(NAME)

#deployment from cuttlefish approach
copy:
	parallel-scp -h hosts.txt -l sphere build/bin/$(NAME) /tmp

many:
	parallel-ssh -h hosts.txt /tmp/$(NAME)

many-kill:
	parallel-ssh -h hosts.txt "pkill $(NAME)"

kill:
	ssh -p $(PORT) $(HOST) "rm -f /tmp/$(NAME)"
	ssh -p $(PORT) $(HOST) "pkill $(NAME)"

deploy:
	ssh -p $(PORT) $(HOST) "rm -f /tmp/$(NAME)"
	scp -P $(PORT) $(BIN_DIR)$(NAME) $(HOST):/tmp
	ssh -t -p $(PORT) $(HOST) /tmp/$(NAME)
