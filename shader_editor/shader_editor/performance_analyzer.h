#pragma once

#include "osg/Node"

typedef void analyse_result;

class performance_analyzer
{
public:
	enum item_e
	{
		integer = 1,
		floating
	};
	typedef struct item_s {
		item_s (int val){i=val; item = item_e::integer;}
		item_s (long val){i=val; item = item_e::integer;}
		item_s (float val){i=val; item = item_e::floating;}
		item_s (double val){i=val; item = item_e::floating;}
		int item;
		long i;
		double f;

	} item_t;

	performance_analyzer(void (*drawfunction)());
	~performance_analyzer(void);

	void (*drawfunction)();

	analyse_result * analyse(osg::Node * tree_to_analyse, int max_depth);

	void setDrawFunction(void (*drawfunction)());

protected:
	analyse_result * analyseRecursive(osg::Node * node_to_analyse, int max_depth);
};

