	.global work
	.intel_syntax noprefix
	.text
work:
	.rept REPEAT_COUNT
	addq    rsi, rdi
	popcnt  rax, rsi
	.endr

	ret

