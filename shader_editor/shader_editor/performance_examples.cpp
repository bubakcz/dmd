#include <osg/Node>
#include <QString>
#include <osg/Geometry>
#include "performance_examples.h"
#include "asteroid_examples.h"
#include "cobblestone_examples.h"
#include "TessellationLOD.h"
#include <osg/PositionAttitudeTransform>

osg::Node * performance_examples::example_performance_dynamic_tess_shader( QString * status_text  )
{
	return asteroid_examples::example_asteroid_dynamic_tessellation_64(status_text);
}

osg::Node * performance_examples::example_performance_dynamic_tess_node( QString * status_text )
{
	osg::Node * node = 	asteroid_examples::example_asteroid_dynamic_tessellation_64(status_text, 
		asteroid_examples::config_bitstring_enum::NO_HIGHLIGHT | asteroid_examples::config_bitstring_enum::TESSLOD_NODE);

	osg::TessellationLOD * tlod = new osg::TessellationLOD(32.0f, 50.0f);

	tlod->addChild(node);

	return tlod;
}

osg::Node * performance_examples::example_performance_dynamic_tess_fixed( QString * status_text )
{
	return asteroid_examples::example_asteroid_better_uvs_fixed_textures(status_text, asteroid_examples::config_bitstring_enum::NO_HIGHLIGHT, 64.0f);

}

osg::Node * performance_examples::example_performance_dynamic_tess_cobblestone_combined( QString * status_text )
{
	osg::Node * node = 	cobblestone_examples::example_cobblestone_numerous_dynamic_fixed_gaps(status_text, "cobblestone_shader_v2_combined", cobblestone_examples::USE_EDGE_FLAGS);

	osg::TessellationLOD * tlod = new osg::TessellationLOD(32.0f, 30.0f);

	tlod->addChild(node);

	return tlod;
}

osg::Node * performance_examples::example_performance_dynamic_tess_cobblestone_shader( QString * status_text )
{
	return cobblestone_examples::example_cobblestone_numerous_dynamic_fixed_gaps(status_text);
}

osg::Node * performance_examples::example_performance_dynamic_tess_cobblestone_shader_modified( QString * status_text )
{
	osg::Node * node = 	cobblestone_examples::example_cobblestone_simple_dynamic_fixed_gaps(status_text, "cobblestone_shader_v2_modified_2");

	osg::TessellationLOD * tlod = NULL;//new osg::TessellationLOD(32.0f, 30.0f);





	osg::Group * cobblestoneGroup = new osg::Group;

	cobblestoneGroup->getOrCreateStateSet()->addUniform(new osg::Uniform("DecreaseRate",2.25f));

	float geometry_range = 2.0f;

	for(int i = - 5; i <= 5; i++)
	{
		float x = i * geometry_range;
		for(int j = - 5; j <= 5; j++)
		{
			float y = j * geometry_range;
			osg::PositionAttitudeTransform * locationNode = new osg::PositionAttitudeTransform();
			tlod = new osg::TessellationLOD(32.0f, 30.0f);
			tlod->addChild(node);
			locationNode->setPosition(osg::Vec3d(x,y,0.0f));
			locationNode->addChild(tlod);

			cobblestoneGroup->addChild(locationNode);
		}
	}

	return cobblestoneGroup;
}

osg::Node * performance_examples::example_performance_asteroid_tessellated( QString * status_text )
{
	return asteroid_examples::example_asteroid_better_uvs_fixed_textures(status_text, asteroid_examples::NO_HIGHLIGHT, 22.0f);
}

osg::Node * performance_examples::example_performance_asteroid_full( QString * status_text )
{
	QString example_folder = QString("examples/full_model_asteroid/%1");

	osg::Node * fullAsteroid = asteroid_examples::returnNodeFromFile(status_text, example_folder.arg("asteroid_full.ive"));

	if(!fullAsteroid)
	{
		status_text->clear();
		status_text->append("Nepodarilo se otevrit soubor s node: %1");
		*status_text = status_text->arg(example_folder.arg("asteroid_full.ive"));
		return NULL;
	}
	osg::Geometry * fullAsteroid_geometry = asteroid_examples::getGeometryOfFirstGeodeFromTheTree(fullAsteroid);
	if(fullAsteroid_geometry)
		 asteroid_examples::addTangentBitangentVectorsToExistingGeometry(fullAsteroid_geometry, osg::PrimitiveSet::TRIANGLES);

	const int normal_texture_unit = 0;

	asteroid_examples::buildTexture( example_folder.arg("fake_normal_map.tga").toStdString().c_str(), "normal_texture",  fullAsteroid->getOrCreateStateSet(), normal_texture_unit, status_text );



	osg::Program * p = asteroid_examples::buildProgramFromFiles(example_folder.arg("sd_shader.vs"),
		"",
		"",
		"",
		example_folder.arg("sd_shader.fs"));
	if (p==NULL)
	{
		status_text->clear();
		*status_text = QString ("asteroid_examples : example_asteroid_gaps : program building failure");
		return NULL;
	}
	fullAsteroid->getOrCreateStateSet()->setAttribute(p);

	int vertices_binding = 0;
	int normals_binding = 1;
	int uvcoords_binding = 2;

	osg::StateSet *state;
	state = fullAsteroid->getOrCreateStateSet();

	//osg::Program * this_program = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));

	p->addBindAttribLocation("myVertex",vertices_binding);
	p->addBindAttribLocation("myNormal",normals_binding);
	p->addBindAttribLocation("myTexCoord",uvcoords_binding);
	p->addBindAttribLocation("myTangents",3);
	p->addBindAttribLocation("myBitangents",4);
	p->addBindFragDataLocation("FragColor",0);

	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.6f, 0.6f, 0.6f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));

	return fullAsteroid;
}

