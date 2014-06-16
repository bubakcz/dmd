/*
    Copyright (C) 2013  Martin Kuèeròák
    Contact: email:  xkucer64@stud.fit.vutbr.cz
             email2: bubak671415@seznam.cz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    Description:
    (kódování: WindowsCP1250)
    
    funkce shader_editor
    
    Naèítání pøíkladù
    
    Prosím o prominutí za ten bordel, bylo to psaný v zaèátcích, když jsem 
    nemìl možnost chystat pøíklady programovì
        
*/

#include "blender_vertex_struct.h"

#include "asteroid_examples.h"

#include "miscellaneous_examples.h"

#include "performance_examples.h"

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
#include <osgUtil/Optimizer>
#include <osgUtil/MeshOptimizers>
#include <QString>
#include "logger.h"
#include "largeSpaceScene.h"
#include "cobblestone_examples.h"
#include "meteor_reduced_better_uv_backup_2.h"

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


/*
void shader_editor::loadExampleInitial()
{
	osg::ref_ptr<osg::Geode>geode=new osg::Geode();
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

	geometry->setUseDisplayList (false);
	geometry->setUseVertexBufferObjects(true);

	const unsigned int Faces[] = {
		2, 1, 0,
		3, 2, 0,
		4, 3, 0,
		5, 4, 0,
		1, 5, 0,

		11, 6,  7,
		11, 7,  8,
		11, 8,  9,
		11, 9,  10,
		11, 10, 6,

		1, 2, 6,
		2, 3, 7,
		3, 4, 8,
		4, 5, 9,
		5, 1, 10,

		2,  7, 6,
		3,  8, 7,
		4,  9, 8,
		5, 10, 9,
		1, 6, 10 
	};

	int IndexCount = sizeof(Faces) / sizeof(unsigned int);
	const float Verts[] = {
		0.000f,  0.000f,  1.000f,
		0.894f,  0.000f,  0.447f,
		0.276f,  0.851f,  0.447f,
		-0.724f,  0.526f,  0.447f,
		-0.724f, -0.526f,  0.447f,
		0.276f, -0.851f,  0.447f,
		0.724f,  0.526f, -0.447f,
		-0.276f,  0.851f, -0.447f,
		-0.894f,  0.000f, -0.447f,
		-0.276f, -0.851f, -0.447f,
		0.724f, -0.526f, -0.447f,
		0.000f,  0.000f, -1.000f 
	};

	int VertexCount = sizeof(Verts)/sizeof(float);
	osg::Vec3Array* vertices = new osg::Vec3Array();
	for(int i=0;i<VertexCount;i+=3){
		vertices->push_back(osg::Vec3(Verts[i],Verts[i+1],Verts[i+2]));
	}
	geometry->setVertexArray(vertices);
	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,IndexCount,Faces));

	osg::VertexBufferObject * test_vbo = geometry->getOrCreateVertexBufferObject();



	geode->addDrawable(geometry);
	geode->dirtyBound();

	osg::ref_ptr<osg::Group> Groupnode = new osg::Group;
	Groupnode->addChild(geode);

	if (!loadShader(ui.textEdit_VS, (char *)NULL, "shader_init/simple_vertex.vs")) return;
	if (!loadShader(ui.textEdit_GS, (char *)NULL, "shader_init/simple_geometry.gs")) return;
	if (!loadShader(ui.textEdit_FS, (char *)NULL, "shader_init/simple_fragment.fs")) return;
	osg::StateSet *state = geode->getOrCreateStateSet();	
	actualStateSet = state;
	//state-> compileShaders(geode, NO_TESSELLATION_SH, NO_GEOMETRY_SH);
	char * msg;
	osg::Program * program;
	if((program = ui.osgGLWidget->compileShader(&(ui.textEdit_VS->toPlainText()),NULL,NULL,NULL,&(ui.textEdit_FS->toPlainText()),&msg))==NULL)
	{
		writeToConsoleQt(QtWarningMsg,msg);
	}

	//program->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 3);
	//program->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
	//program->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
	//program->setParameter(GL_PATCH_VERTICES,3);
	//program->addBindAttribLocation("osg_Vertex",0);
	
	//program->addBindFragDataLocation("FragColor",0);
	

	state->setAttributeAndModes(new osg::CullFace()); 
	state->setAttributeAndModes(program, osg::StateAttribute::ON);

	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.0f, 0.75f, 0.75f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
	//state->setuse

  geode->setCullingActive(false);
	

	ui.osgGLWidget->setSceneData(Groupnode);
	setupClean();
}
*/

void shader_editor::loadExampleSceneTreeTest()
{
	osg::Geode *geode=new osg::Geode();
	osg::Geometry *geometry = new osg::Geometry();
	const unsigned int Faces[] = {
		0,  1, 2,
		1,  2, 3};

		int IndexCount = sizeof(Faces) / sizeof(Faces[0]);
		const float Verts[] = {
			-1.000f,  0.000f,  -1.000f,
			1.000f,  0.000f,  -1.000f,
			1.000f,  0.000f,  1.000f,
			-1.000f,  0.000f,  1.000f };

			const float Normals[] = {
				0.000f,  1.000f,  0.000f,
				0.000f,  1.000f,  0.000f,
				0.000f,  1.000f,  0.000f,
				0.000f,  1.000f,  0.000f };

				int VertexCount = sizeof(Verts)/sizeof(float);
				osg::Vec3Array* vertices = new osg::Vec3Array();
				for(int i=0;i<VertexCount;i+=3){
					vertices->push_back(osg::Vec3(Verts[i],Verts[i+1],Verts[i+2]));
				}
				osg::Vec3Array* normals = new osg::Vec3Array();
				for(int i=0;i<VertexCount;i+=3){
					normals->push_back(osg::Vec3(Normals[i],Normals[i+1],Normals[i+2]));
				}
				geometry->setVertexArray(vertices);
				geometry->setNormalArray(normals);
				geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,IndexCount,Faces));

				geode->addDrawable(geometry);

				geode->dirtyBound();

				osg::ref_ptr<osg::Group> Groupnode = new osg::Group;
				//Groupnode->addChild(loadedModel2.get());
				//Groupnode->addChild(geode);


