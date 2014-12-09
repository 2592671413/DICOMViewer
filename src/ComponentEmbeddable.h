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

/** \file   ComponentEmbeddable.h
  * \brief  Defines ComponentEmbeddable, MenuBuilder, MenuFactory, NullMenuFactory.
  */

#include "Embeddable.h"
#include <Carna/base/Singleton.h>
#include <set>

class QMenu;



// ----------------------------------------------------------------------------------
// MenuBuilder
// ----------------------------------------------------------------------------------

/** \brief  Provides functionality to extend an existing \c QMenu.
  *
  * \see    \ref ComponentEmbeddable
  * \see    \ref MenuBuilder
  * \author Leonid Kostrykin
  * \date   1.3.2011
  */
class MenuBuilder
{

public:

    /** \brief  Instantiates.
      */
    MenuBuilder( QMenu& );


    /** \brief  Appends the action to the menu.
      */
    void addAction( QAction& );

    /** \brief  Appends a sub menu to the menu.
      */
    void addMenu( QMenu& );

    /** \brief  Appends a separator to the menu.
      */
    void addSeparator();


    /** \brief  Returns whether the associated menu has been extended.
      */
    inline bool isTouched() const
    {
        return touched;
    }


private:

    /** \brief  The \c QMenu this object operates with.
      */
    QMenu& menu;

    /** \brief  Holds whether any changed has been made to the associated menu.
      */
    bool touched;

}; // MenuBuilder



// ----------------------------------------------------------------------------------
// MenuFactory
// ----------------------------------------------------------------------------------

/** \brief  Provides interface which operates on MenuBuilder instance.
  *
  * \see    \ref ComponentEmbeddable
  * \author Leonid Kostrykin
  * \date   1.3.2011
  */
class MenuFactory
{

public:

    /** \brief  Does nothing.
      */
    virtual ~MenuFactory()
    {
    }


    /** \brief  Builds the menu.
      */
    virtual void buildMenu( MenuBuilder& ) = 0;

}; // MenuFactory



// ----------------------------------------------------------------------------------
// NullMenuFactory
// ----------------------------------------------------------------------------------

/** \brief  Null-implementation of the MenuFactory interface
  *
  * \author Leonid Kostrykin
  * \date   1.3.2011
  */
class NullMenuFactory : public MenuFactory, public Carna::base::Singleton< NullMenuFactory >
{

public:

    /** \brief  Does nothing.
      */
    virtual inline void buildMenu( MenuBuilder& )
    {
    }

}; // NullMenuFactory



// ----------------------------------------------------------------------------------
// ComponentEmbeddable
// ----------------------------------------------------------------------------------

