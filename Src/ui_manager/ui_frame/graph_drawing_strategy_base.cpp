#include "stdafx.h"
#include "graph_drawing_strategy_base.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_SERIAL(graph_drawing_strategy_base, CObject, 1)
graph_drawing_strategy_base::graph_drawing_strategy_base()
{
}

graph_drawing_strategy_base::~graph_drawing_strategy_base()
{

}

bool graph_drawing_strategy_base::draw(const CString& file_path_name)
{
	return false;
}
