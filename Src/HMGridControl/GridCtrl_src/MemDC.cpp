//11/22/2018  4:37:13 PM
#include "stdafx.h"
#include "MemDC.h"
_HM_GridControl_BEGIN

// constructor sets up the memory DC
HMMemDC::HMMemDC(CDC* pDC) : CDC()
{
	ASSERT(pDC != NULL);

	m_pDC = pDC;
	m_pOldBitmap = NULL;
#ifndef _WIN32_WCE_NO_PRINTING
	m_bMemDC = !pDC->IsPrinting();
#else
	m_bMemDC = FALSE;
#endif

	if (m_bMemDC)    // Create a Memory DC
	{
		pDC->GetClipBox(&m_rect);
		CreateCompatibleDC(pDC);
		m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
		m_pOldBitmap = SelectObject(&m_bitmap);
#ifndef _WIN32_WCE
		SetWindowOrg(m_rect.left, m_rect.top);
#endif
		// EFW - Bug fix - Fill background in case the user has overridden
		// WM_ERASEBKGND.  We end up with garbage otherwise.
		// CJM - moved to fix a bug in the fix.
		FillSolidRect(m_rect, pDC->GetBkColor());
	}
	else        // Make a copy of the relevent parts of the current DC for printing
	{
#if !defined(_WIN32_WCE) || ((_WIN32_WCE > 201) && !defined(_WIN32_WCE_NO_PRINTING))
		m_bPrinting = pDC->m_bPrinting;
#endif
		m_hDC = pDC->m_hDC;
		m_hAttribDC = pDC->m_hAttribDC;
	}

}

// Destructor copies the contents of the mem DC to the original DC
HMMemDC::~HMMemDC()
{
	if (m_bMemDC)
	{
		// Copy the offscreen bitmap onto the screen.
		m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
			this, m_rect.left, m_rect.top, SRCCOPY);

		//Swap back the original bitmap.
		SelectObject(m_pOldBitmap);
	}
	else {
		// All we need to do is replace the DC with an illegal value,
		// this keeps us from accidently deleting the handles associated with
		// the CDC that was passed to the constructor.
		m_hDC = m_hAttribDC = NULL;
	}
}

// Allow usage as a pointer
HMMemDC* HMMemDC::operator->() { return this; }

// Allow usage as a pointer
HMMemDC::operator HMMemDC*() { return this; }









_HM_GridControl_END
