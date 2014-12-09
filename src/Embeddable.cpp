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

#include "Embeddable.h"
#include "EmbedManager.h"
#include "EmbedArea.h"
#include <QMoveEvent>
#include <QPoint>
#include <QApplication>
#include <QTimer>
#include <QPainter>
#include <QVBoxLayout>

static inline QSize getWindowSize( WId );
static inline bool isCtrlPressed();
static inline bool isLeftButtonPressed( WId );

#ifdef _WIN32   // ----------------------- Win32 -----------------------

    #include <windows.h>

    // ----------------------------------------------------------------------------------
    // colorref2qrgb
    // ----------------------------------------------------------------------------------

    static inline QRgb colorref2qrgb( COLORREF col )
    {
        return qRgb( GetRValue( col ), GetGValue( col ), GetBValue( col ) );
    }

    // ----------------------------------------------------------------------------------
    // getWindowSize
    // ----------------------------------------------------------------------------------

    static inline QSize getWindowSize( WId id )
    {
        RECT myRect;
        GetClientRect( id, &myRect );
        return QSize( myRect.right - myRect.left, myRect.bottom - myRect.top );
    }

    // ----------------------------------------------------------------------------------
    // isCtrlPressed
    // ----------------------------------------------------------------------------------

    static inline bool isCtrlPressed()
    {
        return ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) != 0;
    }

    // ----------------------------------------------------------------------------------
    // isLeftButtonPressed
    // ----------------------------------------------------------------------------------

    static inline bool isLeftButtonPressed( WId )
    {
        return ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 ) != 0;
    }

#else           // -----------------------  X11  -----------------------

    #include <X11/Xlib.h>

    // ----------------------------------------------------------------------------------
    // getWindowSize
    // ----------------------------------------------------------------------------------

    static inline QSize getWindowSize( WId id )
    {
        Display* display = XOpenDisplay( 0 );
        Window root;
        int x, y;
        unsigned int width, height, border_width_return, depth_return;
        XGetGeometry( display, id, &root, &x, &y, &width, &height, &border_width_return, &depth_return );
        XCloseDisplay( display );
        return QSize( width, height );
    }

    // ----------------------------------------------------------------------------------
    // isCtrlPressed
    // ----------------------------------------------------------------------------------

    static inline bool isCtrlPressed()
    {
        return false;
    }

    // ----------------------------------------------------------------------------------
    // isLeftButtonPressed
    // ----------------------------------------------------------------------------------

    static inline bool isLeftButtonPressed( WId id )
    {
        Display* display = XOpenDisplay( 0 );
        Window root, child;
        int root_x, root_y, win_x, win_y;
        unsigned int mask;
        XQueryPointer( display, id, &root, &child, &root_x, &root_y, &win_x, &win_y, &mask );
        XCloseDisplay( display );
        return mask & Button1Mask;
    }

#endif          // -----------------------------------------------------



// ----------------------------------------------------------------------------------
// Embeddable
// ----------------------------------------------------------------------------------

const unsigned int Embeddable::embeddedTitlebarHeight = 16;


Embeddable::Embeddable( QWidget* parent )
    : QWidget( parent )
    , hoveringArea( NULL )
    , isAttachedState( false )
    , attachable( false )
#ifndef _WIN32  // -----------------------  X11  -----------------------
    , detachMoveEventSkipped( false )
#endif          // -----------------------------------------------------
{
    this->setLayout( new QVBoxLayout() );

    QTimer::singleShot( 50, this, SLOT( setAttachable() ) );
}


Embeddable::~Embeddable()
{
    if( isAttached() )
    {
        detach();
    }
}


QWidget* Embeddable::widget() const
{
    QLayoutItem* child;
    unsigned int i = 0;
    while( ( child = this->layout()->takeAt( i ) ) != NULL )
    {
        if( child->widget() )
        {
            return child->widget();
        }
        else
        {
            ++i;
        }
    }
    return NULL;
}


void Embeddable::setWidget( QWidget* widget )
{
    QLayoutItem* child;
    while( ( child = this->layout()->takeAt( 0 ) ) != NULL )
    {
        if( child->widget() )
        {
            child->widget()->hide();
            this->layout()->removeWidget( child->widget() );
            delete child;
        }
    }
    
    if( widget )
    {
        this->layout()->addWidget( widget );
    }
}


void Embeddable::setAttachable( bool b )
{
    attachable = b;
}


