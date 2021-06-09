	.intel_syntax noprefix
	.text
	.global  mem
mem:
	.rept REPEAT_COUNT
	movq rcx, [rsi]
	movq [rdx], rcx

	movq rcx, [rdi]
	movq [rsi], rcx

	movq rcx, [rdx]
	movq [rdi], rcx
	.endr

	ret
