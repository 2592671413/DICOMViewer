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

#include "ToolSetEditor.h"
#include "ToolEditor.h"
#include "RegistrationClient.h"
#include "LocalizerProvider.h"
#include <QVBoxLayout>
#include <QInputDialog>
#include <QFileDialog>
#include <QDomDocument>
#include <QDomElement>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>



// ----------------------------------------------------------------------------------
// ToolSetEditor
// ----------------------------------------------------------------------------------

ToolSetEditor::ToolSetEditor( LocalizerComponent& component, Record::Server& server, LocalizerProvider& localizer, QWidget* parent )
    : QWidget( parent )
    , server( server )
    , component( component )
    , localizer( localizer )
    , editors( new QVBoxLayout() )
    , paused( false )
{
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing( 0 );

    editors->addStretch();
    layout ->addLayout( editors );

    layout ->setContentsMargins( 0, 0, 0, 0 );
    editors->setContentsMargins( 0, 0, 0, 0 );

    this->setLayout( layout );
}


void ToolSetEditor::addTool()
{
    const std::string port = promptPort();

    if( port.empty() )
    {
        return;
    }

    this->add( port, port );
}


void ToolSetEditor::remove( ToolEditor* editor )
{
    editorsSet.erase( editor );

    editor->hide();

    editors->removeWidget( editor );

    delete editor;
}


std::string ToolSetEditor::promptPort()
{
    bool ok;

    QString text = QInputDialog::getText( this, this->windowTitle()
                                        , "Port:", QLineEdit::Normal
                                        , "", &ok );

    if( ok && !text.isEmpty() )
    {
        return text.toStdString();
    }
    else
    {
        return "";
    }
}


void ToolSetEditor::updateViews()
{
    if( paused )
    {
        return;
    }

    for( std::set< ToolEditor* >::iterator it = editorsSet.begin(); it != editorsSet.end(); ++it )
    {
        ( **it ).updateView();
    }

    if( server.hasService( Registration::serviceID ) )
    {
        RegistrationClient( server )->updateVirtualElements();
    }
}


void ToolSetEditor::save()
{
    const QString filename
        = QFileDialog::getSaveFileName
            ( this
            , "Save Localizer Setup"
            , ""
            , "XML-Files (*.XML)"
            , 0
            , QFileDialog::DontResolveSymlinks
            | QFileDialog::HideNameFilterDetails );

    if( filename.isEmpty() )
    {
        return;
    }

    QDomDocument dom;
    dom.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"utf-8\"" );

    QDomElement root = dom.createElement( "localizer" );
    dom.appendChild( root );

    const unsigned int interval = localizer.getLazynessThreshold();
    root.setAttribute( "interval", interval );

    // see: http://www.digitalfanatics.org/projects/qt_tutorial/chapter09.html

    for( std::set< ToolEditor* >::const_reverse_iterator it = editorsSet.rbegin(); 
         it != editorsSet.rend();
       ++it )
    {
        const ToolEditor& editor = **it;
        const CRA::ToolByPort& rb = editor.getTool();

        QDomElement node = dom.createElement( "tool" );

        node.setAttribute( "port", QString::fromStdString( rb.getPort() ) );
        node.setAttribute( "name", QString::fromStdString( rb.getName() ) );
        
     // pivot calibration

        node.setAttribute( "dx", QString::number( rb.getOffset().x() ) );
        node.setAttribute( "dy", QString::number( rb.getOffset().y() ) );
        node.setAttribute( "dz", QString::number( rb.getOffset().z() ) );

        root.appendChild( node );
    }

    QFile file( filename );
    if( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        QMessageBox::critical( this, "Save Localizer Setup", "Failed opening file for writing." );
        return;
    }

    QTextStream stream( &file );
    stream << dom.toString();

    file.close();
}


void ToolSetEditor::load()
{
    const QString filename
        = QFileDialog::getOpenFileName
            ( this
            , "Load Localizer Setup"
            , ""
            , "XML-Files (*.xml)"
            , 0
            , QFileDialog::ReadOnly
            | QFileDialog::HideNameFilterDetails );

    if( filename.isEmpty() )
    {
        return;
    }

    QFile file( filename );
    if( !file.open( QIODevice::ReadOnly ) )
    {
        QMessageBox::critical( this, "Load Localizer Setup", "Failed opening file for reading." );
        return;
    }

    this->clear();

    struct unexpected_format_exception { };
    try
    {

        QDomDocument dom;
        if( !dom.setContent( &file ) )
        {
            throw unexpected_format_exception();
        }

        QDomElement root = dom.documentElement();
        if( root.tagName() != "localizer" )
        {
            throw unexpected_format_exception();
        }

        // read general settings

        bool ok = true;
        const QString interval_str = root.attribute( "interval" );
        const double interval = interval_str.toDouble( &ok );
        localizer.setLazynessThreshold( interval );
        if( !ok )
        {
            throw unexpected_format_exception();
        }

        // read tools

        QDomNode n = root.firstChild();
        while( !n.isNull() )
        {
            QDomElement e = n.toElement();
            if( !e.isNull() )
            {
                if( e.tagName() != "tool" && e.tagName() != "rigidbody" )
                {
                    throw unexpected_format_exception();
                }

                const QString port = e.attribute( "port" );
                const QString name = e.attribute( "name" );

                CRA::Tool& rb = this->add( port.toStdString(), name.toStdString() );

                if( e.hasAttribute( "dx" ) )
                {
                    bool ok1 = true, ok2 = true, ok3 = true;

                    const double dx = e.attribute( "dx" ).toDouble( &ok1 );
                    const double dy = e.attribute( "dy" ).toDouble( &ok2 );
                    const double dz = e.attribute( "dz" ).toDouble( &ok3 );

                    if( !ok1 || !ok2 || !ok3 )
                    {
                        throw unexpected_format_exception();
                    }

                    rb.setOffset( Carna::Tools::Vector( dx, dy, dz ) );
                }
            }

            n = n.nextSibling();
        }
    }
    catch( const unexpected_format_exception& )
    {
        QMessageBox::critical( this, "Load Localizer Setup", "Unexpected file format." );
    }
    catch( const std::logic_error& ex )
    {
        std::stringstream ss;
        ss << "Logical Exception: " << ex.what();
        QMessageBox::critical( this, "Load Localizer Setup", QString::fromStdString( ss.str() ) );
    }
}


void ToolSetEditor::setPaused( bool paused )
{
    this->paused = paused;
}


void ToolSetEditor::pause()
{
    this->setPaused( true );
}


void ToolSetEditor::resume()
{
    this->setPaused( false );
}


void ToolSetEditor::clear()
{
    while( !editorsSet.empty() )
    {
        this->remove( *( editorsSet.begin() ) );
    }
}


CRA::Tool& ToolSetEditor::add( const std::string& port, const std::string& name )
{
    ToolEditor* editor = new ToolEditor( component, server, port, name, localizer );

    editorsSet.insert( editor );

    editors->insertWidget( 0, editor );

    connect( editor, SIGNAL( removeRequested( ToolEditor* ) ), this, SLOT( remove( ToolEditor* ) ) );

    return editor->getTool();
}