/** \brief  Embeddable specialization as used by \ref Component implementations.
  *
  * The user of this class has no more access to the layout. Instead, the element
  * presented in this window is specified in the constructor. Furthermore, this class
  * is provided with a mechanism for decorating the window's title with a suffix. To do
  * so, use \c setBaseTitle instead of \c setWindowTitle. The class' behavior
  * is undefined for the combined usage of both functions.
  *
  * Every window of this class offers a fixed set of certain operations, which can be
  * accessed by the user through the context menu, plus an optional set of further
  * operations. The latter ones are configured by supplying a custom MenuFactory to the
  * constructor. When the window is undocked, the context menu will be suppressed when
  * holding \c CTRL down.
  *
  * The fixed operations set include:
  * - Detaching a docked window
  * - Putting a floating window into full-screen mode
  * - Prompting the user for a new window title
  * - Closing the window
  *
  * \see    Embeddable
  * \see    \ref Component
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class ComponentEmbeddable : public Embeddable
{

    Q_OBJECT

public:

    /** \brief  Holds the default string which is placed in between of the title and it's suffix.
      */
    const static QString DEFAULT_SUFFIX_CONNECTOR;


    /** \brief  Instantiates.
      *
      * \param[in]  child           references the widget which is placed within this window.
      * \param[in]  titleSuffix     is the suffix which is appended to this window's \ref baseTitle "base title".
      * \param[in]  suffixConnector is the string which is placed in between of the title and it's suffix when the suffix is not empty.
      * \param[in]  menuFactory     defines the context menu creation strategy.
      * \param[in]  parent          is the parent which is passed to the \c QWidget constructor.
      */
    ComponentEmbeddable( QWidget* child
                       , const QString& titleSuffix = ""
                       , const QString& suffixConnector = DEFAULT_SUFFIX_CONNECTOR
                       , MenuFactory& menuFactory = NullMenuFactory::instance()
                       , QWidget* parent = nullptr );

    virtual ~ComponentEmbeddable();


    /** \brief  Tells this window's title suffix.
      *
      * \see    \ref setSuffix,
      *         \ref getSuffixConnector, \ref \setSuffixConnector,
      *         \ref getBaseTitle, \ref \setBaseTitle
      */
    const QString& getSuffix() const
    {
        return suffix;
    }
    
    /** \brief  Tells this window's title without the suffix.
      *
      * \see    \ref getSuffix, \ref setSuffix,
      *         \ref getSuffixConnector, \ref \setSuffixConnector,
      *         \ref \setBaseTitle
      */
    const QString& getBaseTitle() const
    {
        return baseTitle;
    }
    
    /** \brief  Tells the string which is placed in between of the title and it's suffix.
      *
      * \see    \ref getSuffix, \ref setSuffix,
      *         \ref \setSuffixConnector,
      *         \ref getBaseTitle, \ref \setBaseTitle
      */
    const QString& getSuffixConnector() const
    {
        return suffixConnector;
    }


public slots:

    /** \brief  Sets the window's title to be the concatenation of \c title and the suffix.
      *
      * \see    \ref getSuffix, \ref setSuffix,
      *         \ref getSuffixConnector, \ref \setSuffixConnector,
      *         \ref getBaseTitle
      */
    void setBaseTitle( const QString& title );

    /** \copydoc setBaseTitle(const QString&)
      */
    void setBaseTitle( const std::string& title );

    
    /** \brief  Sets the window's title to be the concatenation of \ref baseTitle and the given \a suffix.
      *
      * \see    \ref getSuffix
      *         \ref getSuffixConnector, \ref \setSuffixConnector,
      *         \ref getBaseTitle, \ref \setBaseTitle
      */
    void setSuffix( const QString& suffix );

    /** \copydoc setSuffix(const QString&)
      */
    void setSuffix( const std::string& suffix );
    

    /** \brief  Sets the string which is placed in between of the title and it's suffix.
      *
      * \see    \ref getSuffix, \ref setSuffix,
      *         \ref \setSuffixConnector,
      *         \ref getBaseTitle, \ref \setBaseTitle
      */
    void setSuffixConnector( const QString& );
    
    /** \copydoc setSuffixConnector(const QString&)
      */
    void setSuffixConnector( const std::string& );


protected:

    /** \brief  Prompts the user by a context menu.
      */
    virtual void mousePressEvent( QMouseEvent* ev ) override;

    /** \brief  Exits full screen mode on \c ESC hit.
      */
    virtual void keyPressEvent( QKeyEvent* ev ) override;


private:

    /** \brief  Holds the suffix which is put to the end of the window title.
      *
      * \see    \ref setBaseTitle, ref \setSuffix
      */
    QString suffix;

    /** \brief  This windows' title.
      */
    QString baseTitle;

    /** \brief  String which is placed between the title and the suffix.
      */
    QString suffixConnector;

    /** \brief  References an object which is queried for the creation of context menus.
      */
    MenuFactory& menuFactory;


    /** \brief  Updates this window's title from the configured \ref suffix, \ref baseTitle
      *         and \ref suffixConnector "their connector".
      */
    void updateWindowTitle();


private slots:

    /** \brief  Prompts the user for a new name.
      */
    void rename();

    /** \brief  Puts this window into full screen mode.
      */
    void fullscreen();


signals:

    /** \brief  Emitted when user has requested a new window title.
      */
    void renamingRequested( const QString& new_name );

}; // ComponentEmbeddable
