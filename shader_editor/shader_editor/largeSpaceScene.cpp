#include "largeSpaceScene.h"
#include "Skybox.h"
#include "asteroid_examples.h"
#include <osg/Vec3d>
#include <random>
#include <QString>
#include <osg/PositionAttitudeTransform>
#include <exception>
#include <osgUtil/CullVisitor>
#include <QTime>

//zdroj teto tridy: 
//http://anidam.sourceforge.net/doxygen/skyBox_8cpp-source.html
class MoveEarthySkyWithEyePointTransform : public osg::Transform
{
public:
	virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const 
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.preMult(osg::Matrix::translate(eyePointLocal));
		}
		return true;
	}

	virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.postMult(osg::Matrix::translate(-eyePointLocal));
		}
		return true;
	}
};

struct missing_file : std::exception {
	missing_file(std::string filename) : std::exception(), 
		file_name (filename) {};


	std::string file_name;
	const char* what() const {return (std::string("Nepodarilo se otevrit soubor: ") + file_name).c_str();}
};

const double distance_scale = 100;
const int asteroid_count = 2;
const char * asteroid_name_list[] =
{
	"asteroid_1",
	"asteroid_2",
	"asteroid_3",
	"asteroid_4"
};

std::map <int, osg::ref_ptr<osg::Node> > asteroidMap;

QString example_location("examples/large_space_scene/%1.%2");
QString example_location_texture("examples/large_space_scene/%1_%2.%3");

int getRandomAsteroidIndex()
{
	/*int asteroidNameIndex = rand()  * 71.0f;
	return asteroid_name_list[asteroidNameIndex%asteroid_count];*/
	int i = ((float)rand()/RAND_MAX * 71.0f);
	return i % asteroid_count;
}

double getRandomScale()
{
	return (float)rand()/RAND_MAX*2.0f + 0.5f ;
}

osg::Vec3d getRandomPosition()
{
	osg::Vec3d directionNormal((float)rand()/RAND_MAX - 0.5f, (float)rand()/RAND_MAX - 0.5f, (float)rand()/RAND_MAX - 0.5f);
	directionNormal.normalize();
	double distance = distance_scale * ((float)rand()/RAND_MAX + 0.15f);
	osg::Vec3d targetPosition = directionNormal * distance;
	return targetPosition;
}

osg::Node * getRandomAsteroid()
{
	int index = getRandomAsteroidIndex();
	if (asteroidMap.count(index) == 1)
		return asteroidMap[index];

	osg::Node * selectedAsteroid = asteroid_examples::returnNodeFromFile(NULL, example_location.arg(asteroid_name_list[index]).arg("ive"));

	if(!selectedAsteroid)
	{
		missing_file exc(asteroid_name_list[index]);
		throw exc;
		return NULL;
	}

	osg::Geometry * asteroidGeometry = asteroid_examples::getGeometryOfFirstGeodeFromTheTree(selectedAsteroid);
	if(asteroidGeometry)
		asteroid_examples::addTangentBitangentVectorsToExistingGeometry(asteroidGeometry, osg::PrimitiveSet::PATCHES);

	const int normal_texture_unit = 0;
	const int diffuse_texture_unit = 1;
	const int displacement_texture_unit = 2;

	QString status_text;

	if(!asteroid_examples::buildTexture( example_location_texture.arg(asteroid_name_list[index]).arg("normal").arg("tga").toStdString().c_str(), 
		"normal_texture",  selectedAsteroid->getOrCreateStateSet(), normal_texture_unit, &status_text ))
	{
		missing_file exc(example_location_texture.arg(asteroid_name_list[index]).arg("normal").arg("tga").toStdString());
		throw exc;
	}
	if(!asteroid_examples::buildTexture( example_location_texture.arg(asteroid_name_list[index]).arg("diffuse").arg("tga").toStdString().c_str(), 
		"diffuse_texture",  selectedAsteroid->getOrCreateStateSet(), diffuse_texture_unit, &status_text ))
	{
		missing_file exc(example_location_texture.arg(asteroid_name_list[index]).arg("diffuse").arg("tga").toStdString());
		throw exc;
	}
	if(!asteroid_examples::buildTexture( example_location_texture.arg(asteroid_name_list[index]).arg("displacement").arg("tga").toStdString().c_str(), 
		"displacement_texture",  selectedAsteroid->getOrCreateStateSet(), displacement_texture_unit, &status_text ))
	{
		missing_file exc(example_location_texture.arg(asteroid_name_list[index]).arg("displacement").arg("tga").toStdString());
		throw exc;
	}

	asteroidMap[index] = selectedAsteroid;
	return selectedAsteroid;
}

