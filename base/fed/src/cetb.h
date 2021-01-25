/*                                     cetb.h   */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/

/* ------------------ Ausdruckstypen ------------------------ */
/* Ausdruckstypen bestehen aus einem 2-elementigen Feld,      */
/* dessen Elemente bit-felder sind (32 bit).                  */
/* Damit wird die Syntax der Sprache (u.a.) definiert.        */
/* Die B_xxx sind die Typen jedes syntaktischen Elements,     */
/* die E_xxx sind diejenigen Typen, auf die das Element       */
/* 'passt'. (L_xxx sind die Typen als Listen). In der Tabelle */
/* etptb (s. etab.c) sind diese zusammengefasst und mit       */
/* Bezeichnungen versehen, die bei der Eingabe angezeigt      */
/* werden. Fuer jeden Index gibt es einen Makro (Konstante)   */
/* n_xxx. Mit diesen Makros werden die Elemente referenziert  */
/* und sie sind als Editinformation (6 bit) u.a. in dem Edit- */
/* feld abgelegt (also 0-2^6-1 = 63d = 077 (oktal)).          */
/* Bei Veraenderungen der Tabelle sind in etab.c die          */
/* Backtracking-Tabellen entsprechend zu aendern.             */
/* Ausserdem ist auf die korrekte Verwendung der Makros zu    */
/* achten. Die Schreibweise wurde in ..l und ..h umgestellt.  */
/* Zusammensetzen durch pair(..l,..h) !                       */
/* ---------------------------------------------------------- */

#define pair(l,h) l,h     /* zusammensetzen der bit-felder */

/* expression-flags     */
#define Bl_EXPR	0x00000001L
#define Bh_EXPR	0L
#define Bl_VAR	0x00000002L
#define Bh_VAR	0L
#define Bl_LBAR	0x00000004L
#define Bh_LBAR	0L
#define Bl_SETEL	0x00000008L
#define Bh_SETEL	0L
#define Bl_NUMBVAL	0x00000010L
#define Bh_NUMBVAL	0L
#define Bl_BOOLVAL	0x00000020L
#define Bh_BOOLVAL	0L
#define Bl_STRVAL	0x00000040L
#define Bh_STRVAL	0L
#define Bl_NMATEL	0x00000080L
#define Bh_NMATEL	0L
#define Bl_BMATEL	0x00000100L
#define Bh_BMATEL	0L
#define Bl_SMATEL	0x00000200L
#define Bh_SMATEL	0L
#define Bl_COND	0x00000400L
#define Bh_COND	0L
#define Bl_SETOBJ	0x00000800L
#define Bh_SETOBJ	0L
#define Bl_FMATEL	0x00001000L
#define Bh_FMATEL	0L
#define Bl_FMATRL	0x00002000L
#define Bh_FMATRL	0L
#define Bl_NMATRL	0x00004000L
#define Bh_NMATRL	0L
#define Bl_BMATRL	0x00008000L
#define Bh_BMATRL	0L
#define Bl_SMATRL	0x00010000L
#define Bh_SMATRL	0L

#define Bl_LST	0x00020000L
#define Bh_LST	0L

#define Bl_FDEC	0x00040000L
#define Bh_FDEC	0L
#define Bl_DECL	0x00080000L
#define Bh_DECL	0L
#define Bl_PRED	0x00100000L
#define Bh_PRED	0L

#define Bl_NOPAR	(Bl_EXPR | Bl_VAR )       /* leere Parameterliste */
#define Bh_NOPAR	(Bh_EXPR | Bh_VAR)
#define Bl_FDEX	(0x00200000L | Bl_EXPR | Bl_FDEC)
#define Bh_FDEX	0L
#define Bl_DCEX	(Bl_EXPR | Bl_DECL)
#define Bh_DCEX	(Bh_EXPR | Bh_DECL)
#define Bl_CALL	Bl_EXPR
#define Bh_CALL	Bh_EXPR
#define Bl_FNAM	Bl_VAR
#define Bh_FNAM	Bh_VAR
#define Bl_LNAM	Bl_VAR
#define Bh_LNAM	Bh_VAR

#define Bl_FCN	0x00200000L
#define Bh_FCN	0L

