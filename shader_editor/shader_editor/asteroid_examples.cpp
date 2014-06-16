#include "asteroid_examples.h"
#include "piece_of_stone.h"
#include "blender_vertex_struct.h"
#include "meteor_reduced_better_uv.h"
#include "meteor_reduced_better_uv_backup_2.h"
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/LOD>
#include <osgDB/ReadFile>
#include <stdexcept>
#include <QMessageBox>
#include <QFile>
#include <QInputDialog>
#include <osgDB/ReadFile>


osg::Program * asteroid_examples::buildProgramFromFiles( QString VS, QString TCS, QString TES, QString GS, QString FS )
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
		np->addShader(new osg::Shader(osg::Shader::TESSEVALUATION,QString(tesFile.readAll()).toStdString()));
	if(!GS.isEmpty())
		np->addShader(new osg::Shader(osg::Shader::GEOMETRY,QString(gsFile.readAll()).toStdString()));
	np->addShader(new osg::Shader(osg::Shader::FRAGMENT,QString(fsFile.readAll()).toStdString()));
	return np;
}

void addTangentBitangentVectors( osg::Geometry * targetGeometry, int *tangentBindingValue, int *bitangentBindingValue,
	int vertexBindingValue, int normalBindingValue, int uvBindingValue,
	/*int VertexCount, */int IndexCount, const unsigned int source_indexes[])
{
	

	osg::Vec3Array * vertices = dynamic_cast<osg::Vec3Array*>(targetGeometry->getVertexAttribArray(vertexBindingValue));
	osg::Vec3Array * normals = dynamic_cast<osg::Vec3Array*>(targetGeometry->getVertexAttribArray(normalBindingValue));
	osg::Vec2Array * texcoords = dynamic_cast<osg::Vec2Array*>(targetGeometry->getVertexAttribArray(uvBindingValue));
	osg::Vec3Array * tangents  = new osg::Vec3Array();
	osg::Vec3Array * bitangents  = new osg::Vec3Array();
	std::vector< std::vector<osg::Vec3> > tangents_per_vertex;
	std::vector< std::vector<osg::Vec3> > bitangents_per_vertex;
	unsigned int VertexCount = vertices->getNumElements();

	tangents_per_vertex.resize(VertexCount);
	bitangents_per_vertex.resize(VertexCount);

	//unsigned int * indexes = source_indexes;//targetGeometry->getPrimitiveSetList;

	unsigned int triangle_idxs[3];

	for (int i = 0; i < IndexCount; i+=3)
	{
		triangle_idxs[0] = source_indexes[i];//meteor::indexes[i];
		triangle_idxs[1] = source_indexes[i+1];//meteor::indexes[i+1];
		triangle_idxs[2] = source_indexes[i+2];//meteor::indexes[i+2];
		for (int j = 0; j < 3; j++)
		{
			osg::Vec3 A = vertices->at(triangle_idxs[j]);
			osg::Vec3 B = vertices->at(triangle_idxs[(j+1)%3]);
			osg::Vec3 C = vertices->at(triangle_idxs[(j+2)%3]);

			osg::Vec3 N = normals->at(triangle_idxs[j]);

			osg::Vec2 H = texcoords->at(triangle_idxs[j]);
			osg::Vec2 K = texcoords->at(triangle_idxs[(j+1)%3]);
			osg::Vec2 L = texcoords->at(triangle_idxs[(j+2)%3]);

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

	if (tangentBindingValue && bitangentBindingValue)
	{
		*tangentBindingValue = 3;
		*bitangentBindingValue = 4;
		targetGeometry->setVertexAttribArray(*tangentBindingValue,tangents);
		targetGeometry->setVertexAttribBinding(*tangentBindingValue,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
		targetGeometry->setVertexAttribArray(*bitangentBindingValue,bitangents);
		targetGeometry->setVertexAttribBinding(*bitangentBindingValue,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	}

	//geometry->addPrimitiveSet(new osg::DrawElementsUInt(/*osg::PrimitiveSet::PATCHES*/PrimitiveSetMode,IndexCount,/*meteor::indexes*/source_indexes));

	return;
}

osg::Geometry * getAsteroidGeometryCust( int *vertexBindingValue, int *normalBindingValue, int *uvBindingValue, int *tangentBindingValue, int *bitangentBindingValue,
	int VertexCount, int IndexCount, const struct vertex_struct source_vertices[], const unsigned int source_indexes[], osg::PrimitiveSet::Mode PrimitiveSetMode)
{
	if (!vertexBindingValue || !normalBindingValue || !uvBindingValue)
	{
		return NULL;
	}
	osg::Geometry *geometry = new osg::Geometry();
	geometry->setUseVertexBufferObjects(true);
// 	int VertexCount = meteor::vertex_count[0];
// 	int IndexCount = sizeof(meteor::indexes) / sizeof(meteor::indexes[0]);

	osg::Vec2Array* tesmap_texcoords = new osg::Vec2Array();
	osg::Vec3Array* vertices = new osg::Vec3Array();
	osg::Vec3Array* normals = new osg::Vec3Array();	
	osg::Vec3Array* tangents = NULL;
	osg::Vec3Array* bitangents = NULL;

	for(int i=0;i<VertexCount;i++){
		vertices->push_back(osg::Vec3(source_vertices[i].x,source_vertices[i].y,source_vertices[i].z));
		normals->push_back(osg::Vec3(source_vertices[i].nx,source_vertices[i].ny,source_vertices[i].nz));
		tesmap_texcoords->push_back(osg::Vec2(source_vertices[i].u,source_vertices[i].v));
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
		addTangentBitangentVectors(geometry, tangentBindingValue, bitangentBindingValue,
			*vertexBindingValue, *normalBindingValue, *uvBindingValue, IndexCount, source_indexes);
		/*geometry->setVertexAttribArray(*tangentBindingValue,tangents);
		geometry->setVertexAttribBinding(*tangentBindingValue,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
		geometry->setVertexAttribArray(*bitangentBindingValue,bitangents);
		geometry->setVertexAttribBinding(*bitangentBindingValue,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);*/
	}

	geometry->addPrimitiveSet(new osg::DrawElementsUInt(/*osg::PrimitiveSet::PATCHES*/PrimitiveSetMode,IndexCount,/*meteor::indexes*/source_indexes));

	return geometry;
}

void asteroid_examples::addTangentBitangentVectorsToExistingGeometry(osg::Geometry * targetGeometry, osg::PrimitiveSet::Mode mode )
{
	int vertexBindingValue = 0;
	int normalBindingValue = 1;
	int uvBindingValue = 2;
	int tangentBindingValue = 3;
	int bitangentBindingValue = 4;

	unsigned int * source_indexes = NULL;
	int IndexCount = 0;

	osg::Geometry::PrimitiveSetList  primList = targetGeometry->getPrimitiveSetList();

	if(primList.size() < 1)
		return;



	std::vector<unsigned int> source_indexes_vector;
	source_indexes_vector.reserve(primList.size() * 3);

	for(int i = 0; i < primList.size(); i++)
		for(int k = 0; k < primList[i]->getNumIndices(); k++)
		{
			source_indexes_vector.push_back(primList[i]->index(k));
		}

	/*osg::PrimitiveSet * primitiveSet = primList[0];
	if(!primitiveSet)
		return;*/



	IndexCount = source_indexes_vector.size();
	source_indexes = new unsigned int [IndexCount];

	for(unsigned int i = 0; i < IndexCount; i++)
		source_indexes[i] = source_indexes_vector[i];

	osg::DrawElementsUInt * indexElements = new osg::DrawElementsUInt(/*osg::PrimitiveSet::TRIANGLES*/mode, IndexCount, source_indexes);

	targetGeometry->removePrimitiveSet(0,primList.size());
	targetGeometry->addPrimitiveSet(indexElements);

	targetGeometry->setUseVertexBufferObjects(true);
	targetGeometry->setUseDisplayList(false);

	addTangentBitangentVectors(targetGeometry, &tangentBindingValue, &bitangentBindingValue,
		vertexBindingValue, normalBindingValue, uvBindingValue, IndexCount, source_indexes);

	delete source_indexes;
}

osg::Geometry * getAsteroidGeometry( int *vertexBindingValue, int *normalBindingValue, int *uvBindingValue, int *tangentBindingValue, int *bitangentBindingValue )
{
	int VertexCount = meteor::vertex_count[0];
	int IndexCount = sizeof(meteor::indexes) / sizeof(meteor::indexes[0]);
	return getAsteroidGeometryCust(vertexBindingValue, normalBindingValue, uvBindingValue, tangentBindingValue, bitangentBindingValue, 
		VertexCount, IndexCount, meteor::vertices, meteor::indexes, osg::PrimitiveSet::PATCHES);
}

osg::Geometry * getAsteroidGeometryBetterUVs( int *vertexBindingValue, int *normalBindingValue, int *uvBindingValue, int *tangentBindingValue, int *bitangentBindingValue )
{
	int VertexCount = asteroid_better_uvs_backup_2::vertex_count[0];
	int IndexCount = sizeof(asteroid_better_uvs_backup_2::indexes) / sizeof(asteroid_better_uvs_backup_2::indexes[0]);
	return getAsteroidGeometryCust(vertexBindingValue, normalBindingValue, uvBindingValue, tangentBindingValue, bitangentBindingValue, 
		VertexCount, IndexCount, asteroid_better_uvs_backup_2::vertices, asteroid_better_uvs_backup_2::indexes, osg::PrimitiveSet::PATCHES);
}

osg::Geometry * getAsteroidGeometryBetterUVs_Triangles( int *vertexBindingValue, int *normalBindingValue, int *uvBindingValue, int *tangentBindingValue, int *bitangentBindingValue )
{
	int VertexCount = /*meteor*/asteroid_better_uvs_backup_2::vertex_count[0];
	int IndexCount = sizeof(/*asteroid_better_uvs*/asteroid_better_uvs_backup_2::indexes) / sizeof(/*asteroid_better_uvs*/asteroid_better_uvs_backup_2::indexes[0]);
	return getAsteroidGeometryCust(vertexBindingValue, normalBindingValue, uvBindingValue, tangentBindingValue, bitangentBindingValue, 
		VertexCount, IndexCount, /*asteroid_better_uvs*/asteroid_better_uvs_backup_2::vertices, /*asteroid_better_uvs*/asteroid_better_uvs_backup_2::indexes, osg::PrimitiveSet::Mode::TRIANGLES);
}

bool asteroid_examples::buildTexture( const char * filename, const char * texture_uniform_name,  osg::StateSet * targetSet, int texture_unit, QString * status_text )
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

osg::Node * asteroid_examples::example_asteroid_gaps( QString * status_text , const char * displacement_map_filename)
{
	QString example_folder = QString("examples/asteroid_gaps/%1");
	int vertices_binding, normals_binding, uvcoords_binding, tangent_binding, bitangent_binding;

	osg::Geometry *geometry = getAsteroidGeometry(&vertices_binding,
		&normals_binding,
		&uvcoords_binding,
		&tangent_binding,
		&bitangent_binding);

	osg::Geode *geode=new osg::Geode();
	geode->addDrawable(geometry);

	osg::Group * Groupnode = new osg::Group;
	Groupnode->addChild(geode);



	osg::Program * p = buildProgramFromFiles(example_folder.arg("sd_shader.vs"),
		example_folder.arg("sd_shader.tcs"),
		displacement_map_filename ? example_folder.arg("sd_shader_MeshMapper.tes") : example_folder.arg("sd_shader.tes"),
		"",
		example_folder.arg("sd_shader.fs"));
	if (p==NULL)
	{
		status_text->clear();
		*status_text = QString ("asteroid_examples : example_asteroid_gaps : program building failure");
		return NULL;
	}
	geode->getOrCreateStateSet()->setAttribute(p);

	osg::StateSet *state;
	state = geode->getOrCreateStateSet();

	osg::Program * this_program = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));
	this_program->addBindAttribLocation("myVertex",vertices_binding);
	this_program->addBindAttribLocation("myNormal",normals_binding);
	this_program->addBindAttribLocation("myTexCoord",uvcoords_binding);
	this_program->addBindAttribLocation("myTangents",tangent_binding);
	this_program->addBindAttribLocation("myBinormals",bitangent_binding);

	const int displacement_texture_unit = 0;
	const int normal_texture_unit = 1;
	const int diffuse_texture_unit = 2;

	if(displacement_map_filename)
		buildTexture( example_folder.arg(displacement_map_filename).toStdString().c_str(), "displ_texture",  state, displacement_texture_unit, status_text );
	else
		buildTexture( example_folder.arg("ag_displacement_texture.tga").toStdString().c_str(), "displ_texture",  state, displacement_texture_unit, status_text );
	buildTexture( example_folder.arg("ag_normal_texture.tga").toStdString().c_str(), "normal_texture",  state, normal_texture_unit, status_text );
	buildTexture( example_folder.arg("ag_diffuse_texture.tga").toStdString().c_str(), "diffuse_texture",  state, diffuse_texture_unit, status_text );

	this_program->setParameter(GL_PATCH_VERTICES,3);

	

	this_program->addBindFragDataLocation("FragColor",0);
	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.75f, 0.75f, 0.75f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));

	state->addUniform(new osg::Uniform("TessLevelInner",16.0f));
	state->addUniform(new osg::Uniform("TessLevelOuter",16.0f));
	state->addUniform(new osg::Uniform("DisplacementFactor",0.290721f));

	return Groupnode;
}