void Embeddable::moveEvent( QMoveEvent* )
{
    if( isAttached() )
    {
#ifndef _WIN32  // -----------------------  X11  -----------------------
        detachMoveEventSkipped = false;
#endif          // -----------------------------------------------------
        return;
    }
#ifndef _WIN32  // -----------------------  X11  -----------------------
    if( !detachMoveEventSkipped )
    {
        detachMoveEventSkipped = true;
        return;
    }
#endif          // -----------------------------------------------------

    // we need to do this through WinAPI / X11 coz Qt invokes moveEvent *before*
    // resizeEvent and therefore this->size() might lie to us...

    const QSize size1 = getWindowSize( this->winId() );
  //if( size0 != size1 )
    if( this->size() != size1 )
    {
        if( hoveringArea != NULL )
        {
            hoveringArea->embedableLeft( *this );
            hoveringArea = NULL;
        }
        return;
    }

    EmbedManager& mgr = EmbedManager::instance();
    EmbedManager::EmbedAreaSet areas = mgr.embedAreas();

    bool bHoveringAny = false;
    if( attachable && !isCtrlPressed() )
    {
        for( EmbedManager::EmbedAreaSet::iterator it = areas.begin(); it != areas.end(); ++it )
        {
            EmbedArea& area = **it;
            if( area.hasClient() )
            {
                continue;
            }

            const QPoint& areaPos = area.mapToGlobal( QPoint( 0, 0 ) );
            const QRegion areaRegion = area.visibleRegion().translated( areaPos );
            /*
            const QPoint& myPos = this->pos();
            const QSize& mySize = this->size();
            */

            const QPoint myCenter( this->frameGeometry().center() );

            if( areaRegion.contains( myCenter ) )
            {
                if( hoveringArea != &area )
                {
                    if( hoveringArea )
                    {
                        hoveringArea->embedableLeft( *this );
                    }
                    hoveringArea = &area;
                    hoveringArea->embedableEntered( *this );
                }
                bHoveringAny = true;
                break;
            }
        }
    }

    if( hoveringArea != NULL )
    {
        if( !bHoveringAny )
        {
            hoveringArea->embedableLeft( *this );
            hoveringArea = NULL;
        }
        else
        {
            embedDown();
        }
    }
}


void Embeddable::embedDown()
{
    if( hoveringArea )
    {
        if( !isLeftButtonPressed( this->winId() ) )
        {
            hoveringArea->embedableLeft( *this );
            hoveringArea->setClient( *this );
            hoveringArea = NULL;
        }
        else
        {
            QTimer::singleShot( 50, this, SLOT( embedDown() ) );
        }
    }
}


void Embeddable::attachedEvent()
{
    isAttachedState = true;

    setContentsMargins( 0, 20, 0, 0 );

    emit attached();
}


void Embeddable::detachedEvent()
{
    isAttachedState = false;

    setContentsMargins( 0, 0, 0, 0 );

    emit detached();
}


void Embeddable::paintEvent( QPaintEvent* ev )
{
    QWidget::paintEvent( ev );

    if( isAttached() )
    {
        QPainter painter( this );
        painter.setRenderHint( QPainter::Antialiasing );

#ifdef _WIN32   // ----------------------- Win32 -----------------------

        QColor bgcolor = colorref2qrgb( GetSysColor( hasFocus() ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION ) );
        QColor fgcolor = colorref2qrgb( GetSysColor( hasFocus() ? COLOR_CAPTIONTEXT : COLOR_INACTIVECAPTIONTEXT ) );

#else           // -----------------------  X11  -----------------------

        QColor bgcolor = this->palette().highlight().color();
        QColor fgcolor = this->palette().highlightedText().color();

#endif          // -----------------------------------------------------

        painter.fillRect( 0, 0, width(), embeddedTitlebarHeight, bgcolor );
        painter.setPen( fgcolor );
        painter.drawText( 5, 0, width(), embeddedTitlebarHeight, Qt::AlignVCenter, windowTitle() );
    }
}


void Embeddable::detach()
{
    if( isAttached() )
    {
        EmbedArea* const area = static_cast< EmbedArea* >( parentWidget() );
        area->releaseClient();
    }
}


void Embeddable::mouseDoubleClickEvent( QMouseEvent* ev )
{
    if( isAttached() && ev->y() <= signed( embeddedTitlebarHeight ) )
    {
        detach();
    }
}


void Embeddable::closeEvent( QCloseEvent* ev )
{
    this->hide();
    this->deleteLater();
}


bool Embeddable::event( QEvent* ev )
{
    if( ev->type() == QEvent::NonClientAreaMouseButtonPress )
    {
        size0 = this->size();
    }
    return QWidget::event( ev );
}