#define Bl_WHEN	0x00400000L
#define Bh_WHEN	0L
#define Bl_OTHER	0x00800000L
#define Bh_OTHER	0L
#define Bl_N_PATT	0x01000000L
#define Bh_N_PATT	0L
#define Bl_GUARD	0x02000000L
#define Bh_GUARD	0L
#define Bl_DO	0x04000000L
#define Bh_DO	0L
#define Bl_USKON	Bl_EXPR
#define Bh_USKON	Bh_EXPR
#define Bl_STRPATT	0x08000000L
#define Bh_STRPATT	0L
#define Bl_CHAR	0x10000000L
#define Bh_CHAR	0L
#define Bl_PATTERN	0x20000000L
#define Bh_PATTERN	0L
#define Bl_PATT2	0x40000000L
#define Bh_PATT2	0L
#define Bl_PATT1	0L
#define Bh_PATT1	0x00000001L
#define Bl_SKIP	0L
#define Bh_SKIP	0x00000002L
#define Bl_USKONP	(Bl_PATT1 | Bl_PATTERN)
#define Bh_USKONP	(Bh_PATT1 | Bh_PATTERN)
#define Bl_LIST	0L
#define Bh_LIST	0x00000004L
#define Bl_LDEC	0L
#define Bh_LDEC	0x00000008L
#if LET_P
#define Bl_PDEC	0L
#define Bh_PDEC	0x00000010L
#define Bl_PNAM	Bl_PATTERN
#define Bh_PNAM	Bh_PATTERN
#endif
#if ClausR
#define Bl_WHEN_CR	0L
#define Bh_WHEN_CR	0x00000020L
#define Bl_WHENST_CR	0L
#define Bh_WHENST_CR	0x00000040L
#endif /* ClausR	*/

#define Bl_VARSEL	(0L|Bl_VAR)
#define Bh_VARSEL	(0x00000080L|Bh_VAR)

#define Bl_PLACEH	0xffffffffL
#define Bh_PLACEH	0xffffffffL

/* list-expressions     */
#define Ll_EXPR	(Bl_EXPR | Bl_LST)
#define Lh_EXPR	(Bh_EXPR | Bh_LST)
#define Ll_VAR	(Bl_VAR | Bl_LST)
#define Lh_VAR	(Bh_VAR | Bh_LST)
#define Ll_SETEL	(Bl_SETEL | Bl_LST)
#define Lh_SETEL	(Bh_SETEL | Bh_LST)
#define Ll_NUMBVAL	(Bl_NUMBVAL | Bl_LST)
#define Lh_NUMBVAL	(Bh_NUMBVAL | Bh_LST)
#define Ll_BOOLVAL	(Bl_BOOLVAL | Bl_LST)
#define Lh_BOOLVAL	(Bh_BOOLVAL | Bh_LST)
#define Ll_STRVAL	(Bl_STRVAL | Bl_LST)
#define Lh_STRVAL	(Bh_STRVAL | Bh_LST)
#define Ll_NMATRL	(Bl_NMATRL | Bl_LST)
#define Lh_NMATRL	(Bh_NMATRL | Bh_LST)
#define Ll_BMATRL	(Bl_BMATRL | Bl_LST)
#define Lh_BMATRL	(Bh_BMATRL | Bh_LST)
#define Ll_SMATRL	(Bl_SMATRL | Bl_LST)
#define Lh_SMATRL	(Bh_SMATRL | Bh_LST)
#define Ll_NMATEL	(Bl_NMATEL | Bl_LST)
#define Lh_NMATEL	(Bh_NMATEL | Bh_LST)
#define Ll_BMATEL	(Bl_BMATEL | Bl_LST)
#define Lh_BMATEL	(Bh_BMATEL | Bh_LST)
#define Ll_SMATEL	(Bl_SMATEL | Bl_LST)
#define Lh_SMATEL	(Bh_SMATEL | Bh_LST)
#define Ll_FMATEL	(Bl_FMATEL | Bl_LST)
#define Lh_FMATEL	(Bh_FMATEL | Bh_LST)
#define Ll_FMATRL	(Bl_FMATRL | Bl_LST)
#define Lh_FMATRL	(Bh_FMATRL | Bh_LST)
#define Ll_FDEC	(Bl_FDEC | Bl_LST)
#define Lh_FDEC	(Bh_FDEC | Bh_LST)
#define Ll_WHEN	(Bl_WHEN | Bl_LST)
#define Lh_WHEN	(Bh_WHEN | Bh_LST)
#define Ll_PATT2	(Bl_PATT2 | Bl_LST)
#define Lh_PATT2	(Bh_PATT2 | Bh_LST)
#define Ll_PATT1	(Bl_PATT1 | Bl_LST)
#define Lh_PATT1	(Bh_PATT1 | Bh_LST)
#define Ll_STRPATT	(Bl_STRPATT | Bl_LST)
#define Lh_STRPATT	(Bh_STRPATT | Bh_LST)
#define Ll_CHAR	(Bl_CHAR    | Bl_LST)
#define Lh_CHAR	(Bh_CHAR    | Bh_LST)
#define Ll_LDEC	(Bl_LDEC | Bl_LST)
#define Lh_LDEC	(Bh_LDEC | Bh_LST)
#define Ll_PDEC	(Bl_PDEC | Bl_LST)
#define Lh_PDEC	(Bh_PDEC | Bh_LST)
#if ClausR
#define Ll_WHEN_CR	(Bl_WHEN_CR | Bl_LST)
#define Lh_WHEN_CR	(Bh_WHEN_CR | Bh_LST)
#define Ll_WHENST_CR	(Bl_WHENST_CR | Bl_LST)
#define Lh_WHENST_CR	(Bh_WHENST_CR | Bh_LST)
#endif /* ClausR	*/

