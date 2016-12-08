/*****************************************************************************
  FILE           : $Source: /projects/higgs1/SNNS/CVS/SNNS/xgui/sources/bn_art1.c,v $
  SHORTNAME      : bn_art1
  SNNS VERSION   : 4.2

  PURPOSE        : Creates the ART1-window and ART1 networks.
  NOTES          :

  AUTHOR         : Kai-Uwe Herrmann
  DATE           : 15.1.1993

  CHANGED BY     : 
  RCS VERSION    : $Revision: 2.10 $
  LAST CHANGE    : $Date: 1998/03/03 14:09:38 $

    Copyright (c) 1990-1995  SNNS Group, IPVR, Univ. Stuttgart, FRG
    Copyright (c) 1996-1998  SNNS Group, WSI, Univ. Tuebingen, FRG

******************************************************************************/
#include <config.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>

#include "ui.h"
#include "kr_ui.h"
#include "ui_mainP.h"
#include "ui_xWidgets.h"
#include "ui_fileP.h"
#include "bn_basics.h"
#include "ui_control.h"
#include "ui_confirmer.h"

#include "bn_art1.ph"




/*****************************************************************************
  FUNCTION : bn_art1_createNet

  PURPOSE  : Generation of the ART1 network using SNNS kernel functions.
  NOTES    :

  UPDATE   : 20.1.1993
******************************************************************************/

static krui_err bn_art1_createNet (int IUnits, int IRow, int CUnits, int CRow)

