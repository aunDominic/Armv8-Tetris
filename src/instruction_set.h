/**
 * @file instruction_set.h
 * @brief ARMv8 Instruction Set Module Interface
 * @author Andrei Cravtov
 *
 * TODO: document this implementation
 */

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "types.h"

// ---- MACRO DEFINITIONS ----

// ---- TYPE DEFINITIONS ----

// ---- INTERFACE DEFINITIONS ----

/**
 * mnemonic                | instruction                               | meaning                                                                     | type
 * ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * add(s) rd, rn,  <op2>   | Add (set flags)                           | Rd  :=  Rn  +  Op2                                                          | (Data Processing > Arithmetic)
 * sub(s) rd, rn,  <op2>   | Subtract (set flags)                      | Rd  :=  Rn  −  Op2                                                          | (Data Processing > Arithmetic)
 *
 * and(s) rd, rn,  <op2>   | And (set flags)                           | Rd  :=  Rn  &  Op2                                                          | (Data Processing > Logical)
 * bic(s) rd, rn,  <op2>   | Bit Clear (set flags)                     | Rd  :=  Rn  & ∼Op2                                                          | (Data Processing > Logical)
 * eor    rd, rn,  <op2>   | Exclusive Or                              | Rd  :=  Rn  ∧  Op2                                                          | (Data Processing > Logical)
 * eon    rd, rn,  <op2>   | Exclusive Or Not                          | Rd  :=  Rn  ∧ ∼Op2                                                          | (Data Processing > Logical)
 * orr    rd, rn,  <op2>   | Or                                        | Rd  :=  Rn  ∧  Op2                                                          | (Data Processing > Logical)
 * orn    rd, rn,  <op2>   | Or Not                                    | Rd  :=  Rn  ∧ ∼Op2                                                          | (Data Processing > Logical)
 *
 * movz   rd, imm, lsl #sh | Move Wide with Zero                       | Rd := imm << sh                                                             | (Data Processing > Wide Moves)
 * movn   rd, imm, lsl #sh | Move Wide with Not                        | Rd := ~(imm << sh)                                                          | (Data Processing > Wide Moves)
 * movk   rd, imm, lsl #sh | Move Wide with Keep                       | Rd[sh + 15 : sh] := imm                                                     | (Data Processing > Wide Moves)
 *
 * madd rd, rn, rm, ra     | Multiply-Add                              | Rd := Ra + (Rm*Rn)                                                          | (Data Processing > Multiply)
 * msub rd, rn, rm, ra     | Multiply-Subtract                         | Rd := Ra - (Rm*Rn)                                                          | (Data Processing > Multiply)
 *
 * b <literal>             | Unconditional Branch Literal              | PC := <literal>                                                             | (Branch > Unconditional)
 * br xn                   | Unconditional Branch Register             | PC := Xn                                                                    | (Branch > Unconditional)
 *
 * b.eq <literal>          | Conditional Branch Literal, Equal         | If Z = 1,                     then PC := <literal>, {Encoding: 0000}        | (Branch > Conditional)
 * b.ne <literal>          | Conditional Branch Literal, Not Equal     | If Z = 0,                     then PC := <literal>, {Encoding: 0001}        | (Branch > Conditional)
 * b.ge <literal>          | Conditional Branch Literal, Signed >=     | If N = V,                     then PC := <literal>, {Encoding: 1010}        | (Branch > Conditional)
 * b.lt <literal>          | Conditional Branch Literal, Signed <      | If N != V,                    then PC := <literal>, {Encoding: 1011}        | (Branch > Conditional)
 * b.gt <literal>          | Conditional Branch Literal, Signed >      | If (Z = 0) and (N = V),       then PC := <literal>, {Encoding: 1100}        | (Branch > Conditional)
 * b.le <literal>          | Conditional Branch Literal, Signed <=     | If not [(Z = 0) and (N = V)], then PC := <literal>, {Encoding: 1101}        | (Branch > Conditional)
 * b.al <literal>          | Conditional Branch Literal, Always        |                                    PC := <literal>, {Encoding: 1110}        | (Branch > Conditional)
 *
 * ldr rt [xn]             | Load Into Register, Zero Unsigned Offset  | Rt := ∗(address) {Transfer Address: Xn}                                     | (Single Data Transfer) <- possibly alias!!
 * ldr rt [xn, #<imm>]     | Load Into Register, Unsigned Offset       | Rt := ∗(address) {Transfer Address: Xn + imm}                               | (Single Data Transfer)
 * ldr rt [xn], #<simm>    | Load Into Register, Post-Indexed          | Rt := ∗(address) {Transfer Address: Xn,        Write-back: Xn := Xn + simm} | (Single Data Transfer) <- possibly alias??
 * ldr rt [xn, #<simm>]!   | Load Into Register, Pre-Indexed           | Rt := ∗(address) {Transfer Address: Xn + simm, Write-back: Xn := Xn + simm} | (Single Data Transfer)
 * ldr rt [xn, xm]         | Load Into Register, Register Offset       | Rt := ∗(address) {Transfer Address: Xn + Xm}                                | (Single Data Transfer)
 * ldr rt <literal>        | Load Into Register, From Literal          | Rt := ∗(address) {Transfer Address: <literal>}                              | (Single Data Transfer)
 *
 * str rt [xn]             | Store From Register, Zero Unsigned Offset | ∗(address) := Rt {Transfer Address: Xn}                                     | (Single Data Transfer) <- possibly alias!!
 * str rt [xn, #<imm>]     | Store From Register, Unsigned Offset      | ∗(address) := Rt {Transfer Address: Xn + imm}                               | (Single Data Transfer)
 * str rt [xn], #<simm>    | Store From Register, Post-Indexed         | ∗(address) := Rt {Transfer Address: Xn,        Write-back: Xn := Xn + simm} | (Single Data Transfer) <- possibly alias??
 * str rt [xn, #<simm>]!   | Store From Register, Pre-Indexed          | ∗(address) := Rt {Transfer Address: Xn + simm, Write-back: Xn := Xn + simm} | (Single Data Transfer)
 * str rt [xn, xm]         | Store From Register, Register Offset      | ∗(address) := Rt {Transfer Address: Xn + Xm}                                | (Single Data Transfer)
 *
 * and x0, x0, x0          | Halt                                      | Halt                                                                        | (Special Instructions > Halt)
 */

#endif //INSTRUCTION_H
