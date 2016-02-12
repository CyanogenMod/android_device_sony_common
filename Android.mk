LOCAL_PATH := $(call my-dir)

ifeq ($(PRODUCT_BRAND),Sony)
    include $(call all-subdir-makefiles,$(LOCAL_PATH))
endif