/*

				if (!loadShader(ui.textEdit_VS, (char *)NULL, ("shader_init/simple_tess_vertex.vs"))) return;
// 				if (!loadShader(ui.textEdit_TCS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_TESELACE "/st_shader.tcs"))) return;
// 				if (!loadShader(ui.textEdit_TES, (char *)NULL, (EXAMPLE_FOLDER FOLDER_TESELACE "/st_shader.tes"))) return;
// 				if (!loadShader(ui.textEdit_GS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_TESELACE "/st_shader.gs"))) return;
				if (!loadShader(ui.textEdit_FS, (char *)NULL, ("shader_init/simple_tess_fragment.fs"))) return;

				compileShaders(geode, NO_TESSELLATION_SH, NO_GEOMETRY_SH);*/
				osg::Program * p = shaderTab::buildProgramFromFiles(("shader_init/simple_tess_vertex.vs"),
					"",
					"",
					"",
					("shader_init/simple_tess_fragment.fs"));
				if (p==NULL)
				{
					return;
				}
				geode->getOrCreateStateSet()->setAttribute(p);

				osg::StateSet *state;
				state = geode->getOrCreateStateSet();
				// 			osg::Program *program = state->getAttribute(osg::StateAttribute::Type::PROGRAM);
				// 			program->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 3);
				// 			program->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
				// 			program->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
				// 			program->setParameter(GL_PATCH_VERTICES,3);
				state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
				state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.0f, 0.75f, 0.75f)));
				state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
				state->addUniform(new osg::Uniform("TessLevelInner",4.0f));
				state->addUniform(new osg::Uniform("TessLevelOuter",4.0f));

				osg::Geode *geode1 = new osg::Geode(*geode);
				osg::Geode *geode2 = new osg::Geode(*geode);
				osg::Geode *geode3 = new osg::Geode(*geode);
				osg::Geode *geode4 = new osg::Geode(*geode);

				osg::PositionAttitudeTransform * trans1 = new osg::PositionAttitudeTransform();
				osg::PositionAttitudeTransform * trans2 = new osg::PositionAttitudeTransform();
				osg::PositionAttitudeTransform * trans3 = new osg::PositionAttitudeTransform();
				osg::PositionAttitudeTransform * trans4 = new osg::PositionAttitudeTransform();

				trans1->addChild(geode1);
				trans1->setPosition(osg::Vec3(-2,0,0));
				trans2->addChild(geode2);
				trans1->setPosition(osg::Vec3(2,0,0));
				trans3->addChild(geode3);
				trans1->setPosition(osg::Vec3(-2,2,2));
				trans4->addChild(geode4);
				trans1->setPosition(osg::Vec3(2,2,-2));
				//trans1->

				Groupnode->addChild(trans1);
				Groupnode->addChild(trans2);
				Groupnode->addChild(trans3);
				Groupnode->addChild(trans4);
				ui.osgGLWidget->setSceneData(Groupnode);
				/*			rebuildSceneTree();*/
				setupClean();
}

void shader_editor::loadExampleSimpleDisplacement()
{
	const int tex_unit = 0;
	const unsigned tesmap_gluint = 0;

	QString text_loc	 = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "../", tr("Image Files (*.png *.jpg *.bmp)"));
	if (text_loc.isEmpty()==true)
		return;
	osg::Image * displmap_image = osgDB::readImageFile(text_loc.toStdString());
	if (displmap_image==NULL)
		return;

	osg::ref_ptr<osg::Texture2D> displmap = new osg::Texture2D;

	displmap->setDataVariance(osg::Object::DYNAMIC);
	displmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	displmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

	displmap->setImage(displmap_image);

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

/*	osg::UIntArray* tessfactors = new osg::UIntArray();*/
	osg::Vec2Array* tesmap_texcoords = new osg::Vec2Array();
	for(int i=0;i<VertexCount;i++){
		tesmap_texcoords->push_back(osg::Vec2(flat_geometry::lfg_vertices[i].u,flat_geometry::lfg_vertices[i].v));

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
// 	geometry->setVertexAttribArray(3,tessfactors);
// 	geometry->setVertexAttribBinding(3,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,IndexCount,flat_geometry::lfg_indexes));

	// 	osg::BoundingBox bb = geometry->getBound();
	// 
	// 	geometry->setInitialBound(bb);

	geode->addDrawable(geometry);

	osg::ref_ptr<osg::Group> Groupnode = new osg::Group;
	Groupnode->addChild(geode);



/*
	if (!loadShader(ui.textEdit_VS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT "/sd_shader.vs"))) return;
	if (!loadShader(ui.textEdit_TCS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT "/sd_shader.tcs"))) return;
	if (!loadShader(ui.textEdit_TES, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT "/sd_shader.tes"))) return;
	if (!loadShader(ui.textEdit_GS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT "/sd_shader.gs"))) return;
	if (!loadShader(ui.textEdit_FS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT "/sd_shader.fs"))) return;

	compileShaders(geode, FORCE_TESSELLATION_SH, FORCE_GEOMETRY_SH);*/

	osg::Program * p = shaderTab::buildProgramFromFiles((EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT "/sd_shader.vs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT "/sd_shader.tcs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT "/sd_shader.tes"),
		"",
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT "/sd_shader.fs"));
	if (p==NULL)
	{

		return;
	}
	geode->getOrCreateStateSet()->setAttribute(p);

	osg::StateSet *state;
	state = geode->getOrCreateStateSet();
