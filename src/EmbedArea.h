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

#pragma once

/** \file   EmbedArea.h
  * \brief  Defines EmbedArea.
  */

#include <QFrame>
#include <Carna/noncopyable.h>

class Embeddable;



// ----------------------------------------------------------------------------------
// EmbedArea
// ----------------------------------------------------------------------------------

/** \brief  Marks an area where embedables can be docked.
  *
  * A single instance of this class may contain either \b none or \b exactly \b one
  * docked client at any certain time. Docking can be performed either programmatically
  * or by user interaction, namely when the user \em 'drops' a window upon an Embedable
  * instance.
  *
  * \see    Embedable, EmbedAreaArray, EmbedManager
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class EmbedArea : public QFrame
{

    Q_OBJECT

    NON_COPYABLE

public:

    EmbedArea( QWidget* parent = NULL );

    virtual ~EmbedArea();


    /** \brief  Docks the specified window to this area.
      *
      * If there is another window docked, it gets detached automatically.
      */
    void setClient( Embeddable& );

    /** \brief  Detaches the window currently docked.
      *
      * If there is no currently docked window, nothing happens.
      */
    void releaseClient();

    /** \brief  References the currently attached window.
      * 
      * \throws std::logic_error    Thrown when no client window is docked.
      */
    Embeddable& getClient() const;

    /** \brief  Returns whether a window is currently docked.
      */
    bool hasClient() const;


    /** \brief  Only relevant to Embedable.
      *
      * Invoked when a dragged Embedable enters the screen-space of this EmbedArea.
      */
    virtual void embedableEntered( Embeddable& );

    /** \brief  Only relevant to Embedable.
      *
      * Invoked when a dragged Embedable leaves the screen-space of this EmbedArea.
      */
    virtual void embedableLeft( Embeddable& );


protected:

    /** \brief  Registers this embed area.
      */
    virtual void showEvent( QShowEvent* );

    /** \brief  Unregisters this embed area.
      */
    virtual void hideEvent( QShowEvent* );

    /** \brief  Indicates by color when this embed area is hovered by an embedable. 
      */
    virtual void paintEvent( QPaintEvent* event );


private:

    /** \brief  References the docked window.
      *
      * Is \c NULL, when there is no window docked.
      */
    Embeddable* pClient;

    /** \brief  Specifies whether the EmbedArea is hovered by any Embedable.
      */
    bool bHovering;


signals:

    /** \brief  Emitted right after a window has been docked.
      */
    void clientEmbedded( Embeddable& );
    
    /** \brief  Emitted right after a window has been undocked.
      */
    void clientReleased( Embeddable& );

}; // EmbedArea
