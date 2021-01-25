/* This file is automatically generated (by scanGROUPS.awk)
** DO NOT EDIT BELOW!
*/

#include "ids.h"

extern char *ids[];

#define MSCAN(F, TIME, PROCESSOR, PROCESS, ID) \
	mscanfTime(F, TIME) + \
	mscanfProcessor(F, PROCESSOR) + \
	mscanfProcess(F, PROCESS) + \
	mscanfId(F, ID)
#define MPRINT(F, TIME, PROCESSOR, PROCESS, ID) \
	mprintfTime(F, TIME) + \
	mprintfDelimiter(F) + \
	mprintfProcessor(F, PROCESSOR) + \
	mprintfDelimiter(F) + \
	mprintfProcess(F, PROCESS) + \
	mprintfDelimiter(F) + \
	mprintfId(F, ID)

#define MSCAN_LENGTH(F, LENGTH) \
	mscanfLength(F, LENGTH)
#define MPRINT_LENGTH(F, LENGTH) \
	mprintfLength(F, LENGTH)

#define MINDEX_HEAP_ALLOC 1
#define MSCAN_HEAP_ALLOC(F, HA_SIZE, HA_ADR, HA_REF) \
	mscanfInt(F, HA_SIZE) + \
	mscanfInt(F, HA_ADR) + \
	mscanfInt(F, HA_REF)
#define MPRINT_HEAP_ALLOC(F, HA_SIZE, HA_ADR, HA_REF) \
	mprintfDelimiter(F) + \
	mprintfInt(F, HA_SIZE) + \
	mprintfDelimiter(F) + \
	mprintfInt(F, HA_ADR) + \
	mprintfDelimiter(F) + \
	mprintfInt(F, HA_REF) + \
	mprintfNewLine(F)

#define MINDEX_HEAP_FREE 2
#define MSCAN_HEAP_FREE(F, HF_ADR, HF_REF) \
	mscanfInt(F, HF_ADR) + \
	mscanfInt(F, HF_REF)
#define MPRINT_HEAP_FREE(F, HF_ADR, HF_REF) \
	mprintfDelimiter(F) + \
	mprintfInt(F, HF_ADR) + \
	mprintfDelimiter(F) + \
	mprintfInt(F, HF_REF) + \
	mprintfNewLine(F)

#define MINDEX_HEAP_COMPACT 3
#define MPRINT_HEAP_COMPACT(F) \
	mprintfNewLine(F)

#define MINDEX_DESC_ALLOC 4
#define MSCAN_DESC_ALLOC(F, DA_ADR) \
	mscanfInt(F, DA_ADR)
#define MPRINT_DESC_ALLOC(F, DA_ADR) \
	mprintfDelimiter(F) + \
	mprintfInt(F, DA_ADR) + \
	mprintfNewLine(F)

#define MINDEX_DESC_FREE 5
#define MSCAN_DESC_FREE(F, DF_ADR) \
	mscanfInt(F, DF_ADR)
#define MPRINT_DESC_FREE(F, DF_ADR) \
	mprintfDelimiter(F) + \
	mprintfInt(F, DF_ADR) + \
	mprintfNewLine(F)

#define MINDEX_STACK_PUSH 6
#define MSCAN_STACK_PUSH(F, PS_STACK) \
	mscanfString(F, PS_STACK)
#define MPRINT_STACK_PUSH(F, PS_STACK) \
	mprintfDelimiter(F) + \
	mprintfString(F, PS_STACK) + \
	mprintfNewLine(F)

#define MINDEX_STACK_POP 7
#define MSCAN_STACK_POP(F, PP_STACK) \
	mscanfString(F, PP_STACK)
#define MPRINT_STACK_POP(F, PP_STACK) \
	mprintfDelimiter(F) + \
	mprintfString(F, PP_STACK) + \
	mprintfNewLine(F)

#define MINDEX_STACK_SEG_FREE 8
#define MSCAN_STACK_SEG_FREE(F, SSF_STACK) \
	mscanfString(F, SSF_STACK)
#define MPRINT_STACK_SEG_FREE(F, SSF_STACK) \
	mprintfDelimiter(F) + \
	mprintfString(F, SSF_STACK) + \
	mprintfNewLine(F)

#define MINDEX_STACK_SEG_ALLOC 9
#define MSCAN_STACK_SEG_ALLOC(F, SSA_STACK) \
	mscanfString(F, SSA_STACK)
#define MPRINT_STACK_SEG_ALLOC(F, SSA_STACK) \
	mprintfDelimiter(F) + \
	mprintfString(F, SSA_STACK) + \
	mprintfNewLine(F)

