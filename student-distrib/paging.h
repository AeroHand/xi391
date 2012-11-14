
#ifndef PAGING_H
#define PAGING_H

#include "x86_desc.h"
#include "types.h"


int32_t init_paging(void);
int32_t setup_new_task( uint8_t process_number );

#endif

