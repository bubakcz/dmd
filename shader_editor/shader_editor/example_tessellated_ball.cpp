
#include "shader_editor.h"
#include <iostream>
#include <osgDB/ReadFile>


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

void shader_editor::loadExampleTeselace()
{
	logger::debug("TessBall loading");
	osg::Geode *geode=new osg::Geode();
	osg::Geometry *geometry = new osg::Geometry();
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
		1, 6, 10 };

		int IndexCount = sizeof(Faces) / sizeof(Faces[0]);
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
			0.000f,  0.000f, -1.000f };

			int VertexCount = sizeof(Verts)/sizeof(float);
			osg::Vec3Array* vertices = new osg::Vec3Array();
			for(int i=0;i<VertexCount;i+=3){
				vertices->push_back(osg::Vec3(Verts[i],Verts[i+1],Verts[i+2]));
			}
			geometry->setVertexArray(vertices);
			geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,IndexCount,Faces));

			geode->addDrawable(geometry);

			
/*			geode->setInitialBound(osg::BoundingSphere())*/
			geode->dirtyBound();

			osg::ref_ptr<osg::Group> Groupnode = new osg::Group;
			//Groupnode->addChild(loadedModel2.get());
			Groupnode->addChild(geode);

			

/*
			if (!loadShader(ui.textEdit_VS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_TESELACE "/st_shader.vs"))) return;
			if (!loadShader(ui.textEdit_TCS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_TESELACE "/st_shader.tcs"))) return;
			if (!loadShader(ui.textEdit_TES, (char *)NULL, (EXAMPLE_FOLDER FOLDER_TESELACE "/st_shader.tes"))) return;
			if (!loadShader(ui.textEdit_GS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_TESELACE "/st_shader.gs"))) return;
			if (!loadShader(ui.textEdit_FS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_TESELACE "/st_shader.fs"))) return;

			compileShaders(geode, FORCE_TESSELLATION_SH, FORCE_GEOMETRY_SH);*/
			osg::Program * p = shaderTab::buildProgramFromFiles((EXAMPLE_FOLDER FOLDER_TESELACE "/st_shader.vs"),
				(EXAMPLE_FOLDER FOLDER_TESELACE "/st_shader.tcs"),
				(EXAMPLE_FOLDER FOLDER_TESELACE "/st_shader.tes"),
				(EXAMPLE_FOLDER FOLDER_TESELACE "/st_shader.gs"),
				(EXAMPLE_FOLDER FOLDER_TESELACE "/st_shader.fs"));
			if (p==NULL)
			{
				return;
			}
			geode->getOrCreateStateSet()->setAttribute(p);


			osg::StateSet *state;
			state = geode->getOrCreateStateSet();
			osg::Program * shaderprogram = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));
			shaderprogram->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 3);
			shaderprogram->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
			shaderprogram->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
			shaderprogram->setParameter(GL_PATCH_VERTICES,3);
			shaderprogram->addBindFragDataLocation("FragColor",0);
			state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
			state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.0f, 0.75f, 0.75f)));
			state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
			state->addUniform(new osg::Uniform("TessLevelInner",4.0f));
			state->addUniform(new osg::Uniform("TessLevelOuter",4.0f));

			//geode->setCullingActive(false);

			ui.osgGLWidget->setSceneData(Groupnode);
			setupClean();
		logger::debug("TessBall loaded");
}
