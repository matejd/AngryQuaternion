-include Makefile.custom
ProjectName = AngryDudeApp
AngryDudeApp_cppfiles   += ./../../AngryDudeApp/AngryDudeApp.cpp

AngryDudeApp_cpp_debug_dep    = $(addprefix $(DEPSDIR)/, $(subst ./, , $(subst ../, , $(patsubst %.cpp, %.cpp.debug.P, $(AngryDudeApp_cppfiles)))))
AngryDudeApp_c_debug_dep      = $(addprefix $(DEPSDIR)/, $(subst ./, , $(subst ../, , $(patsubst %.c, %.c.debug.P, $(AngryDudeApp_cfiles)))))
AngryDudeApp_debug_dep      = $(AngryDudeApp_cpp_debug_dep) $(AngryDudeApp_c_debug_dep)
-include $(AngryDudeApp_debug_dep)
AngryDudeApp_cpp_release_dep    = $(addprefix $(DEPSDIR)/, $(subst ./, , $(subst ../, , $(patsubst %.cpp, %.cpp.release.P, $(AngryDudeApp_cppfiles)))))
AngryDudeApp_c_release_dep      = $(addprefix $(DEPSDIR)/, $(subst ./, , $(subst ../, , $(patsubst %.c, %.c.release.P, $(AngryDudeApp_cfiles)))))
AngryDudeApp_release_dep      = $(AngryDudeApp_cpp_release_dep) $(AngryDudeApp_c_release_dep)
-include $(AngryDudeApp_release_dep)
AngryDudeApp_debug_hpaths    := 
AngryDudeApp_debug_hpaths    += ./../../AngryDudeApp
AngryDudeApp_debug_hpaths    += ./../../../extensions/include
AngryDudeApp_debug_hpaths    += ./../../../extensions/externals/include
AngryDudeApp_debug_hpaths    += ./../../../extensions/externals/include/GLFW
AngryDudeApp_debug_lpaths    := 
AngryDudeApp_debug_lpaths    += ./../../../extensions/externals/lib/linux64
AngryDudeApp_debug_lpaths    += ./../../../extensions/lib/linux64
AngryDudeApp_debug_lpaths    += ./../../../../../../../../../../../../../usr/lib/x86_64-linux-gnu
AngryDudeApp_debug_lpaths    += ./../../../extensions/externals/lib/osx32
AngryDudeApp_debug_lpaths    += ./../../../extensions/lib/linux64
AngryDudeApp_debug_defines   := $(AngryDudeApp_custom_defines)
AngryDudeApp_debug_defines   += LINUX=1
AngryDudeApp_debug_defines   += GLEW_NO_GLU=1
AngryDudeApp_debug_defines   += _DEBUG
AngryDudeApp_debug_libraries := 
AngryDudeApp_debug_libraries += GL
AngryDudeApp_debug_libraries += GLU
AngryDudeApp_debug_libraries += GLEW
AngryDudeApp_debug_libraries += glfw3
AngryDudeApp_debug_libraries += pthread
AngryDudeApp_debug_libraries += rt
AngryDudeApp_debug_libraries += dl
AngryDudeApp_debug_libraries += X11
AngryDudeApp_debug_libraries += Xrandr
AngryDudeApp_debug_libraries += Xxf86vm
AngryDudeApp_debug_libraries += Xi
AngryDudeApp_debug_libraries += HalfD
AngryDudeApp_debug_libraries += NvAppBaseD
AngryDudeApp_debug_libraries += NvAssetLoaderD
AngryDudeApp_debug_libraries += NvGamepadD
AngryDudeApp_debug_libraries += NvGLUtilsD
AngryDudeApp_debug_libraries += NvModelD
AngryDudeApp_debug_libraries += NvUID
AngryDudeApp_debug_common_cflags	:= $(AngryDudeApp_custom_cflags)
AngryDudeApp_debug_common_cflags    += -MMD
AngryDudeApp_debug_common_cflags    += $(addprefix -D, $(AngryDudeApp_debug_defines))
AngryDudeApp_debug_common_cflags    += $(addprefix -I, $(AngryDudeApp_debug_hpaths))
AngryDudeApp_debug_common_cflags  += -m64
AngryDudeApp_debug_cflags	:= $(AngryDudeApp_debug_common_cflags)
AngryDudeApp_debug_cflags  += -Wall -Wextra -Wno-unused-parameter -Wno-ignored-qualifiers -Wno-unused-but-set-variable -Wno-switch -Wno-unused-variable -Wno-unused-function -Wno-reorder
AngryDudeApp_debug_cflags  += -malign-double
AngryDudeApp_debug_cflags  += -g3
AngryDudeApp_debug_cppflags	:= $(AngryDudeApp_debug_common_cflags)
AngryDudeApp_debug_cppflags  += -Wall -Wextra -Wno-unused-parameter -Wno-ignored-qualifiers -Wno-unused-but-set-variable -Wno-switch -Wno-unused-variable -Wno-unused-function -Wno-reorder
AngryDudeApp_debug_cppflags  += -malign-double
AngryDudeApp_debug_cppflags  += -g3
AngryDudeApp_debug_lflags    := $(AngryDudeApp_custom_lflags)
AngryDudeApp_debug_lflags    += $(addprefix -L, $(AngryDudeApp_debug_lpaths))
AngryDudeApp_debug_lflags    += -Wl,--start-group $(addprefix -l, $(AngryDudeApp_debug_libraries)) -Wl,--end-group
AngryDudeApp_debug_lflags  += -Wl,--unresolved-symbols=ignore-in-shared-libs
AngryDudeApp_debug_lflags  += -m64
AngryDudeApp_debug_lflags  += -m64
AngryDudeApp_debug_objsdir  = $(OBJS_DIR)/AngryDudeApp_debug
AngryDudeApp_debug_cpp_o    = $(addprefix $(AngryDudeApp_debug_objsdir)/, $(subst ./, , $(subst ../, , $(patsubst %.cpp, %.cpp.o, $(AngryDudeApp_cppfiles)))))
AngryDudeApp_debug_c_o      = $(addprefix $(AngryDudeApp_debug_objsdir)/, $(subst ./, , $(subst ../, , $(patsubst %.c, %.c.o, $(AngryDudeApp_cfiles)))))
AngryDudeApp_debug_obj      = $(AngryDudeApp_debug_cpp_o) $(AngryDudeApp_debug_c_o)
AngryDudeApp_debug_bin      := ./../../bin/linux64/AngryDudeApp