/* expression-masken */
#define El_EXPR	(Bl_EXPR | Bl_DECL) /*  | Bl_FDEX)	*/
#define Eh_EXPR	(Bh_EXPR | Bh_DECL) /*  | Bh_FDEX)	*/
#define El_COND	(Bl_EXPR | Bl_COND)
#define Eh_COND	(Bh_EXPR | Bh_COND)
#define El_VAR	(Bl_EXPR | Bl_VAR | Bl_LBAR | Bl_FCN | Bl_PATTERN | Bl_PATT1 | Bl_VARSEL)
#define Eh_VAR	(Bh_EXPR | Bh_VAR | Bh_LBAR | Bh_FCN | Bh_PATTERN | Bh_PATT1 | Bl_VARSEL)
#define El_LBAR	(Bl_EXPR | Bl_LBAR | Bl_FCN)
#define Eh_LBAR	(Bh_EXPR | Bh_LBAR | Bh_FCN)
#define El_SCAL	(Bl_EXPR | Bl_SETEL)
#define Eh_SCAL	(Bh_EXPR | Bh_SETEL)
#define El_VEC	(Bl_EXPR | Bl_SETEL)
#define Eh_VEC	(Bh_EXPR | Bh_SETEL)
#define El_MAT	(Bl_EXPR | Bl_SETEL | Bl_FMATEL | Bl_PATTERN | Bl_PATT1)
#define Eh_MAT	(Bh_EXPR | Bh_SETEL | Bh_FMATEL | Bh_PATTERN | Bh_PATT1)
#define El_VECT	(Bl_EXPR | Bl_SETEL | Bl_PATTERN | Bl_PATT1)
#define Eh_VECT	(Bh_EXPR | Bh_SETEL | Bh_PATTERN | Bh_PATT1)
#define El_SET	(Bl_EXPR | Bl_SETEL)
#define Eh_SET	(Bh_EXPR | Bh_SETEL)
#define El_SETOBJ	(Bl_EXPR | Bl_SETOBJ)
#define Eh_SETOBJ	(Bh_EXPR | Bh_SETOBJ)
#define El_SETEL	(Bl_EXPR | Bl_SETEL)
#define Eh_SETEL	(Bh_EXPR | Bh_SETEL)

#define El_NUMBVAL	(Bl_EXPR | Bl_SETEL | Bl_NUMBVAL | Bl_NMATEL | Bl_PATTERN | Bl_PATT1)
#define Eh_NUMBVAL	(Bh_EXPR | Bh_SETEL | Bh_NUMBVAL | Bh_NMATEL | Bh_PATTERN | Bh_PATT1)
#define El_BOOLVAL	(Bl_EXPR | Bl_SETEL | Bl_BOOLVAL | Bl_BMATEL | Bl_PATTERN | Bl_PATT1)
#define Eh_BOOLVAL	(Bh_EXPR | Bh_SETEL | Bh_BOOLVAL | Bh_BMATEL | Bh_PATTERN | Bh_PATT1)
/*
#define El_STRVAL	(Bl_EXPR | Bl_SETEL | Bl_STRVAL | Bl_SMATEL | Bl_PATTERN | Bl_PATT1 | Bl_CHAR)
#define Eh_STRVAL	(Bh_EXPR | Bh_SETEL | Bh_STRVAL | Bh_SMATEL | Bh_PATTERN | Bh_PATT1 | Bh_CHAR)
*/
#define El_STRVAL	(Bl_EXPR | Bl_SETEL | Bl_STRVAL | Bl_SMATEL | Bl_CHAR)
#define Eh_STRVAL	(Bh_EXPR | Bh_SETEL | Bh_STRVAL | Bh_SMATEL | Bh_CHAR)

