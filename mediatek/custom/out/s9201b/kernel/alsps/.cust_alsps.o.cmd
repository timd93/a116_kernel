cmd_mediatek/custom/out/s9201b/kernel/alsps/cust_alsps.o := arm-linux-androideabi-gcc -Wp,-MD,mediatek/custom/out/s9201b/kernel/alsps/.cust_alsps.o.d  -nostdinc -isystem /home/ubuntu/projects/wiko/9201_3g_call/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/bin/../lib/gcc/arm-linux-androideabi/4.6.x-google/include -I/home/ubuntu/projects/GPL/merge_9201/kernel/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include /home/ubuntu/projects/GPL/merge_9201/kernel/include/linux/kconfig.h -I../mediatek/custom///common -I../mediatek/platform/mt6589/kernel/core/include/ -I../mediatek/kernel/include/ -I../mediatek/custom/out/s9201b/kernel/flashlight/ -I../mediatek/custom/out/s9201b/kernel/vibrator/ -I../mediatek/custom/out/s9201b/kernel/leds/ -I../mediatek/custom/out/s9201b/kernel/rtc/ -I../mediatek/custom/out/s9201b/kernel/magnetometer/ -I../mediatek/custom/out/s9201b/kernel/camera/ -I../mediatek/custom/out/s9201b/kernel/headset/ -I../mediatek/custom/out/s9201b/kernel/accelerometer/ -I../mediatek/custom/out/s9201b/kernel/kpd/ -I../mediatek/custom/out/s9201b/kernel/battery/ -I../mediatek/custom/out/s9201b/kernel/core/ -I../mediatek/custom/out/s9201b/kernel/alsps/ -I../mediatek/custom/out/s9201b/kernel/dct/ -I../mediatek/custom/out/s9201b/kernel/usb/ -I../mediatek/custom/out/s9201b/kernel/gyroscope/inc/ -I../mediatek/custom/out/s9201b/kernel/barometer/inc/ -I../mediatek/custom/out/s9201b/kernel/flashlight/inc/ -I../mediatek/custom/out/s9201b/kernel/imgsensor/ -I../mediatek/custom/out/s9201b/kernel/imgsensor/inc/ -I../mediatek/custom/out/s9201b/kernel/leds/inc/ -I../mediatek/custom/out/s9201b/kernel/jogball/inc/ -I../mediatek/custom/out/s9201b/kernel/magnetometer/inc/ -I../mediatek/custom/out/s9201b/kernel/cam_cal/inc/ -I../mediatek/custom/out/s9201b/kernel/cam_cal/ -I../mediatek/custom/out/s9201b/kernel/lens/inc/ -I../mediatek/custom/out/s9201b/kernel/lens/ -I../mediatek/custom/out/s9201b/kernel/sound/ -I../mediatek/custom/out/s9201b/kernel/sound/inc/ -I../mediatek/custom/out/s9201b/kernel/ccci/ -I../mediatek/custom/out/s9201b/kernel/touchpanel/ -I../mediatek/custom/out/s9201b/kernel/accelerometer/inc/ -I../mediatek/custom/out/s9201b/kernel/lcm/ -I../mediatek/custom/out/s9201b/kernel/lcm/inc/ -I../mediatek/custom/out/s9201b/kernel/hdmi/inc/ -I../mediatek/custom/out/s9201b/kernel/ssw/ -I../mediatek/custom/out/s9201b/kernel/ssw/inc/ -I../mediatek/custom/out/s9201b/kernel/./ -I../mediatek/custom/out/s9201b/kernel/eeprom/inc/ -I../mediatek/custom/out/s9201b/kernel/eeprom/ -I../mediatek/custom/out/s9201b/kernel/alsps/inc/ -I../mediatek/custom/out/s9201b/hal/imgsensor/ -I../mediatek/custom/out/s9201b/hal/lens/ -I../mediatek/custom/out/s9201b/hal/camera/ -I../mediatek/custom/out/s9201b/hal/audioflinger/ -I../mediatek/custom/out/s9201b/hal/sensors/ -I../mediatek/custom/out/s9201b/hal/inc/ -I../mediatek/custom/out/s9201b/hal/camera/inc/ -I../mediatek/custom/out/s9201b/hal/flashlight/ -I../mediatek/custom/out/s9201b/hal/fmradio/ -I../mediatek/custom/out/s9201b/hal/matv/ -I../mediatek/custom/out/s9201b/hal/cam_cal/ -I../mediatek/custom/out/s9201b/hal/ant/ -I../mediatek/custom/out/s9201b/hal/vt/ -I../mediatek/custom/out/s9201b/hal/bluetooth/ -I../mediatek/custom/out/s9201b/hal/combo/ -I../mediatek/custom/out/s9201b/hal/eeprom/ -I../mediatek/custom/out/s9201b/common -I../mediatek/custom/s9201b/common -D__KERNEL__ -mlittle-endian -DMODEM_3G -Imediatek/platform/mt6589/kernel/core/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -fno-pic -marm -fno-dwarf2-cfi-asm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -DMTK_BT_PROFILE_AVRCP -DMTK_GPS_SUPPORT -DMTK_FM_SUPPORT -DMTK_USES_HD_VIDEO -DMTK_BT_PROFILE_MANAGER -DMTK_FM_RECORDING_SUPPORT -DMTK_BT_PROFILE_PBAP -DMTK_BT_PROFILE_HFP -DMTK_MASS_STORAGE -DMTK_COMBO_QUICK_SLEEP_SUPPORT -DMTK_THEMEMANAGER_APP -DMTK_HDR_SUPPORT -DMTK_MERGE_INTERFACE_SUPPORT -DHAVE_AACENCODE_FEATURE -DMTK_WIFI_HOTSPOT_SUPPORT -DMTK_COMBO_SUPPORT -DMTK_BT_PROFILE_OPP -DMTK_2SDCARD_SWAP -DMTK_FLIGHT_MODE_POWER_OFF_MD -DMTK_MULTI_STORAGE_SUPPORT -DCUSTOM_KERNEL_ALSPS -DMTK_ENABLE_VIDEO_EDITOR -DCUSTOM_KERNEL_ACCELEROMETER -DMTK_DUAL_MIC_SUPPORT -DMTK_WAPI_SUPPORT -DMTK_FD_SUPPORT -DMTK_BT_SUPPORT -DMTK_FACEBEAUTY_SUPPORT -DMTK_BQ24158_SUPPORT -DMTK_BT_40_SUPPORT -DMTK_HIGH_QUALITY_THUMBNAIL -DMTK_SPH_EHN_CTRL_SUPPORT -DMTK_ENABLE_MD1 -DHAVE_XLOG_FEATURE -DMTK_IPV6_SUPPORT -DMTK_AUTORAMA_SUPPORT -DMTK_EAP_SIM_AKA -DMTK_MATV_ANALOG_SUPPORT -DMTK_BT_PROFILE_A2DP -DMTK_KERNEL_POWER_OFF_CHARGING -DCUSTOM_KERNEL_MAGNETOMETER -DMTK_BT_PROFILE_HIDH -DMTK_BT_PROFILE_PAN -DMTK_CAMERA_APP_3DHW_SUPPORT -DMTK_WLAN_SUPPORT -DMTK_PQ_SUPPORT -DMTK_TETHERINGIPV6_SUPPORT -DMTK_PLATFORM_OPTIMIZE -DMTK_PRODUCT_INFO_SUPPORT -DMTK_M4U_SUPPORT -DMTK_BT_PROFILE_SPP -DMTK_BT_30_SUPPORT -DMTK_ION_SUPPORT -DHAVE_AWBENCODE_FEATURE -DMTK_CAMERA_BSP_SUPPORT -DMTK_FM_RX_SUPPORT -DMTK_WB_SPEECH_SUPPORT -DMTK_VT3G324M_SUPPORT -DCUSTOM_HAL_FMRADIO -DMTK_SENSOR_SUPPORT -DMTK_EMMC_SUPPORT -DMTK_BT_21_SUPPORT -DMTK_COMBO_CORE_DUMP_SUPPORT -DMT6589 -DOV8825AF -DDUMMY_LENS -DDUMMY_LENS -DHD720 -DSSD2075_HD720_DSI_VDO_TRULY -DHX8394A_DSI_VDO -DMODEM_3G -DOV8825_MIPI_RAW -DMTK_MT6628 -DMT6628 -DHI253_YUV -DCONSTANT_FLASHLIGHT -DOV2659_YUV -DDUMMY_LENS -DMTK_AUDIO_BLOUD_CUSTOMPARAMETER_V4 -DOV8825_MIPI_RAW -DOV2659_YUV -DHI253_YUV -DFM_ANALOG_INPUT -DMT6628 -DMTK_GPS_MT6628 -DDUMMY_LENS -DOV8825AF -DFM_ANALOG_OUTPUT -DMT6628_FM -DMTK_SIM1_SOCKET_TYPE=\"1\" -DMTK_LCM_PHYSICAL_ROTATION=\"0\" -DLCM_WIDTH=\"720\" -DMTK_SHARE_MODEM_SUPPORT=\"2\" -DCUSTOM_KERNEL_SSW=\"generic\" -DMTK_NEON_SUPPORT=\"yes\" -DMTK_SHARE_MODEM_CURRENT=\"2\" -DLCM_HEIGHT=\"1280\" -DMTK_SIM2_SOCKET_TYPE=\"1\" -Wframe-larger-than=2048 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(cust_alsps)"  -D"KBUILD_MODNAME=KBUILD_STR(cust_alsps)" -c -o mediatek/custom/out/s9201b/kernel/alsps/cust_alsps.o mediatek/custom/out/s9201b/kernel/alsps/cust_alsps.c

