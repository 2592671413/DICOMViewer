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

/** \file   ViewWindow.h
  * \brief  Defines ViewWindow.
  */

#include <QMainWindow>
#include <vector>
#include "Component.h"

class EmbedAreaArray;
class ComponentLauncher;
class ViewWindowComponentWindowFactory;



// ----------------------------------------------------------------------------------
// ViewWindow
// ----------------------------------------------------------------------------------

/** \brief  Each instance implements one single tab of MainWindow.
  *
  * Provides an EmbedAreaArray, that can be configured by simple user interaction,
  * offering up to four EmbedArea instances. Also provides an user interface for the
  * acquisition of \ref Component instances, which basically are views on the data
  * model. All registred components are listed automatically; refer to the
  * \ref RegistredComponent class documentation for more details.
  *
  * \image  html    viewwindow.png
  *
  * \see    \ref ViewWindowComponentWindowFactory
  * \see    \ref MainWindow
  * \see    \ref EmbedAreaArray
  * \see    \ref Component
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class ViewWindow : public QMainWindow
{

    Q_OBJECT

    NON_COPYABLE

public:

    /** \brief  Instantiates.
      */
    ViewWindow( Record::Server& server, QWidget* parent = 0, Qt::WFlags flags = 0 );

    ~ViewWindow();


    /** \brief  Instantiates the component represented by the given launcher.
      */
    void launch( ComponentLauncher& );


private:

    /** \brief  References the data model.
      */
    Record::Server& server;

    /** \brief  References an array of embed areas.
      */
    EmbedAreaArray* areaArray;

    std::unique_ptr< ViewWindowComponentWindowFactory > componentWindowfactory;


    QAction* const pShow1x1;    ///< \brief  Switches areaArray to \f$ 1 \times 1 \f$ when triggered.
    QAction* const pShow2x1;    ///< \brief  Switches areaArray to \f$ 2 \times 1 \f$ when triggered.
    QAction* const pShow1x2;    ///< \brief  Switches areaArray to \f$ 1 \times 2 \f$ when triggered.
    QAction* const pShow2x2;    ///< \brief  Switches areaArray to \f$ 2 \times 2 \f$ when triggered.

    std::set< Component* > myComponents;


private slots:

    void show1x1(); ///< \brief Switches areaArray to \f$ 1 \times 1 \f$.
    void show1x2(); ///< \brief Switches areaArray to \f$ 1 \times 2 \f$.
    void show2x1(); ///< \brief Switches areaArray to \f$ 2 \times 1 \f$.
    void show2x2(); ///< \brief Switches areaArray to \f$ 2 \times 2 \f$.

    void removeReleasedComponent( QObject* );

}; // ViewWindow
