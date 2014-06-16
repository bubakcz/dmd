#include "Skybox.h"
#include <osg/Array>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

using namespace osg;

#define VERT_COUNT 4
#define FACES_COUNT 2
#define FACE_VERTEXES 3
#define VERTEX_BIND_VALUE 0
#define UV_BIND_VALUE 1

namespace face_all
{
	Vec2 uvs[] = {
		Vec2(0.0,0.0),
		Vec2(0.0,1.0),
		Vec2(1.0,0.0),
		Vec2(1.0,1.0)
	};

	GLuint indexArray[] = {0,1,2,1,2,3};
}

namespace face_front
{
	Vec3 vertices[] = {
		Vec3(1.0,1.0,-1.0),
		Vec3(1.0,1.0,1.0),

		Vec3(1.0,-1.0,-1.0),
		Vec3(1.0,-1.0,1.0)

	};
}

namespace face_back
{
	Vec3 vertices[] = {
		Vec3(-1.0,-1.0,-1.0),
		Vec3(-1.0,-1.0,1.0),
		Vec3(-1.0,1.0,-1.0),
		Vec3(-1.0,1.0,1.0)
	};
}

namespace face_bottom
{
	Vec3 vertices[] = {
		Vec3(-1.0,1.0,-1.0),
		Vec3(1.0,1.0,-1.0),
		Vec3(-1.0,-1.0,-1.0),
		Vec3(1.0,-1.0,-1.0)
	};
}

namespace face_top
{
	Vec3 vertices[] = {
		Vec3(1.0,1.0,1.0),
		Vec3(-1.0,1.0,1.0),
		Vec3(1.0,-1.0,1.0),
		Vec3(-1.0,-1.0,1.0)
	};
}

namespace face_left
{
	Vec3 vertices[] = {
		Vec3(-1.0,1.0,-1.0),
		Vec3(-1.0,1.0,1.0),
		Vec3(1.0,1.0,-1.0),
		Vec3(1.0,1.0,1.0)
	};
}

namespace face_right
{
	Vec3 vertices[] = {
		Vec3(1.0,-1.0,-1.0),

		Vec3(1.0,-1.0,1.0),
		Vec3(-1.0,-1.0,-1.0),
		Vec3(-1.0,-1.0,1.0)

	};
}

Skybox::Skybox(int scale) :  Geode()
	, _scale(scale)
	
{
	this->getOrCreateStateSet();
// 	texUnitStartLocation = 2;
// 	rectangleVertices = 4;

	//_drawables.resize(6);

	//buildGeometry( scale);
}


Skybox::~Skybox(void)
{
}

osg::Geometry *  Skybox::buildGeometry( osg::Vec3 vertices[], int scale)
{
	/*#define VERT_COUNT 4
	 #define FACES_COUNT 2
	 #define FACE_VERTEXES 3*/

	osg::Geometry *geometry = new osg::Geometry();
	geometry->setUseVertexBufferObjects(true);

	osg::Vec2Array* uvArray = new osg::Vec2Array();
	osg::Vec3Array* verticesArray = new osg::Vec3Array();

	for(int i=0;i<VERT_COUNT;i++){
		verticesArray->push_back((vertices[i])*_scale);
		uvArray->push_back(face_all::uvs[i]);
	}

	geometry->setVertexAttribArray(VERTEX_BIND_VALUE,verticesArray);
	geometry->setVertexAttribBinding(VERTEX_BIND_VALUE,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(UV_BIND_VALUE,uvArray);
	geometry->setVertexAttribBinding(UV_BIND_VALUE,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	geometry->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,FACES_COUNT*FACE_VERTEXES,face_all::indexArray));

	ComputeSkyBoxBoundingBoxCallback * sbbb = new Skybox::ComputeSkyBoxBoundingBoxCallback;

	geometry->setComputeBoundingBoxCallback(sbbb);

	return geometry;

	/*
	ref_ptr<Vec3Array> vertices = new Vec3Array();
	ref_ptr<Vec2Array> UVs = new Vec2Array();
	GLuint indexArray[] = {0,1,2,1,2,3};
	ref_ptr<DrawElementsUInt> indexes = new DrawElementsUInt(PrimitiveSet::TRIANGLES,6,indexArray);

	vertices->push_back(Vec3(1.0,1.0,1.0)*scale);
	vertices->push_back(Vec3(1.0,-1.0,1.0)*scale);
	vertices->push_back(Vec3(-1.0,-1.0,1.0)*scale);
	vertices->push_back(Vec3(-1.0,1.0,1.0)*scale);

	UVs->push_back(Vec2(1.0,1.0));
	UVs->push_back(Vec2(1.0,-1.0));
	UVs->push_back(Vec2(-1.0,-1.0));
	UVs->push_back(Vec2(-1.0,1.0));

	Matrix initMatrix;
	initMatrix.makeIdentity();
	Matrix matrixArray[] = {initMatrix,initMatrix,initMatrix,initMatrix,initMatrix,initMatrix};
	matrixArray[BACK].makeRotate(osg::PI,Vec3(0.0,0.0,1.0));
	matrixArray[RIGHT].makeRotate(osg::PI/2,Vec3(0.0,0.0,1.0));
	matrixArray[LEFT].makeRotate(osg::PI/2,Vec3(0.0,0.0,-1.0));
	matrixArray[TOP].makeRotate(osg::PI/2,Vec3(0.0,1.0,0.0));
	matrixArray[BOTTOM].makeRotate(osg::PI/2,Vec3(0.0,-1.0,0.0));

	for (unsigned i = 0; i <getRectCount(); i++)
	{
		ref_ptr<Geometry> geom = new Geometry();
		ref_ptr<Vec3Array> geomVertices = new Vec3Array();
		for (unsigned k = 0; k < rectangleVertices; k++)
			geomVertices->push_back(vertices->at(k)*matrixArray[i]);
		
		geom->setVertexAttribArray(0,geomVertices);
		geom->setVertexAttribBinding(0,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
		geom->setVertexAttribArray(1,UVs);
		geom->setVertexAttribBinding(1,osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
		geom->addPrimitiveSet(indexes);
		this->addDrawable(geom);
	}*/
}

