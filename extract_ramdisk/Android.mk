LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := extract_ramdisk.cpp main.cpp
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_STATIC_LIBRARIES := libelf libc libm libz
LOCAL_C_INCLUDES := \
	external/elfutils/0.153/libelf \
	external/zlib
LOCAL_CPPFLAGS := -g -c -W
LOCAL_MODULE := extract_ramdisk
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_CLASS := RECOVERY_EXECUTABLES
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/utilities
LOCAL_PACK_MODULE_RELOCATIONS := false
include $(BUILD_EXECUTABLE)

