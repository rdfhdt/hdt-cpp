/* Literals.h
 * Copyright (C) 2010, Javier D. Fernandez & Miguel A, Martinez-Prieto
 * all rights reserved.
 *
 * Constant Literals used as Properties
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A Martinez-Prieto:  migumar2@infor.uva.es
 */
 
#ifndef LITERALS_H
#define LITERALS_H

#include <string>
using namespace std;

static const string DATASET="dataset";
static const string SYNTAX="syntax";
static const string OUTPUT="output";
static const string APPLICATION="application";
static const string PARSING="parsing";
static const string MAPPING="mapping";
static const string CLUSTER="cluster";
static const string D_SEPARATOR="dict_separator";
static const string D_ORDERING="dict_ordering";
static const string D_ENCODING="dict_encoding";
static const string D_COMPRESSION="dict_compression";
static const string T_ENCODING="triples_encoding";
static const string T_COMPRESSION="triples_compression";

static const string VSYNTAX_N3="n3";

static const string VPARSING_SPO="spo";
static const string VPARSING_SOP="sop";
static const string VPARSING_PSO="pso";
static const string VPARSING_POS="pos";
static const string VPARSING_OSP="osp";
static const string VPARSING_OPS="ops";

static const string VMAPPING_1="single";
static const string VMAPPING_2="shared";

static const string VD_ORDERING_LEX="lexicographic";

static const string VD_ENCODING_IND="indexed";

static const string VD_COMPRESSION_PPM="ppm";
static const string VD_COMPRESSION_LZ="lzma";

static const string VT_ENCODING_COMP="compact";
static const string VT_ENCODING_BIT="bitmap";
static const string VT_ENCODING_K2="k2tree";

static const string VD_COMPRESSION_HUF="huffman";

static const string VNO="no";
static const string VNONE="none";
static const string VPLAIN="plain";
static const string VYES="yes";

// OPTIONAL LITERALS
static const string HASH_SIZE="hash_size";
static const string GNUPLOT_PATH="gnuplot_path";
static const string DEGREE_PATH="degree_path";
static const string DICTIONARY_PATH="dictionary_path";
static const string TRIPLES_PATH="triples_path";
static const string RDF_PARSER="rdf_parser";
static const string RDF_PARSER_OWN="own";

#endif  /* _LITERALS_H */