source_mediatek/custom/out/s9201b/kernel/alsps/cust_alsps.o := mediatek/custom/out/s9201b/kernel/alsps/cust_alsps.c

deps_mediatek/custom/out/s9201b/kernel/alsps/cust_alsps.o := \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/ubuntu/projects/GPL/merge_9201/kernel/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  arch/arm/include/generated/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /home/ubuntu/projects/GPL/merge_9201/kernel/arch/arm/include/asm/posix_types.h \
  include/asm-generic/posix_types.h \
  ../mediatek/platform/mt6589/kernel/core/include/mach/mt_pm_ldo.h \
  ../mediatek/platform/mt6589/kernel/core/include/mach/mt_typedefs.h \
  include/linux/bug.h \
    $(wildcard include/config/generic/bug.h) \
  /home/ubuntu/projects/GPL/merge_9201/kernel/arch/arm/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/thumb2/kernel.h) \
    $(wildcard include/config/debug/bugverbose.h) \
    $(wildcard include/config/arm/lpae.h) \
  include/linux/linkage.h \
  /home/ubuntu/projects/GPL/merge_9201/kernel/arch/arm/include/asm/linkage.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
    $(wildcard include/config/smp.h) \
  ../mediatek/custom/out/s9201b/kernel/alsps/inc/cust_alsps.h \

mediatek/custom/out/s9201b/kernel/alsps/cust_alsps.o: $(deps_mediatek/custom/out/s9201b/kernel/alsps/cust_alsps.o)

$(deps_mediatek/custom/out/s9201b/kernel/alsps/cust_alsps.o):
