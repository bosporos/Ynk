//
// file ynk/string/utf8_strlen.s
// author Maximilien M. Cura
//

.globl ___ynk_utf8_strlen

//! Adapted for AMD64 from Aristotle Pagaltzis' UTF8 strlen function:
//! Discussed by Kragen Javier Sitaker at canonical.org/~kragen/strlen-utf8
//!
//! rdi ccx
___ynk_utf8_strlen:
    xorq %rcx, %rcx // rcx0
    movq %rdi, %rsi // bkp
___ynk_utf8_strlen.l0:
    decq %rcx
___ynk_utf8_strlen.l1:
    lodsb
    shl $1, %al
    js ___hubstd_fastutf8strlen.l0 // chk utf8 enc
    jc ___hubstd_fastutf8strlen.l1
    jnz ___hubstd_fastutf8strlen.l0
    mov %rcx, %rax
    not %rax // b/c decq fr 0
    retq
