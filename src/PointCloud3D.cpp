/*
 *  Copyright (C) 2010 - 2013 Leonid Kostrykin
 *
 *  Chair of Medical Engineering (mediTEC)
 *  RWTH Aachen University
 *  Pauwelsstr. 20
 *  52074 Aachen
 *  Germany
 *
 */

#include "glew.h"
#include "PointCloud3D.h"
#include "PointCloud.h"
#include "CarnaContextClient.h"
#include "PointCloudsClient.h"
#include "RegistrationClient.h"
#include "NotificationsClient.h"
#include <Carna/ShaderBundle.h>
#include <Carna/ShaderProgram.h>
#include <Carna/Object3DEvent.h>
#include <Carna/Renderer.h>
#include <climits>
#include <algorithm>
#include <QApplication>
#include <QProgressDialog>



// ----------------------------------------------------------------------------------
// Types & Globals
// ----------------------------------------------------------------------------------

const static std::string shaderId = "point-cloud";



// ----------------------------------------------------------------------------------
// PointCloud3D
// ----------------------------------------------------------------------------------

PointCloud3D::PointCloud3D( Record::Server& server, const PointCloud& cloud, QWidget* modalityParent )
    : Carna::Object3D( CarnaContextClient( server ).model(), cloud.getName() )
    , server( server )
    , points()
    , ready( false )
    , minX( +std::numeric_limits< double >::max() )
    , minY( +std::numeric_limits< double >::max() )
    , minZ( +std::numeric_limits< double >::max() )
    , maxX( -std::numeric_limits< double >::max() )
    , maxY( -std::numeric_limits< double >::max() )
    , maxZ( -std::numeric_limits< double >::max() )
    , pointSize( cloud.getList().size() > 20 ? 2.5 : 6.5 )
    , pointColor( 255, 255, 255 )
    , modulateBrightnessByDistance( cloud.getList().size() > 20 )
    , drawBoundingBox( true )
    , applyRegistration( cloud.source == PointCloud::trackingSide )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    QProgressDialog progress( "Processing points...", "", 0, cloud.getList().size() - 1, modalityParent );
    progress.setWindowModality( Qt::WindowModal );
    progress.setCancelButton( nullptr );

    points.resize( cloud.getList().size() );

    for( PointCloud::PointList::const_iterator it  = cloud.getList().begin();
                                               it != cloud.getList().end();
                                             ++it )
    {
        const Carna::Tools::Vector& p = *it;

        std::unique_ptr< Carna::Position > pos;

        switch( cloud.getFormat() )
        {

        case PointCloud::millimeters:
            pos.reset( new Carna::Position( Carna::Position::fromMillimeters( model, p.x(), p.y(), p.z() ) ) );
            break;

        case PointCloud::volumeUnits:
            pos.reset( new Carna::Position( Carna::Position::fromVolumeUnits( model, p.x(), p.y(), p.z() ) ) );
            break;

        default:
            throw std::logic_error( "unknown PointCloud::Unit format" );

        }

        const Carna::Tools::Vector& modelSpace = pos->toVolumeUnits();

        points[ it - cloud.getList().begin() ] = modelSpace;

        minX = std::min( minX, modelSpace.x() );
        minY = std::min( minY, modelSpace.y() );
        minZ = std::min( minZ, modelSpace.z() );

        maxX = std::max( maxX, modelSpace.x() );
        maxY = std::max( maxY, modelSpace.y() );
        maxZ = std::max( maxZ, modelSpace.z() );

        progress.setValue( it - cloud.getList().begin() );
    }

    QApplication::restoreOverrideCursor();

 // ----------------------------------------------------------------------------------
    
    if( cloud.source == PointCloud::unknown )
    {
        const static unsigned int max_sample_count = 100;
        const static unsigned int step = std::max( points.size() / max_sample_count, unsigned( 1 ) );

        unsigned int points_within_data_space = 0;
        unsigned int sample_count = 0;
        for( unsigned int i = 0; i < points.size(); i += step )
        {
            const Carna::Tools::Vector& position = points[ i ];

            ++sample_count;

            if( position.x() >= 0. && position.x() <= 1.
             && position.y() >= 0. && position.y() <= 1.
             && position.z() >= 0. && position.z() <= 1. )
            {
                ++points_within_data_space;
            }
        }

        const double points_within_data_space_ratio = static_cast< double >( points_within_data_space ) / sample_count;

        if( points_within_data_space_ratio < 0.9 )
        {
            this->applyRegistration = true;
        }
    }

 // ----------------------------------------------------------------------------------

    NotificationsClient( server ).connectServiceProvided( this, SLOT( serviceProvided( const std::string& ) ) );
    if( server.hasService( Registration::serviceID ) )
    {
        serviceProvided( Registration::serviceID );
    }

 // ----------------------------------------------------------------------------------

    ready = true;
}


PointCloud3D::~PointCloud3D()
{
    ready = false;

    QApplication::setOverrideCursor( Qt::WaitCursor );

    points.clear();

    QApplication::restoreOverrideCursor();
}


