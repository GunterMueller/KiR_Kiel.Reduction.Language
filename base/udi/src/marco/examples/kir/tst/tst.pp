def 
  SELECT [ I , Liste ] = lselect( I , Liste ) ,
  UNITE [ Liste1 , Liste ] = lunite( Liste1 , Liste ) ,
  CUT [ I , Liste ] = lcut( I , Liste ) ,
  DIM [ Liste ] = ldim( Liste ) ,
  REPLACE [ Index , Substitut , Liste ] = lreplace( Index , Substitut , Liste )
in def 
     Scan [ Str ] = ap case 
                       when ' "as "...  "Rest"` guard true do Scan [ Rest ] ,
                       when '~"as "...  "Rest"` guard true do Convert_bin [ Str ] ,
                       when '0"as "...  "Rest"` guard true do Convert_bin [ Str ] ,
                       when '1"as "...  "Rest"` guard true do Convert_bin [ Str ] ,
                       when '2"as "...  "Rest"` guard true do Convert_bin [ Str ] ,
                       when '3"as "...  "Rest"` guard true do Convert_bin [ Str ] ,
                       when '4"as "...  "Rest"` guard true do Convert_bin [ Str ] ,
                       when '5"as "...  "Rest"` guard true do Convert_bin [ Str ] ,
                       when '6"as "...  "Rest"` guard true do Convert_bin [ Str ] ,
                       when '7"as "...  "Rest"` guard true do Convert_bin [ Str ] ,
                       when '8"as "...  "Rest"` guard true do Convert_bin [ Str ] ,
                       when '9"as "...  "Rest"` guard true do Convert_bin [ Str ] ,
                       when 'and"as "...  "Rest"` guard true do UNITE [ < Dyop_sy{'and`} > , Scan [ Rest ] ] ,
                       when 'or"as "...  "Rest"` guard true do UNITE [ < Dyop_sy{'or`} > , Scan [ Rest ] ] ,
                       when '+"as "...  "Rest"` guard true do UNITE [ < Dyop_sy{'+`} > , Scan [ Rest ] ] ,
                       when '-"as "...  "Rest"` guard true do UNITE [ < Dyop_sy{'-`} > , Scan [ Rest ] ] ,
                       when '*"as "...  "Rest"` guard true do UNITE [ < Dyop_sy{'*`} > , Scan [ Rest ] ] ,
                       when '/"as "...  "Rest"` guard true do UNITE [ < Dyop_sy{'/`} > , Scan [ Rest ] ] ,
                       when 'eq"as "...  "Rest"` guard true do UNITE [ < Dyop_sy{'eq`} > , Scan [ Rest ] ] ,
                       when 'neg"as "...  "Rest"` guard true do UNITE [ < Monop_sy{'neg`} > , Scan [ Rest ] ] ,
                       when 'ne"as "...  "Rest"` guard true do UNITE [ < Dyop_sy{'ne`} > , Scan [ Rest ] ] ,
                       when 'gt"as "...  "Rest"` guard true do UNITE [ < Dyop_sy{'gt`} > , Scan [ Rest ] ] ,
                       when 'ge"as "...  "Rest"` guard true do UNITE [ < Dyop_sy{'ge`} > , Scan [ Rest ] ] ,
                       when 'lt"as "...  "Rest"` guard true do UNITE [ < Dyop_sy{'lt`} > , Scan [ Rest ] ] ,
                       when 'cons"as "...  "Rest"` guard true do UNITE [ < Dyop_sy{'cons`} > , Scan [ Rest ] ] ,
                       when 'empty"as "...  "Rest"` guard true do UNITE [ < Monop_sy{'empty`} > , Scan [ Rest ] ] ,
                       when 'hd"as "...  "Rest"` guard true do UNITE [ < Monop_sy{'hd`} > , Scan [ Rest ] ] ,
                       when 'tl"as "...  "Rest"` guard true do UNITE [ < Monop_sy{'tl`} > , Scan [ Rest ] ] ,
                       when 'not"as "...  "Rest"` guard true do UNITE [ < Monop_sy{'not`} > , Scan [ Rest ] ] ,
                       when 'TEST_CONS"as "...  "Rest"` guard true do UNITE [ < Mon_def_sy{'test_cons`} > , Scan [ Rest ] ] ,
                       when 'TEST_CCONS"as "...  "Rest"` guard true do UNITE [ < Mon_def_sy{'test_ccons`} > , Scan [ Rest ] ] ,
                       when 'TEST_KON"as "...  "Rest"` guard true do UNITE [ < Mon_def_sy{'test_kon`} > , Scan [ Rest ] ] ,
                       when 'TEST_INT"as "...  "Rest"` guard true do UNITE [ < Mon_def_sy{'test_int`} > , Scan [ Rest ] ] ,
                       when 'TEST_BOOL"as "...  "Rest"` guard true do UNITE [ < Mon_def_sy{'test_bool`} > , Scan [ Rest ] ] ,
                       when 'TEST_VAR"as "...  "Rest"` guard true do UNITE [ < Mon_def_sy{'test_var`} > , Scan [ Rest ] ] ,
                       when 'TEST_NULL"as "...  "Rest"` guard true do UNITE [ < Mon_def_sy{'test_null`} > , Scan [ Rest ] ] ,
                       when 'CHD"as "...  "Rest"` guard true do UNITE [ < Mon_def_sy{'chd`} > , Scan [ Rest ] ] ,
                       when 'CTL"as "...  "Rest"` guard true do UNITE [ < Mon_def_sy{'ctl`} > , Scan [ Rest ] ] ,
                       when 'DIM"as "...  "Rest"` guard true do UNITE [ < Mon_def_sy{'dim`} > , Scan [ Rest ] ] ,
                       when 'UNITE"as "...  "Rest"` guard true do UNITE [ < Praeop_sy{'unite`} > , Scan [ Rest ] ] ,
                       when 'SELECT"as "...  "Rest"` guard true do UNITE [ < Praeop_sy{'select`} > , Scan [ Rest ] ] ,
                       when 'CUT"as "...  "Rest"` guard true do UNITE [ < Praeop_sy{'cut`} > , Scan [ Rest ] ] ,
                       when 'NEG_CUT"as "...  "Rest"` guard true do UNITE [ < Praeop_sy{'neg_cut`} > , Scan [ Rest ] ] ,
                       when 'Pm_test"as "...  "Rest"` guard true do UNITE [ < Praeop_sy{'pm_test`} > , Scan [ Rest ] ] ,
                       when 'Struct_test"as "...  "Rest"` guard true do UNITE [ < Praeop_sy{'struct_test`} > , Scan [ Rest ] ] ,
                       when 'Struct_bind"as "...  "Rest"` guard true do UNITE [ < Praeop_sy{'struct_bind`} > , Scan [ Rest ] ] ,
                       when 'Rec_ap"as "...  "Rest"` guard true do UNITE [ < Praeop_sy{'rec_ap`} > , Scan [ Rest ] ] ,
                       when 'REPLACE"as "...  "Rest"` guard true do UNITE [ < Triop_sy{'replace`} > , Scan [ Rest ] ] ,
                       when 'if"as "...  "Rest"` guard true do UNITE [ < If_sy{ } > , Scan [ Rest ] ] ,
                       when 'then"as "...  "Rest"` guard true do UNITE [ < Then_sy{ } > , Scan [ Rest ] ] ,
                       when 'else"as "...  "Rest"` guard true do UNITE [ < Else_sy{ } > , Scan [ Rest ] ] ,
                       when 'true"as "...  "Rest"` guard true do UNITE [ < BOOL{true} > , Scan [ Rest ] ] ,
                       when 'false"as "...  "Rest"` guard true do UNITE [ < BOOL{false} > , Scan [ Rest ] ] ,
                       when 'let"as "...  "Rest"` guard true do UNITE [ < Let_sy{ } > , Scan [ Rest ] ] ,
                       when 'le"as "...  "Rest"` guard true do UNITE [ < Dyop_sy{'le`} > , Scan [ Rest ] ] ,
                       when 'def"as "...  "Rest"` guard true do UNITE [ < Letrec_sy{ } > , Scan [ Rest ] ] ,
                       when 'in"as "...  "Rest"` guard true do UNITE [ < In_sy{ } > , Scan [ Rest ] ] ,
                       when 'ap"as "...  "Rest"` guard true do UNITE [ < Ap_sy{ } > , Scan [ Rest ] ] ,
                       when 'case"as "...  "Rest"` guard true do UNITE [ < Case_sy{ } > , Scan [ Rest ] ] ,
                       when 'guard"as "...  "Rest"` guard true do UNITE [ < Guard_sy{ } > , Scan [ Rest ] ] ,
                       when 'when"as "...  "Rest"` guard true do UNITE [ < When_sy{ } > , Scan [ Rest ] ] ,
                       when 'otherwise"as "...  "Rest"` guard true do UNITE [ < Otherwise_sy{ } > , Scan [ Rest ] ] ,
                       when 'as"as "...  "Rest"` guard true do UNITE [ < As_sy{ } > , Scan [ Rest ] ] ,
                       when 'to"as "...  "Rest"` guard true do UNITE [ < To_sy{ } > , Scan [ Rest ] ] ,
                       when 'do"as "...  "Rest"` guard true do UNITE [ < Do_sy{ } > , Scan [ Rest ] ] ,
                       when 'end"as "...  "Rest"` guard true do UNITE [ < End_sy{ } > , Scan [ Rest ] ] ,
                       when '<>"as "...  "Rest"` guard true do UNITE [ < NIL{ } > , Scan [ Rest ] ] ,
                       when '<"as "...  "Rest"` guard true do UNITE [ < Sp_kl_auf{ } > , Scan [ Rest ] ] ,
                       when '>"as "...  "Rest"` guard true do UNITE [ < Sp_kl_zu{ } > , Scan [ Rest ] ] ,
                       when '["as "...  "Rest"` guard true do UNITE [ < E_kl_auf{ } > , Scan [ Rest ] ] ,
                       when ']"as "...  "Rest"` guard true do UNITE [ < E_kl_zu{ } > , Scan [ Rest ] ] ,
                       when '{"as "...  "Rest"` guard true do UNITE [ < Gesch_kl_auf{ } > , Scan [ Rest ] ] ,
                       when '}"as "...  "Rest"` guard true do UNITE [ < Gesch_kl_zu{ } > , Scan [ Rest ] ] ,
                       when '("as "...  "Rest"` guard true do UNITE [ < R_kl_auf{ } > , Scan [ Rest ] ] ,
                       when ')"as "...  "Rest"` guard true do UNITE [ < R_kl_zu{ } > , Scan [ Rest ] ] ,
                       when ',"as "...  "Rest"` guard true do UNITE [ < Komma_sy{ } > , Scan [ Rest ] ] ,
                       when '="as "...  "Rest"` guard true do UNITE [ < Eq_sy{ } > , Scan [ Rest ] ] ,
                       when '..."as "...  "Rest"` guard true do UNITE [ < Punkte_sy{ } > , Scan [ Rest ] ] ,
                       when 'A"as "...  "Rest"` guard true do Convert_id [ Rest , 'A` ] ,
                       when 'B"as "...  "Rest"` guard true do Convert_id [ Rest , 'B` ] ,
                       when 'C"as "...  "Rest"` guard true do Convert_id [ Rest , 'C` ] ,
                       when 'D"as "...  "Rest"` guard true do Convert_id [ Rest , 'D` ] ,
                       when 'E"as "...  "Rest"` guard true do Convert_id [ Rest , 'E` ] ,
                       when 'F"as "...  "Rest"` guard true do Convert_id [ Rest , 'F` ] ,
                       when 'G"as "...  "Rest"` guard true do Convert_id [ Rest , 'G` ] ,
                       when 'H"as "...  "Rest"` guard true do Convert_id [ Rest , 'H` ] ,
                       when 'I"as "...  "Rest"` guard true do Convert_id [ Rest , 'I` ] ,
                       when 'J"as "...  "Rest"` guard true do Convert_id [ Rest , 'J` ] ,
                       when 'K"as "...  "Rest"` guard true do Convert_id [ Rest , 'K` ] ,
                       when 'L"as "...  "Rest"` guard true do Convert_id [ Rest , 'L` ] ,
                       when 'M"as "...  "Rest"` guard true do Convert_id [ Rest , 'M` ] ,
                       when 'N"as "...  "Rest"` guard true do Convert_id [ Rest , 'N` ] ,
                       when 'O"as "...  "Rest"` guard true do Convert_id [ Rest , 'O` ] ,
                       when 'P"as "...  "Rest"` guard true do Convert_id [ Rest , 'P` ] ,
                       when 'Q"as "...  "Rest"` guard true do Convert_id [ Rest , 'Q` ] ,
                       when 'R"as "...  "Rest"` guard true do Convert_id [ Rest , 'R` ] ,
                       when 'S"as "...  "Rest"` guard true do Convert_id [ Rest , 'S` ] ,
                       when 'T"as "...  "Rest"` guard true do Convert_id [ Rest , 'T` ] ,
                       when 'U"as "...  "Rest"` guard true do Convert_id [ Rest , 'U` ] ,
                       when 'V"as "...  "Rest"` guard true do Convert_id [ Rest , 'V` ] ,
                       when 'W"as "...  "Rest"` guard true do Convert_id [ Rest , 'W` ] ,
                       when 'X"as "...  "Rest"` guard true do Convert_id [ Rest , 'X` ] ,
                       when 'Y"as "...  "Rest"` guard true do Convert_id [ Rest , 'Y` ] ,
                       when 'Z"as "...  "Rest"` guard true do Convert_id [ Rest , 'Z` ] ,
                       when ''"as "...  "S"`"as "...  "Rest"` guard true do UNITE [ < String_sy{S} > , Scan [ Rest ] ] ,
                       when '` guard true do <>
                       otherwise 'Fehler in Scan `
                       end
                    to [ Str ] ,
     Convert_bin [ S ] = def 
                           Next_dig [ S , Val ] = ap case 
                                                     when '0"as "...  "REST"` guard true do Next_dig [ REST , ( 10 * Val ) ] ,
                                                     when '1"as "...  "REST"` guard true do Next_dig [ REST , ( ( 10 * Val ) + 1 ) ] ,
                                                     when '2"as "...  "REST"` guard true do Next_dig [ REST , ( ( 10 * Val ) + 2 ) ] ,
                                                     when '3"as "...  "REST"` guard true do Next_dig [ REST , ( ( 10 * Val ) + 3 ) ] ,
                                                     when '4"as "...  "REST"` guard true do Next_dig [ REST , ( ( 10 * Val ) + 4 ) ] ,
                                                     when '5"as "...  "REST"` guard true do Next_dig [ REST , ( ( 10 * Val ) + 5 ) ] ,
                                                     when '6"as "...  "REST"` guard true do Next_dig [ REST , ( ( 10 * Val ) + 6 ) ] ,
                                                     when '7"as "...  "REST"` guard true do Next_dig [ REST , ( ( 10 * Val ) + 7 ) ] ,
                                                     when '8"as "...  "REST"` guard true do Next_dig [ REST , ( ( 10 * Val ) + 8 ) ] ,
                                                     when '9"as "...  "REST"` guard true do Next_dig [ REST , ( ( 10 * Val ) + 9 ) ] ,
                                                     when Rest guard true do < Val , Rest >
                                                     end
                                                  to [ S ]
                         in ap case 
                               when < Val , Rest > guard true do UNITE [ < Num_sy{if ( lselect( 1 , S ) eq '~` )
                                                                                  then ( ~1 * Val )
                                                                                  else Val} > , Scan [ Rest ] ]
                               end
                            to [ Next_dig [ if ( lselect( 1 , S ) eq '~` )
                                            then lcut( 1 , S )
                                            else S , 0 ] ] ,
     Convert_id [ S , Id ] = ap case 
                                when ' "as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when '{"as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when '}"as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when '<"as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when '>"as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when '["as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when ']"as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when '("as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when ')"as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when '+"as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when '-"as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when '*"as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when '/"as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when '="as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when ',"as "...  "REST"` guard true do UNITE [ < Id_sy{Id} > , Scan [ S ] ] ,
                                when '"as ".  "Symb""as "...  "REST"` guard true do Convert_id [ REST , UNITE [ Id , Symb ] ] ,
                                when '` guard true do < Id_sy{Id} >
                                otherwise 'Fehler in Convert_id`
                                end
                             to [ S ] ,
     Parse [ Inp ] = SELECT [ 1 , Expr [ Inp ] ] ,
     Expr [ Inp ] = ap case 
                       when < R_kl_auf{ } , as ... Rest > guard true do Dy_infix_op [ Rest ] ,
                       when < Monop_sy{Id} , R_kl_auf{ } , as ... Rest > guard true do Mon_apply [ Inp ] ,
                       when < Ap_sy{ } , as ... Rest > guard true do Ap_expr [ Rest ] ,
                       when < Let_sy{ } , as ... Rest > guard true do Let_expr [ Rest ] ,
                       when < Letrec_sy{ } , as ... Rest > guard true do Letrec_expr [ Rest ] ,
                       when < If_sy{ } , as ... Rest > guard true do Cond_expr [ Rest ] ,
                       when < Case_sy{ } , as ... Rest > guard true do Case_expr [ Rest ] ,
                       when < Id_sy{Id} , E_kl_auf{ } , as ... Rest > guard true do Func_call [ Id_sy{Id} , Rest ] ,
                       when < Id_sy{Id} , Gesch_kl_auf{ } , as ... Rest > guard true do User_kon [ Id , Rest ] ,
                       when < Sp_kl_auf{ } , as ... Rest > guard true do Liste [ Rest ] ,
                       when < Id_sy{Id} , as ... Rest > guard true do < Id_sy{Id} , Rest > ,
                       when < BOOL{B} , as ... Rest > guard true do < BOOL{B} , Rest > ,
                       when < Num_sy{N} , as ... Rest > guard true do < INT{N} , Rest > ,
                       when < NIL{ } , as ... Rest > guard true do < NIL{ } , Rest > ,
                       when < String_sy{S} , as ... Rest > guard true do < CCONS{KON{S} , NIL{ }} , Rest > ,
                       when < Dyop_sy{Sy} , as ... Rest > guard true do Dyop [ Dyop_sy{Sy} , Rest ] ,
                       when < Mon_def_sy{Sy} , as ... Rest > guard true do Mon_def_apply [ Inp ] ,
                       when < Praeop_sy{Sy} , as ... Rest > guard true do Praeop_ap [ Inp ] ,
                       when < Triop_sy{Sy} , as ... Rest > guard true do Triop_ap [ Inp ]
                       otherwise 'Fehler in expr`
                       end
                    to [ Inp ] ,
     Dy_infix_op [ Inp ] = ap case 
                              when < Op1 , Rest1 > guard true do ap case 
                                                                    when < Op_expr , Rest2 > guard true do ap case 
                                                                                                              when < Op2 , < R_kl_zu{ } , as ... Rest3 > > guard true do < AP{AP{Op_expr , Op1} , Op2} , Rest3 >
                                                                                                              end
                                                                                                           to [ Expr [ Rest2 ] ]
                                                                    end
                                                                 to [ Expr [ Rest1 ] ]
                              end
                           to [ Expr [ Inp ] ] ,
     Dyop [ Sy , Rest ] = ap case 
                             when Dyop_sy{'+`} guard true do < FUN{1} , Rest > ,
                             when Dyop_sy{'-`} guard true do < FUN{2} , Rest > ,
                             when Dyop_sy{'*`} guard true do < FUN{3} , Rest > ,
                             when Dyop_sy{'/`} guard true do < FUN{4} , Rest > ,
                             when Dyop_sy{'lt`} guard true do < FUN{6} , Rest > ,
                             when Dyop_sy{'le`} guard true do < FUN{7} , Rest > ,
                             when Dyop_sy{'eq`} guard true do < FUN{8} , Rest > ,
                             when Dyop_sy{'ne`} guard true do < FUN{9} , Rest > ,
                             when Dyop_sy{'ge`} guard true do < FUN{10} , Rest > ,
                             when Dyop_sy{'gt`} guard true do < FUN{11} , Rest > ,
                             when Dyop_sy{'and`} guard true do < FUN{12} , Rest > ,
                             when Dyop_sy{'or`} guard true do < FUN{13} , Rest > ,
                             when Dyop_sy{'cons`} guard true do < FUN{15} , Rest >
                             otherwise 'Fehler in dyop`
                             end
                          to [ Sy ] ,
     Mon_apply [ Inp ] = ap case 
                            when < Monop_sy{'empty`} , R_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                    when < Konstr , < R_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{18} , Konstr} , Rest2 >
                                                                                                    end
                                                                                                 to [ Expr [ Rest ] ] ,
                            when < Monop_sy{'not`} , R_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                  when < Konstr , < R_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{14} , Konstr} , Rest2 >
                                                                                                  end
                                                                                               to [ Expr [ Rest ] ] ,
                            when < Monop_sy{'neg`} , R_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                  when < Konstr , < R_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{5} , Konstr} , Rest2 >
                                                                                                  end
                                                                                               to [ Expr [ Rest ] ] ,
                            when < Monop_sy{'hd`} , R_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                 when < Konstr , < R_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{16} , Konstr} , Rest2 >
                                                                                                 end
                                                                                              to [ Expr [ Rest ] ] ,
                            when < Monop_sy{'tl`} , R_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                 when < Konstr , < R_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{17} , Konstr} , Rest2 >
                                                                                                 end
                                                                                              to [ Expr [ Rest ] ]
                            otherwise 'Fehler in mon_apply`
                            end
                         to [ Inp ] ,
     Mon_def_apply [ Inp ] = ap case 
                                when < Mon_def_sy{'test_cons`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                              when < Konstr , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{20} , Konstr} , Rest2 >
                                                                                                              end
                                                                                                           to [ Expr [ Rest ] ] ,
                                when < Mon_def_sy{'test_ccons`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                               when < Konstr , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{21} , Konstr} , Rest2 >
                                                                                                               end
                                                                                                            to [ Expr [ Rest ] ] ,
                                when < Mon_def_sy{'test_kon`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                             when < Konstr , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{22} , Konstr} , Rest2 >
                                                                                                             end
                                                                                                          to [ Expr [ Rest ] ] ,
                                when < Mon_def_sy{'test_int`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                             when < Konstr , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{23} , Konstr} , Rest2 >
                                                                                                             end
                                                                                                          to [ Expr [ Rest ] ] ,
                                when < Mon_def_sy{'test_bool`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                              when < Konstr , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{24} , Konstr} , Rest2 >
                                                                                                              end
                                                                                                           to [ Expr [ Rest ] ] ,
                                when < Mon_def_sy{'test_var`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                             when < Konstr , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{25} , Konstr} , Rest2 >
                                                                                                             end
                                                                                                          to [ Expr [ Rest ] ] ,
                                when < Mon_def_sy{'test_null`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                              when < Konstr , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{38} , Konstr} , Rest2 >
                                                                                                              end
                                                                                                           to [ Expr [ Rest ] ] ,
                                when < Mon_def_sy{'chd`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                        when < Konstr , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{26} , Konstr} , Rest2 >
                                                                                                        end
                                                                                                     to [ Expr [ Rest ] ] ,
                                when < Mon_def_sy{'ctl`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                        when < Konstr , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{27} , Konstr} , Rest2 >
                                                                                                        end
                                                                                                     to [ Expr [ Rest ] ] ,
                                when < Mon_def_sy{'dim`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                        when < Konstr , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{FUN{28} , Konstr} , Rest2 >
                                                                                                        end
                                                                                                     to [ Expr [ Rest ] ]
                                end
                             to [ Inp ] ,
     Praeop_ap [ Inp ] = ap case 
                            when < Praeop_sy{'unite`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                     when < Konstr , < Komma_sy{ } , as ... Rest1 > > guard true do ap case 
                                                                                                                                                                       when < Konstr2 , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{AP{FUN{29} , Konstr} , Konstr2} , Rest2 >
                                                                                                                                                                       end
                                                                                                                                                                    to [ Expr [ Rest1 ] ]
                                                                                                     end
                                                                                                  to [ Expr [ Rest ] ] ,
                            when < Praeop_sy{'select`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                      when < Konstr , < Komma_sy{ } , as ... Rest1 > > guard true do ap case 
                                                                                                                                                                        when < Konstr2 , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{AP{FUN{32} , Konstr} , Konstr2} , Rest2 >
                                                                                                                                                                        end
                                                                                                                                                                     to [ Expr [ Rest1 ] ]
                                                                                                      end
                                                                                                   to [ Expr [ Rest ] ] ,
                            when < Praeop_sy{'cut`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                   when < Konstr , < Komma_sy{ } , as ... Rest1 > > guard true do ap case 
                                                                                                                                                                     when < Konstr2 , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{AP{FUN{30} , Konstr} , Konstr2} , Rest2 >
                                                                                                                                                                     end
                                                                                                                                                                  to [ Expr [ Rest1 ] ]
                                                                                                   end
                                                                                                to [ Expr [ Rest ] ] ,
                            when < Praeop_sy{'neg_cut`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                       when < Konstr , < Komma_sy{ } , as ... Rest1 > > guard true do ap case 
                                                                                                                                                                         when < Konstr2 , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{AP{FUN{31} , Konstr} , Konstr2} , Rest2 >
                                                                                                                                                                         end
                                                                                                                                                                      to [ Expr [ Rest1 ] ]
                                                                                                       end
                                                                                                    to [ Expr [ Rest ] ] ,
                            when < Praeop_sy{'pm_test`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                       when < Konstr , < Komma_sy{ } , as ... Rest1 > > guard true do ap case 
                                                                                                                                                                         when < Konstr2 , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{AP{FUN{34} , Konstr} , Konstr2} , Rest2 >
                                                                                                                                                                         end
                                                                                                                                                                      to [ Expr [ Rest1 ] ]
                                                                                                       end
                                                                                                    to [ Expr [ Rest ] ] ,
                            when < Praeop_sy{'struct_test`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                           when < Konstr , < Komma_sy{ } , as ... Rest1 > > guard true do ap case 
                                                                                                                                                                             when < Konstr2 , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{AP{FUN{35} , Konstr} , Konstr2} , Rest2 >
                                                                                                                                                                             end
                                                                                                                                                                          to [ Expr [ Rest1 ] ]
                                                                                                           end
                                                                                                        to [ Expr [ Rest ] ] ,
                            when < Praeop_sy{'struct_bind`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                           when < Konstr , < Komma_sy{ } , as ... Rest1 > > guard true do ap case 
                                                                                                                                                                             when < Konstr2 , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{AP{FUN{36} , Konstr} , Konstr2} , Rest2 >
                                                                                                                                                                             end
                                                                                                                                                                          to [ Expr [ Rest1 ] ]
                                                                                                           end
                                                                                                        to [ Expr [ Rest ] ] ,
                            when < Praeop_sy{'rec_ap`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                      when < Konstr , < Komma_sy{ } , as ... Rest1 > > guard true do ap case 
                                                                                                                                                                        when < Konstr2 , < E_kl_zu{ } , as ... Rest2 > > guard true do < AP{AP{FUN{37} , Konstr} , Konstr2} , Rest2 >
                                                                                                                                                                        end
                                                                                                                                                                     to [ Expr [ Rest1 ] ]
                                                                                                      end
                                                                                                   to [ Expr [ Rest ] ]
                            end
                         to [ Inp ] ,
     Triop_ap [ Inp ] = ap case 
                           when < Praeop_sy{'replace`} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                      when < Konstr , < Komma_sy{ } , as ... Rest1 > > guard true do ap case 
                                                                                                                                                                        when < Konstr2 , < Komma_sy{ } , as ... Rest2 > > guard true do ap case 
                                                                                                                                                                                                                                           when < Konstr3 , < E_kl_zu{ } , as ... Rest3 > > guard true do < AP{AP{AP{FUN{33} , Konstr} , Kontr2} , Konstr3} , Rest3 >
                                                                                                                                                                                                                                           end
                                                                                                                                                                                                                                        to [ Expr [ Rest2 ] ]
                                                                                                                                                                        end
                                                                                                                                                                     to [ Expr [ Rest1 ] ]
                                                                                                      end
                                                                                                   to [ Expr [ Rest ] ]
                           end
                        to [ Inp ] ,
     Ap_expr [ Inp ] = ap case 
                          when < Konstr1 , < To_sy{ } , E_kl_auf{ } , as ... Rest1 > > guard true do ap case 
                                                                                                        when < Konstr2 , < E_kl_zu{ } , as ... Rest2 > > guard true do < Konstr2 , Rest2 >
                                                                                                        end
                                                                                                     to [ Expr_list [ Konstr1 , Rest1 ] ]
                          end
                       to [ Expr [ Inp ] ] ,
     Expr_list [ Konstr , Inp ] = ap case 
                                     when < Konstr1 , < Komma_sy{ } , as ... Rest1 > > guard true do Expr_list [ AP{Konstr , Konstr1} , Rest1 ] ,
                                     when < Konstr2 , Rest2 > guard true do < AP{Konstr , Konstr2} , Rest2 >
                                     otherwise 'Fehler in expe_list`
                                     end
                                  to [ Expr [ Inp ] ] ,
     Let_expr [ Inp ] = ap case 
                           when < Konstr , Rest > guard true do ap case 
                                                                   when < Konstr2 , Rest2 > guard true do < LET{Konstr , Konstr2} , Rest2 >
                                                                   end
                                                                to [ Expr [ Rest ] ]
                           end
                        to [ Let_list [ <> , Inp ] ] ,
     Let_list [ Konstr , Inp ] = ap case 
                                    when < Id_sy{Id} , Eq_sy{ } , as ... Rest > guard true do ap case 
                                                                                                 when < Konstr2 , < Komma_sy{ } , as ... Rest2 > > guard true do Let_list [ UNITE [ Konstr , < < Id_sy{Id} , Konstr2 > > ] , Rest2 ] ,
                                                                                                 when < Konstr2 , < In_sy{ } , as ... Rest2 > > guard true do < UNITE [ Konstr , < < Id_sy{Id} , Konstr2 > > ] , Rest2 >
                                                                                                 end
                                                                                              to [ Expr [ Rest ] ]
                                    otherwise 'Fehler in let_list`
                                    end
                                 to [ Inp ] ,
     Letrec_expr [ Inp ] = ap case 
                              when < Konstr , Rest > guard true do ap case 
                                                                      when < Konstr2 , Rest2 > guard true do < LETREC{Konstr , Konstr2} , Rest2 >
                                                                      end
                                                                   to [ Expr [ Rest ] ]
                              end
                           to [ Letrec_list [ <> , Inp ] ] ,
     Letrec_list [ Konstr , Inp ] = ap case 
                                       when < Id_sy{Id} , E_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                                       when < Konstr1 , < E_kl_zu{ } , Eq_sy{ } , as ... Rest2 > > guard true do ap case 
                                                                                                                                                                                    when < Konstr2 , < In_sy{ } , as ... Rest3 > > guard true do < UNITE [ Konstr , < < Id_sy{Id} , Konstr1 , Konstr2 > > ] , Rest3 > ,
                                                                                                                                                                                    when < Konstr2 , Rest3 > guard true do Letrec_list [ UNITE [ Konstr , < < Id_sy{Id} , Konstr1 , Konstr2 > > ] , Rest3 ]
                                                                                                                                                                                    end
                                                                                                                                                                                 to [ Expr [ Rest2 ] ]
                                                                                                       end
                                                                                                    to [ Var_list [ <> , Rest ] ]
                                       end
                                    to [ Inp ] ,
     Var_list [ Liste , Inp ] = ap case 
                                   when < Id_sy{Id} , Komma_sy{ } , as ... Rest > guard true do Var_list [ UNITE [ Liste , < Id_sy{Id} > ] , Rest ] ,
                                   when < Id_sy{Id} , as ... Rest > guard true do < UNITE [ Liste , < Id_sy{Id} > ] , Rest > ,
                                   when < E_kl_zu{ } , as ... Rest > guard true do < <> , Inp >
                                   end
                                to [ Inp ] ,
     Cond_expr [ Inp ] = ap case 
                            when < Konstr , < Then_sy{ } , as ... Rest > > guard true do ap case 
                                                                                            when < Konstr2 , < Else_sy{ } , as ... Rest2 > > guard true do ap case 
                                                                                                                                                              when < Konstr3 , Rest3 > guard true do < AP{AP{AP{FUN{19} , Konstr} , Konstr2} , Konstr3} , Rest3 >
                                                                                                                                                              end
                                                                                                                                                           to [ Expr [ Rest2 ] ]
                                                                                            end
                                                                                         to [ Expr [ Rest ] ]
                            end
                         to [ Expr [ Inp ] ] ,
     Case_expr [ Inp ] = ap case 
                            when < Konstr , Rest > guard true do < LETREC{< < Id_sy{'Case_func`} , < Id_sy{'Arg`} > , AP{AP{FUN{34} , Konstr} , Id_sy{'Arg`}} > > , Id_sy{'Case_func`}} , Rest >
                            end
                         to [ When_list [ Inp ] ] ,
     When_list [ Inp ] = ap case 
                            when < Konstr , < Komma_sy{ } , as ... Rest > > guard true do ap case 
                                                                                             when < Konstr_list , Rest2 > guard true do < CONS{Konstr , Konstr_list} , Rest2 >
                                                                                             end
                                                                                          to [ When_list [ Rest ] ] ,
                            when < Konstr , < Otherwise_sy{ } , as ... Rest > > guard true do ap case 
                                                                                                 when < Oth_expr , < End_sy{ } , as ... Rest2 > > guard true do < CONS{Konstr , CONS{Oth_expr , NIL{ }}} , Rest2 >
                                                                                                 end
                                                                                              to [ Expr [ Rest ] ] ,
                            when < Konstr , < End_sy{ } , as ... Rest > > guard true do < CONS{Konstr , CONS{CCONS{KON{'Fehler`} , NIL{ }} , NIL{ }}} , Rest >
                            end
                         to [ When_expr [ Inp ] ] ,
     When_expr [ Inp ] = ap case 
                            when < When_sy{ } , as ... Rest > guard true do ap case 
                                                                               when < < Pat_struct , Pvar_list > , < Guard_sy{ } , as ... Rest2 > > guard true do ap case 
                                                                                                                                                                     when < Guard_expr , < Do_sy{ } , as ... Rest3 > > guard true do ap case 
                                                                                                                                                                                                                                        when < Body_expr , Rest4 > guard true do < CONS{Pat_struct , CONS{LETREC{< < Id_sy{'G_func`} , Pvar_list , Guard_expr > > , Id_sy{'G_func`}} , CONS{LETREC{< < Id_sy{'B_func`} , Pvar_list , Body_expr > > , Id_sy{'B_func`}} , NIL{ }}}} , Rest4 >
                                                                                                                                                                                                                                        end
                                                                                                                                                                                                                                     to [ Expr [ Rest3 ] ]
                                                                                                                                                                     end
                                                                                                                                                                  to [ Expr [ Rest2 ] ]
                                                                               end
                                                                            to [ Pattern [ Rest ] ]
                            end
                         to [ Inp ] ,
     Pattern [ Inp ] = ap case 
                          when < Id_sy{Id} , Gesch_kl_auf{ } , Gesch_kl_zu{ } , as ... Rest > guard true do < < CCONS{KON{Id} , NIL{ }} , <> > , Rest > ,
                          when < Id_sy{Id} , Gesch_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                                              when < < Pat , Liste > , < Gesch_kl_zu , as ... Rest2 > > guard true do < < CCONS{KON{Id} , CCONS{Pat , NIL{ }}} , Liste > , Rest2 >
                                                                                              end
                                                                                           to [ Pattern [ Rest ] ] ,
                          when < Id_sy{Id} , as ... Rest > guard true do < < PVAR{ } , < Id_sy{Id} > > , Rest > ,
                          when < BOOL{B} , as ... Rest > guard true do < < BOOL{B} , <> > , Rest > ,
                          when < Num_sy{N} , as ... Rest > guard true do < < INT{N} , <> > , Rest > ,
                          when < NIL{ } , as ... Rest > guard true do < < NIL{ } , <> > , Rest > ,
                          when < String_sy{Str} , as ... Rest > guard true do < < CCONS{KON{Str} , NIL{ }} , <> > , Rest > ,
                          when < Sp_kl_auf{ } , as ... Rest > guard true do ap case 
                                                                               when < < Pat , Liste > , < Sp_kl_zu{ } , as ... Rest2 > > guard true do < < Pat , Liste > , Rest2 >
                                                                               end
                                                                            to [ Pattern_list [ Rest ] ]
                          end
                       to [ Inp ] ,
     Pattern_list [ Inp ] = ap case 
                               when < As_sy{ } , Punkte_sy{ } , Id_sy{Id} , as ... Rest > guard true do < < PVAR{ } , < Id_sy{Id} > > , Rest > ,
                               when Inp guard true do ap case 
                                                         when < < Pat , Liste > , < Komma_sy{ } , as ... Rest2 > > guard true do ap case 
                                                                                                                                    when < < Pat2 , Liste2 > , Rest3 > guard true do < < CONS{Pat , Pat2} , UNITE [ Liste , Liste2 ] > , Rest3 >
                                                                                                                                    end
                                                                                                                                 to [ Pattern_list [ Rest2 ] ] ,
                                                         when < < Pat , Liste > , Rest2 > guard true do < < CONS{Pat , NIL{ }} , Liste > , Rest2 >
                                                         end
                                                      to [ Pattern [ Inp ] ]
                               end
                            to [ Inp ] ,
     Func_call [ Id , Inp ] = ap case 
                                 when < Konstr , < E_kl_zu{ } , as ... Rest > > guard true do < Konstr , Rest >
                                 end
                              to [ Expr_list [ Id , Inp ] ] ,
     User_kon [ Id , Inp ] = ap case 
                                when < Konstr , Rest > guard true do < CCONS{KON{Id} , Konstr} , Rest >
                                end
                             to [ Kon_list [ Inp ] ] ,
     Kon_list [ Inp ] = ap case 
                           when < Gesch_kl_zu{ } , as ... Rest > guard true do < NIL{ } , Rest > ,
                           when Rest guard true do ap case 
                                                      when < Konstr1 , < Komma_sy{ } , as ... Rest1 > > guard true do ap case 
                                                                                                                         when < Rest_konstr , Rest_inp > guard true do < CCONS{Konstr1 , Rest_konstr} , Rest_inp >
                                                                                                                         otherwise 'Fehler in Kon_list`
                                                                                                                         end
                                                                                                                      to [ Kon_list [ Rest1 ] ] ,
                                                      when < Konstr2 , < Gesch_kl_zu{ } , as ... Rest2 > > guard true do < CCONS{Konstr2 , NIL{ }} , Rest2 >
                                                      otherwise 'Fehler in Kon_list`
                                                      end
                                                   to [ Expr [ Rest ] ]
                           end
                        to [ Inp ] ,
     Liste [ Inp ] = ap case 
                        when < Konstr1 , < Komma_sy{ } , as ... Rest1 > > guard true do ap case 
                                                                                           when < Rest_konstr , Rest_inp > guard true do < CONS{Konstr1 , Rest_konstr} , Rest_inp >
                                                                                           otherwise 'Fehler in Liste`
                                                                                           end
                                                                                        to [ Liste [ Rest1 ] ] ,
                        when < Konstr2 , < Sp_kl_zu{ } , as ... Rest2 > > guard true do < CONS{Konstr2 , NIL{ }} , Rest2 >
                        otherwise 'Fehler in Liste`
                        end
                     to [ Expr [ Inp ] ]
   in def 
        Start [ Inp ] = SELECT [ 1 , Conv_name [ Parse [ Scan [ Inp ] ] , <> , 39 , 1 ] ] ,
        Conv_name [ Expr , Name_list , F_n , V_n ] = ap case 
                                                        when KON{A} guard true do < KON{A} , F_n , V_n > ,
                                                        when INT{I} guard true do < INT{I} , F_n , V_n > ,
                                                        when BOOL{B} guard true do < BOOL{B} , F_n , V_n > ,
                                                        when NIL{ } guard true do < NIL{ } , F_n , V_n > ,
                                                        when FUN{F} guard true do < FUN{F} , F_n , V_n > ,
                                                        when VAR{I} guard true do < VAR{I} , F_n , V_n > ,
                                                        when PVAR{ } guard true do < PVAR{ } , F_n , V_n > ,
                                                        when ADD{E1 , E2} guard true do ap case 
                                                                                           when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                            when < E2_neu , F , V > guard true do < ADD{E1_neu , E2_neu} , F , V >
                                                                                                                                            end
                                                                                                                                         to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                           end
                                                                                        to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when SUB{E1 , E2} guard true do ap case 
                                                                                           when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                            when < E2_neu , F , V > guard true do < SUB{E1_neu , E2_neu} , F , V >
                                                                                                                                            end
                                                                                                                                         to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                           end
                                                                                        to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when MULT{E1 , E2} guard true do ap case 
                                                                                            when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                             when < E2_neu , F , V > guard true do < MULT{E1_neu , E2_neu} , F , V >
                                                                                                                                             end
                                                                                                                                          to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                            end
                                                                                         to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when DIV{E1 , E2} guard true do ap case 
                                                                                           when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                            when < E2_neu , F , V > guard true do < DIV{E1_neu , E2_neu} , F , V >
                                                                                                                                            end
                                                                                                                                         to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                           end
                                                                                        to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when EQ{E1 , E2} guard true do ap case 
                                                                                          when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                           when < E2_neu , F , V > guard true do < EQ{E1_neu , E2_neu} , F , V >
                                                                                                                                           end
                                                                                                                                        to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                          end
                                                                                       to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when NE{E1 , E2} guard true do ap case 
                                                                                          when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                           when < E2_neu , F , V > guard true do < NE{E1_neu , E2_neu} , F , V >
                                                                                                                                           end
                                                                                                                                        to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                          end
                                                                                       to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when GT{E1 , E2} guard true do ap case 
                                                                                          when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                           when < E2_neu , F , V > guard true do < GT{E1_neu , E2_neu} , F , V >
                                                                                                                                           end
                                                                                                                                        to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                          end
                                                                                       to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when GE{E1 , E2} guard true do ap case 
                                                                                          when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                           when < E2_neu , F , V > guard true do < GE{E1_neu , E2_neu} , F , V >
                                                                                                                                           end
                                                                                                                                        to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                          end
                                                                                       to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when LT{E1 , E2} guard true do ap case 
                                                                                          when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                           when < E2_neu , F , V > guard true do < LT{E1_neu , E2_neu} , F , V >
                                                                                                                                           end
                                                                                                                                        to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                          end
                                                                                       to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when LE{E1 , E2} guard true do ap case 
                                                                                          when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                           when < E2_neu , F , V > guard true do < LE{E1_neu , E2_neu} , F , V >
                                                                                                                                           end
                                                                                                                                        to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                          end
                                                                                       to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when AND{E1 , E2} guard true do ap case 
                                                                                           when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                            when < E2_neu , F , V > guard true do < AND{E1_neu , E2_neu} , F , V >
                                                                                                                                            end
                                                                                                                                         to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                           end
                                                                                        to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when OR{E1 , E2} guard true do ap case 
                                                                                          when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                           when < E2_neu , F , V > guard true do < OR{E1_neu , E2_neu} , F , V >
                                                                                                                                           end
                                                                                                                                        to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                          end
                                                                                       to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when CONS{E1 , E2} guard true do ap case 
                                                                                            when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                             when < E2_neu , F , V > guard true do < CONS{E1_neu , E2_neu} , F , V >
                                                                                                                                             end
                                                                                                                                          to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                            end
                                                                                         to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when CCONS{E1 , E2} guard true do ap case 
                                                                                             when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                              when < E2_neu , F , V > guard true do < CCONS{E1_neu , E2_neu} , F , V >
                                                                                                                                              end
                                                                                                                                           to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                             end
                                                                                          to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when IF{E1 , E2 , E3} guard true do ap case 
                                                                                               when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                                when < E2_neu , F , V > guard true do ap case 
                                                                                                                                                                                         when < E3_neu , F1 , V1 > guard true do < IF{E1_neu , E2_neu , E3_neu} , F1 , V1 >
                                                                                                                                                                                         end
                                                                                                                                                                                      to [ Conv_name [ E3 , Name_list , F , V ] ]
                                                                                                                                                end
                                                                                                                                             to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                               end
                                                                                            to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when AP{E1 , E2} guard true do ap case 
                                                                                          when < E1_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                           when < E2_neu , F , V > guard true do < AP{E1_neu , E2_neu} , F , V >
                                                                                                                                           end
                                                                                                                                        to [ Conv_name [ E2 , Name_list , F_neu , V_neu ] ]
                                                                                          end
                                                                                       to [ Conv_name [ E1 , Name_list , F_n , V_n ] ] ,
                                                        when NOT{E} guard true do ap case 
                                                                                     when < E_neu , F_neu , V_neu > guard true do < NOT{E_neu} , F_neu , V_neu >
                                                                                     end
                                                                                  to [ Conv_name [ E , Name_list , F_n , V_n ] ] ,
                                                        when NEG{E} guard true do ap case 
                                                                                     when < E_neu , F_neu , V_neu > guard true do < NEG{E_neu} , F_neu , V_neu >
                                                                                     end
                                                                                  to [ Conv_name [ E , Name_list , F_n , V_n ] ] ,
                                                        when HD{E} guard true do ap case 
                                                                                    when < E_neu , F_neu , V_neu > guard true do < HD{E_neu} , F_neu , V_neu >
                                                                                    end
                                                                                 to [ Conv_name [ E , Name_list , F_n , V_n ] ] ,
                                                        when TL{E} guard true do ap case 
                                                                                    when < E_neu , F_neu , V_neu > guard true do < TL{E_neu} , F_neu , V_neu >
                                                                                    end
                                                                                 to [ Conv_name [ E , Name_list , F_n , V_n ] ] ,
                                                        when NULL{E} guard true do ap case 
                                                                                      when < E_neu , F_neu , V_neu > guard true do < NULL{E_neu} , F_neu , V_neu >
                                                                                      end
                                                                                   to [ Conv_name [ E , Name_list , F_n , V_n ] ] ,
                                                        when Id_sy{Id} guard true do < Suche_id [ Id , Name_list ] , F_n , V_n > ,
                                                        when LET{L , E} guard true do ap case 
                                                                                         when < Let_list , Name_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                                       when < Expr , F , V > guard true do < LET{Let_list , Expr} , F , V >
                                                                                                                                                       end
                                                                                                                                                    to [ Conv_name [ E , UNITE [ Name_neu , Name_list ] , F_neu , V_neu ] ]
                                                                                         end
                                                                                      to [ Build_let [ L , Name_list , F_n , V_n ] ] ,
                                                        when LETREC{Letr , E} guard true do let New_names = Letr_names [ Letr , Name_list , F_n ] ,
                                                                                                F_neu = ( F_n + DIM [ Letr ] )
                                                                                            in ap case 
                                                                                                  when < Letr_neu , F , V > guard true do ap case 
                                                                                                                                             when < Expr , F1 , V1 > guard true do < LETREC{Letr_neu , Expr} , F1 , V1 >
                                                                                                                                             end
                                                                                                                                          to [ Conv_name [ E , New_names , F , V ] ]
                                                                                                  end
                                                                                               to [ Build_letr [ Letr , New_names , F_neu , V_n ] ]
                                                        otherwise 'Fehler in conv_name`
                                                        end
                                                     to [ Expr ] ,
        Suche_id [ Id , Names ] = ap case 
                                     when < < S , VAR{I} , 0 > , as ... Rest > guard ( S eq Id ) do VAR{I} ,
                                     when < < S , FUN{I} , 0 > , as ... Rest > guard ( S eq Id ) do FUN{I} ,
                                     when < < S , FUN{I} , 1 > , as ... Rest > guard ( S eq Id ) do AP{FUN{I} , BOOL{true}} ,
                                     when < H , as ... Rest > guard true do Suche_id [ Id , Rest ] ,
                                     when <> guard true do 'Fehler in suche_id: Variable ganz frei`
                                     end
                                  to [ Names ] ,
        Build_let [ Let_list , Names , F_n , V_n ] = ap case 
                                                        when < < Id_sy{Id} , E > , as ... Rest > guard true do ap case 
                                                                                                                  when < E_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                                                  when < Let_neu , Name , F , V > guard true do < UNITE [ < < VAR{V} , E_neu > > , Let_neu ] , UNITE [ < < Id , VAR{V} , 0 > > , Name ] , F , ( V + 1 ) >
                                                                                                                                                                  end
                                                                                                                                                               to [ Build_let [ Rest , Names , F_neu , V_neu ] ]
                                                                                                                  end
                                                                                                               to [ Conv_name [ E , Names , F_n , V_n ] ] ,
                                                        when <> guard true do < <> , <> , F_n , V_n >
                                                        end
                                                     to [ Let_list ] ,
        Build_letr [ Letr , Names , F , V ] = ap case 
                                                 when < < Id_sy{Id} , <> , E > , as ... Rest > guard true do ap case 
                                                                                                                when < E_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                                                when < Letr_neu , F1 , V1 > guard true do < UNITE [ < < ap case 
                                                                                                                                                                                                                           when AP{F , E} guard true do F
                                                                                                                                                                                                                           end
                                                                                                                                                                                                                        to [ Suche_id [ Id , Names ] ] , < VAR{V1} > , E_neu > > , Letr_neu ] , F1 , ( V1 + 1 ) >
                                                                                                                                                                end
                                                                                                                                                             to [ Build_letr [ Rest , Names , F_neu , V_neu ] ]
                                                                                                                end
                                                                                                             to [ Conv_name [ E , Names , F , V ] ] ,
                                                 when < < Id_sy{Id} , Parms , E > , as ... Rest > guard true do ap case 
                                                                                                                   when < Names_neu , Parms_neu > guard true do ap case 
                                                                                                                                                                   when < E_neu , F_neu , V_neu > guard true do ap case 
                                                                                                                                                                                                                   when < Letr_neu , F1 , V1 > guard true do < UNITE [ < < Suche_id [ Id , Names ] , Parms_neu , E_neu > > , Letr_neu ] , F1 , V1 >
                                                                                                                                                                                                                   end
                                                                                                                                                                                                                to [ Build_letr [ Rest , Names , F_neu , V_neu ] ]
                                                                                                                                                                   end
                                                                                                                                                                to [ Conv_name [ E , Names_neu , F , ( V + DIM [ Parms ] ) ] ]
                                                                                                                   end
                                                                                                                to [ Add_parms [ Parms , Names , V , <> ] ] ,
                                                 when <> guard true do < <> , F , V >
                                                 end
                                              to [ Letr ] ,
        Add_parms [ Parms , Names , V , P_neu ] = ap case 
                                                     when < Id_sy{Id} , as ... Rest > guard true do Add_parms [ Rest , UNITE [ < < Id , VAR{V} , 0 > > , Names ] , ( V + 1 ) , UNITE [ P_neu , < VAR{V} > ] ] ,
                                                     when <> guard true do < Names , P_neu >
                                                     end
                                                  to [ Parms ] ,
        Letr_names [ Letr , Names , F_n ] = ap case 
                                               when < < Id_sy{Id} , <> , E > , as ... Rest > guard true do Letr_names [ Rest , UNITE [ < < Id , FUN{F_n} , 1 > > , Names ] , ( F_n + 1 ) ] ,
                                               when < < Id_sy{Id} , Parms , E > , as ... Rest > guard true do Letr_names [ Rest , UNITE [ < < Id , FUN{F_n} , 0 > > , Names ] , ( F_n + 1 ) ] ,
                                               when <> guard true do Names
                                               end
                                            to [ Letr ]
      in def 
           U [ M1 , M2 ] = if empty( M1 )
                           then M2
                           else if empty( M2 )
                                then M1
                                else if IN [ SELECT [ 1 , M1 ] , M2 ]
                                     then U [ CUT [ 1 , M1 ] , M2 ]
                                     else UNITE [ < SELECT [ 1 , M1 ] > , U [ CUT [ 1 , M1 ] , M2 ] ] ,
           U_sol [ Sol1 , Sol2 ] = if empty( Sol1 )
                                   then Sol2
                                   else if empty( Sol2 )
                                        then Sol1
                                        else if In_sol [ SELECT [ 1 , SELECT [ 1 , Sol1 ] ] , Sol2 ]
                                             then U_sol [ CUT [ 1 , Sol1 ] , Sol2 ]
                                             else UNITE [ < SELECT [ 1 , Sol1 ] > , U_sol [ CUT [ 1 , Sol1 ] , Sol2 ] ] ,
           S [ M1 , M2 ] = if ( empty( M1 ) or empty( M2 ) )
                           then <>
                           else if IN [ SELECT [ 1 , M1 ] , M2 ]
                                then UNITE [ < SELECT [ 1 , M1 ] > , S [ CUT [ 1 , M1 ] , M2 ] ]
                                else S [ CUT [ 1 , M1 ] , M2 ] ,
           IN [ El , M ] = ap case 
                              when < X , <> > guard true do false ,
                              when < FUN{I} , < FUN{J} , ... > > guard ( I eq J ) do true ,
                              when < VAR{I} , < VAR{J} , ... > > guard ( I eq J ) do true
                              otherwise IN [ El , CUT [ 1 , M ] ]
                              end
                           to [ < El , M > ] ,
           In_sol [ F , Sol ] = ap case 
                                   when < X , <> > guard true do false ,
                                   when < FUN{I} , < < FUN{J} , Var_list > , as ... Rest > > guard true do if ( I eq J )
                                                                                                           then true
                                                                                                           else In_sol [ F , Rest ]
                                   end
                                to [ < F , Sol > ] ,
           E_fi [ Fi , Letr , Vars , Funs , Sol ] = E{SELECT [ 1 , Fi ] , S_fi [ Letr , Fi , Vars , Funs , Sol ] , G_h [ Letr , Fi ]} ,
           Liste_aller_Efi_aus_letr [ Letr , R_letr , Vars , Funs , Sol ] = ap case 
                                                                               when < < F , V , E > , as ... Rest > guard true do UNITE [ < E_fi [ < F , V , E > , Letr , Vars , Funs , Sol ] > , Liste_aller_Efi_aus_letr [ Letr , Rest , Vars , Funs , Sol ] ] ,
                                                                               when < < Id , Expr > , as ... Rest > guard true do Liste_aller_Efi_aus_letr [ Letr , Rest , Vars , Funs , Sol ] ,
                                                                               when <> guard true do <>
                                                                               end
                                                                            to [ R_letr ] ,
           S_fi [ Letr , Fi , Vars , Funs , Sol ] = let Fiei = Fi_ei [ Fi ]
                                                    in U [ S [ U [ V1_Vn [ Letr ] , Vars ] , Fiei ] , let G = G1_Gn [ Funs , Fiei ]
                                                                                                      in EG1_Gn [ G , Sol ] ] ,
           EG1_Gn [ G , Sol ] = if empty( G )
                                then <>
                                else U [ Such_fi_in_sol [ SELECT [ 1 , G ] , Sol ] , EG1_Gn [ CUT [ 1 , G ] , Sol ] ] ,
           Such_fi_in_sol [ F , Sol ] = if empty( Sol )
                                        then <>
                                        else ap case 
                                                when < < < FUN{I} , Liste > , as ... Rest > , FUN{J} > guard true do if ( I eq J )
                                                                                                                     then Liste
                                                                                                                     else Such_fi_in_sol [ F , Rest ]
                                                otherwise 'Fehler in Such_fi_in_sol`
                                                end
                                             to [ < Sol , F > ] ,
           Fi_ei [ Fi ] = ap case 
                             when < Id , Var_list , Expr > guard true do Trav [ Expr ]
                             otherwise 'Fehler in Fi_ei`
                             end
                          to [ Fi ] ,
           Trav [ Expr ] = ap case 
                              when KON{A} guard true do <> ,
                              when INT{I} guard true do <> ,
                              when BOOL{B} guard true do <> ,
                              when NIL{ } guard true do <> ,
                              when FUN{F} guard true do < FUN{F} > ,
                              when VAR{I} guard true do < VAR{I} > ,
                              when PVAR{ } guard true do < PVAR{ } > ,
                              when ADD{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when SUB{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when MULT{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when DIV{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when EQ{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when NE{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when GT{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when GE{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when LT{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when LE{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when AND{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when OR{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when CONS{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when CCONS{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when IF{E1 , E2 , E3} guard true do U [ Trav [ E1 ] , U [ Trav [ E2 ] , Trav [ E3 ] ] ] ,
                              when AP{E1 , E2} guard true do U [ Trav [ E1 ] , Trav [ E2 ] ] ,
                              when NOT{E} guard true do Trav [ E ] ,
                              when NEG{E} guard true do Trav [ E ] ,
                              when HD{E} guard true do Trav [ E ] ,
                              when TL{E} guard true do Trav [ E ] ,
                              when NULL{E} guard true do Trav [ E ] ,
                              when LET{< < Id , Ex > , as ... Rest > , E} guard true do U [ U [ U [ < Id > , Trav [ Ex ] ] , Trav [ Rest ] ] , Trav [ E ] ] ,
                              when <> guard true do <> ,
                              when < < Id , Ex > , as ... Rest > guard true do U [ U [ < Id > , Trav [ Ex ] ] , Trav [ Rest ] ] ,
                              when LETREC{< < Id , Var_list , Ex > , as ... Rest > , E} guard true do U [ U [ U [ U [ < Id > , Var_list ] , Trav [ Ex ] ] , Trav [ Rest ] ] , Trav [ E ] ] ,
                              when < < Id , Var_list , Ex > , as ... Rest > guard true do U [ U [ U [ < Id > , Var_list ] , Trav [ Ex ] ] , Trav [ Rest ] ]
                              otherwise 'Fehler in Trav`
                              end
                           to [ Expr ] ,
           F1_Fn [ Letr ] = ap case 
                               when < < Id , Var_list , Expr > , as ... Rest > guard true do UNITE [ < Id > , F1_Fn [ Rest ] ] ,
                               when < < Id , Expr > , as ... Rest > guard true do F1_Fn [ Rest ] ,
                               when <> guard true do <>
                               otherwise 'Fehler in F1_Fn`
                               end
                            to [ Letr ] ,
           G_h [ Letr , Fi ] = S [ F1_Fn [ Letr ] , Fi_ei [ Fi ] ] ,
           V1_Vn [ Letr ] = ap case 
                               when < < Id , Var_list , Expr > , as ... Rest > guard true do V1_Vn [ Rest ] ,
                               when < < Id , Expr > , as ... Rest > guard true do UNITE [ < Id > , V1_Vn [ Rest ] ] ,
                               when <> guard true do <>
                               otherwise 'Fehler in V1_Vn`
                               end
                            to [ Letr ] ,
           G1_Gn [ Funs , Fiei ] = S [ Funs , Fiei ] ,
           Lambda_lifter [ Prog ] = let Sol = Trav_down [ Prog , <> , <> , <> ]
                                    in let Closed_prog = Perform [ Prog , Sol ]
                                       in let Lifted_prog = Lift [ Closed_prog , <> ]
                                          in < SELECT [ 2 , Lifted_prog ] , SELECT [ 1 , Lifted_prog ] > ,
           Lift [ Prog , F_list ] = ap case 
                                       when KON{A} guard true do < KON{A} , F_list > ,
                                       when INT{I} guard true do < INT{I} , F_list > ,
                                       when BOOL{B} guard true do < BOOL{B} , F_list > ,
                                       when NIL{ } guard true do < NIL{ } , F_list > ,
                                       when FUN{F} guard true do < FUN{F} , F_list > ,
                                       when VAR{I} guard true do < VAR{I} , F_list > ,
                                       when PVAR{ } guard true do < PVAR{ } , F_list > ,
                                       when ADD{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                       in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                          in < ADD{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when SUB{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                       in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                          in < SUB{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when MULT{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                        in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                           in < MULT{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when DIV{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                       in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                          in < DIV{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when EQ{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                      in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                         in < EQ{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when NE{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                      in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                         in < NE{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when GT{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                      in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                         in < GT{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when GE{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                      in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                         in < GE{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when LT{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                      in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                         in < LT{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when LE{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                      in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                         in < LE{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when AND{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                       in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                          in < AND{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when OR{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                      in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                         in < OR{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when CONS{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                        in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                           in < CONS{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when CCONS{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                         in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                            in < CCONS{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when IF{E1 , E2 , E3} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                           in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                              in let E3_list = Lift [ E3 , SELECT [ 2 , E2_list ] ]
                                                                                 in < IF{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ] , SELECT [ 1 , E3_list ]} , SELECT [ 2 , E3_list ] > ,
                                       when AP{E1 , E2} guard true do let E1_list = Lift [ E1 , F_list ]
                                                                      in let E2_list = Lift [ E2 , SELECT [ 2 , E1_list ] ]
                                                                         in < AP{SELECT [ 1 , E1_list ] , SELECT [ 1 , E2_list ]} , SELECT [ 2 , E2_list ] > ,
                                       when NOT{E} guard true do let E_list = Lift [ E , F_list ]
                                                                 in < NOT{SELECT [ 1 , E_list ]} , SELECT [ 2 , E_list ] > ,
                                       when NEG{E} guard true do let E_list = Lift [ E , F_list ]
                                                                 in < NEG{SELECT [ 1 , E_list ]} , SELECT [ 2 , E_list ] > ,
                                       when HD{E} guard true do let E_list = Lift [ E , F_list ]
                                                                in < HD{SELECT [ 1 , E_list ]} , SELECT [ 2 , E_list ] > ,
                                       when TL{E} guard true do let E_list = Lift [ E , F_list ]
                                                                in < TL{SELECT [ 1 , E_list ]} , SELECT [ 2 , E_list ] > ,
                                       when NULL{E} guard true do let E_list = Lift [ E , F_list ]
                                                                  in < NULL{SELECT [ 1 , E_list ]} , SELECT [ 2 , E_list ] > ,
                                       when LET{L , Expr} guard true do let Let_list = Lift_letr [ L , F_list ]
                                                                        in let Lift_list = Lift [ Expr , SELECT [ 2 , Let_list ] ]
                                                                           in < LET{SELECT [ 1 , Let_list ] , SELECT [ 1 , Lift_list ]} , SELECT [ 2 , Lift_list ] > ,
                                       when LETREC{Letr , Expr} guard true do let Letr_list = Lift_letr [ Letr , F_list ]
                                                                              in let Lift_list = Lift [ Expr , SELECT [ 2 , Letr_list ] ]
                                                                                 in if empty( V1_Vn [ Letr ] )
                                                                                    then Lift_list
                                                                                    else < LETREC{SELECT [ 1 , Letr_list ] , SELECT [ 1 , Lift_list ]} , SELECT [ 2 , Lift_list ] >
                                       otherwise 'Fehler in Lift`
                                       end
                                    to [ Prog ] ,
           Lift_letr [ Letr , F_list ] = ap case 
                                            when < < Id , Var_list , Expr > , as ... Rest > guard true do ap case 
                                                                                                             when < E_neu , F_neu > guard true do Lift_letr [ Rest , UNITE [ F_neu , < F{Id , Var_list , E_neu} > ] ]
                                                                                                             otherwise 'Fehler in Lift_letr`
                                                                                                             end
                                                                                                          to [ Lift [ Expr , F_list ] ] ,
                                            when <> guard true do < <> , F_list > ,
                                            when < < Id , Expr > , as ... Rest > guard true do let L = Lift_letr [ Rest , F_list ]
                                                                                               in ap case 
                                                                                                     when < E_neu , F_neu > guard true do < UNITE [ < < Id , E_neu > > , SELECT [ 1 , L ] ] , F_neu >
                                                                                                     otherwise 'Fehler in Lift_letr`
                                                                                                     end
                                                                                                  to [ Lift [ Expr , F_list ] ]
                                            otherwise 'Fehler in Lift_letr`
                                            end
                                         to [ Letr ] ,
           Perform [ Prog , Sol ] = ap case 
                                       when KON{A} guard true do KON{A} ,
                                       when INT{I} guard true do INT{I} ,
                                       when BOOL{B} guard true do BOOL{B} ,
                                       when NIL{ } guard true do NIL{ } ,
                                       when FUN{F} guard true do Build_ap [ F , Such_fi_in_sol [ Prog , Sol ] ] ,
                                       when VAR{I} guard true do VAR{I} ,
                                       when PVAR{ } guard true do PVAR{ } ,
                                       when ADD{E1 , E2} guard true do ADD{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when SUB{E1 , E2} guard true do SUB{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when MULT{E1 , E2} guard true do MULT{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when DIV{E1 , E2} guard true do DIV{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when EQ{E1 , E2} guard true do EQ{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when NE{E1 , E2} guard true do NE{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when GT{E1 , E2} guard true do GT{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when GE{E1 , E2} guard true do GE{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when LT{E1 , E2} guard true do LT{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when LE{E1 , E2} guard true do LE{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when AND{E1 , E2} guard true do AND{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when OR{E1 , E2} guard true do OR{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when CONS{E1 , E2} guard true do CONS{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when CCONS{E1 , E2} guard true do CCONS{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when IF{E1 , E2 , E3} guard true do IF{Perform [ E1 , Sol ] , Perform [ E2 , Sol ] , Perform [ E3 , Sol ]} ,
                                       when AP{E1 , E2} guard true do AP{Perform [ E1 , Sol ] , Perform [ E2 , Sol ]} ,
                                       when NOT{E} guard true do NOT{Perform [ E , Sol ]} ,
                                       when NEG{E} guard true do NEG{Perform [ E , Sol ]} ,
                                       when HD{E} guard true do HD{Perform [ E , Sol ]} ,
                                       when TL{E} guard true do TL{Perform [ E , Sol ]} ,
                                       when NULL{E} guard true do NULL{Perform [ E , Sol ]} ,
                                       when LET{L , E} guard true do LET{Build_let [ L , Sol ] , Perform [ E , Sol ]} ,
                                       when LETREC{Letr , E} guard true do LETREC{Build_letr [ Letr , Sol ] , Perform [ E , Sol ]}
                                       otherwise 'Fehler in Perform`
                                       end
                                    to [ Prog ] ,
           Build_ap [ I , Liste ] = if empty( Liste )
                                    then FUN{I}
                                    else AP{Build_ap [ 1 , CUT [ ~1 , Liste ] ] , SELECT [ DIM [ Liste ] , Liste ]} ,
           Build_let [ Liste , Sol ] = ap case 
                                          when <> guard true do <> ,
                                          when < < Id , Expr > , as ... Rest > guard true do UNITE [ < < Id , Perform [ Expr , Sol ] > > , Build_let [ Rest , Sol ] ]
                                          otherwise 'Fehler in Build_let`
                                          end
                                       to [ Liste ] ,
           Build_letr [ Letr , Sol ] = ap case 
                                          when <> guard true do <> ,
                                          when < < Id , Expr > , as ... Rest > guard true do UNITE [ < < Id , Perform [ Expr , Sol ] > > , Build_letr [ Rest , Sol ] ] ,
                                          when < < Id , Var_list , Expr > , as ... Rest > guard true do UNITE [ < < Id , UNITE [ Such_fi_in_sol [ Id , Sol ] , Var_list ] , Perform [ Expr , Sol ] > > , Build_letr [ Rest , Sol ] ]
                                          otherwise 'Fehler in Build_letr`
                                          end
                                       to [ Letr ] ,
           Trav_down [ Prog , Vars , Funs , Sol ] = ap case 
                                                       when KON{A} guard true do Sol ,
                                                       when INT{I} guard true do Sol ,
                                                       when BOOL{B} guard true do Sol ,
                                                       when NIL{ } guard true do Sol ,
                                                       when FUN{F} guard true do Sol ,
                                                       when VAR{I} guard true do Sol ,
                                                       when PVAR{ } guard true do Sol ,
                                                       when ADD{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when SUB{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when MULT{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when DIV{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when EQ{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when NE{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when GT{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when GE{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when LT{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when LE{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when AND{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when OR{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when CONS{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when CCONS{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when IF{E1 , E2 , E3} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , U_sol [ Trav_down [ E2 , Vars , Funs , Sol ] , Trav_down [ E3 , Vars , Funs , Sol ] ] ] ,
                                                       when AP{E1 , E2} guard true do U_sol [ Trav_down [ E1 , Vars , Funs , Sol ] , Trav_down [ E2 , Vars , Funs , Sol ] ] ,
                                                       when NOT{E} guard true do Trav_down [ E , Vars , Funs , Sol ] ,
                                                       when NEG{E} guard true do Trav_down [ E , Vars , Funs , Sol ] ,
                                                       when HD{E} guard true do Trav_down [ E , Vars , Funs , Sol ] ,
                                                       when TL{E} guard true do Trav_down [ E , Vars , Funs , Sol ] ,
                                                       when NULL{E} guard true do Trav_down [ E , Vars , Funs , Sol ] ,
                                                       when LET{L , E} guard true do let Vars_neu = U [ Vars , V1_Vn [ L ] ]
                                                                                     in U_sol [ Letr_down [ L , Vars_neu , Funs , Sol ] , Trav_down [ E , Vars_neu , Funs , Sol ] ] ,
                                                       when LETREC{Letr , E} guard true do let Vars_neu = U [ Vars , V1_Vn [ Letr ] ] ,
                                                                                               Funs_neu = U [ Funs , F1_Fn [ Letr ] ] ,
                                                                                               Sol_neu = let Efi_list = Liste_aller_Efi_aus_letr [ Letr , Letr , Vars , Funs , Sol ]
                                                                                                         in Loese_GLS [ Efi_list , Efi_list , Sol ]
                                                                                           in U_sol [ Letr_down [ Letr , Vars_neu , Funs_neu , Sol_neu ] , Trav_down [ E , Vars_neu , Funs_neu , Sol_neu ] ]
                                                       otherwise 'Fehler in Trav_down`
                                                       end
                                                    to [ Prog ] ,
           Letr_down [ Letr , Vars , Funs , Sol ] = ap case 
                                                       when <> guard true do <> ,
                                                       when < < Id , Var_list , Expr > , as ... Rest > guard true do U_sol [ Trav_down [ Expr , Vars , Funs , Sol ] , Letr_down [ Rest , Vars , Funs , Sol ] ] ,
                                                       when < < Id , Expr > , as ... Rest > guard true do U_sol [ Trav_down [ Expr , Vars , Funs , Sol ] , Letr_down [ Rest , Vars , Funs , Sol ] ]
                                                       otherwise 'Fehler in Letr_down`
                                                       end
                                                    to [ Letr ] ,
           Loese_GLS [ Efi_list , Efi_list_2 , Sol ] = ap case 
                                                          when < E{F , Fr_var , Fr_fun} , as ... Rest > guard true do let Sol_neu = UNITE [ Sol , < < F , Close [ Fr_var , Fr_fun , Out_Efi [ F , Efi_list ] , Sol ] > > ]
                                                                                                                      in Loese_GLS [ Efi_list , Rest , Sol_neu ] ,
                                                          when <> guard true do Sol
                                                          otherwise 'Fehler in Loese_GLS `
                                                          end
                                                       to [ Efi_list_2 ] ,
           Out_Efi [ Element , Efi_list ] = ap case 
                                               when < FUN{I} , < E{FUN{J} , Fv , Ff} , as ... Rest > > guard true do if ( I eq J )
                                                                                                                     then Rest
                                                                                                                     else UNITE [ < E{FUN{J} , Fv , Ff} > , Out_Efi [ Element , Rest ] ] ,
                                               when < A , <> > guard true do <>
                                               otherwise 'Fehler in Out_Efi`
                                               end
                                            to [ < Element , Efi_list > ] ,
           Search_fi_sol [ F , Sol , X ] = ap case 
                                              when < FUN{I} , < < FUN{J} , A > , as ... Rest > > guard true do if ( I eq J )
                                                                                                               then X
                                                                                                               else Search_fi_sol [ F , Rest , ( X + 1 ) ] ,
                                              when < A , <> > guard true do 0
                                              otherwise 'Fehler in search_fi_sol`
                                              end
                                           to [ < F , Sol > ] ,
           Search_fi_Efi [ F , Efi_list , X ] = ap case 
                                                   when < FUN{I} , < E{FUN{J} , ...} , as ... Rest > > guard true do if ( I eq J )
                                                                                                                     then X
                                                                                                                     else Search_fi_Efi [ F , Rest , ( X + 1 ) ] ,
                                                   when < A , <> > guard true do 0
                                                   otherwise 'Fehler in search_fi_Efi`
                                                   end
                                                to [ < F , Efi_list > ] ,
           Close [ Fr_var , Fr_fun , Efi_list , Sol ] = ap case 
                                                           when < F , as ... Rest > guard true do let X = Search_fi_sol [ F , Sol , 1 ]
                                                                                                  in if ( X ne 0 )
                                                                                                     then Close [ U [ Fr_var , SELECT [ 2 , SELECT [ X , Sol ] ] ] , Rest , Efi_list , Sol ]
                                                                                                     else let X = Search_fi_Efi [ F , Efi_list , 1 ]
                                                                                                          in if ( X eq 0 )
                                                                                                             then Close [ Fr_var , Rest , Efi_list , Sol ]
                                                                                                             else ap case 
                                                                                                                     when E{Fx , Fr_var_x , Fr_fun_x} guard true do Close [ U [ Fr_var , Fr_var_x ] , U [ Rest , Fr_fun_x ] , Out_Efi [ Fx , Efi_list ] , Sol ]
                                                                                                                     end
                                                                                                                  to [ SELECT [ X , Efi_list ] ] ,
                                                           when <> guard true do Fr_var
                                                           otherwise 'Fehler in Close`
                                                           end
                                                        to [ Fr_fun ]
         in def 
              Comp [ Prog ] = let Predef_Fs = < < 2 , < PUSH{0} , EVAL{ } , GET{ } , PUSH{1} , EVAL{ } , GET{ } , ADD{ } , MKINT{ } , UPDATE{3} , RET{2} > > , < 2 , < PUSH{0} , EVAL{ } , GET{ } , PUSH{1} , EVAL{ } , GET{ } , SUB{ } , MKINT{ } , UPDATE{3} , RET{2} > > , < 2 , < PUSH{0} , EVAL{ } , GET{ } , PUSH{1} , EVAL{ } , GET{ } , MULT{ } , MKINT{ } , UPDATE{3} , RET{2} > > , < 2 , < PUSH{0} , EVAL{ } , GET{ } , PUSH{1} , EVAL{ } , GET{ } , DIV{ } , MKINT{ } , UPDATE{3} , RET{2} > > , < 1 , < PUSH{0} , EVAL{ } , GET{ } , NEG{ } , MKINT{ } , UPDATE{2} , RET{1} > > , < 2 , < PUSH{0} , EVAL{ } , GET{ } , PUSH{1} , EVAL{ } , GET{ } , LT{ } , MKBOOL{ } , UPDATE{3} , RET{2} > > , < 2 , < PUSH{0} , EVAL{ } , GET{ } , PUSH{1} , EVAL{ } , GET{ } , LE{ } , MKBOOL{ } , UPDATE{3} , RET{2} > > , < 2 , < PUSH{0} , EVAL{ } , GET{ } , PUSH{1} , EVAL{ } , GET{ } , EQ{ } , MKBOOL{ } , UPDATE{3} , RET{2} > > , < 2 , < PUSH{0} , EVAL{ } , GET{ } , PUSH{1} , EVAL{ } , GET{ } , NE{ } , MKBOOL{ } , UPDATE{3} , RET{2} > > , < 2 , < PUSH{0} , EVAL{ } , GET{ } , PUSH{1} , EVAL{ } , GET{ } , GE{ } , MKBOOL{ } , UPDATE{3} , RET{2} > > , < 2 , < PUSH{0} , EVAL{ } , GET{ } , PUSH{1} , EVAL{ } , GET{ } , GT{ } , MKBOOL{ } , UPDATE{3} , RET{2} > > , < 2 , < PUSH{0} , EVAL{ } , GET{ } , JFALSE{1} , PUSH{1} , EVAL{ } , JMP{2} , LABEL{1} , PUSHBOOL{false} , LABEL{2} , UPDATE{3} , RET{2} > > , < 2 , < PUSH{0} , EVAL{ } , GET{ } , JFALSE{1} , PUSHBOOL{true} , JMP{2} , LABEL{1} , PUSH{1} , EVAL{ } , LABEL{2} , UPDATE{3} , RET{2} > > , < 1 , < PUSH{0} , EVAL{ } , GET{ } , NOT{ } , MKBOOL{ } , UPDATE{2} , RET{1} > > , < 2 , < PUSH{0} , PUSH{1} , CONS{ } , UPDATE{3} , RET{2} > > , < 1 , < PUSH{0} , EVAL{ } , HD{ } , EVAL{ } , UPDATE{2} , RET{1} > > , < 1 , < PUSH{0} , EVAL{ } , TL{ } , EVAL{ } , UPDATE{2} , RET{1} > > , < 1 , < PUSH{0} , EVAL{ } , NULL{ } , MKBOOL{ } , UPDATE{2} , RET{1} > > , < 3 , < PUSH{0} , EVAL{ } , GET{ } , JFALSE{1} , PUSH{1} , EVAL{ } , JMP{2} , LABEL{1} , PUSH{2} , EVAL{ } , LABEL{2} , UPDATE{4} , RET{3} > > , < 1 , < PUSH{0} , EVAL{ } , TEST_CONS{ } , MKBOOL{ } , UPDATE{2} , RET{1} > > , < 1 , < PUSH{0} , EVAL{ } , TEST_CCONS{ } , MKBOOL{ } , UPDATE{2} , RET{1} > > , < 1 , < PUSH{0} , EVAL{ } , TEST_KON{ } , MKBOOL{ } , UPDATE{2} , RET{1} > > , < 1 , < PUSH{0} , EVAL{ } , TEST_INT{ } , MKBOOL{ } , UPDATE{2} , RET{1} > > , < 1 , < PUSH{0} , EVAL{ } , TEST_BOOL{ } , MKBOOL{ } , UPDATE{2} , RET{1} > > , < 1 , < PUSH{0} , EVAL{ } , TEST_VAR{ } , MKBOOL{ } , UPDATE{2} , RET{1} > > , < 1 , < PUSH{0} , EVAL{ } , CHD{ } , MKBOOL{ } , UPDATE{2} , RET{1} > > , < 1 , < PUSH{0} , EVAL{ } , CTL{ } , MKBOOL{ } , UPDATE{2} , RET{1} > > , < 1 , < PUSH{0} , EVAL{ } , NULL{ } , JFALSE{1} , PUSHINT{0} , JMP{2} , LABEL{1} , PUSHFUN{1} , PUSHINT{1} , MKAP{ } , PUSHFUN{28} , PUSHFUN{17} , PUSH{3} , MKAP{ } , MKAP{ } , MKAP{ } , EVAL{ } , LABEL{2} , UPDATE{2} , RET{1} > > , < 2 , < PUSH{0} , EVAL{ } , NULL{ } , JFALSE{1} , PUSH{1} , EVAL{ } , JMP{2} , LABEL{1} , PUSHFUN{16} , PUSH{1} , MKAP{ } , PUSHFUN{29} , PUSHFUN{17} , PUSH{3} , MKAP{ } , MKAP{ } , PUSH{3} , MKAP{ } , CONS{ } , LABEL{2} , UPDATE{3} , RET{2} > > , < 2 , < PUSHFUN{19} , PUSHFUN{11} , PUSH{2} , MKAP{ } , PUSHINT{0} , MKAP{ } , MKAP{ } , PUSHFUN{30} , PUSHFUN{2} , PUSH{3} , MKAP{ } , PUSHINT{1} , MKAP{ } , MKAP{ } , PUSHFUN{17} , PUSH{4} , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{19} , PUSHFUN{8} , PUSH{3} , MKAP{ } , PUSHINT{0} , MKAP{ } , MKAP{ } , PUSH{3} , MKAP{ } , PUSHFUN{31} , PUSH{4} , MKAP{ } , PUSHFUN{1} , PUSH{4} , MKAP{ } , PUSHFUN{28} , PUSH{6} , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , EVAL{ } , UPDATE{3} , RET{2} > > , < 2 , < PUSH{1} , EVAL{ } , GET{ } , PUSHBASIC{0} , EQ{ } , JFALSE{1} , PUSHNIL{ } , JMP{2} , LABEL{1} , PUSHFUN{16} , PUSH{1} , MKAP{ } , PUSHFUN{31} , PUSHFUN{17} , PUSH{3} , MKAP{ } , MKAP{ } , PUSHFUN{2} , PUSH{4} , MKAP{ } , PUSHINT{1} , MKAP{ } , MKAP{ } , CONS{ } , LABEL{2} , UPDATE{3} , RET{2} > > , < 2 , < PUSH{0} , EVAL{ } , GET{ } , PUSHBASIC{1} , GT{ } , JFALSE{1} , PUSHFUN{32} , PUSHFUN{2} , PUSH{2} , MKAP{ } , PUSHINT{1} , MKAP{ } , MKAP{ } , PUSHFUN{17} , PUSH{3} , MKAP{ } , MKAP{ } , EVAL{ } , JMP{2} , LABEL{1} , PUSHFUN{16} , PUSH{2} , MKAP{ } , EVAL{ } , LABEL{2} , UPDATE{3} , RET{2} > > , < 3 , < PUSH{0} , EVAL{ } , GET{ } , PUSHBASIC{1} , GT{ } , JFALSE{1} , PUSHFUN{16} , PUSH{3} , MKAP{ } , PUSHFUN{33} , PUSHFUN{2} , PUSH{3} , MKAP{ } , PUSHINT{1} , MKAP{ } , MKAP{ } , PUSH{3} , MKAP{ } , PUSHFUN{17} , PUSH{5} , MKAP{ } , MKAP{ } , CONS{ } , JMP{2} , LABEL{1} , PUSH{1} , PUSHFUN{17} , PUSH{4} , MKAP{ } , CONS{ } , LABEL{2} , UPDATE{4} , RET{3} > > , < 2 , < PUSHFUN{19} , PUSHFUN{18} , PUSHFUN{30} , PUSHINT{1} , MKAP{ } , PUSH{3} , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{32} , PUSHINT{1} , MKAP{ } , PUSH{2} , MKAP{ } , MKAP{ } , PUSHFUN{19} , PUSHFUN{35} , PUSHFUN{32} , PUSHINT{1} , MKAP{ } , PUSHFUN{32} , PUSHINT{1} , MKAP{ } , PUSH{5} , MKAP{ } , MKAP{ } , MKAP{ } , PUSH{4} , MKAP{ } , MKAP{ } , PUSHFUN{36} , PUSHFUN{32} , PUSHINT{1} , MKAP{ } , PUSHFUN{32} , PUSHINT{1} , MKAP{ } , PUSH{5} , MKAP{ } , MKAP{ } , MKAP{ } , PUSH{4} , MKAP{ } , PUSHFUN{19} , PUSHFUN{37} , PUSHFUN{32} , PUSHINT{2} , MKAP{ } , PUSHFUN{32} , PUSHINT{1} , MKAP{ } , PUSH{7} , MKAP{ } , MKAP{ } , MKAP{ } , PUSH{2} , MKAP{ } , MKAP{ } , PUSHFUN{37} , PUSHFUN{32} , PUSHINT{3} , MKAP{ } , PUSHFUN{32} , PUSHINT{1} , MKAP{ } , PUSH{7} , MKAP{ } , MKAP{ } , MKAP{ } , PUSH{2} , MKAP{ } , MKAP{ } , PUSHFUN{34} , PUSHFUN{30} , PUSHINT{1} , MKAP{ } , PUSH{6} , MKAP{ } , MKAP{ } , PUSH{6} , MKAP{ } , MKAP{ } , SLIDE{1} , MKAP{ } , PUSHFUN{34} , PUSHFUN{30} , PUSHINT{1} , MKAP{ } , PUSH{4} , MKAP{ } , MKAP{ } , PUSH{4} , MKAP{ } , MKAP{ } , MKAP{ } , EVAL{ } , UPDATE{3} , RET{2} > > , < 2 , < PUSHFUN{19} , PUSHFUN{12} , PUSHFUN{20} , PUSH{3} , MKAP{ } , MKAP{ } , PUSHFUN{20} , PUSH{4} , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{12} , PUSHFUN{35} , PUSHFUN{32} , PUSHINT{1} , MKAP{ } , PUSH{4} , MKAP{ } , MKAP{ } , PUSHFUN{32} , PUSHINT{1} , MKAP{ } , PUSH{5} , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{35} , PUSHFUN{30} , PUSHINT{1} , MKAP{ } , PUSH{4} , MKAP{ } , MKAP{ } , PUSHFUN{30} , PUSHINT{1} , MKAP{ } , PUSH{5} , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{19} , PUSHFUN{12} , PUSHFUN{21} , PUSH{4} , MKAP{ } , MKAP{ } , PUSHFUN{21} , PUSH{5} , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{12} , PUSHFUN{35} , PUSHFUN{26} , PUSH{5} , MKAP{ } , MKAP{ } , PUSHFUN{26} , PUSH{6} , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{35} , PUSHFUN{27} , PUSH{5} , MKAP{ } , MKAP{ } , PUSHFUN{27} , PUSH{6} , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{19} , PUSHFUN{12} , PUSHFUN{38} , PUSH{5} , MKAP{ } , MKAP{ } , PUSHFUN{38} , PUSH{6} , MKAP{ } , MKAP{ } , MKAP{ } , PUSHBOOL{true} , MKAP{ } , PUSHFUN{19} , PUSHFUN{12} , PUSHFUN{23} , PUSH{6} , MKAP{ } , MKAP{ } , PUSHFUN{23} , PUSH{7} , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{8} , PUSH{5} , MKAP{ } , PUSH{6} , MKAP{ } , MKAP{ } , PUSHFUN{19} , PUSHFUN{12} , PUSHFUN{24} , PUSH{7} , MKAP{ } , MKAP{ } , PUSHFUN{24} , PUSH{8} , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{8} , PUSH{6} , MKAP{ } , PUSH{7} , MKAP{ } , MKAP{ } , PUSHFUN{19} , PUSHFUN{12} , PUSHFUN{22} , PUSH{8} , MKAP{ } , MKAP{ } , PUSHFUN{22} , PUSH{9} , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{8} , PUSH{7} , MKAP{ } , PUSH{8} , MKAP{ } , MKAP{ } , PUSHFUN{19} , PUSHFUN{25} , PUSH{8} , MKAP{ } , MKAP{ } , PUSHBOOL{true} , MKAP{ } , PUSHBOOL{false} , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , EVAL{ } , UPDATE{3} , RET{2} > > , < 2 , < PUSHFUN{19} , PUSHFUN{20} , PUSH{2} , MKAP{ } , MKAP{ } , PUSHFUN{29} , PUSHFUN{36} , PUSHFUN{32} , PUSHINT{1} , MKAP{ } , PUSH{4} , MKAP{ } , MKAP{ } , PUSHFUN{32} , PUSHINT{1} , MKAP{ } , PUSH{5} , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{36} , PUSHFUN{30} , PUSHINT{1} , MKAP{ } , PUSH{4} , MKAP{ } , MKAP{ } , PUSHFUN{30} , PUSHINT{1} , MKAP{ } , PUSH{5} , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{19} , PUSHFUN{21} , PUSH{3} , MKAP{ } , MKAP{ } , PUSHFUN{29} , PUSHFUN{36} , PUSHFUN{26} , PUSH{5} , MKAP{ } , MKAP{ } , PUSHFUN{26} , PUSH{6} , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{36} , PUSHFUN{27} , PUSH{5} , MKAP{ } , MKAP{ } , PUSHFUN{27} , PUSH{6} , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{19} , PUSHFUN{23} , PUSH{4} , MKAP{ } , MKAP{ } , PUSHNIL{ } , MKAP{ } , PUSHFUN{19} , PUSHFUN{24} , PUSH{5} , MKAP{ } , MKAP{ } , PUSHNIL{ } , MKAP{ } , PUSHFUN{19} , PUSHFUN{22} , PUSH{6} , MKAP{ } , MKAP{ } , PUSHNIL{ } , MKAP{ } , PUSHFUN{19} , PUSHFUN{25} , PUSH{7} , MKAP{ } , MKAP{ } , PUSH{7} , PUSHNIL{ } , CONS{ } , MKAP{ } , PUSHFUN{19} , PUSHFUN{38} , PUSH{8} , MKAP{ } , MKAP{ } , PUSHNIL{ } , MKAP{ } , PUSHKON{'fehler in struct_bind`} , PUSHNIL{ } , CCONS{ } , PUSHNIL{ } , CONS{ } , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , MKAP{ } , EVAL{ } , UPDATE{3} , RET{2} > > , < 2 , < PUSHFUN{19} , PUSHFUN{18} , PUSH{3} , MKAP{ } , MKAP{ } , PUSH{1} , MKAP{ } , PUSHFUN{37} , PUSH{2} , PUSHFUN{32} , PUSHINT{1} , MKAP{ } , PUSH{5} , MKAP{ } , MKAP{ } , MKAP{ } , PUSHFUN{30} , PUSHINT{1} , MKAP{ } , PUSH{4} , MKAP{ } , MKAP{ } , MKAP{ } , EVAL{ } , UPDATE{3} , RET{2} > > , < 1 , < PUSH{0} , EVAL{ } , TEST_NULL{ } , MKBOOL{ } , UPDATE{2} , RET{1} > > > ,
                                  F_list = SELECT [ 1 , Prog ]
                              in < <> , E_can [ SELECT [ 2 , Prog ] ] , <> , <> , <> , UNITE [ Predef_Fs , F_can [ Sort [ F_list , Build_dummy_list [ F_list ] ] ] ] , <> > ,
              Sort [ F_list , Sorted_list ] = ap case 
                                                 when < F{FUN{I} , Var_list , Expr} , as ... Rest > guard true do Sort [ Rest , REPLACE [ ( I - 38 ) , F{FUN{I} , Var_list , Expr} , Sorted_list ] ] ,
                                                 when <> guard true do Sorted_list
                                                 otherwise 'Fehler in sort`
                                                 end
                                              to [ F_list ] ,
              Build_dummy_list [ F_list ] = if ( DIM [ F_list ] gt 0 )
                                            then UNITE [ < <> > , Build_dummy_list [ CUT [ 1 , F_list ] ] ]
                                            else <> ,
              E_can [ Expr ] = UNITE [ E_c [ Expr , <> , 1 , 0 ] , < PRINT{ } > ] ,
              F_can [ Liste ] = if empty( Liste )
                                then <>
                                else ap case 
                                        when F{FUN{I} , Var_list , Expr} guard true do UNITE [ < < DIM [ Var_list ] , F_c [ SELECT [ 1 , Liste ] ] > > , F_can [ CUT [ 1 , Liste ] ] ]
                                        otherwise 'Fehler in F_can `
                                        end
                                     to [ SELECT [ 1 , Liste ] ] ,
              F_c [ Funktion ] = ap case 
                                    when F{FUN{I} , Var_list , Expr} guard true do UNITE [ E_c [ Expr , Make_r [ Var_list ] , ( DIM [ Var_list ] + 1 ) , 1 ] , < UPDATE{( DIM [ Var_list ] + 1 )} , RET{DIM [ Var_list ]} > ]
                                    otherwise 'Fehler in F_c`
                                    end
                                 to [ Funktion ] ,
              Make_r [ Liste ] = if empty( Liste )
                                 then <>
                                 else UNITE [ < < SELECT [ 1 , Liste ] , ( DIM [ Liste ] + 1 ) > > , Make_r [ CUT [ 1 , Liste ] ] ] ,
              Ev_4 [ I , R ] = ap case 
                                  when < < VAR{X} , M > , as ... Rest > guard ( X eq I ) do M ,
                                  when < X , as ... Rest > guard true do Ev_4 [ I , Rest ]
                                  otherwise 'Fehler in Ev_4`
                                  end
                               to [ R ] ,
              E_c [ Expr , R , N , L ] = ap case 
                                            when INT{I} guard true do < PUSHINT{I} > ,
                                            when PVAR{ } guard true do < PUSHPVAR{ } > ,
                                            when BOOL{B} guard true do < PUSHBOOL{B} > ,
                                            when NIL{ } guard true do < PUSHNIL{ } > ,
                                            when VAR{X} guard true do < PUSH{( N - Ev_4 [ X , R ] )} , EVAL{ } > ,
                                            when FUN{F} guard true do < PUSHFUN{F} , EVAL{ } > ,
                                            when ADD{E1 , E2} guard true do UNITE [ B_c [ ADD{E1 , E2} , R , N , L ] , < MKINT{ } > ] ,
                                            when SUB{E1 , E2} guard true do UNITE [ B_c [ SUB{E1 , E2} , R , N , L ] , < MKINT{ } > ] ,
                                            when MULT{E1 , E2} guard true do UNITE [ B_c [ MULT{E1 , E2} , R , N , L ] , < MKINT{ } > ] ,
                                            when DIV{E1 , E2} guard true do UNITE [ B_c [ DIV{E1 , E2} , R , N , L ] , < MKINT{ } > ] ,
                                            when NEG{E} guard true do UNITE [ B_c [ NEG{E} , R , N , L ] , < MKINT{ } > ] ,
                                            when EQ{E1 , E2} guard true do UNITE [ B_c [ EQ{E1 , E2} , R , N , L ] , < MKBOOL{ } > ] ,
                                            when LT{E1 , E2} guard true do UNITE [ B_c [ LT{E1 , E2} , R , N , L ] , < MKBOOL{ } > ] ,
                                            when GT{E1 , E2} guard true do UNITE [ B_c [ GT{E1 , E2} , R , N , L ] , < MKBOOL{ } > ] ,
                                            when NE{E1 , E2} guard true do UNITE [ B_c [ NE{E1 , E2} , R , N , L ] , < MKBOOL{ } > ] ,
                                            when GE{E1 , E2} guard true do UNITE [ B_c [ GE{E1 , E2} , R , N , L ] , < MKBOOL{ } > ] ,
                                            when LE{E1 , E2} guard true do UNITE [ B_c [ LE{E1 , E2} , R , N , L ] , < MKBOOL{ } > ] ,
                                            when NOT{E} guard true do UNITE [ B_c [ NOT{E} , R , N , L ] , < MKBOOL{ } > ] ,
                                            when AND{E1 , E2} guard true do E_c [ IF{E1 , E2 , BOOL{false}} , R , N , L ] ,
                                            when OR{E1 , E2} guard true do E_c [ IF{E1 , BOOL{true} , E2} , R , N , L ] ,
                                            when CONS{E1 , E2} guard true do UNITE [ C_c [ E1 , R , N , L ] , UNITE [ C_c [ E2 , R , ( N + 1 ) , L ] , < CONS{ } > ] ] ,
                                            when CCONS{E1 , E2} guard true do UNITE [ C_c [ E1 , R , N , L ] , UNITE [ C_c [ E2 , R , ( N + 1 ) , L ] , < CCONS{ } > ] ] ,
                                            when NULL{E} guard true do UNITE [ E_c [ E , R , N , L ] , < NULL{ } , MKBOOL{ } > ] ,
                                            when HD{E} guard true do UNITE [ E_c [ E , R , N , L ] , < HD{ } , EVAL{ } > ] ,
                                            when TL{E} guard true do UNITE [ E_c [ E , R , N , L ] , < TL{ } , EVAL{ } > ] ,
                                            when IF{E1 , E2 , E3} guard true do UNITE [ B_c [ E1 , R , N , ( L + 2 ) ] , UNITE [ < JFALSE{L} > , UNITE [ E_c [ E2 , R , N , ( L + 2 ) ] , UNITE [ < JMP{( L + 1 )} , LABEL{L} > , UNITE [ E_c [ E3 , R , N , ( L + 2 ) ] , < LABEL{( L + 1 )} > ] ] ] ] ] ,
                                            when LET{D , E} guard true do UNITE [ C_let [ D , R , N , L ] , UNITE [ E_c [ E , SELECT [ 1 , Xr [ D , R , N ] ] , SELECT [ 2 , Xr [ D , R , N ] ] , L ] , < SLIDE{( SELECT [ 2 , Xr [ D , R , N ] ] - N )} > ] ] ,
                                            when LETREC{D , E} guard true do UNITE [ C_letrec [ D , SELECT [ 1 , Xr [ D , R , N ] ] , SELECT [ 2 , Xr [ D , R , N ] ] , L , 1 ] , UNITE [ E_c [ E , SELECT [ 1 , Xr [ D , R , N ] ] , SELECT [ 2 , Xr [ D , R , N ] ] , L ] , < SLIDE{( SELECT [ 2 , Xr [ D , R , N ] ] - N )} > ] ] ,
                                            when E guard true do UNITE [ C_c [ E , R , N , L ] , < EVAL{ } > ]
                                            otherwise 'Fehler in E_c`
                                            end
                                         to [ Expr ] ,
              B_c [ Expr , R , N , L ] = ap case 
                                            when INT{I} guard true do < PUSHBASIC{I} > ,
                                            when PVAR{ } guard true do < PUSHPVAR{ } > ,
                                            when BOOL{B} guard true do < PUSHBASIC{B} > ,
                                            when ADD{E1 , E2} guard true do UNITE [ B_c [ E1 , R , N , L ] , UNITE [ B_c [ E2 , R , N , L ] , < ADD{ } > ] ] ,
                                            when SUB{E1 , E2} guard true do UNITE [ B_c [ E1 , R , N , L ] , UNITE [ B_c [ E2 , R , N , L ] , < SUB{ } > ] ] ,
                                            when MULT{E1 , E2} guard true do UNITE [ B_c [ E1 , R , N , L ] , UNITE [ B_c [ E2 , R , N , L ] , < MULT{ } > ] ] ,
                                            when DIV{E1 , E2} guard true do UNITE [ B_c [ E1 , R , N , L ] , UNITE [ B_c [ E2 , R , N , L ] , < DIV{ } > ] ] ,
                                            when EQ{E1 , E2} guard true do UNITE [ B_c [ E1 , R , N , L ] , UNITE [ B_c [ E2 , R , N , L ] , < EQ{ } > ] ] ,
                                            when NE{E1 , E2} guard true do UNITE [ B_c [ E1 , R , N , L ] , UNITE [ B_c [ E2 , R , N , L ] , < NE{ } > ] ] ,
                                            when GT{E1 , E2} guard true do UNITE [ B_c [ E1 , R , N , L ] , UNITE [ B_c [ E2 , R , N , L ] , < GT{ } > ] ] ,
                                            when GE{E1 , E2} guard true do UNITE [ B_c [ E1 , R , N , L ] , UNITE [ B_c [ E2 , R , N , L ] , < GE{ } > ] ] ,
                                            when LT{E1 , E2} guard true do UNITE [ B_c [ E1 , R , N , L ] , UNITE [ B_c [ E2 , R , N , L ] , < LT{ } > ] ] ,
                                            when LE{E1 , E2} guard true do UNITE [ B_c [ E1 , R , N , L ] , UNITE [ B_c [ E2 , R , N , L ] , < LE{ } > ] ] ,
                                            when NEG{E} guard true do UNITE [ B_c [ E , R , N , L ] , < NEG{ } > ] ,
                                            when NOT{E} guard true do UNITE [ B_c [ E , R , N , L ] , < NOT{ } > ] ,
                                            when NULL{E} guard true do UNITE [ E_c [ E , R , N , L ] , < NULL{ } > ] ,
                                            when IF{E1 , E2 , E3} guard true do UNITE [ B_c [ E1 , R , N , ( L + 2 ) ] , UNITE [ < JFALSE{L} > , UNITE [ B_c [ E2 , R , N , ( L + 2 ) ] , UNITE [ < JMP{( L + 1 )} , LABEL{L} > , UNITE [ B_c [ E3 , R , N , ( L + 2 ) ] , < LABEL{( L + 1 )} > ] ] ] ] ] ,
                                            when LET{D , E} guard true do UNITE [ C_let [ D , R , N , L ] , UNITE [ B_c [ E , SELECT [ 1 , Xr [ D , R , N ] ] , SELECT [ 2 , Xr [ D , R , N ] ] , L ] , < POP{( SELECT [ 2 , Xr [ D , R , N ] ] - N )} > ] ] ,
                                            when LETREC{D , E} guard true do UNITE [ C_letrec [ D , SELECT [ 1 , Xr [ D , R , N ] ] , SELECT [ 2 , Xr [ D , R , N ] ] , L , 1 ] , UNITE [ B_c [ E , SELECT [ 1 , Xr [ D , R , N ] ] , SELECT [ 2 , Xr [ D , R , N ] ] , L ] , < POP{( SELECT [ 2 , Xr [ D , R , N ] ] - N )} > ] ] ,
                                            when E guard true do UNITE [ E_c [ E , R , N , L ] , < GET{ } > ]
                                            otherwise 'Fehler in B_c`
                                            end
                                         to [ Expr ] ,
              C_c [ Expr , R , N , L ] = ap case 
                                            when INT{I} guard true do < PUSHINT{I} > ,
                                            when PVAR{ } guard true do < PUSHPVAR{ } > ,
                                            when BOOL{B} guard true do < PUSHBOOL{B} > ,
                                            when NIL{ } guard true do < PUSHNIL{ } > ,
                                            when FUN{F} guard true do < PUSHFUN{F} > ,
                                            when KON{A} guard true do < PUSHKON{A} > ,
                                            when VAR{I} guard true do < PUSH{( N - Ev_4 [ I , R ] )} > ,
                                            when CONS{E1 , E2} guard true do UNITE [ C_c [ E1 , R , N , L ] , UNITE [ C_c [ E2 , R , ( N + 1 ) , L ] , < CONS{ } > ] ] ,
                                            when CCONS{E1 , E2} guard true do UNITE [ C_c [ E1 , R , N , L ] , UNITE [ C_c [ E2 , R , ( N + 1 ) , L ] , < CCONS{ } > ] ] ,
                                            when AP{E1 , E2} guard true do UNITE [ C_c [ E1 , R , N , L ] , UNITE [ C_c [ E2 , R , ( N + 1 ) , L ] , < MKAP{ } > ] ] ,
                                            when LET{D , E} guard true do UNITE [ C_let [ D , R , N , L ] , UNITE [ C_c [ E , SELECT [ 1 , Xr [ D , R , N ] ] , SELECT [ 2 , Xr [ D , R , N ] ] , L ] , < SLIDE{( SELECT [ 2 , Xr [ D , R , N ] ] - N )} > ] ] ,
                                            when LETREC{D , E} guard true do UNITE [ C_letrec [ D , SELECT [ 1 , Xr [ D , R , N ] ] , SELECT [ 2 , Xr [ D , R , N ] ] , L , 1 ] , UNITE [ C_c [ E , SELECT [ 1 , Xr [ D , R , N ] ] , SELECT [ 2 , Xr [ D , R , N ] ] , L ] , < SLIDE{( SELECT [ 2 , Xr [ D , R , N ] ] - N )} > ] ]
                                            otherwise 'Fehler in C_c`
                                            end
                                         to [ Expr ] ,
              Xr [ D , R , N ] = if empty( D )
                                 then < R , N >
                                 else Xr [ CUT [ 1 , D ] , UNITE [ R , < < SELECT [ 1 , SELECT [ 1 , D ] ] , ( N + 1 ) > > ] , ( N + 1 ) ] ,
              C_let [ D , R , N , L ] = if empty( D )
                                        then <>
                                        else UNITE [ C_c [ SELECT [ 2 , SELECT [ 1 , D ] ] , R , N , L ] , C_let [ CUT [ 1 , D ] , R , ( N + 1 ) , L ] ] ,
              C_letrec [ D , R , N , L , M ] = if empty( D )
                                               then < ALLOC{( M - 1 )} >
                                               else UNITE [ C_letrec [ CUT [ ~1 , D ] , R , N , L , ( M + 1 ) ] , UNITE [ C_c [ SELECT [ 2 , SELECT [ DIM [ D ] , D ] ] , R , ( N + M ) , L ] , < UPDATE{M} > ] ]
            in def 
                 G_m [ Tupel ] = ap case 
                                    when < O , <> , <> , <> , G , E , <> > guard true do O ,
                                    when < A , false > guard true do < 'Fehler!` , A > ,
                                    when < O , C , S , V , G , E , D > guard ( DIM [ O ] ge 10 ) do Tupel ,
                                    when < O , C , S , V , G , E , D > guard true do G_m [ G_mashine [ Tupel ] ]
                                    otherwise < 'irgendetwas falsch in g_m :` , Tupel >
                                    end
                                 to [ Tupel ] ,
                 Step [ Tupel , N ] = ap case 
                                         when < O , <> , <> , <> , G , E , <> > guard true do < O , N > ,
                                         when < A , false > guard true do < 'Fehler!` , N , A > ,
                                         when < O , C , S , V , G , E , D > guard true do if ( N gt 0 )
                                                                                          then Step [ G_mashine [ Tupel ] , ( N - 1 ) ]
                                                                                          else < N , Tupel >
                                         otherwise < 'irgendetwas falsch in g_m :` , N , Tupel >
                                         end
                                      to [ Tupel ] ,
                 Regel_8 [ K , S , G , Z ] = if ( Z eq ( K + 2 ) )
                                             then < true , S >
                                             else let N = SELECT [ Z , S ]
                                                  in ap case 
                                                        when AP{N1 , N2} guard true do Regel_8 [ K , REPLACE [ ( Z - 1 ) , N2 , S ] , G , ( Z + 1 ) ]
                                                        otherwise < false >
                                                        end
                                                     to [ SELECT [ N , G ] ] ,
                 Regel_21 [ S , G , M ] = if ( M eq 0 )
                                          then < S , G >
                                          else Regel_21 [ UNITE [ < ( DIM [ G ] + 1 ) > , S ] , UNITE [ G , < HOLE{ } > ] , ( M - 1 ) ] ,
                 Regel_32 [ L , C ] = ap case 
                                         when < LABEL{K} , as ... Rest > guard true do if ( L eq K )
                                                                                       then Rest
                                                                                       else Regel_32 [ L , Rest ]
                                         otherwise Regel_32 [ L , CUT [ 1 , C ] ]
                                         end
                                      to [ C ] ,
                 G_mashine [ Tupel ] = ap case 
                                          when < O , < PRINT{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                   when INT{I} guard true do < UNITE [ O , < I > ] , C , S , V , G , E , D > ,
                                                                                                                                   when BOOL{B} guard true do < UNITE [ O , < B > ] , C , S , V , G , E , D > ,
                                                                                                                                   when CONS{N1 , N2} guard true do < O , UNITE [ < EVAL{ } , PRINT{ } , EVAL{ } , PRINT{ } > , C ] , UNITE [ < N1 , N2 > , S ] , V , G , E , D > ,
                                                                                                                                   when CCONS{N1 , N2} guard true do ap case 
                                                                                                                                                                        when KON{A} guard true do < UNITE [ O , < A > ] , UNITE [ < EVAL{ } , PRINT{ } > , C ] , UNITE [ < N2 > , S ] , V , G , E , D >
                                                                                                                                                                        otherwise < O , UNITE [ < EVAL{ } , PRINT{ } , EVAL{ } , PRINT{ } > , C ] , UNITE [ < N1 , N2 > , S ] , V , G , E , D >
                                                                                                                                                                        end
                                                                                                                                                                     to [ SELECT [ N1 , G ] ] ,
                                                                                                                                   when NIL{ } guard true do < O , C , S , V , G , E , D >
                                                                                                                                   end
                                                                                                                                to [ SELECT [ N , G ] ] ,
                                          when < O , < EVAL{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                  when AP{N1 , N2} guard true do < O , < UNWIND{ } > , < N > , V , G , E , < C , S , D > > ,
                                                                                                                                  when FUN{F} guard true do < O , < UNWIND{ } > , < N > , V , G , E , < C , S , D > > ,
                                                                                                                                  when INT{I} guard true do < O , C , UNITE [ < N > , S ] , V , G , E , D > ,
                                                                                                                                  when KON{A} guard true do < O , C , UNITE [ < N > , S ] , V , G , E , D > ,
                                                                                                                                  when PVAR{ } guard true do < O , C , UNITE [ < N > , S ] , V , G , E , D > ,
                                                                                                                                  when BOOL{B} guard true do < O , C , UNITE [ < N > , S ] , V , G , E , D > ,
                                                                                                                                  when NIL{ } guard true do < O , C , UNITE [ < N > , S ] , V , G , E , D > ,
                                                                                                                                  when CONS{N1 , N2} guard true do < O , C , UNITE [ < N > , S ] , V , G , E , D > ,
                                                                                                                                  when CCONS{N1 , N2} guard true do < O , C , UNITE [ < N > , S ] , V , G , E , D >
                                                                                                                                  end
                                                                                                                               to [ SELECT [ N , G ] ] ,
                                          when < O , < UNWIND{ } > , < N0 , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                          when AP{N1 , N2} guard true do < O , < UNWIND{ } > , UNITE [ < N1 , N0 > , S ] , V , G , E , D > ,
                                                                                                                          when FUN{F} guard true do ap case 
                                                                                                                                                       when < K , C > guard true do if ( DIM [ S ] ge K )
                                                                                                                                                                                    then if SELECT [ 1 , Regel_8 [ K , UNITE [ < N0 > , S ] , G , 2 ] ]
                                                                                                                                                                                         then < O , C , SELECT [ 2 , Regel_8 [ K , UNITE [ < N0 > , S ] , G , 2 ] ] , V , G , E , D >
                                                                                                                                                                                         else ap case 
                                                                                                                                                                                                 when < C_strich , S_strich , D_strich > guard ( DIM [ S ] lt K ) do < O , C_strich , UNITE [ < SELECT [ DIM [ S ] , S ] > , S_strich ] , V , G , E , D_strich >
                                                                                                                                                                                                 end
                                                                                                                                                                                              to [ if empty( D )
                                                                                                                                                                                                   then < <> , <> , <> >
                                                                                                                                                                                                   else < SELECT [ 1 , D ] , SELECT [ 2 , D ] , SELECT [ 3 , D ] > ]
                                                                                                                                                                                    else ap case 
                                                                                                                                                                                            when < C_strich , S_strich , D_strich > guard ( DIM [ S ] lt K ) do if ( DIM [ S ] ne 0 )
                                                                                                                                                                                                                                                                then < O , C_strich , UNITE [ < SELECT [ DIM [ S ] , S ] > , S_strich ] , V , G , E , D_strich >
                                                                                                                                                                                                                                                                else < O , C_strich , UNITE [ < N0 > , S_strich ] , V , G , E , D_strich >
                                                                                                                                                                                            end
                                                                                                                                                                                         to [ if empty( D )
                                                                                                                                                                                              then < <> , <> , <> >
                                                                                                                                                                                              else < SELECT [ 1 , D ] , SELECT [ 2 , D ] , SELECT [ 3 , D ] > ]
                                                                                                                                                       end
                                                                                                                                                    to [ SELECT [ F , E ] ]
                                                                                                                          end
                                                                                                                       to [ SELECT [ N0 , G ] ] ,
                                          when < O , < RET{M} , as ... C > , S , V , G , E , D_list > guard true do if ( DIM [ S ] eq ( M + 1 ) )
                                                                                                                    then ap case 
                                                                                                                            when < C_strich , S_strich , D > guard true do ap case 
                                                                                                                                                                              when INT{I} guard true do < O , C_strich , UNITE [ < SELECT [ DIM [ S ] , S ] > , S_strich ] , V , G , E , D > ,
                                                                                                                                                                              when KON{A} guard true do < O , C_strich , UNITE [ < SELECT [ DIM [ S ] , S ] > , S_strich ] , V , G , E , D > ,
                                                                                                                                                                              when PVAR{ } guard true do < O , C_strich , UNITE [ < SELECT [ DIM [ S ] , S ] > , S_strich ] , V , G , E , D > ,
                                                                                                                                                                              when BOOL{B} guard true do < O , C_strich , UNITE [ < SELECT [ DIM [ S ] , S ] > , S_strich ] , V , G , E , D > ,
                                                                                                                                                                              when NIL{ } guard true do < O , C_strich , UNITE [ < SELECT [ DIM [ S ] , S ] > , S_strich ] , V , G , E , D > ,
                                                                                                                                                                              when CONS{N1 , N2} guard true do < O , C_strich , UNITE [ < SELECT [ DIM [ S ] , S ] > , S_strich ] , V , G , E , D > ,
                                                                                                                                                                              when CCONS{N1 , N2} guard true do < O , C_strich , UNITE [ < SELECT [ DIM [ S ] , S ] > , S_strich ] , V , G , E , D > ,
                                                                                                                                                                              when AP{N1 , N2} guard true do < O , < UNWIND{ } > , CUT [ M , S ] , V , G , E , D_list > ,
                                                                                                                                                                              when FUN{F} guard true do < O , < UNWIND{ } > , CUT [ M , S ] , V , G , E , D_list >
                                                                                                                                                                              end
                                                                                                                                                                           to [ SELECT [ SELECT [ DIM [ S ] , S ] , G ] ]
                                                                                                                            end
                                                                                                                         to [ if empty( D_list )
                                                                                                                              then < <> , <> , <> >
                                                                                                                              else < SELECT [ 1 , D_list ] , SELECT [ 2 , D_list ] , SELECT [ 3 , D_list ] > ]
                                                                                                                    else if ( DIM [ S ] ge ( M + 1 ) )
                                                                                                                         then ap case 
                                                                                                                                 when AP{N1 , N2} guard true do < O , < UNWIND{ } > , CUT [ M , S ] , V , G , E , D_list > ,
                                                                                                                                 when FUN{F} guard true do < O , < UNWIND{ } > , CUT [ M , S ] , V , G , E , D_list >
                                                                                                                                 end
                                                                                                                              to [ SELECT [ SELECT [ ( M + 1 ) , S ] , G ] ]
                                                                                                                         else false ,
                                          when < O , < PUSHINT{I} , as ... C > , S , V , G , E , D > guard true do < O , C , UNITE [ < ( DIM [ G ] + 1 ) > , S ] , V , UNITE [ G , < INT{I} > ] , E , D > ,
                                          when < O , < PUSHKON{A} , as ... C > , S , V , G , E , D > guard true do < O , C , UNITE [ < ( DIM [ G ] + 1 ) > , S ] , V , UNITE [ G , < KON{A} > ] , E , D > ,
                                          when < O , < PUSHPVAR{ } , as ... C > , S , V , G , E , D > guard true do < O , C , UNITE [ < ( DIM [ G ] + 1 ) > , S ] , V , UNITE [ G , < PVAR{ } > ] , E , D > ,
                                          when < O , < PUSHBOOL{B} , as ... C > , S , V , G , E , D > guard true do < O , C , UNITE [ < ( DIM [ G ] + 1 ) > , S ] , V , UNITE [ G , < BOOL{B} > ] , E , D > ,
                                          when < O , < PUSHNIL{ } , as ... C > , S , V , G , E , D > guard true do < O , C , UNITE [ < ( DIM [ G ] + 1 ) > , S ] , V , UNITE [ G , < NIL{ } > ] , E , D > ,
                                          when < O , < PUSHFUN{F} , as ... C > , S , V , G , E , D > guard true do < O , C , UNITE [ < ( DIM [ G ] + 1 ) > , S ] , V , UNITE [ G , < FUN{F} > ] , E , D > ,
                                          when < O , < PUSH{M} , as ... C > , S , V , G , E , D > guard true do if ( DIM [ S ] ge ( M + 1 ) )
                                                                                                                then < O , C , UNITE [ < SELECT [ ( M + 1 ) , S ] > , S ] , V , G , E , D >
                                                                                                                else false ,
                                          when < O , < MKINT{ } , as ... C > , S , < I , as ... V > , G , E , D > guard true do < O , C , UNITE [ < ( DIM [ G ] + 1 ) > , S ] , V , UNITE [ G , < INT{I} > ] , E , D > ,
                                          when < O , < MKBOOL{ } , as ... C > , S , < B , as ... V > , G , E , D > guard true do < O , C , UNITE [ < ( DIM [ G ] + 1 ) > , S ] , V , UNITE [ G , < BOOL{B} > ] , E , D > ,
                                          when < O , < MKAP{ } , as ... C > , < N1 , N2 , as ... S > , V , G , E , D > guard true do < O , C , UNITE [ < ( DIM [ G ] + 1 ) > , S ] , V , UNITE [ G , < AP{N2 , N1} > ] , E , D > ,
                                          when < O , < CONS{ } , as ... C > , < N1 , N2 , as ... S > , V , G , E , D > guard true do < O , C , UNITE [ < ( DIM [ G ] + 1 ) > , S ] , V , UNITE [ G , < CONS{N2 , N1} > ] , E , D > ,
                                          when < O , < CCONS{ } , as ... C > , < N1 , N2 , as ... S > , V , G , E , D > guard true do < O , C , UNITE [ < ( DIM [ G ] + 1 ) > , S ] , V , UNITE [ G , < CCONS{N2 , N1} > ] , E , D > ,
                                          when < O , < ALLOC{M} , as ... C > , S , V , G , E , D > guard true do let Liste_S_G = Regel_21 [ S , G , M ]
                                                                                                                 in < O , C , SELECT [ 1 , Liste_S_G ] , V , SELECT [ 2 , Liste_S_G ] , E , D > ,
                                          when < O , < UPDATE{M} , as ... C > , S , V , G , E , D > guard ( DIM [ S ] ge ( M + 1 ) ) do < O , C , CUT [ 1 , S ] , V , REPLACE [ SELECT [ ( M + 1 ) , S ] , SELECT [ SELECT [ 1 , S ] , G ] , G ] , E , D > ,
                                          when < O , < SLIDE{M} , as ... C > , S , V , G , E , D > guard ( DIM [ S ] ge ( M + 1 ) ) do < O , C , UNITE [ < SELECT [ 1 , S ] > , CUT [ ( M + 1 ) , S ] ] , V , G , E , D > ,
                                          when < O , < GET{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                 when INT{I} guard true do < O , C , S , UNITE [ < I > , V ] , G , E , D > ,
                                                                                                                                 when BOOL{B} guard true do < O , C , S , UNITE [ < B > , V ] , G , E , D >
                                                                                                                                 end
                                                                                                                              to [ SELECT [ N , G ] ] ,
                                          when < O , < PUSHBASIC{I} , as ... C > , S , V , G , E , D > guard true do < O , C , S , UNITE [ < I > , V ] , G , E , D > ,
                                          when < O , < ADD{ } , as ... C > , S , < I1 , I2 , as ... V > , G , E , D > guard true do < O , C , S , UNITE [ < ( I1 + I2 ) > , V ] , G , E , D > ,
                                          when < O , < SUB{ } , as ... C > , S , < I1 , I2 , as ... V > , G , E , D > guard true do < O , C , S , UNITE [ < ( I2 - I1 ) > , V ] , G , E , D > ,
                                          when < O , < MULT{ } , as ... C > , S , < I1 , I2 , as ... V > , G , E , D > guard true do < O , C , S , UNITE [ < ( I1 * I2 ) > , V ] , G , E , D > ,
                                          when < O , < DIV{ } , as ... C > , S , < I1 , I2 , as ... V > , G , E , D > guard true do < O , C , S , UNITE [ < ( I2 / I1 ) > , V ] , G , E , D > ,
                                          when < O , < EQ{ } , as ... C > , S , < B1 , B2 , as ... V > , G , E , D > guard true do < O , C , S , UNITE [ < ( B1 eq B2 ) > , V ] , G , E , D > ,
                                          when < O , < NE{ } , as ... C > , S , < B1 , B2 , as ... V > , G , E , D > guard true do < O , C , S , UNITE [ < not( ( B1 eq B2 ) ) > , V ] , G , E , D > ,
                                          when < O , < LT{ } , as ... C > , S , < B1 , B2 , as ... V > , G , E , D > guard true do < O , C , S , UNITE [ < ( B2 lt B1 ) > , V ] , G , E , D > ,
                                          when < O , < GT{ } , as ... C > , S , < B1 , B2 , as ... V > , G , E , D > guard true do < O , C , S , UNITE [ < ( B2 gt B1 ) > , V ] , G , E , D > ,
                                          when < O , < LE{ } , as ... C > , S , < B1 , B2 , as ... V > , G , E , D > guard true do < O , C , S , UNITE [ < ( B2 le B1 ) > , V ] , G , E , D > ,
                                          when < O , < GE{ } , as ... C > , S , < B1 , B2 , as ... V > , G , E , D > guard true do < O , C , S , UNITE [ < ( B2 ge B1 ) > , V ] , G , E , D > ,
                                          when < O , < NEG{ } , as ... C > , S , < I , as ... V > , G , E , D > guard true do < O , C , S , UNITE [ < ( 0 - I ) > , V ] , G , E , D > ,
                                          when < O , < NOT{ } , as ... C > , S , < B , as ... V > , G , E , D > guard true do < O , C , S , UNITE [ < not( B ) > , V ] , G , E , D > ,
                                          when < O , < JFALSE{L} , as ... C > , S , < true , as ... V > , G , E , D > guard true do < O , C , S , V , G , E , D > ,
                                          when < O , < JFALSE{L} , as ... C > , S , < false , as ... V > , G , E , D > guard true do < O , UNITE [ < JMP{L} > , C ] , S , V , G , E , D > ,
                                          when < O , < JMP{L} , as ... C > , S , V , G , E , D > guard true do < O , Regel_32 [ L , C ] , S , V , G , E , D > ,
                                          when < O , < LABEL{L} , as ... C > , S , V , G , E , D > guard true do < O , C , S , V , G , E , D > ,
                                          when < O , < HD{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                when CONS{N1 , N2} guard true do < O , C , UNITE [ < N1 > , S ] , V , G , E , D >
                                                                                                                                end
                                                                                                                             to [ SELECT [ N , G ] ] ,
                                          when < O , < CHD{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                 when CCONS{N1 , N2} guard true do < O , C , UNITE [ < N1 > , S ] , V , G , E , D >
                                                                                                                                 end
                                                                                                                              to [ SELECT [ N , G ] ] ,
                                          when < O , < TL{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                when CONS{N1 , N2} guard true do < O , C , UNITE [ < N2 > , S ] , V , G , E , D >
                                                                                                                                end
                                                                                                                             to [ SELECT [ N , G ] ] ,
                                          when < O , < CTL{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                 when CCONS{N1 , N2} guard true do < O , C , UNITE [ < N2 > , S ] , V , G , E , D >
                                                                                                                                 end
                                                                                                                              to [ SELECT [ N , G ] ] ,
                                          when < O , < NULL{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                  when CONS{N1 , N2} guard true do < O , C , S , UNITE [ < false > , V ] , G , E , D > ,
                                                                                                                                  when NIL{ } guard true do < O , C , S , UNITE [ < true > , V ] , G , E , D >
                                                                                                                                  end
                                                                                                                               to [ SELECT [ N , G ] ] ,
                                          when < O , < TEST_CONS{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                       when CONS{N1 , N2} guard true do < O , C , S , UNITE [ < true > , V ] , G , E , D >
                                                                                                                                       otherwise < O , C , S , UNITE [ < false > , V ] , G , E , D >
                                                                                                                                       end
                                                                                                                                    to [ SELECT [ N , G ] ] ,
                                          when < O , < TEST_NULL{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                       when NIL{ } guard true do < O , C , S , UNITE [ < true > , V ] , G , E , D >
                                                                                                                                       otherwise < O , C , S , UNITE [ < false > , V ] , G , E , D >
                                                                                                                                       end
                                                                                                                                    to [ SELECT [ N , G ] ] ,
                                          when < O , < TEST_CCONS{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                        when CCONS{N1 , N2} guard true do < O , C , S , UNITE [ < true > , V ] , G , E , D >
                                                                                                                                        otherwise < O , C , S , UNITE [ < false > , V ] , G , E , D >
                                                                                                                                        end
                                                                                                                                     to [ SELECT [ N , G ] ] ,
                                          when < O , < TEST_KON{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                      when KON{A} guard true do < O , C , S , UNITE [ < true > , V ] , G , E , D >
                                                                                                                                      otherwise < O , C , S , UNITE [ < false > , V ] , G , E , D >
                                                                                                                                      end
                                                                                                                                   to [ SELECT [ N , G ] ] ,
                                          when < O , < TEST_INT{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                      when INT{I} guard true do < O , C , S , UNITE [ < true > , V ] , G , E , D >
                                                                                                                                      otherwise < O , C , S , UNITE [ < false > , V ] , G , E , D >
                                                                                                                                      end
                                                                                                                                   to [ SELECT [ N , G ] ] ,
                                          when < O , < TEST_BOOL{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                       when BOOL{B} guard true do < O , C , S , UNITE [ < true > , V ] , G , E , D >
                                                                                                                                       otherwise < O , C , S , UNITE [ < false > , V ] , G , E , D >
                                                                                                                                       end
                                                                                                                                    to [ SELECT [ N , G ] ] ,
                                          when < O , < TEST_VAR{ } , as ... C > , < N , as ... S > , V , G , E , D > guard true do ap case 
                                                                                                                                      when PVAR{ } guard true do < O , C , S , UNITE [ < true > , V ] , G , E , D >
                                                                                                                                      otherwise < O , C , S , UNITE [ < false > , V ] , G , E , D >
                                                                                                                                      end
                                                                                                                                   to [ SELECT [ N , G ] ] ,
                                          when < O , < POP{M} , as ... C > , S , V , G , E , D > guard ( DIM [ S ] ge M ) do < O , C , CUT [ M , S ] , V , G , E , D > ,
                                          when A guard true do < A , false >
                                          otherwise false
                                          end
                                       to [ Tupel ]
               in G_m [ Comp [ Lambda_lifter [ Start [ 'def f a b = a + b in f 1 2` ] ] ] ]

