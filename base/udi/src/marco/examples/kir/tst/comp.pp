def
    Primfuncs [ ] = case
                    when '+` guard true do 'red_mkact(red_add, 2)` ,
                    when '-` guard true do 'red_mkact(red_sub, 2)` ,
                    when '*` guard true do 'red_mkact(red_mul, 2)` ,
                    when '/` guard true do 'red_mkact(red_div, 2)` ,
                    when 'neg` guard true do 'red_mkact(red_neg, 1)` ,
                    when 'mod` guard true do 'red_mkact(red_mod, 2)` ,
                    when 'abs` guard true do 'red_mkact(red_abs, 1)` ,
                    when 'min` guard true do 'red_mkact(red_min, 2)` ,
                    when 'max` guard true do 'red_mkact(red_max, 2)` ,
                    when 'lt` guard true do 'red_mkact(red_lt, 2)` ,
                    when 'gt` guard true do 'red_mkact(red_gt, 2)` ,
                    when 'eq` guard true do 'red_mkact(red_eq, 2)` ,
                    when 'ne` guard true do 'red_mkact(red_ne, 2)` ,
                    when 'le` guard true do 'red_mkact(red_le, 2)` ,
                    when 'ge` guard true do 'red_mkact(red_ge, 2)` ,
                    when 'and` guard true do 'red_mkact(red_and, 2)` ,
                    when 'or` guard true do 'red_mkact(red_or, 2)` ,
                    when 'xor` guard true do 'red_mkact(red_xor, 2)` ,
                    when 'not` guard true do 'red_mkact(red_not, 1)` ,
                    when 'empty` guard true do 'red_mkact(red_empty, 1)` ,
                    when 'lunite` guard true do 'red_mkact(red_lunite, 2)` ,
                    when 'lcut` guard true do 'red_mkact(red_lcut, 2)` ,
                    when 'lselect` guard true do 'red_mkact(red_lselect, 2)` ,
                    when 'ldim` guard true do 'red_mkact(red_ldim, 1)` ,
                    when 'lreplace` guard true do 'red_mkact(red_lreplace, 3)` ,
                    when X guard true do ( 'red_unknprf(""` ++ ( X ++ '"")` ) )
                    end ,
    Digit_to_char [ X ] = lselect ( ( X + 1 ) , '0123456789` ) ,
    Its_akku [ I , S ] = if ( I eq 0 )
                         then S
                         else Its_akku [ ( ( I - ( I mod 10 ) ) / 10 ) , lunite [ Digit_to_char [ ( I mod 10 ) ] , S ] ] ,
    Int_to_s [ I ] = if ( I eq 0 )
                     then '0`
                     else if ( I lt 0 )
                          then lunite [ '-` , Its_akku [ ( ~1 * I ) , '` ] ]
                          else Its_akku [ I , '` ] ,
    Compile_expr [ ] = case
                       when ( Int { I } , States ) guard true do < <> , <> , ( ( 'red_mkint(` ++ Int_to_s [ I ] ) ++ ')` ) , <> , States > ,
                       when ( Boolean { W } , States ) guard true do < <> , <> , ( ( 'red_mkbool(` ++ if W
                                                                                                      then 'TRUE`
                                                                                                      else 'FALSE`                        ) ++ ')` ) , <> , States > ,
                       when ( Prf { F } , States ) guard true do < <> , <> , Primfuncs [ F ] , <> , States > ,
                       when ( Var { X } , < Lvl , Varlist , as ... Rest > ) guard true do < <> , <> , Var_pos [ X , Varlist ] , <> , ( < Lvl , Varlist > ++ Rest ) > ,
                       when ( Str { S } , < Lvl , as ... Rest > ) guard true do < <> , ( < if ( Lvl eq ~1 )
                                                                                           then ( 'p = red_mkstring (` ++ ( Int_to_s [ ldim ( S ) ] ++ ');` ) )
                                                                                           else ( 'push (red_mkstring (` ++ ( Int_to_s [ ldim ( S ) ] ++ '));` ) )                        > ++ Gen_scode [ 0 , S , if ( Lvl eq ~1 )
                                                 then 'p->e_str`
                                                 else ( 's_p[` ++ ( Int_to_s [ Lvl ] ++ ']->e_str` ) )                        ] ) , if ( Lvl eq ~1 )
                              then 'p`
                              else ( 's_p[` ++ ( Int_to_s [ Lvl ] ++ ']` ) ) ,                          if ( Lvl eq ~1 )
                          then <>
                          else < 'pop();` > ,                          ( < Lvl > ++ Rest ) > ,
                       when ( Tup { Ari , Args } , < Lvl , as ... Rest > ) guard true do letp < F , P , S , O , < Lvl2 , as ... Rest2 > > = if not ( empty ( Args ) )
                                                                                                                                            then Mk_list [ Args , < <> , <> , <> , <> , ( < Lvl > ++ Rest ) > , 'e_targ` , 0 ]
                                                                                                                                            else < <> , <> , <> , <> , ( < Lvl > ++ Rest ) >
                       in < F , ( < if ( Lvl eq ~1 )
                                    then ( 'p` ++ ( ' = ` ++ ( 'red_mktup(` ++ ( Int_to_s [ Ari ] ++ ');` ) ) ) )
                                    else ( 'push(` ++ ( ( 'red_mktup(` ++ ( Int_to_s [ Ari ] ++ ')` ) ) ++ ');` ) )                        > ++ P ) , if ( Lvl eq ~1 )
                                   then 'p`
                                   else ( ( 's_p[` ++ Int_to_s [ Lvl ] ) ++ ']` ) ,                          ( if ( Lvl eq ~1 )
                            then <>
                            else < 'pop();` >                        ++ O ) , ( < Lvl > ++ Rest2 ) > ,
                       when ( Ccons { Bez , Tup { Ari , Args } } , < Lvl , Env , as ... Rest > ) guard true do letp < F , P , S , O , < Lvl2 , as ... Rest2 > > = if not ( empty ( Args ) )
                                                                                                                                                                  then Mk_list [ Args , < <> , <> , <> , <> , ( < Lvl , Env > ++ Rest ) > , 'e_carg` , 0 ]
                                                                                                                                                                  else < <> , <> , <> , <> , ( < Lvl , Env > ++ Rest ) >
                       in let Redcc = ap case
                                         when Str { Name } guard true do ( 'red_mkccons(""` ++ ( Name ++ '"", ` ) ) ,
                                         when Var { V } guard true do ( 'red_mkccons(red_convts(` ++ ( Var_pos [ V , Env ] ++ '), ` ) )
                                         end
                              to [ Bez ]
                       in < F , ( < if ( Lvl eq ~1 )
                                    then ( 'p` ++ ( ' = ` ++ ( Redcc ++ ( Int_to_s [ Ari ] ++ ');` ) ) ) )
                                    else ( 'push(` ++ ( ( Redcc ++ ( Int_to_s [ Ari ] ++ ')` ) ) ++ ');` ) )                        > ++ P ) , if ( Lvl eq ~1 )
                                   then 'p`
                                   else ( ( 's_p[` ++ Int_to_s [ Lvl ] ) ++ ']` ) ,                          ( if ( Lvl eq ~1 )
                            then <>
                            else < 'pop();` >                        ++ O ) , ( < Lvl > ++ Rest2 ) > ,
                       when ( Ap { N , Expro , Args } , < Lvl , as ... Rest > ) guard true do letp < Funo , Preo , So , Posto , < Lvlo , as ... Resto > > = Compile_expr [ Expro , ( < ( Lvl + 1 ) > ++ Rest ) ]
                       in letp < Fun , Pre , S , Post , Global > = Create_appli [ Args , < <> , <> , <> , <> , ( < Lvl > ++ Resto ) > ]
                       in if ( Lvl eq ~1 )
                          then < ( Funo ++ Fun ) , ( ( ( Preo ++ ( < ( ( 'p = ` ++ So ) ++ ';` ) > ++ Posto ) ) ++ Pre ) ++ < ( ( 'p = apply (p, ` ++ S ) ++ ');` ) > ) , 'p` , Post , Global >
                          else letp P = ( ( 's_p[` ++ Int_to_s [ Lvl ] ) ++ ']` )
                          in < ( Funo ++ Fun ) , ( ( if empty ( ( Pre ++ Post ) )
                                                     then < ( ( 'push(` ++ So ) ++ ');` ) >
                                                     else ( < 'push (NULL); /* dummy */` > ++ ( Preo ++ ( < ( ( P ++ ' = ` ) ++ ( So ++ ';` ) ) > ++ Posto ) ) )                           ++ Pre ) ++ < ( ( ( ( ( P ++ ' = apply (` ) ++ P ) ++ ', ` ) ++ S ) ++ ');` ) > ) , P , ( < 'pop();` > ++ Post ) , Global > ,
                       when ( If { Con , The , Els } , < Lvl , as ... Rest > ) guard true do letp < Fun1 , Pre1 , E1 , Pos1 , < Lvl1 , as ... Glob1 > > = Compile_expr [ Con , ( < ( Lvl + 1 ) > ++ Rest ) ]
                       in letp < Fun2 , Pre2 , E2 , Pos2 , < Lvl2 , as ... Glob2 > > = Compile_expr [ The , ( < ( Lvl + 1 ) > ++ Glob1 ) ]
                       in letp < Fun3 , Pre3 , E3 , Pos3 , < Lvl3 , as ... Glob3 > > = Compile_expr [ Els , ( < ( Lvl + 1 ) > ++ Glob2 ) ]
                       in let Subme = if ( Lvl eq ~1 )
                                      then 'p`
                                      else ( ( 's_p[` ++ Int_to_s [ Lvl ] ) ++ ']` )
                       in < ( ( Fun1 ++ Fun2 ) ++ Fun3 ) , ( ( if ( Lvl eq ~1 )
                                                               then <>
                                                               else < 'push(NULL); /* dummy push */` >                        ++ ( Pre1 ++ ( < ( Subme ++ ( ' = ` ++ ( E1 ++ ';` ) ) ) > ++ Pos1 ) ) ) ++ ( < ( 'if (` ++ ( Subme ++ '->e_bool) {` ) ) > ++ ( ( Pre2 ++ < ( ( Subme ++ ' = ` ) ++ ( E2 ++ ';` ) ) > ) ++ ( ( Pos2 ++ < '}` , 'else {` > ) ++ ( ( Pre3 ++ < ( ( Subme ++ ' = ` ) ++ ( E3 ++ ';` ) ) > ) ++ ( Pos3 ++ < '}` > ) ) ) ) ) ) , Subme , if ( Lvl eq ~1 )
                                                                                                                                                                                                                                                                                                                                                            then <>
                                                                                                                                                                                                                                                                                                                                                            else < 'pop();` > ,                          ( < Lvl > ++ Glob3 ) > ,
                       when ( Sub { Ari , Vars , Body } , < Lvl , Env , < Name , Nlvl > , Flag , as ... Rest > ) guard true do letp < Fun , Pre , S , Pos , < X , Y , < Nname , Nnlvl > , as ... Nrest > > = Compile_expr [ Body , ( < ~1 , ( Mkvarlist [ Vars , if Flag
                                                                                                                                                                                                                                                                 then 1
                                                                                                                                                                                                                                                                 else 0                                                                                                                                     ] ++ Increase_lvl [ Env ] ) , < Name , ( Nlvl + 1 ) > , true > ++ Rest ) ]
                       in < ( Fun ++ ( < ( 'EXPR_PTR ` ++ ( Name ++ ( Int_to_s [ Nlvl ] ++ '(args) /* local SUB */` ) ) ) , '` , 'FRAME_PTR args;` , '` , '{` , 'EXPR_PTR p, *s_p = stackpointer;` , '` , ( 'DBUG_ENTER (""` ++ ( Name ++ ( Int_to_s [ Nlvl ] ++ '"");` ) ) ) > ++ ( ( Pre ++ ( Pos ++ < if ( S eq 'p` )
                                                                                                                                                                                                                                                                                                         then 'DBUG_RETURN (p);`
                                                                                                                                                                                                                                                                                                         else ( ( 'DBUG_RETURN (` ++ S ) ++ ');` )                        > ) ) ++ < '}` , '` > ) ) ) , <> , if Flag
                                                           then ( 'red_mkinn(` ++ ( ( Name ++ Int_to_s [ Nlvl ] ) ++ ( ',` ++ ( Int_to_s [ ( Ari + 1 ) ] ++ ',args)` ) ) ) )
                                                           else ( 'red_mkact(` ++ ( ( Name ++ Int_to_s [ Nlvl ] ) ++ ( ',` ++ ( Int_to_s [ Ari ] ++ ')` ) ) ) ) ,                          <> , ( < Lvl , Env , < Name , Nnlvl > , Flag > ++ Rest ) > ,
                       when ( Condi { Then , Else } , < Lvl , Env , < Name , Nlvl > , Flag , as ... Rest > ) guard true do letp < Fun1 , Pre1 , S1 , Pos1 , < Lvl1 , Env1 , < Name1 , Nlvl1 > , Flag1 , as ... Rest1 > > = Compile_expr [ Then , ( < ~1 , Increase_lvl [ Env ] , < Name , ( Nlvl + 1 ) > , true > ++ Rest ) ]
                       in letp < Fun2 , Pre2 , S2 , Pos2 , < Lvl2 , Env2 , < Name2 , Nlvl2 > , Flag2 , as ... Rest2 > > = Compile_expr [ Else , ( < ~1 , Increase_lvl [ Env ] , < Name , Nlvl1 > , true > ++ Rest1 ) ]
                       in < ( ( Fun2 ++ Fun1 ) ++ ( < ( 'EXPR_PTR ` ++ ( Name ++ ( Int_to_s [ Nlvl ] ++ '(args) /* local then ... else ... */` ) ) ) , '` , 'FRAME_PTR args;` , '` , '{` , 'EXPR_PTR p, *s_p = stackpointer;` , '` , ( 'DBUG_ENTER (""` ++ ( Name ++ ( Int_to_s [ Nlvl ] ++ '"");` ) ) ) , ( 'if ((*args)[` ++ if Flag
                                                                                                                                                                                                                                                                                                                               then '1]->e_bool) {`
                                                                                                                                                                                                                                                                                                                               else '0]->e_bool) {`                        ) > ++ ( Pre1 ++ ( < ( 'p = ` ++ ( S1 ++ ';` ) ) > ++ ( Pos1 ++ ( < '}` , 'else {` > ++ ( Pre2 ++ ( < ( 'p = ` ++ ( S2 ++ ';` ) ) > ++ ( Pos2 ++ < '}` , 'DBUG_RETURN (p);` , '}` , '` > ) ) ) ) ) ) ) ) ) , <> , if Flag
                                                                                                                                                                                                                                          then ( 'red_mkinn (` ++ ( Name ++ ( Int_to_s [ Nlvl ] ++ ', 2, args)` ) ) )
                                                                                                                                                                                                                                          else ( 'red_mkact (` ++ ( Name ++ ( Int_to_s [ Nlvl ] ++ ', 1)` ) ) ) ,                          <> , ( < Lvl , Env , < Name , Nlvl2 > , true > ++ Rest2 ) > ,
                       when ( Let { Liste , Expr } , Global ) guard true do Compile_expr [ let Args = Get_second [ Liste ] ,
                                                                                               Vars = Get_first [ Liste ]
                       in Ap { ldim ( Args ) , Sub { ldim ( Vars ) , Vars , Expr } , Args } , Global ] ,
                       when ( Case { Arity , List_of_whens } , < Lvl , Env , < Name , Nlvl > , Flag , as ... Rest > ) guard true do letp < Fun , < Lvl2 , Env2 , < Name2 , Nlvl2 > , Flag2 , as ... Rest2 > > = Gen_case [ List_of_whens , ( < Lvl , Env , < Name , Nlvl > , Flag > ++ Rest ) ]
                       in < Fun , <> , if Flag
                                       then ( 'red_mkinn (` ++ ( Name ++ ( Int_to_s [ Nlvl ] ++ ( ', ` ++ ( Int_to_s [ ( Arity + 1 ) ] ++ ', args)` ) ) ) ) )
                                       else ( 'red_mkact (` ++ ( Name ++ ( Int_to_s [ Nlvl ] ++ ( ', ` ++ ( Int_to_s [ Arity ] ++ ')` ) ) ) ) ) ,                          <> , ( < Lvl , Env , < Name2 , Nlvl2 > , Flag > ++ Rest2 ) > ,
                       when ( Letrec { Funclist , Expr } , < Lvl , Env , < Name , Nlvl > , Flag , as ... Rest > ) guard true do let Env_letrec = Mkletrec [ Funclist , Name , Nlvl , Flag ]
                       in letp < Func1 , Pre1 , S1 , Pos1 , < Lvl1 , Env1 , < Name1 , Nlvl1 > , Flag1 , as ... Rest1 > > = Compileletrec [ Nlvl , Funclist , ( < Lvl , ( Env_letrec ++ Env ) , < Name , ( Nlvl + ldim ( Funclist ) ) > , Flag > ++ Rest ) ]
                       in letp < Func2 , Pre2 , S2 , Pos2 , < Lvl2 , Env2 , < Name2 , Nlvl2 > , Flag2 , as ... Rest2 > > = Compile_expr [ Expr , ( < Lvl , ( Env_letrec ++ Env ) , < Name , Nlvl1 > , Flag > ++ Rest ) ]
                       in < ( Mk_proto [ Funclist , Nlvl , Name ] ++ ( Func2 ++ Func1 ) ) , Pre2 , S2 , Pos2 , ( < Lvl , Env , < Name , Nlvl2 > , Flag > ++ Rest ) > ,
                       when ( X , States ) guard true do < <> , <> , 'red_unkncon()` , <> , States >
                       end ,
    Create_appli [ ] = case
                       when ( < Expr > , < F , P , S , O , < Lvl , as ... Rest > > ) guard true do letp < F2 , P2 , S2 , O2 , < Lvl2 , as ... Rest2 > > = Compile_expr [ Expr , ( < ( Lvl + 1 ) > ++ Rest ) ]
                       in < ( F ++ F2 ) , ( P ++ P2 ) , S2 , O2 , ( < Lvl > ++ Rest2 ) > ,
                       when ( < First , as ... Rest > , < F , P , S , O , < Lvl , as ... Rest2 > > ) guard true do letp < F2 , P2 , S2 , O2 , < Lvl2 , as ... Rest3 > > = Compile_expr [ First , ( < ( Lvl + 1 ) > ++ Rest2 ) ]
                       in Create_appli [ Rest , < ( F ++ F2 ) , ( P ++ ( P2 ++ ( < ( if ( Lvl eq ~1 )
                                                                                     then 'p = apply (p, `
                                                                                     else let P = ( 's_p[` ++ ( Int_to_s [ Lvl ] ++ ']` ) )
                                                                                     in ( P ++ ( ' = apply (` ++ ( P ++ ', ` ) ) )                        ++ ( S2 ++ ');` ) ) > ++ O2 ) ) ) , <> , <> , ( < Lvl > ++ Rest3 ) > ]
                       end ,
    Mk_proto [ ] = case
                   when ( <> , X , Y ) guard true do < '` > ,
                   when ( < < A , B > , as ... Rest > , X , Y ) guard true do ( < ( 'EXPR_PTR ` ++ ( Y ++ ( Int_to_s [ X ] ++ '();` ) ) ) > ++ Mk_proto [ Rest , ( X + 1 ) , Y ] )
                   end ,
    Mk_list [ ] = case
                  when ( < Expr > , < F , P , S , O , < Lvl , as ... Rest > > , Elem , Numb ) guard true do letp < F2 , P2 , S2 , O2 , < Lvl2 , as ... Rest2 > > = Compile_expr [ Expr , ( < ( Lvl + 1 ) > ++ Rest ) ]
                  in < ( F ++ F2 ) , ( P ++ ( P2 ++ ( < ( '(*` ++ ( if ( Lvl eq ~1 )
                                                                    then 'p`
                                                                    else ( 's_p[` ++ ( Int_to_s [ Lvl ] ++ ']` ) )                   ++ ( '->` ++ ( Elem ++ ( ')[` ++ ( Int_to_s [ Numb ] ++ ( '] = ` ++ ( S2 ++ ';` ) ) ) ) ) ) ) ) > ++ O2 ) ) ) , <> , <> , ( < Lvl > ++ Rest2 ) > ,
                  when ( < First , as ... Rest > , < F , P , S , O , < Lvl , as ... Rest2 > > , Elem , Numb ) guard true do letp < F2 , P2 , S2 , O2 , < Lvl2 , as ... Rest3 > > = Compile_expr [ First , ( < ( Lvl + 1 ) > ++ Rest2 ) ]
                  in Mk_list [ Rest , < ( F ++ F2 ) , ( P ++ ( P2 ++ ( < ( '(*` ++ ( if ( Lvl eq ~1 )
                                                                                     then 'p`
                                                                                     else ( 's_p[` ++ ( Int_to_s [ Lvl ] ++ ']` ) )                   ++ ( '->` ++ ( Elem ++ ( ')[` ++ ( Int_to_s [ Numb ] ++ ( '] = ` ++ ( S2 ++ ';` ) ) ) ) ) ) ) ) > ++ O2 ) ) ) , <> , <> , ( < Lvl > ++ Rest3 ) > , Elem , ( Numb + 1 ) ]
                  end ,
    Compile [ ] = case
                  when Letrec { Funclist , Goalexpr } guard true do let Result = let Fl = Mkfunclist [ Funclist ]
                                                                        in ( Compile_funcs [ Funclist , Fl ] ++ letp < Fun , Pre , S , Post , Global > = Compile_expr [ Goalexpr , < ~1 , ( < < '_File_` , 'arg` > > ++ Fl ) , < 'red_goal` , 0 > , false > ]
                                                                        in < Filename { 'goal.c` , ( < '"#include ""kir.h""` , '` > ++ ( ( Mk_extern [ 'red_goal` , Fl ] ++ < '` > ) ++ ( Fun ++ ( ( ( < 'EXPR_PTR red_goal(arg)` , '` , 'EXPR_PTR arg;` , '` , '{` , 'EXPR_PTR p, *s_p = stackpointer;` > ++ ( < '` , 'DBUG_ENTER (""red_goal"");` , '` > ++ Pre ) ) ++ ( Post ++ < '` , if ( S eq 'p` )
                                                                                                                                                                                                                                                                                                                                                                                          then 'DBUG_RETURN (p);`
                                                                                                                                                                                                                                                                                                                                                                                          else ( ( 'DBUG_RETURN (` ++ S ) ++ ');` )                                                                         > ) ) ++ < '}` > ) ) ) ) } > )
                  in ( Result ++ < Filename { 'filelist` , Mkfilelist [ Result ] } > ) ,
                  when Expr guard true do letp < Fun , Pre , S , Post , Global > = Compile_expr [ Expr , < ~1 , < < '_File_` , 'arg` > > , < 'red_goal` , 0 > , false > ]
                  in let Result = < Filename { 'goal.c` , ( ( < '"#include ""kir.h""` , '` > ++ Fun ) ++ ( ( ( < 'EXPR_PTR red_goal(arg)` , '` , 'EXPR_PTR arg;` , '` , '{` , 'EXPR_PTR p, *s_p = stackpointer;` , '` , 'DBUG_ENTER (""red_goal"");` , '` > ++ Pre ) ++ ( Post ++ < '` , if ( S eq 'p` )
                                                                                                                                                                                                                                                                                         then 'DBUG_RETURN (p);`
                                                                                                                                                                                                                                                                                         else ( ( 'DBUG_RETURN (` ++ S ) ++ ');` )                          > ) ) ++ < '}` > ) ) } >
                  in ( Result ++ < Filename { 'filelist` , Mkfilelist [ Result ] } > )
                  end ,
    Mkfunclist [ ] = case
                     when <> guard true do <> ,
                     when < < Name , Sub { N , Vars , Expr } > , as ... Rest > guard true do ( < < Name , ( 'red_mkact (` ++ ( Name ++ ( ',` ++ ( Int_to_s [ N ] ++ ')` ) ) ) ) > > ++ Mkfunclist [ Rest ] ) ,
                     when < < Name , Body > , as ... Rest > guard true do ( < < Name , ( Name ++ '()` ) > > ++ Mkfunclist [ Rest ] )
                     end ,
    Mkfilelist [ ] = case
                     when <> guard true do <> ,
                     when < Filename { Name , Contents } , as ... Restfiles > guard true do ( < Name > ++ Mkfilelist [ Restfiles ] )
                     end ,
    Compile_funcs [ ] = case
                        when ( <> , Flist ) guard true do <> ,
                        when ( < < Fname , Sub { Ari , Lparm , Fbody } > , as ... Rest > , Flist ) guard true do letp < Fun , Pre , S , Post , Global > = Compile_expr [ Fbody , < ~1 , ( Mkvarlist [ Lparm , 0 ] ++ Flist ) , < Fname , 0 > , true > ]
                        in ( < Filename { ( Fname ++ '.c` ) , ( < '"#include ""kir.h""` , '` > ++ ( ( Mk_extern [ Fname , Flist ] ++ < '` > ) ++ ( Fun ++ ( ( ( < ( 'EXPR_PTR ` ++ ( Fname ++ '(args)` ) ) , '` , 'FRAME_PTR args;` , '` , '{` , 'EXPR_PTR p, *s_p = stackpointer;` , '` , ( 'DBUG_ENTER (` ++ ( Fname ++ ');` ) ) , '` > ++ Pre ) ++ ( Post ++ < if ( S eq 'p` )
                                                                                                                                                                                                                                                                                                                                                                  then 'DBUG_RETURN (p);`
                                                                                                                                                                                                                                                                                                                                                                  else ( ( 'DBUG_RETURN (` ++ S ) ++ ');` )                         > ) ) ++ < '}` > ) ) ) ) } > ++ Compile_funcs [ Rest , Flist ] ) ,
                        when ( < < Fname , Fbody > , as ... Rest > , Flist ) guard true do letp < Fun , Pre , S , Post , Global > = Compile_expr [ Fbody , < ~1 , Flist , < Fname , 0 > , false > ]
                        in ( < Filename { ( Fname ++ '.c` ) , ( < '"#include ""kir.h""` , '` > ++ ( ( Mk_extern [ Fname , Flist ] ++ < '` > ) ++ ( Fun ++ ( ( ( < ( 'EXPR_PTR ` ++ ( Fname ++ '()` ) ) , '` , '{` , 'EXPR_PTR p, *s_p = stackpointer;` , '` , ( 'DBUG_ENTER (""` ++ ( Fname ++ '"");` ) ) , '` > ++ Pre ) ++ ( Post ++ < if ( S eq 'p` )
                                                                                                                                                                                                                                                                                                                                         then 'DBUG_RETURN (p);`
                                                                                                                                                                                                                                                                                                                                         else ( ( 'DBUG_RETURN (` ++ S ) ++ ');` )                         > ) ) ++ < '}` > ) ) ) ) } > ++ Compile_funcs [ Rest , Flist ] )
                        end ,
    Var_pos [ ] = case
                  when ( X , <> ) guard true do 'red_freevar()` ,
                  when ( X , < < Y , Z > , as ... Rest > ) guard ( X eq Y ) do Z ,
                  when ( X , < < Y , Z > , as ... Rest > ) guard true do Var_pos [ X , Rest ] ,
                  when ( X , < < Y , Lvl , < Name , Args , Flag > > , as ... Rest > ) guard ( X eq Y ) do Calc_lvar [ Lvl , Name , Args , Flag ] ,
                  when ( X , < < Y , Lvl , < Name , Args , Flag > > , as ... Rest > ) guard true do Var_pos [ X , Rest ] ,
                  when ( X , < < Y , I , Z > , as ... Rest > ) guard ( X eq Y ) do Calc_var [ I , Z ] ,
                  when ( X , < < Y , I , Z > , as ... Rest > ) guard true do Var_pos [ X , Rest ]
                  end ,
    Mkvarlist [ ] = case
                    when ( <> , N ) guard true do <> ,
                    when ( < X , as ... Y > , N ) guard true do ( Mkvarlist [ Y , ( N + 1 ) ] ++ < < X , 0 , N > > )
                    end ,
    Mk_extern [ ] = case
                    when ( X , <> ) guard true do <> ,
                    when ( X , < < F , Syn > , as ... Rest > ) guard true do ( < ( 'extern EXPR_PTR ` ++ ( F ++ '();` ) ) > ++ Mk_extern [ X , Rest ] )
                    end ,
    Calc_var [ X , Y ] = if ( X eq 0 )
                         then ( 'red_copyarg ((*args)[` ++ ( Int_to_s [ Y ] ++ '])` ) )
                         else ( 'red_copyarg ((*((FRAME_PTR) ` ++ ( Calc_rec [ ( X - 1 ) ] ++ ( '))[` ++ ( Int_to_s [ Y ] ++ '])` ) ) ) ) ,
    Calc_rec [ ] = case
                   when 0 guard true do '(*args)[0]` ,
                   when Y guard true do ( '(*((FRAME_PTR) ` ++ ( Calc_rec [ ( Y - 1 ) ] ++ '))[0]` ) )
                   end ,
    Calc_lvar [ Lvl , Name , Args , Flag ] = if ( not ( Flag ) and ( Args eq 0 ) )
                                             then ( Name ++ '()` )
                                             else if Flag
                                                  then let Rightenv = if ( Lvl eq 0 )
                                                                      then 'args`
                                                                      else ( '(FRAME_PTR) ` ++ Calc_rec [ ( Lvl - 1 ) ] )
                                                  in if ( Args eq 0 )
                                                     then ( Name ++ ( ' (` ++ ( Rightenv ++ ')` ) ) )
                                                     else ( 'red_mkinn (` ++ ( Name ++ ( ', ` ++ ( Int_to_s [ ( Args + 1 ) ] ++ ( ', ` ++ ( Rightenv ++ ')` ) ) ) ) ) )
                                                  else ( 'red_mkact (` ++ ( Name ++ ( ', ` ++ ( Int_to_s [ Args ] ++ ')` ) ) ) ) ,
    Increase_lvl [ ] = case
                       when <> guard true do <> ,
                       when < < V , I , S > , as ... Rest > guard true do ( < < V , ( I + 1 ) , S > > ++ Increase_lvl [ Rest ] ) ,
                       when < < F , S > , as ... Rest > guard true do ( < < F , S > > ++ Increase_lvl [ Rest ] )
                       end ,
    Conv_to_c [ S ] = if empty ( S )
                      then '`
                      else ( ap case
                                when '""` guard true do '\""` ,
                                when '\` guard true do '\\` ,
                                when X guard true do X
                                end
                      to [ lselect [ 1 , S ] ] ++ Conv_to_c [ lcut [ 1 , S ] ] ) ,
    Spalte [ X , Y ] = if empty ( X )
                       then < Y , '` >
                       else if ( lselect ( 1 , X ) ne lcut ( ( ( ~1 * ldim ( X ) ) + 1 ) , X ) )
                            then < Y , X >
                            else Spalte [ lcut ( 1 , X ) , ( Y ++ lselect [ 1 , X ] ) ] ,
    Gen_scode [ Count , X , St ] = if empty ( X )
                                   then <>
                                   else letp < U , V > = Spalte [ X , '` ]
                                   in ( if empty ( U )
                                        then <>
                                        else < ( 'red_mksubs (` ++ ( St ++ ( ', ` ++ ( Int_to_s [ Count ] ++ ( ', ""` ++ ( Conv_to_c [ U ] ++ ( '"", ` ++ ( Int_to_s [ ldim ( U ) ] ++ ');` ) ) ) ) ) ) ) ) >                                    ++ if empty ( V )
                                       then <>
                                       else let New = ( '(*` ++ ( St ++ ( ')[` ++ ( Int_to_s [ ( Count + ldim ( U ) ) ] ++ ']` ) ) ) )
                                       in ( < ( New ++ ( ' = red_mksstr (` ++ ( Int_to_s [ ldim ( lselect ( 1 , V ) ) ] ++ ');` ) ) ) > ++ ( Gen_scode [ 0 , lselect ( 1 , V ) , ( New ++ '->s_ref` ) ] ++ Gen_scode [ ( ( Count + ldim ( U ) ) + 1 ) , lcut ( 1 , V ) , St ] ) )                                    ) ,
    Get_first [ ] = case
                    when <> guard true do <> ,
                    when < < A , B > , as ... C > guard true do ( < A > ++ Get_first [ C ] )
                    end ,
    Get_second [ ] = case
                     when <> guard true do <> ,
                     when < < A , B > , as ... C > guard true do ( < B > ++ Get_second [ C ] )
                     end ,
    Gen_case [ ] = case
                   when ( Liste , < Lvl , Env , < Name , Nlvl > , Flag , as ... Rest > ) guard true do letp < Funcs , Code , Newglob > = Gen_whens [ Liste , ( < Lvl , Env , < Name , ( Nlvl + 1 ) > , Flag > ++ Rest ) ]
                   in < ( Funcs ++ ( < ( 'EXPR_PTR ` ++ ( Name ++ ( Int_to_s [ Nlvl ] ++ '(args) /* local CASE */` ) ) ) , '` , 'FRAME_PTR args;` , '` , '{` , 'EXPR_PTR p, *s_p = stackpointer;` , '` , 'FRAME_PTR f;` , '` , ( 'DBUG_ENTER (""` ++ ( Name ++ ( Int_to_s [ Nlvl ] ++ '"");` ) ) ) , '` > ++ ( Code ++ < '}` , '` > ) ) ) , Newglob >
                   end ,
    Gen_whens [ ] = case
                    when ( Nomat { } , Global ) guard true do < <> , < 'fprintf (stderr, ""runtime error:\n\n"");` , 'fprintf (stderr, ""pattern not matching\n\n"");` , 'abnormal_term();` > , Global > ,
                    when ( < Arity , Vars , Patterns , Guard , Expr , Rest > , as < Lvl , Env , < Name , Nlvl > , Flag , as ... Rest1 > Global ) guard true do letp < Funcs , Code , < Lvl2 , Env2 , < Name2 , Nlvl2 > , Flag2 , as ... Rest2 > > = Gen_whencode [ Arity , Vars , Patterns , Guard , Expr , Global ]
                    in letp < Funcs2 , Code2 , < Lvl3 , Env3 , < Name3 , Nlvl3 > , Flag3 , as ... Rest3 > > = Gen_whens [ Rest , ( < Lvl , Env , < Name , Nlvl2 > , Flag > ++ Rest1 ) ]
                    in < ( Funcs2 ++ Funcs ) , ( Code ++ Code2 ) , ( < Lvl , Env , < Name , Nlvl3 > , Flag > ++ Rest1 ) > ,
                    when ( Expr , < Lvl , Env , < Name , Nlvl > , Flag , as ... Rest > ) guard true do letp < Funcs , Pre , S , Post , < Lvl2 , Env2 , < Name2 , Nlvl2 > , Flag2 , as ... Rest2 > > = Compile_expr [ Expr , ( < ~1 , Increase_lvl [ Env ] , < Name , Nlvl > , Flag > ++ Rest ) ]
                    in < Funcs , if empty ( Pre )
                                 then < ( 'DBUG_RETURN (` ++ ( S ++ ');` ) ) >
                                 else ( Pre ++ ( < ( 'p = ` ++ ( S ++ ';` ) ) > ++ ( Post ++ < 'DBUG_RETURN (p);` > ) ) ) ,                       ( < Lvl , Env , < Name , Nlvl2 > , Flag > ++ Rest ) >
                    end ,
    Mkletrec [ ] = case
                   when ( <> , Name , Lvl , Flag ) guard true do <> ,
                   when ( < < Fname , Sub { Arit , Args , Expr } > , as ... Rest > , Name , Lvl , Flag ) guard true do ( < < Fname , 0 , < ( Name ++ Int_to_s [ Lvl ] ) , Arit , Flag > > > ++ Mkletrec [ Rest , Name , ( Lvl + 1 ) , Flag ] ) ,
                   when ( < < Fname , Expr > , as ... Rest > , Name , Lvl , Flag ) guard true do ( < < Fname , 0 , < ( Name ++ Int_to_s [ Lvl ] ) , 0 , Flag > > > ++ Mkletrec [ Rest , Name , ( Lvl + 1 ) , Flag ] )
                   end ,
    Compileletrec [ ] = case
                        when ( Nreallvl , < < Realname , Sub { Arity , Vars , Expr } > , as ... Restletrec > , < Lvl , Env , < Name , Nlvl > , Flag , as ... Rest > ) guard true do letp < Fun1 , Pre1 , S1 , Pos1 , < Lvl1 , Env1 , < Name1 , Nlvl1 > , Flag1 , as ... Rest1 > > = Compile_expr [ Expr , ( < ~1 , ( Mkvarlist [ Vars , if Flag
                                                                                                                                                                                                                                                                                                                                        then 1
                                                                                                                                                                                                                                                                                                                                        else 0                                                                                                                                                                                          ] ++ Increase_lvl [ Env ] ) , < Name , Nlvl > , true > ++ Rest ) ]
                        in letp < Fun2 , Pre2 , S2 , Pos2 , < Lvl2 , Env2 , < Name2 , Nlvl2 > , Flag2 , as ... Rest2 > > = Compileletrec [ ( Nreallvl + 1 ) , Restletrec , ( < Lvl , Env , < Name , Nlvl1 > , Flag > ++ Rest ) ]
                        in < ( Fun2 ++ ( Fun1 ++ ( < ( 'EXPR_PTR ` ++ ( Name ++ ( Int_to_s [ Nreallvl ] ++ ( ' (args) /* function ` ++ ( Realname ++ ' of local letrec */` ) ) ) ) ) , '` , 'FRAME_PTR args;` , '` , '{` , 'EXPR_PTR p, *s_p = stackpointer;` , '` , ( 'DBUG_ENTER (""` ++ ( Name ++ ( Int_to_s [ Nreallvl ] ++ '"");` ) ) ) , '` > ++ ( Pre1 ++ ( Pos1 ++ < ( 'DBUG_RETURN (` ++ ( S1 ++ ');` ) ) , '}` , '` > ) ) ) ) ) , <> , <> , <> , ( < Lvl , Env , < Name , Nlvl2 > , Flag > ++ Rest ) > ,
                        when ( Nreallvl , < < Realname , Expr > , as ... Restletrec > , < Lvl , Env , < Name , Nlvl > , Flag , as ... Rest > ) guard true do letp < Fun1 , Pre1 , S1 , Pos1 , < Lvl1 , Env1 , < Name1 , Nlvl1 > , Flag1 , as ... Rest1 > > = Compile_expr [ Expr , ( < ~1 , Env , < Name , Nlvl > , Flag > ++ Rest ) ]
                        in letp < Fun2 , Pre2 , S2 , Pos2 , < Lvl2 , Env2 , < Name2 , Nlvl2 > , Flag2 , as ... Rest2 > > = Compileletrec [ ( Nreallvl + 1 ) , Restletrec , ( < Lvl , Env , < Name , Nlvl1 > , Flag > ++ Rest ) ]
                        in < ( Fun2 ++ ( Fun1 ++ ( < ( 'EXPR_PTR ` ++ ( Name ++ ( Int_to_s [ Nreallvl ] ++ ( if Flag
                                                                                                             then ' (args) /* function `
                                                                                                             else '() /* function `                         ++ ( Realname ++ ' of local letrec */` ) ) ) ) ) , '` , if Flag
                                                                                 then 'FRAME_PTR args;`
                                                                                 else '/* great, no args */` ,                           '` , '{` , 'EXPR_PTR p, *s_p = stackpointer;` , '` , ( 'DBUG_ENTER (""` ++ ( Name ++ ( Int_to_s [ Nreallvl ] ++ '"");` ) ) ) , '` > ++ ( Pre1 ++ ( Pos1 ++ < ( 'DBUG_RETURN (` ++ ( S1 ++ ');` ) ) , '}` , '` > ) ) ) ) ) , <> , <> , <> , ( < Lvl , Env , < Name , Nlvl2 > , Flag > ++ Rest ) > ,
                        when ( X , <> , Glob ) guard true do < <> , <> , '` , <> , Glob >
                        end ,
    Countr [ ] = case
                 when ( < as ... Rest , As { '...` , V } > , X ) guard true do X ,
                 when ( < as ... Rest , Pat > , X ) guard true do Countr [ Rest , ( X + 1 ) ]
                 end ,
    Pvr [ ] = case
              when ( <> , Ps , S , N , Sa , O , F ) guard true do < <> , N > ,
              when ( < '...` , as ... Rest > , Ps , S , N , Sa , O , F ) guard true do Pvr [ Rest , Ps , S , N , Sa , ( O + 1 ) , true ] ,
              when ( < As { '...` , V } , as ... Rest > , Ps , S , N , Sa , O , F ) guard true do letp < Nl1 , N1 > = Pvr [ Rest , Ps , S , ( N + 1 ) , Sa , ( O + 1 ) , true ]
              in < ( < ( S ++ ( Int_to_s [ N ] ++ ( '] = red_createstup(` ++ ( Ps ++ ( ', ` ++ ( Int_to_s [ O ] ++ ( ', ` ++ ( Sa ++ ( '-` ++ ( Int_to_s [ ( ldim [ Rest ] + 1 ) ] ++ ');` ) ) ) ) ) ) ) ) ) ) > ++ Nl1 ) , N1 > ,
              when ( < Pat , as ... Rest > , Ps , S , N , Sa , O , false ) guard true do letp < Nl1 , N1 > = Gen_pvars [ Pat , ( '(*` ++ ( Ps ++ ( ')[` ++ ( Int_to_s [ O ] ++ ']` ) ) ) ) , S , N ]
              in letp < Nl2 , N2 > = Pvr [ Rest , Ps , S , N1 , Sa , ( O + 1 ) , false ]
              in < ( Nl1 ++ Nl2 ) , N2 > ,
              when ( < Pat , as ... Rest > , Ps , S , N , Sa , O , true ) guard true do letp < Nl1 , N1 > = Gen_pvars [ Pat , ( '(*` ++ ( Ps ++ ( ')[` ++ ( ( Sa ++ ( '-` ++ Int_to_s [ ( ldim [ Rest ] + 1 ) ] ) ) ++ ']` ) ) ) ) , S , N ]
              in letp < Nl2 , N2 > = Pvr [ Rest , Ps , S , N1 , Sa , ( O + 1 ) , true ]
              in < ( Nl1 ++ Nl2 ) , N2 >
              end ,
    Bed_vars [ ] = case
                   when ( <> , Ps , S , N , O ) guard true do <> ,
                   when ( < Pat , as ... Rest > , Ps , S , N , O ) guard true do letp < Nl , N2 > = Gen_pvars [ Pat , ( Ps ++ ( Int_to_s [ O ] ++ ']` ) ) , S , N ]
                   in ( Nl ++ Bed_vars [ Rest , Ps , S , N2 , ( O + 1 ) ] )
                   end ,
    Gen_pvars [ ] = case
                    when ( Var { X } , Ps , S , N ) guard true do < < ( S ++ ( Int_to_s [ N ] ++ ( '] = red_copyarg(` ++ ( Ps ++ ');` ) ) ) ) > , ( N + 1 ) > ,
                    when ( As { Pat , V } , Ps , S , N ) guard true do letp < Nl , Nn > = Gen_pvars [ Pat , Ps , S , N ]
                    in < ( Nl ++ < ( S ++ ( Int_to_s [ Nn ] ++ ( '] = red_copyarg(` ++ ( Ps ++ ');` ) ) ) ) > ) , ( Nn + 1 ) > ,
                    when ( Tup { X , < '...` > } , Ps , S , N ) guard true do < <> , N > ,
                    when ( Tup { X , < As { '...` , X } > } , Ps , S , N ) guard true do < < ( S ++ ( Int_to_s [ N ] ++ ( '] = red_copyarg(` ++ ( Ps ++ ');` ) ) ) ) > , ( N + 1 ) > ,
                    when ( Tup { X , Args } , Ps , S , N ) guard true do ap case
                                                                            when 0 guard true do Pvlist [ Args , ( Ps ++ '->e_targ` ) , S , N , 0 ] ,
                                                                            when 1 guard true do Pvr [ Args , ( Ps ++ '->e_targ` ) , S , N , ( Ps ++ '->e_tari` ) , 0 , false ]
                                                                            otherwise < < 'red_paterror();` > , N >
                                                                            end
                    to [ Count_points [ Args ] ] ,
                    when ( Ccons { X , Tup { Y , Args } } , Ps , S , N ) guard true do letp < Nl1 , N1 > = ap case
                                                                                                              when Var { I } guard true do < < ( S ++ ( Int_to_s [ N ] ++ ( '] = red_convs(` ++ ( Ps ++ '->e_cnam);` ) ) ) ) > , ( N + 1 ) >
                                                                                                              otherwise < <> , N >
                                                                                                              end
                                                                                            to [ X ]
                    in ap case
                          when < '...` > guard true do < Nl1 , N1 > ,
                          when < As { '...` , V } > guard true do < ( Nl1 ++ < ( S ++ ( Int_to_s [ N1 ] ++ ( '] = red_convctot(` ++ ( Ps ++ ');` ) ) ) ) > ) , N1 > ,
                          when Tralala guard true do ap case
                                                        when 0 guard true do letp < Nl2 , N2 > = Pvlist [ Args , ( Ps ++ '->e_carg` ) , S , N , 0 ]
                                                        in < ( Nl1 ++ Nl2 ) , N2 > ,
                                                        when 1 guard true do letp < Nl2 , N2 > = Pvr [ Args , ( Ps ++ '->e_carg` ) , S , N1 , ( Ps ++ '->e_cari` ) , 0 , false ]
                                                        in < ( Nl1 ++ Nl2 ) , N2 >
                                                        otherwise < < 'red_paterror();` > , N >
                                                        end
                          to [ Count_points [ Args ] ]
                          end
                    to [ Args ] ,
                    when ( X , Y , Z , U ) guard true do < <> , U >
                    end ,
    Pat_pointsl [ ] = case
                      when ( < ... , '...` > , S , N , A ) guard true do <> ,
                      when ( < ... , As { '...` , X } > , S , N , A ) guard true do <> ,
                      when ( < as ... Rest , Pat > , S , N , A ) guard true do ( Pat_pointsl [ Rest , S , ( N + 1 ) , A ] ++ Gen_pbed [ Pat , ( '(*` ++ ( S ++ ( ')[` ++ ( A ++ ( if ( N gt 0 )
                                                                                                                                                                                  then ( '-` ++ Int_to_s [ N ] )
                                                                                                                                                                                  else '`                       ++ ']` ) ) ) ) ) ] )
                      end ,
    Gencond [ ] = case
                  when ( 0 , <> ) guard true do < 'if (TRUE)` > ,
                  when ( X , <> ) guard true do < 'TRUE )` > ,
                  when ( 0 , < First , as ... Rest > ) guard true do ( < ( 'if (` ++ ( First ++ ' &&` ) ) > ++ Gencond [ 1 , Rest ] ) ,
                  when ( 15 , < First , as ... Rest > ) guard true do ( < ( First ++ ' )` ) > ++ Gencond [ 0 , Rest ] ) ,
                  when ( X , < First , as ... Rest > ) guard true do ( < ( First ++ ' &&` ) > ++ Gencond [ ( X + 1 ) , Rest ] )
                  end ,
    Pvlist [ ] = case
                 when ( <> , Ps , S , N , O ) guard true do < <> , N > ,
                 when ( < Pat , as ... Rest > , Ps , S , N , O ) guard true do letp < Nl1 , N1 > = Gen_pvars [ Pat , ( '(*` ++ ( Ps ++ ( ')[` ++ ( Int_to_s [ O ] ++ ']` ) ) ) ) , S , N ]
                 in letp < Nl2 , N2 > = Pvlist [ Rest , Ps , S , N1 , ( O + 1 ) ]
                 in < ( Nl1 ++ Nl2 ) , N2 >
                 end ,
    Pat_pointsr [ ] = case
                      when ( < '...` , ... > , S , N ) guard true do <> ,
                      when ( < As { '...` , X } , ... > , S , N ) guard true do <> ,
                      when ( < Pat , as ... Rest > , S , N ) guard true do ( Gen_pbed [ Pat , ( '(*` ++ ( S ++ ( ')[` ++ ( Int_to_s [ N ] ++ ']` ) ) ) ) ] ++ Pat_pointsr [ Rest , S , ( N + 1 ) ] )
                      end ,
    Gen_whencode [ Arity , Vars , Patterns , Guar , Expr , Global ] = ap case
                                                                         when < Lvl , Env , < Name , Nlvl > , Flag , as ... Rest > guard true do letp < Fun1 , Pre1 , S1 , Pos1 , < Lvl1 , Env1 , < Name1 , Nlvl1 > , Flag1 , as ... Rest1 > > = Compile_expr [ Expr , ( < ~1 , ( Mkvarlist [ Vars , if Flag
                                                                                                                                                                                                                                                                                                     then 1
                                                                                                                                                                                                                                                                                                     else 0                                                                                                                                                       ] ++ Increase_lvl [ Env ] ) , < Name , ( Nlvl + 2 ) > , ( Flag or ( Arity gt 0 ) ) > ++ Rest ) ]
                                                                         in letp < Fun2 , Pre2 , S2 , Pos2 , < Lvl2 , Env2 , < Name2 , Nlvl2 > , Flag2 , as ... Rest2 > > = Compile_expr [ Guar , ( < ~1 , ( Mkvarlist [ Vars , if Flag
                                                                                                                                                                                                                                then 1
                                                                                                                                                                                                                                else 0                                                                                  ] ++ Increase_lvl [ Env ] ) , < Name , Nlvl1 > , ( Flag or ( Arity gt 0 ) ) > ++ Rest ) ]
                                                                         in < ( Fun2 ++ ( Fun1 ++ ( < ( 'EXPR_PTR ` ++ ( Name ++ ( Int_to_s [ ( Nlvl + 1 ) ] ++ ( if ( Flag or ( Arity gt 0 ) )
                                                                                                                                                                  then ' (args) `
                                                                                                                                                                  else ' () `                                                                          ++ '/* expr-term of when-term */` ) ) ) ) , '` , if ( Flag or ( Arity gt 0 ) )
                                                                                                                           then 'FRAME_PTR args;`
                                                                                                                           else '/* great, no args */` ,                                                                            '` , '{` , 'EXPR_PTR p, *s_p = stackpointer;` , '` , ( 'DBUG_ENTER (""` ++ ( Name ++ ( Int_to_s [ ( Nlvl + 1 ) ] ++ '"");` ) ) ) , '` > ++ ( Pre1 ++ ( Pos1 ++ ( < ( 'DBUG_RETURN (` ++ ( S1 ++ ');` ) ) , '}` , '` > ++ ( < ( 'EXPR_PTR ` ++ ( Name ++ ( Int_to_s [ Nlvl ] ++ ( if ( Flag or ( Arity gt 0 ) )
                                                                                                                                                                                                                                                                                                                                                             then ' (args) `
                                                                                                                                                                                                                                                                                                                                                             else ' () `                                                                          ++ '/* guard-term of when-term */` ) ) ) ) , '` , if ( Flag or ( ldim [ Vars ] gt 0 ) )
                                                                                                                            then 'FRAME_PTR args;`
                                                                                                                            else '/* great, no args */` ,                                                                            '` , '{` , 'EXPR_PTR p, *s_p = stackpointer;` , '` , ( 'DBUG_ENTER (""` ++ ( Name ++ ( Int_to_s [ Nlvl ] ++ '"");` ) ) ) , '` > ++ ( Pre2 ++ ( Pos2 ++ < ( 'DBUG_RETURN (` ++ ( S2 ++ ');` ) ) , '}` , '` > ) ) ) ) ) ) ) ) ) , ( Gencond [ 0 , Bedlist [ Patterns , if Flag
                                                                                                                                                                                                                                                                                                                                                 then 1
                                                                                                                                                                                                                                                                                                                                                 else 0 ,                                                                            '(*args)[` ] ] ++ ( < '{` , if ( Flag or ( Arity gt 0 ) )
                                                                                                        then ( 'f = (FRAME_PTR) get_cell (` ++ ( Int_to_s [ ( if Flag
                                                                                                                                                              then 1
                                                                                                                                                              else 0                                                                                                         + Arity ) ] ++ '*sizeof(EXPR_PTR));` ) )
                                                                                                        else '/* no activation-record for expr- and guard-terms ! */` ,                                                                            '` > ++ ( ( if Flag
                                                                                        then < '(*f)[0] = (*args)[0];` >
                                                                                        else <>                                                                          ++ if ( Arity gt 0 )
                                                                             then Bed_vars [ Patterns , '(*args)[` , '(*f)[` , if Flag
                                                                                                                               then 1
                                                                                                                               else 0 ,                                                                                if Flag
                                                                                then 1
                                                                                else 0                                                                              ]
                                                                             else <>                                                                          ) ++ < ( 'p = ` ++ ( Name ++ ( Int_to_s [ Nlvl ] ++ ( '(` ++ if ( Flag or ( Arity gt 0 ) )
                                                                                                                                       then 'f);`
                                                                                                                                       else ');`                                                                          ) ) ) ) , '` , 'if ((p->e_class == E_BOOL) && p->e_num)` , ( 'DBUG_RETURN (` ++ ( Name ++ ( Int_to_s [ ( Nlvl + 1 ) ] ++ if ( Flag or ( Arity gt 0 ) )
                                                                                                                                                                                                   then '(f));`
                                                                                                                                                                                                   else '());`                                                                          ) ) ) , '}` , '` > ) ) ) , ( < Lvl , Env , < Name , Nlvl2 > , Flag > ++ Rest2 ) >
                                                                         end
                                                                      to [ Global ] ,
    Count_points [ ] = case
                       when <> guard true do 0 ,
                       when < '...` , as ... Rest > guard true do ( 1 + Count_points [ Rest ] ) ,
                       when < As { '...` , X } , as ... Rest > guard true do ( 1 + Count_points [ Rest ] ) ,
                       when < X , as ... Rest > guard true do Count_points [ Rest ]
                       end ,
    Bedlist [ ] = case
                  when ( <> , N , S ) guard true do <> ,
                  when ( < Pat , as ... Rest > , N , S ) guard true do ( Gen_pbed [ Pat , ( S ++ ( Int_to_s [ N ] ++ ']` ) ) ] ++ Bedlist [ Rest , ( N + 1 ) , S ] )
                  end ,
    Gen_plist [ ] = case
                    when ( <> , S , N ) guard true do <> ,
                    when ( < Pat , as ... Rest > , S , N ) guard true do ( Gen_pbed [ Pat , ( '(*` ++ ( S ++ ( ')[` ++ ( Int_to_s [ N ] ++ ']` ) ) ) ) ] ++ Gen_plist [ Rest , S , ( N + 1 ) ] )
                    end ,
    Gen_pbed [ Pat , S ] = ap case
                              when Int { I } guard true do < ( '(` ++ ( S ++ '->e_class == E_NUM)` ) ) , ( '(` ++ ( S ++ ( '->e_num == ` ++ ( Int_to_s [ I ] ++ ')` ) ) ) ) > ,
                              when Boolean { true } guard true do < ( '(` ++ ( S ++ '->e_class == E_BOOL)` ) ) , ( S ++ '->e_num` ) > ,
                              when Boolean { false } guard true do < ( '(` ++ ( S ++ '->e_class == E_BOOL)` ) ) , ( '!` ++ ( S ++ '->e_num` ) ) > ,
                              when Str { St } guard true do ( < ( '(` ++ ( S ++ '->e_class == E_STRING)` ) ) > ++ Pat_string [ 0 , ( S ++ '->e_union.e_string` ) , St ] ) ,
                              when Var { X } guard true do <> ,
                              when '.` guard true do <> ,
                              when As { P , Var } guard true do Gen_pbed [ P , S ] ,
                              when Tup { 1 , < '...` > } guard true do < ( '(` ++ ( S ++ '->e_class == E_TUP)` ) ) > ,
                              when Tup { 1 , < As { '...` , X } > } guard true do < ( '(` ++ ( S ++ '->e_class == E_TUP)` ) ) > ,
                              when Tup { X , Args } guard true do ( < ( '(` ++ ( S ++ '->e_class == E_TUP)` ) ) > ++ ap case
                                                                                                                        when 0 guard true do ( < ( '(` ++ ( S ++ ( '->e_tari == ` ++ ( Int_to_s [ X ] ++ ')` ) ) ) ) > ++ Gen_plist [ Args , ( S ++ '->e_targ` ) , 0 ] ) ,
                                                                                                                        when 1 guard true do ( < ( '(` ++ ( S ++ ( '->e_tari >= ` ++ ( Int_to_s [ ( X - 1 ) ] ++ ')` ) ) ) ) > ++ ( Pat_pointsr [ Args , ( S ++ '->e_targ` ) , 0 ] ++ Pat_pointsl [ Args , ( S ++ '->e_targ` ) , 1 , ( S ++ '->e_tari` ) ] ) ) ,
                                                                                                                        when X guard true do < 'red_paterror()` >
                                                                                                                        end
                              to [ Count_points [ Args ] ] ) ,
                              when Ccons { Title , Arg } guard true do ( < ( '(` ++ ( S ++ '->e_class == E_CCONS)` ) ) > ++ ( ap case
                                                                                                                                 when Var { Y } guard true do <> ,
                                                                                                                                 when Str { Y } guard true do < ( '(strcmp(""` ++ ( Conv_to_c [ Y ] ++ ( '"",` ++ ( S ++ '->e_cnam) == 0)` ) ) ) ) >
                                                                                                                                 end
                              to [ Title ] ++ ap case
                                                 when Tup { 1 , < '...` > } guard true do <> ,
                                                 when Tup { 1 , < As { '...` , X } > } guard true do <> ,
                                                 when Tup { X , Args } guard true do ap case
                                                                                        when 0 guard true do ( < ( '(` ++ ( S ++ ( '->e_cari == ` ++ ( Int_to_s [ X ] ++ ')` ) ) ) ) > ++ Gen_plist [ Args , ( S ++ '->e_carg` ) , 0 ] ) ,
                                                                                        when 1 guard true do ( < ( '(` ++ ( S ++ ( '->e_cari >= ` ++ ( Int_to_s [ ( X - 1 ) ] ++ ')` ) ) ) ) > ++ ( Pat_pointsr [ Args , ( S ++ '->e_carg` ) , 0 ] ++ Pat_pointsl [ Args , ( S ++ '->e_carg` ) , 0 , ( S ++ '->e_cari` ) ] ) ) ,
                                                                                        when X guard true do < 'red_paterror()` >
                                                                                        end
                                                 to [ Count_points [ Args ] ]
                                                 end
                              to [ Arg ] ) )
                              otherwise < 'red_patgerror()` >
                              end
                           to [ Pat ] ,
    Pat_string [ Count , S , St ] = if empty ( St )
                                    then < ( '(` ++ ( S ++ '->s_tag == 0)` ) ) >
                                    else ( if ( Count eq 0 )
                                           then < ( '(` ++ ( S ++ ( '->s_tag == ` ++ ( Int_to_s [ ldim [ St ] ] ++ ')` ) ) ) ) >
                                           else <>                                     ++ letp < X , Y > = Spalte [ St , '` ]
                                    in ( if empty ( X )
                                         then <>
                                         else < ( 'red_checkss(""` ++ ( Conv_to_c [ X ] ++ ( '"",` ++ ( Int_to_s [ Count ] ++ ( ',` ++ ( Int_to_s [ ldim [ X ] ] ++ ( ',` ++ ( S ++ '->s_union.s_ref)` ) ) ) ) ) ) ) ) >                                     ++ if empty ( Y )
                                        then <>
                                        else ( Pat_string [ 0 , ( '(*` ++ ( S ++ ( '->s_union.s_ref)[` ++ ( Int_to_s [ ( Count + ldim [ X ] ) ] ++ ']` ) ) ) ) , lselect [ 1 , Y ] ] ++ if ( ldim ( Y ) gt 1 )
                                                                                                                                                                                        then Pat_string [ ( ( Count + ldim [ X ] ) + 1 ) , S , lcut [ 1 , Y ] ]
                                                                                                                                                                                        else <>                                         )                                     ) )
in Compile [ Int { 1 } ]
