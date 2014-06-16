#include "performance_analyzer.h"
#include "osg/ref_ptr"

performance_analyzer::performance_analyzer( void (*drawfunction)() )
{
	this->setDrawFunction(drawfunction);
}


performance_analyzer::~performance_analyzer(void)
{
}

void performance_analyzer::setDrawFunction( void (*drawfunction)() )
{
	this->drawfunction = drawfunction;
}

analyse_result * performance_analyzer::analyse(osg::Node * tree_to_analyse, int max_depth)
{
	osg::ref_ptr<osg::Node> backup = tree_to_analyse;

	return NULL;
}

analyse_result * performance_analyzer::analyseRecursive( osg::Node * node_to_analyse, int max_depth )
{
	return NULL;
}
