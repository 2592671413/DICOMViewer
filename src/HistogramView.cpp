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

#include "HistogramView.h"
#include "CarnaContextClient.h"
#include <Carna/base/model/Scene.h>
#include <Carna/base/model/Volume.h>
#include <Carna/base/CarnaException.h>
#include <QPainter>
#include <QProgressDialog>
#include <QFontMetrics>
#include <QApplication>
#include <cmath>
#include <algorithm>
#include <QTimer>



// ----------------------------------------------------------------------------------
// Composition
// ----------------------------------------------------------------------------------

template< unsigned int N >
static HistogramView::Scale Composition( const HistogramView::Scale& f )
{
    return [&]( double x )->double
    {
        double y = f( x );
        for( unsigned int i = 1; i < N; ++i )
        {
            y = f( y );
        }
        return y;
    };
}



// ----------------------------------------------------------------------------------
// HistogramView
// ----------------------------------------------------------------------------------

const HistogramView::Scale HistogramView::linear = []()->HistogramView::Scale
{
    const static HistogramView::Scale linearFunction = []( double relativeProbability )->double{ return relativeProbability; };

    return linearFunction;
}();

const HistogramView::Scale HistogramView::logarithmic = []()->HistogramView::Scale
{
    const static HistogramView::Scale log = []( double relativeProbability )->double{ return std::log( 1 + relativeProbability ); };

    const unsigned int multiplicity = 1;

    return HistogramView::Scale( Composition< multiplicity >( log ) );
}();


const static int offset_x = 50;

const static int offset_y = 40;


HistogramView::HistogramView( Record::Server& server )
    : server( server )
    , histogram( 4096 )
    , scale( logarithmic )
    , huv0( -1024 )
    , huv1(  3071 )
    , minClassSize( 1 )
    , minBarWidth( 2 )
    , zoom( 1 )
    , initialized( false )
{
    QTimer::singleShot( 0, this, SLOT( init() ) );
}


void HistogramView::init()
{
    CARNA_ASSERT( !initialized );

    const Carna::base::model::Volume& volume = CarnaContextClient( server ).model().volume();

    QProgressDialog progress( "Building histogram...", "Abort", 0, volume.size.x - 1, this );
    progress.setWindowModality( Qt::WindowModal );

    for( unsigned int x = 0; x < volume.size.x; ++x )
    {
        QApplication::processEvents();
        if( progress.wasCanceled() )
        {
            std::for_each( histogram.begin(), histogram.end(), []( unsigned long& x ){ x = 0; } );
            break;
        }

        for( unsigned int y = 0; y < volume.size.y; ++y )
        for( unsigned int z = 0; z < volume.size.z; ++z )
        {
            const unsigned short shifted_huv = volume( x, y, z ) + 1024;
            ++histogram[ shifted_huv ];
        }

        progress.setValue( x );
    }

    progress.close();

    sum = 0;
    for( unsigned int i = 0; i < histogram.size(); ++i )
    {
        sum += histogram[ i ];
    }
    if( sum == 0 )
    {
        return;
    }

 // initialize

    this->initialized = true;
    this->update();
}


void HistogramView::setAutoZoom()
{
    this->compute( true );

 // compute zoom

    unsigned long max_absolute_probability = 0;
    for( int min_huv = huv0; min_huv <= huv1 - signed( classSize ); min_huv += classSize )
    {
        unsigned long absolute_probability = 0;
        for( int huv = min_huv; huv < min_huv + signed( classSize ); ++huv )
        {
            absolute_probability += histogram[ huv + 1024 ];
        }
        max_absolute_probability = std::max( max_absolute_probability, absolute_probability );
    }

    const double max_relative_probability = ( scale( max_absolute_probability ) / scale( getSum() ) );
    const double max_class_density = max_relative_probability / classSize;
    zoom = std::max( 1., 1 / max_class_density );

 // emit notifier signals

    emit zoomChanged( zoom );

 // post repaint event

    this->update();
}


void HistogramView::setAutoHuvRange()
{
    const static double threshold = 1e-4;

 // adjust huv0

    for( huv0 = -1024; huv0 < 3071; ++huv0 )
    {
        unsigned long absolute_probability = histogram[ huv0 + 1024 ];
        const double relative_probability = absolute_probability / static_cast< double >( getSum() );
        if( relative_probability >= threshold )
        {
            break;
        }
    }

 // adjust huv1

    for( huv1 = 3071; huv1 > huv0; --huv1 )
    {
        unsigned long absolute_probability = histogram[ huv1 + 1024 ];
        const double relative_probability = absolute_probability / static_cast< double >( getSum() );
        if( relative_probability >= threshold )
        {
            break;
        }
    }

 // emit notifier signals

    emit minHuvChanged( huv0 );
    emit maxHuvChanged( huv1 );

 // re-compute parameters and post repaint event

    this->compute();
    this->update();
}


void HistogramView::setScale( const Scale& scale )
{
    this->scale = scale;

    this->update();
}


void HistogramView::setMinClassSize( int minClassSize )
{
    CARNA_ASSERT( minClassSize >= 0 );

    this->minClassSize = unsigned( minClassSize );

    this->compute();
    this->update();
}


void HistogramView::setMinBarWidth( int minBarWidth )
{
    CARNA_ASSERT( minBarWidth >= 0 );

    this->minBarWidth = unsigned( minBarWidth );

    this->compute();
    this->update();
}


