// Why is this harmless function implemented in assembly, and why
// are we telling the linker to place it in RAM? flexram.c will solve
// all such mysteries.
.syntax unified
.data
.align 2
.thumb_func
.global initialize_flexram_cmd
.type initialize_flexram_cmd STT_FUNC
// void initialize_flexram_cmd(volatile uint8_t*);
initialize_flexram_cmd:
	mvn r3, #127
	strb r3, [r0]
wait:	
	ldrb r3, [r0]
	tst r3, #128
	beq wait
	bx lr
