#ifndef __cobblestone_examples__protection__
#define __cobblestone_examples__protection__
#include <osg/Node>
#include <QString>
#include <osg/Geometry>

#define EQUAL_SPACING "equal_spacing"
#define FRACTIONAL_ODD_SPACING "fractional_odd_spacing"
#define FRACTIONAL_EVEN_SPACING "fractional_even_spacing"

namespace cobblestone_examples
{
	enum config_bistring_enum
	{
		DEFAULT = 0x0,
		USE_EDGE_FLAGS = 0x1
	};

	osg::Node * example_cobblestone_simple_dynamic( QString * status_text, const char * spacing = EQUAL_SPACING );
	
	osg::Node * example_cobblestone_numerous_dynamic( QString * status_text );

	osg::Node * example_cobblestone_simple_dynamic_fixed_gaps( QString * status_text, const char * alternativeShaderProgram = "cobblestone_shader_v2", config_bistring_enum bitstring = cobblestone_examples::DEFAULT );

	osg::Node * example_cobblestone_numerous_dynamic_fixed_gaps( QString * status_text , const char * alternativeShaderProgram = "cobblestone_shader_v2", config_bistring_enum bitstring = cobblestone_examples::DEFAULT);
}
#endif