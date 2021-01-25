double t_v_t_ip_real(T_PTD, T_PTD, int);
double t_tvr_vc_max(T_PTD, int, int);
double t_tvr_vc_plus(T_PTD, int, int);
double t_tvr_vc_mult(T_PTD, int, int);
double t_tvr_vc_minus(T_PTD, int, int);
double t_tvr_vc_div(T_PTD, int, int);
double t_tv_vc_div(T_PTD, int, int);

extern PTR_DESCRIPTOR _desc;
int controlled_exit();

#define push_and_kill(term,kw,ka)  t = ((int)(term));  \
                                   freea((ka));       \
                                   freerfw((kw));       \
                                   push((int)t)
#define push_and_no_w_kill(term, kw, ka) push((int)(term));  \
                                         freea((ka))

#define push_and_kill_atom(term,kw,ka)  t = ((int)(term));  \
                                        freea((ka));       \
                                        drop(w, (kw));       \
                                        push((int)t)
#define push_and_no_w_kill_atom(term, kw, ka) push((int)(term));  \
                                              freea((ka))
#define keep_and_kill_atom(term, kw, ka) _desc = (PTR_DESCRIPTOR)(term)

#define keep_and_no_w_kill_atom(term, kw, ka) _desc = (PTR_DESCRIPTOR)(term)

#define keep_and_no_w_kill(term, kw, ka) _desc = (PTR_DESCRIPTOR)(term)

#define keep_and_kill(term, kw, ka) _desc = (PTR_DESCRIPTOR)(term)
                                            
/*************/                                         
/* ARITY = 1 */
/*************/                                         
#define b_TA_NOT(a)             ((SA_FALSE == (a)) ? SA_TRUE : SA_FALSE)
#define i_TA_ABS(a)             ((0 > VAL_INT(a)) ? (NEG_INT(a)) : (a))
#define r_TA_ABS(a)             ((0 > R_SCALAR(DESC(a), valr)) ?  \
                                (-(R_SCALAR(DESC(a),valr))) : R_SCALAR(DESC(a),valr))
#define i_TA_NEG(a)             (NEG_INT(a))
#define r_TA_NEG(a)             (-(R_SCALAR(DESC(a),valr)))

#define me_TA_NEG(a)            (INC_REFCNT(_nilmat), _nilmat)
#define ve_TA_NEG(a)            (INC_REFCNT(_nilvect), _nilvect)
#define te_TA_NEG(a)            (INC_REFCNT(_niltvect), _niltvect)

#define vi_TA_NEG(a)            mvt_int_neg(a)
#define vr_TA_NEG(a)            mvt_real_neg(a)
#define ti_TA_NEG(a)            mvt_int_neg(a)
#define tr_TA_NEG(a)            mvt_real_neg(a)
#define mi_TA_NEG(a)            mvt_int_neg(a)
#define mr_TA_NEG(a)            mvt_real_neg(a)
#define i_TA_TRUNCATE(a)        (a)
#define r_TA_TRUNCATE(a)        (TAG_INT((int)(R_SCALAR(DESC(a),valr))) )

#define me_TA_TRUNCATE(a)       (INC_REFCNT(_nilmat), _nilmat)
#define ve_TA_TRUNCATE(a)       (INC_REFCNT(_nilvect), _nilvect)
#define te_TA_TRUNCATE(a)       (INC_REFCNT(_niltvect), _niltvect)

#define vi_TA_TRUNCATE(a)       mvt_trunc((a), C_VECTOR, TY_INTEGER)
#define vr_TA_TRUNCATE(a)       mvt_trunc((a), C_VECTOR, TY_REAL)
#define ti_TA_TRUNCATE(a)       mvt_trunc((a), C_TVECTOR, TY_INTEGER)
#define tr_TA_TRUNCATE(a)       mvt_trunc((a), C_TVECTOR, TY_REAL)
#define mi_TA_TRUNCATE(a)       mvt_trunc((a), C_MATRIX, TY_INTEGER)
#define mr_TA_TRUNCATE(a)       mvt_trunc((a), C_MATRIX, TY_REAL)

#define i_TA_FRAC(a)            (ZERO_INT)
#define r_TA_FRAC(a)            (R_SCALAR(DESC(a),valr) - (int)R_SCALAR(DESC(a),valr))

#define me_TA_FRAC(a)           (INC_REFCNT(_nilmat), _nilmat)
#define ve_TA_FRAC(a)           (INC_REFCNT(_nilvect), _nilvect)
#define te_TA_FRAC(a)           (INC_REFCNT(_niltvect), _niltvect)

#define vi_TA_FRAC(a)           mvt_frac((a), C_VECTOR, TY_INTEGER);
#define vr_TA_FRAC(a)           mvt_frac((a), C_VECTOR, TY_REAL)
#define ti_TA_FRAC(a)           mvt_frac((a), C_TVECTOR, TY_INTEGER); 
#define tr_TA_FRAC(a)           mvt_frac((a), C_TVECTOR, TY_REAL);
#define mi_TA_FRAC(a)           mvt_frac((a), C_MATRIX, TY_INTEGER);
#define mr_TA_FRAC(a)           mvt_frac((a), C_MATRIX, TY_REAL);
#define l_TA_EMPTY(a)           ((0 == R_LIST(DESC(a), dim)) ? SA_TRUE : SA_FALSE)
#define s_TA_EMPTY(a)           ((0 == R_LIST(DESC(a), dim)) ? SA_TRUE : SA_FALSE)

#define me_TA_EMPTY(a)          SA_TRUE
#define ve_TA_EMPTY(a)          SA_TRUE
#define te_TA_EMPTY(a)          SA_TRUE

#define vi_TA_EMPTY(a)          ((TY_UNDEF == R_DESC(DESC(a),type)) ? SA_TRUE : SA_FALSE)
#define vr_TA_EMPTY(a)          ((TY_UNDEF == R_DESC(DESC(a),type)) ? SA_TRUE : SA_FALSE)
#define vb_TA_EMPTY(a)          ((TY_UNDEF == R_DESC(DESC(a),type)) ? SA_TRUE : SA_FALSE)
#define ti_TA_EMPTY(a)          ((TY_UNDEF == R_DESC(DESC(a),type)) ? SA_TRUE : SA_FALSE)
#define tr_TA_EMPTY(a)          ((TY_UNDEF == R_DESC(DESC(a),type)) ? SA_TRUE : SA_FALSE)
#define tb_TA_EMPTY(a)          ((TY_UNDEF == R_DESC(DESC(a),type)) ? SA_TRUE : SA_FALSE)
#define mi_TA_EMPTY(a)          ((TY_UNDEF == R_DESC(DESC(a),type)) ? SA_TRUE : SA_FALSE)
#define mr_TA_EMPTY(a)          ((TY_UNDEF == R_DESC(DESC(a),type)) ? SA_TRUE : SA_FALSE)
#define mb_TA_EMPTY(a)          ((TY_UNDEF == R_DESC(DESC(a),type)) ? SA_TRUE : SA_FALSE)

#define i_TA_TAN(a)             (tan(VAL_INT(a)))
#define r_TA_TAN(a)             (tan(R_SCALAR(DESC(a),valr)))
#define i_TA_SIN(a)             (sin(VAL_INT(a)))
#define r_TA_SIN(a)             (sin(R_SCALAR(DESC(a),valr)))
#define i_TA_COS(a)             (cos(VAL_INT(a)))
#define r_TA_COS(a)             (cos(R_SCALAR(DESC(a),valr)))

#define i_TA_CEIL(a)            (a)
#define r_TA_CEIL(a)            (TAG_INT((int)(( R_SCALAR(DESC(a),valr) <= 0.0)    \
                                ? R_SCALAR(DESC(a),valr) : R_SCALAR(DESC(a),valr) + 1 )))

#define ve_TA_CEIL(a)           (INC_REFCNT(_nilmat), _nilmat)
#define te_TA_CEIL(a)           (INC_REFCNT(_nilvect), _nilvect)
#define me_TA_CEIL(a)           (INC_REFCNT(_niltvect), _niltvect)

#define vi_TA_CEIL(a)           mvt_int_ceil(a)
#define vr_TA_CEIL(a)           mvt_real_ceil(a)
#define ti_TA_CEIL(a)           mvt_int_ceil(a)
#define tr_TA_CEIL(a)           mvt_real_ceil(a)
#define mi_TA_CEIL(a)           mvt_int_ceil(a)
#define mr_TA_CEIL(a)           mvt_real_ceil(a)

#define i_TA_FLOOR(a)           (a)
#define r_TA_FLOOR(a)           (TAG_INT(( R_SCALAR(DESC(a),valr) >= 0.0)    \
                                ? R_SCALAR(DESC(a),valr) : R_SCALAR(DESC(a),valr) - 1 ))

#define me_TA_FLOOR(a)          (INC_REFCNT(_nilmat), _nilmat)
#define ve_TA_FLOOR(a)          (INC_REFCNT(_nilvect), _nilvect)
#define te_TA_FLOOR(a)          (INC_REFCNT(_niltvect), _niltvect)

#define vi_TA_FLOOR(a)          mvt_int_floor(a)
#define vr_TA_FLOOR(a)          mvt_real_floor(a)
#define ti_TA_FLOOR(a)          mvt_int_floor(a)
#define vi_TA_FLOOR(a)          mvt_int_floor(a)
#define vr_TA_FLOOR(a)          mvt_real_floor(a)
#define ti_TA_FLOOR(a)          mvt_int_floor(a)
#define tr_TA_FLOOR(a)          mvt_real_floor(a)
#define mi_TA_FLOOR(a)          mvt_int_floor(a)
#define mr_TA_FLOOR(a)          mvt_real_floor(a)
#define i_TA_EXP(a)             my_nr_exp((a))
#define i_TA_LN(a)              my_nr_ln((a))
#define r_TA_EXP(a)             my_nr_exp((a))
#define r_TA_LN(a)              my_nr_ln((a))

/*************************************************/
/* LIST => INTEGER                               */
/*************************************************/
#define l_TA_LDIMENSION(a)      TAG_INT((R_LIST(DESC(a), dim)))

/*************************************************/
/* STRING => INTEGER                             */
/*************************************************/
#define s_TA_LDIMENSION(a)      TAG_INT((R_LIST(DESC(a), dim)))

/*************************************************/
/* (T)VECTOR => INTEGER                          */
/*************************************************/
#define ve_TA_VDIMENSION(a)      ZERO_INT
#define te_TA_VDIMENSION(a)      ZERO_INT

#define vi_TA_VDIMENSION(a)      TAG_INT(R_MVT(DESC(a), ncols, C_VECTOR))
#define vr_TA_VDIMENSION(a)      TAG_INT(R_MVT(DESC(a), ncols, C_VECTOR))
#define vb_TA_VDIMENSION(a)      TAG_INT(R_MVT(DESC(a), ncols, C_VECTOR))
#define ti_TA_VDIMENSION(a)      TAG_INT(R_MVT(DESC(a), nrows, C_TVECTOR))
#define tr_TA_VDIMENSION(a)      TAG_INT(R_MVT(DESC(a), nrows, C_TVECTOR))
#define tb_TA_VDIMENSION(a)      TAG_INT(R_MVT(DESC(a), nrows, C_TVECTOR))

/*************/                                         
/* ARITY = 2 */
/*************/                                         

/*************************************************/
/* BOOLEAN X BOOLEAN => BOOLEAN                  */
/*************************************************/
#define bxb_TA_OR(a, b)         ((T_SA_TRUE(a) || T_SA_TRUE(b)) ? SA_TRUE : SA_FALSE)
#define bxb_TA_AND(a, b)        ((T_SA_TRUE(a) && T_SA_TRUE(b)) ? SA_TRUE : SA_FALSE)
#define bxb_TA_XOR(a, b)        ((T_SA_TRUE(a) != T_SA_TRUE(b)) ? SA_TRUE : SA_FALSE)
#define bxb_TA_EQ(a, b)         (((a) == (b)) ? SA_TRUE : SA_FALSE)
#define bxb_TA_MAX(a, b)        (T_SA_TRUE(a) ? SA_TRUE : b)
#define bxb_TA_MIN(a, b)        (T_SA_FALSE(a) ? SA_FALSE : b)


/*************************************************/
/* INTEGER X INTEGER => BOOLEAN                  */
/*************************************************/
#define ixi_TA_EQ(a, b)         EQ_INT((a), (b))
#define ixi_TA_NE(a, b)         NE_INT((a), (b))
#define ixi_TA_LESS(a, b)       LT_INT((a), (b))
#define ixi_TA_LEQ(a, b)        LE_INT((a), (b))
#define ixi_TA_GREAT(a, b)      GT_INT((a), (b))
#define ixi_TA_GEQ(a, b)        GE_INT((a), (b))

/*************************************************/
/* STRING X STRING => BOOLEAN                    */
/*************************************************/
#define sxs_TA_EQ(a, b)         ((str_eq((a), (b))) ? SA_TRUE : SA_FALSE)
#define sxs_TA_NE(a, b)         ((str_eq((a), (b))) ? SA_FALSE : SA_TRUE)
#define sxs_TA_LEQ(a, b)        ((str_le((a), (b))) ? SA_TRUE : SA_FALSE)
#define sxs_TA_LESS(a, b)       ((str_lt((a), (b))) ? SA_TRUE : SA_FALSE)
#define sxs_TA_GEQ(a, b)        ((str_lt((a), (b))) ? SA_FALSE : SA_TRUE)
#define sxs_TA_GREAT(a, b)      ((str_le((a), (b))) ? SA_FALSE : SA_TRUE)

#define sxs_TA_MAX(a, b)        str_max((a), (b))
#define sxs_TA_MIN(a, b)        str_min((a), (b))

/*************************************************/
/* REAL X REAL => BOOLEAN                        */
/*************************************************/
#define rxr_TA_EQ(a, b)         ((R_SCALAR(DESC(a),valr) == R_SCALAR(DESC(b),valr)) ? \
                                  SA_TRUE : SA_FALSE)
#define rxr_TA_NE(a, b)         ((R_SCALAR(DESC(a),valr) != R_SCALAR(DESC(b),valr)) ? \
                                  SA_TRUE : SA_FALSE)
#define rxr_TA_LESS(a, b)       ((R_SCALAR(DESC(a),valr) < R_SCALAR(DESC(b),valr)) ?  \
                                  SA_TRUE : SA_FALSE)
#define rxr_TA_LEQ(a, b)        ((R_SCALAR(DESC(a),valr) <= R_SCALAR(DESC(b),valr)) ? \
                                  SA_TRUE : SA_FALSE)
#define rxr_TA_GREAT(a, b)      ((R_SCALAR(DESC(a),valr) > R_SCALAR(DESC(b),valr)) ? \
                                  SA_TRUE : SA_FALSE)
#define rxr_TA_GEQ(a, b)        ((R_SCALAR(DESC(a),valr) >= R_SCALAR(DESC(b),valr)) ? \
                                  SA_TRUE : SA_FALSE)

/*************************************************/
/* REAL X INTEGER => BOOLEAN                     */
/*************************************************/
#define rxi_TA_EQ(a, b)         ((R_SCALAR(DESC(a),valr) == (double)VAL_INT((b))) \
                                ?  SA_TRUE : SA_FALSE)