osg::Node * asteroid_examples::example_asteroid_better_uvs( QString * status_text, const char * displacement_map_filename )
{
	QString example_folder = QString("examples/asteroid_better_uvs/%1");
	int vertices_binding, normals_binding, uvcoords_binding, tangent_binding, bitangent_binding;

	osg::Geometry *geometry = getAsteroidGeometryBetterUVs(&vertices_binding,
		&normals_binding,
		&uvcoords_binding,
		&tangent_binding,
		&bitangent_binding);

	osg::Geode *geode=new osg::Geode();
	geode->addDrawable(geometry);

	osg::Group * Groupnode = new osg::Group;
	Groupnode->addChild(geode);



	osg::Program * p = buildProgramFromFiles(example_folder.arg("sd_shader.vs"),
		example_folder.arg("sd_shader.tcs"),
		displacement_map_filename ? example_folder.arg("sd_shader_MeshMapper.tes") : example_folder.arg("sd_shader.tes"),//example_folder.arg("sd_shader.tes"),
		"",
		example_folder.arg("sd_shader.fs"));
	if (p==NULL)
	{
		status_text->clear();
		*status_text = QString ("asteroid_examples : example_asteroid_gaps : program building failure");
		return NULL;
	}
	geode->getOrCreateStateSet()->setAttribute(p);

	osg::StateSet *state;
	state = geode->getOrCreateStateSet();

	osg::Program * this_program = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));
	this_program->addBindAttribLocation("myVertex",vertices_binding);
	this_program->addBindAttribLocation("myNormal",normals_binding);
	this_program->addBindAttribLocation("myTexCoord",uvcoords_binding);
	this_program->addBindAttribLocation("myTangents",tangent_binding);
	this_program->addBindAttribLocation("myBinormals",bitangent_binding);

	const int displacement_texture_unit = 0;
	const int normal_texture_unit = 1;
	const int diffuse_texture_unit = 2;

	//buildTexture( example_folder.arg("abuv_displacement_texture_2.tga").toStdString().c_str(), "displ_texture",  state, displacement_texture_unit, status_text );
	if(displacement_map_filename != NULL)
		buildTexture( example_folder.arg(displacement_map_filename).toStdString().c_str(), "displ_texture",  state, displacement_texture_unit, status_text );
	else
		buildTexture( example_folder.arg("abuv_displacement_texture_2.tga").toStdString().c_str(), "displ_texture",  state, displacement_texture_unit, status_text );
	buildTexture( example_folder.arg("abuv_normal_texture_2.tga").toStdString().c_str(), "normal_texture",  state, normal_texture_unit, status_text );
	buildTexture( example_folder.arg("abuv_diffuse_texture.tga").toStdString().c_str(), "diffuse_texture",  state, diffuse_texture_unit, status_text );

	this_program->setParameter(GL_PATCH_VERTICES,3);



	this_program->addBindFragDataLocation("FragColor",0);
	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.75f, 0.75f, 0.75f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));

	state->addUniform(new osg::Uniform("TessLevelInner",16.0f));
	state->addUniform(new osg::Uniform("TessLevelOuter",16.0f));
	state->addUniform(new osg::Uniform("DisplacementFactor",0.290721f));

	return Groupnode;
}

