def 
  Fib [ N ] = if ( N le 1 )
              then 1
              else letpar D1 = Fib [ ( N - 1 ) ] ,
                          D2 = Fib [ ( N - 2 ) ]
                   in ( D1 + D2 )
in Fib [ 20 ]

