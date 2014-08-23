#!/usr/bin/make

DEPSDIR = .deps
OBJS_DIR  = build
RMDIR     = rm -fr
ECHO      = echo
CCLD      = g++ -std=c++11
CXX       = g++ -std=c++11
CC        = g++
RANLIB    = ranlib
AR		 = ar
STRIP     = 
OBJDUMP   = 
OBJCOPY   =
-include Makedefs.linux64.mk

all: debug

debug: build_NvAppBase_debug build_NvModel_debug build_NvGLUtils_debug build_NvGamepad_debug build_NvAssetLoader_debug build_NvUI_debug build_Half_debug build_R3_debug build_AngryDudeApp_debug

include Makefile.NvAppBase.mk
include Makefile.NvModel.mk
include Makefile.NvGLUtils.mk
include Makefile.NvGamepad.mk
include Makefile.NvAssetLoader.mk
include Makefile.NvUI.mk
include Makefile.Half.mk
include Makefile.R3.mk
include Makefile.AngryDude.mk
