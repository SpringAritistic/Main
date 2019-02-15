//1/24/2019  6:10:56 PM
#pragma  once
#ifndef _TOOL_GRIDCONTROL_H_
#define _TOOL_GRIDCONTROL_H_
_HM_GridControl_BEGIN
//针对枚举类型
template<class T,class U>
bool IsProChange(T& state, U PRO, bool val)
{
	return bool(state & PRO) == val;
}

template<class T, class U>
void SetProEnable(T& state, U PRO, bool val)
{
	if (val)
		state |= PRO;
	else
		state &= ~PRO;

}

template<class T, class U>
bool EnsureProChange(T& state, U PRO, bool val)
{
	bool change = IsProChange(state, PRO, val);
	SetProEnable(state, PRO, val);
	return change;
}





_HM_GridControl_END
#endif
