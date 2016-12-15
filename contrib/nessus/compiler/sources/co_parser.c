extern char *malloc(), *realloc();

# line 22 "Syntax.y"
#include "co_parser.h"             /* contains macro definitions and text substitutions */

#define YYSTYPE ParserStackPType                      /* data type for parser value stack */

static short PA_StackTokenFlg = FALSE;    /* true for first scan of a FOR (FOREACH)  loop */
static short PA_Loop = FALSE;                /* true for repetitive pseudo-scans of loops */

static short PA_Nested = 0;                                         /* counts nested [] ! */
static short PA_ReadEnable = FALSE;      /* true <=> read is an expression (proced. part) */

short ConstantSection;/* true <=> parsing constant, types, structures, or variable decls. */

# define IDENTIFIER 257
# define XINT 258
# define XFLOAT 259
# define STRING 260
# define UNIT 261
# define SITE 262
# define ARRAY 263
# define INTEGER 264
# define LONGSTRING 265
# define SIMPLESTRING 266
# define FLOATNUM 267
# define INPUT 268
# define OUTPUT 269
# define HIDDEN 270
# define NETWORK 271
# define CONST 272
# define TYPEDEF 273
# define VAR 274
# define MAP 275
# define XFILE 276
# define ACT 277
# define IOTYPE 278
# define NAME 279
# define SITEFUNC 280
# define ACTFUNC 281
# define OUTFUNC 282
# define CLIQUE 283
# define STAR 284
# define IRREGULAR 285
# define CHAIN 286
# define RING 287
# define CLUSTER 288
# define SUBNET 289
# define THROUGH 290
# define BY 291
# define OF 292
# define WITH 293
# define GET 294
# define FROM 295
# define CENTER 296
# define FOR 297
# define TO 298
# define DO 299
# define FOREACH 300
# define IN 301
# define IF 302
# define THEN 303
# define ELSE 304
# define ELSEIF 305
# define WHILE 306
# define XBEGIN 307
# define END 308
# define UMINUS 309
# define PLUS 310
# define MINUS 311
# define MULT 312
# define RDIV 313
# define DIV 314
# define MOD 315
# define XARCTAN 316
# define XCOS 317
# define XSIN 318
# define XEXP 319
# define XLN 320
# define XSQR 321
# define XSQRT 322
# define XMIN 323
# define XMAX 324
# define XSTRCAT 325
# define AND 326
# define OR 327
# define NOT 328
# define GT 329
# define GE 330
# define EQ 331
# define NE 332
# define LT 333
# define LE 334
# define PT 335
# define PTF 336
# define PF 337
# define POINT 338
# define PERIOD 339
# define SEMICOLON 340
# define COLON 341
# define OPARENT 342
# define CPARENT 343
# define OBRACKET 344
# define CBRACKET 345
# define ASSIGN 346
# define ARANGE 347
# define ILLEGAL 348
# define RANDOM 349
# define AT 350
# define PLANE 351
# define MATRIX 352
# define ELLIPSE 353
# define STRUCTURE 354
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 1921 "Syntax.y"




int yyexca[] ={
-1, 0,
	0, 340,
	-2, 0,
-1, 1,
	0, -1,
	-2, 0,
-1, 18,
	273, 8,
	274, 8,
	307, 8,
	354, 8,
	-2, 0,
-1, 22,
	343, 340,
	-2, 0,
-1, 24,
	343, 340,
	-2, 0,
-1, 29,
	274, 41,
	307, 41,
	354, 41,
	-2, 0,
-1, 45,
	343, 298,
	-2, 0,
-1, 46,
	343, 299,
	-2, 0,
-1, 55,
	274, 91,
	307, 91,
	-2, 0,
-1, 66,
	293, 340,
	294, 340,
	341, 340,
	350, 340,
	-2, 0,
-1, 86,
	341, 57,
	-2, 0,
-1, 153,
	307, 174,
	-2, 0,
-1, 176,
	293, 340,
	294, 340,
	341, 340,
	351, 340,
	352, 340,
	353, 340,
	-2, 0,
-1, 179,
	275, 340,
	290, 340,
	291, 340,
	293, 340,
	294, 340,
	296, 340,
	335, 340,
	336, 340,
	337, 340,
	341, 340,
	351, 340,
	352, 340,
	353, 340,
	-2, 0,
-1, 219,
	340, 25,
	-2, 0,
-1, 222,
	256, 29,
	339, 29,
	340, 29,
	-2, 250,
-1, 368,
	299, 214,
	-2, 0,
-1, 396,
	350, 156,
	-2, 0,
-1, 424,
	256, 31,
	339, 31,
	340, 31,
	-2, 250,
-1, 444,
	304, 340,
	308, 340,
	-2, 0,
-1, 468,
	275, 139,
	290, 139,
	291, 139,
	296, 139,
	335, 139,
	336, 139,
	337, 139,
	341, 139,
	350, 139,
	351, 139,
	352, 139,
	353, 139,
	-2, 0,
-1, 503,
	308, 340,
	-2, 0,
-1, 504,
	304, 226,
	308, 226,
	-2, 0,
-1, 546,
	339, 322,
	345, 322,
	-2, 0,
	};
# define YYNPROD 341
# define YYLAST 1111
int yyact[]={

   367,   378,   601,   525,   256,     4,   581,   290,   548,   107,
    12,   506,   404,   328,   320,    17,   499,   282,   191,   469,
    27,    94,   524,   486,    28,    93,   363,    47,    92,    47,
    54,   283,   114,   396,   397,   398,    91,    89,   546,   385,
   498,   106,   291,   106,   106,    90,    40,   148,    41,    81,
   346,    42,   369,   116,   162,   163,   164,   159,   140,   254,
   141,    23,    41,   353,   195,   149,    41,   255,    43,   415,
     9,   173,   169,    80,     9,   167,   116,   169,    41,   169,
   116,   274,    79,   162,   163,   164,   159,     9,   158,    83,
    83,   318,   144,   116,   564,   147,   190,   193,   155,     9,
   162,   163,   164,   159,   507,   158,   224,   196,   216,   217,
   226,   227,   228,   229,   405,   407,   406,   449,   289,   449,
   450,   144,   182,   186,   169,   535,    80,   319,   214,    79,
   396,   397,   398,   121,    82,   174,   141,   121,   143,   221,
   133,   135,   136,   134,   133,   135,   136,   134,   250,   251,
   160,   116,   144,   509,   205,   206,   152,   207,   319,   121,
   208,   209,   210,   211,   212,   213,   133,   135,   136,   134,
   281,   172,   275,   276,   144,   214,   286,   267,   141,   160,
   271,   173,   270,   292,   173,   262,   272,   462,    80,   449,
   450,   242,   588,   115,   121,   319,   160,   321,   513,   561,
   252,   133,   135,   136,   134,   349,   350,   351,   144,   309,
   310,   311,   312,   313,   314,   315,   151,   316,   557,    83,
   116,   144,    80,   562,   137,   325,   357,   175,   137,   189,
    80,   294,   295,   512,   335,   336,   337,   338,   339,   340,
   341,   343,   343,   405,   407,   406,   606,   111,   110,   318,
   137,   345,   464,   124,   125,   126,   127,   128,   129,   130,
   131,   132,   193,   445,   560,    80,   556,   152,   334,   323,
   144,   362,   169,   208,   209,   210,   211,   212,   213,    23,
   347,   146,   143,    80,   361,   137,   194,   142,   214,   382,
   327,   275,   402,   356,   582,   147,   183,   463,   408,   372,
   360,   401,   605,    85,   553,   161,   413,   392,   144,   352,
   321,   391,   144,   509,   390,   537,   479,   536,   416,   417,
   285,   418,   389,   387,   383,   226,   226,   426,   226,   376,
   393,   388,   144,   101,   145,   410,   280,   277,   319,   412,
    84,   487,   208,   209,   210,   211,   212,   213,   549,   429,
   430,   431,   432,   433,   434,   435,   436,   214,   437,   185,
   446,    95,   185,   570,   144,   144,   452,   472,   353,   471,
   169,   478,   354,   355,   188,     9,   470,   319,    60,   262,
   467,   116,   187,    41,   472,   477,   471,   441,   442,   443,
   262,   451,   453,    95,   262,   262,   112,   472,   408,   471,
   482,   157,   484,   184,   602,   291,   474,   352,   408,   488,
    22,    24,   475,   476,   438,    53,   485,   444,    16,   496,
   492,   447,   448,   232,   465,   466,    95,   185,   208,   209,
   210,   211,   212,   213,   427,    11,   307,   162,   163,   164,
   494,   497,   308,   214,   261,   495,   500,   500,   500,   505,
   614,   169,   169,   169,   420,   518,   519,   520,   521,   522,
   523,   141,   585,   584,   423,   425,   587,   517,   421,   371,
   296,   297,   529,   371,   300,   301,   514,   515,   262,   488,
   491,   541,   542,   501,   502,   544,   531,   532,   193,     3,
   488,   550,   287,   527,   552,   392,   527,   527,   540,   391,
   193,   248,   390,   317,     7,   510,   551,   324,   558,   547,
   389,   387,   565,   538,   528,   530,   559,   566,   554,   388,
   245,   233,   234,   235,   236,   237,   238,   239,   240,   241,
   210,   211,   212,   213,   568,   181,   563,   567,   490,   489,
   333,   332,   180,   214,   577,   550,   578,   579,   178,   574,
    45,   215,   550,   583,   569,   571,   580,    49,   279,   572,
   573,   329,   330,   331,   370,   371,   586,    78,   594,   527,
   527,    44,    37,    35,   306,   305,   597,   598,   483,   583,
   593,   596,   595,   604,    46,   583,   603,   600,   170,   262,
   262,   599,   414,   409,   262,   262,    51,   411,   176,   608,
   154,   603,   366,   612,   292,   603,   613,   611,   616,   607,
   615,   609,   610,   204,   526,   119,   589,   590,   617,    56,
   326,   591,   592,   150,   403,   201,   202,   203,   422,   118,
   317,   123,   317,   115,   121,   527,   122,   439,   440,   527,
   428,   133,   135,   136,   134,   208,   209,   210,   211,   212,
   213,   120,   117,   108,    87,   103,   461,   103,   103,   377,
   214,   208,   209,   210,   211,   212,   213,   365,   179,   365,
   299,   298,   473,   113,   144,   165,   214,    50,   141,   375,
   379,   115,   121,   247,   248,    30,   399,   111,   110,   133,
   135,   136,   134,   124,   125,   126,   127,   128,   129,   130,
   131,   132,   162,   163,   164,   208,   209,   210,   211,   212,
   213,    31,   359,   508,   493,    77,    37,    35,   278,    23,
   214,   116,   222,   121,   225,   137,   244,   243,   200,   199,
   133,   135,   136,   134,   144,   111,   110,   198,   197,   504,
   516,   124,   125,   126,   127,   128,   129,   130,   131,   132,
    49,    48,    39,    21,   273,   533,   534,    20,    21,    19,
   223,   365,   365,    14,    13,   288,   231,    23,   293,     8,
   543,   545,   555,   137,   194,   253,   111,   110,    38,   503,
   266,   368,   124,   125,   126,   127,   128,   129,   130,   131,
   132,   268,   342,   344,   480,   481,   115,   121,   208,   209,
   210,   211,   212,   213,   133,   135,   136,   134,    23,   358,
   116,   115,   121,   214,   137,   269,    97,   230,   265,   133,
   135,   136,   134,   246,   249,   511,   373,   374,    80,   575,
   576,    96,   100,    95,   264,    98,    99,   208,   209,   210,
   211,   212,   213,   302,   303,   263,   365,   365,   192,   348,
   111,   110,   214,   260,   259,   258,   124,   125,   126,   127,
   128,   129,   130,   131,   132,   111,   110,   257,   364,   156,
   153,   124,   125,   126,   127,   128,   129,   130,   131,   132,
   168,   395,    23,   424,   121,   177,   102,   177,   137,   394,
   400,   133,   135,   136,   134,   322,   468,    23,   381,   116,
   115,   121,   384,   137,   380,   284,   365,   171,   133,   135,
   136,   134,    69,   208,   209,   210,   211,   212,   213,    74,
    75,    76,    72,    73,    71,    68,    67,    65,   214,   138,
   139,    64,    63,    66,    61,    59,    58,   111,   110,   225,
   225,    55,   225,   124,   125,   126,   127,   128,   129,   130,
   131,   132,    26,   304,   111,   110,    86,    36,    34,    33,
   124,   125,   126,   127,   128,   129,   130,   131,   132,    23,
   460,   116,    32,    29,   419,   137,   220,   219,   218,   208,
   209,   210,   211,   212,   213,   109,    23,   105,   104,    18,
     6,    52,   137,    25,   214,    15,    10,     5,   454,   455,
   456,   457,   458,   459,   460,     2,     1,     0,   144,     0,
     0,     0,   141,   208,   209,   210,   211,   212,   213,   166,
     0,     0,     0,     0,    70,     0,    62,     0,   214,     0,
     0,     0,   454,   455,   456,   457,   458,   459,    57,     0,
     0,     0,     0,    70,     0,    62,    74,    75,    76,    72,
    73,    71,    68,   539,     0,     0,     0,     0,     0,    97,
     0,     0,     0,     0,     0,    74,    75,    76,    72,    73,
    71,    68,     0,     0,    96,   100,    95,   386,    98,    99,
     0,     0,     0,    97,    88,     0,     0,     0,     0,     0,
    97,     0,     0,     0,     0,     0,     0,     0,    96,   100,
    95,     0,    98,    99,     0,    96,   100,    95,     0,    98,
    99 };