#define rxi_TA_NE(a, b)         ((R_SCALAR(DESC(a),valr) != (double)VAL_INT((b))) \
                                ? SA_TRUE : SA_FALSE)
#define rxi_TA_LESS(a, b)       ((R_SCALAR(DESC(a),valr) <  (double)VAL_INT((b))) \
                                ?  SA_TRUE : SA_FALSE)
#define rxi_TA_LEQ(a, b)        ((R_SCALAR(DESC(a),valr) <= (double)VAL_INT((b))) \
                                ?  SA_TRUE : SA_FALSE)
#define rxi_TA_GREAT(a, b)      ((R_SCALAR(DESC(a),valr) >  (double)VAL_INT((b))) \
                                ?  SA_TRUE : SA_FALSE)
#define rxi_TA_GEQ(a, b)        ((R_SCALAR(DESC(a),valr) >= (double)VAL_INT((b))) \
                                ?  SA_TRUE : SA_FALSE)

/*************************************************/
/* INTEGER X REAL => BOOLEAN                     */
/*************************************************/
#define ixr_TA_EQ(a, b)         (((double)VAL_INT((a)) == R_SCALAR(DESC(b),valr)) \
                                ?  SA_TRUE : SA_FALSE)
#define ixr_TA_NE(a, b)         (((double)VAL_INT((a)) != R_SCALAR(DESC(a),valr)) \
                                ?  SA_TRUE : SA_FALSE)
#define ixr_TA_LESS(a, b)       (((double)VAL_INT((a)) <  R_SCALAR(DESC(a),valr)) \
                                ?  SA_TRUE : SA_FALSE)
#define ixr_TA_LEQ(a, b)        (((double)VAL_INT((a)) <= R_SCALAR(DESC(a),valr)) \
                                ?  SA_TRUE : SA_FALSE)
#define ixr_TA_GREAT(a, b)      (((double)VAL_INT((a)) >  R_SCALAR(DESC(a),valr)) \
                                ?  SA_TRUE : SA_FALSE)
#define ixr_TA_GEQ(a, b)        (((double)VAL_INT((a)) >= R_SCALAR(DESC(a),valr)) \
                                ?  SA_TRUE : SA_FALSE)

/*************************************************/
/* INTEGER X INTEGER => INTEGER                  */
/*************************************************/
#define ixi_TA_MAX(a, b)        MAX_INT((a), (b))
#define ixi_TA_MIN(a, b)        MIN_INT((a), (b))
#define ixi_TA_PLUS(a, b)       ADD_INT((a), (b))
#define ixi_TA_MINUS(a, b)      SUB_INT((a), (b))
#define ixi_TA_MULT(a, b)       MUL_INT((a), (b))
#define ixi_TA_MOD(a, b)        ((T_ZERO((b))) ? controlled_exit("Division by Zero") \
                                               : MOD_INT((a), (b)))
#define ixi_TA_QUOT(a, b)       ((T_ZERO((b))) ? controlled_exit("Division by Zero") \
                                               : TAG_INT(VAL_INT(a) / VAL_INT(b)))
/* the div really is int x int -> real */
#define ixi_TA_DIV(a, b)        ((T_ZERO((b))) ? controlled_exit("Division by Zero") \
                                               : DIV_INT((a), (b)))

/*************************************************/
/* REAL X REAL => REAL                           */
/*************************************************/
#define MODULO(a, b)            ((a) - (int)((a) / (b)) * (b))
#define rxr_TA_MAX(a, b)        ((R_SCALAR(DESC(a),valr) < R_SCALAR(DESC(b),valr)) \
                                  ? R_SCALAR(DESC(b),valr) : R_SCALAR(DESC(a),valr))
#define rxr_TA_MIN(a, b)        ((R_SCALAR(DESC(a),valr) < R_SCALAR(DESC(b),valr)) \
                                  ? R_SCALAR(DESC(a),valr) : R_SCALAR(DESC(b),valr))
#define rxr_TA_PLUS(a, b)       (R_SCALAR(DESC(a),valr) + R_SCALAR(DESC(b),valr))
#define rxr_TA_MINUS(a, b)      (R_SCALAR(DESC(a),valr) - R_SCALAR(DESC(b),valr))
#define rxr_TA_MULT(a, b)       (R_SCALAR(DESC(a),valr) * R_SCALAR(DESC(b),valr))
#define rxr_TA_DIV(a, b)        ((0.0 == (R_SCALAR(DESC(b),valr))) \
                                ? controlled_exit("Division by Zero") \
                                : (R_SCALAR(DESC(a),valr) / R_SCALAR(DESC(b),valr)))
#define rxr_TA_MOD(a, b)        ((0.0 == R_SCALAR(DESC(b),valr))      \
                                ? controlled_exit("Division by Zero") \
                                : MODULO(R_SCALAR(DESC(a),valr), R_SCALAR(DESC(b),valr)))

#define MODULO(a, b)            ((a) - (int)((a) / (b)) * (b))

/*************************************************/
/* REAL X INTEGER => REAL                        */
/*************************************************/
#define rxi_TA_MAX(a, b)        ((R_SCALAR(DESC(a),valr) < (double)VAL_INT((b))) \
                                   ? (b) : R_SCALAR(DESC(a),valr))
#define rxi_TA_MIN(a, b)        ((R_SCALAR(DESC(a),valr) < (double)VAL_INT((b)))    \
                                  ? R_SCALAR(DESC(a),valr) : (b))
#define rxi_TA_PLUS(a, b)       (R_SCALAR(DESC(a),valr) + (double)VAL_INT((b)))
#define rxi_TA_MINUS(a, b)      (R_SCALAR(DESC(a),valr) - (double)VAL_INT((b)))
#define rxi_TA_MULT(a, b)       (R_SCALAR(DESC(a),valr) * (double)VAL_INT((b)))
#define rxi_TA_DIV(a, b)        ((T_ZERO((b))) ? controlled_exit("Division by Zero") \
                                   : (R_SCALAR(DESC(a),valr) / (double)VAL_INT((b))))
#define rxi_TA_MOD(a, b)        ((T_ZERO((b))) ? controlled_exit("Division by Zero") \
                                   : MODULO((R_SCALAR(DESC(a),valr), (double)VAL_INT(b))))

/*************************************************/
/* INTEGER X REAL => REAL                        */
/*************************************************/
#define ixr_TA_MAX(a, b)        (((double)VAL_INT((a)) < R_SCALAR(DESC(b),valr)) \
                                  ? R_SCALAR(DESC(b),valr) : (a))
#define ixr_TA_MIN(a, b)        (((double)VAL_INT((a)) < R_SCALAR(DESC(b),valr)) \
                                  ? (a) : R_SCALAR(DESC(b),valr))
#define ixr_TA_PLUS(a, b)       ((double)VAL_INT((a)) + R_SCALAR(DESC(b),valr))
#define ixr_TA_MINUS(a, b)      ((double)VAL_INT((a)) - R_SCALAR(DESC(b),valr))
#define ixr_TA_MULT(a, b)       ((double)VAL_INT((a)) * R_SCALAR(DESC(b),valr))
#define ixr_TA_DIV(a, b)        ((0.0 == R_SCALAR(DESC(b),valr))      \
                                ? controlled_exit("Division by Zero") \
                                : ((double)VAL_INT((a)) / R_SCALAR(DESC(b),valr)))
#define ixr_TA_MOD(a, b)        ((0.0 == (R_SCALAR(DESC(b),valr)))    \
                                ? controlled_exit("Division by Zero") \
                                : MODULO((double)VAL_INT(a), (R_SCALAR(DESC(b),valr))))

/*************************************************/
/* SOMETHING => SOMETHING                        */
/*************************************************/
#define me_TA_TO_LIST(a)        (INC_REFCNT(_nil), _nil)  
#define ve_TA_TO_LIST(a)        (INC_REFCNT(_nil), _nil)
#define te_TA_TO_LIST(a)        (INC_REFCNT(_nil), _nil)

#define mi_TA_TO_LIST(a)        t_to_list((a), C_MATRIX, TY_INTEGER)
#define mr_TA_TO_LIST(a)        t_to_list((a), C_MATRIX, TY_REAL)
#define mb_TA_TO_LIST(a)        t_to_list((a), C_MATRIX, TY_BOOL)

#define vi_TA_TO_LIST(a)        t_to_list((a), C_VECTOR, TY_INTEGER)
#define vr_TA_TO_LIST(a)        t_to_list((a), C_VECTOR, TY_REAL)
#define vb_TA_TO_LIST(a)        t_to_list((a), C_VECTOR, TY_BOOL)

#define ti_TA_TO_LIST(a)        t_to_list((a), C_TVECTOR, TY_INTEGER)
#define tr_TA_TO_LIST(a)        t_to_list((a), C_TVECTOR, TY_REAL)
#define tb_TA_TO_LIST(a)        t_to_list((a), C_TVECTOR, TY_BOOL)

/*************************************************/
/* INTEGER X LIST => SOMETHING                   */
/*************************************************/
#define ixl_TA_LSELECT(a, b)    t_lselect((VAL_INT(a) - 1), b)
#define rxl_TA_LSELECT(a, b)    t_lselect((int)R_SCALAR(DESC(a), valr) - 1, b)

#define ixixl_TA_LTRANSFORM(a, b, c) t_ltransform(VAL_INT(a), VAL_INT(b), c)
#define ixrxl_TA_LTRANSFORM(a, b, c) t_ltransform((int)R_SCALAR(DESC(a), valr), \
                                                  VAL_INT(b), c)
#define rxixl_TA_LTRANSFORM(a, b, c) t_ltransform((int)R_SCALAR(DESC(a), valr), \
                                                  VAL_INT(b), c)
#define rxrxl_TA_LTRANSFORM(a, b, c) t_ltransform((int)R_SCALAR(DESC(a), valr), \
                                                  (int)R_SCALAR(DESC(b), valr), c)


/******************************************/
/* LIST => LIST                           */
/******************************************/
#define l_TA_REVERSE(a)         t_reverse((a), TY_UNDEF)
#define s_TA_REVERSE(a)         t_reverse((a), TY_STRING)

/*************************************************/
/* LIST X LIST => LIST                           */
/*************************************************/
#define lxl_TA_LUNI(a, b)       t_lunite((a), (b), TY_UNDEF)

/*************************************************/
/* INTEGER X LIST => LIST                        */
/*************************************************/
#define ixl_TA_LCUT(a, b)       t_lcut(VAL_INT((a)), (b))
#define ixl_TA_LROTATE(a, b)    t_lrotate(VAL_INT((a)), (b))

/*************************************************/
/* INTEGER|REAL X (T)VECTOR => (T)VECTOR         */
/*************************************************/
#define ixve_TA_VCUT(a, b)       (INC_REFCNT(_nilvect), _nilvect)
#define rxve_TA_VCUT(a, b)       (INC_REFCNT(_nilvect), _nilvect)
#define ixte_TA_VCUT(a, b)       (INC_REFCNT(_niltvect), _niltvect)
#define rxte_TA_VCUT(a, b)       (INC_REFCNT(_niltvect), _niltvect)


#define ixvi_TA_VCUT(a, b)       t_vvcut(VAL_INT((a)), (b), C_VECTOR, TY_INTEGER)
#define rxvi_TA_VCUT(a, b)       t_vvcut((int)R_SCALAR(DESC(a), valr), (b), C_VECTOR, TY_INTEGER)
#define ixvr_TA_VCUT(a, b)       t_vvcut(VAL_INT((a)), (b), C_VECTOR, TY_REAL)
#define rxvr_TA_VCUT(a, b)       t_vvcut((int)R_SCALAR(DESC(a), valr), (b), C_VECTOR, TY_REAL)
#define ixti_TA_VCUT(a, b)       t_tvcut(VAL_INT((a)), (b), C_TVECTOR, TY_INTEGER)
#define ixvb_TA_VCUT(a, b)       t_vvcut(VAL_INT((a)), (b), C_VECTOR, TY_BOOL)
#define rxvb_TA_VCUT(a, b)       t_vvcut((int)R_SCALAR(DESC(a), valr), (b), C_VECTOR, TY_BOOL)
#define rxti_TA_VCUT(a, b)       t_tvcut((int)R_SCALAR(DESC(a), valr), (b), C_TVECTOR, TY_INTEGER)
#define ixtr_TA_VCUT(a, b)       t_tvcut(VAL_INT((a)), (b), C_TVECTOR, TY_REAL)
#define rxtr_TA_VCUT(a, b)       t_tvcut((int)R_SCALAR(DESC(a), valr), (b), C_TVECTOR, TY_REAL)
#define ixtb_TA_VCUT(a, b)       t_vvcut(VAL_INT((a)), (b), C_TVECTOR, TY_BOOL)
#define rxtb_TA_VCUT(a, b)       t_vvcut((int)R_SCALAR(DESC(a), valr), (b), C_TVECTOR, TY_BOOL)

#define ixve_TA_VROTATE(a, b)    (INC_REFCNT(_nilvect), _nilvect)
#define rxve_TA_VROTATE(a, b)    (INC_REFCNT(_nilvect), _nilvect)
#define ixte_TA_VROTATE(a, b)    (INC_REFCNT(_niltvect), _niltvect)
#define rxte_TA_VROTATE(a, b)    (INC_REFCNT(_niltvect), _niltvect)
#define ixvi_TA_VROTATE(a, b)     t_vvrotate(VAL_INT((a)), (b), C_VECTOR, TY_INTEGER) 
#define rxvi_TA_VROTATE(a, b)     t_vvrotate((int)R_SCALAR(DESC(a), valr), (b), C_VECTOR, TY_INTEGER)   
#define ixvr_TA_VROTATE(a, b)     t_vvrotate(VAL_INT((a)), (b), C_VECTOR, TY_REAL)   
#define rxvr_TA_VROTATE(a, b)     t_vvrotate((int)R_SCALAR(DESC(a), valr), (b), C_VECTOR, TY_REAL)      
#define ixvb_TA_VROTATE(a, b)     t_vvrotate(VAL_INT((a)), (b), C_VECTOR, TY_BOOL) 
#define rxvb_TA_VROTATE(a, b)     t_vvrotate((int)R_SCALAR(DESC(a), valr), (b), C_VECTOR, TY_BOOL)   
#define ixti_TA_VROTATE(a, b)     t_tvrotate(VAL_INT((a)), (b), C_TVECTOR, TY_INTEGER)
#define rxti_TA_VROTATE(a, b)     t_tvrotate((int)R_SCALAR(DESC(a), valr), (b), C_TVECTOR, TY_INTEGER)  
#define ixtr_TA_VROTATE(a, b)     t_tvrotate(VAL_INT((a)), (b), C_TVECTOR, TY_REAL)  
#define rxtr_TA_VROTATE(a, b)     t_tvrotate((int)R_SCALAR(DESC(a), valr), (b), C_TVECTOR, TY_REAL)     
#define ixtb_TA_VROTATE(a, b)     t_vvrotate(VAL_INT((a)), (b), C_TVECTOR, TY_BOOL) 
#define rxtb_TA_VROTATE(a, b)     t_vvrotate((int)R_SCALAR(DESC(a), valr), (b), C_TVECTOR, TY_BOOL)   

