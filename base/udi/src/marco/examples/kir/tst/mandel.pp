let Depth = 255 ,
    Xmin = ~2 ,
    Xmax = 1 ,
    Ymin = ~1 ,
    Ymax = 1 ,
    Xsize = 20 ,
    Ysize = 10
in let Xm = ( Xsize / ( Xmax - Xmin ) ) ,
       Ym = ( Ysize / ( Ymax - Ymin ) )
   in def 
        Row [ X , Y ] = if ( Y eq 0 )
                        then < Column [ X , Y ] >
                        else ( Row [ X , ( Y - 1 ) ] ++ < Column [ X , Y ] > ) ,
        Column [ X , Y ] = if ( X eq 0 )
                           then < Mandel [ X , Y ] >
                           else ( Column [ ( X - 1 ) , Y ] ++ < Mandel [ X , Y ] > ) ,
        Mandel [ X , Y ] = let Cx = ( ( X / Xm ) + Xmin ) ,
                               Cy = ( ( Y / Ym ) + Ymin )
                           in Calc [ Cx , Cy , Cx , Cy , 0 ] ,
        Calc [ X , Y , Wx , Wy , D ] = if ( D eq Depth )
                                       then 255
                                       else let Nwx = ( ( ( Wx * Wx ) - ( Wy * Wy ) ) + X ) ,
                                                Nwy = ( ( ( 2 * Wx ) * Wy ) + Y )
                                            in if ( ( ( Nwx * Nwx ) + ( Nwy * Nwy ) ) gt 4 )
                                               then D
                                               else Calc [ X , Y , Nwx , Nwy , ( D + 1 ) ]
      in Row [ Xsize , Ysize ]