// 	osg::PolygonMode *polyModeObj = polyModeObj = static_cast< osg::PolygonMode* >( state->getAttribute( osg::StateAttribute::POLYGONMODE ));
// 	if ( !polyModeObj ) 
// 	{
// 		polyModeObj = new osg::PolygonMode;
// 		state->setAttribute( polyModeObj );    
// 	}
// 
// 	polyModeObj->setMode(  osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );

	osg::Program * this_program = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));
	this_program->addBindAttribLocation("myVertex",0);
	this_program->addBindAttribLocation("myNormal",1);
	this_program->addBindAttribLocation("myTexCoord",2);
//	this_program->addBindAttribLocation("myTessFact",3);



	state->setTextureAttributeAndModes(tex_unit,displmap,osg::StateAttribute::ON);
	this_program->setParameter(GL_PATCH_VERTICES,3);

	this_program->addBindFragDataLocation("FragColor",0);
	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.0f, 0.75f, 0.75f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
	state->addUniform(new osg::Uniform("dispmap", tex_unit));
//	state->addUniform(new osg::Uniform("texture", tex_unit));
	state->addUniform(new osg::Uniform("TessLevelInner",16.0f));
	state->addUniform(new osg::Uniform("TessLevelOuter",16.0f));
	state->addUniform(new osg::Uniform("DisplacementFactor",0.3f));

	ui.osgGLWidget->setSceneData(Groupnode);//newTransform);
	setupClean();
}
//
//void shader_editor::loadExampleSimpleDisplacement_complexModel()
//{
//	logger::debug("funkce loadExampleSimpleDisplacement_complexModel");
//	const int tex_unit = 0;
//	const unsigned tesmap_gluint = 0;
//
//	std::string text_loc = EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/disp_map.tga";
//	osg::Image * displmap_image = osgDB::readImageFile(text_loc);
//	if (displmap_image==NULL)
//		return;
//
//	osg::ref_ptr<osg::Texture2D> displmap = new osg::Texture2D;
//
//	displmap->setDataVariance(osg::Object::DYNAMIC);
//	displmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
//	displmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
//
//	displmap->setImage(displmap_image);
//
//	osg::Geode *geode=new osg::Geode();
//	osg::Geometry *geometry = new osg::Geometry();
//	geometry->setUseVertexBufferObjects(true);
//	int VertexCount = meteor::vertex_count[0];
//	int IndexCount = sizeof(meteor::indexes) / sizeof(meteor::indexes[0]);
//
//	osg::Vec2Array* tesmap_texcoords = new osg::Vec2Array();
//	osg::Vec3Array* vertices = new osg::Vec3Array();
//	osg::Vec3Array* normals = new osg::Vec3Array();
//	for(int i=0;i<VertexCount;i++){
//		vertices->push_back(osg::Vec3(meteor::vertices[i].x,meteor::vertices[i].y,meteor::vertices[i].z));
//		normals->push_back(osg::Vec3(meteor::vertices[i].nx,meteor::vertices[i].ny,meteor::vertices[i].nz));
//		tesmap_texcoords->push_back(osg::Vec2(meteor::vertices[i].u,meteor::vertices[i].v));
//	}
//
//
//// 	normals->setName("myNormal");
//// 	tesmap_texcoords->setName("myTexCoord");
//
//	geometry->setVertexAttribArray(0,vertices);
//	geometry->setVertexAttribBinding(0,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
//	geometry->setVertexAttribArray(1,normals);
//	geometry->setVertexAttribBinding(1,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
//	geometry->setVertexAttribArray(2,tesmap_texcoords);
//	geometry->setVertexAttribBinding(2,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
//
//	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,IndexCount,meteor::indexes));
//
//	geode->addDrawable(geometry);
//
//	osg::ref_ptr<osg::Group> Groupnode = new osg::Group;
//	Groupnode->addChild(geode);
//
//
//
///*
//	if (!loadShader(ui.textEdit_VS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.vs"))) return;
//	if (!loadShader(ui.textEdit_TCS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tcs"))) return;
//	if (!loadShader(ui.textEdit_TES, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tes"))) return;
//	if (!loadShader(ui.textEdit_GS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.gs"))) return;
//	if (!loadShader(ui.textEdit_FS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.fs"))) return;
//
//	compileShaders(geode, FORCE_TESSELLATION_SH, FORCE_GEOMETRY_SH);*/
//
//	osg::Program * p = shaderTab::buildProgramFromFiles((EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.vs"),
//		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tcs"),
//		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tes"),
//		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.gs"),
//		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.fs"));
//	if (p==NULL)
//	{
//		return;
//	}
//	geode->getOrCreateStateSet()->setAttribute(p);
//
//	osg::StateSet *state;
//	state = geode->getOrCreateStateSet();
//// 	osg::PolygonMode *polyModeObj = polyModeObj = static_cast< osg::PolygonMode* >( state->getAttribute( osg::StateAttribute::POLYGONMODE ));
//// 	if ( !polyModeObj ) 
//// 	{
//// 		polyModeObj = new osg::PolygonMode;
//// 		state->setAttribute( polyModeObj );    
//// 	}
//// 	
//// 	polyModeObj->setMode(  osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
//
//	osg::Program * this_program = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));
//	this_program->addBindAttribLocation("myVertex",0);
//	this_program->addBindAttribLocation("myNormal",1);
//	this_program->addBindAttribLocation("myTexCoord",2);
//	//	this_program->addBindAttribLocation("myTessFact",3);
//
//
//
//	state->setTextureAttributeAndModes(tex_unit,displmap,osg::StateAttribute::ON);
//	this_program->setParameter(GL_PATCH_VERTICES,3);
//
//	this_program->addBindFragDataLocation("FragColor",0);
//	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
//	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.0f, 0.75f, 0.75f)));
//	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
//	state->addUniform(new osg::Uniform("dispmap", tex_unit));
//	//	state->addUniform(new osg::Uniform("texture", tex_unit));
//	state->addUniform(new osg::Uniform("TessLevelInner",16.0f));
//	state->addUniform(new osg::Uniform("TessLevelOuter",16.0f));
//	state->addUniform(new osg::Uniform("DisplacementFactor",0.1f));
//
//	ui.osgGLWidget->setSceneData(Groupnode);//newTransform);
//	setupClean();
//}

