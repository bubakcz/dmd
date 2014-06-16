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
#include <osg/LOD>
#include <osgDB/ReadFile>
#include "shaderTab.h"
#include <osgUtil/Optimizer>
#include <osgUtil/MeshOptimizers>
#include <osgUtil/Simplifier>

#include "logger.h"

#define EXAMPLE_FOLDER "examples"
#define FOLDER_TESELACE "/teselace"
#define FOLDER_VARIABILNI_TESELACE "/prom_teselace"
#define FOLDER_SIMPLE_DISPLACEMENT "/simple_displacement"
#define FOLDER_SIMPLE_DISPLACEMENT_V2 "/simple_displacement_v2"
#define FOLDER_SIMPLE_DISPLACEMENT_WN "/simple_displacement_with_normal"
#define FOLDER_SIMPLE_DISPLACEMENT_DN "/simple_displacement_with_normal_dynamic_tessellation"
#define FOLDER_SIMPLE_DISPLACEMENT_V2_DD "/simple_displacement_v2_dd"
#define FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/displacement_with_normal"
#define FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING_2 "/displacement_with_normal_2"
#define FORCE_TESSELLATION_SH true
#define NO_TESSELLATION_SH false
#define FORCE_GEOMETRY_SH true
#define NO_GEOMETRY_SH false

void shader_editor::loadExampleSimpleDisplacement_complexModel()
{
	logger::debug("funkce loadExampleSimpleDisplacement_complexModel");
	const int tex_unit = 0;
	const unsigned tesmap_gluint = 0;

	std::string text_loc = EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/disp_map_ast_1.tga";
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

	int vertexBindingValue, normalBindingValue, uvBindingValue;

	geode->addDrawable(getAsteroidGeometry(&vertexBindingValue, &normalBindingValue, &uvBindingValue, NULL, NULL));

//	geode->addDrawable(geometry);

	osg::ref_ptr<osg::Group> Groupnode = new osg::Group;
	Groupnode->addChild(geode);



/*
	if (!loadShader(ui.textEdit_VS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.vs"))) return;
	if (!loadShader(ui.textEdit_TCS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tcs"))) return;
	if (!loadShader(ui.textEdit_TES, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tes"))) return;
	if (!loadShader(ui.textEdit_GS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.gs"))) return;
	if (!loadShader(ui.textEdit_FS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.fs"))) return;

	compileShaders(geode, FORCE_TESSELLATION_SH, FORCE_GEOMETRY_SH);*/

	osg::Program * p = shaderTab::buildProgramFromFiles((EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.vs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tcs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tes"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.gs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.fs"));
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
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.6f, 0.6f, 0.6f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
	state->addUniform(new osg::Uniform("dispmap", tex_unit));
	//	state->addUniform(new osg::Uniform("texture", tex_unit));
	state->addUniform(new osg::Uniform("TessLevelInner",16.0f));
	state->addUniform(new osg::Uniform("TessLevelOuter",16.0f));
	state->addUniform(new osg::Uniform("RecalcNormals",2));
	state->addUniform(new osg::Uniform("DisplacementFactor",0.290721f));

	ui.osgGLWidget->setSceneData(Groupnode);//newTransform);
	setupClean();
}

void shader_editor::loadExampleSimpleDisplacement_complexModel_normalMapping()
{
	loadExampleSimpleDisplacement_complexModel_normalMapping_switch(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_WN "/sd_shader.fs");
}

void shader_editor::loadExampleSimpleDisplacement_complexModel_normalMapping_normalMapProblem()
{
	loadExampleSimpleDisplacement_complexModel_normalMapping_switch(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_WN "/sd_shader_err.fs");
}

