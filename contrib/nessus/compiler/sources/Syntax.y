/*****************************************************************************
  FILE              : Syntax.y
  SHORTNAME         : Syntax
  VERSION           : 2.0

  PURPOSE           : contains the yacc syntax specification of nessus
  NOTES             : 

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

%{
#include "co_parser.h"             /* contains macro definitions and text substitutions */

#define YYSTYPE ParserStackPType                      /* data type for parser value stack */

static short PA_StackTokenFlg = FALSE;    /* true for first scan of a FOR (FOREACH)  loop */
static short PA_Loop = FALSE;                /* true for repetitive pseudo-scans of loops */

static short PA_Nested = 0;                                         /* counts nested [] ! */
static short PA_ReadEnable = FALSE;      /* true <=> read is an expression (proced. part) */

short ConstantSection;/* true <=> parsing constant, types, structures, or variable decls. */

%}

/**** list of tokens ****/

%token IDENTIFIER 	    				/* identifier */
%token XINT XFLOAT STRING UNIT SITE ARRAY			/* data types */
%token INTEGER LONGSTRING SIMPLESTRING	FLOATNUM	/* constant tokens */
%token INPUT OUTPUT HIDDEN				/* unit structure types */
%token NETWORK						/* header keyword */
%token CONST TYPEDEF VAR    	            		/* section keywords */
%token MAP XFILE 					/* in constant definition */
%token ACT IOTYPE NAME 	 			       	/* unit components */
%token SITEFUNC ACTFUNC OUTFUNC 			/* function types */
/* 21 */
%token CLIQUE STAR IRREGULAR CHAIN RING         	/* structures with connection definition */
%token CLUSTER SUBNET					/* structures without connection definition */
%token THROUGH BY OF WITH GET FROM CENTER 	 	/* structure definition keywords */
%token FOR TO DO FOREACH IN IF THEN ELSE ELSEIF 	/* flow of control keywords */
/* 44 */
%token WHILE XBEGIN END					/* procedural block delimiters */
%token UMINUS PLUS MINUS MULT RDIV DIV MOD 		/* numerical operators */
%token XARCTAN XCOS XSIN XEXP XLN XSQR XSQRT XMIN XMAX  /* numerical functions */
%token XSTRCAT  					/* string operators and function */
%token AND OR NOT 					/* logical operators and functions */
/* 61 */
%token GT GE EQ NE LT LE				/* relational operators */
%token PT PTF PF					/* boolean network operators */
%token POINT PERIOD SEMICOLON COLON			/* literals, separators */
%token OPARENT CPARENT OBRACKET CBRACKET		/* parentheses, brackets */
%token ASSIGN ARANGE ILLEGAL	     		   	/* other operators  and illegal symbol */
%token RANDOM 
%token AT PLANE MATRIX ELLIPSE                          /* keywords for structure topology definition */
%token STRUCTURE

/**** precedence relations and associativity ****/
%left OR                                                /* logical operators */
%left AND
%left NOT
%nonassoc GT GE EQ NE LT LE                             /* relational operators */
%left PLUS MINUS                                        /* arithmetic operators */
%left MULT RDIV DIV MOD
%left XSTRCAT		                                /* string operators */
%left UMINUS                                            /* sign (not a real token) */
%left ILLEGAL                                           /* illegal input symbol */


%%



/********** syntax specification for xxx **********/

program	: topology_block pt        
		{ $$ = psNULL; 
		  ER_ListErrors(yylineno, SymTabPtr->FileStr);}
	| error
  		{ yyerror("cannot continue parsing due to errors");
		  ER_ListErrors(yylineno, SymTabPtr->FileStr);
		  return(TRUE);  	/* give up */ 
		}
	| empty
  		{ yyerror("empty nessus program");
		  ER_ListErrors(yylineno, SC_InputStack->InFileName);
		  return(TRUE);  	/* give up */ 
		}
	;

topology_block
	: top_header const_block type_block struct_block var_block progr_block
	| top_header const_block type_block struct_block var_block error progr_block
		{ yyerror("syntax: illegal beginning of new section, 'begin' missing");
		}
	;

/**** program header ****/
top_header
	: network IDENTIFIER op parameter_list cp sc
		{ if(Topology) {
		    SYM_InsertNetName($2);
		    OUT_InsertNetName($2);
		    PA_AssignParamVal($4, PA_ProgramArguments);
		  }
		  $$ = psNULL; 
		}
	| network error op parameter_list cp sc
		{ yyerror("syntax: missing network identifier"); }
	;

/**** constant definitions ****/
const_block
	: CONST cons_def_list
		{ $$ = psNULL; }
	| empty
		{ $$ = psNULL; }
	;

cons_def_list
	: cons_def_list cons_def
		{ yyerrok; 
		  $$ = psNULL;
		}
	| cons_def
		{ $$ = psNULL; }
	| error
  		{ yyerror("syntax: constant definition expected");
		  $$ = psNULL;
		}
	| cons_def_list error
  		{ yyerror("syntax: error in constant definition");
		}
	;

cons_def: IDENTIFIER EQ constant
  		{ if(Topology)
  		    STMT_ValConstant($1, $3);
		  $$ = psNULL; 
		}
	| IDENTIFIER ass constant
  		{ if(Topology) {
  		    STMT_ValConstant($1, $3);
		    $$ = psNULL; 
		  }
		  yywarning("constant definition operator must be '='");
		}
	| error ass  constant
		{ yyerror("syntax: constant identifier expected"); }
	;

constant: array_constant sc                  	
		{ $$ = $1; }
	| file_constant sc 
		{ if(Topology)
		    if($1->ytype != Undef) 
		      if($1->ytype == FILEVAL &&
			 ($1->yvalue->fileval->FileType == XINT || 
			  $1->yvalue->fileval->FileType == XFLOAT ||
			  $1->yvalue->fileval->FileType == STRING))
			$$ = FI_ReadArrayFile($1);
		      else if($1->ytype == FILEVAL && 
			      ($1->yvalue->fileval->FileType == MAP ||
			       $1->yvalue->fileval->FileType == SUBNET))
			$$ = $1;
		      else {
			$$ = psNULL;
			printf("******bug: file type: %s\n", STR_TypeString($1->yvalue->fileval->FileType));
		      }
		    else
		      $$ = psNULL;
		  else
		    $$ = psNULL;
		}
	| map_constant               	 	
		{ $$ = $1;
  		  if(Topology) 
		    if( ! $1)         
		      $$ = EX_FinishUpMap();    /* map constant defined internally */
		    else {
		      $$ = $1;                  /* map constant read from file */
		    }
		  else
		    $$ = psNULL;
		}
	| expression sc                		
		{ $$ = $1; }
	;