int yypact[]={

   233, -1000,  -239, -1000, -1000,   163,   507, -1000, -1000, -1000,
   145,   501, -1000,  -281,  -281,  -330,   455, -1000,   496, -1000,
  -298,  -280,   494, -1000,   494,   141,   782, -1000, -1000,   311,
 -1000,  -267,  -122, -1000, -1000,    47,   828,    40, -1000,  -298,
   377, -1000,   377,   377,  -283,    26,    25, -1000,  -284, -1000,
 -1000,  -283,   -40,  -158, -1000,   763, -1000, -1000,  -252, -1000,
 -1000, -1000,  -291, -1000, -1000, -1000,  -121,  -291,   291,  -291,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -267, -1000,
 -1000,   285,   278, -1000,   147,    82,   -27, -1000, -1000, -1000,
 -1000, -1000, -1000, -1000, -1000,   644,   425,  -193,   481,   472,
   357, -1000, -1000,  -267,  -267, -1000,   488,   644,   644,   466,
   644,   644,   644, -1000,  -264, -1000, -1000,   444, -1000, -1000,
 -1000, -1000, -1000, -1000,  -281,  -281,  -281,  -281,  -281,  -281,
  -281,  -281,  -281, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
  -267, -1000,   470,   263, -1000,   427,   244,   644,   644,  -267,
 -1000,  -151,  -120,  -175, -1000,  -252,  -252, -1000,    45, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   301,    44,   644,
  -319,    27,   235, -1000, -1000, -1000,  -121, -1000,  -232,  -121,
  -267,  -267,  -247,  -247, -1000,   414,  -247,  -247,   554,   554,
   351, -1000, -1000,   351, -1000, -1000,   318, -1000, -1000, -1000,
 -1000,   167,   174, -1000, -1000, -1000, -1000, -1000,   644,   644,
   644,   644,   644,   644,   644,    -7,   -37,   603,  -267,   -31,
 -1000, -1000, -1000,  -281,    -8, -1000,   351, -1000, -1000,   118,
 -1000,   284,   -30,   644,   644,   644,   644,   644,   644,   644,
   644,   644, -1000, -1000, -1000, -1000, -1000, -1000,  -284, -1000,
   351,   351, -1000, -1000,   -57, -1000, -1000, -1000, -1000, -1000,
 -1000,   -63,    37, -1000, -1000, -1000,   -77,   -98,   -15,   -28,
   540,   308,   540, -1000,  -252,   301,   301,  -204,  -118, -1000,
   636,    32, -1000,  -281,    30,   821, -1000, -1000,  -318,  -281,
 -1000,   555, -1000,  -221, -1000, -1000,    79,   114, -1000, -1000,
    79,   114, -1000, -1000,  -187, -1000, -1000, -1000, -1000,   218,
   218,  -197,  -197,  -197,  -197, -1000, -1000,   644,   644, -1000,
   644, -1000,   179, -1000,   627,   555,   644,   555, -1000, -1000,
 -1000, -1000, -1000, -1000,   444,   118,   118,   118,   118,   118,
   118,   118,  -165,   351,  -165, -1000,  -120, -1000, -1000,  -247,
  -247, -1000, -1000, -1000,   -98,   -98,   -98,  -120,   -36,   644,
  -268,  -120,  -120,  -137,   540,   540, -1000,   703,   -69, -1000,
 -1000,    -4,  -137,  -118,  -118, -1000, -1000, -1000, -1000,   644,
 -1000,   120, -1000, -1000,  -247, -1000, -1000, -1000, -1000, -1000,
 -1000, -1000, -1000,   -92,  -319,  -319,    60,  -281,  -281,   644,
 -1000,   644,   351,   -92,    50, -1000, -1000, -1000, -1000, -1000,
 -1000, -1000, -1000, -1000,   282,   223,   351,   351,    32, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000,   335, -1000,  -252, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   150,
   173,  -251,  -251,  -251,  -152,  -120,   527,   -75,  -110,   540,
   540, -1000,   669,  -207,   644,   644,   644,   644,   644,   644,
  -124,   216,   212,  -124,  -124, -1000, -1000,   395,  -131, -1000,
 -1000,    22,    20,   797,    50, -1000, -1000, -1000, -1000, -1000,
   644,   644,   395,  -218,   351,    50,    58,   425, -1000, -1000,
 -1000, -1000, -1000,   644,     9, -1000, -1000, -1000, -1000,   425,
 -1000, -1000, -1000,   -38,     8, -1000, -1000,  -104,   -80,   540,
  -214,   644, -1000, -1000, -1000,  -209,   644, -1000,   351,   351,
   351,   351,   351,   351, -1000, -1000, -1000,  -264, -1000, -1000,
 -1000, -1000, -1000,   644,   107,    90,  -124,  -124, -1000, -1000,
    58,   395,   395,   644, -1000,   644,   644,    58,    -2,   206,
 -1000, -1000,   118,   209, -1000,  -116,  -120,  -120, -1000, -1000,
  -104,  -120,  -120,  -137, -1000,   351,   118,   644,   118, -1000,
 -1000, -1000, -1000, -1000,    -2,   644,   644,   118,   351,   351,
    -2,   129,   644, -1000, -1000, -1000,     7, -1000, -1000,  -267,
  -267,  -267,  -267,   -55,  -150, -1000,   129,   118,   118,   131,
   129, -1000,   -98, -1000,   351,   193,   -98,   644, -1000, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000,    32, -1000 };
int yypgo[]={

     0,  1006,  1005,   766,     4,   997,   996,   995,   993,   991,
   623,   990,   396,   571,    13,    50,   989,   759,   886,    46,
   614,   988,   987,     0,     9,   551,     1,    14,   985,   978,
   305,    16,   977,   976,   287,   139,   106,   974,   973,   685,
   972,   959,   958,    37,   296,   957,   956,   654,    45,    36,
    28,    25,    21,    18,   953,   952,   941,   619,   936,   550,
   935,   378,   934,   933,   588,    17,   817,   932,   931,   927,
   926,   598,   330,    12,    23,     8,     6,     2,     7,   912,
   907,   905,   904,   902,    39,   898,   896,   890,    19,    22,
   889,   881,     3,   870,   600,   869,   401,    59,    67,   867,
   855,   854,   853,   444,   849,    32,    40,   848,   845,   834,
   818,   815,    26,   809,   791,   781,    52,   780,   779,   772,
   739,    11,   713,   673,   651,   636,   631,   629,   615,   602,
   584,   677,   578 };
int yyr1[]={

     0,     1,     1,     1,     2,     2,     5,     5,     6,     6,
    16,    16,    16,    16,    17,    17,    17,    18,    18,    18,
    18,    20,    20,    22,    28,    29,    29,    32,    32,    32,
    32,    32,    33,    33,    35,    36,    36,    21,    37,    37,
    37,     7,     7,    38,    38,    38,    38,    39,    39,    39,
    40,    40,    41,    41,    41,    41,    41,    42,    45,    46,
    46,    46,    46,    47,    47,    47,    47,    47,    47,    43,
    50,    50,    51,    51,    44,    44,    48,    49,    49,    54,
    54,    54,    54,    54,    54,    52,    52,    52,    52,    52,
    52,     8,     8,    55,    56,    56,    56,    56,    57,    58,
    58,    58,    62,    62,    60,    61,    61,    61,    67,    67,
    68,    68,    69,    69,    79,    79,    79,    79,    79,    63,
    70,    71,    64,    80,    80,    80,    80,    81,    81,    83,
    83,    83,    83,    84,    84,    84,    84,    84,    84,    82,
    82,    85,    78,    78,    86,    86,    86,    86,    86,    86,
    88,    88,    72,    72,    90,    90,    90,    90,    91,    91,
    65,    74,    74,    75,    75,    75,    73,    73,    73,    73,
    76,    76,    77,    77,     9,     9,    93,    93,    93,    93,
    94,    94,    95,    95,    96,    96,    96,    10,    97,    97,
    97,    97,    98,    98,    98,    98,    99,    99,   103,   103,
   100,   100,   100,   106,   106,    53,    53,   101,   101,   101,
   111,   110,   113,   108,   114,   109,   115,   115,   115,   115,
   115,   115,   116,   116,   117,   102,   118,   118,   120,   120,
   120,   120,   121,   121,   122,   119,   119,   119,    23,    23,
    23,    23,    23,    23,    23,    23,    23,    23,    23,    23,
    23,   105,   105,   105,   105,   126,   123,   123,   127,   127,
   127,   127,   127,   127,   127,   128,   128,   125,   125,   125,
   125,   125,   104,   104,   104,   104,   104,   112,   112,   112,
   112,   112,   129,   129,   129,   129,   129,   129,   129,   129,
    30,    30,    30,   124,   124,   124,   124,   124,    13,    13,
    13,   130,   130,   130,   130,   130,   131,   131,    59,    59,
    59,    59,    59,    59,    66,    92,    92,    87,    87,   132,
   132,   132,   132,    25,    25,    25,    89,    89,    27,   107,
    11,    19,     3,    34,    15,    31,    12,    14,    24,    26,
     4 };
int yyr2[]={

     0,     5,     3,     3,    12,    15,    13,    13,     5,     3,
     5,     3,     3,     5,     7,     7,     7,     5,     5,     3,
     5,     7,    11,     7,    11,     3,     3,     7,     3,     3,
     7,     7,    17,    13,     7,     3,     3,     9,     3,     3,
     3,     5,     3,     5,     3,     3,     5,     9,     5,     9,
     3,     3,    11,    11,    11,    11,     7,     5,     5,     7,
     3,     3,     7,     3,     3,     3,     3,     3,     3,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     9,     7,
     3,     3,     5,     7,     7,     5,     5,     7,     7,     5,
     5,     5,     3,     3,     5,     3,     3,     5,     9,     3,
     3,     3,     7,     5,     9,     3,     3,     3,    19,     7,
    19,     7,    19,    17,     3,     3,     3,     3,     3,     3,
     3,     3,     7,     5,     3,     3,     3,     5,     3,     7,
     3,     3,     7,     3,     3,     3,     3,     3,     3,     5,
     3,     3,     5,     3,     7,     3,     3,     5,     7,     7,
     7,     7,     5,     5,     5,     5,     3,     5,    13,    13,
    13,     5,     3,     5,     5,     3,     3,     3,     3,     2,
     5,     3,     5,     3,     5,     3,     5,     3,     3,     5,
     9,     9,     3,     7,     3,     3,     3,     7,     7,     5,
     3,     3,     3,     3,     3,     3,     5,     5,     5,     5,
     9,     9,     9,     5,     3,     3,     3,     2,     2,     2,
     5,     9,     9,    11,     5,     8,     7,     2,     3,     5,
     7,     7,     7,     7,     5,    12,     2,     3,     5,     5,
     2,     3,     6,     7,     5,     4,     5,     3,     7,     7,
     7,     7,     7,     7,     7,     5,     5,     7,     3,     3,
     3,     3,     3,     3,     3,     5,     3,     3,     9,     9,
     9,     9,     9,     9,     9,     9,     9,     7,     7,     7,
     7,     7,     3,     3,     3,     7,     7,     7,     7,     5,
     7,     3,     7,     7,     7,     7,     7,     7,     7,    15,
     3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
     2,     7,     3,     5,     7,     7,     7,     7,     7,     3,
     3,     5,     7,     7,     7,    13,     3,     7,     3,     7,
     3,     7,     5,     7,     3,     7,     2,     2,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     1 };
