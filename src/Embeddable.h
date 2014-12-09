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

/** \file   Embeddable.h
  * \brief  Defines Embeddable.
  */

#include <QWidget>

class EmbedArea;



// ----------------------------------------------------------------------------------
// Embeddable
// ----------------------------------------------------------------------------------

/** \brief  Base class for windows which are dockable to embed-areas.
  *
  * In difference to the \c QDockWidget, windows of this class are able to dock
  * anywhere an EmbedArea is placed, in particular even as the central widget of a
  * QMainWindow. Windows derived from \c QDockWidget can only be docked outside of the
  * central widget.
  *
  * No docking is performed while \c CTRL is pressed; this functionality is not yet
  * implemented on Linux.
  *
  * \note  The implementation of this class uses Windows and X11 APIs: On Windows, Qt
  * misses a mechanism to recognize the moment when the user \em 'drops' a window,
  * whereas on Linux this is recognized by \c moveEvent; instead, on Linux, there is no
  * event being triggered, when a window is just carried around the screen by the user
  * without releasing it. One might think that on Windows
  * \c NonClientAreaMouseButtonRelease might be a solution, but this event won't
  * get fired for some Windows-internal reasons: \n\n
  * http://bugreports.qt.nokia.com/browse/QTBUG-1358 \n\n
  * On Linux, neither \c NonClientAreaMouseButtonPress nor
  * \c NonClientAreaMouseButtonRelease are fired.\n\n
  *
  * \note  WinAPI and X11 API are also utilized for the recognition of the window's size
  * from inside of the \c moveEvent implementation. This is required since Qt invokes
  * \c moveEvent \b before \c resizeEvent, which makes \c this->size() lie when the
  * window's size and position are modified simultaneously, e.g. when the user drags the
  * window's left or top border.
  *
  * Use the Embedable::widget() and Embedable::setWidget methods to configure the
  * window's contents unless you don't change the layout, which is a \c QVBoxLayout by
  * default.
  *
  * \see    EmbedArea
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class Embeddable : public QWidget
{

    Q_OBJECT

public:

    /** \brief  Holds the height of the title bar drawn when the window is docked.
      */
    const static unsigned int embeddedTitlebarHeight;


    /** \brief  Instantiates.
      */
    Embeddable( QWidget* parent = NULL );

    /** \brief  Eventually detaches from EmbedArea.
      */
    virtual ~Embeddable();


    /** \brief  Returns whether window is currently docked.
      */
    inline bool isAttached() const
    {
        return isAttachedState;
    }


    /** \brief  References this window's child widget.
      */
    QWidget* widget() const;

    /** \brief  Sets this window's child widget.
      */
    void setWidget( QWidget* );


public slots:

    /** \brief  Detaches this window.
      *
      * Detaches this window from the EmbedArea it's currently docked to.
      * Does nothing, if this windows isn't currently docked anywhere.
      */
    void detach();

    /** \brief  Prevents this window from docking.
      *
      * If the window is already docked, it won't get undocked.
      * If such behavior would be appreciated, call detach.
      */
    void setAttachable( bool b = true );


protected:
    
    friend class EmbedArea;


    /** \brief  Implements this class' main logic.
      */
    virtual void moveEvent( QMoveEvent* );

    /** \brief  Emits the Embedable::embedded signal.
      *
      * Also updates the contents' margins to \f$ (0,20,0,0) \f$.
      *
      * The top-margin is necessary in order to draw the title bar properly.
      */
    virtual void attachedEvent();

    /** \brief  Emits the Embedable::detached signal.
      *
      * Also updates the contents' margins to \f$ (0,0,0,0) \f$.
      *
      * You are free to override this value at will.
      */
    virtual void detachedEvent();

    /** \brief  Paints the title bar when docked.
      */
    virtual void paintEvent( QPaintEvent* );

    /** \brief  Detaches when docked and the title bar was hit.
      */
    virtual void mouseDoubleClickEvent( QMouseEvent* );

    /** \brief  Eventually detaches and emits the Embedable::closed signal.
      */
    virtual void closeEvent( QCloseEvent* );

    /** \brief  Catches the window's size on \c NonClientAreaMouseButtonPress events.
      */
    virtual bool event( QEvent* );


private:

    /** \brief  Reference the EmbedArea this window is hovering.
      *
      * References the EmbedArea this window, when dragged, is hovering.
      * If no EmbedArea is hovered, the value is set to \c NULL.
      */
    EmbedArea* hoveringArea;

    /** \brief  Indicates whether this window is currently docked.
      */
    bool isAttachedState;

    /** \brief  Indicates whether this window is prevented from docking.
      */
    bool attachable;


    /** \brief  Stores this window's size on a \c NonClientAreaMouseButtonPress event.
      */
    QSize size0;


#ifndef _WIN32

    bool detachMoveEventSkipped;

#endif


signals:

    /** \brief  Emitted when window gets docked.
      */
    void attached();

    /** \brief  Emitted when window gets undocked.
      */
    void detached();


private slots:

    /** \brief  Docks this window to the EmbedArea currently hovered.
      *
      * If no EmbedArea is hovered, nothing happens.
      */
    void embedDown();

}; // Embeddable