void shader_editor::loadExampleSimpleDisplacement_complexModel_differentTesselation()
{
	writeToConsoleQt(QtSystemMsg, "funkce loadExampleSimpleDisplacement_complexModel_differentTesselation není plnì implementována");
	return;
	const int tex_unit = 0;
	const unsigned tesmap_gluint = 0;

	std::string text_loc = EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/disp_map.tga";
	osg::Image * displmap_image = osgDB::readImageFile(text_loc);
	if (displmap_image==NULL)
		return;

	osg::ref_ptr<osg::Texture2D> displmap = new osg::Texture2D;

	displmap->setDataVariance(osg::Object::DYNAMIC);
	displmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	displmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

	displmap->setImage(displmap_image);

	osg::Geode *geode=new osg::Geode();
	osg::Geometry *geometry = new osg::Geometry();
	geometry->setUseVertexBufferObjects(true);
	int VertexCount = meteor::vertex_count[0];
	int IndexCount = sizeof(meteor::indexes) / sizeof(meteor::indexes[0]);

	osg::Vec2Array* tesmap_texcoords = new osg::Vec2Array();
	osg::Vec3Array* vertices = new osg::Vec3Array();
	osg::Vec3Array* normals = new osg::Vec3Array();
	for(int i=0;i<VertexCount;i++){
		vertices->push_back(osg::Vec3(meteor::vertices[i].x,meteor::vertices[i].y,meteor::vertices[i].z));
		normals->push_back(osg::Vec3(meteor::vertices[i].nx,meteor::vertices[i].ny,meteor::vertices[i].nz));
		tesmap_texcoords->push_back(osg::Vec2(meteor::vertices[i].u,meteor::vertices[i].v));
	}


	// 	normals->setName("myNormal");
	// 	tesmap_texcoords->setName("myTexCoord");

	geometry->setVertexAttribArray(0,vertices);
	geometry->setVertexAttribBinding(0,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(1,normals);
	geometry->setVertexAttribBinding(1,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(2,tesmap_texcoords);
	geometry->setVertexAttribBinding(2,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,IndexCount,meteor::indexes));

	geode->addDrawable(geometry);

	osg::ref_ptr<osg::Group> Groupnode = new osg::Group;
	Groupnode->addChild(geode);



/*
	if (!loadShader(ui.textEdit_VS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2_DD "/sd_shader.vs"))) return;
	if (!loadShader(ui.textEdit_TCS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2_DD "/sd_shader.tcs"))) return;
	if (!loadShader(ui.textEdit_TES, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2_DD "/sd_shader.tes"))) return;
	if (!loadShader(ui.textEdit_GS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2_DD "/sd_shader.gs"))) return;
	if (!loadShader(ui.textEdit_FS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2_DD "/sd_shader.fs"))) return;

	compileShaders(geode, FORCE_TESSELLATION_SH, FORCE_GEOMETRY_SH);*/
	osg::Program * p = shaderTab::buildProgramFromFiles((EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2_DD "/sd_shader.vs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2_DD "/sd_shader.tcs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2_DD "/sd_shader.tes"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2_DD "/sd_shader.gs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2_DD "/sd_shader.fs"));
	if (p==NULL)
	{
		return;
	}
	geode->getOrCreateStateSet()->setAttribute(p);

	osg::StateSet *state;
	state = geode->getOrCreateStateSet();
	// 	osg::PolygonMode *polyModeObj = polyModeObj = static_cast< osg::PolygonMode* >( state->getAttribute( osg::StateAttribute::POLYGONMODE ));
	// 	if ( !polyModeObj ) 
	// 	{
	// 		polyModeObj = new osg::PolygonMode;
	// 		state->setAttribute( polyModeObj );    
	// 	}
	// 	
	// 	polyModeObj->setMode(  osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );

	osg::Program * this_program = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));
	this_program->addBindAttribLocation("myVertex",0);
	this_program->addBindAttribLocation("myNormal",1);
	this_program->addBindAttribLocation("myTexCoord",2);
	//	this_program->addBindAttribLocation("myTessFact",3);



	state->setTextureAttributeAndModes(tex_unit,displmap,osg::StateAttribute::ON);
	this_program->setParameter(GL_PATCH_VERTICES,3);

	this_program->addBindFragDataLocation("FragColor",0);
	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.75f, 0.75f, 0.75f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
	state->addUniform(new osg::Uniform("dispmap", tex_unit));
	//	state->addUniform(new osg::Uniform("texture", tex_unit));
	state->addUniform(new osg::Uniform("TessLevelInner",16.0f));
	state->addUniform(new osg::Uniform("TessLevelOuter",16.0f));
	state->addUniform(new osg::Uniform("DisplacementFactor",0.1f));

	ui.osgGLWidget->setSceneData(Groupnode);//newTransform);
	setupClean();
}

//tato funkce je pøevzata z http://www.keithlantz.net/2011/10/tangent-space-normal-mapping-with-glsl/
void computeTangentSpaceMatrix(vertex_struct& p0, vertex_struct& p1,vertex_struct& p2, vertex_struct& np0, vertex_struct& np1, vertex_struct& np2) {
	GLfloat v1x, v1y, v1z, v2x, v2y, v2z, u1x, u1y, u2x, u2y, det;

	v1x = p1.x - p0.x;
	v1y = p1.y - p0.y;
	v1z = p1.z - p0.z;

	v2x = p2.x - p0.x;
	v2y = p2.y - p0.y;
	v2z = p2.z - p0.z;

	u1x = p1.u - p0.u;
	u1y = p1.v - p0.v;

	u2x = p2.u - p0.u;
	u2y = p2.v - p0.v;

	det = u1x * u2y - u2x * u1y;

	np0.nx = (v1x * u2y - v2x * u1y) / det;
	np0.ny = (v1y * u2y - v2y * u1y) / det;
	np0.nz = (v1z * u2y - v2z * u1y) / det;

	np1.nx = (-v1x * u2x + v2x * u1x) / det;
	np1.ny = (-v1y * u2x + v2y * u1x) / det;
	np1.nz = (-v1z * u2x + v2z * u1x) / det;

	np2.nx = p0.ny * p0.nz - p0.nz * p0.ny;
	np2.ny = p0.nz * p0.nx - p0.nx * p0.nz;
	np2.nz = p0.nx * p0.ny - p0.ny * p0.nx;
}

void shader_editor::loadExampleSimpleDisplacement_normalMapping()
{
/*
	writeToConsoleQt(QtSystemMsg, "funkce loadExampleSimpleDisplacement_normalMapping není plnì implementována");
	return;*/

	logger::debug("funkce loadExampleSimpleDisplacement_normalMapping");

	//const unsigned tesmap_gluint = 0;

	std::string text_loc =  EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/disp_map_ast_1.tga";
	osg::Image * displmap_image = osgDB::readImageFile(text_loc);
	if (displmap_image==NULL)
		return;

	text_loc =  EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/norm_map_ast_1.tga";
	osg::Image * normmap_image = osgDB::readImageFile(text_loc);
	if (normmap_image==NULL)
		return;

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

	osg::Geode *geode=new osg::Geode();
	osg::Geometry *geometry = new osg::Geometry();
	geometry->setUseVertexBufferObjects(true);
	int VertexCount = meteor::vertex_count[0];
	int IndexCount = sizeof(meteor::indexes) / sizeof(meteor::indexes[0]);

	osg::Vec2Array* tesmap_texcoords = new osg::Vec2Array();
	osg::Vec3Array* vertices = new osg::Vec3Array();
	osg::Vec3Array* normals = new osg::Vec3Array();
	osg::Vec3Array* tangents = new osg::Vec3Array(VertexCount);
	osg::Vec3Array* binormals = new osg::Vec3Array(VertexCount);
	//osg::Vec3Array* n_vector = new osg::Vec3Array(VertexCount);
	vertex_struct np0,np1,np2;
	for(int i=0;i<VertexCount;i++){
		vertices->push_back(osg::Vec3(meteor::vertices[i].x,meteor::vertices[i].y,meteor::vertices[i].z));
		normals->push_back(osg::Vec3(meteor::vertices[i].nx,meteor::vertices[i].ny,meteor::vertices[i].nz));
		tesmap_texcoords->push_back(osg::Vec2(meteor::vertices[i].u,meteor::vertices[i].v));
// 		tangent_vector->push_back(np0.nx, np0.ny, np0.nz);
// 		cotangent_vector->push_back(np1.nx, np1.ny, np1.nz);
// 		n_vector->push_back(np2.nx, np2.ny, np2.nz);
	}
	int k;
	osg::Vec3 ZAxis(0.0,0.0,1.0), YAxis(0.0,1.0,0.0);
	for (int i=0; i<VertexCount; i+=3)
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
			tangents->push_back(tangent);
			binormals->push_back(binormal);
	}

	// 	normals->setName("myNormal");
	// 	tesmap_texcoords->setName("myTexCoord");

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

	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,IndexCount,meteor::indexes));

	geode->addDrawable(geometry);

	osg::ref_ptr<osg::Group> Groupnode = new osg::Group;
	Groupnode->addChild(geode);



