#ifndef DB_MINI_DUMP_H_
#define DB_MINI_DUMP_H_


typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS;

class mini_dump_helper 
{
private:
	mini_dump_helper();
	~mini_dump_helper();

public: 
	static bool m_need_popup_report_window; // 是否弹出消息框,默认不弹出来，此时如有错误将直接不提示并退出（应小周老师要求），可以在WinMain或类似的地方改为true
	static void enable_dump(bool popup_report_window = false); // 在程序初始化时调用一次即可
	
private:
	static LONG WINAPI exception_top_level_filter(EXCEPTION_POINTERS* ex_point);
};

#endif // DB_MINI_DUMP_H_