clean_AngryDudeApp_debug: 
	@$(ECHO) clean AngryDudeApp debug
	@$(RMDIR) $(AngryDudeApp_debug_objsdir)
	@$(RMDIR) $(AngryDudeApp_debug_bin)

build_AngryDudeApp_debug: postbuild_AngryDudeApp_debug
postbuild_AngryDudeApp_debug: mainbuild_AngryDudeApp_debug
mainbuild_AngryDudeApp_debug: prebuild_AngryDudeApp_debug $(AngryDudeApp_debug_bin)
prebuild_AngryDudeApp_debug:

$(AngryDudeApp_debug_bin): $(AngryDudeApp_debug_obj) build_Half_debug build_NvAppBase_debug build_NvAssetLoader_debug build_NvGamepad_debug build_NvGLUtils_debug build_NvModel_debug build_NvUI_debug 
	@mkdir -p `dirname ./../../bin/linux64/AngryDudeAppD`
	@$(CCLD) $(AngryDudeApp_debug_obj) $(AngryDudeApp_debug_lflags) -o $(AngryDudeApp_debug_bin) 
	@$(ECHO) building $@ complete!

AngryDudeApp_debug_DEPDIR = $(dir $(@))/$(*F)
$(AngryDudeApp_debug_cpp_o): $(AngryDudeApp_debug_objsdir)/%.o:
	@$(ECHO) AngryDudeApp: compiling debug $(filter %$(strip $(subst .cpp.o,.cpp, $(subst $(AngryDudeApp_debug_objsdir),, $@))), $(AngryDudeApp_cppfiles))...
	@mkdir -p $(dir $(@))
	@$(CXX) $(AngryDudeApp_debug_cppflags) -c $(filter %$(strip $(subst .cpp.o,.cpp, $(subst $(AngryDudeApp_debug_objsdir),, $@))), $(AngryDudeApp_cppfiles)) -o $@
	@mkdir -p $(dir $(addprefix $(DEPSDIR)/, $(subst ./, , $(subst ../, , $(filter %$(strip $(subst .cpp.o,.cpp, $(subst $(AngryDudeApp_debug_objsdir),, $@))), $(AngryDudeApp_cppfiles))))))
	@cp $(AngryDudeApp_debug_DEPDIR).d $(addprefix $(DEPSDIR)/, $(subst ./, , $(subst ../, , $(filter %$(strip $(subst .cpp.o,.cpp, $(subst $(AngryDudeApp_debug_objsdir),, $@))), $(AngryDudeApp_cppfiles))))).debug.P; \
	  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(AngryDudeApp_debug_DEPDIR).d >> $(addprefix $(DEPSDIR)/, $(subst ./, , $(subst ../, , $(filter %$(strip $(subst .cpp.o,.cpp, $(subst $(AngryDudeApp_debug_objsdir),, $@))), $(AngryDudeApp_cppfiles))))).debug.P; \
	  rm -f $(AngryDudeApp_debug_DEPDIR).d

$(AngryDudeApp_debug_c_o): $(AngryDudeApp_debug_objsdir)/%.o:
	@$(ECHO) AngryDudeApp: compiling debug $(filter %$(strip $(subst .c.o,.c, $(subst $(AngryDudeApp_debug_objsdir),, $@))), $(AngryDudeApp_cfiles))...
	@mkdir -p $(dir $(@))
	@$(CC) $(AngryDudeApp_debug_cflags) -c $(filter %$(strip $(subst .c.o,.c, $(subst $(AngryDudeApp_debug_objsdir),, $@))), $(AngryDudeApp_cfiles)) -o $@ 
	@mkdir -p $(dir $(addprefix $(DEPSDIR)/, $(subst ./, , $(subst ../, , $(filter %$(strip $(subst .c.o,.c, $(subst $(AngryDudeApp_debug_objsdir),, $@))), $(AngryDudeApp_cfiles))))))
	@cp $(AngryDudeApp_debug_DEPDIR).d $(addprefix $(DEPSDIR)/, $(subst ./, , $(subst ../, , $(filter %$(strip $(subst .c.o,.c, $(subst $(AngryDudeApp_debug_objsdir),, $@))), $(AngryDudeApp_cfiles))))).debug.P; \
	  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(AngryDudeApp_debug_DEPDIR).d >> $(addprefix $(DEPSDIR)/, $(subst ./, , $(subst ../, , $(filter %$(strip $(subst .c.o,.c, $(subst $(AngryDudeApp_debug_objsdir),, $@))), $(AngryDudeApp_cfiles))))).debug.P; \
	  rm -f $(AngryDudeApp_debug_DEPDIR).d