/*
	if (!loadShader(ui.textEdit_VS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/sd_shader.vs"))) return;
	if (!loadShader(ui.textEdit_TCS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/sd_shader.tcs"))) return;
	if (!loadShader(ui.textEdit_TES, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/sd_shader.tes"))) return;
	if (!loadShader(ui.textEdit_GS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/sd_shader.gs"))) return;
	if (!loadShader(ui.textEdit_FS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/sd_shader.fs"))) return;

	compileShaders(geode, FORCE_TESSELLATION_SH, FORCE_GEOMETRY_SH);*/

	osg::Program * p = shaderTab::buildProgramFromFiles((EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/sd_shader.vs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/sd_shader.tcs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/sd_shader.tes"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/sd_shader.gs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/sd_shader.fs"));
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
	//	this_program->addBindAttribLocation("myTessFact",3);

	const int dispmap_tex_unit = 0;
	const int normmap_tex_unit = 1;

	state->setTextureAttributeAndModes(dispmap_tex_unit,displmap,osg::StateAttribute::ON);
	state->setTextureAttributeAndModes(normmap_tex_unit,normmap,osg::StateAttribute::ON);
	this_program->setParameter(GL_PATCH_VERTICES,3);

	int normalMapping= 1;

	this_program->addBindFragDataLocation("FragColor",0);
	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.75f, 0.75f, 0.75f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
	state->addUniform(new osg::Uniform("dispmap", dispmap_tex_unit));
	state->addUniform(new osg::Uniform("normmap", normmap_tex_unit));
	state->addUniform(new osg::Uniform("useNormalMapping", normalMapping));
	//	state->addUniform(new osg::Uniform("texture", tex_unit));
	state->addUniform(new osg::Uniform("TessLevelInner",16.0f));
	state->addUniform(new osg::Uniform("TessLevelOuter",16.0f));
	state->addUniform(new osg::Uniform("DisplacementFactor",0.1f));

	ui.osgGLWidget->setSceneData(Groupnode);//newTransform);
	setupClean();
}

