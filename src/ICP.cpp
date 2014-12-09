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

#include "ICP.h"
#include "PointCloud.h"
#include "LocalizerClient.h"
#include "Registration.h"
#include "RegistrationClient.h"
#include "RegistrationComponent.h"
#include "RegistrationArguments.h"
#include "RegistrationController.h"
#include <TRTK/Icp.hpp>
#include <TRTK/EstimateRigidTransformation3D.hpp>
#include <climits>
#include <QCheckBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QProgressDialog>



// ----------------------------------------------------------------------------------
// ICP
// ----------------------------------------------------------------------------------

ICP::ICP( Record::Server& server )
    : RegistrationOperation( "ICP Registration", "Run ICP", server )
    , maxIterations( 500 )
    , minRms( 0.1 )
{
}


QWidget* ICP::createController()
{
    return new IcpController( *this );
}


void ICP::perform( const RegistrationArguments& args
                 , RegistrationController& controller
                 , QWidget* dialogParent )
{
    if( server.hasService( Registration::serviceID ) &&
     &( RegistrationClient( server )->getReferenceBase() ) != &args.referenceBase )
    {
        std::stringstream ss;
        ss << "Your pre-registration is respective to a different reference base than the currently selected.";
        ss << std::endl << std::endl;
        ss << "The pre-registration was performed respectively to: " << RegistrationClient( server )->getReferenceBase().getName();
        throw std::runtime_error( ss.str() );
    }

    const static Carna::Tools::Transformation identity;

    const Carna::Tools::Transformation& initialization =
            ( server.hasService( Registration::serviceID )
            ? RegistrationClient( server )->getTransformation()
            : identity );

 // ----------------------------------------------------------------------------------

    class Status : public QProgressDialog
    {

    public:

        Status( TRTK::Icp3D< double >& icp, QWidget* const dialogParent )
            : QProgressDialog( "Performing ICP...", "Abort", 0, 100, dialogParent )
            , icp( icp )
        {
            QProgressDialog::setWindowTitle( "Registration" );
            QProgressDialog::setWindowModality( Qt::WindowModal );
        }

        void setProgress( unsigned int progress )
        {
            if( QProgressDialog::wasCanceled() )
            {
                icp.abortComputation();
            }
            else
            {
                QProgressDialog::setValue( std::max( static_cast< signed >( progress ), QProgressDialog::value() ) );
            }
        }

    private:

        TRTK::Icp3D< double >& icp;

    };

 // ----------------------------------------------------------------------------------

    TRTK::EstimateRigidTransformation3D< double > estiamtor;
    TRTK::Icp3D< double > icp;

    Status status( icp, dialogParent );
    status.show();

    icp.setSourcePoints( args.recordedPoints.getList() );
    icp.setTargetPoints( args.virtualPoints.getList() );
    icp.setInitialEstimation( initialization.getTransformationMatrix() );
    icp.setEstimationAlgorithm( estiamtor );
    icp.setMaximumNumberIterations( maxIterations );
    icp.setMaximumRMSE( minRms );

    icp.progress.connect( &status, &Status::setProgress );

    const double rms = icp.compute();

    controller.setRegistration( args.referenceBase, Carna::Tools::Transformation( icp.getTransformation() ), rms );
}


unsigned int ICP::getMaxIterations() const
{
    return maxIterations;
}


void ICP::setMaxIterations( int maxIterations )
{
    if( this->maxIterations != maxIterations )
    {
        this->maxIterations = maxIterations;

        emit maxIterationsChanged( this->maxIterations );
    }
}


double ICP::getMinRms() const
{
    return minRms;
}


void ICP::setMinRms( double maxRms )
{
    if( std::abs( this->minRms - maxRms ) > 1e-4 )
    {
        this->minRms = maxRms;

        emit minRmsChanged( this->minRms );
    }
}



// ----------------------------------------------------------------------------------
// ICPController
// ----------------------------------------------------------------------------------

IcpController::IcpController( ICP& icp )
{
    QFormLayout* const form = new QFormLayout();
    this->setLayout( form );

 // max iterations

    QSpinBox* const sbMaxIterations = new QSpinBox();

    sbMaxIterations->setMinimum( 1 );
    sbMaxIterations->setMaximum( std::numeric_limits< int >::max() );
    sbMaxIterations->setValue( static_cast< signed >( icp.getMaxIterations() ) );

    form->addRow( "Maximum Iterations:", sbMaxIterations );

    connect( sbMaxIterations, SIGNAL( valueChanged( int ) ), &icp, SLOT( setMaxIterations( int ) ) );
    connect( &icp, SIGNAL( maxIterationsChanged( int ) ), sbMaxIterations, SLOT( setValue( int ) ) );

 // min RMS

    QDoubleSpinBox* const sbMinRms = new QDoubleSpinBox();

    sbMinRms->setMinimum( 0.1 );
    sbMinRms->setMaximum( std::numeric_limits< double >::max() );
    sbMinRms->setValue( icp.getMinRms() );
    sbMinRms->setDecimals( 1 );
    sbMinRms->setSuffix( " mm" );

    form->addRow( "Minimal RMS:", sbMinRms );

    connect( sbMinRms, SIGNAL( valueChanged( double ) ), &icp, SLOT( setMaxRms( double ) ) );
    connect( &icp, SIGNAL( minRmsChanged( double ) ), sbMinRms, SLOT( setValue( double ) ) );
}