#define ixvi_TA_VSELECT(a, b)     t_vselect(VAL_INT((a)), (b), C_VECTOR, TY_INTEGER)
#define ixvr_TA_VSELECT(a, b)     t_vrselect(VAL_INT((a)), (b), C_VECTOR, TY_REAL)
#define ixvb_TA_VSELECT(a, b)     t_vselect(VAL_INT((a)), (b), C_VECTOR, TY_BOOL)
#define rxvi_TA_VSELECT(a, b)     t_vselect((int)R_SCALAR(DESC(a), valr), (b), C_VECTOR, TY_INTEGER)
#define rxvr_TA_VSELECT(a, b)     t_vrselect((int)R_SCALAR(DESC(a), valr), (b), C_VECTOR, TY_REAL)
#define rxvb_TA_VSELECT(a, b)     t_vselect((int)R_SCALAR(DESC(a), valr), (b), C_VECTOR, TY_BOOL)

#define ixti_TA_VSELECT(a, b)     t_vselect(VAL_INT((a)), (b), C_TVECTOR, TY_INTEGER)
#define ixtr_TA_VSELECT(a, b)     t_vrselect(VAL_INT((a)), (b), C_TVECTOR, TY_REAL)
#define ixtb_TA_VSELECT(a, b)     t_vselect(VAL_INT((a)), (b), C_TVECTOR, TY_BOOL)
#define rxti_TA_VSELECT(a, b)     t_vselect((int)R_SCALAR(DESC(a), valr), (b), C_TVECTOR, TY_INTEGER)
#define rxtr_TA_VSELECT(a, b)     t_vrselect((int)R_SCALAR(DESC(a), valr), (b), C_TVECTOR, TY_REAL)
#define rxtb_TA_VSELECT(a, b)     t_vselect((int)R_SCALAR(DESC(a), valr), (b), C_TVECTOR, TY_BOOL)

/*************************************************/
/* INTEGER X STRING => SOMETHING                 */
/*************************************************/
#define ixs_TA_LSELECT(a, b)    t_sselect((VAL_INT(a) - 1), b)

/*************************************************/
/* STRING X STRING => STRING                     */
/*************************************************/
#define sxs_TA_LUNI(a, b)       t_lunite((a), (b), TY_STRING)

/*************************************************/
/* INTEGER X STRING => STRING                    */
/*************************************************/
#define ixs_TA_LCUT(a, b)       t_lcut(VAL_INT((a)), (b))
#define ixs_TA_LROTATE(a, b)    t_lrotate(VAL_INT((a)), (b))

/*******************************************************/
/* A LOT OF EQ AND NE EVALUATING TO SA_FALSE (SA_TRUE) */
/* BECAUSE OF INCOMPATIBLE TYPES                       */
/*******************************************************/
#define ixs_TA_EQ(a, b)         SA_FALSE
#define sxi_TA_EQ(a, b)         SA_FALSE
#define ixl_TA_EQ(a, b)         SA_FALSE
#define lxi_TA_EQ(a, b)         SA_FALSE
#define ixb_TA_EQ(a, b)         SA_FALSE
#define bxi_TA_EQ(a, b)         SA_FALSE

#define ixme_TA_EQ(a, b)         SA_FALSE
#define mexi_TA_EQ(a, b)         SA_FALSE
#define ixve_TA_EQ(a, b)         SA_FALSE
#define vexi_TA_EQ(a, b)         SA_FALSE
#define ixte_TA_EQ(a, b)         SA_FALSE
#define texi_TA_EQ(a, b)         SA_FALSE

#define bxme_TA_EQ(a, b)         SA_FALSE
#define mexb_TA_EQ(a, b)         SA_FALSE
#define bxve_TA_EQ(a, b)         SA_FALSE
#define vexb_TA_EQ(a, b)         SA_FALSE
#define bxte_TA_EQ(a, b)         SA_FALSE
#define texb_TA_EQ(a, b)         SA_FALSE

#define rxme_TA_EQ(a, b)         SA_FALSE
#define mexr_TA_EQ(a, b)         SA_FALSE
#define rxve_TA_EQ(a, b)         SA_FALSE
#define vexr_TA_EQ(a, b)         SA_FALSE
#define rxte_TA_EQ(a, b)         SA_FALSE
#define texr_TA_EQ(a, b)         SA_FALSE

#define sxme_TA_EQ(a, b)         SA_FALSE
#define mexs_TA_EQ(a, b)         SA_FALSE
#define sxve_TA_EQ(a, b)         SA_FALSE
#define vexs_TA_EQ(a, b)         SA_FALSE
#define sxte_TA_EQ(a, b)         SA_FALSE
#define texs_TA_EQ(a, b)         SA_FALSE

#define lxme_TA_EQ(a, b)         SA_FALSE
#define mexl_TA_EQ(a, b)         SA_FALSE
#define lxve_TA_EQ(a, b)         SA_FALSE
#define vexl_TA_EQ(a, b)         SA_FALSE
#define lxte_TA_EQ(a, b)         SA_FALSE
#define texl_TA_EQ(a, b)         SA_FALSE

#define mexvi_TA_EQ(a, b)         SA_FALSE
#define mexvr_TA_EQ(a, b)         SA_FALSE
#define mexvb_TA_EQ(a, b)         SA_FALSE
#define mexme_TA_EQ(a, b)         SA_TRUE
#define mexve_TA_EQ(a, b)         SA_FALSE
#define mexte_TA_EQ(a, b)         SA_FALSE

#define vexmi_TA_EQ(a, b)         SA_FALSE
#define vexmr_TA_EQ(a, b)         SA_FALSE
#define vexmb_TA_EQ(a, b)         SA_FALSE
#define vexme_TA_EQ(a, b)         SA_FALSE
#define vexve_TA_EQ(a, b)         SA_TRUE
#define vexte_TA_EQ(a, b)         SA_FALSE

#define texmi_TA_EQ(a, b)         SA_FALSE
#define texmr_TA_EQ(a, b)         SA_FALSE
#define texmb_TA_EQ(a, b)         SA_FALSE
#define texme_TA_EQ(a, b)         SA_FALSE
#define texve_TA_EQ(a, b)         SA_FALSE
#define texte_TA_EQ(a, b)         SA_TRUE


#define ixme_TA_NE(a, b)         SA_TRUE
#define mexi_TA_NE(a, b)         SA_TRUE
#define ixve_TA_NE(a, b)         SA_TRUE
#define vexi_TA_NE(a, b)         SA_TRUE
#define ixte_TA_NE(a, b)         SA_TRUE
#define texi_TA_NE(a, b)         SA_TRUE

#define bxme_TA_NE(a, b)         SA_TRUE
#define mexb_TA_NE(a, b)         SA_TRUE
#define bxve_TA_NE(a, b)         SA_TRUE
#define vexb_TA_NE(a, b)         SA_TRUE
#define bxte_TA_NE(a, b)         SA_TRUE
#define texb_TA_NE(a, b)         SA_TRUE

#define rxme_TA_NE(a, b)         SA_TRUE
#define mexr_TA_NE(a, b)         SA_TRUE
#define rxve_TA_NE(a, b)         SA_TRUE
#define vexr_TA_NE(a, b)         SA_TRUE
#define rxte_TA_NE(a, b)         SA_TRUE
#define texr_TA_NE(a, b)         SA_TRUE

#define sxme_TA_NE(a, b)         SA_TRUE
#define mexs_TA_NE(a, b)         SA_TRUE
#define sxve_TA_NE(a, b)         SA_TRUE
#define vexs_TA_NE(a, b)         SA_TRUE
#define sxte_TA_NE(a, b)         SA_TRUE
#define texs_TA_NE(a, b)         SA_TRUE

#define lxme_TA_NE(a, b)         SA_TRUE
#define mexl_TA_NE(a, b)         SA_TRUE
#define lxve_TA_NE(a, b)         SA_TRUE
#define vexl_TA_NE(a, b)         SA_TRUE
#define lxte_TA_NE(a, b)         SA_TRUE
#define texl_TA_NE(a, b)         SA_TRUE

#define mexvi_TA_NE(a, b)         SA_TRUE
#define mexvr_TA_NE(a, b)         SA_TRUE
#define mexvb_TA_NE(a, b)         SA_TRUE
#define mexme_TA_NE(a, b)         SA_FALSE
#define mexve_TA_NE(a, b)         SA_TRUE
#define mexte_TA_NE(a, b)         SA_TRUE

#define vexmi_TA_NE(a, b)         SA_TRUE
#define vexmr_TA_NE(a, b)         SA_TRUE
#define vexmb_TA_NE(a, b)         SA_TRUE
#define vexme_TA_NE(a, b)         SA_TRUE
#define vexve_TA_NE(a, b)         SA_FALSE
#define vexte_TA_NE(a, b)         SA_TRUE

#define texmi_TA_NE(a, b)         SA_TRUE
#define texmr_TA_NE(a, b)         SA_TRUE
#define texmb_TA_NE(a, b)         SA_TRUE
#define texme_TA_NE(a, b)         SA_TRUE
#define texve_TA_NE(a, b)         SA_TRUE
#define texte_TA_NE(a, b)         SA_FALSE




#define ixmb_TA_EQ(a, b)         SA_FALSE
#define mbxi_TA_EQ(a, b)         SA_FALSE
#define ixvb_TA_EQ(a, b)         SA_FALSE
#define vbxi_TA_EQ(a, b)         SA_FALSE
#define ixtb_TA_EQ(a, b)         SA_FALSE
#define tbxi_TA_EQ(a, b)         SA_FALSE

#define sxl_TA_EQ(a, b)         SA_FALSE
#define lxs_TA_EQ(a, b)         SA_FALSE
#define sxr_TA_EQ(a, b)         SA_FALSE
#define rxs_TA_EQ(a, b)         SA_FALSE
#define sxb_TA_EQ(a, b)         SA_FALSE
#define bxs_TA_EQ(a, b)         SA_FALSE
#define sxmi_TA_EQ(a, b)         SA_FALSE
#define mixs_TA_EQ(a, b)         SA_FALSE
#define sxvi_TA_EQ(a, b)         SA_FALSE
#define vixs_TA_EQ(a, b)         SA_FALSE
#define sxti_TA_EQ(a, b)         SA_FALSE
#define tixs_TA_EQ(a, b)         SA_FALSE
#define sxmr_TA_EQ(a, b)         SA_FALSE
#define mrxs_TA_EQ(a, b)         SA_FALSE
#define sxvr_TA_EQ(a, b)         SA_FALSE
#define vrxs_TA_EQ(a, b)         SA_FALSE
#define sxtr_TA_EQ(a, b)         SA_FALSE
#define trxs_TA_EQ(a, b)         SA_FALSE
#define sxmb_TA_EQ(a, b)         SA_FALSE
#define mbxs_TA_EQ(a, b)         SA_FALSE
#define sxvb_TA_EQ(a, b)         SA_FALSE
#define vbxs_TA_EQ(a, b)         SA_FALSE
#define sxtb_TA_EQ(a, b)         SA_FALSE
#define tbxs_TA_EQ(a, b)         SA_FALSE

#define lxr_TA_EQ(a, b)         SA_FALSE
#define rxl_TA_EQ(a, b)         SA_FALSE
#define lxb_TA_EQ(a, b)         SA_FALSE
#define bxl_TA_EQ(a, b)         SA_FALSE
#define lxmi_TA_EQ(a, b)         SA_FALSE
#define mixl_TA_EQ(a, b)         SA_FALSE
#define lxvi_TA_EQ(a, b)         SA_FALSE
#define vixl_TA_EQ(a, b)         SA_FALSE
#define lxti_TA_EQ(a, b)         SA_FALSE
#define tixl_TA_EQ(a, b)         SA_FALSE
#define lxmr_TA_EQ(a, b)         SA_FALSE
#define mrxl_TA_EQ(a, b)         SA_FALSE
#define lxvr_TA_EQ(a, b)         SA_FALSE
#define vrxl_TA_EQ(a, b)         SA_FALSE
#define lxtr_TA_EQ(a, b)         SA_FALSE
#define trxl_TA_EQ(a, b)         SA_FALSE
#define lxmb_TA_EQ(a, b)         SA_FALSE
#define mbxl_TA_EQ(a, b)         SA_FALSE
#define lxvb_TA_EQ(a, b)         SA_FALSE
#define vbxl_TA_EQ(a, b)         SA_FALSE
#define lxtb_TA_EQ(a, b)         SA_FALSE
#define tbxl_TA_EQ(a, b)         SA_FALSE

#define rxb_TA_EQ(a, b)         SA_FALSE
#define bxr_TA_EQ(a, b)         SA_FALSE
#define rxmb_TA_EQ(a, b)         SA_FALSE
#define mbxr_TA_EQ(a, b)         SA_FALSE
#define rxvb_TA_EQ(a, b)         SA_FALSE
#define vbxr_TA_EQ(a, b)         SA_FALSE
#define rxtb_TA_EQ(a, b)         SA_FALSE
#define tbxr_TA_EQ(a, b)         SA_FALSE

#define bxmi_TA_EQ(a, b)         SA_FALSE
#define mixb_TA_EQ(a, b)         SA_FALSE
#define bxvi_TA_EQ(a, b)         SA_FALSE
#define vixb_TA_EQ(a, b)         SA_FALSE
#define bxti_TA_EQ(a, b)         SA_FALSE
#define tixb_TA_EQ(a, b)         SA_FALSE
#define bxmr_TA_EQ(a, b)         SA_FALSE
#define mrxb_TA_EQ(a, b)         SA_FALSE
#define bxvr_TA_EQ(a, b)         SA_FALSE
#define vrxb_TA_EQ(a, b)         SA_FALSE
#define bxtr_TA_EQ(a, b)         SA_FALSE
#define trxb_TA_EQ(a, b)         SA_FALSE
#define bxmb_TA_EQ(a, b)         SA_FALSE
#define mbxb_TA_EQ(a, b)         SA_FALSE
#define bxvb_TA_EQ(a, b)         SA_FALSE
#define vbxb_TA_EQ(a, b)         SA_FALSE
#define bxtb_TA_EQ(a, b)         SA_FALSE
#define tbxb_TA_EQ(a, b)         SA_FALSE

#define mixvi_TA_EQ(a, b)         SA_FALSE
#define mixvr_TA_EQ(a, b)         SA_FALSE
#define mixvb_TA_EQ(a, b)         SA_FALSE
#define mrxvi_TA_EQ(a, b)         SA_FALSE
#define mrxvr_TA_EQ(a, b)         SA_FALSE
#define mrxvb_TA_EQ(a, b)         SA_FALSE
#define mbxvi_TA_EQ(a, b)         SA_FALSE
#define mbxvr_TA_EQ(a, b)         SA_FALSE
#define mbxvb_TA_EQ(a, b)         SA_FALSE
#define mixti_TA_EQ(a, b)         SA_FALSE
#define mixtr_TA_EQ(a, b)         SA_FALSE
#define mixtb_TA_EQ(a, b)         SA_FALSE
#define mrxti_TA_EQ(a, b)         SA_FALSE
#define mrxtr_TA_EQ(a, b)         SA_FALSE
#define mrxtb_TA_EQ(a, b)         SA_FALSE
#define mbxti_TA_EQ(a, b)         SA_FALSE
#define mbxtr_TA_EQ(a, b)         SA_FALSE
#define mbxtb_TA_EQ(a, b)         SA_FALSE

