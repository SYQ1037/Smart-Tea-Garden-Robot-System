#ifndef __MLX90640_H
#define __MLX90640_H		
#include "sys.h"

extern const uint16_t camColors[256];

void uart5_init(uint32_t bound);
void set_bound(uint32_t bound);
void set_refresh(float Freq);
void save_command(void);
void data_processing(volatile uint16_t *raw_data,uint16_t max,uint16_t min,uint8_t model,volatile uint16_t *out_pic);

#endif
