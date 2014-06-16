#include "cobblestone_examples.h"
#include "large_flat_geometry.h"
#include "blender_vertex_struct.h"
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/LOD>
#include <osgDB/ReadFile>
#include <stdexcept>
#include <QMessageBox>
#include <QFile>
#include <QInputDialog>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>

const int vertex_binding = 0;
const int normal_binding = 1;
const int texcoord_binding = 2;
const int tangent_binding = 3;
const int bitangent_binding = 4;
const int edge_center_binding = 5;
const int edge_flag_binding = 6;



osg::Program * buildProgramFromFiles_cb( QString VS, QString TCS, QString TES, QString GS, QString FS, const char * spacing )
{
	bool failure = false;

	QFile vsFile(VS);
	QFile tcsFile(TCS);
	QFile tesFile(TES);
	QFile gsFile(GS);
	QFile fsFile(FS);

	if(!TCS.isEmpty())
		if(!tcsFile.open(QFile::ReadOnly | QFile::Text))
		{
			QMessageBox msgBox;

			msgBox.setWindowTitle("Chyba pøi naèítání souboru");
			msgBox.setText(QString("Nepodaøilo se otevøít soubor: %1").arg(TCS));
			msgBox.exec();
			return NULL;
		}

	if(!TES.isEmpty())
		if(!tesFile.open(QFile::ReadOnly | QFile::Text))
		{
			QMessageBox msgBox;

			msgBox.setWindowTitle("Chyba pøi naèítání souboru");
			msgBox.setText(QString("Nepodaøilo se otevøít soubor: %1").arg(TES));
			msgBox.exec();
			return NULL;
		}

	if(!GS.isEmpty())
		if(!gsFile.open(QFile::ReadOnly | QFile::Text))
		{
			QMessageBox msgBox;

			msgBox.setWindowTitle("Chyba pøi naèítání souboru");
			msgBox.setText(QString("Nepodaøilo se otevøít soubor: %1").arg(GS));
			msgBox.exec();
			return NULL;
		}

	if(!VS.isEmpty())
		if(!vsFile.open(QFile::ReadOnly | QFile::Text))
		{
			QMessageBox msgBox;

			msgBox.setWindowTitle("Chyba pøi naèítání souboru");
			msgBox.setText(QString("Nepodaøilo se otevøít soubor: %1").arg(VS));
			msgBox.exec();
			return NULL;
		}

	if(!FS.isEmpty())
		if(!fsFile.open(QFile::ReadOnly | QFile::Text))
		{
			QMessageBox msgBox;

			msgBox.setWindowTitle("Chyba pøi naèítání souboru");
			msgBox.setText(QString("Nepodaøilo se otevøít soubor: %1").arg(FS));
			msgBox.exec();
			return NULL;
		}

/*
	if (!vsFile.open(QFile::ReadOnly | QFile::Text) || !fsFile.open(QFile::ReadOnly | QFile::Text) ||
		(!tcsFile.open(QFile::ReadOnly | QFile::Text) && !TCS.isEmpty()) || (!tesFile.open(QFile::ReadOnly | QFile::Text) && !TES.isEmpty()) ||
		(!gsFile.open(QFile::ReadOnly | QFile::Text)  && !GS.isEmpty()))
	{
		QMessageBox msgBox;

		msgBox.setWindowTitle("Chyba pøi naèítání souboru");
		msgBox.setText(QString("Nepodaøilo se otevøít nìkterý ze souborù:%1\n%2\n%3\n%4\n%5").arg(VS).arg(TCS).arg(TES).arg(GS).arg(FS));
		msgBox.exec();
		return NULL;
	}*/
	osg::Program * np = new osg::Program;
	np->addShader(new osg::Shader(osg::Shader::VERTEX,QString(vsFile.readAll()).toStdString()));
	if(!TCS.isEmpty())
		np->addShader(new osg::Shader(osg::Shader::TESSCONTROL,QString(tcsFile.readAll()).toStdString()));
	if(!TES.isEmpty())
	{
		if(spacing != NULL)
			np->addShader(new osg::Shader(osg::Shader::TESSEVALUATION,QString(tesFile.readAll()).arg(spacing).toStdString()));
		else
			np->addShader(new osg::Shader(osg::Shader::TESSEVALUATION,QString(tesFile.readAll()).arg(EQUAL_SPACING).toStdString()));
	}
	if(!GS.isEmpty())
		np->addShader(new osg::Shader(osg::Shader::GEOMETRY,QString(gsFile.readAll()).toStdString()));
	np->addShader(new osg::Shader(osg::Shader::FRAGMENT,QString(fsFile.readAll()).toStdString()));
	return np;
}