#define MINDEX_COUNT_DIST 10
#define MPRINT_COUNT_DIST(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_DISTB 11
#define MPRINT_COUNT_DISTB(F) \
	mprintfNewLine(F)

#define MINDEX_PROC_DISTSTACKS 12
#define MSCAN_PROC_DISTSTACKS(F, PDS_SIZE) \
	mscanfInt(F, PDS_SIZE)
#define MPRINT_PROC_DISTSTACKS(F, PDS_SIZE) \
	mprintfDelimiter(F) + \
	mprintfInt(F, PDS_SIZE) + \
	mprintfNewLine(F)

#define MINDEX_PROC_DIST 13
#define MSCAN_PROC_DIST(F, PD_NEXT_PROC, PD_SIZE) \
	mscanfInt(F, PD_NEXT_PROC) + \
	mscanfInt(F, PD_SIZE)
#define MPRINT_PROC_DIST(F, PD_NEXT_PROC, PD_SIZE) \
	mprintfDelimiter(F) + \
	mprintfInt(F, PD_NEXT_PROC) + \
	mprintfDelimiter(F) + \
	mprintfInt(F, PD_SIZE) + \
	mprintfNewLine(F)

#define MINDEX_PROC_NODIST 14
#define MPRINT_PROC_NODIST(F) \
	mprintfNewLine(F)

#define MINDEX_PROC_CREATE 15
#define MSCAN_PROC_CREATE(F, PC_PID) \
	mscanfInt(F, PC_PID)
#define MPRINT_PROC_CREATE(F, PC_PID) \
	mprintfDelimiter(F) + \
	mprintfInt(F, PC_PID) + \
	mprintfNewLine(F)

#define MINDEX_PROC_TERMINATE 16
#define MSCAN_PROC_TERMINATE(F, PT_PID) \
	mscanfInt(F, PT_PID)
#define MPRINT_PROC_TERMINATE(F, PT_PID) \
	mprintfDelimiter(F) + \
	mprintfInt(F, PT_PID) + \
	mprintfNewLine(F)

#define MINDEX_PROC_DISTEND 17
#define MSCAN_PROC_DISTEND(F, PD_RET_PID, PD_RET_PROC) \
	mscanfInt(F, PD_RET_PID) + \
	mscanfInt(F, PD_RET_PROC)
#define MPRINT_PROC_DISTEND(F, PD_RET_PID, PD_RET_PROC) \
	mprintfDelimiter(F) + \
	mprintfInt(F, PD_RET_PID) + \
	mprintfDelimiter(F) + \
	mprintfInt(F, PD_RET_PROC) + \
	mprintfNewLine(F)

#define MINDEX_PROC_SLEEP 18
#define MSCAN_PROC_SLEEP(F, PS_PID) \
	mscanfInt(F, PS_PID)
#define MPRINT_PROC_SLEEP(F, PS_PID) \
	mprintfDelimiter(F) + \
	mprintfInt(F, PS_PID) + \
	mprintfNewLine(F)

#define MINDEX_PROC_RUN 19
#define MSCAN_PROC_RUN(F, PR_PID) \
	mscanfInt(F, PR_PID)
#define MPRINT_PROC_RUN(F, PR_PID) \
	mprintfDelimiter(F) + \
	mprintfInt(F, PR_PID) + \
	mprintfNewLine(F)

#define MINDEX_PROC_WAKEUP 20
#define MSCAN_PROC_WAKEUP(F, PW_PID) \
	mscanfInt(F, PW_PID)
#define MPRINT_PROC_WAKEUP(F, PW_PID) \
	mprintfDelimiter(F) + \
	mprintfInt(F, PW_PID) + \
	mprintfNewLine(F)

#define MINDEX_SEND_BEGIN 21
#define MSCAN_SEND_BEGIN(F, S_TYPE, S_TARGET_PROC, S_TARGET_PID) \
	mscanfString(F, S_TYPE) + \
	mscanfInt(F, S_TARGET_PROC) + \
	mscanfInt(F, S_TARGET_PID)
#define MPRINT_SEND_BEGIN(F, S_TYPE, S_TARGET_PROC, S_TARGET_PID) \
	mprintfDelimiter(F) + \
	mprintfString(F, S_TYPE) + \
	mprintfDelimiter(F) + \
	mprintfInt(F, S_TARGET_PROC) + \
	mprintfDelimiter(F) + \
	mprintfInt(F, S_TARGET_PID) + \
	mprintfNewLine(F)

#define MINDEX_SEND_END 22
#define MSCAN_SEND_END(F, S_SIZE) \
	mscanfInt(F, S_SIZE)
#define MPRINT_SEND_END(F, S_SIZE) \
	mprintfDelimiter(F) + \
	mprintfInt(F, S_SIZE) + \
	mprintfNewLine(F)

