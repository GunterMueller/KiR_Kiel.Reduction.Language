/* command - header                    ccommand.h */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/

#define c_reduce	10
#define c_special	11
#define c_call_red	12
#define c_save_graph	13
#define c_prepro	14

#define c_save	15
#define c_load	16
#define c_store	17
#define c_read	18
#define c_write	19
#define c_pretty	20
#define c_append	21
#define c_width	22
#define c_print	23
#define c_paper	24  /*stt*/
#define c_doc	25
#define c_get	26
#define c_arity	30
#define c_stack	40
#define c_help	50
#define c_keys	55
#define c_showkeys	57
#define c_news	60
#define c_prot	70
#define c_editlib	80
#define c_editext	81
#define c_initfile	90
#define c_exit	100
#define c_ex_graph	110
#define c_edp	150

#define c_environment   160     /* stt 10.01.96 */
#define c_screenswitch  165     /* stt 10.01.96 */

#define c_version	200
#define c_switch	201
#define c_transl	202

#define c_mkap	210
#define c_mkfct	211

#define c_comp	230
#define c_find	231
#define c_next	232
#define c_subst	235

#define c_refresh	250
#define c_resize	251
#define c_setcurpos	252
#define c_gohome	253
#define c_my_stack	254
#define c_size	255
#define c_edsave	256
#define c_edload	257
#define c_archive_expr	258
#define c_myinput	259
#define c_mykeymodes	260
#define c_curmode	261
#define c_findexpr	262
#define c_replace	263
#define c_clearprogram	264
#define c_myspecial	265
#define c_setupload	266
#define c_setupsave	267

#define c_dmode	        270
#define c_iamode	271    /* stt 17.02.97 */

#define c_initparms	300
#define c_saveparms	310
#define c_redparms	320
#define c_proc_time	321
#define c_pgsize	330
#define c_redcmd	340
#define c_redcnt	350
#define c_precis	360
#define c_prec_mult	362
#define c_trunc	370
#define c_indent	380
#define c_hpdes	390
#define c_hpsiz	400
#define c_stdes	410
#define c_stpg	420
#define c_stcntpg	430
#define c_qstsiz	442
#define c_mstsiz	450
#define c_istsiz	460
#define c_fixform	470
#define c_varform	480
#define c_base	490
#define c_betacnt	500
#define c_parms	510

#define c_s_stop	600
#define c_c_stop	601
#define c_s_error	610
#define c_c_error	611

#define c_double	650

#define c_user	700
#define c_position	710
#define c_font	720

#define c_shell	800
#define c_cat	810
#define c_cmd	820

#define c_small	1000
#define c_ddecl	1001
#define c_dfct	1002
#define c_dgoal	1003
#define c_dtdecl	1004
#define c_dvert	1005
#define c_dvert1	1006
#define c_dvert2	1007

#define c_os_preload	1500
#define c_os_unload	1501
#define c_preload	1510

/* max. cmd-nr: 0x0fff    */

#define CODECMD(cmd,x)    ((cmd) | (x))
#define GETCMD(x)         ((x) & 0x0fff)
/* we could put other information in the upper part of cmd-nr	*/
/* i.e. whether a user is allowed to perform the command	*/

struct cmnd {
   char *cmd;
   int  cnmb;
 };

#ifdef iscmd

