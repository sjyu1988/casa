//# SIImageStore.cc: Implementation of Imager.h
//# Copyright (C) 1997-2008
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <casa/Exceptions/Error.h>
#include <casa/iostream.h>
#include <casa/sstream.h>

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>

#include <casa/Logging.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>

#include <casa/OS/DirectoryIterator.h>
#include <casa/OS/File.h>
#include <casa/OS/Path.h>

#include <casa/OS/HostInfo.h>

#include <ms/MeasurementSets/MSHistoryHandler.h>
#include <ms/MeasurementSets/MeasurementSet.h>

#include <synthesis/ImagerObjects/SIImageStore.h>


#include <sys/types.h>
#include <unistd.h>
using namespace std;

namespace casa { //# NAMESPACE CASA - BEGIN

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  
  SIImageStore::SIImageStore() 
  {
    itsPsf=NULL;
    itsModel=NULL;
    itsResidual=NULL;
    itsWeight=NULL;
    itsImage=NULL;

    itsImageName=String("");
    itsImageShape=IPosition();

    itsValidity = False;

  }

  /*
  SIImageStore::SIImageStore(String imagename) 
  {
    LogIO os( LogOrigin("SIImageStore","Open existing Images",WHERE) );

    itsImageName = imagename;

    if( doImagesExist( ) )
      {
	itsResidual = new PagedImage<Float> (itsImageName+String(".residual"));
	itsPsf = new PagedImage<Float> (itsImageName+String(".psf"));
	itsWeight = new PagedImage<Float> (itsImageName+String(".weight"));
	itsModel = new PagedImage<Float> (itsImageName+String(".model"));

	itsImageShape = itsResidual->shape();
	
	if( ( itsPsf->shape() != itsImageShape ) ||
	    ( itsWeight->shape() != itsImageShape ) ||
	    ( itsModel->shape() != itsImageShape ) )
	  {
	    throw( AipsError("Shapes of "+itsImageName+".{residual,psf,weight,model} are not identical") );
	  }

      }
    else
      {
	/// Make this more intelligent. For instance, don't always need 'weight' and model for different stages.
	throw( AipsError( "Images do not exist. Please create them" ) );
      }

    itsValidity = True;
  }
  */

  SIImageStore::SIImageStore(String imagename) 
  {
    LogIO os( LogOrigin("SIImageStore","Open existing Images",WHERE) );

    itsImageName = imagename;

    // The PSF and Residual images must exist. 
    if( doesImageExist(itsImageName+String(".residual")) && doesImageExist(itsImageName+String(".psf")) )
      {
	itsResidual = new PagedImage<Float> (itsImageName+String(".residual"));
	itsPsf = new PagedImage<Float> (itsImageName+String(".psf"));

	// The weight image is optional.
	if( doesImageExist( itsImageName+String(".weight") ) )
	  {
	    itsWeight = new PagedImage<Float> (itsImageName+String(".weight"));
	  }
	else
	  {
	    itsWeight = NULL;
	  }

	// The model must exist, but if it does not, create it using the shape from residual
	if( doesImageExist( itsImageName+String(".model") ))
	  {
	    itsModel = new PagedImage<Float> (itsImageName+String(".model"));
	  }
	else
	  {
	    itsModel = new PagedImage<Float> (itsResidual->shape(), itsResidual->coordinates(), itsImageName+String(".model"));
	    itsModel->set(0.0);
	  }
	
	itsImageShape = itsResidual->shape();
	if( ( itsPsf->shape() != itsImageShape ) ||
	    ( (itsWeight.null())?False:(itsWeight->shape() != itsImageShape ) ) ||
	    ( itsModel->shape() != itsImageShape ) )
	  {
	    throw( AipsError("Shapes of "+itsImageName+".{residual,psf,weight,model} are not identical") );
	  }

      }
    else
      {
	/// Make this more intelligent. For instance, don't always need 'weight' and model for different stages.
	throw( AipsError( "PSF and Residual Images do not exist. Please create them" ) );
      }

    itsValidity = True;
  }// end of Constructor 1