array_constant
  	: ob expression_list cb
		{ if(Topology && Execute)
  		    $$ = EX_MakeExplicitArray();
		}
  	| ob expression range expression cb
		{ if(Topology && Execute)
  		    $$ = EX_MakeRangeArray($2, $4);
		}
	;

map_constant
	: map_header map_body sc
		{ if(Topology)
  		    $$ = $2;
		}
	;

map_header
	: MAP simple_type TO simple_type colon
		{ if(Topology) {
		    $$ = psNULL;
		    /* map type identifiers must be simple types, not array types */
		    EX_InitMap($2, $4);
		  }
		}
	;

map_body: cons_map_body
		{ $$ = psNULL; }
	| file_map_body
		{ if(Topology)
		    $$ = EX_EvalFileMap($1->ynext->ynext, $1, $1->ynext); 
		     /* arguments are (File, domain/range separator, line separator */
		  else
		    $$ = psNULL; 
		}
	;

cons_map_body
	: cons_map_body co relation
		{ $$ = psNULL; 
		}
	| relation
		{ $$ = psNULL; 
		}
	| error
  		{ yyerror("syntax: map definition line expected");
		}
	| cons_map_body error relation
  		{ yyerror("syntax: error in map definition (missing ',' ?)");
		  yyerrok;
		  Topology = Functions = FALSE;
		}
	| cons_map_body co error
  		{ yyerror("syntax: error in map definition line");
		}
	;

file_map_body
	: GET op expression co expression cp FROM IDENTIFIER
  		{ if(Topology) {
		    $3->ynext = $5;
		    $5->ynext = $8;
		    $$ = $3;
		  }
		  else
		    $$ = psNULL;
		}	
	| GET op expression cp FROM IDENTIFIER
  		{ if(Topology) {
		    $3->ynext = PA_GetUndefString();
		    $3->ynext->ynext = $6;
		    $$ = $3;
		  }
		  else
		    $$ = psNULL;
		}		  	  
	;

relation: map_constant_list TO map_constant_list
		{ if(Topology)
  		    EX_InsertMapLine($1, $3);
		  yyerrok;
		}
	;

map_constant_list
	: array_constant		
		{ $$ = $1; }
	| expression
		{ if(Topology) {
  		    EX_InitExprList($1);
		    $$ = EX_MakeExplicitArray();
		  }
		}
	;

file_constant
	: XFILE expression OF file_type	
		{ if(Topology)
  		    $$ = FI_OpenFile($2, $4);
		}
	;

file_type
  	: MAP
		{ if(Topology)
		    $$ = STMT_BuildType(MAP);
		}
	;
  	| SUBNET
		{ if(Topology)
		    $$ = STMT_BuildType(SUBNET);
		}
	| simple_type			
		{ $$ = $1; }
	;


/**** type definition block ****/
type_block
	: TYPEDEF type_body
		{ $$ = psNULL;
		}
	| empty
		{ $$ = psNULL;
		}
	;

type_body
	: type_body type_def
		{ yyerrok; 
		  $$ = psNULL;
		}
	| type_def
		{ $$ = psNULL; }
	| error
  		{ yyerror("syntax: type definition expected");
		}
	| type_body error
  		{ yyerror("syntax: error in type definitions (missing ';' ?)");
		}
	;

type_def: type_def_expr colon IDENTIFIER sc
		{ if(Topology) {
		    ST_DefineType($1, $3);
		    $$ = psNULL;
		  }
		}
	| error sc
  		{ yyerror("syntax: illegal type definition");}
	| type_def_expr error IDENTIFIER sc
		{ yywarning("syntax: missing ':' or ',' in type definition");
		  if(Topology) {
		    ST_DefineType($1, $3);
		    $$ = psNULL;
		  }
		}
	;

type_def_expr
	: site_type_def		
		{ $$ = $1; }
	| unit_type_def		
		{ $$ = $1; }
	;

site_type_def
	: SITE WITH name_def co site_func_def
		{ if(Topology) {
		    OUT_SetSNameLength($3);
		    OUT_SetSiteFuncLength($5);
		    $$ = SI_MakeSite($3, $5);
		   }
		}
	| SITE WITH site_func_def co name_def
		{ if(Topology) {
		    OUT_SetSiteFuncLength($3);
		    OUT_SetSNameLength($5);
		    $$ = SI_MakeSite($5, $3);
		  }
		}
	| SITE error name_def co site_func_def
  		{ yywarning("syntax: missing 'with' in site type definition");
		  if(Topology) {
		    OUT_SetSNameLength($3);
		    OUT_SetSiteFuncLength($5);
		    $$ = SI_MakeSite($3, $5);
		  }
		}
	| SITE error site_func_def co name_def
  		{ yywarning("syntax: missing 'with' in site type definition");
		  if(Topology) {
		    OUT_SetSiteFuncLength($3);
		    OUT_SetSNameLength($5);
		    $$ = SI_MakeSite($5, $3);
		  }
		}
	| SITE WITH error
  		{ yyerror("syntax: name or site function definition expected near 'with'");}
	;

unit_type_def
	: unit_header unit_type_body
		{ if(Topology)
		    $$ = ST_FinishUpUnitType();
		}
	;

unit_header
	: UNIT WITH
		{ if(Topology)
		    ST_InitUnitType();
		  $$ = psNULL;
		}

unit_type_body
	: unit_type_body co unit_type_param
		{ if(Topology)	    
		    ST_InsertUnitTypeParam($3);
		  $$ = psNULL;
		  yyerrok;
		}
	| unit_type_param
		{ if(Topology)
		    ST_InsertUnitTypeParam($1);
		  $$ = psNULL; 
		  yyerrok;
		}
	| error
  		{ yyerror("syntax: error in unit type definition");
		}
	| unit_type_body error unit_type_param
  		{ yyerror("syntax: missing ',' in unit type parameter definition?");
		}
	;