int yychk[]={

 -1000,    -1,    -2,   256,    -4,    -5,   -11,   271,    -3,   338,
    -6,   272,    -4,   257,   256,    -7,   273,    -4,   -16,   -17,
   256,   257,   -12,   342,   -12,    -8,   -55,    -4,   354,   -38,
   -39,   256,   -40,   -41,   -42,   262,   -45,   261,   -17,   256,
   -19,   346,   331,   -19,   -13,   -59,  -130,    -4,   257,   256,
  -131,   -13,    -9,   274,    -4,   -56,   -57,   256,   -58,   -60,
   -61,   -62,   263,   -67,   -68,   -69,   -63,   -70,   289,   -79,
   261,   288,   286,   287,   283,   284,   285,   -39,   256,   -15,
   340,   -31,   256,   341,   293,   256,   -46,   -47,   256,   -43,
   -48,   -49,   -50,   -51,   -52,   279,   277,   262,   281,   282,
   278,   293,   -18,   -20,   -21,   -22,   -23,   -24,   276,   -28,
   311,   310,   -12,  -123,  -105,   256,   344,   275,  -127,  -128,
  -124,   257,  -125,  -126,   316,   317,   318,   319,   320,   321,
   322,   323,   324,   264,   267,   265,   266,   348,   -18,   -18,
   -14,   343,   -34,   256,   339,   -34,   256,   -19,   331,   -14,
   -10,   256,   307,   -93,   -94,   256,   -95,   -96,   263,   261,
   354,   -30,   258,   259,   260,   -57,   256,   -31,   -66,   -24,
   -64,   -80,   292,    -4,   256,   348,   -71,   -66,   257,   -71,
   257,   257,   -43,   -44,   256,   280,   -43,   -44,   -34,   256,
   -23,   -53,  -107,   -23,   349,   257,   -24,   257,   256,   257,
   256,   268,   269,   270,   256,   -15,   -15,   -15,   310,   311,
   312,   313,   314,   315,   325,   -25,   -23,   -23,   -29,   -32,
   -33,   -35,   256,   294,   -36,   -20,   -23,   -23,   -23,   -23,
   -66,    -3,   -30,   -12,   -12,   -12,   -12,   -12,   -12,   -12,
   -12,   -12,   -15,   257,   256,   257,  -131,   256,   257,  -131,
   -23,   -23,   -15,   -10,   -97,   -98,    -4,   -99,  -100,  -101,
  -102,  -103,  -105,  -108,  -109,  -110,  -117,   297,  -114,  -111,
   302,   300,   306,   -94,   256,   -31,   -31,   292,   -59,   257,
   292,   -23,   -65,   350,   -81,   293,    -4,   257,   -64,   350,
   -78,   274,    -4,   -64,   -15,   -15,   -34,   -34,   257,   256,
   -34,   -34,   -47,   -47,   -54,   257,   256,   269,   268,   -23,
   -23,   -23,   -23,   -23,   -23,   -23,   -26,   -34,   256,   345,
   -27,   347,   292,   -15,   -34,   256,   -12,   298,   -14,   277,
   278,   279,   257,   256,   298,   -23,   -23,   -23,   -23,   -23,
   -23,   -23,   -25,   -23,   -25,   308,   -15,   -53,  -104,   268,
   269,   270,   -19,   331,   335,   336,   256,   303,  -113,  -103,
  -105,   299,   299,  -112,   328,   -12,  -129,   -23,  -115,  -116,
   256,   257,  -112,   -59,   -59,   -96,   -15,   -61,   -26,   -12,
   -82,   -85,    -4,   294,   -83,   -84,   256,   -43,   -48,   -49,
   -50,   -51,   -52,   -72,   -90,   -91,   351,   352,   353,   -12,
   -87,   -24,   -23,   -72,   -73,   335,   337,   336,    -4,   -44,
   -43,   -44,   -43,   -26,   -34,   256,   -23,   -23,   -23,   -37,
   275,   289,   -30,   -35,   256,   -35,   -23,   -36,   -30,   -14,
   -14,   -14,   -14,   -14,   -14,   -14,   -14,   -14,   -98,   -34,
   -34,  -105,  -105,  -105,   -97,   299,   -23,   -97,   -97,   326,
   327,  -112,   -23,  -112,   329,   330,   331,   332,   333,   334,
   301,   -34,   256,   301,   256,   -15,   -15,   -23,   -86,   -88,
   256,   279,   277,   -34,   -73,   -65,   -65,   325,   311,   256,
   -12,   -12,   -23,  -132,   -23,   -73,   -74,   291,    -4,   257,
   256,   257,   -26,   -34,   -14,   -31,   269,   268,  -106,   -31,
    -4,  -106,  -106,  -118,  -120,    -4,  -121,   256,  -122,   305,
   -97,   298,   308,   308,  -112,  -112,   -34,   -14,   -23,   -23,
   -23,   -23,   -23,   -23,   -89,   -92,   -20,  -105,  -116,   256,
  -116,   -89,   -89,   -34,   -34,   256,   295,   295,   -84,   256,
   -74,   -23,   -23,   -34,   -26,   -34,   256,   -74,   -75,   290,
    -4,   -53,   -23,   295,   -53,  -119,   304,   256,    -4,  -121,
   256,   303,   303,  -112,   308,   -23,   -23,   -24,   -23,   -88,
   256,   -88,   -89,   -89,   -75,   -34,   -34,   -23,   -23,   -23,
   -75,   -76,   296,    -4,   257,   256,   -14,   257,   308,   -97,
   -97,   -97,   -97,   -14,   -23,   -14,   -76,   -23,   -23,   -14,
   -76,   -77,   275,    -4,   -23,   295,   301,   -27,   -77,   -14,
   -14,   -78,   -77,   -92,   257,   -92,   -23,   -26 };
