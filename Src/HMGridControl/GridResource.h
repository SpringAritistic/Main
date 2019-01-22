//11/23/2018  10:08:25 AM
#pragma  once
#ifndef _GRIDRESOURCE_GRIDCONTROL_H_
#define _GRIDRESOURCE_GRIDCONTROL_H_
_HM_GridControl_BEGIN
// CCheckComboEdit window
#define IDC_COMBOEDIT 1001
// Use this as the classname when inserting this control as a custom control
// in the MSVC++ dialog editor
#define GRIDCTRL_CLASSNAME    _T("HMGridCtrl")  // Window class name
#define IDC_INPLACE_CONTROL   8                  // ID of inplace edit controls


// Cell states
#define GVIS_FOCUSED            0x0001
#define GVIS_SELECTED           0x0002
#define GVIS_DROPHILITED        0x0004
#define GVIS_READONLY           0x0008
#define GVIS_FIXED              0x0010
#define GVIS_FIXEDROW           0x0020
#define GVIS_FIXEDCOL           0x0040
#define GVIS_MODIFIED           0x0080

// Cell data mask
#define GVIF_TEXT               LVIF_TEXT
#define GVIF_IMAGE              LVIF_IMAGE
#define GVIF_PARAM              LVIF_PARAM
#define GVIF_STATE              LVIF_STATE
#define GVIF_BKCLR              (GVIF_STATE<<1)
#define GVIF_FGCLR              (GVIF_STATE<<2)
#define GVIF_FORMAT             (GVIF_STATE<<3)
#define GVIF_FONT               (GVIF_STATE<<4)
#define GVIF_MARGIN             (GVIF_STATE<<5)
#define GVIF_ALL                (GVIF_TEXT|GVIF_IMAGE|GVIF_PARAM|GVIF_STATE|GVIF_BKCLR|GVIF_FGCLR| \
                                 GVIF_FORMAT|GVIF_FONT|GVIF_MARGIN)


///////////////////////////////////////////////////////////////////////////////////
// Defines
///////////////////////////////////////////////////////////////////////////////////

// Grid line/scrollbar selection
#define GVL_NONE                0L      // Neither
#define GVL_HORZ                1L      // Horizontal line or scrollbar
#define GVL_VERT                2L      // Vertical line or scrollbar
#define GVL_BOTH                3L      // Both

// Autosizing option
#define GVS_DEFAULT             0
#define GVS_HEADER              1       // Size using column fixed cells data only
#define GVS_DATA                2       // Size using column non-fixed cells data only
#define GVS_BOTH                3       // Size using column fixed and non-fixed

// Cell Searching options
#define GVNI_FOCUSED            0x0001
#define GVNI_SELECTED           0x0002
#define GVNI_DROPHILITED        0x0004
#define GVNI_READONLY           0x0008
#define GVNI_FIXED              0x0010
#define GVNI_MODIFIED           0x0020

#define GVNI_ABOVE              LVNI_ABOVE
#define GVNI_BELOW              LVNI_BELOW
#define GVNI_TOLEFT             LVNI_TOLEFT
#define GVNI_TORIGHT            LVNI_TORIGHT
#define GVNI_ALL                (LVNI_BELOW|LVNI_TORIGHT|LVNI_TOLEFT)
#define GVNI_AREA               (LVNI_BELOW|LVNI_TORIGHT)

// Hit test values (not yet implemented)
#define GVHT_DATA               0x0000
#define GVHT_TOPLEFT            0x0001
#define GVHT_COLHDR             0x0002
#define GVHT_ROWHDR             0x0004
#define GVHT_COLSIZER           0x0008
#define GVHT_ROWSIZER           0x0010
#define GVHT_LEFT               0x0020
#define GVHT_RIGHT              0x0040
#define GVHT_ABOVE              0x0080
#define GVHT_BELOW              0x0100

// Messages sent to the grid's parent (More will be added in future)
#define GVN_BEGINDRAG           LVN_BEGINDRAG        // LVN_FIRST-9 // BeginDrag
#define GVN_BEGINLABELEDIT      LVN_BEGINLABELEDIT   // LVN_FIRST-5 // BeginLabelEdit
#define GVN_BEGINRDRAG          LVN_BEGINRDRAG						// BeginRdrag
#define GVN_COLUMNCLICK         LVN_COLUMNCLICK						// ColumnClick
#define GVN_DELETEITEM          LVN_DELETEITEM 						// DeleteItem
#define GVN_ENDLABELEDIT        LVN_ENDLABELEDIT     // LVN_FIRST-6 // EndLabelEdit
#define GVN_SELCHANGING         LVN_ITEMCHANGING 					// SelChanging
#define GVN_SELCHANGED          LVN_ITEMCHANGED 					// SelChanged
#define GVN_GETDISPINFO         LVN_GETDISPINFO  					// GetDispinfo
#define GVN_ODCACHEHINT         LVN_ODCACHEHINT  					// OdCacheHint







_HM_GridControl_END
#endif