unit_type_param
	: name_def	
  		{ if(Topology) {
		    if($1->ytype != Undef) {
		      $1->ytype = NAME;
		      OUT_SetUNameLength($1->yvalue->string);
		    }
		    $$ = $1; 
		  }
		}
	| act_def	
		{ if(Topology) {
		    if($1)
		      $1->ytype = ACT;
		    $$ = $1;
		  }
		}
	| sites_def	
		{ if(Topology) {
		    if($1)
		      $1->ytype = SITE;
		    $$ = $1;
		  }
		}
	| actfunc_def		
  		{ if(Topology) {
		    if($1) {
		      $1->ytype = ACTFUNC;
		      OUT_SetActFuncLength($1);
		    }
		    $$ = $1;
		  }
		}
	| outfunc_def			
  		{ if(Topology) {
		    if($1) {
		      $1->ytype = OUTFUNC;
		      OUT_SetOutFuncLength($1);
		    }
		    $$ = $1;
		  }
		}
	| stype_def	
		{ if(Topology)
		    $$ = $1;
		}
	;


/*** component definitions ***/
name_def: NAME expression
		{ if(Topology)
		    $$ = ST_CheckString($2, NAME);
		  yyerrok;
		}
	;

actfunc_def
	: ACTFUNC IDENTIFIER
		{ if(Topology)
		    $$ = ST_CheckFunction($2, ACTFUNC);
		  yyerrok;
		}
	| ACTFUNC error
  		{ yyerror("syntax: error in activation function definition");
		}
	;

outfunc_def: OUTFUNC IDENTIFIER
		{ if(Topology)
		    $$ = ST_CheckFunction($2, OUTFUNC);
		  yyerrok;
		}
	| OUTFUNC error
  		{ yyerror("syntax: error in output function definition");
		}
	;
	
site_func_def
	: SITEFUNC IDENTIFIER
		{ if(Topology)
		    $$ = ST_CheckFunction($2, SITEFUNC);
		  yyerrok;
		}
	| SITEFUNC error
  		{ yyerror("syntax: error in site function definition");
		}
	;

act_def
	: ACT weight_expression
		{ if(Topology)
		    $$ = ST_CheckFloat($2, ACT);
		  yyerrok;
		}
	;

sites_def
	: SITE IDENTIFIER
		{ if(Topology) {
		    if(ST_AssertSite($2)) {
		      ST_SiteNo = 1;
		      $$ = ST_MakeSiteDef($2);
		    }
		    else
		      $$ = psNULL;
		  }
		}
	| SITE ob site_list cb
		{ if(Topology)
		    $$ = ST_MakeSiteDef($3);
		}
	;

site_list 
	: site_list co IDENTIFIER
		{ if(Topology)
		    $$ = ST_InsertSite($1, $3);
		  yyerrok; 
		}
	| IDENTIFIER
		{ if(Topology) 
		    $$ = ST_InsertSite($1, psNULL);
		  yyerrok; 
		}
	| error
  		{ yyerror("syntax: error in site list");
		}
	| site_list error
  		{ yyerror("syntax: illegal end of site list");
		}
	| site_list error IDENTIFIER
  		{ yyerror("syntax: error in site list (missing ',' ?)");
		  yyerrok;
		}
	| site_list co error
  		{ yyerror("syntax: site type expected");
		}
	;

stype_def
	: IOTYPE INPUT
		{ if(Topology)
		    $$ = ST_MakeSType(INPUT);
		}
	| IOTYPE OUTPUT
		{ if(Topology)
		    $$ = ST_MakeSType(OUTPUT);
		}
	| IOTYPE INPUT OUTPUT
		{ if(Topology)
		    $$ = ST_MakeSType(InOut);
		}
	| IOTYPE OUTPUT INPUT
		{ if(Topology)
		    $$ = ST_MakeSType(InOut);
		}
	| IOTYPE HIDDEN
		{ if(Topology)
		    $$ = ST_MakeSType(HIDDEN);
		}
	| IOTYPE error
  		{ yyerror("syntax: type 'input', 'output', or 'hidden' expected");
		  Topology = Functions = FALSE;
		}
	;




/**** structure definition block ****/
struct_block
	: struct struct_body
		{ Structures = FALSE;  /* redefinitions of explicit connections are NOT ignored */
		  $$ = psNULL;  
		}
	| empty
		{ Structures = FALSE;  /* redefinitions of explicit connections are NOT ignored */
		  $$ = psNULL;  
		}
	;

struct	: STRUCTURE
		{ Structures = TRUE;   /* redefinitions of explicit connections are ignored */
		  $$ = psNULL;
		}
	;

struct_body
	: struct_body struct_def
		{ yyerrok; 
		  $$ = psNULL;
		}
	| struct_def
		{ $$ = psNULL; }
	| error
  		{ yyerror("syntax: error in structure definition block");
		}
	| struct_body error
  		{ yyerror("syntax: structure definition expected");
		}
	;

struct_def
	: struct_spec colon identifier_list sc
  		{ if(Topology)
		    ST_AssignStructures($1, $3);
		  $$ = psNULL;
		}
	;

/**** structure types ****/
struct_spec
	: dim2_struct			
		{ $$ = $1; }
	| simple_struct			
		{ $$ = $1; }
  	| single_unit			
		{ $$ = $1; }
	;

single_unit
	: unit unit_spec at_spec
		{ if(Topology) 
		    $$ = ST_MakeSingleUnit();
		  else
		    $$ = psNULL; 
		}
	| unit unit_spec 
		{ if(Topology) 
		    $$ = ST_MakeSingleUnit();
		  else
		    $$ = psNULL;
		  yyserious("missing structure topology definition");
		}
	;

dim2_struct
	: ARRAY field_size OF simple_struct
		{ if(Topology) 
		    $$ = ST_MakeArrayOfStructures($4, $2, FALSE);
		  else
		    $$ = psNULL; 
		}
	;

simple_struct
	: cluster_struct
		{ if(Topology) 
		    $$ = ST_FinishUpStructure();
		  else
		    $$ = psNULL; 
		}
	| subnet_struct
		{ if(Topology) 
		    $$ = ST_FinishUpSubnet($1); 
		  else
		    $$ = psNULL; 
		}
	| connected_struct
		{ if(Topology) 
		    $$ = ST_FinishUpStructure(); 
		  else
		    $$ = psNULL; 
		}
	;

