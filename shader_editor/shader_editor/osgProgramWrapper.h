#pragma once
#include <osg/Program>
class osgProgram
{
public:
	osgProgram(void);
	~osgProgram(void);

	osg::ref_ptr<osg::Program> thisOsgProgram;
};

