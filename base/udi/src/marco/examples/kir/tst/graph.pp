let Graph = Frac{Const{18} , Frac{Const{2} , Const{4}}}
in def 
     Trav [ Fun_list , Graph ] = ap case 
                                    when Const{X} guard true do ap ( 1 | Fun_list )
                                                                to [ X ] ,
                                    when Frac{X , Y} guard true do ap ( 2 | Fun_list )
                                                                   to [ Trav [ Fun_list , X ] , Trav [ Fun_list , Y ] ] ,
                                    when Mult{X , Y} guard true do ap ( 3 | Fun_list )
                                                                   to [ Trav [ Fun_list , X ] , Trav [ Fun_list , Y ] ] ,
                                    when Div{X , Y} guard true do ap ( 4 | Fun_list )
                                                                  to [ Trav [ Fun_list , X ] , Trav [ Fun_list , Y ] ] ,
                                    when Add{X , Y} guard true do ap ( 5 | Fun_list )
                                                                  to [ Trav [ Fun_list , X ] , Trav [ Fun_list , Y ] ] ,
                                    when Sub{X , Y} guard true do ap ( 6 | Fun_list )
                                                                  to [ Trav [ Fun_list , X ] , Trav [ Fun_list , Y ] ]
                                    end
                                 to [ Graph ] ,
     Calc [   ] = def 
                    Z [   ] = when Frac{Const{A} , Const{B}} guard true do A ,
                    N [   ] = when Frac{Const{A} , Const{B}} guard true do B ,
                    Const [ X ] = Frac{Const{X} , Const{1}} ,
                    Norm [ A ] = let G = Gcd [ Z [ A ] , N [ A ] ]
                                 in Frac{Const{trunc( ( Z [ A ] / G ) )} , Const{trunc( ( N [ A ] / G ) )}} ,
                    Frac [ A , B ] = Norm [ Frac{Const{( Z [ A ] * N [ B ] )} , Const{( N [ A ] * Z [ B ] )}} ] ,
                    Mult [ A , B ] = Norm [ Frac{Const{( Z [ A ] * Z [ B ] )} , Const{( N [ A ] * N [ B ] )}} ] ,
                    Div [ A , B ] = Norm [ Frac{Const{( Z [ A ] * N [ B ] )} , Const{( N [ A ] * Z [ B ] )}} ] ,
                    Add [ A , B ] = Norm [ Frac{Const{( ( Z [ A ] * N [ B ] ) + ( Z [ B ] * N [ A ] ) )} , Const{( N [ A ] * N [ B ] )}} ] ,
                    Sub [ A , B ] = Norm [ Frac{Const{( ( Z [ A ] * N [ B ] ) - ( Z [ B ] * N [ A ] ) )} , Const{( N [ A ] * N [ B ] )}} ]
                  in < Const , Frac , Mult , Div , Add , Sub > ,
     Calcstr [   ] = def 
                       Const [ X ] = let L = < '0` , '1` , '2` , '3` , '4` , '5` , '6` , '7` , '8` , '9` >
                                     in def 
                                          Str [ X ] = if ( X le 9 )
                                                      then lselect [ ( X + 1 ) , L ]
                                                      else ( Str [ trunc [ ( A / 10 ) ] ] ++ lselect [ ( ( A mod 10 ) + 1 ) , L ] )
                                        in X ,
                       Frac [ A , B ] = ( '(` ++ ( A ++ ( '/` ++ ( B ++ ')` ) ) ) ) ,
                       Mult [ A , B ] = Frac{Const{( Z [ A ] * Z [ B ] )} , Const{( N [ A ] * N [ B ] )}} ,
                       Div [ A , B ] = Frac{Const{( Z [ A ] * N [ B ] )} , Const{( N [ A ] * Z [ B ] )}} ,
                       Add [ A , B ] = Frac{Const{( ( Z [ A ] * N [ B ] ) + ( Z [ B ] * N [ A ] ) )} , Const{( N [ A ] * N [ B ] )}} ,
                       Sub [ A , B ] = Frac{Const{( ( Z [ A ] * N [ B ] ) - ( Z [ B ] * N [ A ] ) )} , Const{( N [ A ] * N [ B ] )}}
                     in < Const , Frac , Mult , Div , Add , Sub > ,
     Gcd [ X , Y ] = if ( X eq Y )
                     then X
                     else if ( X gt Y )
                          then Gcd [ Y , Modulo [ X , Y ] ]
                          else Gcd [ X , Modulo [ Y , X ] ] ,
     Modulo [ X , Y ] = if ( X le Y )
                        then X
                        else Modulo [ ( X - Y ) , Y ]
   in Trav [ Calc , Graph ]

