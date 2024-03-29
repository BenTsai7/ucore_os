#ifndef __KERN_DRIVER_CLOCK_H__
#define __KERN_DRIVER_CLOCK_H__

#include <defs.h>

extern volatile size_t ticks;
extern volatile size_t pgfault_update_ticks;
void clock_init(void);

long SYSTEM_READ_TIMER( void );


#endif /* !__KERN_DRIVER_CLOCK_H__ */

