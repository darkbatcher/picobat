/*
 *
 *   libinteval, a lib for evaluating integer expression
 *   Copyright (C) 2010-2014 DarkBatcher
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

 #ifndef PARSER_SYM_H
 #define PARSER_SYM_H

/* The following lines are a hack that changes name
   or parser var and function, enabling to include several
   Lex/Yacc-generated parser in a single project */

#define yymaxdepth IntEval_maxdepth
#define yyparse IntEval_parse
#define yylex   IntEval_lex
#define yyerror IntEval_error
#define yylval  IntEval_lval
#define yychar  IntEval_char
#define yydebug IntEval_debug
#define yypact  IntEval_pact
#define yyr1    IntEval_r1
#define yyr2    IntEval_r2
#define yydef   IntEval_def
#define yychk   IntEval_chk
#define yypgo   IntEval_pgo
#define yyact   IntEval_act
#define yyexca  IntEval_exca
#define yyerrflag IntEval_errflag
#define yynerrs IntEval_nerrs
#define yyps    IntEval_ps
#define yypv    IntEval_pv
#define yys     IntEval_s
#define yy_yys  IntEval_yys
#define yystate IntEval_state
#define yytmp   IntEval_tmp
#define yyv     IntEval_v
#define yy_yyv  IntEval_yyv
#define yyval   IntEval_val
#define yylloc  IntEval_lloc
#define yyreds  IntEval_reds
#define yytoks  IntEval_toks
#define yylhs   IntEval_yylhs
#define yylen   IntEval_yylen
#define yydefred IntEval_yydefred
#define yydgoto  IntEval_yydgoto
#define yysindex IntEval_yysindex
#define yyrindex IntEval_yyrindex
#define yygindex IntEval_yygindex
#define yytable  IntEval_yytable
#define yycheck  IntEval_yycheck
#define yyname   IntEval_yyname
#define yyrule   IntEval_yyrule

 #endif // PARSER_SYM_H
