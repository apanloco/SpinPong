CELL_MK_DIR ?= $(CELL_SDK)/samples/mk
include $(CELL_MK_DIR)/sdk.makedef.mk
CELL_FW_DIR ?= $(CELL_SDK)/samples/fw

SUBDIRS			=	$(CELL_FW_DIR)

PPU_OPTIMIZE_LV		:= -O0
PPU_INCDIRS		+= -I$(CELL_FW_DIR)/include
PPU_INCDIRS		+= -I$(CELL_FW_DIR)/include/psgl
PPU_CPPFLAGS		:= -DPSGL -D_DEBUG -Wall
PPU_CXXSTDFLAGS		+= -fno-exceptions

PPU_SRCS		:= main.cpp Log.cpp TimeControl.cpp Settings.cpp Paddle.cpp Ball.cpp Input.cpp Sound.cpp
PPU_TARGET		:= spinpongps3.elf

PPU_LDLIBDIR		:= -L$(CELL_FW_DIR) -L$(PPU_PSGL_LIBDIR)
PPU_LDLIBS		:= -lfw -lPSGL -lPSGLU -lPSGLFX -lio_stub -lgcm_cmd -lgcm_sys_stub -lsysmodule_stub -lsysutil_stub -lresc_stub -lfs_stub -laudio_stub
PPU_LIBS += $(PPU_TARGET_PATH)/lib/libnetctl_stub.a
PPU_LIBS += $(PPU_TARGET_PATH)/lib/libsysutil_stub.a
PPU_LIBS += $(PPU_TARGET_PATH)/lib/libsysmodule_stub.a
PPU_LIBS += $(PPU_TARGET_PATH)/lib/libnet_stub.a

include $(CELL_MK_DIR)/sdk.target.mk