cluster_struct
	: cluster unit_field_size unit_spec topology_spec direction conn_spec site_spec center_spec map_spec
		{ $$ = psNULL;
		  if($6)
		    yywarning("illegal connection weight in cluster (ignored)");
		  if($7)
		    yywarning("illegal connection site in cluster (ignored)");
		  if($5)
		    yywarning("illegal connection direction in cluster (ignored)");
		  if($8)
		    yywarning("illegal center definition in cluster (ignored)");
		  if($9)
		    yywarning("illegal connection map in cluster (ignored)");
		}
	| cluster unit_field_size unit_spec 
		{ $$ = psNULL;
		  yyserious("missing structure topology definition");
		}
	;

subnet_struct
	: SUBNET IDENTIFIER AT op expression co expression cp param_spec  
		{ $$ = PA_GetParserStack();
		  if(Topology)
		    $$->ytype = PA_Parser($2, $9, $5, $7);
		  else 
		    $$->ytype = Undef;
		}
		    
	| SUBNET IDENTIFIER param_spec 
		{ $$ = PA_GetParserStack();
		  if(Topology)
		    $$->ytype = PA_Parser($2, $3, PA_GetUndefValue(), PA_GetUndefValue());
		  else 
		    $$->ytype = Undef;
		  yyserious("missing structure topology definition");
		}
	;

connected_struct
	: connection_key unit_field_size unit_spec topology_spec direction conn_spec 
	  site_spec center_spec map_spec
		{ if(Topology) {
		    ST_InsertWeight($6);
		    ST_InsertMap($9, ST_TempStruct);
		  }
		  $$ = psNULL;
		}
	| connection_key unit_field_size unit_spec direction conn_spec site_spec center_spec map_spec
		{ if(Topology) {
		    ST_InsertWeight($5);
		    ST_InsertMap($8, ST_TempStruct);
		  }
		  yyserious("missing structure topology definition");
		  $$ = psNULL;
		}
	;

	
/**** structure types ****/
connection_key
	: CHAIN
		{ if(Topology) 
		    ST_InitUnitValue(CHAIN);
		  $$ = psNULL;
		}
	| RING
		{ if(Topology) 
		    ST_InitUnitValue(RING);
		  $$ = psNULL;
		}
	| CLIQUE
		{ if(Topology) 
		    ST_InitUnitValue(CLIQUE);
		  $$ = psNULL;
		}
	| STAR
		{ if(Topology) 
		    ST_InitUnitValue(STAR);
		  $$ = psNULL;
		}
	| IRREGULAR
		{ if(Topology) 
		    ST_InitUnitValue(IRREGULAR);
		  $$ = psNULL;
		}
	;

unit	: UNIT
		{ if(Topology) 
		    ST_InitUnitValue(UNIT);
		  $$ = psNULL;
		}
	;

cluster	: CLUSTER
		{ if(Topology) 
		    ST_InitUnitValue(CLUSTER);
		  $$ = psNULL;
		}
	;


/**** structure size ****/
unit_field_size
	: field_size
		{ if(Topology) 
		    ST_InsertStructSize($1);
		  $$ = psNULL;
		}
	;




/**** unit specification for structures ****/

unit_spec
	: unit_struct with_spec get_spec
		{ $$ = psNULL; }
	;
	
/**** specification of named unit type ****/
unit_struct
	: OF IDENTIFIER
		{ if(Topology) { 
		    ST_InitStructFType($2);	
		  }
		  $$ = psNULL;
		}
	| empty
		{ if(Topology) 
		    ST_InitStructTypeExpl();
		  $$ = psNULL;
		}
	| error
  		{ yyerror("syntax: unit type specification expected");
		}
	| ILLEGAL
  		{ yyerror("illegal symbol: unit type or 'unit' expected");
		}
	;


	
/**** complementation of unit parameters ****/
with_spec
	: WITH unit_struct_body
		{ $$ = psNULL; }
	| empty
		{ $$ = psNULL; }
	;

unit_struct_body
	: unit_struct_body co unit_struct_param
		{ yyerrok;
		  $$ = psNULL;
		}
	| unit_struct_param
		{ $$ = psNULL; }
	| error
  		{ yyerror("syntax: error in unit default specification");
		}
	| unit_struct_body co error
  		{ yyerror("syntax: unit default specification expected");
		}
	;

unit_struct_param
	: name_def
		{ if(Topology) { 
		    ST_InsertStructName($1);
		    if($1->ytype != Undef)
		      OUT_SetUNameLength($1->yvalue->string);
		  }
		  $$ = psNULL;
		}
	| act_def
		{ if(Topology) 
		    ST_InsertStructAct($1);
		  $$ = psNULL;
		}
	| sites_def
		{ if(Topology) 
		    ST_InsertStructSites($1);
		  $$ = psNULL;
		}
	| actfunc_def
		{ if(Topology) {
		    OUT_SetActFuncLength($1);
		    ST_InsertStructFunction($1, ACTFUNC);
		  }
		  $$ = psNULL;
		}
	| outfunc_def
		{ if(Topology) {
		    OUT_SetOutFuncLength($1);
		    ST_InsertStructFunction($1, OUTFUNC);
		  }
		  $$ = psNULL;
		}
	| stype_def
		{ if(Topology) 
		    ST_InsertStructSType($1);
		  $$ = psNULL;
		}
	;


/**** insertion of explicit values ****/
get_spec: get struct_field
		{ $$ = psNULL; }
	| empty
		{ $$ = psNULL; }
	;

get	: GET
		{ if(Topology) 
		    ST_InitExplUnits(ST_TempStruct);
		  $$ = psNULL;
		}
	;

param_spec
	: VAR param_val
		{ $$ = $2; }
	| empty
		{ $$ =  psNULL; }
	;

struct_field
	: struct_field co field_struct
		{ yyerrok; 
		  $$ = psNULL;
		}
	| field_struct
		{ $$ = psNULL; }
	| error
  		{ yyerror("syntax: 'get <prm> from <array>' expected");
		}
	| struct_field error
  		{ yyerror("syntax: after 'get'");
		}
	| struct_field error field_struct
  		{ yyerror("syntax: after 'get' (missing ',' ?)");
		  yyerrok;
		}
	| struct_field co error
  		{ yyerror("syntax: '<prm> from <array>' expected");
		}
	;

