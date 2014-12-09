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

#include "ComponentEmbeddable.h"
#include <QMenu>
#include <QPoint>
#include <QMouseEvent>
#include <QInputDialog>
#include <QTimer>
#include <QLayout>



// ----------------------------------------------------------------------------------
// MenuBuilder
// ----------------------------------------------------------------------------------

MenuBuilder::MenuBuilder( QMenu& menu )
    : menu( menu )
    , touched( false )
{
}


void MenuBuilder::addAction( QAction& action )
{
    menu.addAction( &action );
    touched = true;
}


void MenuBuilder::addMenu( QMenu& sub_menu )
{
    menu.addMenu( &sub_menu );
    touched = true;
}


void MenuBuilder::addSeparator()
{
    menu.addSeparator();
    touched = true;
}



// ----------------------------------------------------------------------------------
// ComponentEmbedable
// ----------------------------------------------------------------------------------

const QString ComponentEmbeddable::DEFAULT_SUFFIX_CONNECTOR = ": ";


ComponentEmbeddable::ComponentEmbeddable( QWidget* child
                                        , const QString& suffix
                                        , const QString& suffixConnector
                                        , MenuFactory& menuFactory
                                        , QWidget* parent )
    : Embeddable( parent )
    , suffix( suffix )
    , suffixConnector( suffixConnector )
    , menuFactory( menuFactory )
{
    this->setWidget( child );
    this->setMinimumSize( QSize( 200, 200 ) );
    this->setContentsMargins( 0, 0, 0, 0 );
    this->layout()->setContentsMargins( 0, 0, 0, 0 );
}


ComponentEmbeddable::~ComponentEmbeddable()
{
}


void ComponentEmbeddable::updateWindowTitle()
{
    if( suffix.isEmpty() )
    {
        this->setWindowTitle( baseTitle );
    }
    else
    {
        this->setWindowTitle( baseTitle + suffixConnector + suffix );
    }
    if( this->isAttached() )
    {
        this->update();
    }
}


void ComponentEmbeddable::setBaseTitle( const QString& title )
{
    this->baseTitle = title;
    updateWindowTitle();
}


void ComponentEmbeddable::setBaseTitle( const std::string& title )
{
    setBaseTitle( QString::fromStdString( title ) );
}


void ComponentEmbeddable::setSuffix( const QString& suffix )
{
    this->suffix = suffix;
    updateWindowTitle();
}


void ComponentEmbeddable::setSuffix( const std::string& title )
{
    setSuffix( QString::fromStdString( title ) );
}


void ComponentEmbeddable::setSuffixConnector( const QString& suffixConnector )
{
    this->suffixConnector = suffixConnector;
    updateWindowTitle();
}


void ComponentEmbeddable::setSuffixConnector( const std::string& title )
{
    setSuffixConnector( QString::fromStdString( title ) );
}


void ComponentEmbeddable::mousePressEvent( QMouseEvent* ev )
{
    if( ev->button() == Qt::RightButton )
    {
        if( !isAttached() && ( ev->modifiers() & Qt::ControlModifier ) != 0 )
        {
            QWidget::mousePressEvent( ev );
        }
        else
        {
            QMenu* const menu = new QMenu();
            const bool buildHeaderAndFooter
                    =  !isAttached()
                    || ev->pos().y() <= signed( embeddedTitlebarHeight );

            // build menu header

            if( buildHeaderAndFooter )
            {
                if( isAttached() )
                {
                    QAction* const detach = new QAction( tr( "Detach" ), this );
                    menu->addAction( detach );
                    menu->setDefaultAction( detach );
                    QObject::connect( detach, SIGNAL( triggered() ), this, SLOT( detach() ) );
                }
                else
                {
                    QAction* const fullscreen = new QAction( tr( "Fullscreen" ), this );
                    menu->addAction( fullscreen );
                    QObject::connect( fullscreen, SIGNAL( triggered() ), this, SLOT( fullscreen() ) );
                }
                menu->addAction( tr( "Rename" ), this, SLOT( rename() ) );
                menu->addSeparator();
            }

            // build custom menu

            MenuBuilder mb( *menu );
            menuFactory.buildMenu( mb );

            // build menu footer

            if( buildHeaderAndFooter )
            {
                if( mb.isTouched() )
                {
                    menu->addSeparator();
                }
                menu->addAction( tr( "Close"  ), this, SLOT( close() ) );
            }

            // show menu and respond to user input

            if( buildHeaderAndFooter || mb.isTouched() )
            {
                menu->exec( this->mapToGlobal( ev->pos() ) );
            }

            // clean up

            delete menu;
        }
    }
}


void ComponentEmbeddable::keyPressEvent( QKeyEvent* ev )
{
    if( isAttached() )
    {
        QWidget::keyPressEvent( ev );
        return;
    }

    if( ev->key() == Qt::Key_Escape )
    {
        this->showNormal();
        QTimer::singleShot( 50, this, SLOT( setAttachable() ) );
    }
    else
    {
        QWidget::keyPressEvent( ev );
    }
}


void ComponentEmbeddable::rename()
{
    bool ok;
    QString text = QInputDialog::getText( this, this->windowTitle()
                                        , "Rename:", QLineEdit::Normal
                                        , this->baseTitle, &ok );
    if( ok && !text.isEmpty() )
    {
        emit renamingRequested( text );
    }
}


void ComponentEmbeddable::fullscreen()
{
    if( isFullScreen() )
    {
        this->showNormal();
        QTimer::singleShot( 50, this, SLOT( setAttachable() ) );
    }
    else
    {
        if( isAttached() ) detach();

        this->setAttachable( false );
        this->showFullScreen();
    }
}