#define El_NMATEL	(Bl_EXPR | Bl_NMATEL )
#define Eh_NMATEL	(Bh_EXPR | Bh_NMATEL )
#define El_BMATEL	(Bl_EXPR | Bl_BMATEL)
#define Eh_BMATEL	(Bh_EXPR | Bh_BMATEL)
#define El_SMATEL	(Bl_EXPR | Bl_SMATEL)
#define Eh_SMATEL	(Bh_EXPR | Bh_SMATEL)

#define El_NMATRL	(Bl_EXPR | Bl_NMATRL | Bl_FMATRL)
#define Eh_NMATRL	(Bh_EXPR | Bh_NMATRL | Bh_FMATRL)
#define El_BMATRL	(Bl_EXPR | Bl_BMATRL | Bl_FMATRL)
#define Eh_BMATRL	(Bh_EXPR | Bh_BMATRL | Bh_FMATRL)
#define El_SMATRL	(Bl_EXPR | Bl_SMATRL | Bl_FMATRL)
#define Eh_SMATRL	(Bh_EXPR | Bh_SMATRL | Bh_FMATRL)

#define El_FMATEL	(Bl_EXPR | Bl_FMATEL | Bl_PATTERN | Bl_PATT1)
#define Eh_FMATEL	(Bh_EXPR | Bh_FMATEL | Bh_PATTERN | Bh_PATT1)
#define El_FMATRL	(Bl_EXPR | Bl_FMATRL)
#define Eh_FMATRL	(Bh_EXPR | Bh_FMATRL)

#define El_PLACEH	Bl_PLACEH
#define Eh_PLACEH	Bh_PLACEH
#define El_NOPAR	(Bl_NOPAR | Bl_PATTERN | Bl_PATT1)
#define Eh_NOPAR	(Bh_NOPAR| Bh_PATTERN | Bh_PATT1)
#define El_ATOM	(Bl_EXPR | Bl_PATTERN | Bl_PATT1)
#define Eh_ATOM	(Bh_EXPR | Bh_PATTERN | Bh_PATT1)
#define El_TYPE	(Bl_EXPR | Bl_PATTERN | Bl_PATT1)
#define Eh_TYPE	(Bh_EXPR | Bh_PATTERN | Bh_PATT1)

#define El_FUNC	(Bl_EXPR | Bl_FCN | Bl_PRED | Bl_PATTERN | Bl_PATT1)
#define Eh_FUNC	(Bh_EXPR | Bh_FCN | Bh_PRED | Bh_PATTERN | Bh_PATT1)

#define El_FDEC	Bl_FDEC
#define Eh_FDEC	Bh_FDEC
#if DORSY
#define El_DECL	(Bl_DECL | Bl_EXPR)
#define Eh_DECL	(Bh_DECL | Bh_EXPR)
#define El_DCEX	(Bl_DCEX | Bl_EXPR)
#define Eh_DCEX	(Bh_DCEX | Bh_EXPR)
#else
#define El_DECL	Bl_DECL
#define Eh_DECL	Bh_DECL
#define El_DCEX	Bl_DCEX
#define Eh_DCEX	Bh_DCEX
#endif
#define El_FDEX	Bl_FDEX
#define Eh_FDEX	Bh_FDEX
#define El_CALL	Bl_CALL
#define Eh_CALL	Bh_CALL
#define El_FNAM	(Bl_FNAM | Bl_FDEC)
#define Eh_FNAM	(Bh_FNAM | Bh_FDEC)
#define El_FCN	(Bl_EXPR | Bl_VAR | Bl_LBAR | Bl_PRED)
#define Eh_FCN	(Bh_EXPR | Bh_VAR | Bh_LBAR | Bh_PRED)

#define El_CONST  (Bl_NUMBVAL | Bl_BOOLVAL | Bl_STRVAL | Bl_FMATEL)
#define Eh_CONST  (Bh_NUMBVAL | Bh_BOOLVAL | Bh_STRVAL | Bh_FMATEL)

