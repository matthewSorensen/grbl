.syntax unified
.align 2
.thumb_func
.global _bit_true_atomic
.type _bit_true_atomic STT_FUNC
_bit_true_atomic:
	ldrex r2, [r0]
	orr r1, r1, r2
	strex r2, r1, [r0]
	cmp r2, #0
	bgt _bit_true_atomic
	bx lr	

.align 2
.thumb_func
.global _bit_false_atomic
.type _bit_false_atomic STT_FUNC
_bit_false_atomic:
	mov r2, #0
	orn r1, r2, r1
negated:	
	ldrex r2, [r0]
	and r1, r1, r2
	strex r2, r1, [r0]
	cmp r2, #0
	bgt negated
	bx lr
	
.align 2
.thumb_func
.global _bit_toggle_atomic
.type _bit_toggle_atomic STT_FUNC
_bit_toggle_atomic:
	ldrex r2, [r0]
	eor r1, r1, r2
	strex r2, r1, [r0]
	cmp r2, #0
	bgt _bit_toggle_atomic
	bx lr	