bool osg::Skybox::buildSkybox( const char * front_texture_filename, 
	const char * back_texture_filename, 
	const char * top_texture_filename, 
	const char * bottom_texture_filename, 
	const char * right_texture_filename, 
	const char * left_texture_filename )
{
	Geometry * front = buildGeometry(face_front::vertices,1);
	Geometry * back = buildGeometry(face_back::vertices,1);
	Geometry * bottom = buildGeometry(face_bottom::vertices,1);
	Geometry * top = buildGeometry(face_top::vertices,1);
	Geometry * right = buildGeometry(face_right::vertices,1);
	Geometry * left = buildGeometry(face_left::vertices,1);

	if (!(buildTexture(front_texture_filename, front->getOrCreateStateSet()) &&
		buildTexture(back_texture_filename, back->getOrCreateStateSet()) &&
		buildTexture(top_texture_filename, top->getOrCreateStateSet()) &&
		buildTexture(bottom_texture_filename, bottom->getOrCreateStateSet()) &&
		buildTexture(right_texture_filename, right->getOrCreateStateSet()) &&
		buildTexture(left_texture_filename, left->getOrCreateStateSet())))
	{
		return false;
	}

	buildShader(this->getOrCreateStateSet());
	this->getOrCreateStateSet()->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );

	//this->getOrCreateStateSet()->setMode(osg::GL_DEPTH_CLAMP, osg::StateAttribute::ON);

	if(!(addDrawable(front) &&
		addDrawable(back) &&
		addDrawable(top) &&
		addDrawable(bottom) &&
		addDrawable(right) &&
		addDrawable(left)))
	{
		return false;
	}


	this->setCullingActive(false);

	return true;
}

bool osg::Skybox::buildTexture( const char * filename, osg::StateSet * targetSet )
{
	static const int default_texture_unit = 0;
	osg::Image * diffmap_image = osgDB::readImageFile(filename);
	if (diffmap_image==NULL)
		return false;

	osg::ref_ptr<osg::Texture2D> diffmap = new osg::Texture2D;

	diffmap->setDataVariance(osg::Object::DYNAMIC);
	diffmap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	diffmap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

	diffmap->setWrap(osg::Texture::WrapParameter::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	diffmap->setWrap(osg::Texture::WrapParameter::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

	diffmap->setImage(diffmap_image);

	targetSet->setTextureAttributeAndModes(default_texture_unit,diffmap,osg::StateAttribute::ON);

	targetSet->addUniform(new osg::Uniform("diffmap", default_texture_unit));

	return true;
}

std::string getVertexShaderCode()
{
	return std::string(	"#version 400"  "\n"
						"in vec3 myVertex;"  "\n"
						"in vec2 myTexCoord;"  "\n"
						"out vec2 vTexCoord;"  "\n"
						"uniform mat3 osg_NormalMatrix;"  "\n"
						"uniform mat4 osg_ProjectionMatrix;"  "\n"
						"uniform mat4 osg_ModelViewMatrix;"       "\n"
						"uniform mat4 osg_ViewMatrixInverse;" "\n"
						"uniform vec3 LightPosition;"  "\n"
						"void main(){"  "\n"
						"	vTexCoord = myTexCoord;"	  "\n"
						//"   vec4 tempPosition = osg_ProjectionMatrix*osg_ModelViewMatrix*vec4(myVertex+osg_ViewMatrixInverse[3].xyz,1.0);" "\n"
						//"   tempPosition.z= tempPosition.w - 0.001;"   "\n"

						"   vec4 tempPosition = osg_ProjectionMatrix*vec4(osg_NormalMatrix*myVertex,0.0001);" "\n"
						"	tempPosition.z = tempPosition.w - 0.00001;"  "\n"
						"	gl_Position = tempPosition.xyzw;"  "\n"
						"}"  "\n"
);
}

std::string getFragmentShaderCode()
{
	return std::string(	"#version 400"  "\n"
						"out vec4 FragColor;"  "\n"
						"in vec2 vTexCoord;"  "\n"
						"uniform sampler2D diffmap;"  "\n"
						"void main(){"  "\n"
						"	FragColor = vec4(texture2D (diffmap, vTexCoord).rgb, 1.0);"  "\n"
						"}"  "\n");
}

bool osg::Skybox::buildShader( StateSet * stateset )
{
	osg::Program * np = new osg::Program;
	np->addShader(new osg::Shader(osg::Shader::VERTEX,getVertexShaderCode()));
	np->addShader(new osg::Shader(osg::Shader::FRAGMENT,getFragmentShaderCode()));

	np->addBindAttribLocation("myVertex",VERTEX_BIND_VALUE);
	np->addBindAttribLocation("myTexCoord",UV_BIND_VALUE);

	np->addBindFragDataLocation("FragColor",0);

	stateset->setAttribute(np);

	return true;
}