#define El_WHEN	(El_EXPR|Bl_WHEN)
#define Eh_WHEN	(Eh_EXPR|Bh_WHEN)
#define El_PATT2	(El_EXPR|Bl_PATT2)
#define Eh_PATT2	(Eh_EXPR|Bh_PATT2)
#define El_OTHER	Bl_OTHER
#define Eh_OTHER	Bh_OTHER
#define El_PATTERN	Bl_PATTERN
#define Eh_PATTERN	Bh_PATTERN
#define El_N_PATT	(Bl_N_PATT | Bl_PATTERN)
#define Eh_N_PATT	(Bh_N_PATT | Bh_PATTERN)
#define El_WH_PA	(El_PATTERN|El_WHEN)
#define Eh_WH_PA	(Eh_PATTERN|Eh_WHEN)
#define El_GUARD	Bl_GUARD
#define Eh_GUARD	Bh_GUARD
#define El_DO	(Bl_GUARD | Bl_DO)
#define Eh_DO	(Bh_GUARD | Bh_DO)
#define El_USKON	Bl_USKON
#define Eh_USKON	Bh_USKON
#define El_USKONP	Bl_USKONP
#define Eh_USKONP	Bh_USKONP
#define El_STRPATT	(Bl_STRPATT | Bl_PATTERN | Bl_PATT1)
#define Eh_STRPATT	(Bh_STRPATT | Bh_PATTERN | Bh_PATT1)
#define El_CHAR	(Bl_CHAR | Bl_EXPR | Bl_STRPATT)
#define Eh_CHAR	(Bh_CHAR | Bh_EXPR | Bl_STRPATT)
#define El_PATT1	(Bl_PATT1 | Bl_PATTERN)
#define Eh_PATT1	(Bh_PATT1 | Bh_PATTERN)
#define El_SKIP		Bl_SKIP
#define Eh_SKIP		Bh_SKIP
#define El_LIST	Bl_LIST
#define Eh_LIST	Bh_LIST
#define El_LDEC	Bl_LDEC
#define Eh_LDEC	Bh_LDEC
#define El_LNAM	(Bl_LNAM | Bl_LDEC)
#define Eh_LNAM	(Bh_LNAM | Bh_LDEC)
#define El_PDEC	Bl_PDEC
#define Eh_PDEC	Bh_PDEC
#define El_PNAM	(Bl_PNAM | Bl_PDEC)
#define Eh_PNAM	(Bh_PNAM | Bh_PDEC)
#if ClausR
#define El_WHEN_CR	(El_EXPR|Bl_WHEN_CR)
#define Eh_WHEN_CR	(Eh_EXPR|Bh_WHEN_CR)
#define El_WHENST_CR	(El_EXPR|Bl_WHENST_CR)
#define Eh_WHENST_CR	(Eh_EXPR|Bh_WHENST_CR)
#endif /* ClausR	*/

#define El_VARSEL	(El_EXPR|Bl_VAR|Bl_VARSEL)
#define Eh_VARSEL	(Eh_EXPR|Bh_VAR|Bh_VARSEL)


#define E_EXPR	pair(El_EXPR,Eh_EXPR)
#define E_COND	pair(El_COND,Eh_COND)
#define E_VAR	pair(El_VAR,Eh_VAR)
#define E_LBAR	pair(El_LBAR,Eh_LBAR)
#define E_SCAL	pair(El_SCAL,Eh_SCAL)
#define E_VEC	pair(El_VEC,Eh_VEC)
#define E_MAT	pair(El_MAT,Eh_MAT)
#define E_VECT	pair(El_VECT,Eh_VECT)
#define E_NVECOBJ	pair(El_VECT,Eh_VECT)
#define E_BVECOBJ	pair(El_VECT,Eh_VECT)
#define E_SVECOBJ	pair(El_VECT,Eh_VECT)
#define E_SET	pair(El_SET,Eh_SET)
#define E_SETOBJ	pair(El_SETOBJ,Eh_SETOBJ)
#define E_SETEL	pair(El_SETEL,Eh_SETEL)

#define E_NUMBVAL	pair(El_NUMBVAL,Eh_NUMBVAL)
#define E_BOOLVAL	pair(El_BOOLVAL,Eh_BOOLVAL)
#define E_STRVAL	pair(El_STRVAL,Eh_STRVAL)
#define E_NMATEL	pair(El_NMATEL,Eh_NMATEL)
#define E_BMATEL	pair(El_BMATEL,Eh_BMATEL)
#define E_SMATEL	pair(El_SMATEL,Eh_SMATEL)

#define E_NMATRL	pair(El_NMATRL,Eh_NMATRL)
#define E_BMATRL	pair(El_BMATRL,Eh_BMATRL)
#define E_SMATRL	pair(El_SMATRL,Eh_SMATRL)

#define E_FMATEL	pair(El_FMATEL,Eh_FMATEL)
#define E_FMATRL	pair(El_FMATRL,Eh_FMATRL)

#define E_PLACEH	pair(El_PLACEH,Eh_PLACEH)
#define E_NOPAR	pair(El_NOPAR,Eh_NOPAR)
#define E_ATOM	pair(El_ATOM,Eh_ATOM)
#define E_TYPE	pair(El_TYPE,Eh_TYPE)

#define E_FUNC	pair(El_FUNC,Eh_FUNC)

