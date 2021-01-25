def 
  W [   ] = /* 'W[<S,E,K,Ng>]: milners type inference algorithm (extended)` */
            case 
            when < S , C{. , T} , K , Ng > guard true do W{S , T , K} ,
            when < S , F{. , T} , K , Ng > guard true do W{S , Subst [ < S , T > ] , K} ,
            when < S , V{. , as T{. , 0} T} , K , Ng > guard true do W{S , Subst [ < S , T > ] , K} ,
            when < S , V{. , as T{. , M} T} , K , Ng > guard true do let T0 = Subst [ < S , T{M , 0} > ]
                                                                     in ap case 
                                                                           when < . , T1 , K1 > guard true do W{( S ++ < T , T1 > ) , T1 , K1}
                                                                           end
                                                                        to [ NewVs [ < <> , T0 , K , NgVs [ S , Ng ] > ] ] ,
            when < S , A{F , A , T} , K , Ng > guard true do ap case 
                                                                when W{S1 , T1 , K1} guard true do ap case 
                                                                                                      when W{S2 , T2 , K2} guard true do let U = Unify [ Subst [ < S2 , T1 > ] , Arr{T2 , T} ]
                                                                                                                                         in if ( U eq false )
                                                                                                                                            then Error{'(W/Ap) cannot unify  ` , Subst [ < S2 , T1 > ] , '  with  ` , Arr{T2 , T}}
                                                                                                                                            else W{( S2 ++ U ) , Subst [ < U , T > ] , K2} ,
                                                                                                      when X guard true do X
                                                                                                      end
                                                                                                   to [ W [ < S1 , A , K1 , Ng > ] ] ,
                                                                when X guard true do X
                                                                end
                                                             to [ W [ < S , F , K , Ng > ] ] ,
            when < S , I{P , C , A} , K , Ng > guard true do ap case 
                                                                when W{S0 , T0 , K0} guard true do ap case 
                                                                                                      when W{S1 , T1 , K1} guard true do ap case 
                                                                                                                                            when W{S2 , T2 , K2} guard true do let U = Unify [ Subst [ < S2 , Arr{T0 , T1} > ] , Arr{'bool` , T2} ]
                                                                                                                                                                               in if ( U eq false )
                                                                                                                                                                                  then Error{'(W/If) cannot unify  ` , Subst [ < S2 , Arr{T0 , T1} > ] , '  with  ` , Arr{'bool` , T2}}
                                                                                                                                                                                  else W{( S2 ++ U ) , Subst [ < U , T2 > ] , K2} ,
                                                                                                                                            when X guard true do X
                                                                                                                                            end
                                                                                                                                         to [ W [ < S1 , A , K1 , Ng > ] ] ,
                                                                                                      when X guard true do X
                                                                                                      end
                                                                                                   to [ W [ < S0 , C , K0 , Ng > ] ] ,
                                                                when X guard true do X
                                                                end
                                                             to [ W [ < S , P , K , Ng > ] ] ,
            when < S , L{. , T , E} , K , Ng > guard true do ap case 
                                                                when W{S1 , T1 , K1} guard true do W{S1 , Arr{Subst [ < S1 , T > ] , T1} , K1} ,
                                                                when X guard true do X
                                                                end
                                                             to [ W [ < S , E , K , ( Ng ++ < T > ) > ] ] ,
            when < S , Let{. , T , F , E} , K , Ng > guard true do ap case 
                                                                      when W{S1 , T1 , K1} guard true do W [ < ( S1 ++ < T , T1 > ) , E , K1 , Ng > ] ,
                                                                      when X guard true do X
                                                                      end
                                                                   to [ W [ < S , F , K , Ng > ] ] ,
            when < S , Rec{. , T , F , E} , K , Ng > guard true do ap case 
                                                                      when W{S1 , T1 , K1} guard true do let U = Unify [ Subst [ < S1 , T > ] , T1 ]
                                                                                                         in if ( U eq false )
                                                                                                            then Error{'(W/Rec) cannot unify  ` , Subst [ < S1 , T > ] , '  with  ` , T1}
                                                                                                            else W [ < ( S1 ++ U ) , E , K1 , Ng > ] ,
                                                                      when X guard true do X
                                                                      end
                                                                   to [ W [ < S , F , K , ( Ng ++ < T > ) > ] ] ,
            when < S , Lrec{. , as < T > Tl , < F > , E} , K , Ng > guard true do ap case 
                                                                                     when W{S1 , T1 , K1} guard true do let U = Unify [ Subst [ < S1 , T > ] , T1 ]
                                                                                                                        in if ( U eq false )
                                                                                                                           then Error{'(W/Lrec) cannot unify  ` , Subst [ < S1 , T > ] , '  with  ` , T1}
                                                                                                                           else W [ < ( S1 ++ U ) , E , K1 , Ng > ] ,
                                                                                     when X guard true do X
                                                                                     end
                                                                                  to [ W [ < S , F , K , ( Ng ++ Tl ) > ] ] ,
            when < S , Lrec{. , as < T , as ... Ts > Tl , < F , as ... Fs > , E} , K , Ng > guard true do ap case 
                                                                                                             when W{S1 , T1 , K1} guard true do let U = Unify [ Subst [ < S1 , T > ] , T1 ]
                                                                                                                                                in if ( U eq false )
                                                                                                                                                   then Error{'(W/Lrec) cannot unify  ` , Subst [ < S1 , T > ] , '  with  ` , T1}
                                                                                                                                                   else W [ < ( S1 ++ U ) , Lrec{<> , Ts , Fs , E} , K1 , Ng > ] ,
                                                                                                             when X guard true do X
                                                                                                             end
                                                                                                          to [ W [ < S , F , K , ( Ng ++ Tl ) > ] ] ,
            when < S , LrecM{. , Ts , Fs , E} , K , Ng > guard true do ap case 
                                                                          when W{S1 , T1 , K1} guard true do let U = Unify [ Subst [ < S1 , Ts > ] , T1 ]
                                                                                                             in if ( U eq false )
                                                                                                                then Error{'(W/LrecM) cannot unify  ` , Subst [ < S1 , Ts > ] , '  with  ` , T1}
                                                                                                                else W [ < ( S1 ++ U ) , E , K1 , Ng > ] ,
                                                                          when X guard true do X
                                                                          end
                                                                       to [ W [ < S , Fs , K , ( Ng ++ Ts ) > ] ] ,
            when < S , Lst{Hd , Tl} , K , Ng > guard true do ap case 
                                                                when W{S1 , T1 , K1} guard true do ap case 
                                                                                                      when W{S2 , T2 , K2} guard true do let U = Unify [ Lst{Subst [ < S2 , T1 > ]} , T2 ]
                                                                                                                                         in if ( U eq false )
                                                                                                                                            then Error{'(W/Lst) cannot unify  ` , Lst{Subst [ < S2 , T1 > ]} , '  with  ` , T2}
                                                                                                                                            else W{( S2 ++ U ) , Subst [ < U , T2 > ] , K2} ,
                                                                                                      when X guard true do X
                                                                                                      end
                                                                                                   to [ W [ < S1 , Tl , K1 , Ng > ] ] ,
                                                                when X guard true do X
                                                                end
                                                             to [ W [ < S , Hd , K , Ng > ] ] ,
            when < S , Tup{as ... Xs} , K , Ng > guard true do ap case 
                                                                  when W{S1 , T1 , K1} guard true do W{S1 , ( Tup{ } ++ T1 ) , K1} ,
                                                                  when X guard true do X
                                                                  end
                                                               to [ W [ < S , Xs , K , Ng > ] ] ,
            when < S , < as '_"... ` Con , as ... Xs > , K , Ng > guard true do ap case 
                                                                                   when W{S1 , T1 , K1} guard true do W{S1 , ( < Con > ++ Xs ) , K1} ,
                                                                                   when X guard true do X
                                                                                   end
                                                                                to [ W [ < S , Xs , K , Ng > ] ] ,
            when < S , < X > , K , Ng > guard true do ap case 
                                                         when W{S1 , T1 , K1} guard true do W{S1 , < T1 > , K1} ,
                                                         when X guard true do X
                                                         end
                                                      to [ W [ < S , X , K , Ng > ] ] ,
            when < S , < X , as ... Xs > , K , Ng > guard true do ap case 
                                                                     when W{S1 , T1 , K1} guard true do ap case 
                                                                                                           when W{S2 , T2 , K2} guard true do W{S2 , ( < Subst [ < S2 , T1 > ] > ++ T2 ) , K2} ,
                                                                                                           when X guard true do X
                                                                                                           end
                                                                                                        to [ W [ < S1 , Xs , K1 , Ng > ] ] ,
                                                                     when X guard true do X
                                                                     end
                                                                  to [ W [ < S , X , K , Ng > ] ] ,
            when < S , <> , K , Ng > guard true do W{S , <> , K} ,
            when X guard true do Error{'(W) syntax error` , X}
            end ,
  Unify [ X , Y ] = /* 'Unify[X,Y]: robinsons unification algorithm` */
                    /* 'extern: Constant,Variable,EqVar,Equal,Subst` */
                    def 
                      Unif2 [   ] = case 
                                    when < X , Y , S > guard Variable [ X ] do if ( X EqVar Y )
                                                                               then S
                                                                               else if Occur [ < X , Y > ]
                                                                                    then false
                                                                                    else ( S ++ < X , Y > ) ,
                                    when < X , Y , S > guard Variable [ Y ] do Unif2 [ < Y , X , S > ] ,
                                    when < < as '_"... ` K1 , as ... Xs > , < as '_"... ` K2 , as ... Ys > , S > guard true do if ( ( K1 eq K2 ) and ( ldim( Xs ) eq ldim( Ys ) ) )
                                                                                                                               then Unifn [ < Xs , Ys , S > ]
                                                                                                                               else false ,
                                    when < < as ... Xs > , < as ... Ys > , S > guard true do if ( ldim( Xs ) eq ldim( Ys ) )
                                                                                             then Unifn [ < Xs , Ys , S > ]
                                                                                             else false ,
                                    when < X , Y , S > guard ( X Equal Y ) do S
                                    otherwise false
                                    end ,
                      Unifn [   ] = case 
                                    when < < X , as ... Xs > , < Y , as ... Ys > , S > guard true do let U = Unif2 [ < Subst [ < S , X > ] , Subst [ < S , Y > ] , S > ]
                                                                                                     in if ( U eq false )
                                                                                                        then false
                                                                                                        else Unifn [ < Xs , Ys , U > ] ,
                                    when < <> , . , S > guard true do S
                                    otherwise false
                                    end ,
                      Occur [   ] = case 
                                    when < V , X > guard Variable [ X ] do ( V EqVar X ) ,
                                    when < V , X > guard Constant [ X ] do false ,
                                    when < V , < '_"... ` , as ... Xs > > guard true do Occur [ < V , Xs > ] ,
                                    when < V , < X , as ... Xs > > guard true do if Occur [ < V , X > ]
                                                                                 then true
                                                                                 else Occur [ < V , Xs > ]
                                    otherwise false
                                    end
                    in Unif2 [ < X , Y , <> > ] ,
  Subst [   ] = /* 'Subst[<S,E>]: substitution of variables in E by terms according S` */
                /* 'extern: Variable,EqVar` */
                case 
                when < < X , E , as ... R > , Y > guard ( X EqVar Y ) do Subst [ < R , E > ] ,
                when < < . , . , as ... R > , Y > guard Variable [ Y ] do Subst [ < R , Y > ] ,
                when < <> , E > guard true do E ,
                when < S , < as '_"... ` K , as ... Xs > > guard true do ( < K > ++ Subst [ < S , Xs > ] ) ,
                when < S , < X , as ... Xs > > guard true do ( < Subst [ < S , X > ] > ++ Subst [ < S , Xs > ] ) ,
                when < S , C > guard true do C
                otherwise error: '(Subst) syntax error`
                end ,
  NewVs [   ] = /* 'NewVs[<S,T,K,Ng>]: replace all generic variables in T by fresh ones` */
                case 
                when < S , as T{N , ...} T , K , Ng > guard ( T VarIsIn Ng ) do < S , T{N , 0} , K > ,
                when < S , as T{...} T , K , Ng > guard true do let T1 = Find [ T , S ]
                                                                in if ( T1 eq false )
                                                                   then < ( S ++ < T , T{K , 0} > ) , T{K , 0} , ( K + 1 ) >
                                                                   else < S , T1 , K > ,
                when < S , < as '_"... ` Con , as ... Xs > , K , Ng > guard true do ap case 
                                                                                       when < S1 , T1 , K1 > guard true do < S1 , ( < Con > ++ T1 ) , K1 >
                                                                                       end
                                                                                    to [ NewVs [ < S , Xs , K , Ng > ] ] ,
                when < S , < X , as ... Xs > , K , Ng > guard true do ap case 
                                                                         when < S1 , T1 , K1 > guard true do ap case 
                                                                                                                when < S2 , T2 , K2 > guard true do < S2 , ( < T1 > ++ T2 ) , K2 >
                                                                                                                end
                                                                                                             to [ NewVs [ < S1 , Xs , K1 , Ng > ] ]
                                                                         end
                                                                      to [ NewVs [ < S , X , K , Ng > ] ] ,
                when < S , C , K , Ng > guard true do < S , C , K >
                end ,
  NgVs [ S , Ng ] = 
                    if empty( Ng )
                    then <>
                    else ( Ng ++ Vars [ Subst [ < S , Ng > ] ] ) ,
  Tvars [ E ] =
                def 
                  Tv [   ] = case 
                             when < true , Env , Cnt > guard true do < C{'true` , 'bool`} , Env , Cnt > ,
                             when < false , Env , Cnt > guard true do < C{'false` , 'bool`} , Env , Cnt > ,
                             when < + , Env , Cnt > guard true do < C{'+` , Arr{'num` , Arr{'num` , 'num`}}} , Env , Cnt > ,
                             when < - , Env , Cnt > guard true do < C{'-` , Arr{'num` , Arr{'num` , 'num`}}} , Env , Cnt > ,
                             when < * , Env , Cnt > guard true do < C{'*` , Arr{'num` , Arr{'num` , 'num`}}} , Env , Cnt > ,
                             when < lt , Env , Cnt > guard true do < C{'lt` , Arr{'num` , Arr{'num` , 'bool`}}} , Env , Cnt > ,
                             when < le , Env , Cnt > guard true do < C{'le` , Arr{'num` , Arr{'num` , 'bool`}}} , Env , Cnt > ,
                             when < eq , Env , Cnt > guard true do < C{'eq` , Arr{'num` , Arr{'num` , 'bool`}}} , Env , Cnt > ,
                             when < 'not` , Env , Cnt > guard true do < C{'not` , Arr{'bool` , 'bool`}} , Env , Cnt > ,
                             when < 'neg` , Env , Cnt > guard true do < C{'neg` , Arr{'num` , 'num`}} , Env , Cnt > ,
                             when < <> , Env , Cnt > guard true do < F{'nil` , Lst{T{Cnt , 0}}} , Env , ( Cnt + 1 ) > ,
                             when < 'empty` , Env , Cnt > guard true do < F{'empty` , Arr{Lst{T{Cnt , 0}} , 'bool`}} , Env , ( Cnt + 1 ) > ,
                             when < 'hd` , Env , Cnt > guard true do < F{'hd` , Arr{Lst{T{Cnt , 0}} , T{Cnt , 0}}} , Env , ( Cnt + 1 ) > ,
                             when < 'tl` , Env , Cnt > guard true do < F{'tl` , Arr{Lst{T{Cnt , 0}} , Lst{T{Cnt , 0}}}} , Env , ( Cnt + 1 ) > ,
                             when < 'cons` , Env , Cnt > guard true do < F{'cons` , Arr{T{Cnt , 0} , Arr{Lst{T{Cnt , 0}} , Lst{T{Cnt , 0}}}}} , Env , ( Cnt + 1 ) > ,
                             when < as number X , Env , Cnt > guard true do < C{X , 'num`} , Env , Cnt > ,
                             when < as string X , Env , Cnt > guard true do ap case 
                                                                               when as T{. , 0} T guard true do < V{X , T} , Env , Cnt > ,
                                                                               when as T{N , .} T guard true do < V{X , T{Cnt , N}} , Env , ( Cnt + 1 ) > ,
                                                                               when false guard true do < V{X , T{Cnt , 0}} , ( Env ++ < X , T{Cnt , 0} > ) , ( Cnt + 1 ) >
                                                                               end
                                                                            to [ Find [ X , Env ] ] ,
                             when < L{X , E} , Env , Cnt > guard true do ap case 
                                                                            when < Te , En , Cn > guard true do < L{X , T{Cnt , 0} , Te} , lcut( 2 , En ) , Cn >
                                                                            end
                                                                         to [ Tv [ < E , ( < X , T{Cnt , 0} > ++ Env ) , ( Cnt + 1 ) > ] ] ,
                             when < A{E1 , E2} , Env , Cnt > guard true do ap case 
                                                                              when < Te1 , En1 , Cn1 > guard true do ap case 
                                                                                                                        when < Te2 , En2 , Cn2 > guard true do < A{Te1 , Te2 , T{Cn2 , 0}} , En2 , ( Cn2 + 1 ) >
                                                                                                                        end
                                                                                                                     to [ Tv [ < E2 , En1 , Cn1 > ] ]
                                                                              end
                                                                           to [ Tv [ < E1 , Env , Cnt > ] ] ,
                             when < I{E0 , E1 , E2} , Env , Cnt > guard true do ap case 
                                                                                   when < Te0 , En0 , Cn0 > guard true do ap case 
                                                                                                                             when < Te1 , En1 , Cn1 > guard true do ap case 
                                                                                                                                                                       when < Te2 , En2 , Cn2 > guard true do < I{Te0 , Te1 , Te2} , En2 , Cn2 >
                                                                                                                                                                       end
                                                                                                                                                                    to [ Tv [ < E2 , En1 , Cn1 > ] ]
                                                                                                                             end
                                                                                                                          to [ Tv [ < E1 , En0 , Cn0 > ] ]
                                                                                   end
                                                                                to [ Tv [ < E0 , Env , Cnt > ] ] ,
                             when < Let{X , F , E} , Env , Cnt > guard true do ap case 
                                                                                  when < Te1 , En1 , Cn1 > guard true do ap case 
                                                                                                                            when < Te2 , En2 , Cn2 > guard true do < Let{X , T{Cn1 , 0} , Te1 , Te2} , lcut( 2 , En2 ) , Cn2 >
                                                                                                                            end
                                                                                                                         to [ Tv [ < E , ( < X , T{Cn1 , 1} > ++ En1 ) , ( Cn1 + 1 ) > ] ]
                                                                                  end
                                                                               to [ Tv [ < F , Env , Cnt > ] ] ,
                             when < Rec{X , F , E} , Env , Cnt > guard true do ap case 
                                                                                  when < Te1 , En1 , Cn1 > guard true do ap case 
                                                                                                                            when < Te2 , En2 , Cn2 > guard true do < Rec{X , T{Cnt , 0} , Te1 , Te2} , lcut( 2 , En2 ) , Cn2 >
                                                                                                                            end
                                                                                                                         to [ Tv [ < E , ( < X , T{Cnt , 1} > ++ lcut( 2 , En1 ) ) , Cn1 > ] ]
                                                                                  end
                                                                               to [ Tv [ < F , ( < X , T{Cnt , 0} > ++ Env ) , ( Cnt + 1 ) > ] ] ,
                             when < Lrec{Xs , Fs , E} , Env , Cnt > guard ( ldim( Xs ) eq ldim( Fs ) ) do def 
                                                                                                            Tvs [ N , K , Tag ] = if ( N eq 0 )
                                                                                                                                  then <>
                                                                                                                                  else ( < T{K , Tag} > ++ Tvs [ ( N - 1 ) , ( K + 1 ) , Tag ] )
                                                                                                          in let Len = ldim( Xs )
                                                                                                             in let Ts0 = Tvs [ Len , Cnt , 0 ] ,
                                                                                                                    Ts1 = Tvs [ Len , Cnt , 1 ]
                                                                                                                in let Xt1 = Merge [ < Xs , Ts1 > ]
                                                                                                                   in ap case 
                                                                                                                         when < Te1 , En1 , Cn1 > guard true do ap case 
                                                                                                                                                                   when < Te2 , En2 , Cn2 > guard true do < Lrec{Xs , Ts0 , Te1 , Te2} , lcut( ldim( Xt1 ) , En2 ) , Cn2 >
                                                                                                                                                                   end
                                                                                                                                                                to [ Tv [ < E , En1 , Cn1 > ] ]
                                                                                                                         end
                                                                                                                      to [ Tv [ < Fs , ( Xt1 ++ Env ) , ( Cnt + Len ) > ] ] ,
                             when < LrecM{Xs , Fs , E} , Env , Cnt > guard ( ldim( Xs ) eq ldim( Fs ) ) do def 
                                                                                                             Tvs [ N , K , Tag ] = if ( N eq 0 )
                                                                                                                                   then <>
                                                                                                                                   else ( < T{K , Tag} > ++ Tvs [ ( N - 1 ) , ( K + 1 ) , Tag ] )
                                                                                                           in let Len = ldim( Xs )
                                                                                                              in let Ts0 = Tvs [ Len , Cnt , 0 ] ,
                                                                                                                     Ts1 = Tvs [ Len , Cnt , 1 ]
                                                                                                                 in let Xt0 = Merge [ < Xs , Ts0 > ] ,
                                                                                                                        Xt1 = Merge [ < Xs , Ts1 > ]
                                                                                                                    in ap case 
                                                                                                                          when < Te1 , En1 , Cn1 > guard true do ap case 
                                                                                                                                                                    when < Te2 , En2 , Cn2 > guard true do < LrecM{Xs , Ts0 , Te1 , Te2} , lcut( ldim( Xt1 ) , En2 ) , Cn2 >
                                                                                                                                                                    end
                                                                                                                                                                 to [ Tv [ < E , ( Xt1 ++ lcut( ldim( Xt0 ) , En1 ) ) , Cn1 > ] ]
                                                                                                                          end
                                                                                                                       to [ Tv [ < Fs , ( Xt0 ++ Env ) , ( Cnt + Len ) > ] ] ,
                             when < Lst{Hd , Tl} , Env , Cnt > guard true do ap case 
                                                                                when < Te1 , En1 , Cn1 > guard true do ap case 
                                                                                                                          when < Te2 , En2 , Cn2 > guard true do < Lst{Te1 , Te2} , En2 , Cn2 >
                                                                                                                          end
                                                                                                                       to [ Tv [ < Tl , En1 , Cn1 > ] ]
                                                                                end
                                                                             to [ Tv [ < Hd , Env , Cnt > ] ] ,
                             when < Tup{as ... Xs} , Env , Cnt > guard true do ap case 
                                                                                  when < Te1 , En1 , Cn1 > guard true do < ( Tup{ } ++ Te1 ) , En1 , Cn1 >
                                                                                  end
                                                                               to [ Tv [ < Xs , Env , Cnt > ] ] ,
                             when < < as '_"... ` Con , as ... Xs > , Env , Cnt > guard true do ap case 
                                                                                                   when < Te1 , En1 , Cn1 > guard true do < ( < Con > ++ Te1 ) , En1 , Cn1 >
                                                                                                   end
                                                                                                to [ Tv [ < Xs , Env , Cnt > ] ] ,
                             when < < X > , Env , Cnt > guard true do ap case 
                                                                         when < Te1 , En1 , Cn1 > guard true do < < Te1 > , En1 , Cn1 >
                                                                         end
                                                                      to [ Tv [ < X , Env , Cnt > ] ] ,
                             when < < X , as ... Xs > , Env , Cnt > guard true do ap case 
                                                                                     when < Te1 , En1 , Cn1 > guard true do ap case 
                                                                                                                               when < Te2 , En2 , Cn2 > guard true do < ( < Te1 > ++ Te2 ) , En2 , Cn2 >
                                                                                                                               end
                                                                                                                            to [ Tv [ < Xs , En1 , Cn1 > ] ]
                                                                                     end
                                                                                  to [ Tv [ < X , Env , Cnt > ] ] ,
                             when < <> , Env , Cnt > guard true do < <> , Env , Cnt > ,
                             when X guard true do Error{'(Tv) syntax error` , X}
                             end
                in Tv [ < E , <> , 1 > ] ,
  Print [   ] = 
                case 
                when as number X guard true do Itoa [ X ] ,
                when as string X guard true do X ,
                when T{X , ...} guard true do ( 't` ++ Print [ X ] ) ,
                when Arr{as Arr{...} T1 , T2} guard true do ( '(` ++ ( Print [ T1 ] ++ ( ')->` ++ Print [ T2 ] ) ) ) ,
                when Arr{T1 , T2} guard true do ( Print [ T1 ] ++ ( '->` ++ Print [ T2 ] ) ) ,
                when Lst{T} guard true do ( '[` ++ ( Print [ T ] ++ ']` ) ) ,
                when C{X , T} guard true do ( Print [ X ] ++ ( ':` ++ Print [ T ] ) ) ,
                when F{X , T} guard true do ( X ++ ( ':` ++ Print [ T ] ) ) ,
                when V{X , T} guard true do ( X ++ ( ':` ++ Print [ T ] ) ) ,
                when L{X , T , E} guard true do ( 'lambda ` ++ ( X ++ ( ':` ++ ( Print [ T ] ++ ( ' . ` ++ Print [ E ] ) ) ) ) ) ,
                when A{F , A , T} guard true do ( '(ap ` ++ ( Print [ F ] ++ ( ' to ` ++ ( Print [ A ] ++ ( '):` ++ Print [ T ] ) ) ) ) ) ,
                when I{P , C , A} guard true do ( 'if ` ++ ( Print [ P ] ++ ( ' then ` ++ ( Print [ C ] ++ ( ' else ` ++ Print [ A ] ) ) ) ) ) ,
                when Let{X , T , F , E} guard true do ( 'let ` ++ ( Print [ X ] ++ ( ':` ++ ( Print [ T ] ++ ( ' = ` ++ ( Print [ F ] ++ ( ' in ` ++ Print [ E ] ) ) ) ) ) ) ) ,
                when Rec{X , T , F , E} guard true do ( 'rec ` ++ ( Print [ X ] ++ ( ':` ++ ( Print [ T ] ++ ( ' = ` ++ ( Print [ F ] ++ ( ' in ` ++ Print [ E ] ) ) ) ) ) ) ) ,
                when Lrec{Xs , Ts , Fs , E} guard true do ( 'lrec ` ++ ( Print [ Defs{Xs , Ts , Fs} ] ++ ( ' in ` ++ Print [ E ] ) ) ) ,
                when LrecM{Xs , Ts , Fs , E} guard true do ( 'lrecm ` ++ ( Print [ Defs{Xs , Ts , Fs} ] ++ ( ' in ` ++ Print [ E ] ) ) ) ,
                when Defs{< X > , < T > , < F >} guard true do ( Print [ X ] ++ ( ':` ++ ( Print [ T ] ++ ( ' = ` ++ Print [ F ] ) ) ) ) ,
                when Defs{< X , as ... Xs > , < T , as ... Ts > , < F , as ... Fs >} guard true do ( Print [ X ] ++ ( ':` ++ ( Print [ T ] ++ ( ' = ` ++ ( Print [ F ] ++ ( ' , ` ++ Print [ Defs{Xs , Ts , Fs} ] ) ) ) ) ) ) ,
                when Lst{as ... Xs} guard true do ( '[` ++ ( Print [ Xs ] ++ ']` ) ) ,
                when Tup{as ... Xs} guard true do ( '<` ++ ( Print [ Xs ] ++ '>` ) ) ,
                when W{S , T , K , ...} guard true do ( 'W{` ++ ( Print [ S ] ++ ( ' , ` ++ ( Print [ T ] ++ ( ' , ` ++ ( Print [ K ] ++ '}` ) ) ) ) ) ) ,
                when Error{as ... Msgs} guard true do ( 'error: ` ++ Print [ Msgs ] ) ,
                when < '_"as "...  "Con"` , as ... Xs > guard true do ( ( Con ++ '{` ) ++ ( Print [ Xs ] ++ '}` ) ) ,
                when < X > guard true do Print [ X ] ,
                when < X , as ... Xs > guard true do ( Print [ X ] ++ ( ' ` ++ Print [ Xs ] ) ) ,
                when <> guard true do '`
                otherwise '??`
                end ,
  Constant [   ] =
                   case 
                   when C{...} guard true do true ,
                   when L guard true do ( class( L ) ne list )
                   end ,
  Variable [   ] =
                   case 
                   when T{...} guard true do true
                   otherwise false
                   end ,
  EqVar [ X , Y ] =
                    ap case 
                       when < T{N , ...} , T{M , ...} > guard true do ( N eq M )
                       otherwise false
                       end
                    to [ < X , Y > ] ,
  Equal [ X , Y ] = 
                    ap case 
                       when < < X , as ... Xs > , < Y , as ... Ys > > guard true do if ( ldim( Xs ) eq ldim( Ys ) )
                                                                                    then if ( X Equal Y )
                                                                                         then ( Xs Equal Ys )
                                                                                         else false
                                                                                    else false ,
                       when < X , Y > guard true do ( X eq Y )
                       end
                    to [ < X , Y > ] ,
  VarIsIn [ X , L ] =
                      if empty( L )
                      then false
                      else if ( X EqVar lselect( 1 , L ) )
                           then true
                           else VarIsIn [ X , lcut( 1 , L ) ] ,
  IsIn [ X , L ] =
                   if empty( L )
                   then false
                   else if ( X Equal lselect( 1 , L ) )
                        then true
                        else IsIn [ X , lcut( 1 , L ) ] ,
  Find [ X , L ] =
                   if empty( L )
                   then false
                   else if ( X Equal lselect( 1 , L ) )
                        then lselect( 2 , L )
                        else Find [ X , lcut( 2 , L ) ] ,
  Vars [   ] =
               case 
               when T guard Variable [ T ] do < T > ,
               when < '_"... ` , as ... Xs > guard true do Vars [ Xs ] ,
               when < X , as ... Xs > guard true do ( Vars [ X ] ++ Vars [ Xs ] )
               otherwise <>
               end ,
  Zip [   ] = 
              case 
              when < < X , as ... Xs > , < Y , as ... Ys > > guard true do ( < < X , Y > > ++ Zip [ < Xs , Ys > ] ) ,
              when < X , Y > guard true do ( X ++ Y )
              otherwise error: 'Zip: invalid arguments`
              end ,
  Merge [   ] =
                case 
                when < < X , as ... Xs > , < Y , as ... Ys > > guard true do ( < X , Y > ++ Merge [ < Xs , Ys > ] ) ,
                when < X , Y > guard true do ( X ++ Y )
                otherwise error: 'Merge: invalid arguments`
                end ,
  Itoa [   ] =
               def 
                 I2a [ N ] = if ( N lt 10 )
                             then lselect( ( N + 1 ) , < '0` , '1` , '2` , '3` , '4` , '5` , '6` , '7` , '8` , '9` > )
                             else ( Itoa [ trunc [ ( N / 10 ) ] ] ++ Itoa [ ( N mod 10 ) ] )
               in case 
                  when as number N guard ( N eq trunc( N ) ) do if ( N lt 0 )
                                                                then ( '-` ++ I2a [ neg( N ) ] )
                                                                else I2a [ N ]
                  otherwise error: 'Itoa: integer argument required`
                  end ,
  Type [ E ] = 
               ap case 
                  when < Te , En , Cn > guard true do ap case 
                                                         when W{S , T , K} guard true do Print [ T ] ,
                                                         when X guard true do Print [ X ]
                                                         end
                                                      to [ W [ < <> , Te , Cn , En > ] ] ,
                  when X guard true do X
                  end
               to [ Tvars [ E ] ] ,
  Typp [ E ] = ap case 
                  when < Te , En , Cn > guard true do ap case 
                                                         when W{S , T , K} guard true do Print [ Subst [ < S , Te > ] ] ,
                                                         when X guard true do Print [ X ]
                                                         end
                                                      to [ W [ < <> , Te , Cn , En > ] ] ,
                  when X guard true do X
                  end
               to [ Tvars [ E ] ] ,
  Typs [ E ] = ap case 
                  when < Te , En , Cn > guard true do ap case 
                                                         when W{S , T , K} guard true do S ,
                                                         when X guard true do Print [ X ]
                                                         end
                                                      to [ W [ < <> , Te , Cn , En > ] ] ,
                  when X guard true do X
                  end
               to [ Tvars [ E ] ]
in Type [ Lrec{< 'map` > , < L{'f` , L{'l` , I{A{'empty` , 'l`} , <> , Lst{A{'f` , A{'hd` , 'l`}} , A{A{'map` , 'f`} , A{'tl` , 'l`}}}}}} > , Let{'g` , L{'x` , 'x`} , Tup{'map` , A{A{'map` , 'g`} , Lst{1 , <>}} , A{A{'map` , 'g`} , Lst{true , <>}}}}} ]

