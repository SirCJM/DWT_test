#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif  


#define FIRMWARE_VER "Bootloader_V1_1.0"
#define DEBUG_ENABLE  0

#if (DEBUG_ENABLE == 1)
#define DEBUG_PRINTF
#endif

#ifdef __cplusplus
}
#endif

#endif