#ifndef __PGM_SPACE_H_
#define __PGM_SPACE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>


#define PROGMEM
#define PGM_P  		const char *
#define PGM_VOID_P  const void *
#define PSTR(s)     (s)

#ifdef __cplusplus
extern "C" {
#endif

#define _SFR_BYTE(n) (n)

#ifdef __PROG_TYPES_COMPAT__

typedef void prog_void;
typedef char prog_char;
typedef unsigned char prog_uchar;
typedef int8_t prog_int8_t;
typedef uint8_t prog_uint8_t;
typedef int16_t prog_int16_t;
typedef uint16_t prog_uint16_t;
typedef int32_t prog_int32_t;
typedef uint32_t prog_uint32_t;

#endif // defined(__PROG_TYPES_COMPAT__)

#define SIZE_IRRELEVANT 0x7fffffff

// memchr_P and memrchr_P are not implemented due to danger in its use, and
// how uninteresting their use is
// since its a flash string, you should already know where the char is within it,
// further, it could return a pointer into the flash memory that is not 32bit aligned
// which could cause an exception if read
// PGM_VOID_P memchr_P(PGM_VOID_P bufP, int c, size_t count);
// PGM_VOID_P memrchr_P(PGM_VOID_P bufP, int c, size_t count);

int memcmp_P(const void* buf1, PGM_VOID_P buf2P, size_t size);
// memccpy_P is only valid when used with pointers to 8bit data, due to size aligned pointers
// and endianess of the values greater than 8bit, matching c may return invalid aligned pointers
void* memccpy_P(void* dest, PGM_VOID_P src, int c, size_t count);
void* memmem_P(const void* buf, size_t bufSize, PGM_VOID_P findP, size_t findPSize);
void* memcpy_P(void* dest, PGM_VOID_P src, size_t count);

char* strncpy_P(char* dest, PGM_P src, size_t size);
#define strcpy_P(dest, src)          strncpy_P((dest), (src), SIZE_IRRELEVANT)

char* strncat_P(char* dest, PGM_P src, size_t size);
#define strcat_P(dest, src)          strncat_P((dest), (src), SIZE_IRRELEVANT)

int strncmp_P(const char* str1, PGM_P str2P, size_t size);
#define strcmp_P(str1, str2P)          strncmp_P((str1), (str2P), SIZE_IRRELEVANT)

int strncasecmp_P(const char* str1, PGM_P str2P, size_t size);
#define strcasecmp_P(str1, str2P)          strncasecmp_P((str1), (str2P), SIZE_IRRELEVANT)

size_t strnlen_P(PGM_P s, size_t size);
#define strlen_P(strP)          strnlen_P((strP), SIZE_IRRELEVANT)

char* strstr_P(const char* haystack, PGM_P needle);

int	printf_P(PGM_P formatP, ...) __attribute__((format(printf, 1, 2)));
int	sprintf_P(char *str, PGM_P formatP, ...) __attribute__((format(printf, 2, 3)));
int	snprintf_P(char *str, size_t strSize, PGM_P formatP, ...) __attribute__((format(printf, 3, 4)));
int	vsnprintf_P(char *str, size_t strSize, PGM_P formatP, va_list ap) __attribute__((format(printf, 3, 0)));

// flash memory must be read using 32 bit aligned addresses else a processor
// exception will be triggered
// order within the 32 bit values are
// --------------
// b3, b2, b1, b0
//     w1,     w0

// Commented by @bort707  2023.Jun.30
// reason - to get rid numerous strict-aliasing warnings

/*#define pgm_read_byte(addr)     (*reinterpret_cast<const uint8_t*>(addr))
#define pgm_read_word(addr)     (*reinterpret_cast<const uint16_t*>(addr))
#define pgm_read_dword(addr) 		(*reinterpret_cast<const uint32_t*>(addr))
#define pgm_read_float(addr) 		(*reinterpret_cast<const float*>(addr))
#define pgm_read_ptr(addr) 		(*reinterpret_cast<const void* const *>(addr))*/

// Added from the EarlePhilhower RP2040 core
// https://github.com/earlephilhower/ArduinoCore-API/blob/master/api/deprecated-avr-comp/avr/pgmspace.h
//
#ifdef __cplusplus
extern "C"{
#endif
static inline unsigned char pgm_read_byte(const void *addr) {
    return *(const unsigned char *)(addr);
}
static inline unsigned short pgm_read_word(const void *addr) {
    const unsigned char *a = (const unsigned char *)addr;
    return pgm_read_byte(a) | ( pgm_read_byte(a + 1) << 8 );
}
static inline unsigned long pgm_read_dword(const void *addr) {
    const unsigned char *a = (const unsigned char *)addr;
    return pgm_read_byte(a) | ( pgm_read_byte(a + 1) << 8 ) | 
      ( pgm_read_byte(a + 2) << 16 ) | ( pgm_read_byte(a + 3) << 24 );
}
static inline void *pgm_read_ptr(const void *addr) {
    return (void*) pgm_read_dword(addr);
}
static inline float pgm_read_float(const void *addr) {
    union {
        void *p;
        float f;
    } x;
    x.p = pgm_read_ptr(addr);
    return x.f;
}
#ifdef __cplusplus
}
#endif

#define pgm_read_byte_near(addr) 	pgm_read_byte(addr)
#define pgm_read_word_near(addr) 	pgm_read_word(addr)
#define pgm_read_dword_near(addr) 	pgm_read_dword(addr)
#define pgm_read_float_near(addr)	pgm_read_float(addr)
#define pgm_read_ptr_near(addr) 	pgm_read_ptr(addr)
#define pgm_read_byte_far(addr) 	pgm_read_byte(addr)
#define pgm_read_word_far(addr) 	pgm_read_word(addr)
#define pgm_read_dword_far(addr) 	pgm_read_dword(addr)
#define pgm_read_float_far(addr) 	pgm_read_float(addr)
#define pgm_read_ptr_far(addr)		pgm_read_ptr(addr)

#ifdef __cplusplus
}
#endif

#endif //__PGM_SPACE_H_