#define vixmi_TA_EQ(a, b)         SA_FALSE
#define vixmr_TA_EQ(a, b)         SA_FALSE
#define vixmb_TA_EQ(a, b)         SA_FALSE
#define vrxmi_TA_EQ(a, b)         SA_FALSE
#define vrxmr_TA_EQ(a, b)         SA_FALSE
#define vrxmb_TA_EQ(a, b)         SA_FALSE
#define vbxmi_TA_EQ(a, b)         SA_FALSE
#define vbxmr_TA_EQ(a, b)         SA_FALSE
#define vbxmb_TA_EQ(a, b)         SA_FALSE
#define vixti_TA_EQ(a, b)         SA_FALSE
#define vixtr_TA_EQ(a, b)         SA_FALSE
#define vixtb_TA_EQ(a, b)         SA_FALSE
#define vrxti_TA_EQ(a, b)         SA_FALSE
#define vrxtr_TA_EQ(a, b)         SA_FALSE
#define vrxtb_TA_EQ(a, b)         SA_FALSE
#define vbxti_TA_EQ(a, b)         SA_FALSE
#define vbxtr_TA_EQ(a, b)         SA_FALSE
#define vbxtb_TA_EQ(a, b)         SA_FALSE

#define tixmi_TA_EQ(a, b)         SA_FALSE
#define tixmr_TA_EQ(a, b)         SA_FALSE
#define tixmb_TA_EQ(a, b)         SA_FALSE
#define trxmi_TA_EQ(a, b)         SA_FALSE
#define trxmr_TA_EQ(a, b)         SA_FALSE
#define trxmb_TA_EQ(a, b)         SA_FALSE
#define tbxmi_TA_EQ(a, b)         SA_FALSE
#define tbxmr_TA_EQ(a, b)         SA_FALSE
#define tbxmb_TA_EQ(a, b)         SA_FALSE
#define tixvi_TA_EQ(a, b)         SA_FALSE
#define tixvr_TA_EQ(a, b)         SA_FALSE
#define tixvb_TA_EQ(a, b)         SA_FALSE
#define trxvi_TA_EQ(a, b)         SA_FALSE
#define trxvr_TA_EQ(a, b)         SA_FALSE
#define trxvb_TA_EQ(a, b)         SA_FALSE
#define tbxvi_TA_EQ(a, b)         SA_FALSE
#define tbxvr_TA_EQ(a, b)         SA_FALSE
#define tbxvb_TA_EQ(a, b)         SA_FALSE

#define ixs_TA_NE(a, b)         SA_TRUE
#define sxi_TA_NE(a, b)         SA_TRUE
#define ixl_TA_NE(a, b)         SA_TRUE
#define lxi_TA_NE(a, b)         SA_TRUE
#define ixb_TA_NE(a, b)         SA_TRUE
#define bxi_TA_NE(a, b)         SA_TRUE
#define ixmb_TA_NE(a, b)         SA_TRUE
#define mbxi_TA_NE(a, b)         SA_TRUE
#define ixvb_TA_NE(a, b)         SA_TRUE
#define vbxi_TA_NE(a, b)         SA_TRUE
#define ixtb_TA_NE(a, b)         SA_TRUE
#define tbxi_TA_NE(a, b)         SA_TRUE

#define sxl_TA_NE(a, b)         SA_TRUE
#define lxs_TA_NE(a, b)         SA_TRUE
#define sxr_TA_NE(a, b)         SA_TRUE
#define rxs_TA_NE(a, b)         SA_TRUE
#define sxb_TA_NE(a, b)         SA_TRUE
#define bxs_TA_NE(a, b)         SA_TRUE
#define sxmi_TA_NE(a, b)         SA_TRUE
#define mixs_TA_NE(a, b)         SA_TRUE
#define sxvi_TA_NE(a, b)         SA_TRUE
#define vixs_TA_NE(a, b)         SA_TRUE
#define sxti_TA_NE(a, b)         SA_TRUE
#define tixs_TA_NE(a, b)         SA_TRUE
#define sxmr_TA_NE(a, b)         SA_TRUE
#define mrxs_TA_NE(a, b)         SA_TRUE
#define sxvr_TA_NE(a, b)         SA_TRUE
#define vrxs_TA_NE(a, b)         SA_TRUE
#define sxtr_TA_NE(a, b)         SA_TRUE
#define trxs_TA_NE(a, b)         SA_TRUE
#define sxmb_TA_NE(a, b)         SA_TRUE
#define mbxs_TA_NE(a, b)         SA_TRUE
#define sxvb_TA_NE(a, b)         SA_TRUE
#define vbxs_TA_NE(a, b)         SA_TRUE
#define sxtb_TA_NE(a, b)         SA_TRUE
#define tbxs_TA_NE(a, b)         SA_TRUE

#define lxr_TA_NE(a, b)         SA_TRUE
#define rxl_TA_NE(a, b)         SA_TRUE
#define lxb_TA_NE(a, b)         SA_TRUE
#define bxl_TA_NE(a, b)         SA_TRUE
#define lxmi_TA_NE(a, b)         SA_TRUE
#define mixl_TA_NE(a, b)         SA_TRUE
#define lxvi_TA_NE(a, b)         SA_TRUE
#define vixl_TA_NE(a, b)         SA_TRUE
#define lxti_TA_NE(a, b)         SA_TRUE
#define tixl_TA_NE(a, b)         SA_TRUE
#define lxmr_TA_NE(a, b)         SA_TRUE
#define mrxl_TA_NE(a, b)         SA_TRUE
#define lxvr_TA_NE(a, b)         SA_TRUE
#define vrxl_TA_NE(a, b)         SA_TRUE
#define lxtr_TA_NE(a, b)         SA_TRUE
#define trxl_TA_NE(a, b)         SA_TRUE
#define lxmb_TA_NE(a, b)         SA_TRUE
#define mbxl_TA_NE(a, b)         SA_TRUE
#define lxvb_TA_NE(a, b)         SA_TRUE
#define vbxl_TA_NE(a, b)         SA_TRUE
#define lxtb_TA_NE(a, b)         SA_TRUE
#define tbxl_TA_NE(a, b)         SA_TRUE

#define rxb_TA_NE(a, b)         SA_TRUE
#define bxr_TA_NE(a, b)         SA_TRUE
#define rxmb_TA_NE(a, b)         SA_TRUE
#define mbxr_TA_NE(a, b)         SA_TRUE
#define rxvb_TA_NE(a, b)         SA_TRUE
#define vbxr_TA_NE(a, b)         SA_TRUE
#define rxtb_TA_NE(a, b)         SA_TRUE
#define tbxr_TA_NE(a, b)         SA_TRUE

#define bxmi_TA_NE(a, b)         SA_TRUE
#define mixb_TA_NE(a, b)         SA_TRUE
#define bxvi_TA_NE(a, b)         SA_TRUE
#define vixb_TA_NE(a, b)         SA_TRUE
#define bxti_TA_NE(a, b)         SA_TRUE
#define tixb_TA_NE(a, b)         SA_TRUE
#define bxmr_TA_NE(a, b)         SA_TRUE
#define mrxb_TA_NE(a, b)         SA_TRUE
#define bxvr_TA_NE(a, b)         SA_TRUE
#define vrxb_TA_NE(a, b)         SA_TRUE
#define bxtr_TA_NE(a, b)         SA_TRUE
#define trxb_TA_NE(a, b)         SA_TRUE
#define bxmb_TA_NE(a, b)         SA_TRUE
#define mbxb_TA_NE(a, b)         SA_TRUE
#define bxvb_TA_NE(a, b)         SA_TRUE
#define vbxb_TA_NE(a, b)         SA_TRUE
#define bxtb_TA_NE(a, b)         SA_TRUE
#define tbxb_TA_NE(a, b)         SA_TRUE

#define mixvi_TA_NE(a, b)         SA_TRUE
#define mixvr_TA_NE(a, b)         SA_TRUE
#define mixvb_TA_NE(a, b)         SA_TRUE
#define mrxvi_TA_NE(a, b)         SA_TRUE
#define mrxvr_TA_NE(a, b)         SA_TRUE
#define mrxvb_TA_NE(a, b)         SA_TRUE
#define mbxvi_TA_NE(a, b)         SA_TRUE
#define mbxvr_TA_NE(a, b)         SA_TRUE
#define mbxvb_TA_NE(a, b)         SA_TRUE
#define mixti_TA_NE(a, b)         SA_TRUE
#define mixtr_TA_NE(a, b)         SA_TRUE
#define mixtb_TA_NE(a, b)         SA_TRUE
#define mrxti_TA_NE(a, b)         SA_TRUE
#define mrxtr_TA_NE(a, b)         SA_TRUE
#define mrxtb_TA_NE(a, b)         SA_TRUE
#define mbxti_TA_NE(a, b)         SA_TRUE
#define mbxtr_TA_NE(a, b)         SA_TRUE
#define mbxtb_TA_NE(a, b)         SA_TRUE

#define vixmi_TA_NE(a, b)         SA_TRUE
#define vixmr_TA_NE(a, b)         SA_TRUE
#define vixmb_TA_NE(a, b)         SA_TRUE
#define vrxmi_TA_NE(a, b)         SA_TRUE
#define vrxmr_TA_NE(a, b)         SA_TRUE
#define vrxmb_TA_NE(a, b)         SA_TRUE
#define vbxmi_TA_NE(a, b)         SA_TRUE
#define vbxmr_TA_NE(a, b)         SA_TRUE
#define vbxmb_TA_NE(a, b)         SA_TRUE
#define vixti_TA_NE(a, b)         SA_TRUE
#define vixtr_TA_NE(a, b)         SA_TRUE
#define vixtb_TA_NE(a, b)         SA_TRUE
#define vrxti_TA_NE(a, b)         SA_TRUE
#define vrxtr_TA_NE(a, b)         SA_TRUE
#define vrxtb_TA_NE(a, b)         SA_TRUE
#define vbxti_TA_NE(a, b)         SA_TRUE
#define vbxtr_TA_NE(a, b)         SA_TRUE
#define vbxtb_TA_NE(a, b)         SA_TRUE

#define tixmi_TA_NE(a, b)         SA_TRUE
#define tixmr_TA_NE(a, b)         SA_TRUE
#define tixmb_TA_NE(a, b)         SA_TRUE
#define trxmi_TA_NE(a, b)         SA_TRUE
#define trxmr_TA_NE(a, b)         SA_TRUE
#define trxmb_TA_NE(a, b)         SA_TRUE
#define tbxmi_TA_NE(a, b)         SA_TRUE
#define tbxmr_TA_NE(a, b)         SA_TRUE
#define tbxmb_TA_NE(a, b)         SA_TRUE
#define tixvi_TA_NE(a, b)         SA_TRUE
#define tixvr_TA_NE(a, b)         SA_TRUE
#define tixvb_TA_NE(a, b)         SA_TRUE
#define trxvi_TA_NE(a, b)         SA_TRUE
#define trxvr_TA_NE(a, b)         SA_TRUE
#define trxvb_TA_NE(a, b)         SA_TRUE
#define tbxvi_TA_NE(a, b)         SA_TRUE
#define tbxvr_TA_NE(a, b)         SA_TRUE
#define tbxvb_TA_NE(a, b)         SA_TRUE


/*******************************************************/
/* A LOT OF EQ AND NE ON MVT AND INT OR REAL           */
/*******************************************************/
#define ixme_TA_EQ(a, b)         SA_FALSE
#define mexi_TA_EQ(a, b)         SA_FALSE
#define ixve_TA_EQ(a, b)         SA_FALSE
#define vexi_TA_EQ(a, b)         SA_FALSE
#define ixte_TA_EQ(a, b)         SA_FALSE
#define texi_TA_EQ(a, b)         SA_FALSE

#define rxme_TA_EQ(a, b)         SA_FALSE
#define mexr_TA_EQ(a, b)         SA_FALSE
#define rxve_TA_EQ(a, b)         SA_FALSE
#define vexr_TA_EQ(a, b)         SA_FALSE
#define rxte_TA_EQ(a, b)         SA_FALSE
#define texr_TA_EQ(a, b)         SA_FALSE

#define ixme_TA_NE(a, b)         SA_TRUE
#define mexi_TA_NE(a, b)         SA_TRUE
#define ixve_TA_NE(a, b)         SA_TRUE
#define vexi_TA_NE(a, b)         SA_TRUE
#define ixte_TA_NE(a, b)         SA_TRUE
#define texi_TA_NE(a, b)         SA_TRUE

#define rxme_TA_NE(a, b)         SA_TRUE
#define mexr_TA_NE(a, b)         SA_TRUE
#define rxve_TA_NE(a, b)         SA_TRUE
#define vexr_TA_NE(a, b)         SA_TRUE
#define rxte_TA_NE(a, b)         SA_TRUE
#define texr_TA_NE(a, b)         SA_TRUE

#define ixmi_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define mixi_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define ixvi_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define vixi_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define ixti_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define tixi_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define ixmr_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define mrxi_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define ixvr_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define vrxi_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define ixtr_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define trxi_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))

#define rxmi_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define mixr_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define rxvi_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define vixr_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define rxti_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define tixr_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define rxmr_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define mrxr_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define rxvr_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define vrxr_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define rxtr_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))
#define trxr_TA_EQ(a, b)         t_scal_mvt_equal((T_PTD)(a), (T_PTD)(b))

#define ixmi_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define mixi_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define ixvi_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define vixi_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define ixti_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define tixi_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define ixmr_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define mrxi_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define ixvr_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define vrxi_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define ixtr_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define trxi_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))

#define rxmi_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define mixr_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define rxvi_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define vixr_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define rxti_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define tixr_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define rxmr_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define mrxr_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define rxvr_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define vrxr_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define rxtr_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))
#define trxr_TA_NE(a, b)         t_scal_mvt_nequal((T_PTD)(a), (T_PTD)(b))