{
  krui_err          ret_code                   = KRERR_NO_ERROR;
  int               i, j, unit_no;

  int               ICol, CCol,
                    Row, Col, NoOfUnitsPerRow;

  int               RowOffset, ColOffset;

  struct PosType    unit_pos;

  char              name[10];


  ICol = NoOfUnitsPerRow = ((int) (IUnits/IRow) + POS_SIGN(IUnits % IRow));


  /*  Create Input Units  */
  Col = 1;
  Row = 1;
  RowOffset = 4;
  ColOffset = 1;
  for (i = 1; i <= IUnits; i++)
    {

    unit_no = krui_createDefaultUnit();
    if (unit_no < 0)  CHECK_RETURN( unit_no );
    ret_code = krui_setUnitTType( unit_no, INPUT );
    CHECK_RETURN( ret_code );
    ret_code = krui_setUnitActFunc(unit_no, ART1_ACTF_INP);
    CHECK_RETURN (ret_code);
    ret_code = krui_setUnitOutFunc(unit_no, ART1_OUTFUNC);
    CHECK_RETURN (ret_code);
    sprintf (name,"%s%d","inp",i);
    ret_code = krui_setUnitName (unit_no, name);
    CHECK_RETURN (ret_code);

    unit_pos.x = Col + ColOffset;
    unit_pos.y = Row + RowOffset;
    krui_setUnitPosition( unit_no, &unit_pos );

    if ( (ICol == Col) ||
         ((IUnits%IRow != 0) && (Row != IRow) && 
          ((IUnits-i)%(IRow-Row) == 0) && (Col == ICol-1)
         )
       ) 
    {
      Row++;
      Col = 1;
    } else {
      Col++;
    } /*if*/

  }

  /*  Create Comparison Layer Units */

  RowOffset = 4;
  ColOffset = ICol + 2;
  Row = 1;
  Col = 1;

  for (i = 1; i <= IUnits; i++)
    {
    unit_no = krui_createDefaultUnit();
    if (unit_no < 0)  CHECK_RETURN( unit_no );
    ret_code = krui_setUnitTType( unit_no, HIDDEN );
    CHECK_RETURN( ret_code );
    ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_CMP);
    CHECK_RETURN (ret_code);
    ret_code = krui_setUnitOutFunc (unit_no, ART1_OUTFUNC);
    CHECK_RETURN (ret_code);
    sprintf (name,"%s%d","cmp",i);
    ret_code = krui_setUnitName (unit_no, name);
    CHECK_RETURN (ret_code);

    unit_pos.x = Col + ColOffset;
    unit_pos.y = Row + RowOffset;
    krui_setUnitPosition( unit_no, &unit_pos );

    if ( (ICol == Col) ||
         ((IUnits%IRow != 0) && (Row != IRow) && 
          ((IUnits-i)%(IRow-Row) == 0) && (Col == ICol-1)
         )
       ) 
    {
      Row++;
      Col = 1;
    } else {
      Col++;
    } /*if*/

  }

  CCol = NoOfUnitsPerRow = ((int) (CUnits/CRow) + POS_SIGN(CUnits % CRow));


  /*  Create Recognition layer units */

  RowOffset = 4;
  ColOffset = 2 * ICol + 5;
  Row = 1;
  Col = 1;

  unit_pos.x = 10;
  for (i = 1; i <= CUnits; i++)
    {
    unit_no = krui_createDefaultUnit();
    if (unit_no < 0)  CHECK_RETURN( unit_no );
    ret_code = krui_setUnitTType( unit_no, SPECIAL );
    CHECK_RETURN( ret_code );
    ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_REC);
    CHECK_RETURN (ret_code);
    ret_code = krui_setUnitOutFunc (unit_no, ART1_OUTFUNC);
    CHECK_RETURN (ret_code);
    sprintf (name,"%s%d","rec",i);
    ret_code = krui_setUnitName (unit_no, name);
    CHECK_RETURN (ret_code);

    unit_pos.x = Col + ColOffset;
    unit_pos.y = Row + RowOffset;
    krui_setUnitPosition( unit_no, &unit_pos );

    if ( (CCol == Col) ||
         ((CUnits%CRow != 0) && (Row != CRow) && 
          ((CUnits-i)%(CRow-Row) == 0) && (Col == CCol-1)
         )
       ) 
    {
      Row++;
      Col = 1;
    } else {
      Col++;
    } /*if*/

  }


  /*  Create Delay layer units that correspond to a recognition unit */

  RowOffset = 4;
  ColOffset = 2 * ICol + CCol + 6;
  Row = 1;
  Col = 1;

  for (i = 1; i <= CUnits; i++)
    {
    unit_no = krui_createDefaultUnit();
    if (unit_no < 0)  CHECK_RETURN( unit_no );
    ret_code = krui_setUnitTType( unit_no, HIDDEN );
    CHECK_RETURN( ret_code );
    ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_DEL);
    CHECK_RETURN (ret_code);
    ret_code = krui_setUnitOutFunc (unit_no, ART1_OUTFUNC);
    CHECK_RETURN (ret_code);
    sprintf (name,"%s%d","del",i);
    ret_code = krui_setUnitName (unit_no, name);
    CHECK_RETURN (ret_code);

    unit_pos.x = Col + ColOffset;
    unit_pos.y = Row + RowOffset;
    krui_setUnitPosition( unit_no, &unit_pos );

    if ( (CCol == Col) ||
         ((CUnits%CRow != 0) && (Row != CRow) && 
          ((CUnits-i)%(CRow-Row) == 0) && (Col == CCol-1)
         )
       ) 
    {
      Row++;
      Col = 1;
    } else {
      Col++;
    } /*if*/

  }

  /* Create d1 unit */
  RowOffset = 1;
  ColOffset = 2 * ICol + CCol + 6;
  Row = 1;
  Col = 1;
  unit_no = krui_createDefaultUnit();
  if (unit_no < 0)  CHECK_RETURN( unit_no );
  ret_code = krui_setUnitTType( unit_no, HIDDEN );
  CHECK_RETURN( ret_code );
  ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_D);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitOutFunc (unit_no, ART1_OUTFUNC);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitName (unit_no, "d1");
  CHECK_RETURN (ret_code);

  unit_pos.x = Col + ColOffset;
  unit_pos.y = Row + RowOffset;
  krui_setUnitPosition( unit_no, &unit_pos );

  /* Create d2 unit */
  RowOffset = 1;
  ColOffset = 2 * ICol + 2* CCol + 7;
  Row = 1;
  Col = 1;
  unit_no = krui_createDefaultUnit();
  if (unit_no < 0)  CHECK_RETURN( unit_no );
  ret_code = krui_setUnitTType( unit_no, HIDDEN );
  CHECK_RETURN( ret_code );
  ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_D);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitOutFunc (unit_no, ART1_OUTFUNC);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitName (unit_no, "d2");
  CHECK_RETURN (ret_code);

  unit_pos.x = Col + ColOffset;
  unit_pos.y = Row + RowOffset;
  krui_setUnitPosition( unit_no, &unit_pos );

  /* Create d3 unit */
  RowOffset = 1;
  ColOffset = 2 * ICol + 3 * CCol + 9;
  Row = 1;
  Col = 1;
  unit_no = krui_createDefaultUnit();
  if (unit_no < 0)  CHECK_RETURN( unit_no );
  ret_code = krui_setUnitTType( unit_no, HIDDEN );
  CHECK_RETURN( ret_code );
  ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_D);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitOutFunc (unit_no, ART1_OUTFUNC);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitName (unit_no, "d3");
  CHECK_RETURN (ret_code);

  unit_pos.x = Col + ColOffset;
  unit_pos.y = Row + RowOffset;
  krui_setUnitPosition( unit_no, &unit_pos );

  /*  Create Reset layer units */

  RowOffset = 4;
  ColOffset = 2*ICol + 2*CCol + 7;
  Row = 1;
  Col = 1;

  /* Create Site Table Entries for Reset layer units */
  ret_code = krui_createSiteTableEntry (ART1_SITE_NAME_RST_SELF,
					   ART1_SITE_FUNC_RST_SELF);
  CHECK_RETURN (ret_code);

  ret_code = krui_createSiteTableEntry (ART1_SITE_NAME_RST_SIGNAL,
					   ART1_SITE_FUNC_RST_SIGNAL);
  CHECK_RETURN (ret_code);


  for (i = 1; i <= CUnits; i++)
    {
    unit_no = krui_createDefaultUnit();
    if (unit_no < 0)  CHECK_RETURN( unit_no );
    ret_code = krui_setUnitTType( unit_no, HIDDEN );
    CHECK_RETURN( ret_code );
    ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_RST);
    CHECK_RETURN (ret_code);
    sprintf (name,"%s%d","rst",i);
    ret_code = krui_setUnitName (unit_no, name);
    CHECK_RETURN (ret_code);

    unit_pos.x = Col + ColOffset;
    unit_pos.y = Row + RowOffset;
    krui_setUnitPosition( unit_no, &unit_pos );

    if ( (CCol == Col) ||
         ((CUnits%CRow != 0) && (Row != CRow) && 
          ((CUnits-i)%(CRow-Row) == 0) && (Col == CCol-1)
         )
       ) 
    {
      Row++;
      Col = 1;
    } else {
      Col++;
    } /*if*/

    ret_code = krui_setCurrentUnit (unit_no);
    CHECK_RETURN (ret_code);

    /* Add the two Sites */
    ret_code = krui_addSite (ART1_SITE_NAME_RST_SELF);
    CHECK_RETURN (ret_code);
    ret_code = krui_addSite (ART1_SITE_NAME_RST_SIGNAL);
    CHECK_RETURN (ret_code);
  }




  /*  Create Gain1 unit */

  /* Create Site Table Entries for the two Sites */

  ret_code = krui_createSiteTableEntry (ART1_SITE_NAME_REC_G1,
                                         ART1_SITE_FUNC_REC_G1);
  CHECK_RETURN (ret_code);

  ret_code = krui_createSiteTableEntry (ART1_SITE_NAME_INP_G1,
                                         ART1_SITE_FUNC_INP_G1);
  CHECK_RETURN (ret_code);

  ColOffset = ICol + 2 + (int) ICol/2;
  RowOffset = 1;
  Col = 1;
  Row = 1;

  unit_no = krui_createDefaultUnit();
  if (unit_no < 0)  CHECK_RETURN( unit_no );
  ret_code = krui_setUnitTType( unit_no, HIDDEN );
  CHECK_RETURN( ret_code );
  ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_G1);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitName (unit_no, "g1");
  CHECK_RETURN (ret_code);

  unit_pos.x = Col + ColOffset;
  unit_pos.y = Row + RowOffset;
  krui_setUnitPosition( unit_no, &unit_pos );


  ret_code = krui_setCurrentUnit (unit_no);
  CHECK_RETURN (ret_code);

  /* Add the two Sites */
  ret_code = krui_addSite (ART1_SITE_NAME_INP_G1);
  CHECK_RETURN (ret_code);
  ret_code = krui_addSite (ART1_SITE_NAME_REC_G1);
  CHECK_RETURN (ret_code);


  ColOffset = 1;
  RowOffset = 4 + IRow + 2;
  Col = 1;
  Row = 1;

  /*  Create ResetI unit */

  ret_code = krui_createSiteTableEntry (ART1_SITE_NAME_INP_RI,
                                         ART1_SITE_FUNC_INP_RI);
  CHECK_RETURN (ret_code);

  ret_code = krui_createSiteTableEntry (ART1_SITE_NAME_RHO_RI,
                                         ART1_SITE_FUNC_RHO_RI);
  CHECK_RETURN (ret_code);

  unit_no = krui_createDefaultUnit();
  if (unit_no < 0)  CHECK_RETURN( unit_no );
  ret_code = krui_setUnitTType( unit_no, HIDDEN );
  CHECK_RETURN( ret_code );
  ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_RI);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitName (unit_no, "ri");
  CHECK_RETURN (ret_code);

  unit_pos.x = Col + ColOffset;
  unit_pos.y = Row + RowOffset;
  krui_setUnitPosition( unit_no, &unit_pos );

  ret_code = krui_setCurrentUnit (unit_no);
  CHECK_RETURN (ret_code);

  /* Add the two Sites */
  ret_code = krui_addSite (ART1_SITE_NAME_INP_RI);
  CHECK_RETURN (ret_code);
  ret_code = krui_addSite (ART1_SITE_NAME_RHO_RI);
  CHECK_RETURN (ret_code);


  ColOffset = ICol + 2;
  RowOffset = 4 + IRow + 2;
  Col = 1;
  Row = 1;

  /*  Create ResetC unit */
  unit_no = krui_createDefaultUnit();
  if (unit_no < 0)  CHECK_RETURN( unit_no );
  ret_code = krui_setUnitTType( unit_no, HIDDEN );
  CHECK_RETURN( ret_code );
  ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_RC);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitName (unit_no, "rc");
  CHECK_RETURN (ret_code);

  unit_pos.x = Col + ColOffset;
  unit_pos.y = Row + RowOffset;
  krui_setUnitPosition( unit_no, &unit_pos );

  ColOffset = 2 * ICol + 5;
  RowOffset = 4 + CRow + 2;
  Col = 1;
  Row = 1;

  /*  Create Reset general unit */
  unit_no = krui_createDefaultUnit();
  if (unit_no < 0)  CHECK_RETURN( unit_no );
  ret_code = krui_setUnitTType( unit_no, HIDDEN );
  CHECK_RETURN( ret_code );
  ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_RG);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitName (unit_no, "rg");
  CHECK_RETURN (ret_code);

  unit_pos.x = Col + ColOffset;
  unit_pos.y = Row + RowOffset;
  krui_setUnitPosition( unit_no, &unit_pos );



  ColOffset = 2 * ICol + 3 * CCol + 9;
  RowOffset = 4 + (int) CRow/3;
  Row = 1;
  Col = 1;

  /*  Create Classified unit */
  unit_no = krui_createDefaultUnit();
  if (unit_no < 0)  CHECK_RETURN( unit_no );
  ret_code = krui_setUnitTType( unit_no, HIDDEN );
  CHECK_RETURN( ret_code );
  ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_CL);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitName (unit_no, "cl");
  CHECK_RETURN (ret_code);

  unit_pos.x = Col + ColOffset;
  unit_pos.y = Row + RowOffset;
  krui_setUnitPosition( unit_no, &unit_pos );


  ColOffset = 2 * ICol + 3 * CCol + 9;
  RowOffset = 4 + 2 * (int) CRow/3;
  Row = 1;
  Col = 1;

  /*  Create Not Classifiable unit */

  unit_no = krui_createDefaultUnit();
  if (unit_no < 0)  CHECK_RETURN( unit_no );
  ret_code = krui_setUnitTType( unit_no, HIDDEN );
  CHECK_RETURN( ret_code );
  ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_NCL);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitName (unit_no, "nc");
  CHECK_RETURN (ret_code);

  unit_pos.x = Col + ColOffset;
  unit_pos.y = Row + RowOffset;
  krui_setUnitPosition( unit_no, &unit_pos );


  ColOffset = 1;
  RowOffset = 4 + IRow + 4;
  Col = 1;
  Row = 1;

  /*  Create RHO unit */
  unit_no = krui_createDefaultUnit();
  if (unit_no < 0)  CHECK_RETURN( unit_no );
  ret_code = krui_setUnitTType( unit_no, HIDDEN );
  CHECK_RETURN( ret_code );
  ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_RHO);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitName (unit_no, "rho");
  CHECK_RETURN (ret_code);

  unit_pos.x = Col + ColOffset;
  unit_pos.y = Row + RowOffset;
  krui_setUnitPosition( unit_no, &unit_pos );


  RowOffset = 1;
  ColOffset = 2 * ICol + 5;
  Row = 1;
  Col = 1;

  /*  Create G2 unit */
  unit_no = krui_createDefaultUnit();
  if (unit_no < 0)  CHECK_RETURN( unit_no );
  ret_code = krui_setUnitTType( unit_no, HIDDEN );
  CHECK_RETURN( ret_code );
  ret_code = krui_setUnitActFunc (unit_no, ART1_ACTF_G2);
  CHECK_RETURN (ret_code);
  ret_code = krui_setUnitName (unit_no, "g2");
  CHECK_RETURN (ret_code);

  unit_pos.x = Col + ColOffset;
  unit_pos.y = Row + RowOffset;
  krui_setUnitPosition( unit_no, &unit_pos );






  /* Make Connections now */


  /* Connections to comparison units */
  FOR_ALL_ART1_CMP_UNITS (i) {
    ret_code = krui_setCurrentUnit (i);
    CHECK_RETURN( ret_code );

    /* from input units */
    ret_code = krui_createLink ((i-IUnits), 0.0);
    CHECK_RETURN( ret_code );

    /* from delay units */
    FOR_ALL_ART1_DEL_UNITS (j) {
       ret_code = krui_createLink (j,0.0);
       CHECK_RETURN (ret_code);
    }

    /* from g1 unit */
    ret_code = krui_createLink (G1_UNIT, 0.0);

  }

  /* Connections to recognition units */
  FOR_ALL_ART1_REC_UNITS (i) {
     ret_code = krui_setCurrentUnit (i);
     CHECK_RETURN (ret_code);

     /* from comparison units */
     FOR_ALL_ART1_CMP_UNITS (j) {
        ret_code = krui_createLink (j,0.0);
        CHECK_RETURN (ret_code);
     }

     /* from reset units */
     ret_code = krui_createLink ((i+2*CUnits+3), 0.0);
     CHECK_RETURN (ret_code);

     /* from reset general unit */
     ret_code = krui_createLink (RG_UNIT, 0.0);
     CHECK_RETURN (ret_code);

     /* from gain 2 unit */
     ret_code = krui_createLink (G2_UNIT, 0.0);
     CHECK_RETURN (ret_code);
  }

  /* Connections to delay units */
  FOR_ALL_ART1_DEL_UNITS (i) {
     ret_code = krui_setCurrentUnit (i);
     CHECK_RETURN (ret_code);

     /* from recognition units */
     ret_code = krui_createLink ((i-CUnits), 0.0);
     CHECK_RETURN (ret_code);
  }

  /* Connections to local reset units */
  FOR_ALL_ART1_RST_UNITS (i) {
     ret_code = krui_setCurrentUnit (i);
     CHECK_RETURN (ret_code);

     /* from itself */
     ret_code = krui_setSite (ART1_SITE_NAME_RST_SELF);
     CHECK_RETURN (ret_code);
     ret_code = krui_createLink (i, 0.0);
     CHECK_RETURN (ret_code);

     /* from delay units */
     ret_code = krui_setSite (ART1_SITE_NAME_RST_SIGNAL);
     CHECK_RETURN (ret_code);
     ret_code = krui_createLink ((i-CUnits-3), 0.0);
     CHECK_RETURN (ret_code);

     /* from reset general unit */
     ret_code = krui_createLink (RG_UNIT, 0.0);
     CHECK_RETURN (ret_code);
  }


  /* Connections to units d1-d3 */

  /* d1 */
  ret_code = krui_setCurrentUnit (D_UNIT (1));
  CHECK_RETURN (ret_code);

  /* from all delay units */
  FOR_ALL_ART1_DEL_UNITS (j) {
     ret_code = krui_createLink (j, 0.0);
     CHECK_RETURN (ret_code);
  }

  /* d2 */
  ret_code = krui_setCurrentUnit (D_UNIT (2));
  CHECK_RETURN (ret_code);

  /* from d1 */
  ret_code = krui_createLink (D_UNIT (1), 0.0);
  CHECK_RETURN (ret_code);

  /* d3 */
  ret_code = krui_setCurrentUnit (D_UNIT (3));
  CHECK_RETURN (ret_code);

  /* from d2 */
  ret_code = krui_createLink (D_UNIT (2), 0.0);
  CHECK_RETURN (ret_code);


  /* connections to gain 1 unit */
  ret_code = krui_setCurrentUnit (G1_UNIT);
  CHECK_RETURN (ret_code);

  /* from input units */
  ret_code = krui_setSite (ART1_SITE_NAME_INP_G1);
  CHECK_RETURN (ret_code);
  FOR_ALL_ART1_INP_UNITS (j) {
     ret_code = krui_createLink (j, 0.0);
     CHECK_RETURN (ret_code);
  }

  /* from recognition units */
  ret_code = krui_setSite (ART1_SITE_NAME_REC_G1);
  CHECK_RETURN (ret_code);
  FOR_ALL_ART1_REC_UNITS (j) {
     ret_code = krui_createLink (j, 0.0);
     CHECK_RETURN (ret_code);
  }


  /* connections to ResetI unit */
  ret_code = krui_setCurrentUnit (RI_UNIT);
  CHECK_RETURN (ret_code);

  /* Site for input units */
  ret_code = krui_setSite (ART1_SITE_NAME_INP_RI);
  CHECK_RETURN (ret_code);

  /* from input units */
  FOR_ALL_ART1_INP_UNITS (j) {
     ret_code = krui_createLink (j, 0.0);
     CHECK_RETURN (ret_code);
  }

  /* Site for Unit RHO */
  ret_code = krui_setSite (ART1_SITE_NAME_RHO_RI);
  CHECK_RETURN (ret_code);

  /* from RHO unit */
  ret_code = krui_createLink (RHO_UNIT, 0.0);
  CHECK_RETURN (ret_code);


  /* connections to ResetC unit */
  ret_code = krui_setCurrentUnit (RC_UNIT);
  CHECK_RETURN (ret_code);

  /* from comparison units */
  FOR_ALL_ART1_CMP_UNITS (j) {
     ret_code = krui_createLink (j, 0.0);
     CHECK_RETURN (ret_code);
  }


  /* connections to reset general unit */
  ret_code = krui_setCurrentUnit (RG_UNIT);
  CHECK_RETURN (ret_code);

  /* from ResetI unit */
  ret_code = krui_createLink (RI_UNIT, 0.0);
  CHECK_RETURN (ret_code);

  /* from ResetC unit */
  ret_code = krui_createLink (RC_UNIT, 0.0);
  CHECK_RETURN (ret_code);


  /* connections to Classified unit */
  ret_code = krui_setCurrentUnit (CL_UNIT);
  CHECK_RETURN (ret_code);

  /* from d3 unit */
  ret_code = krui_createLink (D_UNIT (3), 0.0);
  CHECK_RETURN (ret_code);

  /* from reset general unit */
  ret_code = krui_createLink (RG_UNIT, 0.0);
  CHECK_RETURN (ret_code);

  /* from gain 2 unit */
  ret_code = krui_createLink (G2_UNIT, 0.0);
  CHECK_RETURN (ret_code);


  /* connections to Not Classifiable unit */
  ret_code = krui_setCurrentUnit (NCL_UNIT);
  CHECK_RETURN (ret_code);

  /* from local reset units */
  FOR_ALL_ART1_RST_UNITS (j) {
     ret_code = krui_createLink (j, 0.0);
     CHECK_RETURN (ret_code);
  }

  /* connections to RHO unit */
  ret_code = krui_setCurrentUnit (RHO_UNIT);
  CHECK_RETURN (ret_code);

  /* from itself */
  ret_code = krui_createLink (RHO_UNIT, 0.0);
  CHECK_RETURN (ret_code);


  /* connections to G2 unit */
  ret_code = krui_setCurrentUnit (G2_UNIT);
  CHECK_RETURN (ret_code);

  /* from input units */
  FOR_ALL_ART1_INP_UNITS (j) {
     ret_code = krui_createLink (j, 0.0);
     CHECK_RETURN (ret_code);
  } /*FOR_ALL_ART1_INP_UNITS*/




  /*  set the update function  */
  ret_code = krui_setUpdateFunc (ART1_UPDATE_FUNC_NAME);
  CHECK_RETURN( ret_code );

  /* set the learning function */
  ret_code = krui_setLearnFunc (ART1_LEARN_FUNC_NAME);

  ui_NumberOfLearnParamsChanged();
  ui_NumberOfUpdateParamsChanged();
  return (ret_code);

} /* bn_art1_createNet () */






