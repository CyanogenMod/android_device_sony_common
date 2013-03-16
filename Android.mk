LOCAL_PATH := $(call my-dir)

ifeq ($(BOARD_VENDOR),sony)
    include $(call all-subdir-makefiles,$(LOCAL_PATH))
endif
