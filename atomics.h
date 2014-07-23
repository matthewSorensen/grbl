// For correctness, certain bit-masking operations must be performed atomically.
// The idiomatic way of implementing this on the cortex-m4 seems to be with
// LDREX/STREX, which allow for something like software-transactional memory.
// atomics.s contains the actual implementation.
#ifndef atomics_h
#define atomics_h

void _bit_true_atomic(volatile uint32_t*,uint32_t);
void _bit_false_atomic(volatile uint32_t*,uint32_t);
void _bit_toggle_atomic(volatile uint32_t*,uint32_t);

#define bit_true_atomic(x,mask) {_bit_true_atomic(&(x),mask);}
#define bit_false_atomic(x,mask) {_bit_false_atomic(&(x),mask);}
#define bit_toggle_atomic(x,mask) {_bit_toggle_atomic(&(x),mask);}

#endif