void placeRandomAsteroids(int count, osg::Group * target)
{
	srand(QTime::currentTime().second());

	for (int i = 0; i < count; i++)
	{
		osg::Vec3d position = getRandomPosition();
		double scale = getRandomScale();
		osg::PositionAttitudeTransform * transformNode = new osg::PositionAttitudeTransform();
		transformNode->setPosition(position);
		transformNode->setScale(osg::Vec3d(scale,scale,scale));
		osg::Node * asteroid = getRandomAsteroid();
		transformNode->addChild(asteroid);
		target->addChild(transformNode);

	}

	//osg::Vec3d position(0.0,0.0,0.0);// = getRandomPosition();
	/*double scale = 50.0f; //= getRandomScale();
	osg::PositionAttitudeTransform * transformNode = new osg::PositionAttitudeTransform();
	//transformNode->setPosition(position);
	transformNode->setScale(osg::Vec3d(scale,scale,scale));*/
	
	//transformNode->addChild(asteroid);
	

}


osg::Node * largeSpaceScene::buildLargeSpaceScene(QString * status_text)
{
	osg::Skybox * sbox = new osg::Skybox(100);

	if(sbox->buildSkybox(	"examples/skybox_1024_front5.tga",
						"examples/skybox_1024_back6.tga",
						"examples/skybox_1024_top3.tga",
						"examples/skybox_1024_bottom4.tga",
						"examples/skybox_1024_right1.tga",
						"examples/skybox_1024_left2.tga") == false)
		return NULL;
	sbox->setCullingActive(false);
	/*osg::Transform * sbox_transform = new osg::Transform();
	sbox_transform->setReferenceFrame( osg::Transform::ABSOLUTE_RF );

	sbox_transform->addChild(sbox);*/

	/*osg::Transform* transform = new MoveEarthySkyWithEyePointTransform;
	transform->setCullingActive(false);
	transform->addChild(sbox);*/

	osg::Group * group = new osg::Group;
	group->addChild(sbox);
	//group->addChild(transform);
	osg::Group * asteroidGroup = new osg::Group;
	placeRandomAsteroids(10, asteroidGroup);
	group->addChild(asteroidGroup);

	osg::StateSet *state = asteroidGroup->getOrCreateStateSet();

	osg::Program * p = asteroid_examples::buildProgramFromFiles(example_location.arg("sd_shader").arg("vs"),
		example_location.arg("sd_shader").arg("tcs"),
		example_location.arg("sd_shader").arg("tes"),
		"",
		example_location.arg("sd_shader").arg("fs"));
	if (p==NULL)
	{
		status_text->clear();
		*status_text = QString ("asteroid_examples : example_asteroid_gaps : program building failure");
		return NULL;
	}
	asteroidGroup->getOrCreateStateSet()->setAttribute(p);

	//osg::Program * this_program = dynamic_cast<osg::Program *>(state->getAttribute(osg::StateAttribute::Type::PROGRAM));

	static const int vertices_binding = 0;
	static const int normals_binding = 1;
	static const int uvcoords_binding = 2;

	p->addBindAttribLocation("myVertex",vertices_binding);
	p->addBindAttribLocation("myNormal",normals_binding);
	p->addBindAttribLocation("myTexCoord",uvcoords_binding);
	p->addBindAttribLocation("myTangents",3);
	p->addBindAttribLocation("myBitangents",4);
	p->addBindFragDataLocation("FragColor",0);


	state->addUniform(new osg::Uniform("TessLevelInner",16.0f));
	state->addUniform(new osg::Uniform("TessLevelOuter",16.0f));
	state->addUniform(new osg::Uniform("DisplacementFactor",0.290721f));

	state->addUniform(new osg::Uniform("AmbientMaterial",osg::Vec3(0.04f, 0.04f, 0.04f)));
	state->addUniform(new osg::Uniform("DiffuseMaterial",osg::Vec3(0.6f, 0.6f, 0.6f)));
	state->addUniform(new osg::Uniform("LightPosition",osg::Vec3(0.25f, 0.25f, 1.0f)));

	return group;
}
