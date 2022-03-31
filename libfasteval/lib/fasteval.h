/*
 * Copyright (C) 1999, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2011,
 * 2012, 2013, 2014 Free Software Foundation, Inc.
 * Copyright (C) 2022 Astie Teddy and pBat developers.
 *
 * This file is part of pBat libfasteval based on libmatheval
 *
 * pBat libfasteval is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * pBat libfasteval is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pBat libfasteval.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef FASTEVAL_H
#define FASTEVAL_H 1

double fasteval_evaluate(char *expr, double (*get)(const char *),
  double (*set)(char *, double));

#endif