struct cmnd cmdtb[] = {
	 "abb_def"		,c_ddecl
	,"abb_idef"		,c_dtdecl
	,"appLY"		,c_mkap
	,"appEND"		,c_append
        ,"archive_expr"		,c_archive_expr
	,"arITY"		,c_arity
	,"baSE"			,c_base
	,"beTACOUNT"		,c_betacnt
	,"beTA_COUNT"		,c_betacnt
	,"beTA_COUNT_ONLY"	,c_betacnt
	,"calL"			,c_call_red
	,"calLRED"		,c_call_red
	,"cat"			,c_cat
	,"clearprogram"		,c_clearprogram
	,"clr_error"		,c_c_error
	,"clr_stop"		,c_c_stop
	,"cmD"			,c_cmd
	,"coMP"			,c_comp
	,"cuRMODE"		,c_curmode
	,"ddECL"		,c_ddecl
#if UNIX
	,"deFKEYS"		,c_keys
#endif
	,"dfCT"			,c_dfct
	,"dgOAL"		,c_dgoal
	,"diVPRECISION"		,c_precis	/* new	*/
	,"diV_PRECISION"	,c_precis	/* new	*/
	,"dmODE"		,c_dmode
	,"doC"			,c_doc
	,"doUBLE"		,c_double
	,"dtDECL"		,c_dtdecl
	,"edITPARMS"		,c_edp
	,"ed_load"		,c_edload
	,"ed_save"		,c_edsave
	,"envIRONMENT"          ,c_environment
        ,"exit"			,c_exit
	,"exitgRAPH"		,c_ex_graph
	,"ext"			,c_editext
	,"finD"			,c_find
	,"findeXPRESSION"	,c_findexpr /* UDI */
	,"fixFORMAT"		,c_fixform
	,"font"			,c_font
	,"fuNCTION"		,c_mkfct
	,"gET"			,c_get
	,"heapdES"		,c_hpdes
	,"heapsIZE"		,c_hpsiz
	,"helP"			,c_help
        ,"iamODE"               ,c_iamode       /* stt 17.02.97 */
/*
	,"indENTATION"		,c_indent
*/
	,"initfILE"		,c_initfile
	,"initpARMS"		,c_initparms
	,"isTACKSIZE"		,c_istsiz
	,"gohome"		,c_gohome
	,"loAD"			,c_load
	,"liB"			,c_editlib
	,"mULTPRECISION"	,c_prec_mult	/* new	*/
	,"mULT_PRECISION"	,c_prec_mult	/* new	*/
	,"mSTACKSIZE"		,c_mstsiz
        ,"my_input"		,c_myinput   /* new UDI 2.8.93 */
	,"my_keymodes"		,c_mykeymodes /* new UDI 6.8.93 */
	,"my_special"		,c_myspecial /* new UDI 19.8.93 */
	,"my_stack"		,c_my_stack
	,"newS"			,c_news
	,"nexT"			,c_next
	,"pagESIZE"		,c_pgsize
	,"paper"		,c_paper	/*stt*/
	,"parMS"		,c_parms
	,"posITION"		,c_position
	,"pp"			,c_pretty
	,"precISION"		,c_precis
	,"prelOAD"		,c_preload
	,"prepROCESS"		,c_prepro
	,"priNT"		,c_print
	,"proTOCOLL"		,c_prot
	,"quitgRAPH"		,c_ex_graph
	,"qsTACKSIZE"		,c_qstsiz
	,"reaD"			,c_read
	,"redcmD"		,c_redcmd
	,"redcnT"		,c_redcnt
	,"redpARMS"		,c_redparms
	,"rEDUCE"		,c_reduce
	,"refRESH"		,c_refresh
	,"rePLACE"		,c_replace     /* new UDI 15.7.93 */
	,"reSIZE"		,c_resize      /* new UDI 15.7.93 */
	,"save"			,c_save
	,"savegRAPH"		,c_save_graph
	,"savepARMS"		,c_saveparms
	,"scrEENSWITCH"         ,c_screenswitch
        ,"set_error"		,c_s_error
	,"set_stop"		,c_s_stop
        ,"set_curpos"		,c_setcurpos
	,"setupload"		,c_setupload
	,"setupsave"		,c_setupsave
	,"shELL"		,c_shell
	,"showKEYS"		,c_showkeys
	,"siZE"			,c_size
	,"smALL"		,c_small
	,"spECIAL"		,c_special
	,"stack"		,c_stack
/*
	,"stackcNTPAGE"		,c_stcntpg
	,"stackdES"		,c_stdes
	,"stackpAGE"		,c_stpg
*/
	,"stoRE"		,c_store
	,"suBST"		,c_subst
	,"swITCH"		,c_switch
	,"tiME"			,c_proc_time
	,"traNSL"		,c_transl
	,"truNCATION"		,c_trunc
	,"unLOAD"		,c_os_unload
	,"usER"			,c_user
	,"vaRFORMAT"		,c_varform
	,"versION"		,c_version
	,"vertiCAL"		,c_dvert
	,"vert1"		,c_dvert1
	,"vert2"		,c_dvert2
	,"wIDTH"		,c_width
	};

#define CMDTBLEN        (sizeof(cmdtb) / sizeof(struct cmnd))
#else
extern struct cmnd cmdtb[];
#endif
extern int initialize,Changes;
extern STACKELEM *Bottom;
extern T_UEBERGABE Parms;
/* end of      ccommand.h */

