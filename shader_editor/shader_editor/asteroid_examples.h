#ifndef __asteroid_examples__protection__
#define __asteroid_examples__protection__
#include <osg/Node>
#include <QString>
#include <osg/Geometry>


namespace asteroid_examples
{
	osg::Node * example_asteroid_gaps( QString * status_text, const char * displacement_map_filename = NULL );

	osg::Node * example_asteroid_better_uvs( QString * status_text, const char * displacement_map_filename = NULL);

	enum config_bitstring_enum
	{
		NO_HIGHLIGHT = 0x1,
		TESSLOD_NODE = 0x2
	};

	osg::Node * example_asteroid_better_uvs_fixed_textures( QString * status_text , unsigned long config_bitstring = 0, float TessLevel = 16.0f );

	osg::Node * example_asteroid_dynamic_tessellation_16( QString * status_text  );

	osg::Node * example_asteroid_dynamic_tessellation_64( QString * status_text, unsigned long config_bitstring = 0 );

	osg::Node * example_asteroid_dynamic_tessellation_16_with_Common_LOD( QString * status_text  );

	osg::Node * example_asteroid_dynamic_tessellation_64_with_Common_LOD( QString * status_text  );

	osg::Node * example_asteroid_dynamic_tessellation_16_with_Common_LOD_cust_range( QString * status_text  );

	osg::Node * example_asteroid_dynamic_tessellation_64_with_Common_LOD_cust_range( QString * status_text  );

	osg::Node * example_asteroid_LOD_no_tessellation( QString * status_text );

	osg::Node * example_asteroid_LOD_no_tessellation_cust_range( QString * status_text );

	osg::Node * returnNodeFromFile ( QString * status_text, QString file_name );

	osg::Geometry * getGeometryOfFirstGeodeFromTheTree(osg::Node * target);

	void addTangentBitangentVectorsToExistingGeometry(osg::Geometry * targetGeometry, osg::PrimitiveSet::Mode mode);

	bool buildTexture( const char * filename, const char * texture_uniform_name,  osg::StateSet * targetSet, int texture_unit, QString * status_text );

	osg::Program * buildProgramFromFiles( QString VS, QString TCS, QString TES, QString GS, QString FS );
};

#endif