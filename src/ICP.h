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

#include "RegistrationOperation.h"



// ----------------------------------------------------------------------------------
// ICP
// ----------------------------------------------------------------------------------

/** \brief	Represents the ICP registration algorithm.
  *
  * \author Leonid Kostrykin
  * \date   4.6.12 - 10.8.12
  */
class ICP : public QObject, public RegistrationOperation
{

    Q_OBJECT

public:

    /** \brief	Instantiates.
      */
    ICP( Record::Server& server );


    /** \brief	Returns an \ref ICPController instance.
      */
    virtual QWidget* createController() override;



    /** \brief	Tells the number of iterations limit set.
      */
    unsigned int getMaxIterations() const;

    /** \brief	Tells the RMS limit set.
      */
    double getMinRms() const;


protected:

    /** \brief	Performs the registration.
      */
    virtual void perform( const RegistrationArguments& args
                        , RegistrationController& controller
                        , QWidget* dialogParent ) override;


public slots:

    /** \brief	Sets the maximum number of iterations to make.
      *
      * \pre
      * <code>maxIterations > 0</code>
      */
    void setMaxIterations( int maxIterations );

    /** \brief	Sets the RMS threshold at which the algorithm shall terminate.
      *
      * \pre
      * <code>maxRms > 0</code>
      */
    void setMinRms( double maxRms );


signals:

    void maxIterationsChanged( int );

    void minRmsChanged( double );


private:
    
    /** \brief	Holds the maximum number of iterations to make.
      */
    unsigned int maxIterations;
    
    /** \brief	Holds the RMS threshold at which the algorithm shall terminate.
      */
    double minRms;

}; // ICP



// ----------------------------------------------------------------------------------
// IcpController
// ----------------------------------------------------------------------------------

/** \brief	Provides user-interface for ICP configuration.
  *
  * \author Leonid Kostrykin
  * \date   4.6.12 - 10.8.12
  */
class IcpController : public QWidget
{

public:

    /** \brief	Instantiates.
      */
    IcpController( ICP& );

}; // IcpController
