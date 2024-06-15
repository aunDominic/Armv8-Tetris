movz w9, #0
movz w0, #0x3f20, lsl #16
str w9, [w0]
movz w2, #0x40
str w2, [w0]
add w21, w0, #0x1c
add w22, w0, #0x28
movz w4, #0x08

loop:
str w4, [w21]
str w9, [w22]

movz x2, #0x100, lsl #16

delay_loop:
subs x2, x2, #1
b.ne delay_loop

str w4, [w22]
str w9, [w21]

movz x2, #0x100, lsl #16

delay_loo:
subs x2, x2, #1
b.ne delay_loo

b loop