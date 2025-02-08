#ifndef COMMON_H
#define COMMON_H

#define MAX_BUFFER_SIZE (255)
#define CMD_LOOPBACK (0xaa)
// SLAVE --------------------
#if defined(MODE_SLAVE)

#if defined(ARCH_UNO)
#define CS_n (2)
#define CLK (13)
#define DATA (12)

#else
#error "Architecture not supported in MODE SLAVE"
#endif
// MASTER --------------------
#elif defined(MODE_MASTER)

#if defined(ARCH_UNO)
#define CLK (13)
#define DATA (12)

#elif defined(ARCH_PRO_MICRO)
#define CLK (15)
#define DATA (14)

#else
#error "Architecture not supported in MODE MASTER"
#endif /* defined(ARCH_...) */

#endif /* defined(MODE_...) */

#endif /* COMMON_H */
