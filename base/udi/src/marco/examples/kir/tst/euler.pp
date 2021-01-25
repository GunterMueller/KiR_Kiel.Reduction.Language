def 
  Head [ L ] = lselect( 1 , L ) ,
  Tail [ L ] = lselect( 2 , L ) ,
  Cons [ X , L ] = < X , L > ,
  Append [ L1 , L2 ] = if empty( L1 )
                       then L2
                       else Cons [ Head [ L1 ] , Append [ Tail [ L1 ] , L2 ] ] ,
  Length [ L ] = if empty( L )
                 then 0
                 else ( 1 + Length [ Tail [ L ] ] ) ,
  FlatMap [ F , L ] = if empty( L )
                      then <>
                      else ( < F [ Head [ L ] ] > ++ FlatMap [ F , Tail [ L ] ] ) ,
  Filter [ Op , L ] = if empty( L )
                      then <>
                      else if Op [ Head [ L ] ]
                           then Cons [ Head [ L ] , Filter [ Op , Tail [ L ] ] ]
                           else Filter [ Op , Tail [ L ] ] ,
  Mklist [ N , M ] = if ( N gt M )
                     then <>
                     else Cons [ N , Mklist [ ( N + 1 ) , M ] ] ,
  Relprime [ X , Y ] = if ( Y eq 0 )
                       then ( X eq 1 )
                       else Relprime [ Y , ( X mod Y ) ] ,
  Euler [ N ] = Length [ Filter [ Relprime [ N ] , Mklist [ 1 , ( N - 1 ) ] ] ]
in FlatMap [ Euler , Mklist [ 1 , 100 ] ]

