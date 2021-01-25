def 
  Scan [ Term ] = def 
                    Scan [ Term ] = if ( Term eq '` )
                                    then <>
                                    else ap case 
                                            when '"in ". ' `""as "...  "Rest"` guard true do ( <> ++ Scan [ Rest ] ),
                                            when '"as "in ". ',.[](){}<>=`" "Key""as "...  "Rest"` guard true do ( < Sw{Key} > ++ Scan [ Rest ] ),
                                            when '++"as "...  "Rest"` guard true do ( < Prf{'++`} > ++ Scan [ Rest ] ),
                                            when '"as "in ". '+-*/`" "Key""as "...  "Rest"` guard true do ( < Prf{Key} > ++ Scan [ Rest ] ),
                                            when '"as "in "... '0123456789`" "Zahl""as "in ". ',.={}[]<>()+-*/| `" "Trenner""as "...  "Rest"` guard true do ( < Num{StrToNum [ Zahl, 1 ]} > ++ Scan [ ( Trenner ++ Rest ) ] ),
                                            when '"in ". '~`""as "in "... '0123456789`" "Zahl""as "in ". ',.={}[]<>()+-*/| `" "Trenner""as "...  "Rest"` guard ( Zahl ne '` ) do ( < Num{( StrToNum [ Zahl, 1 ] * ~1 )} > ++ Scan [ ( Trenner ++ Rest ) ] ),
                                            when 'false"as "in ". ',.={}[]<>()+-*/| `" "Trenn""as "...  "Rest"` guard true do ( < Bl{false} > ++ Scan [ ( Trenn ++ Rest ) ] ),
                                            when 'true"as "in ". ',.={}[]<>()+-*/| `" "Trenn""as "...  "Rest"` guard true do ( < Bl{true} > ++ Scan [ ( Trenn ++ Rest ) ] ),
                                            when '"as "in "... '+abcdefghijklmnopqrstuvwxyz`" "Word""as "in ". ',.={}[]<>()+-*/| `" "Trenn""as "...  "Rest"` guard true do ( < Get_key [ Word ] > ++ Scan [ ( Trenn ++ Rest ) ] ),
                                            when '"as "in ". 'abcdefghijklmnopjrstuvwxyz`" "Fstr""as "in "... 'abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ`" "Estr""as "in ". ',.=[]<>()+-*/| `" "Trenn""as "...  "Rest"` guard true do ( < Id{( Fstr ++ Estr )} > ++ Scan [ ( Trenn ++ Rest ) ] ),
                                            when '"as "...  "Word""as "in ". ',.={}[]<>()+-*/| `" "Trenn""as "...  "Rest"` guard true do ( < Id{Word} > ++ Scan [ ( Trenn ++ Rest ) ] )
                                            otherwise < Junk{Term} >
                                            end
                                         to [ Term ],
                    Get_key [ Str ] = let Keylist = < 'if`, 'then`, 'else`, 'sub`, 'let`, 'ap`, 'def`, 'in`, 'to` >,
                                          Prflist = < 'zero`, 'succ`, 'pred`, 'hd`, 'tl`, 'empty`, 'ldim`, 'lcut`, 'lselect`, 'lunite`, 'and`, 'or`, 'not`, 'eq`, 'lreplace`, 'empty`, 'ne`, 'le`, 'lt`, 'gt`, 'ge` >
                                      in if Find_inlist [ Str, Keylist ]
                                         then Sw{Str}
                                         else if Find_inlist [ Str, Prflist ]
                                              then Prf{Str}
                                              else Id{Str},
                    Find_inlist [ Str, Llist ] = if ( Llist eq <> )
                                                  then false
                                                  else if ( Str eq ( 1 | Llist ) )
                                                       then true
                                                       else Find_inlist [ Str, lcut [ 1, Llist ] ],
                    StrToNum [ Str, Pot ] = ap case 
                                                when '` guard true do 0,
                                                when '"as "...  "First"0` guard true do ( 0 + StrToNum [ First, ( Pot * 10 ) ] ),
                                                when '"as "...  "First"1` guard true do ( ( 1 * Pot ) + StrToNum [ First, ( Pot * 10 ) ] ),
                                                when '"as "...  "First"2` guard true do ( ( 2 * Pot ) + StrToNum [ First, ( Pot * 10 ) ] ),
                                                when '"as "...  "First"3` guard true do ( ( 3 * Pot ) + StrToNum [ First, ( Pot * 10 ) ] ),
                                                when '"as "...  "First"4` guard true do ( ( 4 * Pot ) + StrToNum [ First, ( Pot * 10 ) ] ),
                                                when '"as "...  "First"5` guard true do ( ( 5 * Pot ) + StrToNum [ First, ( Pot * 10 ) ] ),
                                                when '"as "...  "First"6` guard true do ( ( 6 * Pot ) + StrToNum [ First, ( Pot * 10 ) ] ),
                                                when '"as "...  "First"7` guard true do ( ( 7 * Pot ) + StrToNum [ First, ( Pot * 10 ) ] ),
                                                when '"as "...  "First"8` guard true do ( ( 8 * Pot ) + StrToNum [ First, ( Pot * 10 ) ] ),
                                                when '"as "...  "First"9` guard true do ( ( 9 * Pot ) + StrToNum [ First, ( Pot * 10 ) ] )
                                                end
                                             to [ Str ]
                  in Scan [ ( Term ++ ' ` ) ],
  Parse [ Term ] = def 
                     Parser [ Term ] = letp < Lterm, Rterm > = Parse [ Term ]
                                       in ap case 
                                             when Error{Message} guard true do Error{< Message >},
                                             when <> guard true do Lterm
                                             otherwise Error{< 'More than one Term !!!!!` >}
                                             end
                                          to [ Rterm ],
                     Parse [ Term ] = ap case 
                                         when < Num{Val}, as ... Rest > guard true do < Num{Val}, Rest >,
                                         when < Id{Val}, Sw{'[`}, as ... Rest > guard true do Parse [ ( < Sw{'ap`}, Id{Val}, Sw{'to`}, Sw{'[`} > ++ Rest ) ],
                                         when < Id{Val}, Sw{'{`}, Sw{'}`}, as ... Rest > guard true do < Bdf{Val, <>}, Rest >,
                                         when < Id{Val}, Sw{'{`}, as ... Rest > guard true do letp < Lterm, Rterm > = TermList [ ( < Sw{',`} > ++ Rest ), <>, '}` ]
                                                                                                in ap case 
                                                                                                      when Error{Message} guard true do < Lterm, Error{Message} >
                                                                                                      otherwise < Bdf{Val, Lterm}, Rterm >
                                                                                                      end
                                                                                                   to [ Rterm ],
                                         when < Prf{Fkt}, Sw{'[`}, as ... Rest > guard true do Parse [ ( < Sw{'ap`}, Prf{Fkt}, Sw{'to`}, Sw{'[`} > ++ Rest ) ],
                                         when < Id{Val}, as ... Rest > guard true do < Id{Val}, Rest >,
                                         when < Bl{Val}, as ... Rest > guard true do < Bl{Val}, Rest >,
                                         when < Prf{Fkt}, as ... Rest > guard true do < Prf{Fkt}, Rest >,
                                         when < Sw{'(`}, as ... Rest > guard true do letp < Lterm, Rest1 > = Parse [ Rest ]
                                                                                      in ap case 
                                                                                            when Error{Message} guard true do < Lterm, Error{Message} >
                                                                                            otherwise letp < Fterm, Rest2 > = Parse [ Rest1 ]
                                                                                                      in ap case 
                                                                                                            when Error{Message} guard true do < Lterm, Error{Message} >
                                                                                                            otherwise letp < Rterm, Rest3 > = Parse [ Rest2 ]
                                                                                                                      in ap case 
                                                                                                                            when Error{Message} guard true do < Lterm, Error{Message} >,
                                                                                                                            when < Sw{')`}, as ... Bigrest > guard true do < Ap{Fterm, < Lterm, Rterm >}, Bigrest >
                                                                                                                            otherwise < Rterm, Error{'Missing ) to arithmetic Term !!!`} >
                                                                                                                            end
                                                                                                                         to [ Rest3 ]
                                                                                                            end
                                                                                                         to [ Rest2 ]
                                                                                            end
                                                                                         to [ Rest1 ],
                                         when < Sw{'<`}, Sw{'>`}, as ... Rest > guard true do < Tup{<>}, Rest >,
                                         when < Sw{'<`}, as ... Rest > guard true do letp < Lterm, Rterm > = TermList [ ( < Sw{',`} > ++ Rest ), <>, '>` ]
                                                                                      in ap case 
                                                                                            when Error{Message} guard true do < Lterm, Error{Message} >
                                                                                            otherwise < Tup{Lterm}, Rterm >
                                                                                            end
                                                                                         to [ Rterm ],
                                         when < Sw{'[`}, Sw{']`}, as ... Rest > guard true do < Nil{ }, Rest >,
                                         when < Sw{'[`}, as ... Rest > guard true do letp < Lterm, Rterm > = Parse [ Rest ]
                                                                                      in ap case 
                                                                                            when Error{Message} guard true do < <>, Error{Message} >,
                                                                                            when < Sw{'.`}, as ... Frest > guard true do letp < Rparse, Fterm > = Parse [ Frest ]
                                                                                                                                          in ap case 
                                                                                                                                                when Error{Message} guard true do < <>, Error{Message} >,
                                                                                                                                                when < Sw{']`}, as ... Final > guard true do < Cons{Lterm, Rparse}, Final >
                                                                                                                                                otherwise < <>, Error{'Missing ] to binary List !!!!`} >
                                                                                                                                                end
                                                                                                                                             to [ Fterm ]
                                                                                            otherwise < <>, Error{'Bad binary list ! Missig . !!!`} >
                                                                                            end
                                                                                         to [ Rterm ],
                                         when < Sw{'ap`}, as ... Rest > guard true do letp < Konstr, Restterm > = Parse [ Rest ]
                                                                                       in ap case 
                                                                                             when Error{Message} guard true do < Konstr, Error{Message} >,
                                                                                             when < Sw{'to`}, Sw{'[`}, as ... Rest > guard true do letp < Lterm, Rterm > = TermList [ ( < Sw{',`} > ++ Rest ), <>, ']` ]
                                                                                                                                                     in ap case 
                                                                                                                                                           when Error{Message} guard true do < Lterm, Error{Message} >
                                                                                                                                                           otherwise < Ap{Konstr, Lterm}, Rterm >
                                                                                                                                                           end
                                                                                                                                                        to [ Rterm ]
                                                                                             end
                                                                                          to [ Restterm ],
                                         when < Sw{'sub`}, as ... Rest > guard true do ap case 
                                                                                           when < Sw{'[`}, as ... End > guard true do letp < Lterm, Rterm > = IdenterList [ End, <> ]
                                                                                                                                       in ap case 
                                                                                                                                             when Error{Message} guard true do < Lterm, Error{Message} >,
                                                                                                                                             when < Sw{'in`}, as ... End > guard true do letp < Lparse, Rparse > = Parse [ End ]
                                                                                                                                                                                          in ap case 
                                                                                                                                                                                                when Error{Message} guard true do < Lparse, Error{Message} >
                                                                                                                                                                                                otherwise < Lam{Lterm, Lparse}, Rparse >
                                                                                                                                                                                                end
                                                                                                                                                                                             to [ Rparse ]
                                                                                                                                             end
                                                                                                                                          to [ Rterm ]
                                                                                           otherwise < Term, Error{' Bad Definition !!!`} >
                                                                                           end
                                                                                        to [ Rest ],
                                         when < Sw{'then`}, as ... Rest > guard true do letp < Lterm, Rterm > = Parse [ Rest ]
                                                                                         in ap case 
                                                                                               when Error{Message} guard true do < Lterm, Error{Message} >,
                                                                                               when < Sw{'else`}, as ... End > guard true do letp < Lparse, Rparse > = Parse [ End ]
                                                                                                                                              in ap case 
                                                                                                                                                    when Error{Message} guard true do < Lterm, Error{Message} >
                                                                                                                                                    otherwise < Cnd{Lterm, Lparse}, Rparse >
                                                                                                                                                    end
                                                                                                                                                 to [ Rparse ]
                                                                                               otherwise < Lterm, Error{'Missing else to then-term !!!`} >
                                                                                               end
                                                                                            to [ Rterm ],
                                         when < Sw{'if`}, as ... Rest > guard true do letp < Lterm, Rterm > = Parse [ Rest ]
                                                                                       in ap case 
                                                                                             when Error{Message} guard true do < Lterm, Error{Message} >,
                                                                                             when < Sw{'then`}, as ... End > guard true do letp < Lparse, Rparse > = Parse [ ( < Sw{'then`} > ++ End ) ]
                                                                                                                                            in ap case 
                                                                                                                                                  when Error{Message} guard true do < Lterm, Error{Message} >
                                                                                                                                                  otherwise < Ap{Lparse, < Lterm >}, Rparse >
                                                                                                                                                  end
                                                                                                                                               to [ Rparse ]
                                                                                             otherwise < Lterm, Error{'Missing then to if-term !!!`} >
                                                                                             end
                                                                                          to [ Rterm ],
                                         when < Sw{'let`}, as ... Rest > guard true do letp < Lterm, Rterm > = Idtermlist [ ( < Sw{',`} > ++ Rest ), <> ]
                                                                                        in ap case 
                                                                                              when Error{Message} guard true do < Lterm, Error{Message} >,
                                                                                              when < Sw{'in`}, as ... End > guard true do letp < Lparse, Rparse > = Parse [ End ]
                                                                                                                                           in ap case 
                                                                                                                                                 when Error{Message} guard true do < Lterm, Error{Message} >
                                                                                                                                                 otherwise < Let{Lterm, Lparse}, Rparse >
                                                                                                                                                 end
                                                                                                                                              to [ Rparse ]
                                                                                              otherwise < Lterm, Error{'Missing in to let-term !!!`} >
                                                                                              end
                                                                                           to [ Rterm ],
                                         when < Sw{'def`}, as ... Rest > guard true do letp < Lterm, Rterm > = Deflist [ ( < Sw{',`} > ++ Rest ), <> ]
                                                                                        in ap case 
                                                                                              when Error{Message} guard true do < Lterm, Error{Message} >,
                                                                                              when < Sw{'in`}, as ... End > guard true do letp < Lparse, Rparse > = Parse [ End ]
                                                                                                                                           in ap case 
                                                                                                                                                 when Error{Message} guard true do < Lterm, Error{Message} >
                                                                                                                                                 otherwise < Def{Lterm, Lparse}, Rparse >
                                                                                                                                                 end
                                                                                                                                              to [ Rparse ]
                                                                                              otherwise < Lterm, Error{'Missing in to def-term !!!`} >
                                                                                              end
                                                                                           to [ Rterm ],
                                         when < Sw{Irgend}, as ... Rest > guard true do < Term, Error{( 'Unknown Key Word or Key Word in a bad place  !!!! --> ` ++ Irgend )} >,
                                         when < Junk{Something}, as ... Rest > guard true do < Term, Error{( 'Unidentified Term !!! : ` ++ Something )} >
                                         otherwise < <>, Error{'No Term or Missing Term !!!>`} >
                                         end
                                      to [ Term ],
                     TermList [ Alist, Ko_list, Close_sign ] = ap case 
                                                                    when < Sw{Sign}, as ... Rest > guard ( Sign eq Close_sign ) do < Ko_list, Rest >,
                                                                    when < Sw{',`}, as ... Rest > guard true do letp < Lterm, Rterm > = Parse [ Rest ]
                                                                                                                 in ap case 
                                                                                                                       when Error{Message} guard true do < Lterm, Error{Message} >
                                                                                                                       otherwise TermList [ Rterm, ( Ko_list ++ < Lterm > ), Close_sign ]
                                                                                                                       end
                                                                                                                    to [ Rterm ],
                                                                    when <> guard true do < Ko_list, Error{'No Closing ] to ap !!!`} >
                                                                    otherwise < Alist, Error{( 'A Missing Komma in ap-Term  !!! --> ` ++ Getkeystring [ Alist, 7 ] )} >
                                                                    end
                                                                 to [ Alist ],
                     IdenterList [ Alist, Ko_list ] = ap case 
                                                          when < Sw{']`}, as ... Rest > guard true do < Ko_list, Rest >,
                                                          when <> guard true do < Ko_list, Error{'No Closing ] to sub !!!`} >,
                                                          when < Id{Val}, Sw{',`}, as ... Rest > guard true do let Fterm = Id{Val}
                                                                                                                 in IdenterList [ Rest, ( Ko_list ++ < Fterm > ) ],
                                                          when < Id{Val}, Sw{']`}, as ... Rest > guard true do < ( Ko_list ++ < Id{Val} > ), Rest >
                                                          otherwise < Alist, Error{( 'Bad Identifiers or missing Komma !!! --> ` ++ Getkeystring [ Alist, 7 ] )} >
                                                          end
                                                       to [ Alist ],
                     Getkeystring [ Alist, N ] = if ( ( N eq 0 ) or ( Alist eq <> ) )
                                                  then ' `
                                                  else ap case 
                                                          when < Sw{Key}, as ... Rest > guard true do ( ( Key ++ ' ` ) ++ Getkeystring [ Rest, ( N - 1 ) ] ),
                                                          when < Junk{Key}, as ... Rest > guard true do ( ( Key ++ ' ` ) ++ Getkeystring [ Rest, ( N - 1 ) ] ),
                                                          when < Id{Key}, as ... Rest > guard true do ( ( Key ++ ' ` ) ++ Getkeystring [ Rest, ( N - 1 ) ] ),
                                                          when < Prf{Key}, as ... Rest > guard true do ( ( Key ++ ' ` ) ++ Getkeystring [ Rest, ( N - 1 ) ] ),
                                                          when < Bl{true}, as ... Rest > guard true do ( 'true ` ++ Getkeystring [ Rest, ( N - 1 ) ] ),
                                                          when < Bl{false}, as ... Rest > guard true do ( 'false ` ++ Getkeystring [ Rest, ( N - 1 ) ] ),
                                                          when < Num{Val}, as ... Rest > guard true do ( '!` ++ Getkeystring [ Rest, ( N - 1 ) ] )
                                                          otherwise ( ' ` ++ Getkeystring [ Rest, ( N - 1 ) ] )
                                                          end
                                                       to [ Alist ],
                     Idtermlist [ Alist, Ko_list ] = ap case 
                                                         when <> guard true do < Alist, Error{'List of Id-Terms does not seem to end !!!!`} >,
                                                         when < Sw{',`}, Id{Val}, Sw{'=`}, as ... Rest > guard true do letp < Lterm, Rterm > = Parse [ Rest ]
                                                                                                                          in ap case 
                                                                                                                                when Error{Message} guard true do < Lterm, Error{Message} >
                                                                                                                                otherwise Idtermlist [ Rterm, ( Ko_list ++ < < Id{Val}, Lterm > > ) ]
                                                                                                                                end
                                                                                                                             to [ Rterm ],
                                                         when < Sw{'in`}, as ... Rest > guard true do < Ko_list, ( < Sw{'in`} > ++ Rest ) >
                                                         otherwise < Alist, Error{( 'Bad Identifier-construct in let-term or A Missing in or Missing or bad Komma !!! -->` ++ Getkeystring [ Alist, 4 ] )} >
                                                         end
                                                      to [ Alist ],
                     Deflist [ Alist, Ko_list ] = ap case 
                                                      when <> guard true do < Alist, Error{'List of Def-Terms does not seem to end !!!!`} >,
                                                      when < Sw{',`}, Id{Val}, Sw{'[`}, Sw{']`}, Sw{'=`}, as ... Rest > guard true do letp < Lterm, Rterm > = Parse [ Rest ]
                                                                                                                                           in ap case 
                                                                                                                                                 when Error{Message} guard true do < Lterm, Error{Message} >
                                                                                                                                                 otherwise Deflist [ Rterm, ( Ko_list ++ < < Id{Val}, Lterm > > ) ]
                                                                                                                                                 end
                                                                                                                                              to [ Rterm ],
                                                      when < Sw{',`}, Id{Val}, Sw{'[`}, as ... Rest > guard true do letp < IdentList, Rterm > = IdenterList [ Rest, <> ]
                                                                                                                       in ap case 
                                                                                                                             when Error{Message} guard true do < IdentList, Error{Message} >
                                                                                                                             otherwise ap case 
                                                                                                                                          when < Sw{'=`}, as ... Rest > guard true do letp < TheTerm, End > = Parse [ Rest ]
                                                                                                                                                                                       in ap case 
                                                                                                                                                                                             when Error{Message} guard true do < TheTerm, Error{Message} >
                                                                                                                                                                                             otherwise Deflist [ End, ( Ko_list ++ < < Id{Val}, Lam{IdentList, TheTerm} > > ) ]
                                                                                                                                                                                             end
                                                                                                                                                                                          to [ End ]
                                                                                                                                          otherwise < IdentList, Error{'Missing = in definition-list !!!`} >
                                                                                                                                          end
                                                                                                                                       to [ Rterm ]
                                                                                                                             end
                                                                                                                          to [ Rterm ],
                                                      when < Sw{'in`}, as ... Rest > guard true do < Ko_list, ( < Sw{'in`} > ++ Rest ) >
                                                      otherwise < Alist, Error{( 'Bad Identifier-construct in def-term or A Missing in or Missing or bad Komma !!! -->` ++ Getkeystring [ Alist, 7 ] )} >
                                                      end
                                                   to [ Alist ]
                   in Parser [ Term ]
in Parse [ Scan [ '(5*(1+2) )` ] ]

