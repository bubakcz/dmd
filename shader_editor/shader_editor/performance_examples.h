#ifndef __performance_examples__protection__
#define __performance_examples__protection__
#include <osg/Node>
#include <QString>
#include <osg/Geometry>

namespace performance_examples
{
	osg::Node * example_performance_dynamic_tess_shader( QString * status_text );

	osg::Node * example_performance_dynamic_tess_node( QString * status_text );

	osg::Node * example_performance_dynamic_tess_fixed( QString * status_text );

	osg::Node * example_performance_dynamic_tess_cobblestone_combined( QString * status_text );

	osg::Node * example_performance_dynamic_tess_cobblestone_shader( QString * status_text );

	osg::Node * example_performance_dynamic_tess_cobblestone_shader_modified( QString * status_text );

	osg::Node * example_performance_asteroid_tessellated( QString * status_text );

	osg::Node * example_performance_asteroid_full( QString * status_text );
};

#endif