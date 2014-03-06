/*
 * Copyright (C) 1999, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2011,
 * 2012, 2013, 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU libmatheval
 *
 * GNU libmatheval is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GNU libmatheval is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU libmatheval.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef PARSER_SYMBOLS_H
#define PARSER_SYMBOLS_H 1

/* The following lines are a hack that changes name
   or parser var and function, enabling to include several
   Lex/Yacc-generated parser in a single project */

#define yymaxdepth evaluator_maxdepth
#define yyparse evaluator_parse
#define yylex   evaluator_lex
#define yyerror evaluator_error
#define yylval  evaluator_lval
#define yychar  evaluator_char
#define yydebug evaluator_debug
#define yypact  evaluator_pact
#define yyr1    evaluator_r1
#define yyr2    evaluator_r2
#define yydef   evaluator_def
#define yychk   evaluator_chk
#define yypgo   evaluator_pgo
#define yyact   evaluator_act
#define yyexca  evaluator_exca
#define yyerrflag evaluator_errflag
#define yynerrs evaluator_nerrs
#define yyps    evaluator_ps
#define yypv    evaluator_pv
#define yys     evaluator_s
#define yy_yys  evaluator_yys
#define yystate evaluator_state
#define yytmp   evaluator_tmp
#define yyv     evaluator_v
#define yy_yyv  evaluator_yyv
#define yyval   evaluator_val
#define yylloc  evaluator_lloc
#define yyreds  evaluator_reds
#define yytoks  evaluator_toks
#define yylhs   evaluator_yylhs
#define yylen   evaluator_yylen
#define yydefred evaluator_yydefred
#define yydgoto  evaluator_yydgoto
#define yysindex evaluator_yysindex
#define yyrindex evaluator_yyrindex
#define yygindex evaluator_yygindex
#define yytable  evaluator_yytable
#define yycheck  evaluator_yycheck
#define yyname   evaluator_yyname
#define yyrule   evaluator_yyrule

#endif // PARSER_SYMBOLS_H