void shader_editor::loadExampleAsteroid_fromIVE()
{
	osg::ref_ptr<osgDB::Options> options = new osgDB::Options();
	options->setOptionString("noTriStripPolygons noTesselateLargePolygons");

	osg::Node * loadedNode = osgDB::readNodeFile(EXAMPLE_FOLDER "/asteroid.ive",options);
	if(loadedNode!=NULL)
	{
		osgUtil::Optimizer * opt = new osgUtil::Optimizer();
		opt->optimize(loadedNode,osgUtil::Optimizer::OptimizationOptions::INDEX_MESH | osgUtil::Optimizer::OptimizationOptions::VERTEX_PRETRANSFORM | osgUtil::Optimizer::OptimizationOptions::VERTEX_POSTTRANSFORM);

		ui.osgGLWidget->setSceneData(loadedNode);

		delete opt;

		setupClean();
	}
}


void shader_editor::loadExampleLargeSkyboxScene()
{
	QString status_text;
	osg::Node * node = largeSpaceScene::buildLargeSpaceScene(&status_text);
	if (!node)
	{
		writeToConsole(osg::FATAL, (QString("loadExampleLargeSkyboxScene failure: ")+status_text).toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean(true);
}



osg::Geometry * shader_editor::getAsteroidGeometry( int *vertexBindingValue, int *normalBindingValue, int *uvBindingValue, int *tangentBindingValue, int *bitangentBindingValue )
{
	if (!vertexBindingValue || !normalBindingValue || !uvBindingValue)
	{
		return NULL;
	}
	osg::Geometry *geometry = new osg::Geometry();
	geometry->setUseVertexBufferObjects(true);
	int VertexCount = asteroid_better_uvs_backup_2::vertex_count[0];
	int IndexCount = sizeof(asteroid_better_uvs_backup_2::indexes) / sizeof(asteroid_better_uvs_backup_2::indexes[0]);

	osg::Vec2Array* tesmap_texcoords = new osg::Vec2Array();
	osg::Vec3Array* vertices = new osg::Vec3Array();
	osg::Vec3Array* normals = new osg::Vec3Array();	
	osg::Vec3Array* tangents = NULL;
	osg::Vec3Array* bitangents = NULL;

	for(int i=0;i<VertexCount;i++){
		vertices->push_back(osg::Vec3(/*meteor*/asteroid_better_uvs_backup_2::vertices[i].x,asteroid_better_uvs_backup_2::vertices[i].y,asteroid_better_uvs_backup_2::vertices[i].z));
		normals->push_back(osg::Vec3(asteroid_better_uvs_backup_2::vertices[i].nx,asteroid_better_uvs_backup_2::vertices[i].ny,asteroid_better_uvs_backup_2::vertices[i].nz));
		tesmap_texcoords->push_back(osg::Vec2(asteroid_better_uvs_backup_2::vertices[i].u,asteroid_better_uvs_backup_2::vertices[i].v));
	}

	if (tangentBindingValue && bitangentBindingValue)
	{
		tangents  = new osg::Vec3Array();
		bitangents  = new osg::Vec3Array();
		std::vector< std::vector<osg::Vec3> > tangents_per_vertex;
		std::vector< std::vector<osg::Vec3> > bitangents_per_vertex;
		tangents_per_vertex.resize(VertexCount);
		bitangents_per_vertex.resize(VertexCount);

		unsigned int triangle_idxs[3];

		for (int i = 0; i < IndexCount; i+=3)
		{
			triangle_idxs[0] = asteroid_better_uvs_backup_2::indexes[i];
			triangle_idxs[1] = asteroid_better_uvs_backup_2::indexes[i+1];
			triangle_idxs[2] = asteroid_better_uvs_backup_2::indexes[i+2];
			for (int j = 0; j < 3; j++)
			{
				osg::Vec3 A = vertices->at(triangle_idxs[j]);
				osg::Vec3 B = vertices->at(triangle_idxs[(j+1)%3]);
				osg::Vec3 C = vertices->at(triangle_idxs[(j+2)%3]);

				osg::Vec3 N = normals->at(triangle_idxs[j]);

				osg::Vec2 H = tesmap_texcoords->at(triangle_idxs[j]);
				osg::Vec2 K = tesmap_texcoords->at(triangle_idxs[(j+1)%3]);
				osg::Vec2 L = tesmap_texcoords->at(triangle_idxs[(j+2)%3]);

				osg::Vec3 D = B - A;
				osg::Vec3 E = C - A;

				osg::Vec2 F = K - H;
				osg::Vec2 G = L - H;

				double fact = 1 / (F.x() * G.y() - F.y() * G.x());

				osg::Vec3 T (fact*(G.y()*D.x() - F.y()*E.x() ),
					fact*(G.y()*D.y() - F.y()*E.y() ),
					fact*(G.y()*D.z() - F.y()*E.z() ));

				osg::Vec3 U (-fact*(G.x()*D.x() + F.x()*E.x() ),
					-fact*(G.x()*D.y() + F.x()*E.y() ),
					-fact*(G.x()*D.z() + F.x()*E.z() ));

				osg::Vec3 T_inv = T - (N * ( N*T ) );
				osg::Vec3 U_inv = U - N* (N*U)  - T_inv * (T_inv*U) ;
				T_inv.normalize();
				U_inv.normalize();
				tangents_per_vertex[triangle_idxs[j]].push_back(T_inv);
				bitangents_per_vertex[triangle_idxs[j]].push_back(U_inv);

				//				| T.x T.y T.z |           1         |  G.t  -F.t | | D.x D.y D.z |
				//				|             | = ----------------- |            | |             |
				//				| U.x U.y U.z |   F.s G.t - F.t G.s | -G.s   F.s | | E.x E.y E.z |
			}

			//D = B-A
			//E = C-A

			//F = K-H
			//G = L-H
		}

		//prumerovani tangent a bitangent z okolnich trojuhelniku

		for(int i=0;i<VertexCount;i++){
			osg::Vec3 tangent(0.0,0.0,0.0);
			for (int j = 0; j < tangents_per_vertex.at(i).size(); j++)
				tangent += tangents_per_vertex.at(i).at(j);
			tangent.normalize();
			tangents->push_back(tangent);
		}

		for(int i=0;i<VertexCount;i++){
			osg::Vec3 bitangent(0.0,0.0,0.0);
			for (int j = 0; j < bitangents_per_vertex.at(i).size(); j++)
				bitangent += bitangents_per_vertex.at(i).at(j);
			bitangent.normalize();
			bitangents->push_back(bitangent);
		}

		//zprumerovani hran site
		{
			std::map<osg::Vec3,std::vector<int> > vertex_clones;
			for (int i = 0; i < vertices->size(); i++)
			{
				std::pair<std::map<osg::Vec3,std::vector<int> >::iterator, bool> ret = vertex_clones.insert(std::make_pair((*vertices)[i], std::vector<int>()));
				ret.first->second.push_back(i);
			}

			for (std::map<osg::Vec3,std::vector<int> >::iterator it = vertex_clones.begin(); it != vertex_clones.end(); it++)
			{
				if (it->second.size() < 2)
					continue;

				osg::Vec3 avgTangent (0.0,0.0,0.0);
				osg::Vec3 avgBitangent (0.0,0.0,0.0);
				for (int i = 0; i < it->second.size(); i++)
				{
					avgTangent += (*tangents)[it->second[i]];
					avgBitangent += (*bitangents)[it->second[i]];
				}
				avgTangent.normalize();
				avgBitangent.normalize();
				for (int i = 0; i < it->second.size(); i++)
				{
					(*tangents)[it->second[i]] = avgTangent;
					(*bitangents)[it->second[i]] = avgBitangent;
				}

			}
		}
	}

	*vertexBindingValue = 0;
	*normalBindingValue = 1;
	*uvBindingValue = 2;

	geometry->setVertexAttribArray(*vertexBindingValue,vertices);
	geometry->setVertexAttribBinding(*vertexBindingValue,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(*normalBindingValue,normals);
	geometry->setVertexAttribBinding(*normalBindingValue,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(*uvBindingValue,tesmap_texcoords);
	geometry->setVertexAttribBinding(*uvBindingValue,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	if (tangentBindingValue && bitangentBindingValue)
	{
		*tangentBindingValue = 3;
		*bitangentBindingValue = 4;
		geometry->setVertexAttribArray(*tangentBindingValue,tangents);
		geometry->setVertexAttribBinding(*tangentBindingValue,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
		geometry->setVertexAttribArray(*bitangentBindingValue,bitangents);
		geometry->setVertexAttribBinding(*bitangentBindingValue,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	}

	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,IndexCount,asteroid_better_uvs_backup_2::indexes));

	return geometry;
}

void shader_editor::loadExample_Issues_Gaps_Blender()
{
	QString status_text;
	osg::Node * node =  asteroid_examples::example_asteroid_gaps(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Issues_Gaps_MeshMapper()
{
	QString status_text;
	osg::Node * node =  asteroid_examples::example_asteroid_gaps(&status_text, "agm_displacement_texture.tga");
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Issues_Sewing()
{

}
void shader_editor::loadExample_Issues_UVs_Blender()
{
	QString status_text;
	osg::Node * node =  asteroid_examples::example_asteroid_better_uvs(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
	//example_asteroid_better_uvs
}
void shader_editor::loadExample_Issues_UVs_MeshMapper()
{
	QString status_text;
	osg::Node * node =  asteroid_examples::example_asteroid_better_uvs(&status_text, "abuvm_displacement_texture.tga");
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
	//example_asteroid_better_uvs
}
void shader_editor::loadExample_Issues_UVsWithManualTexture()
{//example_asteroid_better_uvs_fixed_textures
	QString status_text;
	osg::Node * node =  asteroid_examples::example_asteroid_better_uvs_fixed_textures(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_LOD_DynamicTess_16()
{//example_asteroid_better_uvs_fixed_textures
	QString status_text;
	osg::Node * node =  asteroid_examples::example_asteroid_dynamic_tessellation_16(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}


void shader_editor::loadExample_LOD_DynamicTess_64()
{//example_asteroid_better_uvs_fixed_textures
	QString status_text;
	osg::Node * node =  asteroid_examples::example_asteroid_dynamic_tessellation_64(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_LOD_DynamicTess_16_with_Common_LOD()
{//example_asteroid_better_uvs_fixed_textures
	QString status_text;
	osg::Node * node =  asteroid_examples::example_asteroid_dynamic_tessellation_16_with_Common_LOD(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}


void shader_editor::loadExample_LOD_DynamicTess_64_with_Common_LOD()
{//example_asteroid_better_uvs_fixed_textures
	QString status_text;
	osg::Node * node =  asteroid_examples::example_asteroid_dynamic_tessellation_64_with_Common_LOD(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_LOD_DynamicTess_16_with_Common_LOD_cust_range()
{//example_asteroid_better_uvs_fixed_textures
	QString status_text;
	osg::Node * node =  asteroid_examples::example_asteroid_dynamic_tessellation_64_with_Common_LOD_cust_range(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_LOD_DynamicTess_64_with_Common_LOD_cust_range()
{//example_asteroid_better_uvs_fixed_textures
	QString status_text;
	osg::Node * node =  asteroid_examples::example_asteroid_dynamic_tessellation_64_with_Common_LOD_cust_range(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_LOD_no_tessellation()
{
	QString status_text;
	osg::Node * node =  asteroid_examples::example_asteroid_LOD_no_tessellation(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}
void shader_editor::loadExample_LOD_no_tessellation_custom_range()
{
	QString status_text;
	osg::Node * node =  asteroid_examples::example_asteroid_LOD_no_tessellation_cust_range(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Cobblestone_SinglePart_Inner(const char * spacing, bool wireframe)
{
	QString status_text;
	osg::Node * node =  cobblestone_examples::example_cobblestone_simple_dynamic(&status_text, spacing);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}

	if(wireframe)
	{
		node->getOrCreateStateSet()->setAttribute(new osg::PolygonMode(osg::PolygonMode::Face::FRONT_AND_BACK, osg::PolygonMode::LINE));
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Cobblestone_SinglePart()
{
	loadExample_Cobblestone_SinglePart_Inner();
	/*QString status_text;
	osg::Node * node =  cobblestone_examples::example_cobblestone_simple_dynamic(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();*/
}

void shader_editor::loadExample_Cobblestone_MultiPart()
{
	QString status_text;
	osg::Node * node =  cobblestone_examples::example_cobblestone_numerous_dynamic(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Cobblestone_SinglePart_Gaps()
{
	QString status_text;
	osg::Node * node =  cobblestone_examples::example_cobblestone_simple_dynamic_fixed_gaps(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Cobblestone_MultiPart_Gaps()
{
	QString status_text;
	osg::Node * node =  cobblestone_examples::example_cobblestone_numerous_dynamic_fixed_gaps(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Cobblestone_spacing_equal()
{
	loadExample_Cobblestone_SinglePart_Inner(EQUAL_SPACING);
}
void shader_editor::loadExample_Cobblestone_spacing_fractional_odd()
{
	loadExample_Cobblestone_SinglePart_Inner(FRACTIONAL_ODD_SPACING);
}
void shader_editor::loadExample_Cobblestone_spacing_fractional_even()
{
	loadExample_Cobblestone_SinglePart_Inner(FRACTIONAL_EVEN_SPACING);
}

void shader_editor::loadExample_Cobblestone_spacing_equal_w()
{
	loadExample_Cobblestone_SinglePart_Inner(EQUAL_SPACING, true);
}
void shader_editor::loadExample_Cobblestone_spacing_fractional_odd_w()
{
	loadExample_Cobblestone_SinglePart_Inner(FRACTIONAL_ODD_SPACING, true);
}
void shader_editor::loadExample_Cobblestone_spacing_fractional_even_w()
{
	loadExample_Cobblestone_SinglePart_Inner(FRACTIONAL_EVEN_SPACING, true);
}

void shader_editor::loadExample_Miscellaneous_Triangle_Inner(const char * shader_file_name)
{
	QString status_text;
	osg::Node * node =  miscellaneous_examples::triangle_example(shader_file_name);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Miscellaneous_Triangle()
{
	loadExample_Miscellaneous_Triangle_Inner("triangle_simple");
}

void shader_editor::loadExample_Miscellaneous_Triangle_Dynamic()
{
	loadExample_Miscellaneous_Triangle_Inner("triangle_dynamic");
}
void shader_editor::loadExample_Miscellaneous_Triangle_Dynamic_Regular()
{
	loadExample_Miscellaneous_Triangle_Inner("triangle_regular");
}

void shader_editor::loadExample_Miscellaneous_Quads_Inner(const char * shader_file_name)
{
	QString status_text;
	osg::Node * node =  miscellaneous_examples::quads_example(shader_file_name);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Miscellaneous_Quads()
{
	loadExample_Miscellaneous_Quads_Inner("quads_simple");
}
void shader_editor::loadExample_Miscellaneous_Quads_Dynamic()
{
	loadExample_Miscellaneous_Quads_Inner("quads_dynamic");
}

void shader_editor::loadExample_Performance_DynamicTessellation_Shader()
{
	QString status_text;
	osg::Node * node =  performance_examples::example_performance_dynamic_tess_shader(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Performance_DynamicTessellation_Callback()
{
	QString status_text;
	osg::Node * node =  performance_examples::example_performance_dynamic_tess_node(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Performance_DynamicTessellation_Fixed()
{
	QString status_text;
	osg::Node * node =  performance_examples::example_performance_dynamic_tess_fixed(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Performance_DynamicTessellation_Cobblestone_Combined()
{
	QString status_text;
	osg::Node * node =  performance_examples::example_performance_dynamic_tess_cobblestone_combined(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Performance_DynamicTessellation_Cobblestone_Shader()
{
	QString status_text;
	osg::Node * node =  performance_examples::example_performance_dynamic_tess_cobblestone_shader(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Performance_DynamicTessellation_Cobblestone_Shader_Modified()
{
	QString status_text;
	osg::Node * node =  performance_examples::example_performance_dynamic_tess_cobblestone_shader_modified(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Performance_Tessellated()
{
	QString status_text;
	osg::Node * node =  performance_examples::example_performance_asteroid_tessellated(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}

void shader_editor::loadExample_Performance_FullModel()
{
	QString status_text;
	osg::Node * node =  performance_examples::example_performance_asteroid_full(&status_text);
	if (!node)
	{
		writeToConsole(osg::NotifySeverity::WARN, status_text.toStdString().c_str());
		return;
	}
	ui.osgGLWidget->setSceneData(node);
	setupClean();
}




