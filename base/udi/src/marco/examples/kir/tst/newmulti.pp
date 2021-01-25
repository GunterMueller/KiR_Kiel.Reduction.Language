def 
  MULTIGRID [ DIM , M , V1 , V2 , SCALE , H ] = def 
                                                  MINIT [   ] = MKMAT [ DIM ] ,
                                                  MATF [   ] = MKM4 [ DIM ] ,
                                                  MKM4 [ DIM ] = def 
                                                                   NEXT_ROW [ Z_Y ] = if ( Z_Y lt 0 )
                                                                                      then nilmat
                                                                                      else munite( 1 , NEXT_COL [ 1 ] , NEXT_ROW [ ( Z_Y - 1 ) ] ) ,
                                                                   NEXT_COL [ Z_X ] = if ( Z_X eq DIM )
                                                                                      then to_mat( 4 )
                                                                                      else munite( 2 , to_mat( 4 ) , NEXT_COL [ ( Z_X + 1 ) ] )
                                                                 in if ( DIM le 0 )
                                                                    then nilmat
                                                                    else if ( DIM eq 1 )
                                                                         then mat 
                                                                              < 4 >.
                                                                         else NEXT_ROW [ ( DIM - 1 ) ] ,
                                                  MKMAT [ DIM ] = def 
                                                                    NEXT_ROW [ Z_Y ] = if ( Z_Y lt 0 )
                                                                                       then nilmat
                                                                                       else munite( 1 , NEXT_COL [ 1 , 1 , Z_Y ] , NEXT_ROW [ ( Z_Y - 1 ) ] ) ,
                                                                    NEXT_COL [ Z_X , R_X , I_X ] = if ( I_X eq DIM )
                                                                                                   then if ( Z_X eq DIM )
                                                                                                        then to_mat( ( ( ( I_X - 1 ) * ( I_X - 1 ) ) + ( R_X * R_X ) ) )
                                                                                                        else munite( 2 , to_mat( ( ( ( I_X - 1 ) * ( I_X - 1 ) ) + ( R_X * R_X ) ) ) , NEXT_COL [ ( Z_X + 1 ) , ( R_X + 1 ) , I_X ] )
                                                                                                   else if ( Z_X eq DIM )
                                                                                                        then to_mat( ( I_X * I_X ) )
                                                                                                        else munite( 2 , to_mat( ( I_X * I_X ) ) , NEXT_COL [ ( Z_X + 1 ) , R_X , ( I_X + 1 ) ] )
                                                                  in if ( DIM le 0 )
                                                                     then nilmat
                                                                     else if ( DIM eq 1 )
                                                                          then mat 
                                                                               < 0 >.
                                                                          else NEXT_ROW [ ( DIM - 1 ) ] ,
                                                  APMUL [ UK0 , FMAT ] = def 
                                                                           FINE_COARSE [ MAT ] = def 
                                                                                                   NEW_MAT [   ] = FTC_ROW [ ( MAT + mrotate( ~1 , 0 , MAT ) ) ] ,
                                                                                                   FTC_COL [ REST_MAT ] = if ( mdim( 2 , REST_MAT ) eq 2 )
                                                                                                                          then to_mat( mselect( 0 , 1 , REST_MAT ) )
                                                                                                                          else munite( 2 , to_mat( mselect( 0 , 1 , REST_MAT ) ) , FTC_COL [ mcut( 0 , 2 , REST_MAT ) ] ) ,
                                                                                                   FTC_ROW [ REST_MAT ] = if ( mdim( 1 , REST_MAT ) eq 2 )
                                                                                                                          then to_mat( mselect( 1 , 0 , REST_MAT ) )
                                                                                                                          else munite( 1 , to_mat( mselect( 1 , 0 , REST_MAT ) ) , FTC_ROW [ mcut( 2 , 0 , REST_MAT ) ] )
                                                                                                 in ( FTC_COL [ ( NEW_MAT + mrotate( 0 , ~1 , NEW_MAT ) ) ] / 4 ) ,
                                                                           COARSE_FINE [ MAT ] = def 
                                                                                                   MAKE_COL [ Y , COL ] = if ( mdim( 1 , COL ) eq 0 )
                                                                                                                          then munite( 1 , to_mat( Y ) , to_mat( Y ) )
                                                                                                                          else munite( 1 , munite( 1 , to_mat( Y ) , to_mat( Y ) ) , MAKE_COL [ MAKE_ROW [ mselect( 1 , 1 , COL ) , mcut( 0 , 1 , COL ) ] , mcut( 1 , 0 , COL ) ] ) ,
                                                                                                   MAKE_ROW [ X , ROW ] = if ( mdim( 2 , ROW ) eq 0 )
                                                                                                                          then munite( 2 , to_mat( X ) , to_mat( X ) )
                                                                                                                          else munite( 2 , munite( 2 , to_mat( X ) , to_mat( X ) ) , MAKE_ROW [ mselect( 1 , 1 , ROW ) , mcut( 0 , 1 , ROW ) ] )
                                                                                                 in MAKE_COL [ MAKE_ROW [ mselect( 1 , 1 , MAT ) , mcut( ~2 , 1 , MAT ) ] , mcut( 1 , 0 , MAT ) ] ,
                                                                           ROW_TAKE_1_2 [ ROW ] = if ( vdim( ROW ) lt 3 )
                                                                                                  then to_vect( vselect( 1 , ROW ) )
                                                                                                  else vunite( to_vect( vselect( 1 , ROW ) ) , ROW_TAKE_1_2 [ vcut( 2 , ROW ) ] ) ,
                                                                           COL_TAKE_DIM_2 [ COL ] = if ( vdim( COL ) lt 3 )
                                                                                                    then to_tvect( vselect( vdim( COL ) , COL ) )
                                                                                                    else vunite( COL_TAKE_DIM_2 [ vcut( ( 0 - ( vdim( COL ) - 1 ) ) , COL ) ] , to_tvect( vselect( vdim( COL ) , COL ) ) ) ,
                                                                           RELAX [ UMAT , MATF , COUNT , H , X_1 , X_N , Y_1 , Y_N ] = def 
                                                                                                                                         SWEEP [ MATU , MATD , MATL , MATR ] = ( ( ( ( ( MATU + MATD ) + ( MATL + MATR ) ) - MATF ) / 4 ) * ( ( H * H ) / 4 ) )
                                                                                                                                       in if ( COUNT gt 0 )
                                                                                                                                          then RELAX [ mreplace_r( 1 , X_1 , mreplace_r( mdim( 1 , UMAT ) , X_N , mreplace_c( 1 , Y_1 , mreplace_c( mdim( 2 , UMAT ) , Y_N , SWEEP [ mrotate( 1 , 0 , UMAT ) , mrotate( ~1 , 0 , UMAT ) , mrotate( 0 , 1 , UMAT ) , mrotate( 0 , ~1 , UMAT ) ] ) ) ) ) , MATF , ( COUNT - 1 ) , H , X_1 , X_N , Y_1 , Y_N ]
                                                                                                                                          else UMAT ,
                                                                           MGC [ H , K , UK0 , FK , UK0_X1 , UK0_XN , UK0_Y1 , UK0_YN ] = def 
                                                                                                                                            UK [   ] = RELAX [ UK0 , FK , V1 , H , UK0_X1 , UK0_XN , UK0_Y1 , UK0_YN ] ,
                                                                                                                                            IK0_X1 [   ] = ROW_TAKE_1_2 [ UK0_X1 ] ,
                                                                                                                                            IK0_XN [   ] = ROW_TAKE_1_2 [ UK0_XN ] ,
                                                                                                                                            IK0_Y1 [   ] = vunite( tvect< 0 > , vunite( COL_TAKE_DIM_2 [ UK0_Y1 ] , tvect< 0 > ) ) ,
                                                                                                                                            IK0_YN [   ] = vunite( tvect< 0 > , vunite( COL_TAKE_DIM_2 [ UK0_YN ] , tvect< 0 > ) ) ,
                                                                                                                                            IK [   ] = mreplace_r( mdim( 1 , IKK ) , IK0_XN , mreplace_r( 1 , IK0_X1 , mreplace_c( 1 , IK0_Y1 , mreplace_c( mdim( 2 , IKK ) , IK0_YN , IKK ) ) ) ) ,
                                                                                                                                            IKK [   ] = FINE_COARSE [ ( FK - UK ) ]
                                                                                                                                          in if ( K gt 1 )
                                                                                                                                             then ( UK + mreplace_r( mdim( 1 , UK0 ) , UK0_XN , mreplace_c( 1 , UK0_Y1 , mreplace_r( 1 , UK0_X1 , mreplace_c( mdim( 2 , UK0 ) , UK0_YN , COARSE_FINE [ RELAX [ MGC [ ( 2 * H ) , ( K - 1 ) , mreplace_r( mdim( 1 , IKK ) , IK0_XN , mreplace_r( 1 , IK0_X1 , mreplace_c( 1 , IK0_Y1 , mreplace_c( mdim( 2 , IKK ) , IK0_YN , IKK ) ) ) ) , IK , IK0_X1 , IK0_XN , IK0_Y1 , IK0_YN ] , IK , V2 , ( 2 * H ) , IK0_X1 , IK0_XN , IK0_Y1 , IK0_YN ] ] ) ) ) ) )
                                                                                                                                             else RELAX [ UK0 , FK , ( V1 + V2 ) , H , UK0_X1 , UK0_XN , UK0_Y1 , UK0_YN ] ,
                                                                           MKM4 [ DIM ] = def 
                                                                                            NEXT_ROW [ Z_Y ] = if ( Z_Y lt 0 )
                                                                                                               then nilmat
                                                                                                               else munite( 1 , NEXT_COL [ 1 ] , NEXT_ROW [ ( Z_Y - 1 ) ] ) ,
                                                                                            NEXT_COL [ Z_X ] = if ( Z_X eq DIM )
                                                                                                               then to_mat( 4 )
                                                                                                               else munite( 2 , to_mat( 4 ) , NEXT_COL [ ( Z_X + 1 ) ] )
                                                                                          in if ( DIM le 0 )
                                                                                             then nilmat
                                                                                             else if ( DIM eq 1 )
                                                                                                  then mat 
                                                                                                       < 4 >.
                                                                                                  else NEXT_ROW [ ( DIM - 1 ) ] ,
                                                                           UK0_X1 [   ] = mselect( 1 , 0 , UK0 ) ,
                                                                           UK0_XN [   ] = mselect( mdim( 1 , UK0 ) , 0 , UK0 ) ,
                                                                           UK0_Y1 [   ] = mselect( 0 , 1 , UK0 ) ,
                                                                           UK0_YN [   ] = mselect( 0 , mdim( 2 , UK0 ) , UK0 )
                                                                         in RELAX [ MGC [ H , M , UK0 , FMAT , UK0_X1 , UK0_XN , UK0_Y1 , UK0_YN ] , FMAT , V2 , H , UK0_X1 , UK0_XN , UK0_Y1 , UK0_YN ]
                                                in APMUL [ MINIT , MATF ]
in MULTIGRID [ 8 , 2 , 1 , 2 , 1 , 1 ]

