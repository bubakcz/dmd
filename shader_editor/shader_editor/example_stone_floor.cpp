#include "shader_editor.h"
#include <iostream>
#include <osgDB/ReadFile>

#include "large_flat_geometry.h"
#include "piece_of_stone.h"

#include <QFileDialog>
#include <QMessageBox>
#include <Windows.h>
#include <QFile>
#include "osgNodeTreeItem.h"
#include <QDebug>
#include <osg/PositionAttitudeTransform>
#include <osg/CullFace>
#include <osg/Texture2D>
#include <osg/PolygonMode>
#include <osgDB/ReadFile>
#include "shaderTab.h"

#include "logger.h"

#define EXAMPLE_FOLDER "examples"
#define FOLDER_TESELACE "/teselace"
#define FOLDER_VARIABILNI_TESELACE "/prom_teselace"
#define FOLDER_SIMPLE_DISPLACEMENT "/simple_displacement"
#define FOLDER_SIMPLE_DISPLACEMENT_V2 "/simple_displacement_v2"
#define FOLDER_SIMPLE_DISPLACEMENT_V2_DD "/simple_displacement_v2_dd"
#define FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/displacement_with_normal"
#define FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/displacement_with_normal_2"
#define FORCE_TESSELLATION_SH true
#define NO_TESSELLATION_SH false
#define FORCE_GEOMETRY_SH true
#define NO_GEOMETRY_SH false

