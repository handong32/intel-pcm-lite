	.intel_syntax noprefix
	.text
	.global  suma
suma:
	.rept REPEAT_COUNT
	addq [rdi], 0x1
	.endr

	ret