void PointCloud3D::draw( const Carna::Renderer& renderer, const Carna::Tools::Vector3ui& color, bool simplified ) const
{
    if( !ready )
    {
        return;
    }

    glPushMatrix();

    const Carna::Tools::Vector& position = this->position().toVolumeUnits();
    glTranslatef( position.x(), position.y(), position.z() );

    if( applyRegistration )
    {
        try
        {
            using Carna::Tools::glMultMatrix;
            using Carna::Tools::glTranslate;
            using Carna::Tools::Transformation;
            using Carna::Tools::Vector;
            using Carna::Position;

            const Transformation trafo = RegistrationClient( server )->getTransformation();
            const Carna::Model& model = CarnaContextClient( server ).model();

            glMultMatrix( Position::toVolumeUnitsTransformation( model ) );
            glMultMatrix( trafo );
            glMultMatrix( Position::toMillimetersTransformation( model ) );
        }
        catch( const Record::Server::Exception& )
        {
        }
    }

    glColor3ub( color.x, color.y, color.z );

    auto emitVertices = [&]()
    {
        glPointSize( static_cast< float >( pointSize ) );
        glBegin( GL_POINTS );

        for( auto it = points.begin(); it != points.end(); ++it )
        {
            const Carna::Tools::Vector& point = *it;

            glVertex3f( point.x(), point.y(), point.z() );
        }

        glEnd();
    };

    if( simplified || !modulateBrightnessByDistance )
    {
        emitVertices();
    }
    else
    {
        glPushAttrib( GL_ALL_ATTRIB_BITS );

        Carna::ShaderProgram::Binding shader( renderer.shader( shaderId ) );

        glDepthMask( GL_TRUE );
        glEnable( GL_DEPTH_TEST );
        glDepthFunc( GL_LEQUAL );

        emitVertices();

        glPopAttrib();
    }

    if( drawBoundingBox )
    {

        glLineWidth( 0.5f );
        glBegin( GL_LINE_LOOP );

        glVertex3f( minX, minY, minZ );
        glVertex3f( maxX, minY, minZ );
        glVertex3f( maxX, maxY, minZ );
        glVertex3f( minX, maxY, minZ );

        glEnd();
        glBegin( GL_LINE_LOOP );

        glVertex3f( minX, minY, maxZ );
        glVertex3f( maxX, minY, maxZ );
        glVertex3f( maxX, maxY, maxZ );
        glVertex3f( minX, maxY, maxZ );

        glEnd();
        glBegin( GL_LINES );

        glVertex3f( minX, minY, minZ );
        glVertex3f( minX, minY, maxZ );

        glVertex3f( maxX, minY, minZ );
        glVertex3f( maxX, minY, maxZ );

        glVertex3f( maxX, maxY, minZ );
        glVertex3f( maxX, maxY, maxZ );

        glVertex3f( minX, maxY, minZ );
        glVertex3f( minX, maxY, maxZ );

        glEnd();

    }

    glPopMatrix();
}


void PointCloud3D::paint( const Carna::Renderer& renderer ) const
{
    draw( renderer, Carna::Tools::Vector3ui
                                    ( pointColor.red()
                                    , pointColor.green()
                                    , pointColor.blue()
                    ), false );
}


void PointCloud3D::paintFalseColor( const Carna::Renderer& renderer, const Carna::Tools::Vector3ui& color ) const
{
    draw( renderer, color, true );
}


double PointCloud3D::getPointSize() const
{
    return pointSize;
}


const QColor& PointCloud3D::getPointColor() const
{
    return pointColor;
}


void PointCloud3D::setPointSize( double size )
{
    if( std::abs( this->pointSize - size ) > 1e-4 )
    {
        this->pointSize = size;

        invalidateObjects3D( Carna::Object3DEvent( Carna::Object3DEvent::shape ) );

        emit pointSizeChanged( this->pointSize );
    }
}


void PointCloud3D::setPointColor( const QColor& color )
{
    if( this->pointColor != color )
    {
        this->pointColor = color;

        invalidateObjects3D( Carna::Object3DEvent( Carna::Object3DEvent::shape ) );

        emit pointColorChanged( this->pointColor );
    }
}


bool PointCloud3D::modulatesBrightnessByDistance() const
{
    return modulateBrightnessByDistance;
}


void PointCloud3D::setBrightnessByDistanceModulation( bool modulateBrightnessByDistance )
{
    if( this->modulateBrightnessByDistance != modulateBrightnessByDistance )
    {
        this->modulateBrightnessByDistance = modulateBrightnessByDistance;

        invalidateObjects3D( Carna::Object3DEvent( Carna::Object3DEvent::shape ) );

        emit brightnessByDistanceModulationChanged( this->modulateBrightnessByDistance );
    }
}


bool PointCloud3D::drawsBoundingBox() const
{
    return drawBoundingBox;
}


void PointCloud3D::setBoundingBoxDrawing( bool drawBoundingBox )
{
    if( this->drawBoundingBox != drawBoundingBox )
    {
        this->drawBoundingBox = drawBoundingBox;

        invalidateObjects3D( Carna::Object3DEvent( Carna::Object3DEvent::shape ) );

        emit boundingBoxDrawingChanged( this->drawBoundingBox );
    }
}


bool PointCloud3D::appliesRegistration() const
{
    return applyRegistration;
}


void PointCloud3D::setRegistrationApplication( bool applyRegistration )
{
    if( this->applyRegistration != applyRegistration )
    {
        this->applyRegistration = applyRegistration;

        if( server.hasService( Registration::serviceID ) )
        {
            invalidateObjects3D( Carna::Object3DEvent( Carna::Object3DEvent::position ) );
        }

        emit registrationApplicationChanged( this->applyRegistration );
    }
}


void PointCloud3D::serviceProvided( const std::string& serviceID )
{
    if( serviceID == Registration::serviceID && this->applyRegistration )
    {
        RegistrationClient( server ).connectTransformationChanged( this, SLOT( transformationChanged() ) );
        transformationChanged();
    }
}


void PointCloud3D::transformationChanged()
{
    if( this->applyRegistration )
    {
        invalidateObjects3D( Carna::Object3DEvent( Carna::Object3DEvent::position ) );
    }
}