void shader_editor::loadExampleSimpleDisplacement_normalMapping_2()
{

	logger::debug("funkce loadExampleSimpleDisplacement_normalMapping_2");


	const int tex_unit = 0;
	const unsigned tesmap_gluint = 0;

	std::string text_loc =  EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/disp_map_plane.tga";
	osg::Image * displmap_image = osgDB::readImageFile(text_loc);
	if (displmap_image==NULL)
	{
		logger::debug("displmap_image==NULL");
		return;
	}
	else
		logger::debug("displmap_image loaded");

	text_loc =  EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/norm_map_plane.tga";
	osg::Image * normmap_image = osgDB::readImageFile(text_loc);
	if (normmap_image==NULL)
	{
		logger::debug("normmap_image==NULL");
		return;
	}
	else
		logger::debug("normmap_image loaded");

	text_loc =  EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/diff_map_plane.tga";
	osg::Image * diffmap_image = osgDB::readImageFile(text_loc);
	if (diffmap_image==NULL)
	{
		logger::debug("diffmap_image==NULL");
		return;
	}
	else
		logger::debug("diffmap_image loaded");

	text_loc =  EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/spec_map_plane.tga";
	osg::Image * specmap_image = osgDB::readImageFile(text_loc);
	if (specmap_image==NULL)
	{
		logger::debug("specmap_image==NULL");
		return;
	}
	else
		logger::debug("specmap_image loaded");

	osg::ref_ptr<osg::Texture2D> displmap = new osg::Texture2D;

	displmap->setDataVariance(osg::Object::DYNAMIC);
	displmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	displmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

	displmap->setImage(displmap_image);

	osg::ref_ptr<osg::Texture2D> normmap = new osg::Texture2D;

	normmap->setDataVariance(osg::Object::DYNAMIC);
	normmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	normmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

	normmap->setImage(normmap_image);

	osg::ref_ptr<osg::Texture2D> diffmap = new osg::Texture2D;

	diffmap->setDataVariance(osg::Object::DYNAMIC);
	diffmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	diffmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

	diffmap->setImage(diffmap_image);

	osg::ref_ptr<osg::Texture2D> specmap = new osg::Texture2D;

	specmap->setDataVariance(osg::Object::DYNAMIC);
	specmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	specmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

	specmap->setImage(specmap_image);

	osg::Geode *geode=new osg::Geode();
	osg::Geometry *geometry = new osg::Geometry();
	geometry->setUseVertexBufferObjects(true);
	int VertexCount = sizeof(flat_geometry::lfg_vertices)/sizeof(flat_geometry::lfg_vertices[0]);
	int IndexCount = sizeof(flat_geometry::lfg_indexes) / sizeof(flat_geometry::lfg_indexes[0]);

	osg::Vec2Array* tesmap_texcoords = new osg::Vec2Array();
	osg::Vec3Array* vertices = new osg::Vec3Array();
	osg::Vec3Array* normals = new osg::Vec3Array();
	osg::Vec3Array* tangents = new osg::Vec3Array(VertexCount);
	osg::Vec3Array* binormals = new osg::Vec3Array(VertexCount);
	//osg::Vec3Array* n_vector = new osg::Vec3Array(VertexCount);
	vertex_struct np0,np1,np2;
	for(int i=0;i<VertexCount;i++){
		vertices->push_back(osg::Vec3(flat_geometry::lfg_vertices[i].x,flat_geometry::lfg_vertices[i].y,flat_geometry::lfg_vertices[i].z));
		normals->push_back(osg::Vec3(flat_geometry::lfg_vertices[i].nx,flat_geometry::lfg_vertices[i].ny,flat_geometry::lfg_vertices[i].nz));
		tesmap_texcoords->push_back(osg::Vec2(flat_geometry::lfg_vertices[i].u,flat_geometry::lfg_vertices[i].v));
		// 		tangent_vector->push_back(np0.nx, np0.ny, np0.nz);
		// 		cotangent_vector->push_back(np1.nx, np1.ny, np1.nz);
		// 		n_vector->push_back(np2.nx, np2.ny, np2.nz);
	}
	logger::debug(QString("finished basic geometry with <").append(QString::number(normals->size())).append("> items"));
	int k;
	osg::Vec3 ZAxis(0.0,0.0,1.0), YAxis(0.0,1.0,0.0);
	for (int i=0; i<VertexCount; /*i+=3*/i++)
	{

		//computeTangentSpaceMatrix(meteor::vertices[meteor::indexes[i+k]], meteor::vertices[meteor::indexes[i+(k+1)%3]], meteor::vertices[meteor::indexes[i+(k+2)%3]], np0, np1, np2);
		//(*tangent_vector)[meteor::indexes[i+k]] = osg::Vec3(np0.nx, np0.ny, np0.nz);
		//(*cotangent_vector)[meteor::indexes[i+k]] = osg::Vec3(np1.nx, np1.ny, np1.nz);
		//(*n_vector)[meteor::indexes[i+k]] = osg::Vec3(np2.nx, np2.ny, np2.nz);
		osg::Vec3 tangent, binormal;
		osg::Vec3 c1 = normals->at(i) ^ ZAxis;
		osg::Vec3 c2 = normals->at(i) ^ YAxis;
		if (c1.length2()>c2.length2())
			tangent = c1;
		else
			tangent = c2;
		tangent.normalize();
		binormal = normals->at(i) ^ tangent;
		binormal.normalize();

		(*tangents)[i] = tangent;
		(*binormals)[i] = binormal;
		//tangents->push_back(tangent);
		//binormals->push_back(binormal);
	}

	logger::debug(QString("finished tangents with <").append(QString::number(tangents->size())).append("> items"));

	// 	geometry->setVertexArray(vertices);
	// 	geometry->setNormalArray(normals);
	// 	geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	// 	geometry->setTexCoordArray(tex_unit,tesmap_texcoords);

	logger::debug(QString("adding AttribArrays with sizes <").append(QString::number(vertices->size())).append(">")
		.append("<").append(QString::number(normals->size())).append(">")
		.append("<").append(QString::number(tesmap_texcoords->size())).append(">")
		.append("<").append(QString::number(tangents->size())).append(">")
		.append("<").append(QString::number(binormals->size())).append(">"));
	geometry->setVertexAttribArray(0,vertices);
	geometry->setVertexAttribBinding(0,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(1,normals);
	geometry->setVertexAttribBinding(1,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(2,tesmap_texcoords);
	geometry->setVertexAttribBinding(2,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(3,tangents);
	geometry->setVertexAttribBinding(3,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(4,binormals);
	geometry->setVertexAttribBinding(4,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	logger::debug(QString("addPrimitiveSet <PATCHES> <").append(QString::number(IndexCount)).append("> items"));
	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,IndexCount,flat_geometry::lfg_indexes));

	// 	osg::BoundingBox bb = geometry->getBound();
	// 
	// 	geometry->setInitialBound(bb);

	geode->addDrawable(geometry);

	osg::ref_ptr<osg::Group> Groupnode = new osg::Group;
	Groupnode->addChild(geode);

/*
	if (!loadShader(ui.textEdit_VS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/sd_shader.vs"))) return;
	if (!loadShader(ui.textEdit_TCS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/sd_shader.tcs"))) return;
	if (!loadShader(ui.textEdit_TES, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/sd_shader.tes"))) return;
	if (!loadShader(ui.textEdit_GS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/sd_shader.gs"))) return;
	if (!loadShader(ui.textEdit_FS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/sd_shader.fs"))) return;

	compileShaders(geode, FORCE_TESSELLATION_SH, FORCE_GEOMETRY_SH);*/
	osg::Program * p = shaderTab::buildProgramFromFiles((EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/sd_shader.vs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/sd_shader.tcs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/sd_shader.tes"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/sd_shader.gs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/sd_shader.fs"));
	if (p==NULL)
	{
		return;
	}
	geode->getOrCreateStateSet()->setAttribute(p);

	osg::StateSet *state;
	state = geode->getOrCreateStateSet();
/*
	osg::PolygonMode *polyModeObj = polyModeObj = static_cast< osg::PolygonMode* >( state->getAttribute( osg::StateAttribute::POLYGONMODE ));
	if ( !polyModeObj ) 
	{
		polyModeObj = new osg::PolygonMode;
		state->setAttribute( polyModeObj );    
	}

	polyModeObj->setMode(  osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );*/

	osg::Program * this_program = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));
	this_program->addBindAttribLocation("myVertex",0);
	this_program->addBindAttribLocation("myNormal",1);
	this_program->addBindAttribLocation("myTexCoord",2);
	this_program->addBindAttribLocation("myTangents",3);
	this_program->addBindAttribLocation("myBinormals",4);


	int normalMapping = 1;
	int specularMapping = 1;
	const int dispmap_tex_unit = 0;
	const int normmap_tex_unit = 1;
	const int diffmap_tex_unit = 2;
	const int specmap_tex_unit = 3;
	state->setTextureAttributeAndModes(dispmap_tex_unit,displmap,osg::StateAttribute::ON);
	state->setTextureAttributeAndModes(normmap_tex_unit,normmap,osg::StateAttribute::ON);
	state->setTextureAttributeAndModes(diffmap_tex_unit,diffmap,osg::StateAttribute::ON);
	state->setTextureAttributeAndModes(specmap_tex_unit,specmap,osg::StateAttribute::ON);
	this_program->setParameter(GL_PATCH_VERTICES,3);

	this_program->addBindFragDataLocation("FragColor",0);
	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.75f, 0.75f, 0.75f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
	state->addUniform(new osg::Uniform("dispmap", dispmap_tex_unit));
	state->addUniform(new osg::Uniform("normmap", normmap_tex_unit));
	state->addUniform(new osg::Uniform("diffmap", diffmap_tex_unit));
	state->addUniform(new osg::Uniform("specmap", specmap_tex_unit));
	state->addUniform(new osg::Uniform("useNormalMapping", normalMapping));
	//state->addUniform(new osg::Uniform("useNormalMapping", specularMapping));
	//	state->addUniform(new osg::Uniform("texture", tex_unit));
	state->addUniform(new osg::Uniform("TessLevelInner",16.0f));
	state->addUniform(new osg::Uniform("TessLevelOuter",16.0f));
	state->addUniform(new osg::Uniform("DisplacementFactor",0.05f));

	ui.osgGLWidget->setSceneData(Groupnode);//newTransform);
	setupClean();
}
