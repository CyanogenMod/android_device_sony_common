# Boot Logo
ifneq ($(FXP_BUILD),true)
PRODUCT_COPY_FILES += \
     device/sony/common/bootlogo/$(TARGET_SCREEN_WIDTH)x$(TARGET_SCREEN_HEIGHT).rle:root/logo.rle
else
PRODUCT_COPY_FILES += \
     device/sony/common/bootlogo/fxp/$(TARGET_SCREEN_WIDTH)x$(TARGET_SCREEN_HEIGHT).rle:root/logo.rle
endif

# For formatting data after unlock
PRODUCT_PACKAGES += \
    setup_fs
