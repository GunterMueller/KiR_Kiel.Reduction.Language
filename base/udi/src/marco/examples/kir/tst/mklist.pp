def 
  Mklist [ N ] = if ( N gt 1 )
                 then < N , Mklist [ ( N - 1 ) ] >
                 else <>
in Mklist [ 5 ]