osg::Node * asteroid_examples::example_asteroid_better_uvs_fixed_textures( QString * status_text, unsigned long config_bitstring , float TessLevel)
{
	QString example_folder = QString("examples/asteroid_better_uvs_fixed_texture/%1");
	int vertices_binding, normals_binding, uvcoords_binding, tangent_binding, bitangent_binding;

	osg::Geometry *geometry = getAsteroidGeometryBetterUVs(&vertices_binding,
		&normals_binding,
		&uvcoords_binding,
		&tangent_binding,
		&bitangent_binding);

	osg::Geode *geode=new osg::Geode();
	geode->addDrawable(geometry);

	osg::Group * Groupnode = new osg::Group;
	Groupnode->addChild(geode);



	osg::Program * p = buildProgramFromFiles(example_folder.arg("sd_shader.vs"),
		example_folder.arg(config_bitstring & config_bitstring_enum::TESSLOD_NODE?"sd_shader_tesslod.tcs":"sd_shader.tcs"),
		example_folder.arg("sd_shader.tes"),
		"",
		example_folder.arg(config_bitstring & config_bitstring_enum::NO_HIGHLIGHT ?"sd_shader_nohighlight.fs":"sd_shader.fs"));
	if (p==NULL)
	{
		status_text->clear();
		*status_text = QString ("asteroid_examples : example_asteroid_gaps : program building failure");
		return NULL;
	}
	geode->getOrCreateStateSet()->setAttribute(p);

	osg::StateSet *state;
	state = geode->getOrCreateStateSet();

	osg::Program * this_program = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));
	this_program->addBindAttribLocation("myVertex",vertices_binding);
	this_program->addBindAttribLocation("myNormal",normals_binding);
	this_program->addBindAttribLocation("myTexCoord",uvcoords_binding);
	this_program->addBindAttribLocation("myTangents",tangent_binding);
	this_program->addBindAttribLocation("myBinormals",bitangent_binding);

	const int displacement_texture_unit = 0;
	const int normal_texture_unit = 1;
	const int diffuse_texture_unit = 2;

	//buildTexture( example_folder.arg("abuv_displacement_texture_4.tga").toStdString().c_str(), "displ_texture",  state, displacement_texture_unit, status_text );
	buildTexture( example_folder.arg("abuvm_displacement_texture.tga").toStdString().c_str(), "displ_texture",  state, displacement_texture_unit, status_text );
	buildTexture( example_folder.arg("abuv_normal_texture_2.tga").toStdString().c_str(), "normal_texture",  state, normal_texture_unit, status_text );
	buildTexture( example_folder.arg("abuv_diffuse_texture_2.tga").toStdString().c_str(), "diffuse_texture",  state, diffuse_texture_unit, status_text );

	this_program->setParameter(GL_PATCH_VERTICES,3);



	this_program->addBindFragDataLocation("FragColor",0);
	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.75f, 0.75f, 0.75f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));

	state->addUniform(new osg::Uniform("TessLevelInner",TessLevel));
	state->addUniform(new osg::Uniform("TessLevelOuter",TessLevel));
	state->addUniform(new osg::Uniform("DisplacementFactor",0.290721f));

	return Groupnode;
}

