//1/24/2019  5:37:30 PM
#pragma  once
#ifndef _STRTOOL_GRIDCONTROL_H_
#define _STRTOOL_GRIDCONTROL_H_
_HM_GridControl_BEGIN

void split_string(std::vector<CString>& result, const CString& text, const CString& split = _T(" ,;"));
vector<size_t>FindIndexs(const std::vector<CString>& inStr, const std::vector<CString>& findStr);

//允许多行显示
CSize GetTextExtent(CClientDC& dc,const CString& str);
void ShowText(CClientDC& dc, const CString& str,int EachHeight);//
//多行文字
vector<CString>GetMutiStr(const CString& text);





_HM_GridControl_END
#endif