void shader_editor::loadExampleSimpleDisplacement_complexModel_normalMapping_switch(const char * fragment_shader)
{
	logger::debug("funkce loadExampleSimpleDisplacement_complexModel_normalMapping");
	const int tex_unit = 0;
	const unsigned tesmap_gluint = 0;

	std::string displmap_text_loc = EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/abuvm_displacement_texture.tga";//"/disp_map_ast_1.tga";
	osg::Image * displmap_image = osgDB::readImageFile(displmap_text_loc);
	if (displmap_image==NULL)
		return;

	osg::ref_ptr<osg::Texture2D> displmap = new osg::Texture2D;

	displmap->setDataVariance(osg::Object::DYNAMIC);
	displmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	displmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

	displmap->setWrap(osg::Texture::WrapParameter::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	displmap->setWrap(osg::Texture::WrapParameter::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

	displmap->setImage(displmap_image);

	std::string normalmap_text_loc = EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/abuv_normal_texture_2.tga";//"/norm_map_ast_1.tga";
	osg::Image * normmap_image = osgDB::readImageFile(normalmap_text_loc);
	if (normmap_image==NULL)
		return;

	osg::ref_ptr<osg::Texture2D> normmap = new osg::Texture2D;

	normmap->setDataVariance(osg::Object::DYNAMIC);
	normmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	normmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

	normmap->setWrap(osg::Texture::WrapParameter::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	normmap->setWrap(osg::Texture::WrapParameter::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

	normmap->setImage(normmap_image);



	osg::Geode *geode=new osg::Geode();
/*	osg::Geometry *geometry = new osg::Geometry();
	geometry->setUseVertexBufferObjects(true);
	int VertexCount = meteor::vertex_count[0];
	int IndexCount = sizeof(meteor::indexes) / sizeof(meteor::indexes[0]);

	osg::Vec2Array* tesmap_texcoords = new osg::Vec2Array();
	osg::Vec3Array* vertices = new osg::Vec3Array();
	osg::Vec3Array* normals = new osg::Vec3Array();	
	osg::Vec3Array* tangents = new osg::Vec3Array();
	osg::Vec3Array* bitangents = new osg::Vec3Array();

	for(int i=0;i<VertexCount;i++){
		vertices->push_back(osg::Vec3(meteor::vertices[i].x,meteor::vertices[i].y,meteor::vertices[i].z));
		normals->push_back(osg::Vec3(meteor::vertices[i].nx,meteor::vertices[i].ny,meteor::vertices[i].nz));
		tesmap_texcoords->push_back(osg::Vec2(meteor::vertices[i].u,meteor::vertices[i].v));
	}

	//tangenty a bitangenty pro kazdy bod, jedna za kazdy polygon jehoz je casti
	std::vector< std::vector<osg::Vec3> > tangents_per_vertex;
	std::vector< std::vector<osg::Vec3> > bitangents_per_vertex;
	tangents_per_vertex.resize(VertexCount);
	bitangents_per_vertex.resize(VertexCount);

	unsigned int triangle_idxs[3];

	for (int i = 0; i < IndexCount; i+=3)
	{
		triangle_idxs[0] = meteor::indexes[i];
		triangle_idxs[1] = meteor::indexes[i+1];
		triangle_idxs[2] = meteor::indexes[i+2];
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
	geometry->setVertexAttribArray(4,bitangents);
	geometry->setVertexAttribBinding(4,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,IndexCount,meteor::indexes));*/

	int vertexBindingValue, normalBindingValue, uvBindingValue, tangentBindingValue, bitangentBindingValue;

	geode->addDrawable(getAsteroidGeometry(&vertexBindingValue, &normalBindingValue, &uvBindingValue, &tangentBindingValue, &bitangentBindingValue));

	osg::ref_ptr<osg::Group> Groupnode = new osg::Group;
	Groupnode->addChild(geode);



/*
	if (!loadShader(ui.textEdit_VS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.vs"))) return;
	if (!loadShader(ui.textEdit_TCS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tcs"))) return;
	if (!loadShader(ui.textEdit_TES, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tes"))) return;
	if (!loadShader(ui.textEdit_GS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.gs"))) return;
	if (!loadShader(ui.textEdit_FS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.fs"))) return;

	compileShaders(geode, FORCE_TESSELLATION_SH, FORCE_GEOMETRY_SH);*/

	osg::Program * p = shaderTab::buildProgramFromFiles((EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_WN "/sd_shader.vs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_WN "/sd_shader.tcs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_WN "/sd_shader.tes"),
		"",//(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.gs"),
		(fragment_shader));
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


//	int vertexBindingValue, normalBindingValue, uvBindingValue, tangentBindingValue, bitangentBindingValue;
	osg::Program * this_program = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));
	this_program->addBindAttribLocation("myVertex",vertexBindingValue);
	this_program->addBindAttribLocation("myNormal",normalBindingValue);
	this_program->addBindAttribLocation("myTexCoord",uvBindingValue);
	this_program->addBindAttribLocation("myTangent",tangentBindingValue);
	this_program->addBindAttribLocation("myBitangent",bitangentBindingValue);



	state->setTextureAttributeAndModes(tex_unit,displmap,osg::StateAttribute::ON);
	state->setTextureAttributeAndModes(tex_unit+1,normmap,osg::StateAttribute::ON);

	this_program->setParameter(GL_PATCH_VERTICES,3);

	this_program->addBindFragDataLocation("FragColor",0);
	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.6f, 0.6f, 0.6f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
	state->addUniform(new osg::Uniform("dispmap", tex_unit));
	state->addUniform(new osg::Uniform("normmap", tex_unit+1));
	state->addUniform(new osg::Uniform("TessLevelInner",16.0f));
	state->addUniform(new osg::Uniform("TessLevelOuter",16.0f));
	state->addUniform(new osg::Uniform("RecalcNormals",2));
	state->addUniform(new osg::Uniform("DisplacementFactor",0.290721f));

	ui.osgGLWidget->setSceneData(Groupnode);
	setupClean();
}
//
//void shader_editor::loadExampleSimpleDisplacement_complexModel_normalMapping()
//{
//	logger::debug("funkce loadExampleSimpleDisplacement_complexModel_normalMapping");
//	const int tex_unit = 0;
//	const unsigned tesmap_gluint = 0;
//
//	std::string displmap_text_loc = EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/disp_map_ast_1.tga";
//	osg::Image * displmap_image = osgDB::readImageFile(displmap_text_loc);
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
//	std::string normalmap_text_loc = EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/norm_map_ast_1.tga";
//	osg::Image * normmap_image = osgDB::readImageFile(normalmap_text_loc);
//	if (normmap_image==NULL)
//		return;
//
//	osg::ref_ptr<osg::Texture2D> normmap = new osg::Texture2D;
//
//	normmap->setDataVariance(osg::Object::DYNAMIC);
//	normmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
//	normmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
//
//	normmap->setImage(normmap_image);
//
//
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
//	osg::Vec3Array* tangents = new osg::Vec3Array();
//	osg::Vec3Array* bitangents = new osg::Vec3Array();
//
//	for(int i=0;i<VertexCount;i++){
//		vertices->push_back(osg::Vec3(meteor::vertices[i].x,meteor::vertices[i].y,meteor::vertices[i].z));
//		normals->push_back(osg::Vec3(meteor::vertices[i].nx,meteor::vertices[i].ny,meteor::vertices[i].nz));
//		tesmap_texcoords->push_back(osg::Vec2(meteor::vertices[i].u,meteor::vertices[i].v));
//	}
//
//	//tangenty a bitangenty pro kazdy bod, jedna za kazdy polygon jehoz je casti
//	std::vector< std::vector<osg::Vec3> > tangents_per_vertex;
//	std::vector< std::vector<osg::Vec3> > bitangents_per_vertex;
//	tangents_per_vertex.resize(VertexCount);
//	bitangents_per_vertex.resize(VertexCount);
//
//	unsigned int triangle_idxs[3];
//
//	for (int i = 0; i < IndexCount; i+=3)
//	{
//		triangle_idxs[0] = meteor::indexes[i];
//		triangle_idxs[1] = meteor::indexes[i+1];
//		triangle_idxs[2] = meteor::indexes[i+2];
//		for (int j = 0; j < 3; j++)
//		{
//			osg::Vec3 A = vertices->at(triangle_idxs[j]);
//			osg::Vec3 B = vertices->at(triangle_idxs[(j+1)%3]);
//			osg::Vec3 C = vertices->at(triangle_idxs[(j+2)%3]);
//
//			osg::Vec3 N = normals->at(triangle_idxs[j]);
//
//			osg::Vec2 H = tesmap_texcoords->at(triangle_idxs[j]);
//			osg::Vec2 K = tesmap_texcoords->at(triangle_idxs[(j+1)%3]);
//			osg::Vec2 L = tesmap_texcoords->at(triangle_idxs[(j+2)%3]);
//
//			osg::Vec3 D = B - A;
//			osg::Vec3 E = C - A;
//
//			osg::Vec2 F = K - H;
//			osg::Vec2 G = L - H;
//
//			double fact = 1 / (F.x() * G.y() - F.y() * G.x());
//
//			osg::Vec3 T (fact*(G.y()*D.x() - F.y()*E.x() ),
//				         fact*(G.y()*D.y() - F.y()*E.y() ),
//						 fact*(G.y()*D.z() - F.y()*E.z() ));
//
//			osg::Vec3 U (-fact*(G.x()*D.x() + F.x()*E.x() ),
//						 -fact*(G.x()*D.y() + F.x()*E.y() ),
//						 -fact*(G.x()*D.z() + F.x()*E.z() ));
//
//			osg::Vec3 T_inv = T - (N * ( N*T ) );
//			osg::Vec3 U_inv = U - N* (N*U)  - T_inv * (T_inv*U) ;
//			T_inv.normalize();
//			U_inv.normalize();
//			tangents_per_vertex[triangle_idxs[j]].push_back(T_inv);
//			bitangents_per_vertex[triangle_idxs[j]].push_back(U_inv);
//
////				| T.x T.y T.z |           1         |  G.t  -F.t | | D.x D.y D.z |
////				|             | = ----------------- |            | |             |
////				| U.x U.y U.z |   F.s G.t - F.t G.s | -G.s   F.s | | E.x E.y E.z |
//		}
//		
//		//D = B-A
//		//E = C-A
//
//		//F = K-H
//		//G = L-H
//	}
//
//	//prumerovani tangent a bitangent z okolnich trojuhelniku
//
//	for(int i=0;i<VertexCount;i++){
//		osg::Vec3 tangent(0.0,0.0,0.0);
//		for (int j = 0; j < tangents_per_vertex.at(i).size(); j++)
//			tangent += tangents_per_vertex.at(i).at(j);
//		tangent.normalize();
//		tangents->push_back(tangent);
//	}
//
//	for(int i=0;i<VertexCount;i++){
//		osg::Vec3 bitangent(0.0,0.0,0.0);
//		for (int j = 0; j < bitangents_per_vertex.at(i).size(); j++)
//			bitangent += bitangents_per_vertex.at(i).at(j);
//		bitangent.normalize();
//		bitangents->push_back(bitangent);
//	}
//
//	//zprumerovani hran site
//	{
//		std::map<osg::Vec3,std::vector<int> > vertex_clones;
//		for (int i = 0; i < vertices->size(); i++)
//		{
//			std::pair<std::map<osg::Vec3,std::vector<int> >::iterator, bool> ret = vertex_clones.insert(std::make_pair((*vertices)[i], std::vector<int>()));
//			ret.first->second.push_back(i);
//		}
//
//		for (std::map<osg::Vec3,std::vector<int> >::iterator it = vertex_clones.begin(); it != vertex_clones.end(); it++)
//		{
//			if (it->second.size() < 2)
//				continue;
//				
//			osg::Vec3 avgTangent (0.0,0.0,0.0);
//			osg::Vec3 avgBitangent (0.0,0.0,0.0);
//			for (int i = 0; i < it->second.size(); i++)
//			{
//				avgTangent += (*tangents)[it->second[i]];
//				avgBitangent += (*bitangents)[it->second[i]];
//			}
//			avgTangent.normalize();
//			avgBitangent.normalize();
//			for (int i = 0; i < it->second.size(); i++)
//			{
//				(*tangents)[it->second[i]] = avgTangent;
//				(*bitangents)[it->second[i]] = avgBitangent;
//			}
//
//		}
//	}
//
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
//	geometry->setVertexAttribArray(3,tangents);
//	geometry->setVertexAttribBinding(3,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
//	geometry->setVertexAttribArray(4,bitangents);
//	geometry->setVertexAttribBinding(4,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
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
//	osg::Program * p = shaderTab::buildProgramFromFiles((EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_WN "/sd_shader.vs"),
//		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_WN "/sd_shader.tcs"),
//		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_WN "/sd_shader.tes"),
//		"",//(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.gs"),
//		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_WN "/sd_shader.fs"));
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
//	this_program->addBindAttribLocation("myTangent",3);
//	this_program->addBindAttribLocation("myBitangent",4);
//
//
//
//	state->setTextureAttributeAndModes(tex_unit,displmap,osg::StateAttribute::ON);
//	state->setTextureAttributeAndModes(tex_unit+1,normmap,osg::StateAttribute::ON);
//
//	this_program->setParameter(GL_PATCH_VERTICES,3);
//
//	this_program->addBindFragDataLocation("FragColor",0);
//	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
//	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.6f, 0.6f, 0.6f)));
//	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
//	state->addUniform(new osg::Uniform("dispmap", tex_unit));
//	state->addUniform(new osg::Uniform("normmap", tex_unit+1));
//	state->addUniform(new osg::Uniform("TessLevelInner",16.0f));
//	state->addUniform(new osg::Uniform("TessLevelOuter",16.0f));
//	state->addUniform(new osg::Uniform("RecalcNormals",2));
//	state->addUniform(new osg::Uniform("DisplacementFactor",0.290721f));
//
//	ui.osgGLWidget->setSceneData(Groupnode);
//	setupClean();
//}

void shader_editor::loadExampleSimpleDisplacement_complexModel_normalMapping_sewing()
{
	logger::debug("funkce loadExampleSimpleDisplacement_complexModel_normalMapping");
	const int tex_unit = 0;
	const unsigned tesmap_gluint = 0;

	std::string displmap_text_loc = EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/disp_map_ast_1.tga";
	osg::Image * displmap_image = osgDB::readImageFile(displmap_text_loc);
	if (displmap_image==NULL)
		return;

	osg::ref_ptr<osg::Texture2D> displmap = new osg::Texture2D;

	displmap->setDataVariance(osg::Object::DYNAMIC);
	displmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	displmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

	displmap->setImage(displmap_image);

	std::string normalmap_text_loc = EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/norm_map_ast_1.tga";
	osg::Image * normmap_image = osgDB::readImageFile(normalmap_text_loc);
	if (normmap_image==NULL)
		return;

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
	for(int i=0;i<VertexCount;i++){
		vertices->push_back(osg::Vec3(meteor::vertices[i].x,meteor::vertices[i].y,meteor::vertices[i].z));
		normals->push_back(osg::Vec3(meteor::vertices[i].nx,meteor::vertices[i].ny,meteor::vertices[i].nz));
		tesmap_texcoords->push_back(osg::Vec2(meteor::vertices[i].u,meteor::vertices[i].v));
	}

	std::vector< std::vector<osg::Vec3> > tangents_per_vertex;
	std::vector< std::vector<osg::Vec3> > bitangents_per_vertex;
	tangents_per_vertex.resize(VertexCount);
	bitangents_per_vertex.resize(VertexCount);

	unsigned int triangle_idxs[3];

	for (int i = 0; i < IndexCount; i+=3)
	{
		triangle_idxs[0] = meteor::indexes[i];
		triangle_idxs[1] = meteor::indexes[i+1];
		triangle_idxs[2] = meteor::indexes[i+2];
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

	osg::Vec3Array* tangents = new osg::Vec3Array();
	osg::Vec3Array* bitangents = new osg::Vec3Array();
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
	geometry->setVertexAttribArray(4,bitangents);
	geometry->setVertexAttribBinding(4,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,IndexCount,meteor::indexes));

	int vertexBindingValue, normalBindingValue, uvBindingValue, tangentBindingValue, bitangentBindingValue;

	geode->addDrawable(getAsteroidGeometry(&vertexBindingValue, &normalBindingValue, &uvBindingValue, &tangentBindingValue, &bitangentBindingValue));

	//geode->addDrawable(geometry);

	osg::ref_ptr<osg::Group> Groupnode = new osg::Group;
	Groupnode->addChild(geode);



/*
	if (!loadShader(ui.textEdit_VS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.vs"))) return;
	if (!loadShader(ui.textEdit_TCS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tcs"))) return;
	if (!loadShader(ui.textEdit_TES, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tes"))) return;
	if (!loadShader(ui.textEdit_GS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.gs"))) return;
	if (!loadShader(ui.textEdit_FS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.fs"))) return;

	compileShaders(geode, FORCE_TESSELLATION_SH, FORCE_GEOMETRY_SH);*/

	osg::Program * p = shaderTab::buildProgramFromFiles((EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_WN "/sd_shader.vs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_WN "/sd_shader.tcs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_WN "/sd_shader.tes"),
		"",//(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.gs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_WN "/sd_shader.fs"));
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
	this_program->addBindAttribLocation("myTangent",3);
	this_program->addBindAttribLocation("myBitangent",4);



	state->setTextureAttributeAndModes(tex_unit,displmap,osg::StateAttribute::ON);
	state->setTextureAttributeAndModes(tex_unit+1,normmap,osg::StateAttribute::ON);

	this_program->setParameter(GL_PATCH_VERTICES,3);

	this_program->addBindFragDataLocation("FragColor",0);
	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.6f, 0.6f, 0.6f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
	state->addUniform(new osg::Uniform("dispmap", tex_unit));
	state->addUniform(new osg::Uniform("normmap", tex_unit+1));
	state->addUniform(new osg::Uniform("TessLevelInner",16.0f));
	state->addUniform(new osg::Uniform("TessLevelOuter",16.0f));
	state->addUniform(new osg::Uniform("RecalcNormals",2));
	state->addUniform(new osg::Uniform("DisplacementFactor",0.290721f));

	ui.osgGLWidget->setSceneData(Groupnode);
	setupClean();
}


void shader_editor::loadExampleSimpleDisplacement_complexModel_normalMapping_dynamicTessellation()
{
	logger::debug("funkce loadExampleSimpleDisplacement_complexModel_normalMapping_dynamicTessellation");
	const int tex_unit = 0;
	const unsigned tesmap_gluint = 0;

	std::string displmap_text_loc = EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/disp_map_ast_1.tga";
	osg::Image * displmap_image = osgDB::readImageFile(displmap_text_loc);
	if (displmap_image==NULL)
		return;

	osg::ref_ptr<osg::Texture2D> displmap = new osg::Texture2D;

	displmap->setDataVariance(osg::Object::DYNAMIC);
	displmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	displmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

	displmap->setImage(displmap_image);

	std::string normalmap_text_loc = EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_NORMAL_MAPPING "/norm_map_ast_1.tga";
	osg::Image * normmap_image = osgDB::readImageFile(normalmap_text_loc);
	if (normmap_image==NULL)
		return;

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
	osg::Vec3Array* tangents = new osg::Vec3Array();
	osg::Vec3Array* bitangents = new osg::Vec3Array();

	for(int i=0;i<VertexCount;i++){
		vertices->push_back(osg::Vec3(meteor::vertices[i].x,meteor::vertices[i].y,meteor::vertices[i].z));
		normals->push_back(osg::Vec3(meteor::vertices[i].nx,meteor::vertices[i].ny,meteor::vertices[i].nz));
		tesmap_texcoords->push_back(osg::Vec2(meteor::vertices[i].u,meteor::vertices[i].v));
	}

	//tangenty a bitangenty pro kazdy bod, jedna za kazdy polygon jehoz je casti
	std::vector< std::vector<osg::Vec3> > tangents_per_vertex;
	std::vector< std::vector<osg::Vec3> > bitangents_per_vertex;
	tangents_per_vertex.resize(VertexCount);
	bitangents_per_vertex.resize(VertexCount);

	unsigned int triangle_idxs[3];

	for (int i = 0; i < IndexCount; i+=3)
	{
		triangle_idxs[0] = meteor::indexes[i];
		triangle_idxs[1] = meteor::indexes[i+1];
		triangle_idxs[2] = meteor::indexes[i+2];
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
	geometry->setVertexAttribArray(4,bitangents);
	geometry->setVertexAttribBinding(4,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,IndexCount,meteor::indexes));

	//geode->addDrawable(geometry);
	int vertexBindingValue, normalBindingValue, uvBindingValue, tangentBindingValue, bitangentBindingValue;

	geode->addDrawable(getAsteroidGeometry(&vertexBindingValue, &normalBindingValue, &uvBindingValue, &tangentBindingValue, &bitangentBindingValue));

	osg::ref_ptr<osg::Group> Groupnode = new osg::Group;
	Groupnode->addChild(geode);



/*
	if (!loadShader(ui.textEdit_VS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.vs"))) return;
	if (!loadShader(ui.textEdit_TCS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tcs"))) return;
	if (!loadShader(ui.textEdit_TES, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.tes"))) return;
	if (!loadShader(ui.textEdit_GS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.gs"))) return;
	if (!loadShader(ui.textEdit_FS, (char *)NULL, (EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.fs"))) return;

	compileShaders(geode, FORCE_TESSELLATION_SH, FORCE_GEOMETRY_SH);*/

	osg::Program * p = shaderTab::buildProgramFromFiles((EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_DN "/sd_shader.vs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_DN "/sd_shader.tcs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_DN "/sd_shader.tes"),
		"",//(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_V2 "/sd_shader.gs"),
		(EXAMPLE_FOLDER FOLDER_SIMPLE_DISPLACEMENT_DN "/sd_shader.fs"));
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
	this_program->addBindAttribLocation("myTangent",3);
	this_program->addBindAttribLocation("myBitangent",4);



	state->setTextureAttributeAndModes(tex_unit,displmap,osg::StateAttribute::ON);
	state->setTextureAttributeAndModes(tex_unit+1,normmap,osg::StateAttribute::ON);

	this_program->setParameter(GL_PATCH_VERTICES,3);

	this_program->addBindFragDataLocation("FragColor",0);
	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.6f, 0.6f, 0.6f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));
	state->addUniform(new osg::Uniform("dispmap", tex_unit));
	state->addUniform(new osg::Uniform("normmap", tex_unit+1));
	state->addUniform(new osg::Uniform("TessLevelInner",16.0f));
	state->addUniform(new osg::Uniform("TessLevelOuter",16.0f));
	state->addUniform(new osg::Uniform("RecalcNormals",2));
	state->addUniform(new osg::Uniform("DisplacementFactor",0.290721f));

	ui.osgGLWidget->setSceneData(Groupnode);
	setupClean();
}

void shader_editor::loadExampleAsteroid_BasicLod()
{
	//asteroid_full_LOD.ive
	osg::ref_ptr<osgDB::Options> options = new osgDB::Options();
	options->setOptionString("noTriStripPolygons noTesselateLargePolygons");
	/*QMessageBox::information(this, tr("LOD"),
		tr("Loading model"),
		QMessageBox::Ok);*/

	osg::ref_ptr<osg::Node>  asteroidLevel1 = osgDB::readNodeFile(EXAMPLE_FOLDER "/asteroid_full_LOD.ive",options);
	osg::ref_ptr<osg::Node>  asteroidLevel2 = osgDB::readNodeFile(EXAMPLE_FOLDER "/asteroid_minimal_LOD.obj",options);
	/*QMessageBox::information(this, tr("LOD"),
		tr("Model loaded"),
		QMessageBox::Ok);*/
	if(asteroidLevel1!=NULL && asteroidLevel2!=NULL)
	{
		/*osgUtil::Optimizer * opt = new osgUtil::Optimizer();
		opt->optimize(asteroidLevel3,osgUtil::Optimizer::OptimizationOptions::INDEX_MESH | osgUtil::Optimizer::OptimizationOptions::VERTEX_PRETRANSFORM | osgUtil::Optimizer::OptimizationOptions::VERTEX_POSTTRANSFORM);
*/
		osg::LOD * lod_node = new osg::LOD();

		/*QMessageBox::information(this, tr("LOD"),
			tr("First copy"),
			QMessageBox::Ok);*/
		//osg::ref_ptr<osg::Node> asteroidLevel3 = dynamic_cast<osg::Node*>( loadedNode->clone( osg::CopyOp::DEEP_COPY_ALL ) );
		// clone the model
		/*osg::ref_ptr<osg::Node> asteroidLevel2 = dynamic_cast<osg::Node*>( asteroidLevel3->clone( osg::CopyOp::DEEP_COPY_ALL ) );
		QMessageBox::information(this, tr("LOD"),
			tr("Second copy"),
			QMessageBox::Ok);
		// make another copy
		osg::ref_ptr<osg::Node> asteroidLevel1 = dynamic_cast<osg::Node*>( asteroidLevel3->clone( osg::CopyOp::DEEP_COPY_ALL ) );*/
/*
		QMessageBox::information(this, tr("LOD"),
			tr("Preparing simplication"),
			QMessageBox::Ok);

		osgUtil::Simplifier simplifier;*/

		/*simplifier.setSampleRatio( 0.5 );
		asteroidLevel2->accept( simplifier );

		QMessageBox::information(this, tr("LOD"),
			tr("simplication step 2"),
			QMessageBox::Ok);

		simplifier.setSampleRatio( 0.1 );
		asteroidLevel3->accept( simplifier );

		QMessageBox::information(this, tr("LOD"),
			tr("Simplication done"),
			QMessageBox::Ok);*/

		lod_node->addChild( asteroidLevel2.get(), 100.0f, FLT_MAX );
		lod_node->addChild( asteroidLevel1.get(), 00.0f, 100.0f );
//		lod_node->addChild( asteroidLevel1.get(), 0.0f, 50.0f );

		ui.osgGLWidget->setSceneData(lod_node);

//		delete opt;


		setupClean();
	}
}