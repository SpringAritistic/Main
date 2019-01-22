// 本文件定义了用于控件表格界面状态的一些类

#pragma once

enum steel_grid_condition_action
{
	SGCA_HIDE,		// 满足条件隐藏，不满足则显示，某一行中只要有一个单元格设置要隐藏，就会把整个行都隐藏起来
	SGCA_READONLY	// 满足条件设为只读，不满足则设为可读写
};


// 定位一个钢筋表中的数据区单元格
class X_FRAMEWORK_API steel_grid_pos
{
public:
	steel_grid_pos();
	steel_grid_pos(IN const CString& id, IN int index); // index取值只能从0到5
	~steel_grid_pos();

public:
	void set(IN const CString& id, IN int index); // index取值只能从0到5

public:
	CString		m_id;				// 表格中设置的标识（包含了主标识和子标识），不是概念筋文档中的标识
	int			m_value_index;		// 有效数据列的第几列，0 为内容中的第一列（注意不是表格的第一列）
};


// 当满足时按steel_grid_condition_action的行为进行操作，不满足时按steel_grid_condition_action的相反行为进行操作
// 当有多个条件时，条件之间的关系为“与”，如果有条件之间需要“或”关系，请转化为“与”再设置
// 单元格中如果不是文本，而是控件，设置条件时的字符串值为对应单元格的text，比如：复选框选中
// 时为“1”，没有选中时为“0”，其它的同理，具体可咨询表格控件的负责人，不能按照概念筋文档中的值传过来。
// 条件中的单元格字符串区分大小写！！！
// 注意:
//   1.本数据结构仅用于实现简单的界面状态设置，请不要设置复杂关系，如嵌套修改状态的条件或递归修改状态的条件。
//   2.如果对同一个单元格设置了多个条件，则后面的条件会覆盖之前的设置
//   3.如果同一个单元格既会影响其它单元格的状态，同时也需要被其实单元格条件影响，那么执行的先后顺序受放入vector时的先后顺序影响，原则上是先放入先执行。
class X_FRAMEWORK_API steel_grid_conditions
{
public:
	steel_grid_conditions();
	steel_grid_conditions(IN const CString& cond_id, IN int cond_index, IN const CString& cond_action, IN const CString& cond_value, IN const CString& des_id, IN int des_index, IN steel_grid_condition_action des_action); // cond_action只能传入_T("==")和_T("!=")
	steel_grid_conditions(IN const steel_grid_pos& cond_pos, IN const CString& cond_action, IN const CString& cond_value, IN const steel_grid_pos& des_pos, IN steel_grid_condition_action des_action); // cond_action只能传入_T("==")和_T("!=")
	steel_grid_conditions(IN bool is_ok, IN const steel_grid_pos& des_pos, IN steel_grid_condition_action des_action); // is_ok为true表示条件满足，否则表示不满足(具体的条件由使用者自己控制，并将条件的满足情况传给is_ok即可)
	~steel_grid_conditions();

public:
	void set(IN const CString& cond_id, IN int cond_index, IN const CString& cond_action, IN const CString& cond_value, IN const CString& des_id, IN int des_index, IN steel_grid_condition_action des_action); // cond_action只能传入_T("==")和_T("!=")
	void set(IN const steel_grid_pos& cond_pos, IN const CString& cond_action, IN const CString& cond_value, IN const steel_grid_pos& des_pos, IN steel_grid_condition_action des_action); // cond_action只能传入_T("==")和_T("!=")
	void set(IN bool is_ok, IN const steel_grid_pos& des_pos, IN steel_grid_condition_action des_action); // is_ok为true表示条件满足，否则表示不满足(具体的条件由使用者自己控制，并将条件的满足情况传给is_ok即可)
	void and(IN const CString& cond_id, IN int cond_index, IN const CString& cond_action, IN const CString& cond_value); // 附加And条件，cond_action只能传入_T("==")和_T("!=")
	void and(IN const steel_grid_pos& cond_pos, IN const CString& cond_action , IN const CString& cond_value); // 附加And条件，cond_action只能传入_T("==")和_T("!=")
	void clear_all_conditions();
	int get_condition_count() const;
	const steel_grid_pos get_condition_pos_at(int index) const;
	const CString get_condition_type_at(int index) const; // 操作未设置返回_T("")，其它时候返回_T("==")或者_T("!=")
	const CString get_condition_value_at(int index) const;
	const steel_grid_pos get_des_pos() const;
	enum steel_grid_condition_action get_condition_action() const;
	bool is_force_condition() const; // 为true表示此操作不需要任何条件限制，总是执行指定的操作
	bool is_condition_exist_and_true() const; // 条件是否满足

private:
	// 一个条件
	typedef struct
	{
		steel_grid_pos	cell_condition;
		CString			condition_action_text; // 为_T("==")时条件单元格与strVAlue相等当作真，否则当作假，反之，当为_T("!=")时相同原理
		CString			value;
	}single_condition;

	bool							m_is_force_condition;			// 为true表示不限定源单元格取值，即此条件中指定的目标单元格总是按条件中指定的操作进行处理
	bool							m_is_condition_exist_and_true;	// 条件是否满足（m_is_force_condition为true时使用）

	std::vector<single_condition>	m_conditions;					// 参照的条件
	steel_grid_pos					m_des_cell;						// 要操作的单元格
	steel_grid_condition_action		m_action;						// 要操作的行为
};