osg::Geode * example_asteroid_dynamic_tessellation(QString * status_text, float TessLevel, unsigned long config_bitstring = 0)
{
	QString example_folder = QString("examples/asteroid_dynamic_tessellation/%1");
	int vertices_binding, normals_binding, uvcoords_binding, tangent_binding, bitangent_binding;

	osg::Geometry *geometry = getAsteroidGeometryBetterUVs(&vertices_binding,
		&normals_binding,
		&uvcoords_binding,
		&tangent_binding,
		&bitangent_binding);

	osg::Geode *geode=new osg::Geode();
	geode->addDrawable(geometry);





	osg::Program * p = asteroid_examples::buildProgramFromFiles(example_folder.arg("sd_shader.vs"),
		example_folder.arg(config_bitstring & asteroid_examples::config_bitstring_enum::TESSLOD_NODE ?"sd_shader_tesslod.tcs":"sd_shader.tcs"),
		example_folder.arg("sd_shader.tes"),
		"",
		example_folder.arg("sd_shader.fs"));
	if (p==NULL)
	{
		status_text->clear();
		*status_text = QString ("asteroid_examples : example_asteroid_gaps : program building failure");
		return NULL;
	}
	geode->getOrCreateStateSet()->setAttribute(p);

	osg::StateSet *state;
	state = geode->getOrCreateStateSet();

	osg::Program * this_program = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));
	this_program->addBindAttribLocation("myVertex",vertices_binding);
	this_program->addBindAttribLocation("myNormal",normals_binding);
	this_program->addBindAttribLocation("myTexCoord",uvcoords_binding);
	this_program->addBindAttribLocation("myTangents",tangent_binding);
	this_program->addBindAttribLocation("myBinormals",bitangent_binding);

	const int displacement_texture_unit = 0;
	const int normal_texture_unit = 1;
	const int diffuse_texture_unit = 2;

	asteroid_examples::buildTexture( example_folder.arg("adtm_displacement_texture.tga").toStdString().c_str(), "displ_texture",  state, displacement_texture_unit, status_text );
	asteroid_examples::buildTexture( example_folder.arg("adt_normal_texture.tga").toStdString().c_str(), "normal_texture",  state, normal_texture_unit, status_text );

	this_program->setParameter(GL_PATCH_VERTICES,3);



	this_program->addBindFragDataLocation("FragColor",0);
	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.75f, 0.75f, 0.75f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));

	if(!(config_bitstring & asteroid_examples::config_bitstring_enum::TESSLOD_NODE))
	{
		state->addUniform(new osg::Uniform("TessLevelInner",TessLevel));
		state->addUniform(new osg::Uniform("TessLevelOuter",TessLevel));
	}
	state->addUniform(new osg::Uniform("DisplacementFactor",0.290721f));

	return geode;

}

