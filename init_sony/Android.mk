LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    init_exec.cpp \
    init_files.cpp \
    init_main.cpp

ifeq ($(strip $(BOARD_SONY_INIT_BOARD)),)
    LOCAL_SRC_FILES += init_board_default.cpp
else
    LOCAL_SRC_FILES += ../../../../$(BOARD_SONY_INIT_BOARD)
endif

LOCAL_CPPFLAGS := \
    -Wall \
    -Wextra \
    -Werror

LOCAL_MODULE := init_sony
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_PATH := $(PRODUCT_OUT)/utilities
LOCAL_UNSTRIPPED_PATH := $(PRODUCT_OUT)/symbols/utilities

LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_STATIC_LIBRARIES := \
    libc \
    libcutils

LOCAL_CLANG := true
LOCAL_SANITIZE := integer

include $(BUILD_EXECUTABLE)