#define mixi_TA_GEQ(a, b)           t_scal_mvt_gequal((T_PTD)(a), (T_PTD)(b))
#define mixi_TA_GREAT(a, b)         t_scal_mvt_greater((T_PTD)(a), (T_PTD)(b))
#define mixi_TA_LESS(a, b)          t_scal_mvt_less((T_PTD)(a), (T_PTD)(b))
#define mixi_TA_LEQ(a, b)           t_scal_mvt_lequal((T_PTD)(a), (T_PTD)(b))
#define mixr_TA_GEQ(a, b)           t_scal_mvt_gequal((T_PTD)(a), (T_PTD)(b))
#define mixr_TA_GREAT(a, b)         t_scal_mvt_greater((T_PTD)(a), (T_PTD)(b))
#define mixr_TA_LESS(a, b)          t_scal_mvt_less((T_PTD)(a), (T_PTD)(b))
#define mixr_TA_LEQ(a, b)           t_scal_mvt_lequal((T_PTD)(a), (T_PTD)(b))
#define mrxi_TA_GEQ(a, b)           t_scal_mvt_gequal((T_PTD)(a), (T_PTD)(b))
#define mrxi_TA_GREAT(a, b)         t_scal_mvt_greater((T_PTD)(a), (T_PTD)(b))
#define mrxi_TA_LESS(a, b)          t_scal_mvt_less((T_PTD)(a), (T_PTD)(b))
#define mrxi_TA_LEQ(a, b)           t_scal_mvt_lequal((T_PTD)(a), (T_PTD)(b))
#define mrxr_TA_GEQ(a, b)           t_scal_mvt_gequal((T_PTD)(a), (T_PTD)(b))
#define mrxr_TA_GREAT(a, b)         t_scal_mvt_greater((T_PTD)(a), (T_PTD)(b))
#define mrxr_TA_LESS(a, b)          t_scal_mvt_less((T_PTD)(a), (T_PTD)(b))
#define mrxr_TA_LEQ(a, b)           t_scal_mvt_lequal((T_PTD)(a), (T_PTD)(b))

#define vixi_TA_GEQ(a, b)           t_scal_mvt_gequal((T_PTD)(a), (T_PTD)(b))
#define vixi_TA_GREAT(a, b)         t_scal_mvt_greater((T_PTD)(a), (T_PTD)(b))
#define vixi_TA_LESS(a, b)          t_scal_mvt_less((T_PTD)(a), (T_PTD)(b))
#define vixi_TA_LEQ(a, b)           t_scal_mvt_lequal((T_PTD)(a), (T_PTD)(b))
#define vixr_TA_GEQ(a, b)           t_scal_mvt_gequal((T_PTD)(a), (T_PTD)(b))
#define vixr_TA_GREAT(a, b)         t_scal_mvt_greater((T_PTD)(a), (T_PTD)(b))
#define vixr_TA_LESS(a, b)          t_scal_mvt_less((T_PTD)(a), (T_PTD)(b))
#define vixr_TA_LEQ(a, b)           t_scal_mvt_lequal((T_PTD)(a), (T_PTD)(b))
#define vrxi_TA_GEQ(a, b)           t_scal_mvt_gequal((T_PTD)(a), (T_PTD)(b))
#define vrxi_TA_GREAT(a, b)         t_scal_mvt_greater((T_PTD)(a), (T_PTD)(b))
#define vrxi_TA_LESS(a, b)          t_scal_mvt_less((T_PTD)(a), (T_PTD)(b))
#define vrxi_TA_LEQ(a, b)           t_scal_mvt_lequal((T_PTD)(a), (T_PTD)(b))
#define vrxr_TA_GEQ(a, b)           t_scal_mvt_gequal((T_PTD)(a), (T_PTD)(b))
#define vrxr_TA_GREAT(a, b)         t_scal_mvt_greater((T_PTD)(a), (T_PTD)(b))
#define vrxr_TA_LESS(a, b)          t_scal_mvt_less((T_PTD)(a), (T_PTD)(b))
#define vrxr_TA_LEQ(a, b)           t_scal_mvt_lequal((T_PTD)(a), (T_PTD)(b))

#define tixi_TA_GEQ(a, b)           t_scal_mvt_gequal((T_PTD)(a), (T_PTD)(b))
#define tixi_TA_GREAT(a, b)         t_scal_mvt_greater((T_PTD)(a), (T_PTD)(b))
#define tixi_TA_LESS(a, b)          t_scal_mvt_less((T_PTD)(a), (T_PTD)(b))
#define tixi_TA_LEQ(a, b)           t_scal_mvt_lequal((T_PTD)(a), (T_PTD)(b))
#define tixr_TA_GEQ(a, b)           t_scal_mvt_gequal((T_PTD)(a), (T_PTD)(b))
#define tixr_TA_GREAT(a, b)         t_scal_mvt_greater((T_PTD)(a), (T_PTD)(b))
#define tixr_TA_LESS(a, b)          t_scal_mvt_less((T_PTD)(a), (T_PTD)(b))
#define tixr_TA_LEQ(a, b)           t_scal_mvt_lequal((T_PTD)(a), (T_PTD)(b))
#define trxi_TA_GEQ(a, b)           t_scal_mvt_gequal((T_PTD)(a), (T_PTD)(b))
#define trxi_TA_GREAT(a, b)         t_scal_mvt_greater((T_PTD)(a), (T_PTD)(b))
#define trxi_TA_LESS(a, b)          t_scal_mvt_less((T_PTD)(a), (T_PTD)(b))
#define trxi_TA_LEQ(a, b)           t_scal_mvt_lequal((T_PTD)(a), (T_PTD)(b))
#define trxr_TA_GEQ(a, b)           t_scal_mvt_gequal((T_PTD)(a), (T_PTD)(b))
#define trxr_TA_GREAT(a, b)         t_scal_mvt_greater((T_PTD)(a), (T_PTD)(b))
#define trxr_TA_LESS(a, b)          t_scal_mvt_less((T_PTD)(a), (T_PTD)(b))
#define trxr_TA_LEQ(a, b)           t_scal_mvt_lequal((T_PTD)(a), (T_PTD)(b))

/**********************************************************/
/* MVT X MVT -> MVT                                       */
/* 0 = intxint, 1, = intxreal, 2 = realxint, 3= realxreal */
/**********************************************************/
#define mixmi_TA_MULT(a, b)              t_mvt_mvt_mult((a), (b), 0)
#define mixmr_TA_MULT(a, b)              t_mvt_mvt_mult((a), (b), 1)
#define mrxmi_TA_MULT(a, b)              t_mvt_mvt_mult((a), (b), 2)
#define mrxmr_TA_MULT(a, b)              t_mvt_mvt_mult((a), (b), 3)

#define mixmi_TA_DIV(a, b)               t_mvt_mvt_div((a), (b), 0)
#define mixmr_TA_DIV(a, b)               t_mvt_mvt_div((a), (b), 1)
#define mrxmi_TA_DIV(a, b)               t_mvt_mvt_div((a), (b), 2)
#define mrxmr_TA_DIV(a, b)               t_mvt_mvt_div((a), (b), 3)

#define mixmi_TA_PLUS(a, b)              t_mvt_mvt_plus((a), (b), 0)
#define mixmr_TA_PLUS(a, b)              t_mvt_mvt_plus((a), (b), 1)
#define mrxmi_TA_PLUS(a, b)              t_mvt_mvt_plus((a), (b), 2)
#define mrxmr_TA_PLUS(a, b)              t_mvt_mvt_plus((a), (b), 3)

#define mixmi_TA_MINUS(a, b)             t_mvt_mvt_minus((a), (b), 0)
#define mixmr_TA_MINUS(a, b)             t_mvt_mvt_minus((a), (b), 1)
#define mrxmi_TA_MINUS(a, b)             t_mvt_mvt_minus((a), (b), 2)
#define mrxmr_TA_MINUS(a, b)             t_mvt_mvt_minus((a), (b), 3)

#define mixmi_TA_MOD(a, b)               t_mvt_mvt_mod((a), (b), 0)
#define mixmr_TA_MOD(a, b)               t_mvt_mvt_mod((a), (b), 1)
#define mrxmi_TA_MOD(a, b)               t_mvt_mvt_mod((a), (b), 2)
#define mrxmr_TA_MOD(a, b)               t_mvt_mvt_mod((a), (b), 3)

#define vixvi_TA_MULT(a, b)              t_mvt_mvt_mult((a), (b), 0)
#define vixvr_TA_MULT(a, b)              t_mvt_mvt_mult((a), (b), 1)
#define vrxvi_TA_MULT(a, b)              t_mvt_mvt_mult((a), (b), 2)
#define vrxvr_TA_MULT(a, b)              t_mvt_mvt_mult((a), (b), 3)

#define vixvi_TA_DIV(a, b)               t_mvt_mvt_div((a), (b), 0)
#define vixvr_TA_DIV(a, b)               t_mvt_mvt_div((a), (b), 1)
#define vrxvi_TA_DIV(a, b)               t_mvt_mvt_div((a), (b), 2)
#define vrxvr_TA_DIV(a, b)               t_mvt_mvt_div((a), (b), 3)

#define vixvi_TA_PLUS(a, b)              t_mvt_mvt_plus((a), (b), 0)
#define vixvr_TA_PLUS(a, b)              t_mvt_mvt_plus((a), (b), 1)
#define vrxvi_TA_PLUS(a, b)              t_mvt_mvt_plus((a), (b), 2)
#define vrxvr_TA_PLUS(a, b)              t_mvt_mvt_plus((a), (b), 3)

#define vixvi_TA_MINUS(a, b)             t_mvt_mvt_vinus((a), (b), 0)
#define vixvr_TA_MINUS(a, b)             t_mvt_mvt_vinus((a), (b), 1)
#define vrxvi_TA_MINUS(a, b)             t_mvt_mvt_vinus((a), (b), 2)
#define vrxvr_TA_MINUS(a, b)             t_mvt_mvt_vinus((a), (b), 3)

#define vixvi_TA_MOD(a, b)               t_mvt_mvt_mod((a), (b), 0)
#define vixvr_TA_MOD(a, b)               t_mvt_mvt_mod((a), (b), 1)
#define vrxvi_TA_MOD(a, b)               t_mvt_mvt_mod((a), (b), 2)
#define vrxvr_TA_MOD(a, b)               t_mvt_mvt_mod((a), (b), 3)

#define tixti_TA_MULT(a, b)              t_mvt_mvt_mult((a), (b), 0)
#define tixtr_TA_MULT(a, b)              t_mvt_mvt_mult((a), (b), 1)
#define trxti_TA_MULT(a, b)              t_mvt_mvt_mult((a), (b), 2)
#define trxtr_TA_MULT(a, b)              t_mvt_mvt_mult((a), (b), 3)

#define tixti_TA_DIV(a, b)               t_mvt_mvt_div((a), (b), 0)
#define tixtr_TA_DIV(a, b)               t_mvt_mvt_div((a), (b), 1)
#define trxti_TA_DIV(a, b)               t_mvt_mvt_div((a), (b), 2)
#define trxtr_TA_DIV(a, b)               t_mvt_mvt_div((a), (b), 3)

#define tixti_TA_PLUS(a, b)              t_mvt_mvt_plus((a), (b), 0)
#define tixtr_TA_PLUS(a, b)              t_mvt_mvt_plus((a), (b), 1)
#define trxti_TA_PLUS(a, b)              t_mvt_mvt_plus((a), (b), 2)
#define trxtr_TA_PLUS(a, b)              t_mvt_mvt_plus((a), (b), 3)

#define tixti_TA_MINUS(a, b)             t_mvt_mvt_tinus((a), (b), 0)
#define tixtr_TA_MINUS(a, b)             t_mvt_mvt_tinus((a), (b), 1)
#define trxti_TA_MINUS(a, b)             t_mvt_mvt_tinus((a), (b), 2)
#define trxtr_TA_MINUS(a, b)             t_mvt_mvt_tinus((a), (b), 3)

#define tixti_TA_MOD(a, b)               t_mvt_mvt_mod((a), (b), 0)
#define tixtr_TA_MOD(a, b)               t_mvt_mvt_mod((a), (b), 1)
#define trxti_TA_MOD(a, b)               t_mvt_mvt_mod((a), (b), 2)
#define trxtr_TA_MOD(a, b)               t_mvt_mvt_mod((a), (b), 3)



#define mixmi_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 0, C_MATRIX)
#define mixmr_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 1, C_MATRIX)
#define mrxmi_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 2, C_MATRIX)
#define mrxmr_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 3, C_MATRIX)

#define tixti_TA_IP(a, b)                t_tv_tv_ip((a), (b), 0)
#define tixtr_TA_IP(a, b)                R_SCALAR(DESC(t_tv_tv_ip((a), (b), 1)), valr)
#define trxti_TA_IP(a, b)                R_SCALAR(DESC(t_tv_tv_ip((a), (b), 2)), valr)
#define trxtr_TA_IP(a, b)                R_SCALAR(DESC(t_tv_tv_ip((a), (b), 3)), valr)

#define vixvi_TA_IP(a, b)                t_tv_tv_ip((a), (b), 0)
#define vixvr_TA_IP(a, b)                R_SCALAR(DESC(t_tv_tv_ip((a), (b), 1)), valr)
#define vrxvi_TA_IP(a, b)                R_SCALAR(DESC(t_tv_tv_ip((a), (b), 2)), valr)
#define vrxvr_TA_IP(a, b)                R_SCALAR(DESC(t_tv_tv_ip((a), (b), 3)), valr)

#define mixvi_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 0, C_MATRIX)
#define mixvr_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 1, C_MATRIX)
#define mrxvi_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 2, C_MATRIX)
#define mrxvr_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 3, C_MATRIX)

#define vixmi_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 0, C_VECTOR)
#define vixmr_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 1, C_VECTOR)
#define vrxmi_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 2, C_VECTOR)
#define vrxmr_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 3, C_VECTOR)

#define mixti_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 0, C_TVECTOR)
#define mixtr_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 1, C_TVECTOR)
#define mrxti_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 2, C_TVECTOR)
#define mrxtr_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 3, C_TVECTOR)

#define tixmi_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 0, C_MATRIX)
#define tixmr_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 1, C_MATRIX)
#define trxmi_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 2, C_MATRIX)
#define trxmr_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 3, C_MATRIX)

#define vixti_TA_IP(a, b)                t_v_t_ip_int((a), (b))
#define vixtr_TA_IP(a, b)                t_v_t_ip_real((a), (b), 1)
#define vrxti_TA_IP(a, b)                t_v_t_ip_real((a), (b), 2)
#define vrxtr_TA_IP(a, b)                t_v_t_ip_real((a), (b), 3)

#define tixvi_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 0, C_MATRIX)
#define tixvr_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 1, C_MATRIX)
#define trxvi_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 2, C_MATRIX)
#define trxvr_TA_IP(a, b)                t_mvt_mvt_ip((a), (b), 3, C_MATRIX)

#define ve_TA_VC_MAX(a)                  ZERO_INT
#define te_TA_VC_MAX(a)                  ZERO_INT

#define ti_TA_VC_MAX(a)                  t_tv_vc_max((a), TY_INTEGER, C_TVECTOR)
#define tr_TA_VC_MAX(a)                  t_tvr_vc_max((a), TY_REAL, C_TVECTOR)
#define tb_TA_VC_MAX(a)                  t_tv_vc_max((a), TY_BOOL, C_TVECTOR)
#define vi_TA_VC_MAX(a)                  t_tv_vc_max((a), TY_INTEGER, C_VECTOR)
#define vr_TA_VC_MAX(a)                  t_tvr_vc_max((a), TY_REAL, C_VECTOR)
#define vb_TA_VC_MAX(a)                  t_tv_vc_max((a), TY_BOOL, C_VECTOR)

#define ve_TA_VC_MIN(a)                  ZERO_INT
#define te_TA_VC_MIN(a)                  ZERO_INT