osg::Geode * example_asteroid_without_tessellation(QString * status_text)
{
	QString example_folder = QString("examples/asteroid_no_tessellation/%1");
	int vertices_binding, normals_binding, uvcoords_binding, tangent_binding, bitangent_binding;

	osg::Geometry *geometry = getAsteroidGeometryBetterUVs_Triangles(&vertices_binding,
		&normals_binding,
		&uvcoords_binding,
		&tangent_binding,
		&bitangent_binding);

	osg::Geode *geode=new osg::Geode();
	geode->addDrawable(geometry);





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
	geode->getOrCreateStateSet()->setAttribute(p);

	osg::StateSet *state;
	state = geode->getOrCreateStateSet();

	osg::Program * this_program = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));
	this_program->addBindAttribLocation("myVertex",vertices_binding);
	this_program->addBindAttribLocation("myNormal",normals_binding);
	this_program->addBindAttribLocation("myTexCoord",uvcoords_binding);
	this_program->addBindAttribLocation("myTangents",tangent_binding);
	this_program->addBindAttribLocation("myBinormals",bitangent_binding);

	const int normal_texture_unit = 1;

	asteroid_examples::buildTexture( example_folder.arg("ant_normal_texture.tga").toStdString().c_str(), "normal_texture",  state, normal_texture_unit, status_text );

	this_program->setParameter(GL_TRIANGLES,3);



	this_program->addBindFragDataLocation("FragColor",0);
	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.6f, 0.6f, 0.6f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));

	return geode;
}