#define E_FDEC	pair(El_FDEC,Eh_FDEC)
#define E_DECL	pair(El_DECL,Eh_DECL)
#define E_FDEX	pair(El_FDEX,Eh_FDEX)
#define E_DCEX	pair(El_DCEX,Eh_DCEX)
#define E_CALL	pair(El_CALL,Eh_CALL)
#define E_FNAM	pair(El_FNAM,Eh_FNAM)
#define E_FCN	pair(El_FCN,Eh_FCN)

#define E_CONST	pair(El_CONST,Eh_CONST)

#define E_WHEN	pair(El_WHEN,Eh_WHEN)
#define E_PATT2	pair(El_PATT2,Eh_PATT2)
#define E_OTHER	pair(El_OTHER,Eh_OTHER)
#define E_PATTERN	pair(El_PATTERN,Eh_PATTERN)
#define E_N_PATT	pair(El_N_PATT,Eh_N_PATT)
#define E_WH_PA	pair(El_WH_PA,Eh_WH_PA)
#define E_GUARD	pair(El_GUARD,Eh_GUARD)
#define E_DO	pair(El_DO,Eh_DO)
#define E_USKON	pair(El_USKON,Eh_USKON)
#define E_USKONP	pair(El_USKONP,Eh_USKONP)
#define E_STRPATT	pair(El_STRPATT,Eh_STRPATT)
#define E_CHAR	pair(El_CHAR,Eh_CHAR)
#define E_PATT1	pair(El_PATT1,Eh_PATT1)
#define E_SKIP	pair(El_SKIP,Eh_SKIP)
#define E_LIST	pair(El_LIST,Eh_LIST)
#define E_LDEC	pair(El_LDEC,Eh_LDEC)
#define E_LNAM	pair(El_LNAM,Eh_LNAM)
#define E_PDEC	pair(El_PDEC,Eh_PDEC)
#define E_PNAM	pair(El_PNAM,Eh_PNAM)
#if ClausR
#define E_WHEN_CR	pair(El_WHEN_CR,Eh_WHEN_CR)
#define E_WHENST_CR	pair(El_WHENST_CR,Eh_WHENST_CR)
#endif /* ClausR	*/

#define E_VARSEL	pair(El_VARSEL,Eh_VARSEL)

#define B_EXPR	pair(Bl_EXPR,Bh_EXPR)
#define B_COND	pair(Bl_COND,Bh_COND)
#define B_VAR	pair(Bl_VAR,Bh_VAR)
#define B_LBAR	pair(Bl_LBAR,Bh_LBAR)
#define B_SCAL	pair(Bl_SCAL,Bh_SCAL)
#define B_VEC	pair(Bl_VEC,Bh_VEC)
#define B_MAT	pair(Bl_MAT,Bh_MAT)
#define B_SET	pair(Bl_SET,Bh_SET)
#define B_SETOBJ	pair(Bl_SETOBJ,Bh_SETOBJ)
#define B_SETEL	pair(Bl_SETEL,Bh_SETEL)

#define B_NUMBVAL	pair(Bl_NUMBVAL,Bh_NUMBVAL)
#define B_BOOLVAL	pair(Bl_BOOLVAL,Bh_BOOLVAL)
#define B_STRVAL	pair(Bl_STRVAL,Bh_STRVAL)
#define B_NMATEL	pair(Bl_NMATEL,Bh_NMATEL)
#define B_BMATEL	pair(Bl_BMATEL,Bh_BMATEL)
#define B_SMATEL	pair(Bl_SMATEL,Bh_SMATEL)

#define B_NMATRL	pair(Bl_NMATRL,Bh_NMATRL)
#define B_BMATRL	pair(Bl_BMATRL,Bh_BMATRL)
#define B_SMATRL	pair(Bl_SMATRL,Bh_SMATRL)

#define B_FMATEL	pair(Bl_FMATEL,Bh_FMATEL)
#define B_FMATRL	pair(Bl_FMATRL,Bh_FMATRL)

#define B_PLACEH	pair(Bl_PLACEH,Bh_PLACEH)
#define B_NOPAR	pair(Bl_NOPAR,Bh_NOPAR)
#define B_ATOM	pair(Bl_ATOM,Bh_ATOM)

#define B_FUNC	pair(Bl_FUNC,Bh_FUNC)

#define B_FDEC	pair(Bl_FDEC,Bh_FDEC)
#define B_DECL	pair(Bl_DECL,Bh_DECL)
#define B_FDEX	pair(Bl_FDEX,Bh_FDEX)
#define B_DCEX	pair(Bl_DCEX,Bh_DCEX)
#define B_CALL	pair(Bl_CALL,Bh_CALL)
#define B_FNAM	pair(Bl_FNAM,Bh_FNAM)
#define B_FCN	pair(Bl_FCN,Bh_FCN)

