//1/24/2019  6:10:56 PM
#pragma  once
#ifndef _TOOL_GRIDCONTROL_H_
#define _TOOL_GRIDCONTROL_H_
_HM_GridControl_BEGIN
//针对枚举类型
template<class T,class U>
bool SetProEnable(T& state, U PRO, bool val)
{
	if (bool(state & PRO) == val)
		return false;
	if (val)
		state |= PRO;
	else
		state &= ~PRO;
	return true;
}








_HM_GridControl_END
#endif