osg::Node * asteroid_examples::example_asteroid_dynamic_tessellation_16( QString * status_text  )
{
	osg::Geode * geode = example_asteroid_dynamic_tessellation(status_text, 16.0f);
	osg::Group * Groupnode = new osg::Group;
	Groupnode->addChild(geode);
	return Groupnode;
}

osg::Node * asteroid_examples::example_asteroid_dynamic_tessellation_64( QString * status_text, unsigned long config_bitstring  )
{
	osg::Geode * geode = example_asteroid_dynamic_tessellation(status_text, 64.0f, config_bitstring);
	osg::Group * Groupnode = new osg::Group;
	Groupnode->addChild(geode);
	return Groupnode;
}

osg::Node * example_asteroid_dynamic_tessellation_cust_with_common_LOD( QString * status_text, float LOD_distance, float basic_tess)
{
	osg::LOD * lod_node = new osg::LOD();


	osg::Geode * geode_dt = example_asteroid_dynamic_tessellation(status_text, basic_tess);
	osg::Geode * geode_nt = example_asteroid_without_tessellation(status_text);

	lod_node->addChild( geode_nt, LOD_distance, FLT_MAX );
	lod_node->addChild( geode_dt, 00.0f, LOD_distance );

	return lod_node;
}

osg::Node * asteroid_examples::example_asteroid_dynamic_tessellation_16_with_Common_LOD( QString * status_text  )
{
	return example_asteroid_dynamic_tessellation_cust_with_common_LOD(status_text, 100.0f, 16.0f);
}