void HistogramView::setMinHuv( int huv0 )
{
    CARNA_ASSERT( -1024 <= huv0 && huv0 <= 3071 );

    if( huv0 != this->huv0 )
    {
        this->huv0 = huv0;

        emit minHuvChanged( huv0 );

        this->compute();
        this->update();
    }
}


void HistogramView::setMaxHuv( int huv1 )
{
    CARNA_ASSERT( -1024 <= huv1 && huv1 <= 3071 );

    if( huv1 != this->huv1 )
    {
        this->huv1 = huv1;

        emit maxHuvChanged( huv1 );

        this->compute();
        this->update();
    }
}


void HistogramView::setZoom( double zoom )
{
    CARNA_ASSERT( zoom > 0 );

    if( std::abs( zoom - this->zoom ) > 1e-6 )
    {
        this->zoom = zoom;

        emit zoomChanged( zoom );

        this->update();
    }
}


void HistogramView::compute( bool forceComputation )
{
    if( !forceComputation && ( !initialized || huv1 <= huv0 ) )
    {
        return;
    }

    const unsigned int screen_width = this->width() - 1 - offset_x;
    const unsigned int huv_range = huv1 - huv0;

    valuesPerPixel = static_cast< double >( huv_range ) / screen_width;

    classSize = std::max( valuesPerPixel * minBarWidth, static_cast< double >( std::max(
                          minClassSize,
                          static_cast< unsigned int >( std::ceil( valuesPerPixel ) ) ) ) );

    barWidth = static_cast< unsigned int >( std::ceil( classSize / valuesPerPixel ) );

    emit parametersChanged();
}


void HistogramView::resizeEvent( QResizeEvent* ev )
{
    this->compute();

    QWidget::resizeEvent( ev );
}


void HistogramView::paintEvent( QPaintEvent* ev )
{
    if( !initialized || getSum() == 0 || huv1 <= huv0 )
    {
        QWidget::paintEvent( ev );
        return;
    }

    const unsigned int screen_width  = this->width () - 1 - offset_x;
    const unsigned int screen_height = this->height() - 1 - offset_y;

 // prepare

    QPainter p( this );
    p.translate( offset_x, screen_height );
    p.scale( 1, -1 );
    p.setPen( Qt::black );
    p.fillRect( 0, 0, screen_width + 1, screen_height + 1, Qt::white );
    p.drawRect( 0, 0, screen_width + 1, screen_height + 1 );

 // paint histogram

    for( unsigned int x = 0; x < screen_width; x += barWidth )
    {
        const int huv_class_min = huv0 + static_cast< int >( x * valuesPerPixel + 0.5 );
        const int huv_class_max = huv_class_min + classSize - 1;

     // compute absolute probability
        
        unsigned long absolute_probability = 0;
        for( int huv = huv_class_min; huv <= huv_class_max; ++huv )
        {
            const unsigned short shifted_huv = huv + 1024;
            if( shifted_huv >= histogram.size() )
            {
                break;
            }
            absolute_probability += histogram[ shifted_huv ];
        }

     // compute relative probability

        const double relative_probability = ( scale( absolute_probability ) / scale( getSum() ) );

     // compute class density

        const double class_density = relative_probability / classSize;

     // draw

        const int x0 = signed( x );
        const int x1 = x0 + barWidth;

        const int y = static_cast< int >( class_density * screen_height * zoom + 0.5 );

        p.drawLine( QPoint( x0, 0 ), QPoint( x0, y ) );
        p.drawLine( QPoint( x0, y ), QPoint( x1, y ) );
        p.drawLine( QPoint( x1, y ), QPoint( x1, 0 ) );

     // for the case x0 == x1 we need:

        p.drawPoint( QPoint( x0, y ) );
    }

 // prepare scaling painting

    QFont font = p.font();
    font.setPixelSize( 10 );
    p.setFont( font );
    const QFontMetrics fm = p.fontMetrics();

    const unsigned int tickInterval = fm.height() * 3 / 2;

 // paint probability scaling

    const QRect verticalCaptionRect( 0, -fm.height() / 2, offset_x - 5, fm.height() );
    for( unsigned int y = 0; y < screen_height; y += tickInterval )
    {
        const double class_density = y / ( zoom * screen_height );
        const QString caption = QString::number( class_density * 100, 'f', 2 ) + " %";

        p.drawLine( -2, y, 0, y );

        p.save();

        p.translate( -offset_x, y );
        p.scale( 1, -1 );

        p.drawText( verticalCaptionRect, Qt::AlignRight | Qt::AlignVCenter, caption );

        p.restore();
    }

 // paint HUV scaling

    p.save();

    p.rotate( 90 );
    p.scale( 1, -1 );

    const QRect horizontalCaptionRect( 0, -fm.height() / 2, offset_y - 5, fm.height() );
    for( unsigned int x = 0; x < screen_width; x += tickInterval )
    {
        const int huv = this->huv0 + x * this->valuesPerPixel;
        const QString caption = QString::number( huv );

        p.drawLine( -2, x, 0, x );

        p.save();

        p.translate( -offset_y, x );

        p.drawText( horizontalCaptionRect, Qt::AlignRight | Qt::AlignVCenter, caption );

        p.restore();
    }

    p.restore();
}
