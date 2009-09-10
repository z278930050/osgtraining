#include "DynamicGroupUpdateCallback.h"

#include "dataPatch.h"

#include "GeometryPatch.h"
#include "GeometryTexturePatch.h"

#include <osgDB/ReadFile>

#include <iostream>

DynamicGroupUpdateCallback::DynamicGroupUpdateCallback()
{
	// ��������� �����������
	m_ImageIndex = osgDB::readImageFile( "index.bmp" );
}

DynamicGroupUpdateCallback::~DynamicGroupUpdateCallback()
{

}

void DynamicGroupUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
	//������������ ������ ������ � ������� ������
	m_VisiblePatchArray.Update();

	osg::ref_ptr< osg::Group > group = dynamic_cast< osg::Group* >( node );

	if ( group )
	{
		//������� ������ �� ������ ������� ������
		const std::vector< dataPatch > &data_vis = m_VisiblePatchArray.GetVisibleArray();

		if ( !data_vis.empty() )
		{
			std::cout << data_vis.size() << "-" << FindMax() << " ";

			//�������� ���� �����
			group->removeChildren( 0 , group->getNumChildren() );

			//��������� ��� ������� ����
			for( int i = 0 ; i < data_vis.size() ; ++i )
			{
				//������������ ���������
				//GeometryPatch patch( data_vis[ i ].m_iX , 
				//	data_vis[ i ].m_iY , 65 , 
				//	data_vis[ i ].m_iSize );

				GeometryTexturePatch patch( data_vis[ i ].m_iX , 
					data_vis[ i ].m_iY , 65 , 
					data_vis[ i ].m_iSize , m_ImageIndex.get() );

				osg::ref_ptr< osg::Geode > geode = new osg::Geode;
				geode->addDrawable( patch.GetGeometry().get() );

				//�������� ��������� � �����
				group->addChild( geode.get() );
			}
		}
	}
}

int DynamicGroupUpdateCallback::FindMax()
{
	//����� ������������� ������� �����

	//������� ������ �� ������ ������� ������
	const std::vector< dataPatch > &data_vis = m_VisiblePatchArray.GetVisibleArray();

	int iMax = data_vis[ 0 ].m_iSize;

	for ( int i = 1 ; i < data_vis.size() ; ++i )
		if ( data_vis[ i ].m_iSize > iMax)
			iMax = data_vis[ i ].m_iSize;

	if ( iMax > 32768 )
	{
		int a = 1;
	}

	return iMax;
}