int yydef[]={

    -2,    -2,     0,     2,     3,   340,     0,   330,     1,   332,
   340,     0,     9,     0,     0,   340,     0,    42,    -2,    11,
    12,     0,    -2,   336,    -2,   340,     0,    92,    93,    -2,
    44,    45,     0,    50,    51,     0,     0,     0,    10,    13,
     0,   331,     0,     0,     0,    -2,    -2,   300,   309,   310,
   302,     0,     0,     0,   175,    -2,    95,    96,     0,    99,
   100,   101,     0,   105,   106,   107,    -2,     0,     0,     0,
   119,   120,   114,   115,   116,   117,   118,    43,    46,    48,
   334,     0,     0,   335,     0,     0,    -2,    60,    61,    63,
    64,    65,    66,    67,    68,     0,     0,     0,     0,     0,
     0,    58,    16,     0,     0,    19,     0,     0,     0,     0,
     0,     0,     0,   248,   249,   250,   338,     0,   256,   257,
   251,   252,   253,   254,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   293,   294,   295,   296,   297,    14,    15,
     0,   337,     0,   311,   333,     0,   303,     0,     0,     0,
     4,     0,   340,    -2,   177,   178,     0,   182,     0,   184,
   185,   186,   290,   291,   292,    94,    97,     0,     0,     0,
   103,   340,     0,   124,   125,   126,    -2,   121,   340,    -2,
     0,     0,     0,     0,    56,     0,     0,     0,     0,     0,
    69,    76,   205,   206,   329,    77,     0,    70,    71,    72,
    73,    85,    86,    89,    90,    17,    18,    20,     0,     0,
     0,     0,     0,     0,     0,     0,   324,     0,     0,    -2,
    26,    28,    -2,     0,     0,    35,    36,   245,   246,     0,
   255,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     6,   308,   313,   312,   301,   305,     0,   304,
   306,   307,     7,     5,     0,   190,   191,   192,   193,   194,
   195,     0,     0,   207,   208,   209,     0,     0,     0,     0,
     0,     0,     0,   176,   179,     0,     0,     0,     0,   309,
     0,     0,   102,     0,   340,     0,   128,   123,   109,     0,
   111,     0,   143,   340,    47,    49,     0,     0,    74,    75,
     0,     0,    59,    62,     0,    80,    81,    87,    88,   238,
   239,   240,   241,   242,   243,   244,    21,     0,     0,   339,
     0,   328,     0,    23,     0,     0,     0,     0,   247,   267,
   268,   269,   270,   271,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   324,     0,   187,   189,   196,   197,   272,
   273,   274,   198,   199,     0,     0,     0,   340,     0,     0,
     0,   340,   340,   224,     0,     0,   281,     0,    -2,   217,
   218,     0,   210,     0,     0,   183,    98,   104,   314,     0,
   122,     0,   140,   141,   127,   130,   131,   133,   134,   135,
   136,   137,   138,   340,     0,     0,    -2,     0,     0,     0,
   142,     0,   318,   340,   340,   166,   167,   168,   169,    52,
    53,    54,    55,    78,     0,    82,   323,   325,     0,    37,
    38,    39,    40,    27,    -2,    30,     0,    34,     0,   258,
   259,   260,   261,   262,   263,   264,   265,   266,   188,     0,
     0,   340,   340,   340,    -2,   340,     0,     0,     0,     0,
     0,   279,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   219,     0,     0,   181,   180,     0,    -2,   145,
   146,     0,     0,     0,   340,   152,   153,   154,   155,   157,
     0,     0,     0,     0,   320,   340,   340,     0,   162,    79,
    84,    83,    22,     0,     0,    24,   275,   276,   200,     0,
   204,   201,   202,    -2,    -2,   227,   230,   231,     0,     0,
     0,     0,   215,   211,   277,   278,     0,   280,   282,   283,
   284,   285,   286,   287,   288,   326,   327,   316,   216,   221,
   220,   222,   223,     0,     0,   147,     0,     0,   129,   132,
   340,     0,     0,     0,   317,     0,    -2,   340,   340,     0,
   165,   161,     0,     0,   203,     0,   340,   340,   237,   228,
   229,   340,   340,   234,   213,   212,     0,     0,     0,   144,
   149,   148,   150,   151,   340,     0,     0,     0,   319,   321,
   340,   340,     0,   171,   163,   164,     0,    33,   225,   235,
   236,   233,   232,     0,     0,   160,   340,     0,     0,   340,
   340,   113,     0,   173,   170,     0,     0,     0,   108,   158,
   159,   110,   112,   172,    32,   289,     0,   315 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"IDENTIFIER",	257,
	"XINT",	258,
	"XFLOAT",	259,
	"STRING",	260,
	"UNIT",	261,
	"SITE",	262,
	"ARRAY",	263,
	"INTEGER",	264,
	"LONGSTRING",	265,
	"SIMPLESTRING",	266,
	"FLOATNUM",	267,
	"INPUT",	268,
	"OUTPUT",	269,
	"HIDDEN",	270,
	"NETWORK",	271,
	"CONST",	272,
	"TYPEDEF",	273,
	"VAR",	274,
	"MAP",	275,
	"XFILE",	276,
	"ACT",	277,
	"IOTYPE",	278,
	"NAME",	279,
	"SITEFUNC",	280,
	"ACTFUNC",	281,
	"OUTFUNC",	282,
	"CLIQUE",	283,
	"STAR",	284,
	"IRREGULAR",	285,
	"CHAIN",	286,
	"RING",	287,
	"CLUSTER",	288,
	"SUBNET",	289,
	"THROUGH",	290,
	"BY",	291,
	"OF",	292,
	"WITH",	293,
	"GET",	294,
	"FROM",	295,
	"CENTER",	296,
	"FOR",	297,
	"TO",	298,
	"DO",	299,
	"FOREACH",	300,
	"IN",	301,
	"IF",	302,
	"THEN",	303,
	"ELSE",	304,
	"ELSEIF",	305,
	"WHILE",	306,
	"XBEGIN",	307,
	"END",	308,
	"UMINUS",	309,
	"PLUS",	310,
	"MINUS",	311,
	"MULT",	312,
	"RDIV",	313,
	"DIV",	314,
	"MOD",	315,
	"XARCTAN",	316,
	"XCOS",	317,
	"XSIN",	318,
	"XEXP",	319,
	"XLN",	320,
	"XSQR",	321,
	"XSQRT",	322,
	"XMIN",	323,
	"XMAX",	324,
	"XSTRCAT",	325,
	"AND",	326,
	"OR",	327,
	"NOT",	328,
	"GT",	329,
	"GE",	330,
	"EQ",	331,
	"NE",	332,
	"LT",	333,
	"LE",	334,
	"PT",	335,
	"PTF",	336,
	"PF",	337,
	"POINT",	338,
	"PERIOD",	339,
	"SEMICOLON",	340,
	"COLON",	341,
	"OPARENT",	342,
	"CPARENT",	343,
	"OBRACKET",	344,
	"CBRACKET",	345,
	"ASSIGN",	346,
	"ARANGE",	347,
	"ILLEGAL",	348,
	"RANDOM",	349,
	"AT",	350,
	"PLANE",	351,
	"MATRIX",	352,
	"ELLIPSE",	353,
	"STRUCTURE",	354,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"program : topology_block pt",
	"program : error",
	"program : empty",
	"topology_block : top_header const_block type_block struct_block var_block progr_block",
	"topology_block : top_header const_block type_block struct_block var_block error progr_block",
	"top_header : network IDENTIFIER op parameter_list cp sc",
	"top_header : network error op parameter_list cp sc",
	"const_block : CONST cons_def_list",
	"const_block : empty",
	"cons_def_list : cons_def_list cons_def",
	"cons_def_list : cons_def",
	"cons_def_list : error",
	"cons_def_list : cons_def_list error",
	"cons_def : IDENTIFIER EQ constant",
	"cons_def : IDENTIFIER ass constant",
	"cons_def : error ass constant",
	"constant : array_constant sc",
	"constant : file_constant sc",
	"constant : map_constant",
	"constant : expression sc",
	"array_constant : ob expression_list cb",
	"array_constant : ob expression range expression cb",
	"map_constant : map_header map_body sc",
	"map_header : MAP simple_type TO simple_type colon",
	"map_body : cons_map_body",
	"map_body : file_map_body",
	"cons_map_body : cons_map_body co relation",
	"cons_map_body : relation",
	"cons_map_body : error",
	"cons_map_body : cons_map_body error relation",
	"cons_map_body : cons_map_body co error",
	"file_map_body : GET op expression co expression cp FROM IDENTIFIER",
	"file_map_body : GET op expression cp FROM IDENTIFIER",
	"relation : map_constant_list TO map_constant_list",
	"map_constant_list : array_constant",
	"map_constant_list : expression",
	"file_constant : XFILE expression OF file_type",
	"file_type : MAP",
	"file_type : SUBNET",
	"file_type : simple_type",
	"type_block : TYPEDEF type_body",
	"type_block : empty",
	"type_body : type_body type_def",
	"type_body : type_def",
	"type_body : error",
	"type_body : type_body error",
	"type_def : type_def_expr colon IDENTIFIER sc",
	"type_def : error sc",
	"type_def : type_def_expr error IDENTIFIER sc",
	"type_def_expr : site_type_def",
	"type_def_expr : unit_type_def",
	"site_type_def : SITE WITH name_def co site_func_def",
	"site_type_def : SITE WITH site_func_def co name_def",
	"site_type_def : SITE error name_def co site_func_def",
	"site_type_def : SITE error site_func_def co name_def",
	"site_type_def : SITE WITH error",
	"unit_type_def : unit_header unit_type_body",
	"unit_header : UNIT WITH",
	"unit_type_body : unit_type_body co unit_type_param",
	"unit_type_body : unit_type_param",
	"unit_type_body : error",
	"unit_type_body : unit_type_body error unit_type_param",
	"unit_type_param : name_def",
	"unit_type_param : act_def",
	"unit_type_param : sites_def",
	"unit_type_param : actfunc_def",
	"unit_type_param : outfunc_def",
	"unit_type_param : stype_def",
	"name_def : NAME expression",
	"actfunc_def : ACTFUNC IDENTIFIER",
	"actfunc_def : ACTFUNC error",
	"outfunc_def : OUTFUNC IDENTIFIER",
	"outfunc_def : OUTFUNC error",
	"site_func_def : SITEFUNC IDENTIFIER",
	"site_func_def : SITEFUNC error",
	"act_def : ACT weight_expression",
	"sites_def : SITE IDENTIFIER",
	"sites_def : SITE ob site_list cb",
	"site_list : site_list co IDENTIFIER",
	"site_list : IDENTIFIER",
	"site_list : error",
	"site_list : site_list error",
	"site_list : site_list error IDENTIFIER",
	"site_list : site_list co error",
	"stype_def : IOTYPE INPUT",
	"stype_def : IOTYPE OUTPUT",
	"stype_def : IOTYPE INPUT OUTPUT",
	"stype_def : IOTYPE OUTPUT INPUT",
	"stype_def : IOTYPE HIDDEN",
	"stype_def : IOTYPE error",
	"struct_block : struct struct_body",
	"struct_block : empty",
	"struct : STRUCTURE",
	"struct_body : struct_body struct_def",
	"struct_body : struct_def",
	"struct_body : error",
	"struct_body : struct_body error",
	"struct_def : struct_spec colon identifier_list sc",
	"struct_spec : dim2_struct",
	"struct_spec : simple_struct",
	"struct_spec : single_unit",
	"single_unit : unit unit_spec at_spec",
	"single_unit : unit unit_spec",
	"dim2_struct : ARRAY field_size OF simple_struct",
	"simple_struct : cluster_struct",
	"simple_struct : subnet_struct",
	"simple_struct : connected_struct",
	"cluster_struct : cluster unit_field_size unit_spec topology_spec direction conn_spec site_spec center_spec map_spec",
	"cluster_struct : cluster unit_field_size unit_spec",
	"subnet_struct : SUBNET IDENTIFIER AT op expression co expression cp param_spec",
	"subnet_struct : SUBNET IDENTIFIER param_spec",
	"connected_struct : connection_key unit_field_size unit_spec topology_spec direction conn_spec site_spec center_spec map_spec",
	"connected_struct : connection_key unit_field_size unit_spec direction conn_spec site_spec center_spec map_spec",
	"connection_key : CHAIN",
	"connection_key : RING",
	"connection_key : CLIQUE",
	"connection_key : STAR",
	"connection_key : IRREGULAR",
	"unit : UNIT",
	"cluster : CLUSTER",
	"unit_field_size : field_size",
	"unit_spec : unit_struct with_spec get_spec",
	"unit_struct : OF IDENTIFIER",
	"unit_struct : empty",
	"unit_struct : error",
	"unit_struct : ILLEGAL",
	"with_spec : WITH unit_struct_body",
	"with_spec : empty",
	"unit_struct_body : unit_struct_body co unit_struct_param",
	"unit_struct_body : unit_struct_param",
	"unit_struct_body : error",
	"unit_struct_body : unit_struct_body co error",
	"unit_struct_param : name_def",
	"unit_struct_param : act_def",
	"unit_struct_param : sites_def",
	"unit_struct_param : actfunc_def",
	"unit_struct_param : outfunc_def",
	"unit_struct_param : stype_def",
	"get_spec : get struct_field",
	"get_spec : empty",
	"get : GET",
	"param_spec : VAR param_val",
	"param_spec : empty",
	"struct_field : struct_field co field_struct",
	"struct_field : field_struct",
	"struct_field : error",
	"struct_field : struct_field error",
	"struct_field : struct_field error field_struct",
	"struct_field : struct_field co error",
	"field_struct : NAME FROM element_group",
	"field_struct : ACT FROM element_group",
	"topology_spec : plane_spec at_spec",
	"topology_spec : area_spec at_spec",
	"plane_spec : PLANE XSTRCAT",
	"plane_spec : PLANE MINUS",
	"plane_spec : PLANE",
	"plane_spec : PLANE error",
	"area_spec : MATRIX op expression co expression cp",
	"area_spec : ELLIPSE op expression co expression cp",
	"at_spec : AT op expression co expression cp",
	"conn_spec : BY weight_expression",
	"conn_spec : empty",
	"site_spec : THROUGH IDENTIFIER",
	"site_spec : THROUGH error",
	"site_spec : empty",
	"direction : PT",
	"direction : PF",
	"direction : PTF",
	"direction : empty",
	"center_spec : CENTER expression",
	"center_spec : empty",
	"map_spec : MAP field_range",
	"map_spec : empty",
	"var_block : VAR var_decl_block",
	"var_block : empty",
	"var_decl_block : var_decl_block var_decl",
	"var_decl_block : var_decl",
	"var_decl_block : error",
	"var_decl_block : var_decl_block error",
	"var_decl : var_type colon identifier_list sc",
	"var_decl : error colon identifier_list sc",
	"var_type : singular_type",
	"var_type : ARRAY OF singular_type",
	"singular_type : UNIT",
	"singular_type : STRUCTURE",
	"singular_type : simple_type",
	"progr_block : XBEGIN statement_list END",
	"statement_list : statement_list sc statement",
	"statement_list : statement_list sc",
	"statement_list : statement",
	"statement_list : empty",
	"statement : ass_stmt",
	"statement : conn_stmt",
	"statement : loop_stmt",
	"statement : if_stmt",
	"ass_stmt : stmt_left weight_expression",
	"ass_stmt : stmt_left topology_key",
	"stmt_left : variable ass",
	"stmt_left : variable EQ",
	"conn_stmt : variable PT variable conn_weight",
	"conn_stmt : variable PTF variable conn_weight",
	"conn_stmt : variable error variable conn_weight",
	"conn_weight : colon weight_expression",
	"conn_weight : empty",
	"weight_expression : random",
	"weight_expression : expression",
	"loop_stmt : for_loop",
	"loop_stmt : foreach_loop",
	"loop_stmt : while_loop",
	"while_header : WHILE log_expr",
	"while_loop : while_header DO statement_list END",
	"for_ass_stmt : stmt_left expression TO expression",
	"for_loop : FOR for_ass_stmt DO statement_list END",
	"foreach_head : FOREACH loop_var_list",
	"foreach_loop : foreach_head DO statement_list END",
	"loop_var_list : loop_var_list co loop_var_def",
	"loop_var_list : loop_var_def",
	"loop_var_list : error",
	"loop_var_list : loop_var_list error",
	"loop_var_list : loop_var_list error loop_var_def",
	"loop_var_list : loop_var_list co error",
	"loop_var_def : IDENTIFIER IN element_group",
	"loop_var_def : IDENTIFIER error element_group",
	"if_head : IF log_expr",
	"if_stmt : if_head THEN statement_list elseIf_part else_part END",
	"elseIf_part : elseIf_body",
	"elseIf_part : empty",
	"elseIf_body : elseIf_body elseIf",
	"elseIf_body : elseIf_body error",
	"elseIf_body : elseIf",
	"elseIf_body : error",
	"elseIf : elseIf_head THEN statement_list",
	"elseIf : error THEN statement_list",
	"elseIf_head : ELSEIF log_expr",
	"else_part : ELSE statement_list",
	"else_part : error statement_list",
	"else_part : empty",
	"expression : expression PLUS expression",
	"expression : expression MINUS expression",
	"expression : expression MULT expression",
	"expression : expression RDIV expression",
	"expression : expression DIV expression",
	"expression : expression MOD expression",
	"expression : expression XSTRCAT expression",
	"expression : MINUS expression",
	"expression : PLUS expression",
	"expression : op expression cp",
	"expression : function_expression",
	"expression : variable",
	"expression : error",
	"variable : unsigned_constant",
	"variable : IDENTIFIER",
	"variable : selection_expression",
	"variable : field_element",
	"field_element : variable field_size",
	"function_expression : simple_func_expr",
	"function_expression : list_func_expr",
	"simple_func_expr : XARCTAN op expression cp",
	"simple_func_expr : XCOS op expression cp",
	"simple_func_expr : XSIN op expression cp",
	"simple_func_expr : XEXP op expression cp",
	"simple_func_expr : XLN op expression cp",
	"simple_func_expr : XSQR op expression cp",
	"simple_func_expr : XSQRT op expression cp",
	"list_func_expr : XMIN op expression_list cp",
	"list_func_expr : XMAX op expression_list cp",
	"selection_expression : variable pt ACT",
	"selection_expression : variable pt IOTYPE",
	"selection_expression : variable pt NAME",
	"selection_expression : variable pt IDENTIFIER",
	"selection_expression : variable pt error",
	"topology_key : INPUT",
	"topology_key : OUTPUT",
	"topology_key : HIDDEN",
	"topology_key : INPUT co OUTPUT",
	"topology_key : OUTPUT co INPUT",
	"log_expr : log_expr AND log_expr",
	"log_expr : log_expr OR log_expr",
	"log_expr : NOT log_expr",
	"log_expr : op log_expr cp",
	"log_expr : bool_expr",
	"bool_expr : expression GT expression",
	"bool_expr : expression GE expression",
	"bool_expr : expression EQ expression",
	"bool_expr : expression NE expression",
	"bool_expr : expression LT expression",
	"bool_expr : expression LE expression",
	"bool_expr : expression IN element_group",
	"bool_expr : op expression co expression cp IN field_range",
	"simple_type : XINT",
	"simple_type : XFLOAT",
	"simple_type : STRING",
	"unsigned_constant : INTEGER",
	"unsigned_constant : FLOATNUM",
	"unsigned_constant : LONGSTRING",
	"unsigned_constant : SIMPLESTRING",
	"unsigned_constant : ILLEGAL",
	"parameter_list : identifier_list",
	"parameter_list : default_prm_list",
	"parameter_list : empty",
	"default_prm_list : default_prm_list co default_prm",
	"default_prm_list : default_prm",
	"default_prm_list : default_prm_list error",
	"default_prm_list : default_prm_list error default_prm",
	"default_prm_list : default_prm_list co error",
	"default_prm : IDENTIFIER ass expression",
	"default_prm : IDENTIFIER EQ expression",
	"identifier_list : identifier_list co IDENTIFIER",
	"identifier_list : IDENTIFIER",
	"identifier_list : error",
	"identifier_list : identifier_list error",
	"identifier_list : identifier_list error IDENTIFIER",
	"identifier_list : identifier_list co error",
	"field_size : ob expression cb",
	"field_range : variable ob expression range expression cb",
	"field_range : variable",
	"param_val : ob subnet_param_list cb",
	"param_val : expression",
	"subnet_param_list : subnet_param_list co expression",
	"subnet_param_list : expression",
	"subnet_param_list : subnet_param_list error expression",
	"subnet_param_list : subnet_param_list error",
	"expression_list : expression_list co expression",
	"expression_list : expression",
	"expression_list : expression_list error expression",
	"element_group : field_range",
	"element_group : array_constant",
	"range : ARANGE",
	"random : RANDOM",
	"network : NETWORK",
	"ass : ASSIGN",
	"pt : POINT",
	"co : PERIOD",
	"sc : SEMICOLON",
	"colon : COLON",
	"op : OPARENT",
	"cp : CPARENT",
	"ob : OBRACKET",
	"cb : CBRACKET",
	"empty : /* empty */",
};
#endif /* YYDEBUG */
#line 1 "/usr/lib/yaccpar"
/*	@(#)yaccpar 1.10 89/04/04 SMI; from S5R3 1.10	*/

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	{ free(yys); free(yyv); return(0); }
#define YYABORT		{ free(yys); free(yyv); return(1); }
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-1000)

/*
** static variables used by the parser
*/
YYSTYPE *yyv;                           /* value stack */  /* sparc */             
static int *yys;			/* state stack */

static YYSTYPE *yypv;			/* top of value stack */
static int *yyps;			/* top of state stack */