#define ti_TA_VC_MIN(a)                  t_tv_vc_min((a), TY_INTEGER, C_TVECTOR)
#define tr_TA_VC_MIN(a)                  t_tvr_vc_min((a), TY_REAL, C_TVECTOR)
#define tb_TA_VC_MIN(a)                  t_tv_vc_min((a), TY_BOOL, C_TVECTOR)
#define vi_TA_VC_MIN(a)                  t_tv_vc_min((a), TY_INTEGER, C_VECTOR)
#define vr_TA_VC_MIN(a)                  t_tvr_vc_min((a), TY_REAL, C_VECTOR)
#define vb_TA_VC_MIN(a)                  t_tv_vc_min((a), TY_BOOL, C_VECTOR)

#define ve_TA_VC_PLUS(a)                  ZERO_INT
#define te_TA_VC_PLUS(a)                  ZERO_INT

#define ti_TA_VC_PLUS(a)                  t_tv_vc_plus((a), TY_INTEGER, C_TVECTOR)
#define tr_TA_VC_PLUS(a)                  t_tvr_vc_plus((a), TY_REAL, C_TVECTOR)
#define tb_TA_VC_PLUS(a)                  t_tv_vc_plus((a), TY_BOOL, C_TVECTOR)
#define vi_TA_VC_PLUS(a)                  t_tv_vc_plus((a), TY_INTEGER, C_VECTOR)
#define vr_TA_VC_PLUS(a)                  t_tvr_vc_plus((a), TY_REAL, C_VECTOR)
#define vb_TA_VC_PLUS(a)                  t_tv_vc_plus((a), TY_BOOL, C_VECTOR)

#define ve_TA_VC_MINUS(a)                  ZERO_INT
#define te_TA_VC_MINUS(a)                  ZERO_INT

#define ti_TA_VC_MINUS(a)                  t_tv_vc_minus((a), TY_INTEGER, C_TVECTOR)
#define tr_TA_VC_MINUS(a)                  t_tvr_vc_minus((a), TY_REAL, C_TVECTOR)
#define tb_TA_VC_MINUS(a)                  t_tv_vc_minus((a), TY_BOOL, C_TVECTOR)
#define vi_TA_VC_MINUS(a)                  t_tv_vc_minus((a), TY_INTEGER, C_VECTOR)
#define vr_TA_VC_MINUS(a)                  t_tvr_vc_minus((a), TY_REAL, C_VECTOR)
#define vb_TA_VC_MINUS(a)                  t_tv_vc_minus((a), TY_BOOL, C_VECTOR)

#define ve_TA_VC_DIV(a)                  ZERO_INT
#define te_TA_VC_DIV(a)                  ZERO_INT

#define ti_TA_VC_DIV(a)                  t_tv_vc_div((a), TY_INTEGER, C_TVECTOR)
#define tr_TA_VC_DIV(a)                  t_tvr_vc_div((a), TY_REAL, C_TVECTOR)
#define tb_TA_VC_DIV(a)                  t_tv_vc_div((a), TY_BOOL, C_TVECTOR)
#define vi_TA_VC_DIV(a)                  t_tv_vc_div((a), TY_INTEGER, C_VECTOR)
#define vr_TA_VC_DIV(a)                  t_tvr_vc_div((a), TY_REAL, C_VECTOR)
#define vb_TA_VC_DIV(a)                  t_tv_vc_div((a), TY_BOOL, C_VECTOR)

#define ve_TA_VC_MULT(a)                  ZERO_INT
#define te_TA_VC_MULT(a)                  ZERO_INT

#define ti_TA_VC_MULT(a)                  t_tv_vc_mult((a), TY_INTEGER, C_TVECTOR)
#define tr_TA_VC_MULT(a)                  t_tvr_vc_mult((a), TY_REAL, C_TVECTOR)
#define tb_TA_VC_MULT(a)                  t_tv_vc_mult((a), TY_BOOL, C_TVECTOR)
#define vi_TA_VC_MULT(a)                  t_tv_vc_mult((a), TY_INTEGER, C_VECTOR)
#define vr_TA_VC_MULT(a)                  t_tvr_vc_mult((a), TY_REAL, C_VECTOR)
#define vb_TA_VC_MULT(a)                  t_tv_vc_mult((a), TY_BOOL, C_VECTOR)

#define me_TA_ABS(a)                     (INC_REFCNT(_nilmat), _nilmat)
#define ve_TA_ABS(a)                     (INC_REFCNT(_nilvect), _nilvect)
#define te_TA_ABS(a)                     (INC_REFCNT(_niltvect), _niltvect)

#define mi_TA_ABS(a)                     t_mvt_abs((a), C_MATRIX, TY_INTEGER)
#define mr_TA_ABS(a)                     t_mvt_abs((a), C_MATRIX, TY_REAL)
#define vi_TA_ABS(a)                     t_mvt_abs((a), C_VECTOR, TY_INTEGER)
#define vr_TA_ABS(a)                     t_mvt_abs((a), C_VECTOR, TY_REAL)
#define ti_TA_ABS(a)                     t_mvt_abs((a), C_TVECTOR, TY_INTEGER)
#define tr_TA_ABS(a)                     t_mvt_abs((a), C_TVECTOR, TY_REAL)

/*************/                                         
/* ARITY = 3 */
/*************/                                         
#define ixixl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixrxl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixlxl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixsxl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixvxl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixmixl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixvixl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixvixl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixmrxl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixvrxl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixvrxl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixmbxl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixvbxl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixvbxl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixbxl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixfxl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define ixcxl_TA_LREPLACE(a, b, c)      t_lreplace(VAL_INT((a)), (T_PTD)(b), (T_PTD)(c))
#define rxixl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxrxl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxlxl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxsxl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxtixl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxtexl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxvixl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxvexl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxmixl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxmexl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxmrxl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxtrxl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxvrxl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxmbxl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxtbxl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxvbxl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxbxl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxfxl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxcxl_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define ixsxs_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))
#define rxsxs_TA_LREPLACE(a, b, c)      \
        (((double)(int)R_SCALAR(DESC((a)), valr) == R_SCALAR(DESC((a)), valr)) \
        ? t_lreplace((int)R_SCALAR(DESC((a)), valr), (b), (c)) \
        : controlled_exit("index has fracture <> 0"))

/**********************************************/
/* (T)VECTOR X (T)VECTOR => (T)VECTOR         */
/**********************************************/
#define vixve_TA_VUNI(a, b)        t_vv_unite_empty((a))
#define vbxve_TA_VUNI(a, b)        t_vv_unite_empty((a))
#define vrxve_TA_VUNI(a, b)        t_vv_unite_empty((a))
#define vexvi_TA_VUNI(a, b)        t_vv_unite_empty((b))
#define vexvb_TA_VUNI(a, b)        t_vv_unite_empty((b))
#define vexvr_TA_VUNI(a, b)        t_vv_unite_empty((b))
#define vexve_TA_VUNI(a, b)        (INC_REFCNT(_nilvect), _nilvect)

#define tixte_TA_VUNI(a, b)        t_vv_unite_empty((a))           
#define tbxte_TA_VUNI(a, b)        t_vv_unite_empty((a))           
#define trxte_TA_VUNI(a, b)        t_vv_unite_empty((a))           
#define texti_TA_VUNI(a, b)        t_vv_unite_empty((b))           
#define textb_TA_VUNI(a, b)        t_vv_unite_empty((b))           
#define textr_TA_VUNI(a, b)        t_vv_unite_empty((b))           
#define texte_TA_VUNI(a, b)        (INC_REFCNT(_niltvect), _niltvect)

#define vixvi_TA_VUNI(a, b)        t_vv_unite((a), (b), C_VECTOR, TY_INTEGER)
#define vbxvb_TA_VUNI(a, b)        t_vv_unite((a), (b), C_VECTOR, TY_BOOL)
#define vrxvr_TA_VUNI(a, b)        t_vv_unite_rr((a), (b), C_VECTOR, TY_REAL)
#define vrxvi_TA_VUNI(a, b)        t_vv_unite_ri((a), (b), C_VECTOR, TY_REAL)
#define vixvr_TA_VUNI(a, b)        t_vv_unite_ir((a), (b), C_VECTOR, TY_REAL)
#define tixti_TA_VUNI(a, b)        t_vv_unite((a), (b), C_TVECTOR, TY_INTEGER)
#define tbxtb_TA_VUNI(a, b)        t_vv_unite((a), (b), C_TVECTOR, TY_BOOL)
#define trxtr_TA_VUNI(a, b)        t_vv_unite_rr((a), (b), C_TVECTOR, TY_REAL)
#define trxti_TA_VUNI(a, b)        t_vv_unite_ri((a), (b), C_TVECTOR, TY_REAL)
#define tixtr_TA_VUNI(a, b)        t_vv_unite_ir((a), (b), C_TVECTOR, TY_REAL)

#define ixixvi_TA_VREPLACE(a, b, c) \
        t_v_replace(VAL_INT(a), VAL_INT(b), (c), C_VECTOR, TY_INTEGER)
#define rxixvi_TA_VREPLACE(a, b, c) \
        t_v_replace((int)R_SCALAR(DESC(a), valr), VAL_INT(b), (c), C_VECTOR, TY_INTEGER)

#define ixrxvr_TA_VREPLACE(a, b, c)  \
        t_vr_replace(VAL_INT(a), R_SCALAR(DESC(b), valr), (c), C_VECTOR, TY_REAL)
#define rxrxvr_TA_VREPLACE(a, b, c) \
        t_vr_replace((int)R_SCALAR(DESC(a), valr),  \
        R_SCALAR(DESC(b), valr), (c), C_VECTOR, TY_REAL)

#define ixbxvb_TA_VREPLACE(a, b, c) t_v_replace(VAL_INT(a), (b), (c), C_VECTOR, TY_BOOL)
#define rxbxvb_TA_VREPLACE(a, b, c) \
        t_v_replace((int)R_SCALAR(DESC(a), valr), (b), (c), C_VECTOR, TY_BOOL)

#define ixixti_TA_VREPLACE(a, b, c) \
        t_v_replace(VAL_INT(a), VAL_INT(b), (c), C_TVECTOR, TY_INTEGER)
#define rxixti_TA_VREPLACE(a, b, c) \
        t_v_replace((int)R_SCALAR(DESC(a), valr), VAL_INT(b), (c), C_TVECTOR, TY_INTEGER)

#define ixrxtr_TA_VREPLACE(a, b, c) \
        t_vr_replace(VAL_INT(a), R_SCALAR(DESC(b), valr), (c), C_TVECTOR, TY_REAL)
#define rxrxtr_TA_VREPLACE(a, b, c) \
        t_vr_replace((int)R_SCALAR(DESC(a), valr), \
        R_SCALAR(DESC(b), valr), (c), C_TVECTOR, TY_REAL)

#define ixbxtb_TA_VREPLACE(a, b, c) t_v_replace(VAL_INT(a), (b), (c), C_TVECTOR, TY_BOOL)
#define rxbxtb_TA_VREPLACE(a, b, c) \
        t_v_replace((int)R_SCALAR(DESC(a), valr), (b), (c), C_TVECTOR, TY_BOOL)

/***************************************************/
/* Opreations on MATRICES                          */
/***************************************************/
#define ixixme_TA_MCUT(a, b, c)   t_mcut(VAL_INT(a), VAL_INT(b), (c), TY_INTEGER)
#define rxixme_TA_MCUT(a, b, c)   t_mcut((int)R_SCALAR(DESC(a), valr), \
                                          VAL_INT(b), (c), TY_INTEGER)
#define ixrxme_TA_MCUT(a, b, c)   t_mcut(VAL_INT(a), (int)R_SCALAR(DESC(b), valr), (c), \
                                          TY_INTEGER)
#define rxrxme_TA_MCUT(a, b, c)   t_mcut((int)R_SCALAR(DESC(a),  \
                                         (int)R_SCALAR(DESC(b), valr), (c), TY_INTEGER)

#define ixixmi_TA_MCUT(a, b, c)   t_mcut(VAL_INT(a), VAL_INT(b), (c), TY_INTEGER)
#define ixixmb_TA_MCUT(a, b, c)   t_mcut(VAL_INT(a), VAL_INT(b), (c), TY_BOOL)
#define rxixmi_TA_MCUT(a, b, c)   t_mcut((int)R_SCALAR(DESC(a), valr), \
                                          VAL_INT(b), (c), TY_INTEGER)
#define rxixmb_TA_MCUT(a, b, c)   t_mcut((int)R_SCALAR(DESC(a), valr), \
                                          VAL_INT(b), (c), TY_BOOL)
#define ixrxmi_TA_MCUT(a, b, c)   t_mcut(VAL_INT(a), (int)R_SCALAR(DESC(b), valr), (c), \
                                          TY_INTEGER)
#define ixrxmb_TA_MCUT(a, b, c)   t_mcut(VAL_INT(a), (int)R_SCALAR(DESC(b), valr), (c), \
                                          TY_BOOL)
#define rxrxmi_TA_MCUT(a, b, c)   t_mcut((int)R_SCALAR(DESC(a),  \
                                         (int)R_SCALAR(DESC(b), valr), (c), TY_INTEGER)
#define rxrxmb_TA_MCUT(a, b, c)   t_mcut((int)R_SCALAR(DESC(a), valr),  \
                                         (int)R_SCALAR(DESC(b), valr), (c), TY_BOOL)

#define ixixmr_TA_MCUT(a, b, c)   t_mcutr(VAL_INT(a), VAL_INT(b), (c), TY_REAL)
#define rxixmr_TA_MCUT(a, b, c)   t_mcutr((int)R_SCALAR(DESC(a), valr), \
                                          VAL_INT(b), (c), TY_REAL)
#define ixrxmr_TA_MCUT(a, b, c)   t_mcutr(VAL_INT(a), (int)R_SCALAR(DESC(b), valr), (c), \
                                          TY_REAL)
#define rxrxmr_TA_MCUT(a, b, c)   t_mcutr((int)R_SCALAR(DESC(a),  \
                                         (int)R_SCALAR(DESC(b), valr), (c), TY_REAL)


#define me_TA_TRANSFORM(a)        (INC_REFCNT(_nil), _nil)
#define ve_TA_TRANSFORM(a)        (INC_REFCNT(_nil), _nil)
#define te_TA_TRANSFORM(a)        (INC_REFCNT(_nil), _nil)

#define mi_TA_TRANSFORM(a)        t_transform((a), C_MATRIX, TY_INTEGER)
#define mb_TA_TRANSFORM(a)        t_transform((a), C_MATRIX, TY_BOOL)
#define mr_TA_TRANSFORM(a)        t_transformr((a), C_MATRIX)
#define vi_TA_TRANSFORM(a)        t_transform((a), C_VECTOR, TY_INTEGER)
#define vb_TA_TRANSFORM(a)        t_transform((a), C_VECTOR, TY_BOOL)
#define vr_TA_TRANSFORM(a)        t_transformr((a), C_VECTOR)
#define ti_TA_TRANSFORM(a)        t_transform((a), C_TVECTOR, TY_INTEGER)
#define tb_TA_TRANSFORM(a)        t_transform((a), C_TVECTOR, TY_BOOL)
#define tr_TA_TRANSFORM(a)        t_transformr((a), C_TVECTOR)

