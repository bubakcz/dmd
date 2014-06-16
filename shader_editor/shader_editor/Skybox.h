#pragma once

#include <osg/Geode>
#include <osg/Drawable>


namespace osg
{
	const int texUnitStartLocation = 2;
	const int rectangleVertices = 4;

	class Skybox :
		public osg::Geode
	{
	public:
		Skybox(int scale);
		~Skybox(void);

		enum
		{
			TOP=0,
			FRONT,
			RIGHT,
			LEFT,
			BOTTOM,
			BACK
		};

		bool buildSkybox(	const char * front_texture_filename,
							const char * back_texture_filename,
							const char * top_texture_filename,
							const char * bottom_texture_filename,
							const char * right_texture_filename,
							const char * left_texture_filename);

		unsigned getRectCount() {return 6;};

		struct ComputeSkyBoxBoundingBoxCallback : public osg::Drawable::ComputeBoundingBoxCallback
		{
			META_Object(osg,ComputeBoundingBoxCallback);

			virtual BoundingBox computeBound(const osg::Drawable&) const  { return BoundingBox(); }
		};

	protected:

		int _scale;

		osg::Geometry * buildGeometry(osg::Vec3 vertices[], int scale);

		bool buildTexture(const char * filename, osg::StateSet * targetSet);

		bool buildShader(StateSet * stateset);


		/*virtual bool 	removeDrawable (Drawable *drawable){return Geode::removeDrawable(drawable);};
		virtual bool 	addDrawable (Drawable *drawable){return Geode::addDrawable(drawable);};
		virtual bool 	removeDrawables (unsigned int i, unsigned int numDrawablesToRemove=1){return Geode::removeDrawables(i,numDrawablesToRemove);};
		virtual bool 	replaceDrawable (Drawable *origDraw, Drawable *newDraw){return Geode::replaceDrawable(origDraw,newDraw);};
		virtual bool 	setDrawable (unsigned int i, Drawable *drawable){return Geode::setDrawable(i,drawable);};*/
		/*
		Add a Drawable to the Geode.
		
		Remove a Drawable from the Geode.
		
		Remove Drawable(s) from the specified position in Geode's drawable list.
		
		Replace specified Drawable with another Drawable.
		
		Set Drawable at position i. */
	};
}