static int yystate;			/* current state */
static int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */

int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */


/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
yyparse()
{
	register YYSTYPE *yypvt;	/* top of value stack for $vars */
register short yyn;             /* sparc */
	unsigned yymaxdepth = YYMAXDEPTH;

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yyv = (YYSTYPE*)malloc(yymaxdepth*sizeof(YYSTYPE));
	yys = (int*)malloc(yymaxdepth*sizeof(int));
	if (!yyv || !yys)
	{
		yyerror( "out of memory" );
		return(1);
	}
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

	goto yystack;
	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			(void)printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				(void)printf( "end-of-file\n" );
			else if ( yychar < 0 )
				(void)printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				(void)printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ yymaxdepth ] )	/* room on stack? */
		{
			/*
			** reallocate and recover.  Note that pointers
			** have to be reset, or bad things will happen
			*/
			int yyps_index = (yy_ps - yys);
			int yypv_index = (yy_pv - yyv);
			int yypvt_index = (yypvt - yyv);
			yymaxdepth += YYMAXDEPTH;
			yyv = (YYSTYPE*)realloc((char*)yyv,
				yymaxdepth * sizeof(YYSTYPE));
			yys = (int*)realloc((char*)yys,
				yymaxdepth * sizeof(int));
			if (!yyv || !yys)
			{
				yyerror( "yacc stack overflow" );
				return(1);
			}
			yy_ps = yys + yyps_index;
			yy_pv = yyv + yypv_index;
			yypvt = yyv + yypvt_index;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			(void)printf( "Received token " );
			if ( yychar == 0 )
				(void)printf( "end-of-file\n" );
			else if ( yychar < 0 )
				(void)printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				(void)printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				(void)printf( "Received token " );
				if ( yychar == 0 )
					(void)printf( "end-of-file\n" );
				else if ( yychar < 0 )
					(void)printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					(void)printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
   if((yyn = yypact[yystate]) > YYFLAG && yyn  < YYLAST)
{ register int x;
  for(x = yyn>0? yyn: 0; x < YYLAST;  ++x)
    if(yychk[yyact[x]] == x - yyn && x - yyn != YYERRCODE)
      yyNewerror(0, yydisplay(x-yyn));
}
yyNewerror(0,0);
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
				yynerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						(void)printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					(void)printf( "Error recovery discards " );
					if ( yychar == 0 )
						(void)printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						(void)printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						(void)printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			(void)printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 1:
# line 87 "Syntax.y"
{ yyval = psNULL; 
		  ER_ListErrors(yylineno, SymTabPtr->FileStr);} break;
case 2:
# line 90 "Syntax.y"
{ yyerror("cannot continue parsing due to errors");
		  ER_ListErrors(yylineno, SymTabPtr->FileStr);
		  return(TRUE);  	/* give up */ 
		} break;
case 3:
# line 95 "Syntax.y"
{ yyerror("empty nessus program");
		  ER_ListErrors(yylineno, SC_InputStack->InFileName);
		  return(TRUE);  	/* give up */ 
		} break;
case 5:
# line 104 "Syntax.y"
{ yyerror("syntax: illegal beginning of new section, 'begin' missing");
		} break;
case 6:
# line 111 "Syntax.y"
{ if(Topology) {
		    SYM_InsertNetName(yypvt[-4]);
		    OUT_InsertNetName(yypvt[-4]);
		    PA_AssignParamVal(yypvt[-2], PA_ProgramArguments);
		  }
		  yyval = psNULL; 
		} break;
case 7:
# line 119 "Syntax.y"
{ yyerror("syntax: missing network identifier"); } break;
case 8:
# line 125 "Syntax.y"
{ yyval = psNULL; } break;
case 9:
# line 127 "Syntax.y"
{ yyval = psNULL; } break;
case 10:
# line 132 "Syntax.y"
{ yyerrok; 
		  yyval = psNULL;
		} break;
case 11:
# line 136 "Syntax.y"
{ yyval = psNULL; } break;
case 12:
# line 138 "Syntax.y"
{ yyerror("syntax: constant definition expected");
		  yyval = psNULL;
		} break;
case 13:
# line 142 "Syntax.y"
{ yyerror("syntax: error in constant definition");
		} break;
case 14:
# line 147 "Syntax.y"
{ if(Topology)
  		    STMT_ValConstant(yypvt[-2], yypvt[-0]);
		  yyval = psNULL; 
		} break;
case 15:
# line 152 "Syntax.y"
{ if(Topology) {
  		    STMT_ValConstant(yypvt[-2], yypvt[-0]);
		    yyval = psNULL; 
		  }
		  yywarning("constant definition operator must be '='");
		} break;
case 16:
# line 159 "Syntax.y"
{ yyerror("syntax: constant identifier expected"); } break;
case 17:
# line 163 "Syntax.y"
{ yyval = yypvt[-1]; } break;
case 18:
# line 165 "Syntax.y"
{ if(Topology)
		    if(yypvt[-1]->ytype != Undef) 
		      if(yypvt[-1]->ytype == FILEVAL &&
			 (yypvt[-1]->yvalue->fileval->FileType == XINT || 
			  yypvt[-1]->yvalue->fileval->FileType == XFLOAT ||
			  yypvt[-1]->yvalue->fileval->FileType == STRING))
			yyval = FI_ReadArrayFile(yypvt[-1]);
		      else if(yypvt[-1]->ytype == FILEVAL && 
			      (yypvt[-1]->yvalue->fileval->FileType == MAP ||
			       yypvt[-1]->yvalue->fileval->FileType == SUBNET))
			yyval = yypvt[-1];
		      else {
			yyval = psNULL;
			printf("******bug: file type: %s\n", STR_TypeString(yypvt[-1]->yvalue->fileval->FileType));
		      }
		    else
		      yyval = psNULL;
		  else
		    yyval = psNULL;
		} break;
case 19:
# line 186 "Syntax.y"
{ yyval = yypvt[-0];
  		  if(Topology) 
		    if( ! yypvt[-0])         
		      yyval = EX_FinishUpMap();    /* map constant defined internally */
		    else {
		      yyval = yypvt[-0];                  /* map constant read from file */
		    }
		  else
		    yyval = psNULL;
		} break;
case 20:
# line 197 "Syntax.y"
{ yyval = yypvt[-1]; } break;
case 21:
# line 202 "Syntax.y"
{ if(Topology && Execute)
  		    yyval = EX_MakeExplicitArray();
		} break;
case 22:
# line 206 "Syntax.y"
{ if(Topology && Execute)
  		    yyval = EX_MakeRangeArray(yypvt[-3], yypvt[-1]);
		} break;
case 23:
# line 213 "Syntax.y"
{ if(Topology)
  		    yyval = yypvt[-1];
		} break;
case 24:
# line 220 "Syntax.y"
{ if(Topology) {
		    yyval = psNULL;
		    /* map type identifiers must be simple types, not array types */
		    EX_InitMap(yypvt[-3], yypvt[-1]);
		  }
		} break;
case 25:
# line 229 "Syntax.y"
{ yyval = psNULL; } break;
case 26:
# line 231 "Syntax.y"
{ if(Topology)
		    yyval = EX_EvalFileMap(yypvt[-0]->ynext->ynext, yypvt[-0], yypvt[-0]->ynext); 
		     /* arguments are (File, domain/range separator, line separator */
		  else
		    yyval = psNULL; 
		} break;
case 27:
# line 241 "Syntax.y"
{ yyval = psNULL; 
		} break;
case 28:
# line 244 "Syntax.y"
{ yyval = psNULL; 
		} break;
case 29:
# line 247 "Syntax.y"
{ yyerror("syntax: map definition line expected");
		} break;
case 30:
# line 250 "Syntax.y"
{ yyerror("syntax: error in map definition (missing ',' ?)");
		  yyerrok;
		  Topology = Functions = FALSE;
		} break;
case 31:
# line 255 "Syntax.y"
{ yyerror("syntax: error in map definition line");
		} break;
case 32:
# line 261 "Syntax.y"
{ if(Topology) {
		    yypvt[-5]->ynext = yypvt[-3];
		    yypvt[-3]->ynext = yypvt[-0];
		    yyval = yypvt[-5];
		  }
		  else
		    yyval = psNULL;
		} break;
case 33:
# line 270 "Syntax.y"
{ if(Topology) {
		    yypvt[-3]->ynext = PA_GetUndefString();
		    yypvt[-3]->ynext->ynext = yypvt[-0];
		    yyval = yypvt[-3];
		  }
		  else
		    yyval = psNULL;
		} break;
case 34:
# line 281 "Syntax.y"
{ if(Topology)
  		    EX_InsertMapLine(yypvt[-2], yypvt[-0]);
		  yyerrok;
		} break;
case 35:
# line 289 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 36:
# line 291 "Syntax.y"
{ if(Topology) {
  		    EX_InitExprList(yypvt[-0]);
		    yyval = EX_MakeExplicitArray();
		  }
		} break;
case 37:
# line 300 "Syntax.y"
{ if(Topology)
  		    yyval = FI_OpenFile(yypvt[-2], yypvt[-0]);
		} break;
case 38:
# line 307 "Syntax.y"
{ if(Topology)
		    yyval = STMT_BuildType(MAP);
		} break;
case 39:
# line 312 "Syntax.y"
{ if(Topology)
		    yyval = STMT_BuildType(SUBNET);
		} break;
case 40:
# line 316 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 41:
# line 323 "Syntax.y"
{ yyval = psNULL;
		} break;
case 42:
# line 326 "Syntax.y"
{ yyval = psNULL;
		} break;
case 43:
# line 332 "Syntax.y"
{ yyerrok; 
		  yyval = psNULL;
		} break;
case 44:
# line 336 "Syntax.y"
{ yyval = psNULL; } break;
case 45:
# line 338 "Syntax.y"
{ yyerror("syntax: type definition expected");
		} break;
case 46:
# line 341 "Syntax.y"
{ yyerror("syntax: error in type definitions (missing ';' ?)");
		} break;
case 47:
# line 346 "Syntax.y"
{ if(Topology) {
		    ST_DefineType(yypvt[-3], yypvt[-1]);
		    yyval = psNULL;
		  }
		} break;
case 48:
# line 352 "Syntax.y"
{ yyerror("syntax: illegal type definition");} break;
case 49:
# line 354 "Syntax.y"
{ yywarning("syntax: missing ':' or ',' in type definition");
		  if(Topology) {
		    ST_DefineType(yypvt[-3], yypvt[-1]);
		    yyval = psNULL;
		  }
		} break;
case 50:
# line 364 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 51:
# line 366 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 52:
# line 371 "Syntax.y"
{ if(Topology) {
		    OUT_SetSNameLength(yypvt[-2]);
		    OUT_SetSiteFuncLength(yypvt[-0]);
		    yyval = SI_MakeSite(yypvt[-2], yypvt[-0]);
		   }
		} break;
case 53:
# line 378 "Syntax.y"
{ if(Topology) {
		    OUT_SetSiteFuncLength(yypvt[-2]);
		    OUT_SetSNameLength(yypvt[-0]);
		    yyval = SI_MakeSite(yypvt[-0], yypvt[-2]);
		  }
		} break;
case 54:
# line 385 "Syntax.y"
{ yywarning("syntax: missing 'with' in site type definition");
		  if(Topology) {
		    OUT_SetSNameLength(yypvt[-2]);
		    OUT_SetSiteFuncLength(yypvt[-0]);
		    yyval = SI_MakeSite(yypvt[-2], yypvt[-0]);
		  }
		} break;
case 55:
# line 393 "Syntax.y"
{ yywarning("syntax: missing 'with' in site type definition");
		  if(Topology) {
		    OUT_SetSiteFuncLength(yypvt[-2]);
		    OUT_SetSNameLength(yypvt[-0]);
		    yyval = SI_MakeSite(yypvt[-0], yypvt[-2]);
		  }
		} break;
case 56:
# line 401 "Syntax.y"
{ yyerror("syntax: name or site function definition expected near 'with'");} break;
case 57:
# line 406 "Syntax.y"
{ if(Topology)
		    yyval = ST_FinishUpUnitType();
		} break;
case 58:
# line 413 "Syntax.y"
{ if(Topology)
		    ST_InitUnitType();
		  yyval = psNULL;
		} break;
case 59:
# line 420 "Syntax.y"
{ if(Topology)	    
		    ST_InsertUnitTypeParam(yypvt[-0]);
		  yyval = psNULL;
		  yyerrok;
		} break;
case 60:
# line 426 "Syntax.y"
{ if(Topology)
		    ST_InsertUnitTypeParam(yypvt[-0]);
		  yyval = psNULL; 
		  yyerrok;
		} break;
case 61:
# line 432 "Syntax.y"
{ yyerror("syntax: error in unit type definition");
		} break;
case 62:
# line 435 "Syntax.y"
{ yyerror("syntax: missing ',' in unit type parameter definition?");
		} break;
case 63:
# line 441 "Syntax.y"
{ if(Topology) {
		    if(yypvt[-0]->ytype != Undef) {
		      yypvt[-0]->ytype = NAME;
		      OUT_SetUNameLength(yypvt[-0]->yvalue->string);
		    }
		    yyval = yypvt[-0]; 
		  }
		} break;
case 64:
# line 450 "Syntax.y"
{ if(Topology) {
		    if(yypvt[-0])
		      yypvt[-0]->ytype = ACT;
		    yyval = yypvt[-0];
		  }
		} break;
case 65:
# line 457 "Syntax.y"
{ if(Topology) {
		    if(yypvt[-0])
		      yypvt[-0]->ytype = SITE;
		    yyval = yypvt[-0];
		  }
		} break;
case 66:
# line 464 "Syntax.y"
{ if(Topology) {
		    if(yypvt[-0]) {
		      yypvt[-0]->ytype = ACTFUNC;
		      OUT_SetActFuncLength(yypvt[-0]);
		    }
		    yyval = yypvt[-0];
		  }
		} break;
case 67:
# line 473 "Syntax.y"
{ if(Topology) {
		    if(yypvt[-0]) {
		      yypvt[-0]->ytype = OUTFUNC;
		      OUT_SetOutFuncLength(yypvt[-0]);
		    }
		    yyval = yypvt[-0];
		  }
		} break;
case 68:
# line 482 "Syntax.y"
{ if(Topology)
		    yyval = yypvt[-0];
		} break;
case 69:
# line 490 "Syntax.y"
{ if(Topology)
		    yyval = ST_CheckString(yypvt[-0], NAME);
		  yyerrok;
		} break;
case 70:
# line 498 "Syntax.y"
{ if(Topology)
		    yyval = ST_CheckFunction(yypvt[-0], ACTFUNC);
		  yyerrok;
		} break;
case 71:
# line 503 "Syntax.y"
{ yyerror("syntax: error in activation function definition");
		} break;
case 72:
# line 508 "Syntax.y"
{ if(Topology)
		    yyval = ST_CheckFunction(yypvt[-0], OUTFUNC);
		  yyerrok;
		} break;
case 73:
# line 513 "Syntax.y"
{ yyerror("syntax: error in output function definition");
		} break;
case 74:
# line 519 "Syntax.y"
{ if(Topology)
		    yyval = ST_CheckFunction(yypvt[-0], SITEFUNC);
		  yyerrok;
		} break;
case 75:
# line 524 "Syntax.y"
{ yyerror("syntax: error in site function definition");
		} break;
case 76:
# line 530 "Syntax.y"
{ if(Topology)
		    yyval = ST_CheckFloat(yypvt[-0], ACT);
		  yyerrok;
		} break;
case 77:
# line 538 "Syntax.y"
{ if(Topology) {
		    if(ST_AssertSite(yypvt[-0])) {
		      ST_SiteNo = 1;
		      yyval = ST_MakeSiteDef(yypvt[-0]);
		    }
		    else
		      yyval = psNULL;
		  }
		} break;
case 78:
# line 548 "Syntax.y"
{ if(Topology)
		    yyval = ST_MakeSiteDef(yypvt[-1]);
		} break;
case 79:
# line 555 "Syntax.y"
{ if(Topology)
		    yyval = ST_InsertSite(yypvt[-2], yypvt[-0]);
		  yyerrok; 
		} break;
case 80:
# line 560 "Syntax.y"
{ if(Topology) 
		    yyval = ST_InsertSite(yypvt[-0], psNULL);
		  yyerrok; 
		} break;
case 81:
# line 565 "Syntax.y"
{ yyerror("syntax: error in site list");
		} break;
case 82:
# line 568 "Syntax.y"
{ yyerror("syntax: illegal end of site list");
		} break;
case 83:
# line 571 "Syntax.y"
{ yyerror("syntax: error in site list (missing ',' ?)");
		  yyerrok;
		} break;
case 84:
# line 575 "Syntax.y"
{ yyerror("syntax: site type expected");
		} break;
case 85:
# line 581 "Syntax.y"
{ if(Topology)
		    yyval = ST_MakeSType(INPUT);
		} break;
case 86:
# line 585 "Syntax.y"
{ if(Topology)
		    yyval = ST_MakeSType(OUTPUT);
		} break;
case 87:
# line 589 "Syntax.y"
{ if(Topology)
		    yyval = ST_MakeSType(InOut);
		} break;
case 88:
# line 593 "Syntax.y"
{ if(Topology)
		    yyval = ST_MakeSType(InOut);
		} break;
case 89:
# line 597 "Syntax.y"
{ if(Topology)
		    yyval = ST_MakeSType(HIDDEN);
		} break;
case 90:
# line 601 "Syntax.y"
{ yyerror("syntax: type 'input', 'output', or 'hidden' expected");
		  Topology = Functions = FALSE;
		} break;
case 91:
# line 612 "Syntax.y"
{ Structures = FALSE;  /* redefinitions of explicit connections are NOT ignored */
		  yyval = psNULL;  
		} break;
case 92:
# line 616 "Syntax.y"
{ Structures = FALSE;  /* redefinitions of explicit connections are NOT ignored */
		  yyval = psNULL;  
		} break;
case 93:
# line 622 "Syntax.y"
{ Structures = TRUE;   /* redefinitions of explicit connections are ignored */
		  yyval = psNULL;
		} break;
case 94:
# line 629 "Syntax.y"
{ yyerrok; 
		  yyval = psNULL;
		} break;
case 95:
# line 633 "Syntax.y"
{ yyval = psNULL; } break;
case 96:
# line 635 "Syntax.y"
{ yyerror("syntax: error in structure definition block");
		} break;
case 97:
# line 638 "Syntax.y"
{ yyerror("syntax: structure definition expected");
		} break;
case 98:
# line 644 "Syntax.y"
{ if(Topology)
		    ST_AssignStructures(yypvt[-3], yypvt[-1]);
		  yyval = psNULL;
		} break;
case 99:
# line 653 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 100:
# line 655 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 101:
# line 657 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 102:
# line 662 "Syntax.y"
{ if(Topology) 
		    yyval = ST_MakeSingleUnit();
		  else
		    yyval = psNULL; 
		} break;
case 103:
# line 668 "Syntax.y"
{ if(Topology) 
		    yyval = ST_MakeSingleUnit();
		  else
		    yyval = psNULL;
		  yyserious("missing structure topology definition");
		} break;
case 104:
# line 678 "Syntax.y"
{ if(Topology) 
		    yyval = ST_MakeArrayOfStructures(yypvt[-0], yypvt[-2], FALSE);
		  else
		    yyval = psNULL; 
		} break;
case 105:
# line 687 "Syntax.y"
{ if(Topology) 
		    yyval = ST_FinishUpStructure();
		  else
		    yyval = psNULL; 
		} break;
case 106:
# line 693 "Syntax.y"
{ if(Topology) 
		    yyval = ST_FinishUpSubnet(yypvt[-0]); 
		  else
		    yyval = psNULL; 
		} break;
case 107:
# line 699 "Syntax.y"
{ if(Topology) 
		    yyval = ST_FinishUpStructure(); 
		  else
		    yyval = psNULL; 
		} break;
case 108:
# line 708 "Syntax.y"
{ yyval = psNULL;
		  if(yypvt[-3])
		    yywarning("illegal connection weight in cluster (ignored)");
		  if(yypvt[-2])
		    yywarning("illegal connection site in cluster (ignored)");
		  if(yypvt[-4])
		    yywarning("illegal connection direction in cluster (ignored)");
		  if(yypvt[-1])
		    yywarning("illegal center definition in cluster (ignored)");
		  if(yypvt[-0])
		    yywarning("illegal connection map in cluster (ignored)");
		} break;
case 109:
# line 721 "Syntax.y"
{ yyval = psNULL;
		  yyserious("missing structure topology definition");
		} break;
case 110:
# line 728 "Syntax.y"
{ yyval = PA_GetParserStack();
		  if(Topology)
		    yyval->ytype = PA_Parser(yypvt[-7], yypvt[-0], yypvt[-4], yypvt[-2]);
		  else 
		    yyval->ytype = Undef;
		} break;
case 111:
# line 736 "Syntax.y"
{ yyval = PA_GetParserStack();
		  if(Topology)
		    yyval->ytype = PA_Parser(yypvt[-1], yypvt[-0], PA_GetUndefValue(), PA_GetUndefValue());
		  else 
		    yyval->ytype = Undef;
		  yyserious("missing structure topology definition");
		} break;
case 112:
# line 748 "Syntax.y"
{ if(Topology) {
		    ST_InsertWeight(yypvt[-3]);
		    ST_InsertMap(yypvt[-0], ST_TempStruct);
		  }
		  yyval = psNULL;
		} break;
case 113:
# line 755 "Syntax.y"
{ if(Topology) {
		    ST_InsertWeight(yypvt[-3]);
		    ST_InsertMap(yypvt[-0], ST_TempStruct);
		  }
		  yyserious("missing structure topology definition");
		  yyval = psNULL;
		} break;
case 114:
# line 768 "Syntax.y"
{ if(Topology) 
		    ST_InitUnitValue(CHAIN);
		  yyval = psNULL;
		} break;
case 115:
# line 773 "Syntax.y"
{ if(Topology) 
		    ST_InitUnitValue(RING);
		  yyval = psNULL;
		} break;
case 116:
# line 778 "Syntax.y"
{ if(Topology) 
		    ST_InitUnitValue(CLIQUE);
		  yyval = psNULL;
		} break;
case 117:
# line 783 "Syntax.y"
{ if(Topology) 
		    ST_InitUnitValue(STAR);
		  yyval = psNULL;
		} break;
case 118:
# line 788 "Syntax.y"
{ if(Topology) 
		    ST_InitUnitValue(IRREGULAR);
		  yyval = psNULL;
		} break;
case 119:
# line 795 "Syntax.y"
{ if(Topology) 
		    ST_InitUnitValue(UNIT);
		  yyval = psNULL;
		} break;
case 120:
# line 802 "Syntax.y"
{ if(Topology) 
		    ST_InitUnitValue(CLUSTER);
		  yyval = psNULL;
		} break;
case 121:
# line 812 "Syntax.y"
{ if(Topology) 
		    ST_InsertStructSize(yypvt[-0]);
		  yyval = psNULL;
		} break;
case 122:
# line 825 "Syntax.y"
{ yyval = psNULL; } break;
case 123:
# line 831 "Syntax.y"
{ if(Topology) { 
		    ST_InitStructFType(yypvt[-0]);	
		  }
		  yyval = psNULL;
		} break;
case 124:
# line 837 "Syntax.y"
{ if(Topology) 
		    ST_InitStructTypeExpl();
		  yyval = psNULL;
		} break;
case 125:
# line 842 "Syntax.y"
{ yyerror("syntax: unit type specification expected");
		} break;
case 126:
# line 845 "Syntax.y"
{ yyerror("illegal symbol: unit type or 'unit' expected");
		} break;
case 127:
# line 854 "Syntax.y"
{ yyval = psNULL; } break;
case 128:
# line 856 "Syntax.y"
{ yyval = psNULL; } break;
case 129:
# line 861 "Syntax.y"
{ yyerrok;
		  yyval = psNULL;
		} break;
case 130:
# line 865 "Syntax.y"
{ yyval = psNULL; } break;
case 131:
# line 867 "Syntax.y"
{ yyerror("syntax: error in unit default specification");
		} break;
case 132:
# line 870 "Syntax.y"
{ yyerror("syntax: unit default specification expected");
		} break;
case 133:
# line 876 "Syntax.y"
{ if(Topology) { 
		    ST_InsertStructName(yypvt[-0]);
		    if(yypvt[-0]->ytype != Undef)
		      OUT_SetUNameLength(yypvt[-0]->yvalue->string);
		  }
		  yyval = psNULL;
		} break;
case 134:
# line 884 "Syntax.y"
{ if(Topology) 
		    ST_InsertStructAct(yypvt[-0]);
		  yyval = psNULL;
		} break;
case 135:
# line 889 "Syntax.y"
{ if(Topology) 
		    ST_InsertStructSites(yypvt[-0]);
		  yyval = psNULL;
		} break;
case 136:
# line 894 "Syntax.y"
{ if(Topology) {
		    OUT_SetActFuncLength(yypvt[-0]);
		    ST_InsertStructFunction(yypvt[-0], ACTFUNC);
		  }
		  yyval = psNULL;
		} break;
case 137:
# line 901 "Syntax.y"
{ if(Topology) {
		    OUT_SetOutFuncLength(yypvt[-0]);
		    ST_InsertStructFunction(yypvt[-0], OUTFUNC);
		  }
		  yyval = psNULL;
		} break;
case 138:
# line 908 "Syntax.y"
{ if(Topology) 
		    ST_InsertStructSType(yypvt[-0]);
		  yyval = psNULL;
		} break;
case 139:
# line 917 "Syntax.y"
{ yyval = psNULL; } break;
case 140:
# line 919 "Syntax.y"
{ yyval = psNULL; } break;
case 141:
# line 923 "Syntax.y"
{ if(Topology) 
		    ST_InitExplUnits(ST_TempStruct);
		  yyval = psNULL;
		} break;
case 142:
# line 931 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 143:
# line 933 "Syntax.y"
{ yyval =  psNULL; } break;
case 144:
# line 938 "Syntax.y"
{ yyerrok; 
		  yyval = psNULL;
		} break;
case 145:
# line 942 "Syntax.y"
{ yyval = psNULL; } break;
case 146:
# line 944 "Syntax.y"
{ yyerror("syntax: 'get <prm> from <array>' expected");
		} break;
case 147:
# line 947 "Syntax.y"
{ yyerror("syntax: after 'get'");
		} break;
case 148:
# line 950 "Syntax.y"
{ yyerror("syntax: after 'get' (missing ',' ?)");
		  yyerrok;
		} break;
case 149:
# line 954 "Syntax.y"
{ yyerror("syntax: '<prm> from <array>' expected");
		} break;
case 150:
# line 960 "Syntax.y"
{ if(Topology) 
		    ST_InsertExplValues(NAME, yypvt[-0]);
		  yyval = psNULL;
		} break;
case 151:
# line 965 "Syntax.y"
{ if(Topology) 
		    ST_InsertExplValues(ACT, yypvt[-0]);
		  yyval = psNULL;
		} break;
case 152:
# line 976 "Syntax.y"
{ yyval = psNULL; } break;
case 153:
# line 978 "Syntax.y"
{ yyval = psNULL; } break;
case 154:
# line 983 "Syntax.y"
{ if(Topology) 
		    ST_InsertTopology(MATRIX, ST_MakePos(1), ST_MakePos(ST_TempStruct->Length));
		  yyval = psNULL;
		} break;
case 155:
# line 988 "Syntax.y"
{ if(Topology) 
		    ST_InsertTopology(MATRIX, ST_MakePos(ST_TempStruct->Length), ST_MakePos(1));
		  yyval = psNULL;
		} break;
case 156:
# line 993 "Syntax.y"
{ if(Topology) 
		    ST_InsertTopology(MATRIX, ST_MakePos(ST_TempStruct->Length), ST_MakePos(1));
		  yyval = psNULL;
		} break;
case 157:
# line 998 "Syntax.y"
{ if(Topology) {
		    yywarning("illegal plane orientation definition (using '-')");
		    ST_InsertTopology(MATRIX, ST_MakePos(ST_TempStruct->Length), ST_MakePos(1));
		  }
		  yyval = psNULL;
		} break;
case 158:
# line 1008 "Syntax.y"
{ if(Topology) 
		    ST_InsertTopology(MATRIX, yypvt[-3], yypvt[-1]);
		  yyval = psNULL;
		} break;
case 159:
# line 1013 "Syntax.y"
{ if(Topology) 
		    ST_InsertTopology(ELLIPSE, yypvt[-3], yypvt[-1]);
		  yyval = psNULL;
		} break;
case 160:
# line 1020 "Syntax.y"
{ if(Topology) 
		    ST_InsertTopCenter(yypvt[-3], yypvt[-1]);
		  yyval = psNULL;
		} break;
case 161:
# line 1031 "Syntax.y"
{ if(Topology)
		    yyval = ST_CheckFloat(yypvt[-0], BY);
		} break;
case 162:
# line 1035 "Syntax.y"
{ yyval = psNULL; } break;
case 163:
# line 1042 "Syntax.y"
{ if(Topology)
		    ST_InsertThrough(yypvt[-0]);
		  yyval = psNULL;
		} break;
case 164:
# line 1047 "Syntax.y"
{ yyerror("syntax: site type expected");
		} break;
case 165:
# line 1050 "Syntax.y"
{ if(Topology)
		    ST_InsertThrough(psNULL);
		  yyval = psNULL;
		} break;
case 166:
# line 1060 "Syntax.y"
{ if(Topology) {
		    ST_InsertStructDirection(PT);
		    yyval = psNULL;
		  }
		} break;
case 167:
# line 1066 "Syntax.y"
{ if(Topology) {
		    ST_InsertStructDirection(PF);
		    yyval = psNULL;
		  }
		} break;
case 168:
# line 1072 "Syntax.y"
{ if(Topology) {
		    ST_InsertStructDirection(PTF);
		    yyval = psNULL;
		  }
		} break;
case 170:
# line 1084 "Syntax.y"
{ if(Topology) {
		    ST_InsertStructCenter(EX_CorrectSize(yypvt[-0]));
		    yyval = psNULL;
		  }
		} break;
case 171:
# line 1090 "Syntax.y"
{ yyval = psNULL; } break;
case 172:
# line 1096 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 173:
# line 1098 "Syntax.y"
{ yyval = psNULL; } break;
case 174:
# line 1106 "Syntax.y"
{ RightSide = FALSE;
                  ConstantSection = FALSE;
		  yyval = psNULL; } break;
case 175:
# line 1110 "Syntax.y"
{ RightSide = FALSE;
		  yyval = psNULL; } break;
case 176:
# line 1116 "Syntax.y"
{ yyval = psNULL;
		  yyerrok; 
		} break;
case 177:
# line 1120 "Syntax.y"
{ yyval = psNULL; } break;
case 178:
# line 1122 "Syntax.y"
{ yyerror("syntax: error in variable declarations (missing ';' ?)");
		} break;
case 179:
# line 1125 "Syntax.y"
{ yyerror("syntax: variable declaration or 'begin' expected");
		} break;
case 180:
# line 1130 "Syntax.y"
{ if(Topology)
		    STMT_DeclareVar(yypvt[-3], yypvt[-1]);
		  yyval = psNULL;
		} break;
case 181:
# line 1135 "Syntax.y"
{ yyerror("syntax: illegal variable type");} break;
case 182:
# line 1140 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 183:
# line 1142 "Syntax.y"
{ if(Topology)
		    yyval = STMT_BuildArrayType(yypvt[-0]);
		} break;
case 184:
# line 1149 "Syntax.y"
{ if(Topology)
		    yyval = STMT_BuildType(UNIT);
		} break;
case 185:
# line 1153 "Syntax.y"
{ if(Topology)
		    yyval = STMT_BuildType(STRUCTURE);
		} break;
case 186:
# line 1157 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 187:
# line 1165 "Syntax.y"
{ yyval = psNULL; } break;
case 188:
# line 1170 "Syntax.y"
{ yyval = psNULL;
		  yyerrok; 
		} break;
case 189:
# line 1174 "Syntax.y"
{ yyval = psNULL;
		  /* abortloop generates an extra  ';' before end token */
		  /* FOREACH loop: give parser an additional ';' to separate FOREACH */
		  /* and some loop variable (on stack, further iterations) */
		  if( ! EvalLoopMode) 
                    yywarning("illegal ';' ignored");
		  yyerrok; 
		} break;
case 190:
# line 1183 "Syntax.y"
{ yyval = psNULL; } break;
case 191:
# line 1185 "Syntax.y"
{ yyval = psNULL; } break;
case 192:
# line 1190 "Syntax.y"
{ RightSide = FALSE; 
		  yyval = psNULL; 
		} break;
case 193:
# line 1194 "Syntax.y"
{ yyval = psNULL; } break;
case 194:
# line 1196 "Syntax.y"
{ yyval = psNULL;	
		  if( ! StackMode) 
		    Execute = TRUE;     /* time delay, execute after reduction of expression */
		} break;
case 195:
# line 1201 "Syntax.y"
{ yyval = psNULL;
		  if( ! StackMode) 
		    Execute = TRUE;     /* time delay, execute after reduction of expression */
		} break;
case 196:
# line 1209 "Syntax.y"
{ if(Topology && Execute)               /* only if there's not been a fatal error */
		      STMT_AssignValue(yypvt[-1], yypvt[-0]);         /* normal processing: Execute statement */
		  yyval = psNULL;
		} break;
case 197:
# line 1214 "Syntax.y"
{ if(Topology && Execute)
		    STMT_AssignValue(yypvt[-1], yypvt[-0]);
		  yyval = psNULL;
		} break;
case 198:
# line 1222 "Syntax.y"
{ yyval = yypvt[-1];
		  RightSide = TRUE; 
		} break;
case 199:
# line 1226 "Syntax.y"
{ yyval = yypvt[-1];
		  RightSide = TRUE;
		  if( ! EvalLoopMode)
		    yywarning("assignment  operator must be ':='"); 
		} break;
case 200:
# line 1235 "Syntax.y"
{ if(Topology && Execute) {
		    STMT_GetIdValue(yypvt[-3]);
		    STMT_GetIdValue(yypvt[-1]);
		    STMT_MakeExplicitConnection(yypvt[-3], yypvt[-1], yypvt[-0], TRUE);
		    PA_FreeParserStack(yypvt[-3]);
		    PA_FreeParserStack(yypvt[-1]);
		    if(yypvt[-0]) PA_FreeParserStack(yypvt[-0]);
		  }
		  yyval = psNULL;
		} break;
case 201:
# line 1246 "Syntax.y"
{ if(Topology && Execute) {
		    STMT_GetIdValue(yypvt[-3]);
		    STMT_GetIdValue(yypvt[-1]);
		    STMT_MakeExplicitConnection(yypvt[-3], yypvt[-1], yypvt[-0], FALSE);
		    STMT_MakeExplicitConnection(yypvt[-1], yypvt[-3], yypvt[-0], FALSE);
		    PA_FreeParserStack(yypvt[-3]);
		    PA_FreeParserStack(yypvt[-1]);
		    if(yypvt[-0]) PA_FreeParserStack(yypvt[-0]);
		  }
		  yyval = psNULL;
		} break;
case 202:
# line 1258 "Syntax.y"
{ yyerror("syntax: '->' or '<->' expected in connection definition");
		} break;
case 203:
# line 1264 "Syntax.y"
{ if(Topology && Execute) {
		    ST_CheckFloat(yypvt[-0], BY);
		    yyval = yypvt[-0];
		  }
		  else
		    yyval = psNULL;
		} break;
case 204:
# line 1272 "Syntax.y"
{ yyval = psNULL; } break;
case 205:
# line 1277 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 206:
# line 1279 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 210:
# line 1293 "Syntax.y"
{ if(Topology && Execute)
		    LOOP_AssignCondValue(yypvt[-0]);
		  yyval = psNULL;
		} break;
case 211:
# line 1301 "Syntax.y"
{ yyval = psNULL; } break;
case 212:
# line 1306 "Syntax.y"
{ if(Topology && Execute) { 
		    RightSide = FALSE;
		    LOOP_InitForLoop(yypvt[-3], yypvt[-2]);       /* initialize and lock loop variable */
		    LOOP_CalcForIterations(yypvt[-0]);     /* insert no. of interations into loop header */
		  }
		  yyval = psNULL;
		} break;
case 213:
# line 1316 "Syntax.y"
{ yyval = psNULL; } break;
case 214:
# line 1321 "Syntax.y"
{ if(Topology && Execute) { 
		    RightSide = FALSE;
		    LOOP_InitForeachLoop(yypvt[-1], yypvt[-1]);      /* initialize and lock loop variable */
		  }
		  yyval = psNULL;
		} break;
case 216:
# line 1335 "Syntax.y"
{ yyerrok; } break;
case 218:
# line 1338 "Syntax.y"
{ yyerror("syntax: illegal condition list in 'foreach' - statement");
		} break;
case 219:
# line 1341 "Syntax.y"
{ yyerror("syntax: illegal end of condition list in 'foreach' - statement");
		} break;
case 220:
# line 1344 "Syntax.y"
{ yyerror("syntax: error in 'foreach' - condition list (missing ',' ?)");
		  yyerrok;
		} break;
case 221:
# line 1348 "Syntax.y"
{ yyerror("syntax: condition expected");
		} break;
case 222:
# line 1354 "Syntax.y"
{ if(Topology &&  Execute) {
		    LOOP_CheckLoopVar(yypvt[-2], yypvt[-0]);
		    LOOP_InsertLoopVar(yypvt[-2], yypvt[-0]);
		  }
		  yyval = psNULL;
		} break;
case 223:
# line 1361 "Syntax.y"
{ yyerror("syntax: loop variable definition expected");
		} break;
case 224:
# line 1370 "Syntax.y"
{ if(Topology &&  Execute) 
		    LOOP_AssignCondValue(yypvt[-0]);
                } break;
case 227:
# line 1381 "Syntax.y"
{ yyval = psNULL; } break;
case 228:
# line 1386 "Syntax.y"
{ yyerrok; } break;
case 229:
# line 1388 "Syntax.y"
{yyerror("syntax: illegal ELSEIF branch");} break;
case 231:
# line 1391 "Syntax.y"
{yyerror("syntax: illegal ELSEIF statements");} break;
case 233:
# line 1396 "Syntax.y"
{yyerror("syntax: illegal ELSEIF condition");} break;
case 234:
# line 1401 "Syntax.y"
{ if(Topology &&  Execute)   
		    LOOP_AssignCondValue(yypvt[-0]);
                } break;
case 236:
# line 1409 "Syntax.y"
{yyerror("syntax: illegal ELSE part");} break;
case 237:
# line 1411 "Syntax.y"
{ yyval = psNULL; } break;
case 238:
# line 1421 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_EvalNumExpr(PLUS, yypvt[-2], yypvt[-0]); 
		} break;
