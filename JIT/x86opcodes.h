#ifndef X86OPCODES_H_INCLUDED
#define X86OPCODES_H_INCLUDED

const int BYTE_SIZE = 8;

// mov rax , 0      ->  48 c7 c0 | 00 00 00 00      // MOV_REG_NUM += 4 NOP
// mov rbx , 0      ->  48 c7 c3 | 00 00 00 00
// mov rcx , 0      ->  48 c7 c1 | 00 00 00 00

// mov rbp , 0      ->  48 c7 c5 | 00 00 00 00
// mov rsp , 0      ->  48 c7 c4 | 00 00 00 00

// mov rbp, rsp     ->  48 89 e5
// mov rsp, rbp     ->  48 89 ec

// mov rbx, rbp     ->  48 89 eb
// mov rbx, rax     ->  48 89 c3

// mov rax, [rbx]   ->  48 8b 03
// mov [rbx], rax   ->  48 89 03

// =================================================

// push rax         ->  50                          // add NOP to x86
// push rbp         ->  55

// pop  rax         ->  58
// pop  rbx         ->  5b
// pop  rcx         ->  59
// pop  rbp         ->  5d

// =================================================

// add  rbx, rax    ->  48 01 c3
// add  rax, rbx    ->  48 01 d8
// add  rbx, rcx    ->  48 01 cb

// sub  rax, rbx    ->  48 29 d8
// sub  rbx, rax    ->  48 29 c3
// sub  rbx, rcx    ->  48 29 cb
// sub  rsp, rcx    ->  48 29 cc

// div rbx          ->  48 f7 f3                    // add NOP to div, mul
// mul rbx          ->  48 f7 e3

// =================================================

// cmp  rax, rbx    ->  48 39 d8                    // Aligned to 6 bytes

// jmp  rcx         ->  ff e1                       //  ?

// jmp  lbl         ->  eb 01
// ja   lbl         ->  77 01
// jb   lbl         ->  72 01
// je   lbl         ->  74 01
// jne  lbl         ->  75 01

// jmp  lbl         ->  e9 00 ff ff ff
// ja   lbl         ->  0f 87 00 ff ff ff
// jb   lbl         ->  0f 82 00 ff ff ff
// je   lbl         ->  0f 84 00 ff ff ff
// jne  lbl         ->  0f 85 00 ff ff ff

// call lbl         ->  e8 00 00 00 00

// =================================================

// ret              ->  c3

// =================================================

// out              ->  call ***                    // Aligned to 6 bytes


#endif // X86OPCODES_H_INCLUDED