  SIImageStore::SIImageStore(String imagename, 
			     CoordinateSystem &imcoordsys, 
			     IPosition imshape)
  {
    LogIO os( LogOrigin("SIImageStore","Open new Images",WHERE) );

    itsImageName = imagename;
    itsImageShape = imshape;

    // Make the Images.
    //     -- Check, and create a new image only if it does not already exist on disk.
    //     -- If it exists on disk, check that it's shape and coordinates 

    if( doImagesExist( ) )
      {
	os << "Images already exist. Overwriting them";
      }

    itsResidual = new PagedImage<Float> (itsImageShape, imcoordsys, itsImageName+String(".residual"));
    itsPsf = new PagedImage<Float> (itsImageShape, imcoordsys, itsImageName+String(".psf"));
    itsWeight = new PagedImage<Float> (itsImageShape, imcoordsys, itsImageName+String(".weight"));
    itsModel = new PagedImage<Float> (itsImageShape, imcoordsys, itsImageName+String(".model"));

    itsResidual->set(0.0);
    itsPsf->set(1.0);
    itsWeight->set(1.0);
    itsModel->set(0.0);

    if( itsImageShape[0]==3 && itsImageShape[1]==3 )
      {
	// Make a PSF with 1 in the center pixel.
	itsPsf->set(-0.1);
	for(uInt i=0;i<itsImageShape[2];i++)
	  for(uInt j=0;j<itsImageShape[3];j++)
	    {
	      itsPsf->putAt( 1.0, IPosition(4,1,1,i,j) );
	    }
      }

    os << LogIO::POST;
    
    itsValidity=True;

  }// End of constructor 2

  
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  SIImageStore::~SIImageStore() 
  {
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  void SIImageStore::setModelImage( String modelname )
  {
    LogIO os( LogOrigin("SIImageStore","setModelImage",WHERE) );
    
    Directory immodel( modelname+String(".model") );
    if( !immodel.exists() ) 
      {
	os << "Starting model image does not exist. No initial prediction will be done" << LogIO::POST;
	return;
      }

    CountedPtr<PagedImage<Float> > model = new PagedImage<Float>( modelname+String(".model") );
    // Check shapes, coordsys with those of other images.  If different, try to re-grid here.

    if( model->shape() != itsModel->shape() )
      {
	// For now, throw an exception.
	throw( AipsError( "Input model image "+modelname+".model is not the same shape as that defined for output in "+ itsImageName + ".model" ) );
      }

    os << "Setting " << modelname << " as model " << LogIO::POST;
    // Then, add its contents to itsModel.
    //itsModel->put( itsModel->get() + model->get() );
    itsModel->put( model->get() );
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  IPosition SIImageStore::getShape()
  {
    return itsImageShape;
  }

  String SIImageStore::getName()
  {
    return itsImageName;
  }

  CountedPtr<PagedImage<Float> > SIImageStore::psf()
  {
    return itsPsf;
  }
  CountedPtr<PagedImage<Float> > SIImageStore::residual()
  {
    return itsResidual;
  }
  CountedPtr<PagedImage<Float> > SIImageStore::weight()
  {
    if( itsWeight.null() )
      {
	throw( AipsError("Internal error : Weight Image does not exist. Please check with SIImageStore.hasWeight() before accessing the weight image. If not present, treat it as a scalar = 1.0") );
      }
    return itsWeight;
  }
  CountedPtr<PagedImage<Float> > SIImageStore::model()
  {
    return itsModel;
  }
  CountedPtr<PagedImage<Float> > SIImageStore::image()
  {
    return itsImage;
  }

  // TODO : Move to an image-wrapper class ? Same function exists in SynthesisDeconvolver.
  Bool SIImageStore::doImagesExist()
  {
    LogIO os( LogOrigin("SIImageStore","doImagesExist",WHERE) );
    // Check if imagename.residual, imagename.psf. imagename.weight
    // exist on disk and if they're the right shape.
    // If the shape is not right, complain here and throw an exception (or just say it will get overwritten)

    Directory impsf( itsImageName+String(".psf") );
    Directory imresidual( itsImageName+String(".residual") );
    Directory imweight( itsImageName+String(".weight") );
    Directory immodel( itsImageName+String(".model") );

    return impsf.exists() & imresidual.exists() & imweight.exists() & immodel.exists();
  }

  // TODO : Move to an image-wrapper class ? Same function exists in SynthesisDeconvolver.
  Bool SIImageStore::doesImageExist(String imagename)
  {
    LogIO os( LogOrigin("SIImageStore","doesImageExist",WHERE) );
    Directory image( imagename );
    return image.exists();
  }


   void SIImageStore::allocateRestoredImage()
  {

    //    itsImage = new PagedImage<Float> (itsImageShape, *imcoordsys, itsImageName+String(".residual"));
  }

  void SIImageStore::resetImages( Bool resetpsf, Bool resetresidual, Bool resetweight )
  {
    if( resetpsf ) itsPsf->set(0.0);
    if( resetresidual ) itsResidual->set(0.0);
    if( resetweight && !itsWeight.null() ) itsWeight->set(0.0);
  }

  void SIImageStore::addImages( CountedPtr<SIImageStore> imagestoadd,
				Bool addpsf, Bool addresidual, Bool addweight)
  {

    if( itsWeight.null() )
      {
	throw( AipsError("Internal Error : Weight image from major cycle is not present. Cannot gather a weighted sum from all nodes") );
      }

    if(addpsf)
      {
	LatticeExpr<Float> adderPsf( *itsPsf + *(imagestoadd->psf()) ); 
	itsPsf->copyData(adderPsf);
      }
    if(addresidual)
      {
	LatticeExpr<Float> adderRes( *itsResidual + *(imagestoadd->residual()) ); 
	itsResidual->copyData(adderRes);
      }
    if(addweight)
      {
	LatticeExpr<Float> adderWeight( *itsWeight + *(imagestoadd->weight()) ); 
	itsWeight->copyData(adderWeight);
      }
    ///cout << "Res : " << itsResidual->getAt( IPosition(4,0,0,0,0) ) << "  Wt : " << itsWeight->getAt( IPosition(4,0,0,0,0) ) << endl;
  }

  // Make another for the PSF too.
  void SIImageStore::divideResidualByWeight(Float weightlimit)
  {
    LogIO os( LogOrigin("SIImageStore","divideResidualByWeight",WHERE) );

    if( itsWeight.null() )
      {
	os << "Weights are 1.0. Not dividing " << itsImageName+String(".residual") << LogIO::POST;
      }
    else
      {
	os << "Dividing " << itsImageName+String(".residual") << " by the weight image " << itsImageName+String(".weight") << LogIO::POST;
	
	///cout << " Dividing : " << itsResidual->getAt( IPosition(4,0,0,0,0) ) << " by " << itsWeight->getAt( IPosition(4,0,0,0,0) ) << endl;
	
	LatticeExpr<Float> mask( iif( (*itsWeight) > weightlimit , 1.0, 0.0 ) );
	LatticeExpr<Float> maskinv( iif( (*itsWeight) > weightlimit , 0.0, 1.0 ) );
	
	LatticeExpr<Float> ratio( ( (*itsResidual) * mask ) / ( (*itsWeight) + maskinv) );
	itsResidual->copyData(ratio);
      }
    // createMask
  }

  void SIImageStore::dividePSFByWeight(Float weightlimit)
  {
    LogIO os( LogOrigin("SIImageStore","dividePSFByWeight",WHERE) );

    if( itsWeight.null() )
      {
	os << "Weights are 1.0. Not dividing " << itsImageName+String(".psf") << LogIO::POST;
      }
    else
      {
	os << "Dividing " << itsImageName+String(".psf") << " by the weight image " << itsImageName+String(".weight") << LogIO::POST;
	
	///cout << " Dividing : " << itsResidual->getAt( IPosition(4,0,0,0,0) ) << " by " << itsWeight->getAt( IPosition(4,0,0,0,0) ) << endl;
	
	LatticeExpr<Float> mask( iif( (*itsWeight) > weightlimit , 1.0, 0.0 ) );
	LatticeExpr<Float> maskinv( iif( (*itsWeight) > weightlimit , 0.0, 1.0 ) );
	
	LatticeExpr<Float> ratio( ( (*itsPsf) * mask ) / ( (*itsPsf) + maskinv) );
	itsPsf->copyData(ratio);
      }
    // createMask
  }

  void SIImageStore::divideModelByWeight(Float weightlimit)
  {
    LogIO os( LogOrigin("SIImageStore","divideModelByWeight",WHERE) );

    if( itsWeight.null() )
      {
	os << "Weights are 1.0. Not dividing " << itsImageName+String(".residual") << LogIO::POST;
      }
    else
      {
	os << "Dividing " << itsImageName+String(".model") << " by the weight image " << itsImageName+String(".weight") << LogIO::POST;
	
	LatticeExpr<Float> mask( iif( (*itsWeight) > weightlimit , 1.0, 0.0 ) );
	LatticeExpr<Float> maskinv( iif( (*itsWeight) > weightlimit , 0.0, 1.0 ) );
	
	LatticeExpr<Float> ratio( ( (*itsModel) * mask ) / ( (*itsModel) + maskinv) );
	itsModel->copyData(ratio);
      }    
    // createMask
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////

} //# NAMESPACE CASA - END

