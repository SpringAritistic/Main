//1/24/2019  5:37:30 PM
#pragma  once
#ifndef _STRTOOL_GRIDCONTROL_H_
#define _STRTOOL_GRIDCONTROL_H_
_HM_GridControl_BEGIN

void split_string(std::vector<CString>& result, const CString& text, const CString& split = _T(" ,;"));
 std::vector<CString> split_string(const CString& text, const CString& split = _T(" ,;"));

void split_matrix_string(std::vector<std::vector<CString>>& result, const CString& text,
	TCHAR row_split = _T('\n'), TCHAR column_split = _T('\t'));

 void merge_string(CString& result, const std::vector<CString>& all_strings, const CString& split = _T(","), bool end_with_split = false);
 CString merge_string(const std::vector<CString>& all_strings, const CString& split = _T(","), bool end_with_split = false);
 void merge_matrix_string(CString& result, const std::vector<std::vector<CString>>& all_strings,
	TCHAR row_split = _T('\n'), TCHAR column_split = _T('\t'), bool end_with_split = false);


vector<size_t>FindIndexs(const std::vector<CString>& inStr, const std::vector<CString>& findStr);

//允许多行显示
CSize GetTextExtent(CDC& dc, const CString& str=_T(" "),bool ExtX=true,bool ExtY=true);
void ShowText(CDC& dc, const CString& str, int EachHeight);//
//字符串中的 \r\n 替换为 \n
void SelfRepalceToNormReturn(CString &str);
CString RepalceToNormReturn(const CString &str);

//多行文字
vector<CString>GetMutiStr(const CString& text);





_HM_GridControl_END
#endif