/*****************************************************************************
  FUNCTION : bn_art1_donePROC

  PURPOSE  : Callback function for DONE button in art1
  NOTES    :

  UPDATE   : 20.1.1993
******************************************************************************/

static void bn_art1_donePROC (void)

{

   XtDestroyWidget (baseWidget);
   bn_art1_open = 0;

}



/*****************************************************************************
  FUNCTION : bn_art1_createPROC

  PURPOSE  : Callback function for CREATE NET button in art1
  NOTES    :

  UPDATE   : 20.1.1993
******************************************************************************/

static void bn_art1_createPROC (void)

{
  int units[2], rows[2];
  int NoOfLayers = 2;

  if (bn_basics_check_existingNetwork()) {

     bn_basics_getValues (NoOfLayers, units, rows, art1UnitWidget, art1RowWidget);

     if (bn_basics_checkValues(NoOfLayers, units, rows)) {
        bn_art1_createNet (units[0], rows[0], units[1], rows[1]);
        bn_basics_refresh ();
        ui_confirmOk ("Network created!");
     } /*if*/

  } /*if*/

  return;

}



/*****************************************************************************
  FUNCTION : bn_createART1

  PURPOSE  : create ART1 panel
  NOTES    :

  UPDATE   : 20.1.1993
******************************************************************************/

void bn_createART1(void)

{
  bn_basics_createART (ART1_MODEL, &baseWidget, &bn_art1_open,
                       art1UnitWidget, art1RowWidget,
                       (XtCallbackProc) bn_art1_createPROC,
                       (XtCallbackProc) bn_art1_donePROC);


}