case 239:
# line 1425 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_EvalNumExpr(MINUS, yypvt[-2], yypvt[-0]); 
		} break;
case 240:
# line 1429 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_EvalNumExpr(MULT, yypvt[-2], yypvt[-0]); 
		} break;
case 241:
# line 1433 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_EvalNumExpr(RDIV, yypvt[-2], yypvt[-0]); 
		} break;
case 242:
# line 1437 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_EvalIntExpr(DIV, yypvt[-2], yypvt[-0]); 
		} break;
case 243:
# line 1441 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_EvalIntExpr(MOD, yypvt[-2], yypvt[-0]); 
		} break;
case 244:
# line 1445 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_EvalStringExpr(XSTRCAT, yypvt[-2], yypvt[-0]); 
		} break;
case 245:
# line 1449 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_EvalSign(MINUS, yypvt[-0]); 
		} break;
case 246:
# line 1453 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_EvalSign(PLUS, yypvt[-0]); 
		} break;
case 247:
# line 1457 "Syntax.y"
{ if(Topology && Execute)
		    yyval = yypvt[-1]; 
		} break;
case 248:
# line 1461 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 249:
# line 1463 "Syntax.y"
{ if(Topology && Execute) {
		    EX_AssertValue(yypvt[-0]);
		    InnerSubnet = NULL;     /* access to a subnet through "." ends here */
		    yyval = yypvt[-0];
		  }
		} break;
