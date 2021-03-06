#include "TextureShaderTile.h"

#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/ReadFile>
#include <osg/Image>
#include <osgDB/FileUtils>
#include <osg/Texture2D>
#include <osg/AlphaFunc>
#include <osg/CullFace>

TextureShaderTile::TextureShaderTile()
{
	m_rootNode = new osg::Group;

	//���������������� ���������
	InitGeom();

	//�������� �������� � �������
	AddTextureTile();

	//�������� ������
	AddShader();
}

TextureShaderTile::~TextureShaderTile()
{

}

void TextureShaderTile::InitGeom()
{
	//���������������� ���������

	//Create an object to store geometry in.
	osg::ref_ptr< osg::Geometry > geom = new osg::Geometry;

	// Create an array of vertices.
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;

	// Create a Vec2Array of texture coordinates for texture unit 0
	// and attach it to the geom.
	osg::ref_ptr<osg::Vec2Array> tc0 = new osg::Vec2Array;

	// Load the texture image
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile( "index.bmp" );
	unsigned char *dataR = image->data();
	
	for ( int z = 0 ; z < 512 ; ++z )
		for ( int x = 0 ; x < 512 ; ++x )
		{
			v->push_back( osg::Vec3( x * 2048.0 , 0 ,  z * 2048.0) );
			v->push_back( osg::Vec3( ( x + 1.0 ) * 2048.0 , 0 ,  z * 2048.0) );
			v->push_back( osg::Vec3( ( x + 1.0 ) * 2048.0 , 0 ,  ( z + 1.0 ) * 2048.0 ) );
			v->push_back( osg::Vec3( x * 2048.0 , 0 , ( z + 1.0 ) * 2048.0 ) );

			int r = dataR[ z * 512 * 3 + x * 3 ];
			int g = dataR[ z * 512 * 3 + x * 3 + 1];

			tc0->push_back( osg::Vec2( r / 16.0 + 1 / 8192.0 , g / 16.0 + 1 / 8192.0) );
			tc0->push_back( osg::Vec2( ( r + 1.0 ) / 16.0 - 1 / 8192.0, g / 16.0 + 1 / 8192.0 ) );
			tc0->push_back( osg::Vec2( ( r + 1.0 ) / 16.0 - 1 / 8192.0, ( g + 1.0 ) / 16.0 - 1 / 8192.0 ) );
			tc0->push_back( osg::Vec2( r / 16.0 + 1 / 8192.0 , ( g + 1.0 ) / 16.0 - 1 / 8192.0 ) );
		}

	geom->setVertexArray( v.get() );
	geom->setTexCoordArray( 0, tc0.get() );

	geom->addPrimitiveSet( new osg::DrawArrays(
		osg::PrimitiveSet::QUADS , 0 , v->size() ) );

	//�������� ���� ��������� ������
	osg::ref_ptr< osg::Geode > geode = new osg::Geode;

	geode->addDrawable( geom.get() );

	m_rootNode->addChild( geode.get() );
}

void TextureShaderTile::AddTextureTile()
{
	//�������� �������� � �������
	osg::StateSet* state = m_rootNode->getOrCreateStateSet();

	// Load the texture image
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile( "big.bmp" );

	// Attach the image in a Texture2D object
	osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;
	tex->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR_MIPMAP_LINEAR );
	tex->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR_MIPMAP_LINEAR );
	tex->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP); 
	tex->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP); 

	tex->setImage( image.get() );

	//���������� ������ �� image
	tex->setUnRefImageDataAfterApply( true );

	// Attach the 2D texture attribute and enable GL_TEXTURE_2D,
	// both on texture unit 1.
	state->setTextureAttributeAndModes( 1 , tex.get() );
}

void TextureShaderTile::AddShader()
{
	//�������� ������ � �����

	//������������ ����� � ��������
	osg::StateSet* ss = m_rootNode->getOrCreateStateSet();

	//������� ��������� ���������
	osg::Program* program = new osg::Program;
	program->setName( "quad_shader" );

	osg::Shader *VertObj = new osg::Shader( osg::Shader::VERTEX );
	osg::Shader *FragObj = new osg::Shader( osg::Shader::FRAGMENT );
	program->addShader( VertObj );
	program->addShader( FragObj );

	LoadShaderSource( VertObj , "glsl/quad.vert" );
	LoadShaderSource( FragObj , "glsl/quad.frag" );

	ss->setAttributeAndModes( program, osg::StateAttribute::ON );

	//���������� uniform'�� ��� ������ � ����������� ��������
	ss->addUniform( new osg::Uniform( "u_texture0" , 0 ) );
	ss->addUniform( new osg::Uniform( "u_texture1" , 1 ) );
	ss->addUniform( new osg::Uniform( "u_texture2" , 2 ) );

	//����������� ��������
	m_OffsetX = new osg::Uniform( "offsetX" , 0.0f );
	ss->addUniform( m_OffsetX.get() );
	m_OffsetY = new osg::Uniform( "offsetY" , 0.0f );
	ss->addUniform( m_OffsetY.get() );

	m_ScaleX = new osg::Uniform( "scaleX" , 1.0f );
	ss->addUniform( m_ScaleX.get() );
	m_ScaleY = new osg::Uniform( "scaleY" , 1.0f );
	ss->addUniform( m_ScaleY.get() );

	m_OffsetScaleX = new osg::Uniform( "offsetScaleX" , 0.0f );
	ss->addUniform( m_OffsetScaleX.get() );
	m_OffsetScaleY = new osg::Uniform( "offsetScaleY" , 0.0f );
	ss->addUniform( m_OffsetScaleY.get() );
}

void TextureShaderTile::LoadShaderSource( osg::Shader* shader, const std::string& fileName )
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