osg::Node * asteroid_examples::example_asteroid_dynamic_tessellation_64_with_Common_LOD( QString * status_text  )
{
	/*osg::Geode * geode = example_asteroid_dynamic_tessellation(status_text, 64.0f);
	osg::Group * Groupnode = new osg::Group;
	Groupnode->addChild(geode);
	return Groupnode;*/

	return example_asteroid_dynamic_tessellation_cust_with_common_LOD(status_text, 100.0f, 64.0f);
/*
	osg::LOD * lod_node = new osg::LOD();


	osg::Geode * geode_dt = example_asteroid_dynamic_tessellation(status_text, 64.0f);
	osg::Geode * geode_nt = example_asteroid_without_tessellation(status_text);

	lod_node->addChild( geode_nt, 70.0f, FLT_MAX );
	lod_node->addChild( geode_dt, 00.0f, 100.0f );

	return lod_node;*/
}

osg::Node * asteroid_examples::example_asteroid_dynamic_tessellation_16_with_Common_LOD_cust_range( QString * status_text  )
{
	QInputDialog * dialog = new QInputDialog(NULL, Qt::Dialog);

	dialog->setComboBoxEditable(true);
	dialog->setInputMode(QInputDialog::TextInput);

	QStringList rangeList;
	rangeList << "10.0";
	rangeList << "20.0";
	rangeList << "30.0";
	rangeList << "40.0";
	rangeList << "50.0";
	rangeList << "60.0";
	rangeList << "70.0";
	rangeList << "80.0";
	rangeList << "90.0";
	rangeList << "100.0";

	dialog->setComboBoxItems(rangeList);
	dialog->setTextValue("50.0");

	if(!dialog->exec())
		return NULL;

	bool ok = true;
	float range = dialog->textValue().toFloat(&ok);

	if(!ok)
		return NULL;	

	return example_asteroid_dynamic_tessellation_cust_with_common_LOD(status_text, range, 16.0f);
}

osg::Node * asteroid_examples::example_asteroid_dynamic_tessellation_64_with_Common_LOD_cust_range( QString * status_text  )
{
	QInputDialog * dialog = new QInputDialog(NULL, Qt::Dialog);

	dialog->setComboBoxEditable(true);

	dialog->setInputMode(QInputDialog::TextInput);

	QStringList rangeList;

	rangeList << "10.0";
	rangeList << "20.0";
	rangeList << "30.0";
	rangeList << "40.0";
	rangeList << "50.0";
	rangeList << "60.0";
	rangeList << "70.0";
	rangeList << "80.0";
	rangeList << "90.0";
	rangeList << "100.0";

	dialog->setComboBoxItems(rangeList);
	dialog->setTextValue("50.0");
	if(!dialog->exec())
		return NULL;

	bool ok = true;
	float range = dialog->textValue().toFloat(&ok);

	if(!ok)
		return NULL;

	return example_asteroid_dynamic_tessellation_cust_with_common_LOD(status_text, range, 16.0f);
}

