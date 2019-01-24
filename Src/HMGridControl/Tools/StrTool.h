//1/24/2019  5:37:30 PM
#pragma  once
#ifndef _STRTOOL_GRIDCONTROL_H_
#define _STRTOOL_GRIDCONTROL_H_
_HM_GridControl_BEGIN

void split_string(std::vector<CString>& result, const CString& text, const CString& split = _T(" ,;"));
vector<size_t>FindIndexs(const std::vector<CString>& inStr, const std::vector<CString>& findStr);








_HM_GridControl_END
#endif