case 250:
# line 1470 "Syntax.y"
{ yyerror("syntax: expression expected");} break;
case 251:
# line 1475 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 252:
# line 1477 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 253:
# line 1479 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 254:
# line 1481 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 255:
# line 1486 "Syntax.y"
{ if(Topology && Execute)
		    yyval = STMT_GetElement(yypvt[-1], yypvt[-0]); 
		} break;
case 256:
# line 1493 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 257:
# line 1495 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 258:
# line 1500 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_ArctanFunc(yypvt[-1]); 
		} break;
case 259:
# line 1504 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_CosFunc(yypvt[-1]);
		} break;
case 260:
# line 1508 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_SinFunc(yypvt[-1]);
		} break;
case 261:
# line 1512 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_ExpFunc(yypvt[-1]);
		} break;
case 262:
# line 1516 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_LnFunc(yypvt[-1]); 
		} break;
case 263:
# line 1520 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_SqrFunc(yypvt[-1]);
		} break;
case 264:
# line 1524 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_SqrtFunc(yypvt[-1]); 
		} break;
case 265:
# line 1531 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_ListFunc(XMIN); 
		} break;
case 266:
# line 1535 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_ListFunc(XMAX);
		} break;
case 267:
# line 1542 "Syntax.y"
{ if(Topology && Execute)
		    yyval = STMT_GetUnitComponent(yypvt[-2], ACT); 
		} break;
