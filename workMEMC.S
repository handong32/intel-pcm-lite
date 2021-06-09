	.intel_syntax noprefix
	.text
	.global  memc
memc:
	.rept REPEAT_COUNT
	rep movsb
	.endr

	ret
