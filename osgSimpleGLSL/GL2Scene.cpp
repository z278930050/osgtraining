#include "GL2Scene.h"

#include "GL2SceneCallback.h"
#include "GL2SceneUniformCallback.h"

#include <osg/Geode>
#include <osg/Node>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Texture2D>
#include <osg/Image>

static osg::ref_ptr<osg::Group> rootNode;

GL2Scene::GL2Scene()
{
	// the root of our scenegraph.
	_rootNode = new osg::Group;

	//�������� ���� � ��������������� ����������
	_rootNode->addChild( buildHelp().get() );

	//�������� ���� � ��������
	_rootNode->addChild( buildScene().get() );

	if ( rootNode.valid() )
	{
		//�������� ������
		AddShader();

		//�������� ��������
		AddTexture();
	}
}

GL2Scene::~GL2Scene()
{
}

osg::ref_ptr<osg::Group> GL2Scene::buildScene()
{
	// the root of our scenegraph.
	rootNode = new osg::Group;

	// Create an object to store geometry in.
	osg::ref_ptr< osg::Geometry > geom = new osg::Geometry;

	// Create an array of four vertices.
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	geom->setVertexArray( v.get() );

	// Create a Vec2Array of texture coordinates for texture unit 0
	// and attach it to the geom.
	osg::ref_ptr<osg::Vec2Array> tc = new osg::Vec2Array;
	geom->setTexCoordArray( 0, tc.get() );

	for ( int i = 0; i < 16 ; ++i )
	{
		v->push_back( osg::Vec3( i, 0.f, 0 ) );
		v->push_back( osg::Vec3( i, 0.f, 0 ) );

		tc->push_back( osg::Vec2( 0 + i / 16.0f , 0 ) );
		tc->push_back( osg::Vec2( 0 + i / 16.0f , 1.0 ) );
	}

	geom->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, 32 ) );

	// Add the Geometry (Drawable) to a Geode and
	// return the Geode.
	osg::ref_ptr< osg::Geode > geode = new osg::Geode;
	geode->addDrawable( geom.get() );

	rootNode->addChild( geode.get() );

	return rootNode.get();
}

osg::ref_ptr<osg::Group> GL2Scene::buildHelp()
{
	//���������� ��������������� ���������
	osg::ref_ptr<osg::Group> rootHelp = new osg::Group;

	// Create an object to store geometry in.
	osg::ref_ptr< osg::Geometry > geom = new osg::Geometry;

	// Create an array of four vertices.
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	geom->setVertexArray( v.get() );

	v->push_back( osg::Vec3( 0.0, 0.f, 0.0 ) );
	v->push_back( osg::Vec3( 7.0, 0.f, 0.0 ) );
	v->push_back( osg::Vec3( 7.0, 0.f, 128.0 / 255.0 * 10.0 ) );
	v->push_back( osg::Vec3( 0.0, 0.f, 128.0 / 255.0 * 10.0 ) );


	// Create an array of four vertices.
	osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;
	geom->setColorArray( c.get() );
	geom->setColorBinding( osg::Geometry::BIND_OVERALL );
	c->push_back( osg::Vec4( 1.0 , 1.0 , 1.0 , 1.0 ) );

	geom->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 ) );

	// Add the Geometry (Drawable) to a Geode and
	// return the Geode.
	osg::ref_ptr< osg::Geode > geode = new osg::Geode;
	geode->addDrawable( geom.get() );

	rootHelp->addChild( geode.get() );

	return rootHelp.get();
}

void GL2Scene::AddShader()
{
	//�������� ������ � �����
	osg::StateSet* ss = rootNode->getOrCreateStateSet();

	//������� ��������� ���������
	osg::Program* program = new osg::Program;
	program->setName( "microshader" );

	osg::Shader *VertObj = new osg::Shader( osg::Shader::VERTEX );
	osg::Shader *FragObj = new osg::Shader( osg::Shader::FRAGMENT );
	program->addShader( VertObj );
	program->addShader( FragObj );

	LoadShaderSource( VertObj , "glsl/C3E1v.vert" );
	LoadShaderSource( FragObj , "glsl/C2E2f.frag" );
	
	ss->setAttributeAndModes( program, osg::StateAttribute::ON );

	//�������� ��������� ��� �������� � ������
	osg::Uniform *_color = new osg::Uniform("_ZZ3SconstantColor", osg::Vec3( 1.0f, 0.0f, 1.0f));

	//������� callback'a ��� ������������� ��������� ���������
	_color->setUpdateCallback( new GL2SceneUniformCallback );

	//���������� � ��������� �����
	ss->addUniform( _color );

	//���������� uniform'�� ��� ������ � ����������� ��������
	ss->addUniform( new osg::Uniform( "u_texture0" , 0 ) );
}  

void GL2Scene::LoadShaderSource( osg::Shader* shader, const std::string& fileName )
{
	// load source from a file.
	std::string fqFileName = osgDB::findDataFile(fileName);
	if( fqFileName.length() != 0 )
	{
		shader->loadShaderSourceFromFile( fqFileName.c_str() );
	}
	else
	{
		osg::notify(osg::WARN) << "File \"" << fileName << "\" not found." << std::endl;
	}
}

void GL2Scene::AddTexture()
{
	//�������� ��������
	osg::StateSet* state = rootNode->getOrCreateStateSet();

	// Load the texture image
	osg::ref_ptr<osg::Image> image0 =
		osgDB::readImageFile( "Images/4x4.png" );

	// Attach the image in a Texture2D object
	osg::ref_ptr<osg::Texture2D> tex0 = new osg::Texture2D;
	tex0->setImage( image0.get() );

	tex0->setFilter(osg::Texture::MIN_FILTER,osg::Texture::NEAREST);
	tex0->setFilter(osg::Texture::MAG_FILTER,osg::Texture::NEAREST);
	tex0->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP); 
	tex0->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP); 


	// Attach the 2D texture attribute and enable GL_TEXTURE_2D,
	// both on texture unit 0.
	state->setTextureAttributeAndModes( 0, tex0.get() , osg::StateAttribute::ON );
}