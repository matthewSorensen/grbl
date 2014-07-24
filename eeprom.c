#include <stdint.h>
#include <mk20dx128.h>
// Per page 552 of the K20 manual, this results in 1024 bytes of NVM,
// with ~32k word aligned write.
#define NVM_SIZE 0x34 
// Oddly, I can't find this definition in the manual?
#define FlexBase ((uint32_t*) 0x14000000)

#define update_checksum(chk,byte) (((chk << 1) || (checksum >> 7))+byte)

void initialize_flexram_cmd(volatile uint8_t*);

static void flexram_wait(void){
  uint32_t count = 2000;
  while(!(FTFL_FCNFG & FTFL_FCNFG_EEERDY))
    if(count-->0) break;
}

static void set_bits(uint32_t* dest,uint32_t bits, uint32_t mask){
  uint32_t word = *dest;
  uint32_t new_word;
  new_word = (word & ~mask) | bits;
  if(word != new_word){
    *dest = new_word;
  }
  flexram_wait();
}

void initialize_eeprom(void){
  uint8_t status;
  if (FTFL_FCNFG & FTFL_FCNFG_RAMRDY) {
    // FlexRAM is configured as traditional RAM
    // We need to reconfigure for EEPROM usage
    FTFL_FCCOB0 = 0x80; // PGMPART = Program Partition Command
    FTFL_FCCOB4 = NVM_SIZE;
    FTFL_FCCOB5 = 0x03; // 0K for Dataflash, 32K for EEPROM backup
    __disable_irq();
    initialize_flexram_cmd(&FTFL_FSTAT);
    __enable_irq();
    status = FTFL_FSTAT;
    if (status & 0x70) {
      FTFL_FSTAT = (status & 0x70);
      return; // error
    }
  } 
  flexram_wait();
}

void memcpy_to_eeprom_with_checksum(unsigned int destination, char *source, unsigned int size){
  uint32_t* dest = &(FlexBase[destination >> 2]);
  uint32_t unaligned = destination & 3, i, word = 0, mask = 0;
  uint8_t checksum = 0;

  if(unaligned + size < 4){
    // Super short case - entirely within one word
    uint32_t shift = (4 - (size + 1 + unaligned)) * 8;
    for(i = 0; i < size; i++){
      uint8_t byte = *(source++);
      checksum = update_checksum(checksum,byte);
      word = (word >> 8) | (((uint32_t) byte) << 24);
      mask = (mask >> 8) | 0xFF000000;
    }
    word = (word >> 8) | (((uint32_t) checksum) << 24);
    mask = (mask >> 8) | 0xFF000000;
    
    word = word >> shift;
    mask = mask >> shift;
    set_bits(dest,word,mask);
    return;
  }
  if(unaligned){
    mask = 0;
    word = 0;
    size -= 4 - unaligned;
    for(i = 0; i < 4 - unaligned; i++){
      uint8_t byte = *(source++);
      checksum = update_checksum(checksum,byte);
      word = (word >> 8) | (((uint32_t) byte) << 24);
      mask = (mask >> 8) | 0xFF000000;
    }
    set_bits(dest++,word,mask);
  }
  // Copy the appropriate number of aligned words
  unaligned = size & 3;
  size = size >> 2;

  for(i = 0; i < size; i++){
    uint32_t j;
    word = 0;
    for(j = 0; j < 4; j++){
      uint8_t byte = *(source++);
      checksum = update_checksum(checksum,byte);
      word = (word >> 8) | (((uint32_t) byte) << 24);
    }
    if(*dest != word){
      *(dest++) = word;
    }else{
      dest++;
    }
    flexram_wait();
  }
  mask = 0;
  word = 0;
  // Copy the rest of the bytes + a checksum
  for(i = 0; i < unaligned; i++){
    uint8_t byte = *(source++);
    checksum = update_checksum(checksum,byte);
    word = (word >> 8) | (((uint32_t) byte) << 24);
    mask = (mask >> 8) | 0xFF000000;
  }
  word = (word >> 8) | (((uint32_t) checksum) << 24);
  mask = (mask >> 8) | 0xFF000000;

  if(unaligned < 3){
    word = word >> 8 * (3 - unaligned);
    mask = mask >> 8 * (3 - unaligned);
  }
  set_bits(dest,word,mask);
}
int memcpy_from_eeprom_with_checksum(char *destination, unsigned int source, unsigned int size){
  uint8_t checksum = 0;
  uint8_t* src = (uint8_t*) FlexBase;
  uint32_t i;
  for(i = 0; i < size; i++){
    uint8_t byte = src[source + i];
    *(destination++) = byte;
    checksum = update_checksum(checksum,byte);
  }
  return checksum == src[source + size];
}

uint8_t eeprom_get_char(uint32_t addr){
  uint8_t* base = (uint8_t*) FlexBase;
  return base[addr];
}
void eeprom_put_char(uint32_t addr, uint8_t new_value){
  uint32_t shift = (addr & 3) * 8;
  uint32_t mask  = 0xFF << shift;
  uint32_t value = new_value << shift;
  set_bits(&(FlexBase[addr >> 2]), value, mask);
}