osg::Geometry * getGeometryCust_cb( int VertexCount, int IndexCount, const struct vertex_struct_v3 source_vertices[], 
	const unsigned int source_indexes[], osg::PrimitiveSet::Mode PrimitiveSetMode, cobblestone_examples::config_bistring_enum bitstring = cobblestone_examples::DEFAULT)
{
	osg::Geometry *geometry = new osg::Geometry();
	geometry->setUseVertexBufferObjects(true);
// 	int VertexCount = meteor::vertex_count[0];
// 	int IndexCount = sizeof(meteor::indexes) / sizeof(meteor::indexes[0]);

	osg::Vec2Array* tesmap_texcoords = new osg::Vec2Array();
	osg::Vec3Array* vertices = new osg::Vec3Array();
	osg::Vec3Array* normals = new osg::Vec3Array();	
	osg::Vec3Array* tangents = NULL;
	osg::Vec3Array* bitangents = NULL;
	osg::Vec2Array* edge_flags = new osg::Vec2Array();

	for(int i=0;i<VertexCount;i++){
// 		vertices->push_back(osg::Vec3(meteor::vertices[i].x,meteor::vertices[i].y,meteor::vertices[i].z));
// 		normals->push_back(osg::Vec3(meteor::vertices[i].nx,meteor::vertices[i].ny,meteor::vertices[i].nz));
// 		tesmap_texcoords->push_back(osg::Vec2(meteor::vertices[i].u,meteor::vertices[i].v));
		vertices->push_back(osg::Vec3(source_vertices[i].x,source_vertices[i].y,source_vertices[i].z));
		normals->push_back(osg::Vec3(source_vertices[i].nx,source_vertices[i].ny,source_vertices[i].nz));
		tesmap_texcoords->push_back(osg::Vec2(source_vertices[i].u,source_vertices[i].v));
		edge_flags->push_back(osg::Vec2(source_vertices[i].flag_x, source_vertices[i].flag_y));
	}

		tangents  = new osg::Vec3Array();
		bitangents  = new osg::Vec3Array();
		std::vector< std::vector<osg::Vec3> > tangents_per_vertex;
		std::vector< std::vector<osg::Vec3> > bitangents_per_vertex;
		tangents_per_vertex.resize(VertexCount);
		bitangents_per_vertex.resize(VertexCount);

		unsigned int triangle_idxs[3];

		for (int i = 0; i < IndexCount; i+=3)
		{
			triangle_idxs[0] = source_indexes[i];//meteor::indexes[i];
			triangle_idxs[1] = source_indexes[i+1];//;meteor::indexes[i+1];
			triangle_idxs[2] = source_indexes[i+2];//meteor::indexes[i+2];
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



	geometry->setVertexAttribArray(vertex_binding,vertices);
	geometry->setVertexAttribBinding(vertex_binding,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(normal_binding,normals);
	geometry->setVertexAttribBinding(normal_binding,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(texcoord_binding,tesmap_texcoords);
	geometry->setVertexAttribBinding(texcoord_binding,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
/*
		*tangentBindingValue = 3;
		*bitangentBindingValue = 4;
		addTangentBitangentVectors(geometry, tangentBindingValue, bitangentBindingValue,
			*vertexBindingValue, *normalBindingValue, *uvBindingValue, IndexCount, source_indexes);*/
	geometry->setVertexAttribArray(tangent_binding,tangents);
	geometry->setVertexAttribBinding(tangent_binding,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(bitangent_binding,bitangents);
	geometry->setVertexAttribBinding(bitangent_binding,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	geometry->setVertexAttribArray(edge_center_binding,edge_flags);
	geometry->setVertexAttribBinding(edge_center_binding,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	if(bitstring & cobblestone_examples::config_bistring_enum::USE_EDGE_FLAGS)
	{
		osg::UIntArray * edgeFlagArray = new osg::UIntArray;
		for(unsigned long i = 0; i < VertexCount; i++)
			edgeFlagArray->push_back(source_vertices[i].edge);
		//edge_flag_binding
		geometry->setVertexAttribArray(edge_flag_binding,edgeFlagArray);
		geometry->setVertexAttribBinding(edge_flag_binding,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	}



	geometry->addPrimitiveSet(new osg::DrawElementsUInt(/*osg::PrimitiveSet::PATCHES*/PrimitiveSetMode,IndexCount,/*meteor::indexes*/source_indexes));

	return geometry;
}

bool buildTexture_cb( const char * filename, const char * texture_uniform_name,  osg::StateSet * targetSet, int texture_unit, QString * status_text )
{
	if (targetSet == NULL)
		throw std::invalid_argument("asteroid_examples : buildTexture : osg::StateSet * targetSet is NULL");
	osg::Image * texture_image = osgDB::readImageFile(filename);
	if (texture_image==NULL)
	{
		*status_text = QString("asteroid_examples : buildTexture failure: can't open texture %1").arg(filename);
		return false;
	}

	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;

	texture->setDataVariance(osg::Object::DYNAMIC);
	texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

	texture->setWrap(osg::Texture::WrapParameter::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	texture->setWrap(osg::Texture::WrapParameter::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

	texture->setImage(texture_image);

	targetSet->setTextureAttributeAndModes(texture_unit,texture,osg::StateAttribute::ON);

	targetSet->addUniform(new osg::Uniform(texture_uniform_name, texture_unit));

	return true;
}

bool example_cobblestone_add_program_and_textures( osg::Node * target, QString filename, QString * status_text, const char * spacing = EQUAL_SPACING , cobblestone_examples::config_bistring_enum bitstring = cobblestone_examples::DEFAULT)
{
	if(target == NULL)
		return false;
	if(target->getOrCreateStateSet() == NULL)
		return false;
	osg::StateSet * targetStateSet = target->getOrCreateStateSet();

	QString example_target("examples/cobblestone_simple_dynamic/%1.%2");
	osg::Program * targetProgram = buildProgramFromFiles_cb(example_target.arg(filename).arg("vs"),
														example_target.arg(filename).arg("tcs"),
														example_target.arg(filename).arg("tes"),
														"",
														example_target.arg(filename).arg("fs"),
														spacing);

	/*const int vertex_binding = 0;
	const int normal_binding = 1;
	const int texcoord_binding = 2;
	const int tangent_binding = 3;
	const int bitangent_binding = 4;
	const int edge_flag_binding = 5;*/

	if(targetProgram == NULL)
		return false;
	targetProgram->addBindAttribLocation("myVertex",vertex_binding);
	targetProgram->addBindAttribLocation("myNormal",normal_binding);
	targetProgram->addBindAttribLocation("myTexCoord",texcoord_binding);
	targetProgram->addBindAttribLocation("myTangents",tangent_binding);
	targetProgram->addBindAttribLocation("myBinormals",bitangent_binding);
	targetProgram->addBindAttribLocation("myTessLevelOrigin",edge_center_binding);
	if(bitstring & cobblestone_examples::config_bistring_enum::USE_EDGE_FLAGS)
	{
		targetProgram->addBindAttribLocation("myTessEdgeFlag",edge_flag_binding);
	}

	targetProgram->setParameter(GL_PATCH_VERTICES,3);

	targetStateSet->addUniform(new osg::Uniform("TessLevelInner",32.0f));
	targetStateSet->addUniform(new osg::Uniform("TessLevelOuter",32.0f));
	targetStateSet->addUniform(new osg::Uniform("DisplacementFactor",0.05f));
	targetStateSet->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));

	const int dispmap_tex_unit = 0;
	const int normmap_tex_unit = 1;
	const int diffmap_tex_unit = 2;
	const int specmap_tex_unit = 3;

	buildTexture_cb(example_target.arg("diff_map_plane").arg("tga").toStdString().c_str(), "diffuse_map", targetStateSet, diffmap_tex_unit,status_text);
	buildTexture_cb(example_target.arg("disp_map_plane").arg("tga").toStdString().c_str(), "displacement_map", targetStateSet, dispmap_tex_unit,status_text);
	buildTexture_cb(example_target.arg("norm_map_plane").arg("tga").toStdString().c_str(), "normal_map", targetStateSet, normmap_tex_unit,status_text);
	//buildTexture_cb(example_target.arg("spec_map_plane").arg("tga").toStdString().c_str(), "specular_map", targetStateSet, specmap_tex_unit,status_text);



	targetStateSet->setAttribute(targetProgram);

	return true;
}

osg::Geode * get_cobblestone_geode_v2( QString * status_text, cobblestone_examples::config_bistring_enum bitstring = cobblestone_examples::DEFAULT)
{
	osg::Geode * cobblestone = new osg::Geode;

	int VertexCount = sizeof(flat_geometry::lfg_vertices_version_2)/sizeof(flat_geometry::lfg_vertices_version_2[0]);
	int IndexCount = sizeof(flat_geometry::lfg_indexes_version_2) / sizeof(flat_geometry::lfg_indexes_version_2[0]);

	cobblestone->addDrawable( getGeometryCust_cb(VertexCount,IndexCount,flat_geometry::lfg_vertices_version_2, flat_geometry::lfg_indexes_version_2,osg::PrimitiveSet::PATCHES, bitstring));

	return cobblestone;
}

osg::Node * cobblestone_examples::example_cobblestone_simple_dynamic( QString * status_text, const char * spacing/* = EQUAL_SPACING*/ )
{
	osg::Geode * cobblestone = get_cobblestone_geode_v2( status_text );

	example_cobblestone_add_program_and_textures( cobblestone, QString("cobblestone_shader"), status_text, spacing);

	return cobblestone;
}

osg::Node * cobblestone_examples::example_cobblestone_numerous_dynamic( QString * status_text )
{
	osg::Group * cobblestoneGroup = new osg::Group;

	osg::Geode * cobblestoneGeode = get_cobblestone_geode_v2( status_text );

	example_cobblestone_add_program_and_textures( cobblestoneGroup, QString("cobblestone_shader"), status_text);

	float geometry_range = 2.0f;

	for(int i = - 5; i <= 5; i++)
	{
		float x = i * geometry_range;
		for(int j = - 5; j <= 5; j++)
		{
			float y = j * geometry_range;
			osg::PositionAttitudeTransform * locationNode = new osg::PositionAttitudeTransform();
			locationNode->setPosition(osg::Vec3d(x,y,0.0f));
			locationNode->addChild(cobblestoneGeode);

			cobblestoneGroup->addChild(locationNode);
		}
	}

	return cobblestoneGroup;


}



osg::Node * cobblestone_examples::example_cobblestone_simple_dynamic_fixed_gaps( QString * status_text, const char * alternativeShaderProgram, cobblestone_examples::config_bistring_enum bitstring )
{
	osg::Geode * cobblestone = get_cobblestone_geode_v2( status_text, bitstring );

	example_cobblestone_add_program_and_textures( cobblestone, QString(alternativeShaderProgram), status_text, EQUAL_SPACING, bitstring);//QString("cobblestone_shader_v2"), status_text);

	return cobblestone;
}

osg::Node * cobblestone_examples::example_cobblestone_numerous_dynamic_fixed_gaps( QString * status_text, const char * alternativeShaderProgram, cobblestone_examples::config_bistring_enum bitstring)
{
	osg::Group * cobblestoneGroup = new osg::Group;

	osg::Geode * cobblestoneGeode = get_cobblestone_geode_v2( status_text, bitstring );

	example_cobblestone_add_program_and_textures( cobblestoneGroup, QString(alternativeShaderProgram /*!= NULL ? alternativeShaderProgram : "cobblestone_shader_v2"*/), status_text, EQUAL_SPACING, bitstring);

	float geometry_range = 2.0f;

	for(int i = - 5; i <= 5; i++)
	{
		float x = i * geometry_range;
		for(int j = - 5; j <= 5; j++)
		{
			float y = j * geometry_range;
			osg::PositionAttitudeTransform * locationNode = new osg::PositionAttitudeTransform();
			locationNode->setPosition(osg::Vec3d(x,y,0.0f));
			locationNode->addChild(cobblestoneGeode);

			cobblestoneGroup->addChild(locationNode);
		}
	}

	return cobblestoneGroup;
}