#define me_TA_TRANSPOSE(a)        (INC_REFCNT(_nil), _nil)
#define ve_TA_TRANSPOSE(a)        (INC_REFCNT(_nilvect), _nilvect)
#define te_TA_TRANSPOSE(a)        (INC_REFCNT(_niltvect), _niltvect)

#define vi_TA_TRANSPOSE(a)        t_transpose_vt((a), C_VECTOR, TY_INTEGER)
#define vb_TA_TRANSPOSE(a)        t_transpose_vt((a), C_VECTOR, TY_BOOL)
#define vr_TA_TRANSPOSE(a)        t_transpose_vtr((a), C_VECTOR, TY_REAL)
#define ti_TA_TRANSPOSE(a)        t_transpose_vt((a), C_TVECTOR, TY_INTEGER)
#define tb_TA_TRANSPOSE(a)        t_transpose_vt((a), C_TVECTOR, TY_BOOL)
#define tr_TA_TRANSPOSE(a)        t_transpose_vtr((a), C_TVECTOR, TY_REAL)

#define mi_TA_TRANSPOSE(a)        t_transpose_m((a), TY_INTEGER)
#define mb_TA_TRANSPOSE(a)        t_transpose_m((a), TY_BOOL)
#define mr_TA_TRANSPOSE(a)        t_transpose_mr((a), TY_REAL)

#define ixme_TA_MDIMENSION(a, b)        ZERO_INT
#define rxme_TA_MDIMENSION(a, b)        ZERO_INT

#define ixmi_TA_MDIMENSION(a, b)        t_mdim(VAL_INT(a), (b))
#define ixmb_TA_MDIMENSION(a, b)        t_mdim(VAL_INT(a), (b))
#define ixmr_TA_MDIMENSION(a, b)        t_mdim(VAL_INT(a), (b))
#define rxmi_TA_MDIMENSION(a, b)        t_mdim(R_SCALAR(DESC(a), valr), (b))
#define rxmb_TA_MDIMENSION(a, b)        t_mdim(R_SCALAR(DESC(a), (b))
#define rxmr_TA_MDIMENSION(a, b)        t_mdim(R_SCALAR(DESC(a), (b))

#define ixvixmi_TA_MREPLACE_R(a, b, c)      t_mreplace_r(VAL_INT(a), (b), (c), TY_INTEGER)
#define ixvbxmb_TA_MREPLACE_R(a, b, c)      t_mreplace_r(VAL_INT(a), (b), (c), TY_BOOL)
#define ixvrxmr_TA_MREPLACE_R(a, b, c)      t_mreplace_rr(VAL_INT(a), (b), (c), TY_REAL)
#define rxvixmi_TA_MREPLACE_R(a, b, c)      t_mreplace_r((int)R_SCALAR(DESC(a), valr), (b), (c), TY_INTEGER)
#define rxvbxmb_TA_MREPLACE_R(a, b, c)      t_mreplace_r((int)R_SCALAR(DESC(a), valr), (b), (c), TY_BOOL)
#define rxvrxmr_TA_MREPLACE_R(a, b, c)      t_mreplace_rr((int)R_SCALAR(DESC(a), valr), (b), (c), TY_REAL)

#define ixtixmi_TA_MREPLACE_C(a, b, c)      t_mreplace_c(VAL_INT(a), (b), (c), TY_INTEGER)
#define ixtbxmb_TA_MREPLACE_C(a, b, c)      t_mreplace_c(VAL_INT(a), (b), (c), TY_BOOL)
#define ixtrxmr_TA_MREPLACE_C(a, b, c)      t_mreplace_cr(VAL_INT(a), (b), (c), TY_REAL)
#define rxtixmi_TA_MREPLACE_C(a, b, c)      t_mreplace_c((int)R_SCALAR(DESC(a), valr), (b), (c), TY_INTEGER)
#define rxtbxmb_TA_MREPLACE_C(a, b, c)      t_mreplace_c((int)R_SCALAR(DESC(a), valr), (b), (c), TY_BOOL)
#define rxtrxmr_TA_MREPLACE_C(a, b, c)      t_mreplace_cr((int)R_SCALAR(DESC(a), valr), (b), (c), TY_REAL)

#define ixixme_TA_MROTATE(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define rxixme_TA_MROTATE(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define ixrxme_TA_MROTATE(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define rxrxme_TA_MROTATE(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)

#define ixixmi_TA_MROTATE(a, b, c)          t_mrotate(VAL_INT(a), VAL_INT(b), c, TY_INTEGER)
#define ixixmb_TA_MROTATE(a, b, c)          t_mrotate(VAL_INT(a), VAL_INT(b), c, TY_BOOL)
#define ixixmr_TA_MROTATE(a, b, c)          t_mrotate_r(VAL_INT(a), VAL_INT(b), c, TY_REAL)
#define rxixmi_TA_MROTATE(a, b, c)          t_mrotate((int)R_SCALAR(DESC(a), valr), VAL_INT(b), c, TY_INTEGER)
#define rxixmb_TA_MROTATE(a, b, c)          t_mrotate((int)R_SCALAR(DESC(a), valr), VAL_INT(b), c, TY_BOOL)
#define rxixmr_TA_MROTATE(a, b, c)          t_mrotate_r((int)R_SCALAR(DESC(a), valr), VAL_INT(b), c, TY_REAL)
#define ixrxmi_TA_MROTATE(a, b, c)          t_mrotate(VAL_INT(a), (int)R_SCALAR(DESC(b), valr), c, TY_INTEGER)
#define ixrxmb_TA_MROTATE(a, b, c)          t_mrotate(VAL_INT(a), (int)R_SCALAR(DESC(b), valr), c, TY_BOOL)
#define ixrxmr_TA_MROTATE(a, b, c)          t_mrotate_r(VAL_INT(a), (int)R_SCALAR(DESC(b), valr) c, TY_REAL)
#define rxrxmi_TA_MROTATE(a, b, c)          t_mrotate((int)R_SCALAR(DESC(a), valr), (int)R_SCALAR(DESC(b), valr), c, TY_INTEGER)
#define rxrxmb_TA_MROTATE(a, b, c)          t_mrotate((int)R_SCALAR(DESC(a), valr), (int)R_SCALAR(DESC(b), valr), c, TY_BOOL)
#define rxrxmr_TA_MROTATE(a, b, c)          t_mrotate_r((int)R_SCALAR(DESC(a), valr), (int)R_SCALAR(DESC(b), valr), c, TY_REAL)

#define ixixixmi_TA_MREPLACE(a, b, c, d)    t_mreplace(VAL_INT(a), VAL_INT(b), \
                                             VAL_INT(c), (d), TY_INTEGER)
#define rxixixmi_TA_MREPLACE(a, b, c, d)    t_mreplace((int)R_SCALAR(DESC(a), valr), \
                                             VAL_INT(b), VAL_INT(c), (d), TY_INTEGER)
#define ixrxixmi_TA_MREPLACE(a, b, c, d)    t_mreplace(VAL_INT(a), \
                              (int)R_SCALAR(DESC(b), valr), VAL_INT(c), (d), TY_INTEGER)
#define rxrxixmi_TA_MREPLACE(a, b, c, d)    t_mreplace((int)R_SCALAR(DESC(a), valr), \
                                             (int)R_SCALAR(DESC(b), valr), VAL_INT(c), \
                                             (d), TY_INTEGER)
#define ixixbxmb_TA_MREPLACE(a, b, c, d)    t_mreplace(VAL_INT(a), VAL_INT(b), \
                                             VAL_INT(c), (d), TY_BOOL)
#define rxixbxmb_TA_MREPLACE(a, b, c, d)    t_mreplace((int)R_SCALAR(DESC(a), valr), \
                                             VAL_INT(b), VAL_INT(c), (d), TY_BOOL)
#define ixrxbxmb_TA_MREPLACE(a, b, c, d)    t_mreplace(VAL_INT(a), (int)R_SCALAR(DESC(b), valr), \
                                             VAL_INT(c), (d), TY_BOOL)
#define rxrxbxmb_TA_MREPLACE(a, b, c, d)    t_mreplace((int)R_SCALAR(DESC(a), valr), \
                                             (int)R_SCALAR(DESC(b), valr), VAL_INT(c), \
                                             (d), TY_BOOL)
#define ixixrxmr_TA_MREPLACE(a, b, c, d)    t_mreplace_real(VAL_INT(a), VAL_INT(b), \
                                             R_SCALAR(DESC(c), valr), (d), TY_REAL)
#define rxixrxmr_TA_MREPLACE(a, b, c, d)    t_mreplace_real((int)R_SCALAR(DESC(a), valr), \
                                     VAL_INT(b), R_SCALAR(DESC(c), valr), (d), TY_REAL)
#define ixrxrxmr_TA_MREPLACE(a, b, c, d)    t_mreplace_real(VAL_INT(a),   \
                    (int)R_SCALAR(DESC(b), valr), R_SCALAR(DESC(c), valr), (d), TY_REAL)
#define rxrxrxmr_TA_MREPLACE(a, b, c, d)    t_mreplace_real((int)R_SCALAR(DESC(a), valr), \
                                             (int)R_SCALAR(DESC(b), valr), \
                                             R_SCALAR(DESC(c), valr), (d), TY_REAL)

#define me_TA_NOT(a)                         (INC_REFCNT(_nilmat), _nilmat)
#define ve_TA_NOT(a)                         (INC_REFCNT(_nilvect), _nilvect)
#define te_TA_NOT(a)                         (INC_REFCNT(_niltvect), _niltvect)

#define mb_TA_NOT(a)                         t_not_bmat((a), C_MATRIX)
#define vb_TA_NOT(a)                         t_not_bmat((a), C_VECTOR)
#define tb_TA_NOT(a)                         t_not_bmat((a), C_TVECTOR)

#define mbxmb_TA_AND(a, b)                   t_and_bmat((a), (b), C_MATRIX)
#define mbxmb_TA_XOR(a, b)                   t_xor_bmat((a), (b), C_MATRIX)
#define mbxmb_TA_OR(a, b)                    t_or_bmat((a), (b), C_MATRIX)
#define mbxvb_TA_AND(a, b)                   t_and_bmat((a), (b), C_VECTOR)
#define mbxvb_TA_XOR(a, b)                   t_xor_bmat((a), (b), C_VECTOR)
#define mbxvb_TA_OR(a, b)                    t_or_bmat((a), (b), C_VECTOR)
#define mbxtb_TA_AND(a, b)                   t_and_bmat((a), (b), C_TVECTOR)
#define mbxtb_TA_XOR(a, b)                   t_xor_bmat((a), (b), C_TVECTOR)
#define mbxtb_TA_OR(a, b)                    t_or_bmat((a), (b), C_TVECTOR)

#define bxmb_TA_AND(a, b)                   t_b_and_bmat((a), (b), C_MATRIX)
#define bxmb_TA_XOR(a, b)                   t_b_xor_bmat((a), (b), C_MATRIX)
#define bxmb_TA_OR(a, b)                    t_b_or_bmat((a), (b), C_MATRIX)
#define bxvb_TA_AND(a, b)                   t_b_and_bmat((a), (b), C_VECTOR)
#define bxvb_TA_XOR(a, b)                   t_b_xor_bmat((a), (b), C_VECTOR)
#define bxvb_TA_OR(a, b)                    t_b_or_bmat((a), (b), C_VECTOR)
#define bxtb_TA_AND(a, b)                   t_b_and_bmat((a), (b), C_TVECTOR)
#define bxtb_TA_XOR(a, b)                   t_b_xor_bmat((a), (b), C_TVECTOR)
#define bxtb_TA_OR(a, b)                    t_b_or_bmat((a), (b), C_TVECTOR)

#define bmxb_TA_AND(a, b)                   t_b_and_bmat((b), (a), C_MATRIX)
#define bmxb_TA_XOR(a, b)                   t_b_xor_bmat((b), (a), C_MATRIX)
#define bmxb_TA_OR(a, b)                    t_b_or_bmat((b), (a), C_MATRIX)
#define bvxb_TA_AND(a, b)                   t_b_and_bmat((b), (a), C_VECTOR)
#define bvxb_TA_XOR(a, b)                   t_b_xor_bmat((b), (a), C_VECTOR)
#define bvxb_TA_OR(a, b)                    t_b_or_bmat((b), (a), C_VECTOR)
#define btxb_TA_AND(a, b)                   t_b_and_bmat((b), (a), C_TVECTOR)
#define btxb_TA_XOR(a, b)                   t_b_xor_bmat((b), (a), C_TVECTOR)
#define btxb_TA_OR(a, b)                    t_b_or_bmat((b), (a), C_TVECTOR)

#define mixmi_TA_MIN(a, b)                   t_min_mixmi((a), (b), C_MATRIX, TY_INTEGER)
#define mrxmi_TA_MIN(a, b)                   t_min_mrxmi((a), (b), C_MATRIX, TY_REAL)
#define mixmr_TA_MIN(a, b)                   t_min_mrxmi((b), (a), C_MATRIX, TY_REAL)
#define mrxmr_TA_MIN(a, b)                   t_min_mrxmr((a), (b), C_MATRIX, TY_REAL)
#define mbxmb_TA_MIN(a, b)                   t_min_mixmi((a), (b), C_MATRIX, TY_BOOL)
#define vixvi_TA_MIN(a, b)                   t_min_mixmi((a), (b), C_VECTOR, TY_INTEGER)
#define vrxvi_TA_MIN(a, b)                   t_min_mrxmi((a), (b), C_VECTOR, TY_REAL)
#define vixvr_TA_MIN(a, b)                   t_min_mrxmi((b), (a), C_VECTOR, TY_REAL)
#define vrxvr_TA_MIN(a, b)                   t_min_mrxmr((a), (b), C_VECTOR, TY_REAL)
#define vbxvb_TA_MIN(a, b)                   t_min_mixmi((a), (b), C_VECTOR, TY_BOOL)
#define tixti_TA_MIN(a, b)                   t_min_mixmi((a), (b), C_TVECTOR, TY_INTEGER)
#define trxti_TA_MIN(a, b)                   t_min_mrxmi((a), (b), C_TVECTOR, TY_REAL)
#define tixtr_TA_MIN(a, b)                   t_min_mrxmi((b), (a), C_TVECTOR, TY_REAL)
#define trxtr_TA_MIN(a, b)                   t_min_mrxmr((a), (b), C_TVECTOR, TY_REAL)
#define tbxtb_TA_MIN(a, b)                   t_min_mixmi((a), (b), C_TVECTOR, TY_BOOL)


