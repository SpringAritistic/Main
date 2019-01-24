//11/22/2018  4:37:13 PM
#ifndef _MEMDC_GRIDCONTROL_H_
#define _MEMDC_GRIDCONTROL_H_
#pragma  once

_HM_GridControl_BEGIN

class HMMemDC : public CDC
{
public:

    // constructor sets up the memory DC
	HMMemDC(CDC* pDC);
    // Destructor copies the contents of the mem DC to the original DC
	~HMMemDC();

    // Allow usage as a pointer
	HMMemDC* operator->();
    // Allow usage as a pointer
	operator HMMemDC*();
private:
    CBitmap  m_bitmap;      // Offscreen bitmap
    CBitmap* m_pOldBitmap;  // bitmap originally found in HMMemDC
    CDC*     m_pDC;         // Saves CDC passed in constructor
    CRect    m_rect;        // Rectangle of drawing area.
    BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.
};

/////////////////////////////////////////////////////////////////////////////



_HM_GridControl_END
#endif
