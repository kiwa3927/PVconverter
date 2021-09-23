# envs  
PLATFORM= "$(shell ./config/guess-platform)"
PROCESSOR= "$(shell ./config/GuessPlatform -processor)"
_COMPILE_JOBS = $(shell grep processor /proc/cpuinfo | wc -l)

# LINUX: fedora3-linux26-athlon, fedora4-linux26-i686, fedora5-linux26-athlon,
#		 fedora6-linux26-i686, fedora7-linux26-i686, linux24, redhat8, redhat9,
#		 redhate3-linux24-athlon, redhate3-linux24-i686, redhate3-linux24-x86_64,
#		 redhate4-linux26-athlon, redhate4-linux26-i686, redhate4-linux26-x86_64,
#		 redhate5-linux26-i686
# SUN  : solaris, solaris26, solaris7, solaris8, solaris9, sunos
# HP   : hpux10, hpux11
MACHINE="LINUX"
ifeq ($(PLATFORM), sunos)
MACHINE="SUN"
else 
ifeq ($(patsubst solaris%, solaris, $(PLATFORM)), solaris)
MACHINE="SUN"
else 
ifeq ($(patsubst hpux%, hpux, $(PLATFORM)), hpux)
MACHINE="HP"
endif
endif
endif

# processor type

BYTE_ORDER="HV_LITTLE_ENDIAN" 
CPU_INSN_TYPE="HV_CISC" 
ARCH = "-march=nocona" #"-DHV_X86 -mfpmath=sse"
#ARCH = "-march=athlon"

KERNAL="LINUX26"
ifeq ($(shell echo $(PLATFORM) | cut -c 1-7), linux24)
KERNAL="LINUX24"
endif

BITFLAG="BIT32"
ifeq ($(shell ./config/GuessPlatform -processor), i686)
else
ifeq ($(shell ./config/GuessPlatform -processor), x86_64)
BITFLAG="BIT64"
else
ifeq ($(shell ./config/GuessPlatform -processor), sparc)
BYTE_ORDER="HV_BIG_ENDIAN"
CPU_INSN_TYPE="HV_RISC"
endif
endif
endif

ifeq ($(BITFLAG), "BIT64")
ASYNC      = ""
else
ASYNC      = "-fasynchronous-unwind-tables"
endif

_SOURCE_ROOT = $(SOURCE_ROOT)
ifeq ($(strip $(SOURCE_ROOT)), )
_SOURCE_ROOT = .
endif

SHARE_DIR=""$(shell pwd)"/share"
SHARE_INC="-isystem$(SHARE_DIR)/include" "-I/usr/X11R6/include"
SHARE_LIB="-L$(SHARE_DIR)/$(PLATFORM)/lib" "$(SHARE_DIR)/$(PLATFORM)/lib/libtcl8.5.a" "-ldl" "-lX11" "-lXft" "-lpthread" "-lfontconfig" "$(SHARE_DIR)/$(PLATFORM)/lib/libtk8.5.a"  

time:
	@echo ----------------------------------------------

install:
	@make project \
	        WHAT=install TYPE=publish \
	        SOURCE_ROOT="./public ./preprocess ./lexical ./syntax ./tvf ./gui ./main" \
	        TARGET_NAME="svrf2pvrs"\
	        CFLAGS=" -g -O3 -fomit-frame-pointer -Wall -Wno-deprecated $(ARCH) -DNDEBUG -DINSTALL \
	        		 -D$(MACHINE) -D$(KERNAL) -D$(BITFLAG) -D$(BYTE_ORDER) -D$(CPU_INSN_TYPE)"\
	        INCFLAGS="-I"$(shell pwd)"/src $(SHARE_INC)"\
	        LIBFLAGS="$(SHARE_LIB)"

debug:
	@make project \
		WHAT=debug \
		SOURCE_ROOT="./public ./preprocess ./lexical ./syntax ./tvf ./gui ./main" \
		TARGET_NAME="svrf2pvrs"\
		CFLAGS=" -g -Wall -Wno-deprecated $(ARCH) -DDEBUG \
			     -D$(MACHINE) -D$(KERNAL) -D$(BITFLAG) -D$(BYTE_ORDER) -D$(CPU_INSN_TYPE)"\
        INCFLAGS="-I"$(shell pwd)"/src $(SHARE_INC)"\
        LIBFLAGS="$(SHARE_LIB)"

gcov:
	@make project \
	        WHAT=install TYPE=publish \
	        SOURCE_ROOT="./public ./preprocess ./lexical ./syntax ./tvf ./gui ./main" \
	        TARGET_NAME="svrf2pvrs"\
	        CFLAGS=" -g -O3 -fomit-frame-pointer -fprofile-arcs -ftest-coverage -Wall -Wno-deprecated $(ARCH) -DNDEBUG -DINSTALL \
	        		 -D$(MACHINE) -D$(KERNAL) -D$(BITFLAG) -D$(BYTE_ORDER) -D$(CPU_INSN_TYPE)"\
	        INCFLAGS="-I"$(shell pwd)"/src $(SHARE_INC)"\
	        LIBFLAGS="-lpthread -fprofile-arcs -ftest-coverage $(SHARE_LIB)"

project:
	@if test "$(WHAT)" != ""; then \
		if [ -d ./platform/$(PLATFORM)/bin ]; then \
			echo "./platform/$(PLATFORM)/bin exists."; \
		else \
			echo "Build a bin directory..."; \
			mkdir -p ./platform/$(PLATFORM)/bin; \
			echo DONE; \
		fi; \
		echo "Making $(WHAT), please wait ... "; \
		cd ./src; \
		time -p make -j$(_COMPILE_JOBS) all \
			WHAT="$(WHAT)" TARGET_NAME="$(TARGET_NAME)"\
			SOURCE_ROOT="$(_SOURCE_ROOT)" \
			CFLAGS="$(CFLAGS)" \
			PLATFORM="$(PLATFORM)"\
			INCFLAGS="$(INCFLAGS)" \
			LIBFLAGS="$(LIBFLAGS)"; \
		cd ..; \
		echo DONE; \
	fi