field_struct
	: NAME FROM element_group
		{ if(Topology) 
		    ST_InsertExplValues(NAME, $3);
		  $$ = psNULL;
		}
	| ACT FROM element_group
		{ if(Topology) 
		    ST_InsertExplValues(ACT, $3);
		  $$ = psNULL;
		}
	;



/**** specification of topology for structures ****/
topology_spec 
	: plane_spec at_spec
		{ $$ = psNULL; }
	| area_spec at_spec
		{ $$ = psNULL; }
	;

plane_spec
	: PLANE XSTRCAT 	                      	 /* plane | (vertical) */
		{ if(Topology) 
		    ST_InsertTopology(MATRIX, ST_MakePos(1), ST_MakePos(ST_TempStruct->Length));
		  $$ = psNULL;
		}
	| PLANE MINUS 	                       	/* plane - (horizontal) */
		{ if(Topology) 
		    ST_InsertTopology(MATRIX, ST_MakePos(ST_TempStruct->Length), ST_MakePos(1));
		  $$ = psNULL;
		}
	| PLANE 	                       	/* default: plane  (horizontal) */
		{ if(Topology) 
		    ST_InsertTopology(MATRIX, ST_MakePos(ST_TempStruct->Length), ST_MakePos(1));
		  $$ = psNULL;
		}
	| PLANE error	                       	/* error in plane specification */
		{ if(Topology) {
		    yywarning("illegal plane orientation definition (using '-')");
		    ST_InsertTopology(MATRIX, ST_MakePos(ST_TempStruct->Length), ST_MakePos(1));
		  }
		  $$ = psNULL;
		}
	;

area_spec
	: MATRIX op expression co expression cp    	 /* (x, y) indicates dimension of matrix */
		{ if(Topology) 
		    ST_InsertTopology(MATRIX, $3, $5);
		  $$ = psNULL;
		}
	| ELLIPSE  op expression co expression cp    	 /* (x, y) indicates extension of matrix */
		{ if(Topology) 
		    ST_InsertTopology(ELLIPSE, $3, $5);
		  $$ = psNULL;
		}
	;

at_spec : AT op expression co expression cp       	 /*  (x, y) indicates topological center of structure */
		{ if(Topology) 
		    ST_InsertTopCenter($3, $5);
		  $$ = psNULL;
		}
	;



/**** specification of weight for implicit connections ****/
conn_spec
	: BY weight_expression
		{ if(Topology)
		    $$ = ST_CheckFloat($2, BY);
		}
	| empty
		{ $$ = psNULL; }
	;


/**** specification of target site for implicit connections ****/
site_spec
	: THROUGH IDENTIFIER
		{ if(Topology)
		    ST_InsertThrough($2);
		  $$ = psNULL;
		}
	| THROUGH error
  		{ yyerror("syntax: site type expected");
		}
	| empty
		{ if(Topology)
		    ST_InsertThrough(psNULL);
		  $$ = psNULL;
		}
	;


/**** specification of direction for implicit connections ****/
direction
	: PT
		{ if(Topology) {
		    ST_InsertStructDirection(PT);
		    $$ = psNULL;
		  }
		}
	| PF
		{ if(Topology) {
		    ST_InsertStructDirection(PF);
		    $$ = psNULL;
		  }
		}
	| PTF
		{ if(Topology) {
		    ST_InsertStructDirection(PTF);
		    $$ = psNULL;
		  }
		}
	| empty
	;


/**** specification of center for star structure ****/
center_spec
	: CENTER expression
  		{ if(Topology) {
		    ST_InsertStructCenter(EX_CorrectSize($2));
		    $$ = psNULL;
		  }
		}
	| empty
		{ $$ = psNULL; }
	;


/**** specification of map constant for irregular structures ****/
map_spec: MAP field_range
		{ $$ = $2; }
	| empty
		{ $$ = psNULL; }
	;



/**** variable declarations ****/
var_block
	: VAR var_decl_block
		{ RightSide = FALSE;
                  ConstantSection = FALSE;
		  $$ = psNULL; }
	| empty
		{ RightSide = FALSE;
		  $$ = psNULL; }
	;

var_decl_block
	: var_decl_block var_decl
		{ $$ = psNULL;
		  yyerrok; 
		}
	| var_decl
		{ $$ = psNULL; }
	| error
  		{ yyerror("syntax: error in variable declarations (missing ';' ?)");
		}
	| var_decl_block error
  		{ yyerror("syntax: variable declaration or 'begin' expected");
		}
	;

var_decl: var_type colon identifier_list sc
		{ if(Topology)
		    STMT_DeclareVar($1, $3);
		  $$ = psNULL;
		}
	| error colon identifier_list sc
  		{ yyerror("syntax: illegal variable type");}
	;

var_type
	: singular_type		
		{ $$ = $1; }
	| ARRAY OF singular_type
		{ if(Topology)
		    $$ = STMT_BuildArrayType($3);
		}
	;

singular_type
	: UNIT
		{ if(Topology)
		    $$ = STMT_BuildType(UNIT);
		}
	| STRUCTURE
		{ if(Topology)
		    $$ = STMT_BuildType(STRUCTURE);
		}
	| simple_type		
		{ $$ = $1; }
	;



/**** topology definition block ****/
progr_block
	: XBEGIN statement_list END
		{ $$ = psNULL; }
	;

statement_list
	: statement_list sc statement
		{ $$ = psNULL;
		  yyerrok; 
		}
	| statement_list sc 
		{ $$ = psNULL;
		  /* abortloop generates an extra  ';' before end token */
		  /* FOREACH loop: give parser an additional ';' to separate FOREACH */
		  /* and some loop variable (on stack, further iterations) */
		  if( ! EvalLoopMode) 
                    yywarning("illegal ';' ignored");
		  yyerrok; 
		}
	| statement
		{ $$ = psNULL; }
	| empty
		{ $$ = psNULL; }
	;

statement
	: ass_stmt
		{ RightSide = FALSE; 
		  $$ = psNULL; 
		}
	| conn_stmt
		{ $$ = psNULL; }
	| loop_stmt 
		{ $$ = psNULL;	
		  if( ! StackMode) 
		    Execute = TRUE;     /* time delay, execute after reduction of expression */
		}
	| if_stmt 
		{ $$ = psNULL;
		  if( ! StackMode) 
		    Execute = TRUE;     /* time delay, execute after reduction of expression */
		}
	;

