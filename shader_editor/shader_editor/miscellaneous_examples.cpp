#include "miscellaneous_examples.h"

#include <osg/Geode>
#include <QMessageBox>
#include <QFile>
#include <osg/PolygonMode>

osg::Program * buildProgramFromFiles( QString VS, QString TCS, QString TES, QString GS, QString FS )
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


osg::Node * miscellaneous_examples::triangle_example(const char * shader_file_name)
{
	QString example_folder = QString("examples/miscellaneous_examples/%1.%2").arg(shader_file_name);
	osg::Vec3d v1(-1.0,-1.0,0.0);
	osg::Vec3d v2(1.0,-1.0,0.0);
	osg::Vec3d v3(0.0,0.86,0.0);
	unsigned int indexes[] = {0,1,2};
	osg::Vec3Array * gArray = new osg::Vec3Array;

	gArray->push_back(v1);
	gArray->push_back(v2);
	gArray->push_back(v3);

	osg::Geometry  * geom = new osg::Geometry();
	geom->setUseVertexBufferObjects(true);

	geom->setVertexAttribArray(0,gArray);
	geom->setVertexAttribBinding(0,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geom->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,3,indexes));

	osg::Program * this_program = buildProgramFromFiles(example_folder.arg("vs"),
														example_folder.arg("tcs"),
														example_folder.arg("tes"),
														"",
														example_folder.arg("fs"));
	this_program->addBindAttribLocation("myVertex",0);
	this_program->addBindFragDataLocation("FragColor",0);
	this_program->setParameter(GL_PATCH_VERTICES,3);

	osg::Geode * geode = new osg::Geode();

	geode->getOrCreateStateSet()->setAttribute(this_program);
	geode->getOrCreateStateSet()->setAttribute(new osg::PolygonMode(osg::PolygonMode::Face::FRONT_AND_BACK, osg::PolygonMode::LINE));
	
	geode->getOrCreateStateSet()->addUniform(new osg::Uniform("TessLevelInner",32.0f));
	geode->getOrCreateStateSet()->addUniform(new osg::Uniform("TessLevelOuter",32.0f));

	geode->addDrawable(geom);

	return geode;
}

osg::Node * miscellaneous_examples::quads_example(const char * shader_file_name)
{
	QString example_folder = QString("examples/miscellaneous_examples/%1.%2").arg(shader_file_name);
	osg::Vec3d v1(-1.0,-1.0,0.0);
	osg::Vec3d v2(1.0,-1.0,0.0);
	osg::Vec3d v3(1.0,1.00,0.0);
	osg::Vec3d v4(-1.0,1.00,0.0);
	osg::Vec3d c1(1.0,0.0,0.0);
	osg::Vec3d c2(0.0,1.0,0.0);
	osg::Vec3d c3(0.0,0.0,1.0);
	osg::Vec3d c4(1.0,1.0,0.0);
	unsigned int indexes[] = {0,1,3,2};
	osg::Vec3Array * gArray = new osg::Vec3Array;
	osg::Vec3Array * cArray = new osg::Vec3Array;

	gArray->push_back(v1);
	gArray->push_back(v2);
	gArray->push_back(v3);
	gArray->push_back(v4);

	cArray->push_back(c1);
	cArray->push_back(c2);
	cArray->push_back(c3);
	cArray->push_back(c4);

	osg::Geometry  * geom = new osg::Geometry();
	geom->setUseVertexBufferObjects(true);

	geom->setVertexAttribArray(0,gArray);
	geom->setVertexAttribBinding(0,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geom->setVertexAttribArray(1,cArray);
	geom->setVertexAttribBinding(1,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geom->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::PATCHES,4,indexes));

	osg::Program * this_program = buildProgramFromFiles(example_folder.arg("vs"),
		example_folder.arg("tcs"),
		example_folder.arg("tes"),
		"",
		example_folder.arg("fs"));
	this_program->addBindAttribLocation("myVertex",0);
	this_program->addBindAttribLocation("myColor",1);
	this_program->addBindFragDataLocation("FragColor",0);
	this_program->setParameter(GL_PATCH_VERTICES,4);

	osg::Geode * geode = new osg::Geode();

	geode->getOrCreateStateSet()->setAttribute(this_program);
	geode->getOrCreateStateSet()->setAttribute(new osg::PolygonMode(osg::PolygonMode::Face::FRONT_AND_BACK, osg::PolygonMode::LINE));

	geode->getOrCreateStateSet()->addUniform(new osg::Uniform("TessLevelInner",32.0f));
	geode->getOrCreateStateSet()->addUniform(new osg::Uniform("TessLevelOuter",32.0f));

	geode->addDrawable(geom);

	return geode;
}