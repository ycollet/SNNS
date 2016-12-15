/* MACROS */
#define EDIT_INPUT                  1
#define EDIT_OUTPUT                 2
#define FILL_VERT                   1
#define FILL_HORIZ                  2
#define SEE_SUFFIX_MENU_LABEL       "see suffix"
#define N01_MENU_LABEL              "n01"
#define LVQ_MENU_LABEL              "lvq"
#define MENU_LEFTMARGIN             18
#define MENU_RIGHTMARGIN            18
#define TEXT_MARGINS                1
#define PATTERNS_WIDTH              600
#define PATTERNS_HEIGHT             600
#define EXAMPLE_NUMBER              "8888888888"
#define EXAMPLE_CLASSNAME           "EXAMPLE_CLASSNAME"
#define SWAP_WIDTH                  600
#define SWAP_HEIGHT                 200
#define SWAP_TITLE                  (inOutSwitch == EDIT_INPUT ?    \
                                     "output patterns" : "input patterns")
#define STR2_LEFTMARGIN             8
#define STR2_TOPMARGIN              8
#define STR2_LABELWIDTH             100
#define STR2_HDLINE_LABEL_DIST      16
#define STR2_DIST_BETWEEN_LABELS    8
#define STR2_DIST_BETWEEN_COMMANDS  16
#define STR2_LABEL_TEXT_DIST        16
#define STR2_LABEL_COMMAND_DIST     16
#define STR2_TEXTWIDTH              100
#define DIST_TITLE_VALUE            0
#define DIST_BETWEEN_INFOS          16
#define PV_TOPMARGIN                16
#define PV_LEFTMARGIN               4
#define VERT_SKIP                   8
#define HORIZ_SKIP                  8
#define BORDER_WIDTH                1
#define STRDLG_WIDTH                300
#define STATISTICS_WIDTH            400
#define STATISTICS_HEIGHT           300

#define INT2_LEFTMARGIN             8
#define INT2_TOPMARGIN              8
#define INT2_LABELWIDTH             150
#define INT2_HDLINE_LABEL_DIST      16
#define INT2_DIST_BETWEEN_LABELS    8
#define INT2_TEXT_ARROW_DIST        16
#define INT2_DIST_BETWEEN_COMMANDS  16
#define INT2_LABEL_TEXT_DIST        16
#define INT2_LABEL_COMMAND_DIST     16
#define INT2_COMMANDWITH            64
#define INT2_TEXTWIDTH              100

#define SSW_WIDTH                   600
#define SSW_HEIGHT                  600
#define SSW_COL_SKIP                8

#define editedVecColl(p) (inOutSwitch == EDIT_INPUT ? inputs(p) : outputs(p))
#define numCols(p) (inOutSwitch == EDIT_INPUT ? inputDims(p) : outputDims(p))
#define selVec() (scalarSwitch == FILL_VERT ? vert : horiz)
#define inOutChar() (inOutSwitch == EDIT_INPUT ? 'i' : 'o')
#define inOutString() (inOutSwitch == EDIT_INPUT ? "input" : "output")
#define clearInfo() XtVaSetValues(infoLabel, XtNlabel, "", NULL)










