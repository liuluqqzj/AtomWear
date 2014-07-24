#ifndef _FLASH_CTRL_H
#define _FLASH_CTRL_H

void flash_word_write(uint32_t *address, uint32_t value);

void flash_specword_write(uint32_t *address, uint8_t offset, uint32_t data);

#endif
