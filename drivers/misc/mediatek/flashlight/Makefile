#
# Makefile for the kernel flashlight device drivers.
#

flashlight-y := flashlight-core.o
flashlight-y += flashlight-device.o

obj-$(CONFIG_MTK_FLASHLIGHT)             += flashlight.o
obj-$(CONFIG_MTK_V4L2_FLASHLIGHT)        += mtk-composite.o
obj-$(CONFIG_MTK_FLASHLIGHT_LED191)      += flashlights-led191.o
obj-$(CONFIG_MTK_FLASHLIGHT_LM3642)      += flashlights-lm3642.o
obj-$(CONFIG_MTK_FLASHLIGHT_LM3643)      += flashlights-lm3643.o
obj-$(CONFIG_MTK_FLASHLIGHT_LM3644)      += flashlights-lm3644.o
obj-$(CONFIG_MTK_FLASHLIGHT_RT4505)      += flashlights-rt4505.o
obj-$(CONFIG_MTK_FLASHLIGHT_RT5081)      += flashlights-rt5081.o
obj-$(CONFIG_MTK_FLASHLIGHT_MT6336)      += flashlights-mt6336.o
obj-$(CONFIG_MTK_FLASHLIGHT_MT6370)      += flashlights-mt6370.o
obj-$(CONFIG_MTK_FLASHLIGHT_MT6360)      += flashlights-mt6360.o
obj-$(CONFIG_MTK_FLASHLIGHT_AW3644)      += flashlights-aw3644.o
obj-$(CONFIG_MTK_FLASHLIGHT_DUMMY)       += flashlights-dummy.o
obj-$(CONFIG_MTK_FLASHLIGHT_DUMMY_GPIO)  += flashlights-dummy-gpio.o
obj-$(CONFIG_MTK_FLASHLIGHT_BARLEY)      += flashlights-barley.o
ifeq ($(CONFIG_OPLUS_CHARGER_MTK6835T),y)
obj-$(CONFIG_MTK_FLASHLIGHT_AVATARL5)    += flashlights-avatarl5.o
obj-$(CONFIG_MTK_FLASHLIGHT_AVATARB5)    += flashlights-avatarb5.o
obj-$(CONFIG_MTK_FLASHLIGHT_23281)       += flashlights-23281.o
obj-$(CONFIG_MTK_FLASHLIGHT_RADO)        += flashlights-rado.o
obj-$(CONFIG_MTK_FLASHLIGHT_RADO_GPIO)   += flashlights-rado-gpio.o
obj-$(CONFIG_MTK_FLASHLIGHT_ALPHAL5)     += flashlights-alphal5.o
obj-$(CONFIG_MTK_FLASHLIGHT_DONGFENG)    += flashlights-dongfeng.o
endif
ccflags-$(CONFIG_MTK_FLASHLIGHT_DEBUG)     := -DDEBUG
ccflags-$(CONFIG_MTK_FLASHLIGHT_PT)        += -DCONFIG_MTK_FLASHLIGHT_PT
ccflags-$(CONFIG_MTK_FLASHLIGHT_PT_STRICT) += -DCONFIG_MTK_FLASHLIGHT_PT_STRICT
ccflags-$(CONFIG_MTK_FLASHLIGHT_DLPT)      += -DCONFIG_MTK_FLASHLIGHT_DLPT

MTK_CUSTOM_PATH := $(srctree)/drivers/misc/mediatek

ifeq ($(CONFIG_MTK_FLASHLIGHT_AW3644),y)
ccflags-y += -DCONFIG_MTK_FLASHLIGHT_AW3644
endif
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include

subdir-ccflags-y += \
	-I$(MTK_CUSTOM_PATH)/power_throttling
subdir-ccflags-y += \
    -I$(MTK_CUSTOM_PATH)/pbm
