LOCAL_PATH := $(call my-dir)

ifeq ($(BOARD_VENDOR),Sony)
    include $(call all-subdir-makefiles,$(LOCAL_PATH))
endif