#define B_CONST	pair(Bl_CONST,Bh_CONST)

#define B_WHEN	pair(Bl_WHEN,Bh_WHEN)
#define B_PATT2	pair(Bl_PATT2,Bh_PATT2)
#define B_OTHER	pair(Bl_OTHER,Bh_OTHER)
#define B_PATTERN	pair(Bl_PATTERN,Bh_PATTERN)
#define B_N_PATT	pair(Bl_N_PATT,Bh_N_PATT)
#define B_GUARD	pair(Bl_GUARD,Bh_GUARD)
#define B_DO	pair(Bl_DO,Bh_DO)
#define B_USKON	pair(Bl_USKON,Bh_USKON)
#define B_USKONP	pair(Bl_USKONP,Bh_USKONP)
#define B_STRPATT	pair(Bl_STRPATT,Bh_STRPATT)
#define B_CHAR	pair(Bl_CHAR,Bh_CHAR)
#define B_PATT1	pair(Bl_PATT1,Bh_PATT1)
#define B_SKIP	pair(Bl_SKIP,Bh_SKIP)
#define B_LIST	pair(Bl_LIST,Bh_LIST)
#define B_LDEC	pair(Bl_LDEC,Bh_LDEC)
#define B_LNAM	pair(Bl_LNAM,Bh_LNAM)
#define B_PDEC	pair(Bl_PDEC,Bh_PDEC)
#define B_PNAM	pair(Bl_PNAM,Bh_PNAM)
#if ClausR
#define B_WHEN_CR	pair(Bl_WHEN_CR,Bh_WHEN_CR)
#define B_WHENST_CR	pair(Bl_WHENST_CR,Bh_WHENST_CR)
#endif /* ClausR	*/

#define B_VARSEL	pair(Bl_VARSEL,Bh_VARSEL)

#define L_EXPR	pair(Ll_EXPR,Lh_EXPR)
#define L_COND	pair(Ll_COND,Lh_COND)
#define L_VAR	pair(Ll_VAR,Lh_VAR)
#define L_LBAR	pair(Ll_LBAR,Lh_LBAR)
#define L_SCAL	pair(Ll_SCAL,Lh_SCAL)
#define L_VEC	pair(Ll_VEC,Lh_VEC)
#define L_MAT	pair(Ll_MAT,Lh_MAT)
#define L_SET	pair(Ll_SET,Lh_SET)
#define L_SETOBJ	pair(Ll_SETOBJ,Lh_SETOBJ)
#define L_SETEL	pair(Ll_SETEL,Lh_SETEL)

#define L_NUMBVAL	pair(Ll_NUMBVAL,Lh_NUMBVAL)
#define L_BOOLVAL	pair(Ll_BOOLVAL,Lh_BOOLVAL)
#define L_STRVAL	pair(Ll_STRVAL,Lh_STRVAL)
#define L_NMATEL	pair(Ll_NMATEL,Lh_NMATEL)
#define L_BMATEL	pair(Ll_BMATEL,Lh_BMATEL)
#define L_SMATEL	pair(Ll_SMATEL,Lh_SMATEL)

#define L_NMATRL	pair(Ll_NMATRL,Lh_NMATRL)
#define L_BMATRL	pair(Ll_BMATRL,Lh_BMATRL)
#define L_SMATRL	pair(Ll_SMATRL,Lh_SMATRL)

#define L_FMATEL	pair(Ll_FMATEL,Lh_FMATEL)
#define L_FMATRL	pair(Ll_FMATRL,Lh_FMATRL)

#define L_PLACEH	pair(Ll_PLACEH,Lh_PLACEH)

#define L_FDEC	pair(Ll_FDEC,Lh_FDEC)

#define L_CONST	pair(Ll_CONST,Lh_CONST)

#define L_WHEN	pair(Ll_WHEN,Lh_WHEN)
#define L_PATT2	pair(Ll_PATT2,Lh_PATT2)
#define L_OTHER	pair(Ll_OTHER,Lh_OTHER)
#define L_PATT1	pair(Ll_PATT1,Lh_PATT1)
#define L_GUARD	pair(Ll_GUARD,Lh_GUARD)
#define L_DO	pair(Ll_DO,Lh_DO)
#define L_STRPATT	pair(Ll_STRPATT,Lh_STRPATT)
#define L_CHAR	pair(Ll_CHAR,Lh_CHAR)
#define L_LDEC	pair(Ll_LDEC,Lh_LDEC)
#define L_PDEC	pair(Ll_PDEC,Lh_PDEC)
#if ClausR
#define L_WHEN_CR	pair(Ll_WHEN_CR,Lh_WHEN_CR)
#define L_WHENST_CR	pair(Ll_WHENST_CR,Lh_WHENST_CR)
#endif /* ClausR	*/

