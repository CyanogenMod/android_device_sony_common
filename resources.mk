# Boot Logo
PRODUCT_COPY_FILES += \
     device/sony/common/bootlogo/$(TARGET_SCREEN_WIDTH)x$(TARGET_SCREEN_HEIGHT).rle:root/logo.rle

# For formatting data after unlock
PRODUCT_PACKAGES += \
    setup_fs