case 268:
# line 1546 "Syntax.y"
{ if(Topology && Execute)
		    yyval = STMT_GetUnitComponent(yypvt[-2], IOTYPE); 
		} break;
case 269:
# line 1550 "Syntax.y"
{ if(Topology && Execute)
		    yyval = STMT_GetUnitComponent(yypvt[-2], NAME); 
		} break;
case 270:
# line 1554 "Syntax.y"
{ if(Topology && Execute)
		    yyval = STMT_SelectIdentifierComponent(yypvt[-2], yypvt[-0]); 
		} break;
case 271:
# line 1558 "Syntax.y"
{ yyerror("syntax: error near selection operator - unit component expected");
		} break;
case 272:
# line 1564 "Syntax.y"
{ yyval = PA_GetParserStack(); yyval->ytype = INPUT;  } break;
case 273:
# line 1566 "Syntax.y"
{ yyval = PA_GetParserStack(); yyval->ytype = OUTPUT;  } break;
case 274:
# line 1568 "Syntax.y"
{ yyval = PA_GetParserStack(); yyval->ytype = HIDDEN;  } break;
case 275:
# line 1570 "Syntax.y"
{ yyval = PA_GetParserStack(); yyval->ytype = InOut;  } break;
case 276:
# line 1572 "Syntax.y"
{ yyval = PA_GetParserStack(); yyval->ytype = InOut;  } break;
case 277:
# line 1578 "Syntax.y"
{ if(Topology && Execute) {
		    yyval = LOG_EvalLogicalExpr(AND, yypvt[-2], yypvt[-0]);
		  }
		  else
		    yyval = psNULL;
		} break;
case 278:
# line 1585 "Syntax.y"
{ if(Topology && Execute) {
		    yyval = LOG_EvalLogicalExpr(OR, yypvt[-2], yypvt[-0]);
		  }
		  else
		    yyval = psNULL;
		} break;
case 279:
# line 1592 "Syntax.y"
{ if(Topology && Execute) {
		    yyval = LOG_EvalLogicalNot(yypvt[-0]);
		  }
		  else
		    yyval = psNULL;
		} break;
case 280:
# line 1599 "Syntax.y"
{ if(Topology && Execute) {
		    yyval = yypvt[-1];
		  }
		  else
		    yyval = psNULL;
		} break;
case 281:
# line 1606 "Syntax.y"
{ if(Topology && Execute) {
		    yyval =  yypvt[-0];
		  }
		  else
		    yyval = psNULL;
		} break;
case 282:
# line 1618 "Syntax.y"
{ if(Topology && Execute)
		    yyval = LOG_EvalRelation(GT, yypvt[-2], yypvt[-0]);
		  else
		    yyval = psNULL;
		} break;
case 283:
# line 1624 "Syntax.y"
{ if(Topology && Execute)
		    yyval = LOG_EvalRelation(GE, yypvt[-2], yypvt[-0]);
		  else
		    yyval = psNULL;
		} break;
case 284:
# line 1630 "Syntax.y"
{ if(Topology && Execute)
		    yyval = LOG_EvalRelation(EQ, yypvt[-2], yypvt[-0]);
		  else
		    yyval = psNULL;
		} break;
case 285:
# line 1636 "Syntax.y"
{ if(Topology && Execute)
		    yyval = LOG_EvalRelation(NE, yypvt[-2], yypvt[-0]);
		  else
		    yyval = psNULL;
		} break;
case 286:
# line 1642 "Syntax.y"
{ if(Topology && Execute)
		    yyval = LOG_EvalRelation(LT, yypvt[-2], yypvt[-0]);
		  else
		    yyval = psNULL;
		} break;
case 287:
# line 1648 "Syntax.y"
{ if(Topology && Execute)
		    yyval = LOG_EvalRelation(LE, yypvt[-2], yypvt[-0]);
		  else
		    yyval = psNULL;
		} break;
case 288:
# line 1654 "Syntax.y"
{ if(Topology && Execute)
		    yyval = LOG_EvalInclusion(yypvt[-2], yypvt[-0]);
		  else
		    yyval = psNULL;
		} break;
case 289:
# line 1660 "Syntax.y"
{ if(Topology && Execute)
		    yyval = LOG_EvalMapRelation(yypvt[-5], yypvt[-3], yypvt[-0]);
		  else
		    yyval = psNULL;
		} break;
case 290:
# line 1673 "Syntax.y"
{ if(Topology && Execute)
		    yyval = STMT_BuildType(XINT);
		} break;
case 291:
# line 1677 "Syntax.y"
{ if(Topology && Execute)
		    yyval = STMT_BuildType(XFLOAT);
		} break;
case 292:
# line 1681 "Syntax.y"
{ if(Topology && Execute)
		    yyval = STMT_BuildType(STRING);
		} break;
case 293:
# line 1688 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 294:
# line 1690 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 295:
# line 1692 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 296:
# line 1694 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 297:
# line 1696 "Syntax.y"
{ yyerror("illegal symbol: constant or variable expected");
		} break;
case 301:
# line 1708 "Syntax.y"
{ if(Topology)
		    yyval = PA_StackPrm(yypvt[-2], yypvt[-0]);
		  yyerrok; 
		} break;
case 302:
# line 1713 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 303:
# line 1715 "Syntax.y"
{ yyerror("syntax: illegal end of parameter list");
		} break;
case 304:
# line 1718 "Syntax.y"
{ yyerror("syntax: error in parameter list (missing ',' ?)");
		  yyerrok;
		} break;
case 305:
# line 1722 "Syntax.y"
{ yyerror("syntax: default (!) parameter definition expected");
		} break;
case 306:
# line 1728 "Syntax.y"
{ if(Topology) {
		    PA_UpdateParameter(yypvt[-2]->yvalue->var, yypvt[-0]);
		    yyval = yypvt[-2];
		  }
		  else
		    yyval = psNULL;
		} break;
case 307:
# line 1736 "Syntax.y"
{ if(Topology) {
		    PA_UpdateParameter(yypvt[-2]->yvalue->var, yypvt[-0]);
		    yyval = yypvt[-2];
		  }
		  else
		    yyval = psNULL;
		  yywarning("operator for parameter default definition must be ':='"); 
		} break;
case 308:
# line 1748 "Syntax.y"
{ if(Topology) {
		    yyval = PA_StackPrm(yypvt[-2], yypvt[-0]);
		  }
		  yyerrok; 
		} break;
case 309:
# line 1754 "Syntax.y"
{ yyval = yypvt[-0]; } break;
case 310:
# line 1756 "Syntax.y"
{ yyerror("syntax: illegal identifier list");
		} break;
case 311:
# line 1759 "Syntax.y"
{ yyerror("syntax: illegal end of identifier list");
		} break;
case 312:
# line 1762 "Syntax.y"
{ yyerror("syntax: error in identifier list (missing ',' ?)");
		  yyerrok;
		} break;
case 313:
# line 1766 "Syntax.y"
{ yyerror("syntax: identifier expected");
		} break;
case 314:
# line 1772 "Syntax.y"
{ if(Topology && Execute)
		    yyval = EX_CorrectSize(yypvt[-1]);
		} break;
case 315:
# line 1779 "Syntax.y"
{ if(Topology && Execute)
		    yyval = ST_BuildRangeSelection(yypvt[-5], EX_CorrectSize(yypvt[-3]), EX_CorrectSize(yypvt[-1]));
		  else
	 	    yyval = psNULL;
		} break;
case 316:
# line 1785 "Syntax.y"
{ if(Topology && Execute) {
		    yyval = yypvt[-0];
		  }
		  else
	 	    yyval = psNULL;
		} break;
case 317:
# line 1795 "Syntax.y"
{ if(Topology) 
		    yyval = yypvt[-1];
		  else
		    yyval = psNULL;
		} break;
case 318:
# line 1801 "Syntax.y"
{ if(Topology) 
		    yyval = yypvt[-0];
		  else
		    yyval = psNULL;
		} break;
case 319:
# line 1810 "Syntax.y"
{ if(Topology) {
		    yyval = PA_StackPrm(yypvt[-2], yypvt[-0]);
		  }
		  yyerrok; 
		} break;
case 320:
# line 1816 "Syntax.y"
{ if(Topology) 
		    yyval = yypvt[-0];
		  else
		    yyval = psNULL;
		} break;
case 321:
# line 1822 "Syntax.y"
{ yyerror("syntax: error in subnet parameter list (missing ',' ?)");
		  yyerrok;
		} break;
case 322:
# line 1826 "Syntax.y"
{ yyerror("syntax: illegal end of subnet parameter list");
		} break;
case 323:
# line 1833 "Syntax.y"
{ if(Topology && Execute)
		    EX_AppendExprList(yypvt[-0]);
	 	  yyval = psNULL;
		  yyerrok;
		} break;
case 324:
# line 1839 "Syntax.y"
{ if(Topology && Execute)
		    EX_InitExprList(yypvt[-0]);
	 	  yyval = psNULL;
		} break;
case 325:
# line 1844 "Syntax.y"
{ yyerror("syntax: error in expression list (missing ',' ?)"); } break;
case 328:
# line 1855 "Syntax.y"
{ yyval = psNULL; } break;
case 329:
# line 1859 "Syntax.y"
{ if(Topology && Execute) {
		    yyval = PA_GetParserStack();
		    yyval->ytype = RANDOM; 
		  }
		} break;
case 330:
# line 1867 "Syntax.y"
{
#ifndef NOACTIONS
		  Topology = TRUE;
		  RightSide = TRUE;
		  ConstantSection = TRUE;
#endif
		  yyval = psNULL; 	
		} break;
case 331:
# line 1878 "Syntax.y"
{ yyval = psNULL;
  		  yyerrok; 
		} break;
case 332:
# line 1883 "Syntax.y"
{ yyval = psNULL; 
		} break;
case 333:
# line 1887 "Syntax.y"
{ yyval = psNULL; } break;
case 334:
# line 1890 "Syntax.y"
{ yyval = psNULL;
                  yyerrok; 
		} break;
case 335:
# line 1895 "Syntax.y"
{ yyval = psNULL;
  		  yyerrok; 
		} break;
case 336:
# line 1900 "Syntax.y"
{ yyval = psNULL; } break;
case 337:
# line 1903 "Syntax.y"
{ yyval = psNULL;
  		  yyerrok; 
		} break;
case 338:
# line 1908 "Syntax.y"
{ yyval = psNULL; } break;
case 339:
# line 1911 "Syntax.y"
{ yyval = psNULL;
  		  yyerrok; 
		} break;
case 340:
# line 1918 "Syntax.y"
{ yyval = psNULL; } break;
	}
	goto yystack;		/* reset registers in driver code */
}