/* expression-nummern */
#define n_expr	0
#define n_exprlst	01
#define n_var	02
#define n_varlst	03
#define n_cond	04
#define n_lbar	05
#define n_setel	06
#define n_setellst	07
#define n_nval	010
#define n_nvallst	011
#define n_bval	012
#define n_bvallst	013
#define n_sval	014
#define n_svallst	015
#define n_setobj	016
#define n_nmatrl	017
#define n_bmatrl	020
#define n_smatrl	021
#define n_nmatels	022
#define n_bmatels	023
#define n_smatels	024
#define n_fmatels	025
#define n_fmatrl	026
#define n_fdec	027
#define n_fdeclst	030
#define n_decl	031
#define n_fnam	032
#define n_fdex	033
#define n_call	034
#define n_fcn	035
#define n_dcex	036
#define n_when	037
#define n_whenlst	040
#define n_other	041
#define n_pattern	042
#define n_pattlst	043
#define n_guard	044
#define n_do	045
#define n_uskon	046
#define n_uskonp	047
#define n_strpatt	050
#define n_strpl	051
#define n_char	052
#define n_str	053
#define n_n_patt	054
#define n_patt2	055
#define n_patt2lst	056
#define n_patt1	057
#define n_skip	060
#define n_list	061
#define n_ldec	062
#define n_ldlst	063
#define n_lnam	064
#if LET_P
#define n_pdec	065
#define n_pdlst	066
#define n_pnam	067
#endif
#if ClausR
#define n_when_cr	070
#define n_whenlst_cr	071
#define n_whenst_cr	072
#define n_whenlstst_cr	073
#endif /* ClausR	*/
/* 077 ist letzte nummer !	*/

/* makros zur identifikation    */
#define ismatrl(enr) (((n_nmatrl <= enr) && (enr <= n_smatrl)) || (enr == n_fmatrl))
#define ismatel(enr) (((n_nmatels <= enr) && (enr <= n_smatels)) || (enr == n_fmatels))

#define isexpr(e)	(((e[0]) & Bl_EXPR) != 0L || ((e[1]) & Bh_EXPR) != 0L)
#define islist(e)	(((e[0]) & Bl_LST) != 0L || ((e[1]) & Bh_LST) != 0L)
#define is_e_list(e)	(((e[0]) & (Bl_EXPR | Bl_LST)) == (Bl_EXPR | Bl_LST) && \
	((e[1]) & (Bh_EXPR | Bh_LST)) == (Bh_EXPR | Bh_LST))
#define isvar(e)	(((e[0]) & El_VAR) != 0L || ((e[1]) & Eh_VAR) != 0L)
#define isnumbval(e)	(((e[0]) & (El_NUMBVAL | El_NMATEL)) != 0L || \
	((e[1]) & (Eh_NUMBVAL | Eh_NMATEL)) != 0L)
#define isstrval(e)	(((e[0]) & (Bl_STRVAL | Bl_SMATEL)) != 0L || \
	((e[1]) & (Bh_STRVAL | Bh_SMATEL)) != 0L)
#define isstr(e)	(((e[0]) & (Bl_CHAR | Bl_STRPATT)) != 0L || \
	((e[1]) & (Bh_CHAR | Bh_STRPATT)) != 0L)
#if LET_P
#define IS_DECN(eno)	((eno) == n_fnam || (eno) == n_lnam || (eno) == n_pnam)
#define IS_DECL(eno)	((eno) == n_fdeclst || (eno) == n_ldlst || (eno) == n_pdlst)
#else
#define IS_DECN(eno)	((eno) == n_fnam || (eno) == n_lnam)
#define IS_DECL(eno)	((eno) == n_fdeclst || (eno) == n_ldlst)
#endif

#define iskomp(a,b)	(((a[0])& (b[0]))!= 0L || ((a[1])& (b[1]))!= 0L)
/* vorsicht !   */
#define assex(e1,e2)    { e1[0] = e2[0]; e1[1] = e2[1]; }

#define start_eno	(isapvers ? n_expr : n_dcex)

struct etps {
	EXTYP exptp,eetp;
	char *tmsg;
 };
#ifndef isetptb
extern struct etps etptb[];
#endif

/* end of      cetb.h */