ass_stmt 						/* assignment statement */
	: stmt_left weight_expression
  		{ if(Topology && Execute)               /* only if there's not been a fatal error */
		      STMT_AssignValue($1, $2);         /* normal processing: Execute statement */
		  $$ = psNULL;
		}
	| stmt_left topology_key
  		{ if(Topology && Execute)
		    STMT_AssignValue($1, $2);
		  $$ = psNULL;
		}
	;

stmt_left
	: variable ass
		{ $$ = $1;
		  RightSide = TRUE; 
		}
	| variable EQ 
		{ $$ = $1;
		  RightSide = TRUE;
		  if( ! EvalLoopMode)
		    yywarning("assignment  operator must be ':='"); 
		}
	;

conn_stmt						/* connection definition */
	: variable PT variable conn_weight 
  		{ if(Topology && Execute) {
		    STMT_GetIdValue($1);
		    STMT_GetIdValue($3);
		    STMT_MakeExplicitConnection($1, $3, $4, TRUE);
		    PA_FreeParserStack($1);
		    PA_FreeParserStack($3);
		    if($4) PA_FreeParserStack($4);
		  }
		  $$ = psNULL;
		}
	| variable PTF variable conn_weight 
  		{ if(Topology && Execute) {
		    STMT_GetIdValue($1);
		    STMT_GetIdValue($3);
		    STMT_MakeExplicitConnection($1, $3, $4, FALSE);
		    STMT_MakeExplicitConnection($3, $1, $4, FALSE);
		    PA_FreeParserStack($1);
		    PA_FreeParserStack($3);
		    if($4) PA_FreeParserStack($4);
		  }
		  $$ = psNULL;
		}
	| variable error variable conn_weight
  		{ yyerror("syntax: '->' or '<->' expected in connection definition");
		}
	;

conn_weight
	: colon weight_expression
		{ if(Topology && Execute) {
		    ST_CheckFloat($2, BY);
		    $$ = $2;
		  }
		  else
		    $$ = psNULL;
		}
	| empty
		{ $$ = psNULL; }
	;

weight_expression
  	: random				
		{ $$ = $1; }
	| expression				
		{ $$ = $1; }
	;

loop_stmt
	: for_loop
	| foreach_loop
  	| while_loop
	;


/* grammar rules for loop statements */

while_header
	: WHILE log_expr 
		{ if(Topology && Execute)
		    LOOP_AssignCondValue($2);
		  $$ = psNULL;
		}
	;

while_loop
	: while_header DO statement_list END                   /* while loop (general) */
		{ $$ = psNULL; }
	;

for_ass_stmt
	: stmt_left expression TO expression
		{ if(Topology && Execute) { 
		    RightSide = FALSE;
		    LOOP_InitForLoop($1, $2);       /* initialize and lock loop variable */
		    LOOP_CalcForIterations($4);     /* insert no. of interations into loop header */
		  }
		  $$ = psNULL;
		}
	;

for_loop: FOR for_ass_stmt DO statement_list END 	 /* for loop (int or simplestring) */
		{ $$ = psNULL; }
	;

foreach_head
	: FOREACH loop_var_list
		{ if(Topology && Execute) { 
		    RightSide = FALSE;
		    LOOP_InitForeachLoop($1, $1);      /* initialize and lock loop variable */
		  }
		  $$ = psNULL;
		}
	;

foreach_loop
	: foreach_head DO statement_list END           /* foreach loop (set inclusion) */
	;

loop_var_list
	: loop_var_list co loop_var_def
		{ yyerrok; }
	| loop_var_def
	| error
  		{ yyerror("syntax: illegal condition list in 'foreach' - statement");
		}
	| loop_var_list error
  		{ yyerror("syntax: illegal end of condition list in 'foreach' - statement");
		}
	| loop_var_list error loop_var_def
  		{ yyerror("syntax: error in 'foreach' - condition list (missing ',' ?)");
		  yyerrok;
		}
	| loop_var_list co error
  		{ yyerror("syntax: condition expected");
		}
	;

loop_var_def
	: IDENTIFIER IN element_group
		{ if(Topology &&  Execute) {
		    LOOP_CheckLoopVar($1, $3);
		    LOOP_InsertLoopVar($1, $3);
		  }
		  $$ = psNULL;
		}
	| IDENTIFIER error element_group
  		{ yyerror("syntax: loop variable definition expected");
		}
	;



/* grammar rules for conditional statements */

if_head : IF log_expr
		{ if(Topology &&  Execute) 
		    LOOP_AssignCondValue($2);
                }
	;	    

if_stmt	: if_head THEN statement_list elseIf_part else_part END
	;

elseIf_part
	: elseIf_body
	| empty
		{ $$ = psNULL; }
	;
	
elseIf_body
	: elseIf_body elseIf
		{ yyerrok; }
  	| elseIf_body error
		{yyerror("syntax: illegal ELSEIF branch");}
	| elseIf
	| error
		{yyerror("syntax: illegal ELSEIF statements");}
	;

elseIf	: elseIf_head THEN statement_list
	| error THEN statement_list
		{yyerror("syntax: illegal ELSEIF condition");}
	;

elseIf_head
	: ELSEIF log_expr
		{ if(Topology &&  Execute)   
		    LOOP_AssignCondValue($2);
                }
	; 

else_part
	: ELSE statement_list
	| error statement_list
		{yyerror("syntax: illegal ELSE part");}
	| empty
		{ $$ = psNULL; }
	;




/**** syntax of expressions ****/

