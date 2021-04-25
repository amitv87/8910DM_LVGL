#ifndef MIDDLEWARE_UNISOC_BLUETOOTH_BLUEU_HAL_FREERTOS_OSI_INCLUDE_OSI_UTILS_H_
#define MIDDLEWARE_UNISOC_BLUETOOTH_BLUEU_HAL_FREERTOS_OSI_INCLUDE_OSI_UTILS_H_

#define ARRAY_REV_TO_STREAM(p, a, len) do{register int ijk; for (ijk = 0; ijk < (len);        ijk++) *(p)++ = (UINT8) ((a)[(len) - 1 - (ijk)]) ;} while(0)
#define ARRAY_TO_STREAM(p, a, len) do{register int ijk; for (ijk = 0; ijk < (len);        ijk++) *(p)++ = (UINT8) ((a)[(ijk)]); } while(0)

#define UINT32_TO_STREAM(p, u32) do{*(p)++ = (UINT8)(u32); *(p)++ = (UINT8)((u32) >> 8); *(p)++ = (UINT8)((u32) >> 16); *(p)++ = (UINT8)((u32) >> 24);} while(0)
#define UINT24_TO_STREAM(p, u24) do{*(p)++ = (UINT8)(u24); *(p)++ = (UINT8)((u24) >> 8); *(p)++ = (UINT8)((u24) >> 16);} while(0)
#define UINT16_TO_STREAM(p, u16) do{*(p)++ = (UINT8)(u16); *(p)++ = (UINT8)((u16) >> 8);} while(0)
#define UINT8_TO_STREAM(p, u8)   do{*(p)++ = (UINT8)(u8);} while(0)

#define STREAM_TO_UINT8(u8, p)   do{u8 = (UINT8)(*(p)); (p) += 1;} while(0)
#define STREAM_TO_UINT16(u16, p) do{u32 = ((UINT16)(*(p)) + (((UINT16)(*((p) + 1))) << 8)); (p) += 2;} while(0)
#define STREAM_TO_UINT24(u32, p) do{u32 = (((UINT32)(*(p))) + ((((UINT32)(*((p) + 1)))) << 8) + ((((UINT32)(*((p) + 2)))) << 16) ); (p) += 3;} while(0)
#define STREAM_TO_UINT32(u32, p) do{u32 = (((UINT32)(*(p))) + ((((UINT32)(*((p) + 1)))) << 8) + ((((UINT32)(*((p) + 2)))) << 16) + ((((UINT32)(*((p) + 3)))) << 24)); (p) += 4;} while(0)


#endif /* MIDDLEWARE_UNISOC_BLUETOOTH_BLUEU_HAL_FREERTOS_OSI_INCLUDE_OSI_UTILS_H_ */
