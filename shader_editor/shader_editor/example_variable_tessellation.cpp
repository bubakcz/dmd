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

void shader_editor::loadExampleVariableTesselation()
{
	logger::debug("funkce loadExampleVariableTesselation");
	const int tex_unit = 0;
	const unsigned tesmap_gluint = 0;

	QString text_loc	 = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "../", tr("Image Files (*.png *.jpg *.bmp)"));
	if (text_loc.isEmpty()==true)
		return;
	osg::Image * tesmap_image = osgDB::readImageFile(text_loc.toStdString());
	if (tesmap_image==NULL)
		return;

	osg::ref_ptr<osg::Texture2D> tesmap = new osg::Texture2D;

	tesmap->setDataVariance(osg::Object::DYNAMIC);
	tesmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::NEAREST);
	tesmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::NEAREST);

	tesmap->setImage(tesmap_image);

	osg::Geode *geode=new osg::Geode();
	osg::Geometry *geometry = new osg::Geometry();
 	geometry->setUseVertexBufferObjects(true);
 	int VertexCount = sizeof(flat_geometry::lfg_vertices)/sizeof(flat_geometry::lfg_vertices[0]);
 	int IndexCount = sizeof(flat_geometry::lfg_indexes) / sizeof(flat_geometry::lfg_indexes[0]);

	osg::Vec3Array* vertices = new osg::Vec3Array();
	for(int i=0;i<VertexCount;i++){
		vertices->push_back(osg::Vec3(flat_geometry::lfg_vertices[i].x,flat_geometry::lfg_vertices[i].y,flat_geometry::lfg_vertices[i].z));
	}


	osg::Vec3Array* normals = new osg::Vec3Array();
	for(int i=0;i<VertexCount;i++){
		normals->push_back(osg::Vec3(flat_geometry::lfg_vertices[i].nx,flat_geometry::lfg_vertices[i].ny,flat_geometry::lfg_vertices[i].nz));
	}

	osg::UIntArray* tessfactors = new osg::UIntArray();
	osg::Vec2Array* tesmap_texcoords = new osg::Vec2Array();
	for(int i=0;i<VertexCount;i++){
		tesmap_texcoords->push_back(osg::Vec2(flat_geometry::lfg_vertices[i].u,flat_geometry::lfg_vertices[i].v));
		float point = tesmap_image->getColor(tesmap_texcoords->back()).g();
		point = int(point * 64.0)>0 ? point * 64.0 : 1.0;
		tessfactors->push_back(point);
	}
	normals->setName("myNormal");
	tesmap_texcoords->setName("myTexCoord");

// 	geometry->setVertexArray(vertices);
// 	geometry->setNormalArray(normals);
// 	geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
// 	geometry->setTexCoordArray(tex_unit,tesmap_texcoords);

	geometry->setVertexAttribArray(0,vertices);
	geometry->setVertexAttribBinding(0,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(1,normals);
	geometry->setVertexAttribBinding(1,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(2,tesmap_texcoords);
	geometry->setVertexAttribBinding(2,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(3,tessfactors);
	geometry->setVertexAttribBinding(3,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,IndexCount,flat_geometry::lfg_indexes));

// 	osg::BoundingBox bb = geometry->getBound();
// 
// 	geometry->setInitialBound(bb);

	geode->addDrawable(geometry);

	osg::ref_ptr<osg::Group> Groupnode = new osg::Group;
	Groupnode->addChild(geode);

/*
	if (!loadShader(ui.textEdit_VS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_VARIABILNI_TESELACE "/vartes_shader.vs"))) return;
	if (!loadShader(ui.textEdit_TCS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_VARIABILNI_TESELACE "/vartes_shader.tcs"))) return;
	if (!loadShader(ui.textEdit_TES, (char *)NULL, (EXAMPLE_FOLDER FOLDER_VARIABILNI_TESELACE "/vartes_shader.tes"))) return;
	if (!loadShader(ui.textEdit_FS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_VARIABILNI_TESELACE "/vartes_shader.fs"))) return;

	compileShaders(geode, FORCE_TESSELLATION_SH, NO_GEOMETRY_SH);*/


	osg::Program * p = shaderTab::buildProgramFromFiles((EXAMPLE_FOLDER FOLDER_VARIABILNI_TESELACE "/vartes_shader.vs"),
		(EXAMPLE_FOLDER FOLDER_VARIABILNI_TESELACE "/vartes_shader.tcs"),
		(EXAMPLE_FOLDER FOLDER_VARIABILNI_TESELACE "/vartes_shader.tes"),
		"",
		(EXAMPLE_FOLDER FOLDER_VARIABILNI_TESELACE "/vartes_shader.fs"));
	if (p==NULL)
	{
		return;
	}
	geode->getOrCreateStateSet()->setAttribute(p);

	osg::StateSet *state;
	state = geode->getOrCreateStateSet();
	osg::PolygonMode *polyModeObj = polyModeObj = static_cast< osg::PolygonMode* >( state->getAttribute( osg::StateAttribute::POLYGONMODE ));
	if ( !polyModeObj ) 
	{
		polyModeObj = new osg::PolygonMode;
		state->setAttribute( polyModeObj );    
	}

	polyModeObj->setMode(  osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );

 	osg::Program * this_program = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));
	this_program->addBindAttribLocation("myVertex",0);
	this_program->addBindAttribLocation("myNormal",1);
	this_program->addBindAttribLocation("myTexCoord",2);
	this_program->addBindAttribLocation("myTessFact",3);



	state->setTextureAttributeAndModes(tex_unit,tesmap,osg::StateAttribute::ON);
	this_program->setParameter(GL_PATCH_VERTICES,3);

	this_program->addBindFragDataLocation("FragColor",0);
 	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
 	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.0f, 0.75f, 0.75f)));
 	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
	state->addUniform(new osg::Uniform("tesmap", tex_unit));
	state->addUniform(new osg::Uniform("texture", tex_unit));

	ui.osgGLWidget->setSceneData(Groupnode);//newTransform);
	setupClean();
}