#define MINDEX_RECEIVE_BEGIN 23
#define MPRINT_RECEIVE_BEGIN(F) \
	mprintfNewLine(F)

#define MINDEX_RECEIVE_END 24
#define MSCAN_RECEIVE_END(F, R_TYPE, R_FROM_PROC, R_FROM_PID, R_SIZE, R_FOR_PID) \
	mscanfString(F, R_TYPE) + \
	mscanfInt(F, R_FROM_PROC) + \
	mscanfInt(F, R_FROM_PID) + \
	mscanfInt(F, R_SIZE) + \
	mscanfInt(F, R_FOR_PID)
#define MPRINT_RECEIVE_END(F, R_TYPE, R_FROM_PROC, R_FROM_PID, R_SIZE, R_FOR_PID) \
	mprintfDelimiter(F) + \
	mprintfString(F, R_TYPE) + \
	mprintfDelimiter(F) + \
	mprintfInt(F, R_FROM_PROC) + \
	mprintfDelimiter(F) + \
	mprintfInt(F, R_FROM_PID) + \
	mprintfDelimiter(F) + \
	mprintfInt(F, R_SIZE) + \
	mprintfDelimiter(F) + \
	mprintfInt(F, R_FOR_PID) + \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSH_W 25
#define MPRINT_COUNT_PUSH_W(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSHC_W 26
#define MPRINT_COUNT_PUSHC_W(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSH_AW 27
#define MPRINT_COUNT_PUSH_AW(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSH_AW0 28
#define MPRINT_COUNT_PUSH_AW0(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MOVE_AW 29
#define MPRINT_COUNT_MOVE_AW(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_DELTA1 30
#define MPRINT_COUNT_DELTA1(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_DELTA2 31
#define MPRINT_COUNT_DELTA2(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_DELTA3 32
#define MPRINT_COUNT_DELTA3(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_DELTA4 33
#define MPRINT_COUNT_DELTA4(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_ADD 34
#define MPRINT_COUNT_ADD(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_SUB 35
#define MPRINT_COUNT_SUB(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MUL 36
#define MPRINT_COUNT_MUL(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_DIV 37
#define MPRINT_COUNT_DIV(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MOD 38
#define MPRINT_COUNT_MOD(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_EQ 39
#define MPRINT_COUNT_EQ(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_NE 40
#define MPRINT_COUNT_NE(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_LT 41
#define MPRINT_COUNT_LT(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_LE 42
#define MPRINT_COUNT_LE(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_GT 43
#define MPRINT_COUNT_GT(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_GE 44
#define MPRINT_COUNT_GE(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_NEG 45
#define MPRINT_COUNT_NEG(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_NOT 46
#define MPRINT_COUNT_NOT(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_OR 47
#define MPRINT_COUNT_OR(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_AND 48
#define MPRINT_COUNT_AND(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_XOR 49
#define MPRINT_COUNT_XOR(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JFALSE 50
#define MPRINT_COUNT_JFALSE(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JTRUE 51
#define MPRINT_COUNT_JTRUE(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JCOND 52
#define MPRINT_COUNT_JCOND(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSHRET 53
#define MPRINT_COUNT_PUSHRET(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_BETA 54
#define MPRINT_COUNT_BETA(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_BETA0 55
#define MPRINT_COUNT_BETA0(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_BETA1 56
#define MPRINT_COUNT_BETA1(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JTAIL 57
#define MPRINT_COUNT_JTAIL(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JTAIL0 58
#define MPRINT_COUNT_JTAIL0(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JTAIL1 59
#define MPRINT_COUNT_JTAIL1(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_BETAQ 60
#define MPRINT_COUNT_BETAQ(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_BETAQ0 61
#define MPRINT_COUNT_BETAQ0(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_BETAQ1 62
#define MPRINT_COUNT_BETAQ1(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JTAILQ 63
#define MPRINT_COUNT_JTAILQ(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JTAILQ0 64
#define MPRINT_COUNT_JTAILQ0(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JTAILQ1 65
#define MPRINT_COUNT_JTAILQ1(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_FREE_A 66
#define MPRINT_COUNT_FREE_A(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_FREE1_A 67
#define MPRINT_COUNT_FREE1_A(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_RTF 68
#define MPRINT_COUNT_RTF(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_RTM 69
#define MPRINT_COUNT_RTM(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_RTP 70
#define MPRINT_COUNT_RTP(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_RTC 71
#define MPRINT_COUNT_RTC(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_RTS 72
#define MPRINT_COUNT_RTS(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MKGCLOS 73
#define MPRINT_COUNT_MKGCLOS(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MKBCLOS 74
#define MPRINT_COUNT_MKBCLOS(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MKCCLOS 75
#define MPRINT_COUNT_MKCCLOS(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MKSCLOS 76
#define MPRINT_COUNT_MKSCLOS(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MKDCLOS 77
#define MPRINT_COUNT_MKDCLOS(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MKAP 78
#define MPRINT_COUNT_MKAP(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_APPLY 79
#define MPRINT_COUNT_APPLY(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_CASE 80
#define MPRINT_COUNT_CASE(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_WHEN 81
#define MPRINT_COUNT_WHEN(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_GUARD 82
#define MPRINT_COUNT_GUARD(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_BODY 83
#define MPRINT_COUNT_BODY(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_NOMATCH 84
#define MPRINT_COUNT_NOMATCH(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_DUPARG 85
#define MPRINT_COUNT_DUPARG(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_LSEL 86
#define MPRINT_COUNT_LSEL(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_CONS 87
#define MPRINT_COUNT_CONS(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_FCONS 88
#define MPRINT_COUNT_FCONS(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_FIRST 89
#define MPRINT_COUNT_FIRST(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_REST 90
#define MPRINT_COUNT_REST(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_EXT 91
#define MPRINT_COUNT_EXT(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MKILIST 92
#define MPRINT_COUNT_MKILIST(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MKLIST 93
#define MPRINT_COUNT_MKLIST(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSHH 94
#define MPRINT_COUNT_PUSHH(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_DISTEND 95
#define MPRINT_COUNT_DISTEND(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_WAIT 96
#define MPRINT_COUNT_WAIT(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_POPH 97
#define MPRINT_COUNT_POPH(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSH_R 98
#define MPRINT_COUNT_PUSH_R(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSHC_R 99
#define MPRINT_COUNT_PUSHC_R(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MOVE_WR 100
#define MPRINT_COUNT_MOVE_WR(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSHRET_T 101
#define MPRINT_COUNT_PUSHRET_T(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_FREE_R 102
#define MPRINT_COUNT_FREE_R(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_FREE1_R 103
#define MPRINT_COUNT_FREE1_R(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_RTT 104
#define MPRINT_COUNT_RTT(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_RTM_T 105
#define MPRINT_COUNT_RTM_T(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_RTC_T 106
#define MPRINT_COUNT_RTC_T(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_GAMMA 107
#define MPRINT_COUNT_GAMMA(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_GAMMA0 108
#define MPRINT_COUNT_GAMMA0(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_GAMMA1 109
#define MPRINT_COUNT_GAMMA1(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_GAMMAQ 110
#define MPRINT_COUNT_GAMMAQ(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_GAMMAQ0 111
#define MPRINT_COUNT_GAMMAQ0(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_GAMMAQ1 112
#define MPRINT_COUNT_GAMMAQ1(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JTAIL_T 113
#define MPRINT_COUNT_JTAIL_T(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JTAIL0_T 114
#define MPRINT_COUNT_JTAIL0_T(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JTAIL1_T 115
#define MPRINT_COUNT_JTAIL1_T(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JTAILQ_T 116
#define MPRINT_COUNT_JTAILQ_T(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JTAILQ0_T 117
#define MPRINT_COUNT_JTAILQ0_T(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_JTAILQ1_T 118
#define MPRINT_COUNT_JTAILQ1_T(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSH_TW 119
#define MPRINT_COUNT_PUSH_TW(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSH_TW0 120
#define MPRINT_COUNT_PUSH_TW0(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSH_TR 121
#define MPRINT_COUNT_PUSH_TR(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSH_TR0 122
#define MPRINT_COUNT_PUSH_TR0(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSH_AR 123
#define MPRINT_COUNT_PUSH_AR(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_PUSH_AR0 124
#define MPRINT_COUNT_PUSH_AR0(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MOVE_TW 125
#define MPRINT_COUNT_MOVE_TW(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MOVE_TR 126
#define MPRINT_COUNT_MOVE_TR(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MOVE_AR 127
#define MPRINT_COUNT_MOVE_AR(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_GAMMABETA 128
#define MPRINT_COUNT_GAMMABETA(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_FREESW_T 129
#define MPRINT_COUNT_FREESW_T(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_GAMMACASE 130
#define MPRINT_COUNT_GAMMACASE(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MKGACLOS 131
#define MPRINT_COUNT_MKGACLOS(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_POPFREE_T 132
#define MPRINT_COUNT_POPFREE_T(F) \
	mprintfNewLine(F)

#define MINDEX_COUNT_MKGSCLOS 133
#define MPRINT_COUNT_MKGSCLOS(F) \
	mprintfNewLine(F)