expression
	: expression PLUS expression 		%prec PLUS
		{ if(Topology && Execute)
		    $$ = EX_EvalNumExpr(PLUS, $1, $3); 
		}
	| expression MINUS expression 		%prec MINUS
		{ if(Topology && Execute)
		    $$ = EX_EvalNumExpr(MINUS, $1, $3); 
		}
	| expression MULT expression  		%prec MULT
		{ if(Topology && Execute)
		    $$ = EX_EvalNumExpr(MULT, $1, $3); 
		}
	| expression RDIV expression  		%prec RDIV
		{ if(Topology && Execute)
		    $$ = EX_EvalNumExpr(RDIV, $1, $3); 
		}
	| expression DIV expression  		%prec DIV
		{ if(Topology && Execute)
		    $$ = EX_EvalIntExpr(DIV, $1, $3); 
		}
	| expression MOD expression  		%prec MOD
		{ if(Topology && Execute)
		    $$ = EX_EvalIntExpr(MOD, $1, $3); 
		}
	| expression XSTRCAT expression  	%prec XSTRCAT
		{ if(Topology && Execute)
		    $$ = EX_EvalStringExpr(XSTRCAT, $1, $3); 
		}
	| MINUS expression			%prec UMINUS
		{ if(Topology && Execute)
		    $$ = EX_EvalSign(MINUS, $2); 
		}
	| PLUS expression			%prec UMINUS
		{ if(Topology && Execute)
		    $$ = EX_EvalSign(PLUS, $2); 
		}
	| op expression cp
		{ if(Topology && Execute)
		    $$ = $2; 
		}
	| function_expression			
		{ $$ = $1; }
	| variable
		{ if(Topology && Execute) {
		    EX_AssertValue($1);
		    InnerSubnet = NULL;     /* access to a subnet through "." ends here */
		    $$ = $1;
		  }
		}
	| error 
  		{ yyerror("syntax: expression expected");}
	;
    
variable
	: unsigned_constant                 	
		{ $$ = $1; }
	| IDENTIFIER				
		{ $$ = $1; }
	| selection_expression			
		{ $$ = $1; }
	| field_element
		{ $$ = $1; }
	;

field_element
	: variable field_size
		{ if(Topology && Execute)
		    $$ = STMT_GetElement($1, $2); 
		}
	;

function_expression
	: simple_func_expr                 	
		{ $$ = $1; }
	| list_func_expr			
		{ $$ = $1; }
	;

simple_func_expr
	: XARCTAN op expression cp
		{ if(Topology && Execute)
		    $$ = EX_ArctanFunc($3); 
		}
	| XCOS op expression cp
		{ if(Topology && Execute)
		    $$ = EX_CosFunc($3);
		}
	| XSIN op expression cp
		{ if(Topology && Execute)
		    $$ = EX_SinFunc($3);
		}
	| XEXP op expression cp
		{ if(Topology && Execute)
		    $$ = EX_ExpFunc($3);
		}
	| XLN op expression cp
		{ if(Topology && Execute)
		    $$ = EX_LnFunc($3); 
		}
	| XSQR op expression cp
		{ if(Topology && Execute)
		    $$ = EX_SqrFunc($3);
		}
	| XSQRT op expression cp
		{ if(Topology && Execute)
		    $$ = EX_SqrtFunc($3); 
		}
	;

list_func_expr
	: XMIN op expression_list cp
		{ if(Topology && Execute)
		    $$ = EX_ListFunc(XMIN); 
		}
	| XMAX op expression_list cp
		{ if(Topology && Execute)
		    $$ = EX_ListFunc(XMAX);
		}
	;

selection_expression
	: variable pt ACT					/* activation selection of a unit */
		{ if(Topology && Execute)
		    $$ = STMT_GetUnitComponent($1, ACT); 
		}
	| variable pt IOTYPE 					/* name selection of a unit */
		{ if(Topology && Execute)
		    $$ = STMT_GetUnitComponent($1, IOTYPE); 
		}
	| variable pt NAME 					/* name selection of a unit */
		{ if(Topology && Execute)
		    $$ = STMT_GetUnitComponent($1, NAME); 
		}
	| variable pt IDENTIFIER                      		/* subnet component or unit site selection */
		{ if(Topology && Execute)
		    $$ = STMT_SelectIdentifierComponent($1, $3); 
		}
	| variable pt error
  		{ yyerror("syntax: error near selection operator - unit component expected");
		}
	;

topology_key
	: INPUT
		{ $$ = PA_GetParserStack(); $$->ytype = INPUT;  }
	| OUTPUT
		{ $$ = PA_GetParserStack(); $$->ytype = OUTPUT;  }
	| HIDDEN
		{ $$ = PA_GetParserStack(); $$->ytype = HIDDEN;  }
	| INPUT co OUTPUT
		{ $$ = PA_GetParserStack(); $$->ytype = InOut;  }
	| OUTPUT co INPUT
		{ $$ = PA_GetParserStack(); $$->ytype = InOut;  }
	;


/*** logical expressions ***/
log_expr: log_expr AND log_expr		%prec AND
  		{ if(Topology && Execute) {
		    $$ = LOG_EvalLogicalExpr(AND, $1, $3);
		  }
		  else
		    $$ = psNULL;
		}
	| log_expr OR log_expr		%prec OR
  		{ if(Topology && Execute) {
		    $$ = LOG_EvalLogicalExpr(OR, $1, $3);
		  }
		  else
		    $$ = psNULL;
		}
	| NOT log_expr			%prec NOT
  		{ if(Topology && Execute) {
		    $$ = LOG_EvalLogicalNot($2);
		  }
		  else
		    $$ = psNULL;
		}
	| op log_expr cp
  		{ if(Topology && Execute) {
		    $$ = $2;
		  }
		  else
		    $$ = psNULL;
		}
	| bool_expr
  		{ if(Topology && Execute) {
		    $$ =  $1;
		  }
		  else
		    $$ = psNULL;
		}
	;


/*** relational expressions ***/
bool_expr
	: expression GT expression
  		{ if(Topology && Execute)
		    $$ = LOG_EvalRelation(GT, $1, $3);
		  else
		    $$ = psNULL;
		}
	| expression GE expression
  		{ if(Topology && Execute)
		    $$ = LOG_EvalRelation(GE, $1, $3);
		  else
		    $$ = psNULL;
		}
	| expression EQ expression	
  		{ if(Topology && Execute)
		    $$ = LOG_EvalRelation(EQ, $1, $3);
		  else
		    $$ = psNULL;
		}
	| expression NE expression	
  		{ if(Topology && Execute)
		    $$ = LOG_EvalRelation(NE, $1, $3);
		  else
		    $$ = psNULL;
		}
	| expression LT expression	
  		{ if(Topology && Execute)
		    $$ = LOG_EvalRelation(LT, $1, $3);
		  else
		    $$ = psNULL;
		}
	| expression LE expression	
  		{ if(Topology && Execute)
		    $$ = LOG_EvalRelation(LE, $1, $3);
		  else
		    $$ = psNULL;
		}
        | expression IN element_group 	
  		{ if(Topology && Execute)
		    $$ = LOG_EvalInclusion($1, $3);
		  else
		    $$ = psNULL;
		}
  	| op expression co expression cp IN field_range             /* map relation */	
  		{ if(Topology && Execute)
		    $$ = LOG_EvalMapRelation($2, $4, $7);
		  else
		    $$ = psNULL;
		}
        ;