#define mixmi_TA_MAX(a, b)                   t_max_mixmi((a), (b), C_MATRIX, TY_INTEGER)
#define mrxmi_TA_MAX(a, b)                   t_max_mrxmi((a), (b), C_MATRIX, TY_REAL)
#define mixmr_TA_MAX(a, b)                   t_max_mrxmi((b), (a), C_MATRIX, TY_REAL)
#define mrxmr_TA_MAX(a, b)                   t_max_mrxmr((a), (b), C_MATRIX, TY_REAL)
#define mbxmb_TA_MAX(a, b)                   t_max_mixmi((a), (b), C_MATRIX, TY_BOOL)
#define vixvi_TA_MAX(a, b)                   t_max_mixmi((a), (b), C_VECTOR, TY_INTEGER)
#define vrxvi_TA_MAX(a, b)                   t_max_mrxmi((a), (b), C_VECTOR, TY_REAL)
#define vixvr_TA_MAX(a, b)                   t_max_mrxmi((b), (a), C_VECTOR, TY_REAL)
#define vrxvr_TA_MAX(a, b)                   t_max_mrxmr((a), (b), C_VECTOR, TY_REAL)
#define vbxvb_TA_MAX(a, b)                   t_max_mixmi((a), (b), C_VECTOR, TY_BOOL)
#define tixti_TA_MAX(a, b)                   t_max_mixmi((a), (b), C_TVECTOR, TY_INTEGER)
#define trxti_TA_MAX(a, b)                   t_max_mrxmi((a), (b), C_TVECTOR, TY_REAL)
#define tixtr_TA_MAX(a, b)                   t_max_mrxmi((b), (a), C_TVECTOR, TY_REAL)
#define trxtr_TA_MAX(a, b)                   t_max_mrxmr((a), (b), C_TVECTOR, TY_REAL)
#define tbxtb_TA_MAX(a, b)                   t_max_mixmi((a), (b), C_TVECTOR, TY_BOOL)

#define i_TA_TO_MAT(a)                       t_to_mat(VAL_INT(a), TY_INTEGER)
#define b_TA_TO_MAT(a)                       t_to_mat((a), TY_BOOL)
#define r_TA_TO_MAT(a)                       t_r_to_mat(R_SCALAR(DESC(a), valr), TY_REAL)
#define ve_TA_TO_MAT(a)                      (INC_REFCNT(_nilmat), _nilmat)
#define te_TA_TO_MAT(a)                      (INC_REFCNT(_nilmat), _nilmat)
#define vi_TA_TO_MAT(a)                      t_v_to_mat((a), C_VECTOR, TY_INTEGER)
#define vb_TA_TO_MAT(a)                      t_v_to_mat((a), C_VECTOR, TY_BOOL)
#define vr_TA_TO_MAT(a)                      t_vr_to_mat((a), C_VECTOR, TY_REAL)
#define ti_TA_TO_MAT(a)                      t_v_to_mat((a), C_TVECTOR, TY_INTEGER)
#define tb_TA_TO_MAT(a)                      t_v_to_mat((a), C_TVECTOR, TY_BOOL)
#define tr_TA_TO_MAT(a)                      t_vr_to_mat((a), C_TVECTOR, TY_REAL)


#define ixmixme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixmrxme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixmbxme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixvixme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixvrxme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixvbxme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixtixme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixtrxme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixtbxme_TA_MUNI(a, b, c)          t_munite_empty((a))


#define rxmixme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxmrxme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxmbxme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxvixme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxvrxme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxvbxme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxtixme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxtrxme_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxtbxme_TA_MUNI(a, b, c)          t_munite_empty((a))


#define ixmexmi_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixmexmb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixmexmr_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixmexvi_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixmexvb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixmexvr_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixmexti_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixmextb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixmextr_TA_MUNI(a, b, c)          t_munite_empty((b))

#define rxmexmi_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxmexmb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxmexmr_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxmexvi_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxmexvb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxmexvr_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxmexti_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxmextb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxmextr_TA_MUNI(a, b, c)          t_munite_empty((b))


#define ixmixve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixmrxve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixmbxve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixvixve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixvrxve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixvbxve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixtixve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixtrxve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixtbxve_TA_MUNI(a, b, c)          t_munite_empty((a))


#define rxmixve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxmrxve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxmbxve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxvixve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxvrxve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxvbxve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxtixve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxtrxve_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxtbxve_TA_MUNI(a, b, c)          t_munite_empty((a))


#define ixvexmi_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixvexmb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixvexmr_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixvexvi_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixvexvb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixvexvr_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixvexti_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixvextb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixvextr_TA_MUNI(a, b, c)          t_munite_empty((b))

#define rxvexmi_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxvexmb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxvexmr_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxvexvi_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxvexvb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxvexvr_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxvexti_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxvextb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxvextr_TA_MUNI(a, b, c)          t_munite_empty((b))


#define ixmixte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixmrxte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixmbxte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixvixte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixvrxte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixvbxte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixtixte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixtrxte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define ixtbxte_TA_MUNI(a, b, c)          t_munite_empty((a))


#define rxmixte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxmrxte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxmbxte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxvixte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxvrxte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxvbxte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxtixte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxtrxte_TA_MUNI(a, b, c)          t_munite_empty((a))
#define rxtbxte_TA_MUNI(a, b, c)          t_munite_empty((a))


#define ixtexmi_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixtexmb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixtexmr_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixtexvi_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixtexvb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixtexvr_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixtexti_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixtextb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define ixtextr_TA_MUNI(a, b, c)          t_munite_empty((b))

#define rxtexmi_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxtexmb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxtexmr_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxtexvi_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxtexvb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxtexvr_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxtexti_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxtextb_TA_MUNI(a, b, c)          t_munite_empty((b))
#define rxtextr_TA_MUNI(a, b, c)          t_munite_empty((b))



#define ixmexme_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define rxmexme_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define ixmexve_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define rxmexte_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define ixvexme_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define rxtexme_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)

#define ixvexve_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define rxvexve_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define ixvexte_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define rxvexte_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define ixtexve_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define rxtexve_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)

#define ixtexte_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)
#define rxtexte_TA_MUNI(a, b, c)          (INC_REFCNT(_nilmat), _nilmat)


#define ixmixmi_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_MATRIX, TY_INTEGER)
#define rxmixmi_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                      (c), C_MATRIX, C_MATRIX, TY_INTEGER) 
#define ixmrxmi_TA_MUNI(a, b, c)           t_munite_ri(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_MATRIX, TY_REAL)
#define rxmrxmi_TA_MUNI(a, b, c)           t_munite_ri((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_MATRIX, C_MATRIX,  TY_REAL) 
#define ixmixmr_TA_MUNI(a, b, c)           t_munite_ir(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_MATRIX, TY_REAL)
#define rxmixmr_TA_MUNI(a, b, c)           t_munite_ir((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_MATRIX, C_MATRIX, TY_REAL) 
#define ixmrxmr_TA_MUNI(a, b, c)           t_munite_rr(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_MATRIX, TY_REAL)
#define rxmrxmr_TA_MUNI(a, b, c)           t_munite_rr((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_MATRIX, C_MATRIX, TY_REAL) 
#define ixmbxmb_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_MATRIX, TY_BOOL)
#define rxmbxmb_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_MATRIX, C_MATRIX, TY_BOOL) 


#define ixvixmi_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_VECTOR, \
                                                        C_MATRIX, TY_INTEGER)
#define rxvixmi_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                      (c), C_VECTOR, C_MATRIX, TY_INTEGER) 
#define ixvrxmi_TA_MUNI(a, b, c)           t_munite_ri(VAL_INT(a), (b), (c), C_VECTOR, \
                                                        C_MATRIX, TY_REAL)
#define rxvrxmi_TA_MUNI(a, b, c)           t_munite_ri((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_VECTOR, C_MATRIX,  TY_REAL) 
#define ixvixmr_TA_MUNI(a, b, c)           t_munite_ir(VAL_INT(a), (b), (c), C_VECTOR, \
                                                        C_MATRIX, TY_REAL)
#define rxvixmr_TA_MUNI(a, b, c)           t_munite_ir((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_VECTOR, C_MATRIX, TY_REAL) 
#define ixvrxmr_TA_MUNI(a, b, c)           t_munite_rr(VAL_INT(a), (b), (c), C_VECTOR, \
                                                        C_MATRIX, TY_REAL)
#define rxvrxmr_TA_MUNI(a, b, c)           t_munite_rr((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_VECTOR, C_MATRIX, TY_REAL) 
#define ixvbxmb_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_VECTOR, \
                                                        C_MATRIX, TY_BOOL)
#define rxvbxmb_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_VECTOR, C_MATRIX, TY_BOOL) 



#define ixmixvi_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_VECTOR, TY_INTEGER)
#define rxmixvi_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                      (c), C_MATRIX, C_VECTOR, TY_INTEGER) 
#define ixmrxvi_TA_MUNI(a, b, c)           t_munite_ri(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_VECTOR, TY_REAL)
#define rxmrxvi_TA_MUNI(a, b, c)           t_munite_ri((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_MATRIX, C_VECTOR,  TY_REAL) 
#define ixmixvr_TA_MUNI(a, b, c)           t_munite_ir(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_VECTOR, TY_REAL)
#define rxmixvr_TA_MUNI(a, b, c)           t_munite_ir((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_MATRIX, C_VECTOR, TY_REAL) 
#define ixmrxvr_TA_MUNI(a, b, c)           t_munite_rr(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_VECTOR, TY_REAL)
#define rxmrxvr_TA_MUNI(a, b, c)           t_munite_rr((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_MATRIX, C_VECTOR, TY_REAL) 
#define ixmbxvb_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_VECTOR, TY_BOOL)
#define rxmbxvb_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_MATRIX, C_VECTOR, TY_BOOL) 



#define ixvixvi_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_VECTOR, \
                                                        C_VECTOR, TY_INTEGER)
#define rxvixvi_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                      (c), C_VECTOR, C_VECTOR, TY_INTEGER) 
#define ixvrxvi_TA_MUNI(a, b, c)           t_munite_ri(VAL_INT(a), (b), (c), C_VECTOR, \
                                                        C_VECTOR, TY_REAL)
#define rxvrxvi_TA_MUNI(a, b, c)           t_munite_ri((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_VECTOR, C_VECTOR,  TY_REAL) 
#define ixvixvr_TA_MUNI(a, b, c)           t_munite_ir(VAL_INT(a), (b), (c), C_VECTOR, \
                                                        C_VECTOR, TY_REAL)
#define rxvixvr_TA_MUNI(a, b, c)           t_munite_ir((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_VECTOR, C_VECTOR, TY_REAL) 
#define ixvrxvr_TA_MUNI(a, b, c)           t_munite_rr(VAL_INT(a), (b), (c), C_VECTOR, \
                                                        C_VECTOR, TY_REAL)
#define rxvrxvr_TA_MUNI(a, b, c)           t_munite_rr((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_VECTOR, C_VECTOR, TY_REAL) 
#define ixvbxvb_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_VECTOR, \
                                                        C_VECTOR, TY_BOOL)
#define rxvbxvb_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_VECTOR, C_VECTOR, TY_BOOL) 


#define ixtixmi_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_TVECTOR, \
                                                        C_MATRIX, TY_INTEGER)
#define rxtixmi_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                   (c), C_TVECTOR, C_MATRIX, TY_INTEGER) 
#define ixtrxmi_TA_MUNI(a, b, c)           t_munite_ri(VAL_INT(a), (b), (c), C_TVECTOR, \
                                                        C_MATRIX, TY_REAL)
#define rxtrxmi_TA_MUNI(a, b, c)           t_munite_ri((int)R_SCALAR(DESC(a), valr), (b),\
                                                   (c), C_TVECTOR, C_MATRIX,  TY_REAL) 
#define ixtixmr_TA_MUNI(a, b, c)           t_munite_ir(VAL_INT(a), (b), (c), C_TVECTOR, \
                                                        C_MATRIX, TY_REAL)
#define rxtixmr_TA_MUNI(a, b, c)           t_munite_ir((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_TVECTOR, C_MATRIX, TY_REAL) 
#define ixtrxmr_TA_MUNI(a, b, c)           t_munite_rr(VAL_INT(a), (b), (c), C_TVECTOR, \
                                                        C_MATRIX, TY_REAL)
#define rxtrxmr_TA_MUNI(a, b, c)           t_munite_rr((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_TVECTOR, C_MATRIX, TY_REAL) 
#define ixtbxmb_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_TVECTOR, \
                                                        C_MATRIX, TY_BOOL)
#define rxtbxmb_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_TVECTOR, C_MATRIX, TY_BOOL) 



#define ixmixti_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_TVECTOR, TY_INTEGER)
#define rxmixti_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                    (c), C_MATRIX, C_TVECTOR, TY_INTEGER) 
#define ixmrxti_TA_MUNI(a, b, c)           t_munite_ri(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_TVECTOR, TY_REAL)
#define rxmrxti_TA_MUNI(a, b, c)           t_munite_ri((int)R_SCALAR(DESC(a), valr), (b),\
                                                      (c), C_MATRIX, C_TVECTOR,  TY_REAL) 
#define ixmixtr_TA_MUNI(a, b, c)           t_munite_ir(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_TVECTOR, TY_REAL)
#define rxmixtr_TA_MUNI(a, b, c)           t_munite_ir((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_MATRIX, C_TVECTOR, TY_REAL) 
#define ixmrxtr_TA_MUNI(a, b, c)           t_munite_rr(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_TVECTOR, TY_REAL)
#define rxmrxtr_TA_MUNI(a, b, c)           t_munite_rr((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_MATRIX, C_TVECTOR, TY_REAL) 
#define ixmbxtb_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_MATRIX, \
                                                        C_TVECTOR, TY_BOOL)
#define rxmbxtb_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_MATRIX, C_TVECTOR, TY_BOOL) 



#define ixtixti_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_TVECTOR, \
                                                        C_TVECTOR, TY_INTEGER)
#define rxtixti_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                    (c), C_TVECTOR, C_TVECTOR, TY_INTEGER) 
#define ixtrxti_TA_MUNI(a, b, c)           t_munite_ri(VAL_INT(a), (b), (c), C_TVECTOR, \
                                                        C_TVECTOR, TY_REAL)
#define rxtrxti_TA_MUNI(a, b, c)           t_munite_ri((int)R_SCALAR(DESC(a), valr), (b),\
                                                      (c), C_TVECTOR, C_TVECTOR,  TY_REAL) 
#define ixtixtr_TA_MUNI(a, b, c)           t_munite_ir(VAL_INT(a), (b), (c), C_TVECTOR, \
                                                        C_TVECTOR, TY_REAL)
#define rxtixtr_TA_MUNI(a, b, c)           t_munite_ir((int)R_SCALAR(DESC(a), valr), (b),\
                                                       (c), C_TVECTOR, C_TVECTOR, TY_REAL) 
#define ixtrxtr_TA_MUNI(a, b, c)           t_munite_rr(VAL_INT(a), (b), (c), C_TVECTOR, \
                                                        C_TVECTOR, TY_REAL)
#define rxtrxtr_TA_MUNI(a, b, c)           t_munite_rr((int)R_SCALAR(DESC(a), valr), (b),\
                                                       (c), C_TVECTOR, C_TVECTOR, TY_REAL) 
#define ixtbxtb_TA_MUNI(a, b, c)           t_munite_ii(VAL_INT(a), (b), (c), C_TVECTOR, \
                                                        C_TVECTOR, TY_BOOL)
#define rxtbxtb_TA_MUNI(a, b, c)           t_munite_ii((int)R_SCALAR(DESC(a), valr), (b),\
                                                        (c), C_TVECTOR, C_TVECTOR, TY_BOOL) 