osg::Node * asteroid_examples::returnNodeFromFile ( QString * status_text, QString file_name )
{
	osg::ref_ptr<osgDB::Options> options = new osgDB::Options();
	options->setOptionString("noTriStripPolygons noTesselateLargePolygons");

	osg::Node * asteroidLevel1 = osgDB::readNodeFile(file_name.toStdString().c_str(), options);
	return asteroidLevel1;
}

osg::Geometry * asteroid_examples::getGeometryOfFirstGeodeFromTheTree(osg::Node * target)
{
	if(target)
	{
		if(target->asGeode())
		{
			if(target->asGeode()->getNumDrawables() > 0)
				return target->asGeode()->getDrawable(0)->asGeometry();
			/*for(unsigned int i = 0; i < target->asGeode()->getNumDrawables(); i++)
				addTangentBitangentVectorsToExistingGeometry(target->asGeode()->getDrawable(i)->asGeometry());*/
		}
		else if(target->asGroup())
			for(unsigned int i = 0; i < target->asGroup()->getNumChildren(); i++)
				return getGeometryOfFirstGeodeFromTheTree(target->asGroup()->getChild(i));
		
		//for
	}
	return NULL;
	//addTangentBitangentVectorsToExistingGeometry
}

osg::Node * asteroid_examples::example_asteroid_LOD_no_tessellation( QString * status_text )
{
	QString example_folder = QString("examples/asteroid_no_tessellation_lod/%1");
	
	osg::Node * node_0_01 = returnNodeFromFile(status_text, example_folder.arg("asteroid_lod_notess_0_01_fixed_2.ive"));

	osg::Node * node_0_05 = returnNodeFromFile(status_text, example_folder.arg("asteroid_lod_notess_0_05_fixed_2.ive"));


	if(!node_0_01 || !node_0_05)
	{
		status_text->clear();
		status_text->append("Nepodarilo se otevrit soubor s node: %1");
		if(!node_0_01)
			*status_text = status_text->arg(example_folder.arg("asteroid_lod_notess_0_01.ive"));;
		if(!node_0_05)
			*status_text = status_text->arg(example_folder.arg("asteroid_lod_notess_0_05.ive"));;
		return NULL;
	}
	osg::Geometry * node_0_01_geometry = getGeometryOfFirstGeodeFromTheTree(node_0_01);
	if(node_0_01_geometry)
		addTangentBitangentVectorsToExistingGeometry(node_0_01_geometry, osg::PrimitiveSet::TRIANGLES);
	osg::Geometry * node_0_05_geometry = getGeometryOfFirstGeodeFromTheTree(node_0_05);
	if(node_0_05_geometry)
		addTangentBitangentVectorsToExistingGeometry(node_0_05_geometry, osg::PrimitiveSet::TRIANGLES);

	const int normal_texture_unit = 0;

	buildTexture( example_folder.arg("ant_lod_normal_texture_0_001_4.tga").toStdString().c_str(), "normal_texture",  node_0_01->getOrCreateStateSet(), normal_texture_unit, status_text );
	buildTexture( example_folder.arg("ant_lod_normal_texture_0_005_3.tga").toStdString().c_str(), "normal_texture",  node_0_05->getOrCreateStateSet(), normal_texture_unit, status_text );

	osg::LOD * lod_node = new osg::LOD();
	osg::Program * p = buildProgramFromFiles(example_folder.arg("sd_shader.vs"),
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
	lod_node->getOrCreateStateSet()->setAttribute(p);

	osg::Geode * from_code = example_asteroid_without_tessellation(status_text);

	//from_code->

	lod_node->addChild( from_code, 50.0f, FLT_MAX);
	//lod_node->addChild( node_0_01, 20.0f, 50.0f );
	lod_node->addChild( node_0_05, 00.0f, 50.0f );


	int vertices_binding = 0;
	int normals_binding = 1;
	int uvcoords_binding = 2;

	osg::StateSet *state;
	state = lod_node->getOrCreateStateSet();

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

	return lod_node;
}

osg::Node * asteroid_examples::example_asteroid_LOD_no_tessellation_cust_range( QString * status_text )
{
	return NULL;
}