/**** general grammar symbols ****/
simple_type
	: XINT
		{ if(Topology && Execute)
		    $$ = STMT_BuildType(XINT);
		}
	| XFLOAT
		{ if(Topology && Execute)
		    $$ = STMT_BuildType(XFLOAT);
		}
	| STRING
		{ if(Topology && Execute)
		    $$ = STMT_BuildType(STRING);
		}
	;

unsigned_constant                          	
	: INTEGER				
		{ $$ = $1; }
	| FLOATNUM				
		{ $$ = $1; }
	| LONGSTRING				
		{ $$ = $1; }
	| SIMPLESTRING				
		{ $$ = $1; }
	| ILLEGAL
  		{ yyerror("illegal symbol: constant or variable expected");
		}
	;

parameter_list
	: identifier_list
	| default_prm_list
	| empty
	;

default_prm_list
	: default_prm_list co default_prm
		{ if(Topology)
		    $$ = PA_StackPrm($1, $3);
		  yyerrok; 
		}
	| default_prm				
		{ $$ = $1; }
	| default_prm_list error
  		{ yyerror("syntax: illegal end of parameter list");
		}
	| default_prm_list error default_prm
  		{ yyerror("syntax: error in parameter list (missing ',' ?)");
		  yyerrok;
		}
	| default_prm_list co error
  		{ yyerror("syntax: default (!) parameter definition expected");
		} 
	;

default_prm
	: IDENTIFIER ass  expression	
		{ if(Topology) {
		    PA_UpdateParameter($1->yvalue->var, $3);
		    $$ = $1;
		  }
		  else
		    $$ = psNULL;
		}
	| IDENTIFIER EQ  expression
		{ if(Topology) {
		    PA_UpdateParameter($1->yvalue->var, $3);
		    $$ = $1;
		  }
		  else
		    $$ = psNULL;
		  yywarning("operator for parameter default definition must be ':='"); 
		}
	;

identifier_list
	: identifier_list co IDENTIFIER
		{ if(Topology) {
		    $$ = PA_StackPrm($1, $3);
		  }
		  yyerrok; 
		}
	| IDENTIFIER				
		{ $$ = $1; }
	| error
  		{ yyerror("syntax: illegal identifier list");
		}
	| identifier_list error
  		{ yyerror("syntax: illegal end of identifier list");
		}
	| identifier_list error IDENTIFIER
  		{ yyerror("syntax: error in identifier list (missing ',' ?)");
		  yyerrok;
		}
	| identifier_list co error
  		{ yyerror("syntax: identifier expected");
		}
	;

field_size   /* declare field size */
	: ob expression cb
		{ if(Topology && Execute)
		    $$ = EX_CorrectSize($2);
		}
	;

field_range
	: variable ob expression range expression cb
		{ if(Topology && Execute)
		    $$ = ST_BuildRangeSelection($1, EX_CorrectSize($3), EX_CorrectSize($5));
		  else
	 	    $$ = psNULL;
		}
	| variable				
		{ if(Topology && Execute) {
		    $$ = $1;
		  }
		  else
	 	    $$ = psNULL;
		}
	;

param_val
	: ob subnet_param_list cb
		{ if(Topology) 
		    $$ = $2;
		  else
		    $$ = psNULL;
		}
	| expression
		{ if(Topology) 
		    $$ = $1;
		  else
		    $$ = psNULL;
		}
	;

subnet_param_list
	: subnet_param_list co expression
		{ if(Topology) {
		    $$ = PA_StackPrm($1, $3);
		  }
		  yyerrok; 
		}
	| expression
		{ if(Topology) 
		    $$ = $1;
		  else
		    $$ = psNULL;
		}
	| subnet_param_list error expression
  		{ yyerror("syntax: error in subnet parameter list (missing ',' ?)");
		  yyerrok;
		}
	| subnet_param_list error
  		{ yyerror("syntax: illegal end of subnet parameter list");
		}
	;


expression_list
	: expression_list co expression
		{ if(Topology && Execute)
		    EX_AppendExprList($3);
	 	  $$ = psNULL;
		  yyerrok;
		}
	| expression
		{ if(Topology && Execute)
		    EX_InitExprList($1);
	 	  $$ = psNULL;
		}
	| expression_list error expression
  		{ yyerror("syntax: error in expression list (missing ',' ?)"); }
	;

element_group
	: field_range
        | array_constant
	;


/**** literals - symbols and operators ****/
range 	: ARANGE
		{ $$ = psNULL; }	
	;

random  : RANDOM
		{ if(Topology && Execute) {
		    $$ = PA_GetParserStack();
		    $$->ytype = RANDOM; 
		  }
		}
	;

network : NETWORK
                {
#ifndef NOACTIONS
		  Topology = TRUE;
		  RightSide = TRUE;
		  ConstantSection = TRUE;
#endif
		  $$ = psNULL; 	
		}
	;

ass	: ASSIGN
		{ $$ = psNULL;
  		  yyerrok; 
		}
	;
pt	: POINT
		{ $$ = psNULL; 
		}	
	;
co	: PERIOD
		{ $$ = psNULL; }	
	;
sc	: SEMICOLON
                { $$ = psNULL;
                  yyerrok; 
		}
	;
colon	: COLON
  		{ $$ = psNULL;
  		  yyerrok; 
		}
	;
op	: OPARENT
		{ $$ = psNULL; }	
	;
cp	: CPARENT
  		{ $$ = psNULL;
  		  yyerrok; 
		}
	;
ob      : OBRACKET
		{ $$ = psNULL; }	
	;
cb	: CBRACKET
  		{ $$ = psNULL;
  		  yyerrok; 
		}
	;

/**** special symbols ****/
empty	:
		{ $$ = psNULL; }
	;

%%



