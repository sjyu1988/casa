#define DDPRIORITY 1
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <assert.h>
#include <cmath>
#include <complex>
#include <string>

#include <boost/algorithm/string.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <boost/algorithm/string.hpp>
using namespace boost;

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/regex.hpp> 

#include <ASDMAll.h>

#include "SDMBinData.h"
using namespace sdmbin;

#include <exception>
using namespace asdm;
#include "IllegalAccessException.h"

#include "UvwCoords.h"
#include "ASDM2MSFiller.h"

#include "measures/Measures/Stokes.h"
#include "measures/Measures/MFrequency.h"
using namespace casa;

#include "CBasebandName.h"
#include "CCalibrationDevice.h"
using namespace CalibrationDeviceMod;
#include "CFrequencyReferenceCode.h"
#include "CPolarizationType.h"
#include "CProcessorSubType.h"
#include "CProcessorType.h"
#include "CScanIntent.h"
#include "CSubscanIntent.h"
using namespace SubscanIntentMod;
#include "CStokesParameter.h"

#include "Name2Table.h"
#include "ASDMVerbatimFiller.h"

#include "SDMDataObjectReader.h"
#include "SDMDataObject.h"

#include "TableStreamReader.h"

#include	"time.h"			/* <time.h> */
#if	defined(__sysv__)
#include	<sys/time.h>
#elif	defined(__bsd__)
#define		ftime	FTIME
#include	<sys/timeb.h>			/* from system */
#undef		ftime
extern	void	ftime( struct timeb * );	/* this is the funtion */
#else
#endif


void	myTimer( double *cpu_time ,		/* cpu timer */
		 double *real_time ,		/* real timer */
		 int  *mode )			/* the mode */
{
  clock_t	tc;				/* clock time */
  double	ct;				/* cpu time in seconds */
  double	rt = 0.0 ;           		/* real time in seconds */

#if	defined(__sysv__)
  struct timeval 	Tp;
  struct timezone	Tzp;
#elif	defined(__bsd__)
  struct timeb tr;				/* struct from ftime */
#else
#endif
  tc = clock( );				/* get clock time */
  ct = (double)(tc) / (double)CLOCKS_PER_SEC;	/* to seconds */
#if	defined(__sysv__)
  gettimeofday( &Tp, &Tzp );			/* get timeofday */
  rt = (double) Tp.tv_sec + 0.000001 * (double) Tp.tv_usec;
#elif	defined(__bsd__)
  ftime( &tr );				/* get real time */
  rt = (double) tr.time + 0.001 * (double) tr.millitm;	/* in seconds */
#else
#endif
  if (*mode) {					/* calculate difference */
    (*cpu_time)  = ct - (*cpu_time);		/* cpu time */
    (*real_time) = rt - (*real_time);		/* real time */
  } else {
    (*cpu_time)  = ct;			/* set cpu time */
    (*real_time) = rt;			/* set real time */
  }
}


//using namespace casa;

ASDM2MSFiller* msFiller;
string appName;
bool verbose = true;
bool isEVLA = false;

bool debug = (getenv("ASDM_DEBUG") != NULL);
//LogIO os;

#include <casa/Logging/StreamLogSink.h>
#include <casa/Logging/LogSink.h>

void info (const string& message) {
  
  if (!verbose){
    return;
  }
  LogSink::postGlobally(LogMessage(message, LogOrigin(appName,WHERE), LogMessage::NORMAL));
}

void error(const string& message) {
  LogSink::postGlobally(LogMessage(message, LogOrigin(appName,WHERE), LogMessage::NORMAL));
  //os << LogIO::POST;
  exit(1);
}

#include <iostream>
#include <sstream>

ostringstream errstream;
ostringstream infostream;
using namespace std;


//
// A class to describe Exceptions.
//
class ASDM2MSException {  
public:
  /**
   * An empty contructor.
   */
  ASDM2MSException();
  
  /**
   * A constructor with a message associated with the exception.
   * @param m a string containing the message.
   */
  ASDM2MSException(string m);
  
  /**
   * The destructor.
   */
  virtual ~ASDM2MSException();
  
  /**
   * Returns the message associated to this exception.
   * @return a string.
   */
  string getMessage() const;
  
protected:
  string message;
  
};

inline ASDM2MSException::ASDM2MSException() : message ("ASDM2MSException") {}
inline ASDM2MSException::ASDM2MSException(string m) : message(m) {}
inline ASDM2MSException::~ASDM2MSException() {}
inline string ASDM2MSException::getMessage() const {
  return "ASDM2MSException : " + message;
}

// A facility to get rid of blanks at start and end of a string.
// 
string lrtrim(std::string& s,const std::string& drop = " ")
{
  std::string r=s.erase(s.find_last_not_of(drop)+1);
  return r.erase(0,r.find_first_not_of(drop));
}




// These classes provide mappings from some ALMA Enumerations to their CASA counterparts.
class StokesMapper {
private :
  Stokes::StokesTypes* sa;

public :
  StokesMapper();
  ~StokesMapper();

  static Stokes::StokesTypes value(StokesParameterMod::StokesParameter s);
  Stokes::StokesTypes* to1DArray(const vector<StokesParameterMod::StokesParameter>& v);
  static vector<Stokes::StokesTypes> toVectorST(const vector<StokesParameterMod::StokesParameter>& v);
  static vector<int> toVectorI(const vector<StokesParameterMod::StokesParameter>& v);
}
  ;

StokesMapper::StokesMapper() {
  sa = 0;
}

StokesMapper::~StokesMapper() {
  if (sa)  delete[] sa;
}

Stokes::StokesTypes StokesMapper::value(StokesParameterMod::StokesParameter s) {
  switch (s) {
  case I : return Stokes::I; 
  case Q : return Stokes::Q; 
  case U : return Stokes::U; 
  case V : return Stokes::V; 
  case RR : return Stokes::RR; 
  case RL : return Stokes::RL; 
  case LR : return Stokes::LR; 
  case LL : return Stokes::LL; 
  case XX : return Stokes::XX; 
  case XY : return Stokes::XY; 
  case YX : return Stokes::YX; 
  case YY : return Stokes::YY; 
  case RX : return Stokes::RX; 
  case RY : return Stokes::RY; 
  case LX : return Stokes::LX; 
  case LY : return Stokes::LY; 
  case XR : return Stokes::XR; 
  case XL : return Stokes::XL; 
  case YR : return Stokes::YR; 
  case YL : return Stokes::YL; 
  case PP : return Stokes::PP; 
  case PQ : return Stokes::PQ; 
  case QP : return Stokes::QP; 
  case QQ : return Stokes::QQ; 
  case RCIRCULAR : return Stokes::RCircular; 
  case LCIRCULAR : return Stokes::LCircular; 
  case LINEAR : return Stokes::Linear; 
  case PTOTAL : return Stokes::Ptotal; 
  case PLINEAR : return Stokes::Plinear; 
  case PFTOTAL : return Stokes::PFtotal; 
  case PFLINEAR : return Stokes::PFlinear; 
  case PANGLE : return Stokes::Pangle;  
  }
  return Stokes::Undefined;
}

Stokes::StokesTypes* StokesMapper::to1DArray(const vector<StokesParameterMod::StokesParameter>& v) {
  if (v.size() == 0) return 0;

  if (sa) {
    delete[] sa;
    sa = 0;
  }

  sa = new Stokes::StokesTypes[v.size()];
  for (unsigned int i = 0; i < v.size(); i++) 
    sa[i] = value(v.at(i));

  return sa;
}

vector<int> StokesMapper::toVectorI(const vector<StokesParameterMod::StokesParameter>& v) {
  vector<int> result;

  for (unsigned int i = 0; i < v.size(); i++)
    result.push_back(value(v[i]));

  return result;
}

vector<Stokes::StokesTypes> StokesMapper::toVectorST(const vector<StokesParameterMod::StokesParameter>& v) {
  vector<Stokes::StokesTypes> result;

  for (unsigned int i = 0; i < v.size(); i++)
    result.push_back(value(v[i]));

  return result;
}

class FrequencyReferenceMapper {

public :
  FrequencyReferenceMapper();
  ~FrequencyReferenceMapper();

  static MFrequency::Types value(FrequencyReferenceCodeMod::FrequencyReferenceCode frc);
}
  ;

FrequencyReferenceMapper::FrequencyReferenceMapper() {
  ;
}

FrequencyReferenceMapper::~FrequencyReferenceMapper() {
  ;
}

MFrequency::Types FrequencyReferenceMapper::value(FrequencyReferenceCodeMod::FrequencyReferenceCode frc) {
  switch (frc) {
  case FrequencyReferenceCodeMod::LABREST : 
    errstream.str("");
    errstream << " Can't map FrequencyReferenceCode::LABREST to an MFrequency::Types" << endl;
    error(errstream.str());
    break;
    
  case FrequencyReferenceCodeMod::LSRD : return MFrequency::LSRD; 
  case FrequencyReferenceCodeMod::LSRK : return MFrequency::LSRK; 
  case FrequencyReferenceCodeMod::BARY : return MFrequency::BARY;
  case FrequencyReferenceCodeMod::REST : return MFrequency::REST;
  case FrequencyReferenceCodeMod::GEO  : return MFrequency::GEO; 
  case FrequencyReferenceCodeMod::GALACTO : return MFrequency::GALACTO;
  case FrequencyReferenceCodeMod::TOPO : return MFrequency::TOPO; 
  }
  return MFrequency::TOPO; // Never happens.
}

class PolTypeMapper {
private :
  vector<string> polType;

public :
  PolTypeMapper();
  ~PolTypeMapper();

  static char value(PolarizationTypeMod::PolarizationType p);
  vector<string> toStringVector(const vector<PolarizationTypeMod::PolarizationType>& v);
};


PolTypeMapper::PolTypeMapper() {
  ;
}

PolTypeMapper::~PolTypeMapper() {
  ;
}

char PolTypeMapper::value(PolarizationTypeMod::PolarizationType p) {
  return (CPolarizationType::name(p)).at(0);
}

vector<string> PolTypeMapper::toStringVector(const vector<PolarizationTypeMod::PolarizationType>& v) {
  polType.clear();
  for (unsigned int i = 0; i < v.size(); i++) {
    polType.push_back((CPolarizationType::name(v.at(i))));
  }
  return polType;
}


// These classes provide methods to convert from vectors of "anything" into vectors of basic types.
class FConverter {
public:

  static vector<float> toVectorF(const vector<vector<float> >& vvF, bool tranpose=false);

  template<class T>
  static vector<float> toVectorF(const vector<vector<T> >& vvT, bool transpose=false) {
    vector<float> result;

    if (transpose == false) {
      //
      // Simply linearize the vector of vectors.
      //
      for (unsigned int i = 0; i < vvT.size(); i++)
	for (unsigned int j = 0; j < vvT.at(i).size(); j++)
	  result.push_back(vvT.at(i).at(j).get());
    }
    else {
      //
      // We want to transpose.
      // Let's consider the very general case where our vector of vectors does not represent
      // a rectangular matrix, i.e. all the elements of this vector are vectors with possibly
      // different size. 
      //
      unsigned int maxsize = 0;
      unsigned int cursize = 0;
      for (unsigned int i = 0; i < vvT.size(); i++) {
	cursize = vvT.at(i).size();
	if (cursize > maxsize) maxsize = cursize;
      }
      
      for (unsigned int i = 0; i < maxsize; i++)
	for (unsigned int j = 0; j < vvT.size(); j++)
	  if (i < vvT.at(j).size())
	    result.push_back(vvT.at(j).at(i).get());     
    }
    return result;
  }
};

float d2f(double d) { return (float) d; }

vector<float> FConverter::toVectorF(const vector< vector <float> > & vvF, bool transpose) {
  vector<float> result;
  if (transpose == false) {
    //
    // Simply linearize the vector of vectors.
    //
    for (unsigned int i = 0; i < vvF.size(); i++)
      for (unsigned int j = 0; j < vvF.at(i).size(); j++)
	result.push_back(vvF.at(i).at(j));
  }
  else {
    //
    // We want to transpose.
    // Let's consider the very general case where our vector of vectors does not represent
    // a rectangular matrix, i.e. all the elements of this vector are vectors with possibly
    // different size. 
    //
    unsigned int maxsize = 0;
    unsigned int cursize = 0;
    for (unsigned int i = 0; i < vvF.size(); i++) {
      cursize = vvF.at(i).size();
      if (cursize > maxsize) maxsize = cursize;
    }
    
    for (unsigned int i = 0; i < maxsize; i++)
      for (unsigned int j = 0; j < vvF.size(); j++)
	if (i < vvF.at(j).size())
	  result.push_back(vvF.at(j).at(i));     
  }
  
  return result;
}   

class DConverter {
  
public :

  static vector<double> toVectorD(const vector<vector<double> >& vv);
  template<class T>

  static vector<double> toVectorD(const vector<T>& v) {
    vector<double> result;
    for (typename vector<T>::const_iterator iter = v.begin(); iter != v.end(); ++iter)
      result.push_back(iter->get());
    return result;
  }

  template<class T>
  static vector<double> toVectorD(const vector<vector<T> >& vv) {
    vector<double> result;
    for (typename vector<vector<T> >::const_iterator iter = vv.begin(); iter != vv.end(); ++iter)
      for (typename vector<T>::const_iterator iiter = iter->begin(); iiter != iter->end(); ++iiter)
	result.push_back(iiter->get());
    return result;
  }
};

vector<double> DConverter::toVectorD(const vector<vector<double> >& vv) {
  vector<double> result;
  
  for (vector<vector<double> >::const_iterator iter = vv.begin(); iter != vv.end(); ++iter)
    result.insert(result.end(), iter->begin(), iter->end());

  return result;
}

class IConverter {
  
public :
  vector<int> static toVectorI(vector<Tag>& v);
};

vector<int> IConverter::toVectorI(vector<Tag>& v) {
  vector<int> result(v.size());
  vector<Tag>::const_iterator iiter = v.begin();
  vector<int>::iterator oiter = result.begin();
  for (; iiter != v.end(); ++iiter, ++oiter)
    *oiter = iiter->getTagValue();

  return result;
}

class SConverter {
public :
  template<class Enum, class EnumHelper>
  static vector<string> toVectorS(const vector<Enum>& vEnum) {
    vector<string> result(vEnum.size());
    typename vector<Enum>::const_iterator iiter = vEnum.begin();
    vector<string>::iterator     oiter = result.begin();

    for (; iiter != vEnum.begin(); ++iiter, ++oiter)
      *oiter = EnumHelper::name(*iiter);

    return result;
  }
};

class  CConverter {
public :
  static vector<std::complex<float> > toVectorCF(const vector<vector<asdm::Complex> >& vv);
};

vector<std::complex<float> > CConverter::toVectorCF(const vector<vector<asdm::Complex> >& vv) {
  vector<std::complex<float> > result;

  for (vector<vector<asdm::Complex> >::const_iterator iter = vv.begin(); iter != vv.end(); ++iter) 
    result.insert(result.end(), iter->begin(), iter->end());

  return result;
}

class ComplexDataFilter {

public:
  ComplexDataFilter();
  virtual ~ComplexDataFilter();
  virtual float* to4Pol(int numPol, int numChan, float* cdata);

private:
  vector<float *> storage; 
};

ComplexDataFilter::ComplexDataFilter() {
  ;
}

ComplexDataFilter::~ComplexDataFilter() {
  for (unsigned int i = 0; i < storage.size(); i++) 
    delete[] storage.at(i);
}

float *ComplexDataFilter::to4Pol(int numCorr, int numChan, float* cdata) {
  // Do nothing if numCorr != 3
  if (numCorr != 3) return cdata;

  
  // Increase the storage size by appending nul chars.
  float* filtered = new float[ 2 * 4 * numChan ];

  storage.push_back(filtered);
  
  for (int i = 0; i < numChan; i++) {
    // The 1st row goes to the first row.
    filtered[ 8 * i ]     = cdata[ 6 * i ] ;
    filtered[ 8 * i + 1 ] = cdata[ 6 * i + 1 ] ;

    // The second row goes the second row.
    filtered [ 8 * i + 2 ] = cdata[ 6 * i + 2 ] ;
    filtered [ 8 * i + 3 ] = cdata[ 6 * i + 3 ] ;

    // The second row's conjugate goes to the third row.
    filtered [ 8 * i + 4 ] = cdata[ 6 * i + 2 ] ;
    filtered [ 8 * i + 5 ] = - cdata[ 6 * i + 3 ] ;

    // The third row goes to the third row.
    filtered [ 8 * i + 6 ] = cdata[ 6 * i + 4 ] ;
    filtered [ 8 * i + 7 ] = cdata[ 6 * i + 5 ] ;    
  }

  return filtered;
}

//
//  A collection of functions to combine target and offset
//  into a pointing direction in the Pointing table.
//
/**
 *
 * From a Fortran 90 subroutine, written by Didier Despois (1980)
 * and revised by Michel Perault (1984).
 *
 * Computes a coodinates conversion matrix for a rotation defined
 * by Euler angles psi, the and phi (values in radians).
 *
 * The product of the euclidean coordinates of a vector in the original
 * base by this matrix gives the coordinates in the rotated base.
 * 
 */
void eulmat(double psi, double the, double phi, vector<vector<double> >& mat) {
  double cpsi, spsi, cthe, sthe, cphi, sphi;
  double x1, x2;

  cpsi = cos(psi);
  spsi = sin(psi);
  cthe = cos(the);
  sthe = sin(the);
  cphi = cos(phi);
  sphi = sin(phi);

  x1 = spsi*cthe;
  x2 = cpsi * cthe; 
  mat.at(0).at(0) =  cpsi * cphi - x1 * sphi;
  mat.at(0).at(1) =  spsi * cphi + x2 * sphi;
  mat.at(0).at(2) =  sthe * sphi;

  mat.at(1).at(0) = -cpsi * sphi - x1 * cphi;
  mat.at(1).at(1) = -spsi * sphi + x2 * cphi;
  mat.at(1).at(2) = sthe * cphi;

  mat.at(2).at(0) =  spsi * sthe;
  mat.at(2).at(1) =  -cpsi * sthe;
  mat.at(2).at(2) =  cthe;
}

/**
 * Performs a product Matrix x Vector.
 *
 * Attention ! vFactor , mFactor and vResult must be correctly
 * sized ! And also vFactor and vResult must be different objects.
 */
void matvec (const vector<vector<double> >& mFactor, const vector<double>& vFactor, vector<double>& vResult) {
  vResult.at(0) = vFactor.at(0)*mFactor.at(0).at(0) + vFactor.at(1)*mFactor.at(0).at(1) + vFactor.at(2)*mFactor.at(0).at(2);
  vResult.at(1) = vFactor.at(0)*mFactor.at(1).at(0) + vFactor.at(1)*mFactor.at(1).at(1) + vFactor.at(2)*mFactor.at(1).at(2);
  vResult.at(2) = vFactor.at(0)*mFactor.at(2).at(0) + vFactor.at(1)*mFactor.at(2).at(1) + vFactor.at(2)*mFactor.at(2).at(2);
}

/**
 * Spherical to cartesian conversion.
 * Radius assumed to be equal to 1.
 *
 * Attention ! a and x must be correctly sized !
 */
void rect(const vector<double>& s, vector<double>& x) {
  x.at(0) = cos(s.at(0)) * cos(s.at(1));
  x.at(1) = sin(s.at(0)) * cos(s.at(1));
  x.at(2) = sin(s.at(1));
}

/**
 * Cartesian to spherical conversion.
 * Radius assumed to be equal to 1.
 *
 * Attention ! a and x must be correctly sized !
 */
void spher(const vector<double>& x, vector<double>& s) {
  s.at(0) = atan2(x.at(1) , x.at(0));
  s.at(1) = atan2(x.at(2), sqrt(x.at(0)*x.at(0) + x.at(1)*x.at(1)));
}

/**
 * Reorder the collection of spectral windows ids.
 *
 * Given a dataset 'ds', the statement 'ds.spectralWindowTable.get()' returns
 * a vector of pointers on instances of 'SpectralWindowRow' and 
 * defines implicitely an ordered collection of Tag (of type TagType::SpectralWindow) instances
 * obtained by using 'getSpectralWindowId()' on each instance.
 *
 * This method partitions this ordered collection into two collections
 * based on the criterium : does the Tag appear in at least one row
 * of the DataDescription table in the SpectralWindowId attribute or not.
 *
 * The methods returns a vector of (SpactralWindow) Tag  obtained by appending the collections
 * of (SpectralWindow) Tag which appear in the DataDescription table
 * followed by the collection of (SpectralWindow) Tag which does not appear
 * in the DataDescription table. The int value which associated with a Tag
 * is the order number of the insertion of the pair (Tag, int) in the map.
 * 
 */

struct TagCmp {
  bool operator() (const Tag& t1, const Tag& t2) const {
    return t1.getTagValue() < t2.getTagValue();
  }
};

vector<Tag> reorderSwIds(const ASDM& ds) {
  vector<SpectralWindowRow *> swRs = ds.getSpectralWindow().get();
  vector<DataDescriptionRow *> ddRs = ds.getDataDescription().get();
  map<Tag, bool, TagCmp> isInDD;

  for (vector<SpectralWindowRow *>::size_type  i = 0; i < swRs.size(); i++) isInDD[swRs[i]->getSpectralWindowId()] = false;
  for (vector<DataDescriptionRow *>::size_type i = 0; i < ddRs.size(); i++) isInDD[ddRs[i]->getSpectralWindowId()] = true;

  vector<Tag> swIdsDD, swIdsNoDD;
  for (map<Tag, bool, TagCmp>::iterator iter = isInDD.begin(); iter != isInDD.end(); ++iter)
    if (iter->second) swIdsDD.push_back(iter->first);
    else swIdsNoDD.push_back(iter->first);

  vector<Tag> result (swIdsDD.begin(), swIdsDD.end());
  for (vector<Tag>::size_type i = 0; i < swIdsNoDD.size(); i++)
    result.push_back(swIdsNoDD[i]);

  return result;
}
  
void usage(char* command) {
  cout << "Usage : " << command << " DataSetName" << endl;
}

template<class T>
void checkVectorSize(const string& vectorAttrName,
		     const vector<T>& vectorAttr,
		     const string& sizeAttrName,
		     unsigned int sizeAttr,
		     const string& tableName,
		     unsigned int rowNumber) {
  if (vectorAttr.size() != sizeAttr) {
    errstream.str("");
    errstream << "In the '"
	      << tableName 
	      << " table, at row #"
	      << rowNumber
	      << ", I found '"
	      << vectorAttrName
	      << "' with a size of '"
	      << vectorAttr.size() 
	      << "', I was expecting it to be equal to the size of '"
	      << sizeAttrName
	      <<"' which is '"
	      <<"'"
	      <<sizeAttr
	      <<"'. I can't go further."
	      << endl;
    error(errstream.str());
  }
}

EnumSet<AtmPhaseCorrection> apcLiterals(const ASDM& ds, const string& asdmBinaryPath) {
  EnumSet<AtmPhaseCorrection> result;

  vector<MainRow *> mRs = ds.getMain().get();
  
  for (unsigned int i = 0; i < mRs.size(); i++) {
    ConfigDescriptionRow * configDescriptionRow = mRs.at(i)->getConfigDescriptionUsingConfigDescriptionId();
    vector<AtmPhaseCorrection> apc = configDescriptionRow -> getAtmPhaseCorrection();
    for (unsigned int i = 0; i < apc.size(); i++)
      result.set(apc.at(i));
  }
  return result;
}

bool hasCorrectedData(const EnumSet<AtmPhaseCorrection>& es) {
  return es[AP_CORRECTED];
}

bool hasUncorrectedData(const EnumSet<AtmPhaseCorrection>& es) {
  return es[AP_UNCORRECTED];
}
		    
//
// A number of EnumSet to encode the different selection criteria.
//
EnumSet<CorrelationMode>         es_cm;
EnumSet<SpectralResolutionType>  es_srt;
EnumSet<TimeSampling>            es_ts;
Enum<CorrelationMode>            e_query_cm; 
EnumSet<AtmPhaseCorrection>      es_query_apc;    

// An EnumSet to store the different values of AtmPhaseCorrection present
// in the binary data (apc in datastruct).
//
EnumSet<AtmPhaseCorrection>      es_apc;

//
// By default the resulting MS will not contain compressed columns
// unless the 'compress' option has been given.
// 
bool                             withCompression = false;

//
// A function to determine if overTheTop is present in a given row of the Pointing table.
//
bool overTheTopExists(PointingRow* row) { return row->isOverTheTopExists(); }

//
// A collection of declarations and functions used for the parsing of the 'scans' option.
//
#include <boost/spirit.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
using namespace boost::spirit;

vector<int> eb_v;
int allEbs = -1;
int ebNumber = allEbs;
int readEb = allEbs;

set<int> scan_s;
int scanNumber0, scanNumber1;

map<int, set<int> > eb_scan_m;

template<typename T>
string displaySet(const set<T> &aSet) {
  ostringstream oss;
  oss << "{";
  typename set<T>::const_iterator iter = aSet.begin();
  if (iter != aSet.end())
    oss << *iter++;
   
  while (iter != aSet.end())
    oss << "," << *iter++;  
  oss<< "}";
  return oss.str();
}

/*
** Inserts all the integer values in the range [scanNumber0, scanNumber1] in the set referred 
** to by the global variable scan_s.
** The two parameters begin and end are not used and here only to comply with the Spirit parser's convention.
**
*/
void fillScanSet(const char* begin, const char* end) {
  for (int i = scanNumber0; i < (scanNumber1+1); i++)
    scan_s.insert(i);
}

/*
** Inserts all the elements of the set referred to by the global variable scan_s into
** the set associated with the (int) key equal to the last value of the global vector eb_v
** in the global map eb_scan_m.
** The two parameters begin and end are not used and here only to comply with the Spirit parser's convention.
*/  
void mergeScanSet(const char* begin, const char* end) {
  int key = eb_v.back();
  eb_scan_m[key].insert(scan_s.begin(), scan_s.end());
}

/*
** Empties the global set scan_s.
** The two parameters begin and end are not used and here only to comply with the Spirit parser's convention.
*/
void clearScanSet(const char* begin, const char* end) {
  scan_s.clear();
}

/*
** Defines the grammar and the behaviour of a Spirit parser
** able to process a scan selection.
*/
struct eb_scan_selection : public grammar<eb_scan_selection> {
  template<typename ScannerT> struct definition {
    definition (eb_scan_selection const& self) {
      eb_scan_list   = eb_scan >> *(';' >> eb_scan);
      eb_scan	     = (eb[push_back_a(eb_v, ebNumber)][assign_a(ebNumber, allEbs)] >> scan_list)[&mergeScanSet][&clearScanSet];
      eb	     = !(int_p[assign_a(readEb)] >> ':')[assign_a(ebNumber, readEb)];
      scan_list	     = !((scan_selection >> *(',' >> scan_selection)));
      scan_selection = (int_p[assign_a(scanNumber0)][assign_a(scanNumber1)] >> !('~' >> int_p[assign_a(scanNumber1)]))
	[&fillScanSet]
	[assign_a(scanNumber0, -1)]
	[assign_a(scanNumber1, -1)];
    }
    rule<ScannerT> eb_scan_list, eb_scan, eb, scan_list, scan_selection;
    rule<ScannerT> const& start() const { return eb_scan_list ; }
  };
};

/*
** Returns the intersection of two sets.
**
** @param T the base type of the two sets.
** @param s1 the first set.
** @param s2 the second set.
** @return a set equal to the intersection of s1 and s2.
*/
template<typename T> 
set<T> SetAndSet(const set<T>& s1, const set<T>& s2) {
  set<T> result;
  typename set<T>::iterator iter1_s, iter2_s;
  for (iter1_s = s1.begin(); iter1_s != s1.end(); iter1_s++) {
    if ((iter2_s = s2.find(*iter1_s)) != s2.end())
      result.insert(*iter1_s);
  }
  return result;
}

//
// A template function which returns true if and only there is at least
// one element in the vector 'scans' for which the time interval
// defined by its attributes startTime and endTime has a non empty
// intersection with the time interval defined by the 'timeInterval'
// attribute of the generic parameter of 'row' which is expected to 
// have a method getTimeInterval which returns a TimeInterval object.
//
template<typename T>
bool timeIntervalIntersectsAScan (T* row, const vector<ScanRow *>& scans) {
  bool result = false;

  int64_t currentScanStartTime, currentScanEndTime;
  int64_t rowStartTime, rowEndTime;
  for (vector<ScanRow *>::const_iterator iter = scans.begin(); iter != scans.end(); iter++) {
    currentScanStartTime = (*iter)->getStartTime().get();
    currentScanEndTime = (*iter)->getEndTime().get();

    rowStartTime = row->getTimeInterval().getStart().get();
    rowEndTime = rowStartTime + row->getTimeInterval().getDuration().get();
    if (max(currentScanStartTime, rowStartTime) < min(currentScanEndTime, rowEndTime))
      return true;
  }
  return result;
}

template<typename T>
struct rowsInAScanbyTimeIntervalFunctor {
private:
  const vector<ScanRow *>&	scans;
  vector<T *>			result;

public:
  rowsInAScanbyTimeIntervalFunctor(const vector<ScanRow *>& scans): scans(scans) {};
  const vector<T *> & operator() (const vector<T *>& rows, bool ignoreTime=false) {
    if (ignoreTime) return rows;

    for (typename vector<T *>::const_iterator iter = rows.begin(); iter != rows.end(); iter++) {
      if (timeIntervalIntersectsAScan (*iter, scans))
	result.push_back(*iter);
    }
    return result;    
  }
};

//
// A template function which calls the template function timeIntervalIntersectsAScan for each
// element of 'rows' by using the parameter 'scans' to determine if there is at least an intersection.
// It returns a vector of T* containing copies of elements of 'rows' for which the function timeIntervalIntersectsAScan.
// returns true.
//
template<typename T>
vector<T *> rowsInAScanbyTimeInterval(const vector<T* >& rows, const vector<ScanRow *>& scans, bool midpoint) {
  vector<T *> result ;
  for (typename vector<T *>::const_iterator iter = rows.begin(); iter != rows.end(); iter++) {
    if (timeIntervalIntersectsAScan (*iter, scans, midpoint))
      result.push_back(*iter);
  }
  return result;
}

//
// A template function which checks if there is at least one element scan of the vector scans for which
// the time  contained by returned by row->getTime() is embedded in the time range defined in scan. 
// Returns true there is such a scan.
//
template<typename T>
bool timeIsInAScan(T* row, const vector<ScanRow *>& scans, bool midpoint) {
  bool result = false;

  int64_t currentScanStartTime, currentScanEndTime;
  int64_t rowTime;
  rowTime = row->getTime().get();
  for (vector<ScanRow *>::const_iterator iter = scans.begin(); iter != scans.end(); iter++) {
    currentScanStartTime = (*iter)->getStartTime().get();
    currentScanEndTime = (*iter)->getEndTime().get();
    if ((currentScanStartTime <= rowTime) && (rowTime < currentScanEndTime))
      return true;
  }
  return result;
}

template<typename T>
struct rowsInAScanbyTimeFunctor {
private:
  const vector<ScanRow *>&	scans;
  bool				midpoint;
  vector<T *>			result;

public:
  rowsInAScanbyTimeFunctor(const vector<ScanRow *>& scans, bool midpoint): scans(scans), midpoint(midpoint) {};
  const vector<T *> & operator() (const vector<T *>& rows, bool ignoreTime=false) {
    if (ignoreTime) return rows;

    for (typename vector<T *>::const_iterator iter = rows.begin(); iter != rows.end(); iter++) {
      if (timeIsInAScan (*iter, scans, midpoint))
	result.push_back(*iter);
    }

    return result;    
  }
};

//
// A template function which calls the template function timeIsInAScan for each
// element of 'rows' by using the parameter 'scans' to determine if there is at 
// least a scan which contains the 'time' attribute's value for each row.
// It returns a vector of T* containing copies of elements of 'rows' for which the function 
// timeIsInAScan returns true.
//
//
template<typename T>
vector<T *> rowsInAScanbyTime(const vector<T* >& rows, const vector<ScanRow *>& scans, bool midpoint) {
  vector<T *> result ;
  for (typename vector<T *>::const_iterator iter = rows.begin(); iter != rows.end(); iter++) {
    if (timeIsInAScan (*iter, scans, midpoint))
      result.push_back(*iter);
  }
  return result;
}

//
// A boolean template functor which returns the value of the expression x.size() < y.
template<typename T>
struct size_lt {
public: 
  size_lt(unsigned int y) : y(y) {}
  bool operator()(vector<T>& x) {return x.size() < y;}

private:
  unsigned int  y;
};

//
// A template function which returns a string from an (expectedly) enumeration and its associated 
// helper class.
template<typename Enum, typename CEnum> 
string stringValue(Enum literal) {
  return CEnum::name(literal);
}

//
// template function meant to return a value of type BasicType out of a value expected to be of one of the Physical Quantities type (Pressure, Speed ....)
template<typename PhysicalQuantity, typename BasicType> 
BasicType basicTypeValue (PhysicalQuantity value) {
  return (BasicType) value.get();
}

map<int, int> swIdx2Idx ;                       // A map which associates old and new index of Spectral Windows before/after reordering.

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Some functions defined for the processing of the SysPower table 
// with 'functional programming' techniques.
//
int sysPowerAntennaId(SysPowerRow* row) {
  return row->getAntennaId().getTagValue();
}

int sysPowerSpectralWindowId(const SysPowerRow* row) {
  return swIdx2Idx[row->getSpectralWindowId().getTagValue()];
}

int sysPowerFeedId(const SysPowerRow* row) {
  return row->getFeedId();
}

double sysPowerMidTimeInSeconds(const SysPowerRow* row) {
  //if (isEVLA) {
  //  return row->getTimeInterval().getStartInMJD()*86400 ; 
  //}
  return row->getTimeInterval().getStartInMJD()*86400 + ((double) row->getTimeInterval().getDuration().get()) / ArrayTime::unitsInASecond / 2.;
}

double sysPowerIntervalInSeconds(const SysPowerRow* row) {
  return ((double) row->getTimeInterval().getDuration().get()) / ArrayTime::unitsInASecond ;
}

int sysPowerNumReceptor(const SysPowerRow* row) {
  return row->getNumReceptor();
}

struct sysPowerCheckConstantNumReceptor {
private:
  unsigned int numReceptor;

public:
  sysPowerCheckConstantNumReceptor(unsigned int numReceptor) : numReceptor(numReceptor) {}
  bool operator()(SysPowerRow* row) {
    return numReceptor != (unsigned int) row->getNumReceptor();
  }
};

struct sysPowerCheckSwitchedPowerDifference {
private:
  unsigned int	numReceptor;
  bool		exists;

public:
  sysPowerCheckSwitchedPowerDifference(unsigned int numReceptor, bool exists) : numReceptor(numReceptor), exists(exists) {}
  bool operator()(SysPowerRow* row) {
    return (exists != row->isSwitchedPowerDifferenceExists()) || (row->getSwitchedPowerDifference().size() != numReceptor);
  }
};

struct sysPowerCheckSwitchedPowerSum {
private:
  unsigned int	numReceptor;
  bool		exists;

public:
  sysPowerCheckSwitchedPowerSum(unsigned int numReceptor, bool exists) : numReceptor(numReceptor), exists(exists) {}
  bool operator()(SysPowerRow* row) {
    return (exists != row->isSwitchedPowerSumExists()) || (row->getSwitchedPowerSum().size() != numReceptor);
  }
};

struct sysPowerCheckRequantizerGain {
private:
  unsigned int	numReceptor;
  bool		exists;

public:
  sysPowerCheckRequantizerGain(unsigned int numReceptor, bool exists) : numReceptor(numReceptor), exists(exists) {}
  bool operator()(SysPowerRow* row) {
    return (exists != row->isRequantizerGainExists()) || (row->getRequantizerGain().size() != numReceptor);
  }
};

struct sysPowerSwitchedPowerDifference {
private:
  vector<float>::iterator iter;
  
public:
  sysPowerSwitchedPowerDifference(vector<float>::iterator iter): iter(iter) {}
  void operator()(SysPowerRow* row) { 
    vector<float> tmp = row->getSwitchedPowerDifference();
    copy(tmp.begin(), tmp.end(), iter);
    iter += tmp.size();
  }
};

struct sysPowerSwitchedPowerSum {
private:
  vector<float>::iterator iter;
  
public:
  sysPowerSwitchedPowerSum(vector<float>::iterator iter): iter(iter) {}
  void operator()(SysPowerRow* row) { 
    vector<float> tmp = row->getSwitchedPowerSum();
    copy(tmp.begin(), tmp.end(), iter);
    iter += tmp.size();
  }
};


struct sysPowerRequantizerGain {
private:
  vector<float>::iterator iter;

public:
  sysPowerRequantizerGain(vector<float>::iterator iter): iter(iter) {}
  void operator()(SysPowerRow* row) { 
    vector<float> tmp = row->getRequantizerGain();
    copy(tmp.begin(), tmp.end(), iter);
    iter += tmp.size();
  }
};


/**
 * This function tries to calculate the sizes of the successives slices of the BDF 
 * to be processed given a) the overall size of the BDF and b) the approximative maximum
 * size that one wants for one slice.
 * @paramater BDFsize the overall size of the BDF expressed in bytes,
 * @parameter approxSizeInMemory the approximative size that one wants for one slice, expressed in byte.
 *
 * \par Note:
 * It tries to avoid a last slice, corresponding to 
 * the remainder in the euclidean division BDFsize / approxSizeInMemory.
 */
vector<uint64_t> sizeInMemory(uint64_t BDFsize, uint64_t approxSizeInMemory) {
  if (debug) cout << "sizeInMemory: entering" << endl;
  vector<uint64_t> result;
  uint64_t Q = BDFsize / approxSizeInMemory;
  if (Q == 0) { 
    result.push_back(BDFsize);
  }
  else {
    result.resize(Q, approxSizeInMemory);
    unsigned int R = BDFsize % approxSizeInMemory;
    if ( R > (Q * approxSizeInMemory / 5) )  {
      result.push_back(R); 
    }
    else {
      while (R > 0) 
	for (unsigned int i = 0; R >0 && i < result.size(); i++) {
	  result[i]++ ; R--;
	}
    }
  }
  if (debug) cout << "sizeInMemory: exiting" << endl;
  return result;
} 

map<AtmPhaseCorrection, ASDM2MSFiller*> msFillers; // There will be one filler per value of the axis APC.

vector<int>	dataDescriptionIdx2Idx;
int		ddIdx;
map<MainRow*, int>     stateIdx2Idx;

/**
 * This function fills the MS State table.
 * given :
 * @parameter r_p a pointer on the row of the ASDM Main table being processed.
 * @parameter vmsData_p a pointer on a VMSData structure containing the data of the last slice of BDF being processed.
 *
 */

void fillState(MainRow* r_p) {
  if (debug) cout << "fillState : entering" << endl;

  ASDM&			ds	   = r_p -> getTable() . getContainer();
  ScanRow*		scanR_p	   = ds.getScan().getRowByKey(r_p -> getExecBlockId(),	r_p -> getScanNumber());
  vector<ScanIntent>	scanIntent = scanR_p -> getScanIntent();
  SubscanRow*		sscanR_p   = ds.getSubscan().getRowByKey(r_p -> getExecBlockId(),
								 r_p -> getScanNumber(),
								 r_p -> getSubscanNumber());
  if (sscanR_p == 0) {
    errstream.str("");
    errstream << "Could not find a row in the Subscan table for the following key value (execBlockId=" << r_p->getExecBlockId().toString()
	      <<", scanNumber="<< r_p->getScanNumber()
	      <<", subscanNum=" << r_p->getSubscanNumber() << "). Aborting. "
	      << endl;
    error(errstream.str());
  }	  

  SubscanIntent subscanIntent = sscanR_p->getSubscanIntent();
  string obs_mode;
  if (scanIntent.size() > 0) {
    obs_mode = CScanIntent::name(scanIntent.at(0))+"#"+CSubscanIntent::name(subscanIntent);
   
    for (unsigned int iScanIntent = 1; iScanIntent < scanIntent.size(); iScanIntent++) {
      obs_mode += ",";
      obs_mode +=  CScanIntent::name(scanIntent.at(iScanIntent))+"#"+CSubscanIntent::name(subscanIntent);
    }
  }

  const vector<StateRow *>& sRs =  ds.getState().get() ;
  for (unsigned int iState = 0; iState < sRs.size(); iState++) {							     	    
    bool pushed = false;
    
    for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	 iter != msFillers.end();
	 ++iter) {
      int retId = iter->second->addUniqueState(sRs[iState]->getSig(),
					       sRs[iState]->getRef(),
					       0.0, 
					       0.0, 
					       r_p->getSubscanNumber(),
					       obs_mode, 
					       false);
      if (!pushed) {
	stateIdx2Idx[r_p] = retId;
	pushed = true;
      }
    }	    
  }
  if (debug) cout << "fillState : exiting" << endl;
}

/**
 * This function fills the MS Main table from an ASDM Main table which refers to correlator data.
 *
 * given:
 * @parameter rowNum an integer expected to contain the number of the row being processed.
 * @parameter r_p a pointer to the MainRow being processed.
 * @parameter sdmBinData a reference to the SDMBinData containing a lot of information about the binary data being processed. Useful to know the requested ordering of data.
 * @parameter uvwCoords a reference to the UVW calculator.
 * @parameter complexData a bool which says if the DATA is going to be filled (true) or if it will be the FLOAT_DATA (false).
 * @parameter mute if the value of this parameter is false then nothing is written in the MS .
 *
 * !!!!! One must be carefull to the fact that fillState must have been called before fillMain. During the execution of fillState , the global vector<int> msStateID
 * is filled and will be used by fillMain.
 */ 
void fillMain(int		rowNum,
	      MainRow*	r_p,
	      SDMBinData&	sdmBinData,
	      const VMSData* vmsData_p,
	      UvwCoords&	uvwCoords,
	      bool		complexData,
	      bool           mute) {
  
  if (debug) cout << "fillMain : entering" << endl;

  ASDM & ds = r_p -> getTable() . getContainer();

  // Then populate the Main table.
  ComplexDataFilter filter; // To process the case numCorr == 3
  
  if (vmsData_p->v_antennaId1.size() == 0) {
    infostream.str("");
    infostream << "No MS data produced for the current row." << endl;
    info(infostream.str());
    return;
  }

  vector<vector<unsigned int> > filteredShape = vmsData_p->vv_dataShape;
  for (unsigned int ipart = 0; ipart < vmsData_p->vv_dataShape.size(); ipart++) {
    if (filteredShape.at(ipart).at(0) == 3)
      filteredShape.at(ipart).at(0) = 4;
  }
	  
  vector<int> filteredDD;
  for (unsigned int idd = 0; idd < vmsData_p->v_dataDescId.size(); idd++)
    filteredDD.push_back(dataDescriptionIdx2Idx.at(vmsData_p->v_dataDescId.at(idd)));
  vector<float *> uncorrectedData;
  vector<float *> correctedData;

  /* compute the UVW */
  vector<double> uvw(3*vmsData_p->v_time.size());
	  
  vector<casa::Vector<casa::Double> > vv_uvw;
#if DDPRIORITY
  uvwCoords.uvw_bl(r_p, sdmBinData.timeSequence(), e_query_cm, 
		   sdmbin::SDMBinData::dataOrder(),
		   vv_uvw);
#else
  uvwCoords.uvw_bl(r, vmsData_p->v_timeCentroid, e_query_cm, 
		   sdmbin::SDMBinData::dataOrder(),
		   vv_uvw);
#endif
  int k = 0;
  for (unsigned int iUvw = 0; iUvw < vv_uvw.size(); iUvw++) {
    uvw[k++] = vv_uvw[iUvw](0); 
    uvw[k++] = vv_uvw[iUvw](1);
    uvw[k++] = vv_uvw[iUvw](2);
  } 

  // Here we make the assumption that the State is the same for all the antennas and let's use the first State found in the vector stateId contained in the ASDM Main Row
  int asdmStateIdx = r_p->getStateId().at(0).getTagValue();  
  vector<int> msStateId(vmsData_p->v_m_data.size(), stateIdx2Idx[r_p]);

  ComplexDataFilter cdf;
  map<AtmPhaseCorrectionMod::AtmPhaseCorrection, float*>::const_iterator iter;

  vector<double>	correctedTime;
  vector<int>		correctedAntennaId1;
  vector<int>		correctedAntennaId2;
  vector<int>		correctedFeedId1;
  vector<int>		correctedFeedId2;
  vector<int>		correctedFieldId;
  vector<int>           correctedFilteredDD;
  vector<double>	correctedInterval;
  vector<double>	correctedExposure;
  vector<double>	correctedTimeCentroid;
  vector<int>		correctedMsStateId(msStateId);
  vector<double>	correctedUvw ;
  vector<unsigned int>	correctedFlag;

  for (unsigned int iData = 0; iData < vmsData_p->v_m_data.size(); iData++) {

    if ((msFillers.find(AP_UNCORRECTED) != msFillers.end()) &&
	(iter=vmsData_p->v_m_data.at(iData).find(AtmPhaseCorrectionMod::AP_UNCORRECTED)) != vmsData_p->v_m_data.at(iData).end()){
      uncorrectedData.push_back(cdf.to4Pol(vmsData_p->vv_dataShape.at(iData).at(0),
					   vmsData_p->vv_dataShape.at(iData).at(1),
					   iter->second));
    }
	    
    if ((msFillers.find(AP_CORRECTED) != msFillers.end()) &&
	(iter=vmsData_p->v_m_data.at(iData).find(AtmPhaseCorrectionMod::AP_CORRECTED)) != vmsData_p->v_m_data.at(iData).end()){
      correctedTime.push_back(vmsData_p->v_time.at(iData));
      correctedAntennaId1.push_back(vmsData_p->v_antennaId1.at(iData));
      correctedAntennaId2.push_back(vmsData_p->v_antennaId2.at(iData));
      correctedFeedId1.push_back(vmsData_p->v_feedId1.at(iData));
      correctedFeedId2.push_back(vmsData_p->v_feedId2.at(iData));
      correctedFilteredDD.push_back(filteredDD.at(iData));
      correctedFieldId.push_back(vmsData_p->v_fieldId.at(iData));
      correctedInterval.push_back(vmsData_p->v_interval.at(iData));
      correctedExposure.push_back(vmsData_p->v_exposure.at(iData));
      correctedTimeCentroid.push_back(vmsData_p->v_timeCentroid.at(iData));
      correctedUvw.push_back(vv_uvw.at(iData)(0));
      correctedUvw.push_back(vv_uvw.at(iData)(1));
      correctedUvw.push_back(vv_uvw.at(iData)(2));
      correctedData.push_back(cdf.to4Pol(vmsData_p->vv_dataShape.at(iData).at(0),
					 vmsData_p->vv_dataShape.at(iData).at(1),
					 iter->second));
      correctedFlag.push_back(vmsData_p->v_flag.at(iData));
    }
  }
	  
  if (uncorrectedData.size() > 0 && (msFillers.find(AP_UNCORRECTED) != msFillers.end())) {
    if (! mute) {
      msFillers[AP_UNCORRECTED]->addData(complexData,
					 (vector<double>&) vmsData_p->v_time, // this is already time midpoint
					 (vector<int>&) vmsData_p->v_antennaId1,
					 (vector<int>&) vmsData_p->v_antennaId2,
					 (vector<int>&) vmsData_p->v_feedId1,
					 (vector<int>&) vmsData_p->v_feedId2,
					 filteredDD,
					 vmsData_p->processorId,
					 (vector<int>&)vmsData_p->v_fieldId,
					 (vector<double>&) vmsData_p->v_interval,
					 (vector<double>&) vmsData_p->v_exposure,
					 (vector<double>&) vmsData_p->v_timeCentroid,
					 (int) r_p->getScanNumber(), 
					 0,                                               // Array Id
					 (int) r_p->getExecBlockId().getTagValue(), // Observation Id
					 (vector<int>&)msStateId,
					 uvw,
					 filteredShape, // vmsData_p->vv_dataShape after filtering the case numCorr == 3
					 uncorrectedData,
					 (vector<unsigned int>&)vmsData_p->v_flag);
    }
  }

  if (correctedData.size() > 0 && (msFillers.find(AP_CORRECTED) != msFillers.end())) {
    if (! mute) {
      msFillers[AP_CORRECTED]->addData(complexData,
				       correctedTime, // this is already time midpoint
				       correctedAntennaId1, 
				       correctedAntennaId2,
				       correctedFeedId1,
				       correctedFeedId2,
				       correctedFilteredDD,
				       vmsData_p->processorId,
				       correctedFieldId,
				       correctedInterval,
				       correctedExposure,
				       correctedTimeCentroid,
				       (int) r_p->getScanNumber(), 
				       0,                                               // Array Id
				       (int) r_p->getExecBlockId().getTagValue(), // Observation Id
				       correctedMsStateId,
				       correctedUvw,
				       filteredShape, // vmsData_p->vv_dataShape after filtering the case numCorr == 3
				       correctedData,
				       correctedFlag);
    }
  }
  if (debug) cout << "fillMain : exiting" << endl;
}

/**
 * This function fills the MS SysPower table from an ASDM SysPower table.
 *
 * @param ds the ASDM dataset the ASDM SysPower table belongs to.
 * @param ignoreTime a boolean value to indicate if the selected scans are taken into account or if all the table is going to be processed.
 * @param selectedScanRow_v a vector of pointers on ScanRow used to determine which rows of SysPower are going to be processed.
 *
 */
void fillSysPower(const string asdmDirectory, ASDM* ds_p, bool ignoreTime, const vector<ScanRow *>& selectedScanRow_v) {

  if (debug) cout << "fillSysPower : entering" << endl;

  try {

    const SysPowerTable& sysPowerT = ds_p->getSysPower();
    infostream.str("");
    infostream << "The dataset has " << sysPowerT.size() << " syspower(s).";
    info(infostream.str()); 

    if (sysPowerT.size() == 0) return;
    
    TableStreamReader<SysPowerTable, SysPowerRow> tsrSysPower;
    tsrSysPower.open(asdmDirectory);

    while (tsrSysPower.hasRows()) {
      const vector<SysPowerRow*>&	sysPowerRows = tsrSysPower.untilNBytes(50000000);
      vector<int>			antennaId;
      vector<int>			spectralWindowId;
      vector<int>			feedId;
      vector<double>			time;
      vector<double>			interval;
      vector<int>			numReceptor;
      vector<float>			switchedPowerDifference;
      vector<float>			switchedPowerSum;
      vector<float>			requantizerGain;
    
      unsigned int        numReceptor0;
      {
	infostream.str("");
	infostream << "(considering the next " << sysPowerRows.size() << " rows of the SysPower table. ";
	rowsInAScanbyTimeIntervalFunctor<SysPowerRow> selector(selectedScanRow_v);
      
	
	const vector<SysPowerRow *>& sysPowers = selector(sysPowerRows, ignoreTime);

	if (!ignoreTime) 
	  infostream << sysPowers.size() << " of them are in the selected exec blocks / scans";

	infostream << ")";	     
	info(infostream.str());
      
	infostream.str("");
	errstream.str("");

	if (sysPowers.size() == 0) continue;
      
	antennaId.resize(sysPowers.size());
	spectralWindowId.resize(sysPowers.size());
	feedId.resize(sysPowers.size());
	time.resize(sysPowers.size());
	interval.resize(sysPowers.size());
      
	/*
	 * Prepare the mandatory attributes.
	 */
	transform(sysPowers.begin(), sysPowers.end(), antennaId.begin(), sysPowerAntennaId);
	transform(sysPowers.begin(), sysPowers.end(), spectralWindowId.begin(), sysPowerSpectralWindowId);
	transform(sysPowers.begin(), sysPowers.end(), feedId.begin(), sysPowerFeedId);
	transform(sysPowers.begin(), sysPowers.end(), time.begin(), sysPowerMidTimeInSeconds);
	transform(sysPowers.begin(), sysPowers.end(), interval.begin(), sysPowerIntervalInSeconds);
      
	/*
	 * Prepare the optional attributes.
	 */
	numReceptor0 = (unsigned int) sysPowers[0]->getNumReceptor();
	//
	// Do we have a constant numReceptor all over the array numReceptor.
	if (find_if(sysPowers.begin(), sysPowers.end(), sysPowerCheckConstantNumReceptor(numReceptor0)) != sysPowers.end()) {
	  errstream << "In SysPower table, numReceptor is varying. Can't go further." << endl;
	  error(errstream.str());
	}
	/*
	  else 
	  infostream << "In SysPower table, numReceptor is uniformly equal to '" << numReceptor0 << "'." << endl;
	*/
            
	bool switchedPowerDifferenceExists0 = sysPowers[0]->isSwitchedPowerDifferenceExists();
	if (find_if(sysPowers.begin(), sysPowers.end(), sysPowerCheckSwitchedPowerDifference(numReceptor0, switchedPowerDifferenceExists0)) == sysPowers.end()) {
	  
	  if (switchedPowerDifferenceExists0) {
	    //  infostream << "In SysPower table all rows have switchedPowerDifference with " << numReceptor0 << " elements." << endl;
	    switchedPowerDifference.resize(numReceptor0 * sysPowers.size());
	    for_each(sysPowers.begin(), sysPowers.end(), sysPowerSwitchedPowerDifference(switchedPowerDifference.begin()));
	  }
	  /*
	    else
	    infostream << "In SysPower table no switchedPowerDifference recorded." << endl;
	  */
	}
	else {
	  errstream << "In SysPower table, switchedPowerDifference has a variable shape or is not present everywhere or both. Can't go further." ;
	  error(errstream.str());
	}
      
	bool switchedPowerSumExists0 = sysPowers[0]->isSwitchedPowerSumExists();
	if (find_if(sysPowers.begin(), sysPowers.end(), sysPowerCheckSwitchedPowerSum(numReceptor0, switchedPowerSumExists0)) == sysPowers.end()) {

	  if (switchedPowerSumExists0) {
	    //infostream << "In SysPower table all rows have switchedPowerSum with " << numReceptor0 << " elements." << endl;
	    switchedPowerSum.resize(numReceptor0 * sysPowers.size());
	    for_each(sysPowers.begin(), sysPowers.end(), sysPowerSwitchedPowerSum(switchedPowerSum.begin()));
	  }
	  /*
	    else
	    infostream << "In SysPower table no switchedPowerSum recorded." << endl;
	  */
	}
	else {
	  errstream << "In SysPower table, switchedPowerSum has a variable shape or is not present everywhere or both. Can't go further." ;
	  error(errstream.str());
	}
      
	bool requantizerGainExists0 = sysPowers[0]->isRequantizerGainExists();
	if (find_if(sysPowers.begin(), sysPowers.end(), sysPowerCheckRequantizerGain(numReceptor0, requantizerGainExists0)) == sysPowers.end()) {
	  if (requantizerGainExists0) {
	    //infostream << "In SysPower table all rows have switchedPowerSum with " << numReceptor0 << " elements." << endl;
	    requantizerGain.resize(numReceptor0 * sysPowers.size());
	    for_each(sysPowers.begin(), sysPowers.end(), sysPowerRequantizerGain(requantizerGain.begin()));  
	  }
	  /*
	    else
	    infostream << "In SysPower table no switchedPowerSum recorded." << endl;
	  */
	}
	else {
	  errstream << "In SysPower table, requantizerGain has a variable shape or is not present everywhere or both. Can't go further." ;
	  error(errstream.str());
	}
      }
        
      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator msIter = msFillers.begin();
	   msIter != msFillers.end();
	   ++msIter) {
	msIter->second->addSysPowerSlice(antennaId.size(),
					 antennaId,
					 spectralWindowId,
					 feedId,
					 time,
					 interval,
					 (unsigned int) numReceptor0,
					 switchedPowerDifference,
					 switchedPowerSum,
					 requantizerGain);
      }      
      
    } // end of filling SysPower by slice.

    unsigned int numMSSysPowers =  (const_cast<casa::MeasurementSet*>(msFillers.begin()->second->ms()))->rwKeywordSet().asTable("SYSPOWER").nrow();
    if (numMSSysPowers > 0) {
      infostream.str("");
      infostream << "converted in " << numMSSysPowers << " syspower(s) in the measurement set.";
      info(infostream.str());
    }
    tsrSysPower.close();
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }
  if (debug) cout << "fillSysPower : exiting" << endl;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * The main function.
 */
int main(int argc, char *argv[]) {

  string dsName;
  string msNamePrefix;
  string msNameExtension;

  appName = string(argv[0]);
  ofstream ofs;

  LogSinkInterface& lsif = LogSink::globalSink();

  uint64_t bdfSliceSizeInMb = 0; // The default size of the BDF slice hold in memory.

  bool mute = false;


  //   Process command line options and parameters.
  po::variables_map vm;

  try {


    // Declare the supported options.

    po::options_description generic("Converts an ASDM dataset into a CASA measurement set.\n"
				    "Usage : " + appName +" [options] asdm-directory [ms-directory-prefix]\n\n"
				    "Command parameters: \n"
				    " asdm-directory : the pathname to the ASDM dataset to be converted \n"
				    " ms-directory-prefix : the prefix of the pathname(s) of the measurement set(s ) to be created,\n"
				    " this prefis is completed by a suffix to form the name(s) of the resulting measurement set(s), \n"
				    " this suffix depends on the selected options (see options compression and wvr-correction) \n"
				    ".\n\n"
				    "Allowed options:");
    generic.add_options()
      ("help", "produces help message.")
      ("icm",  po::value<string>()->default_value("all"), "specifies the correlation mode to be considered on input. A quoted string containing a sequence of 'ao' 'co' 'ac' 'all' separated by whitespaces is expected")
      ("isrt", po::value<string>()->default_value("all"), "specifies the spectral resolution type to be considered on input. A quoted string containing a sequence of 'fr' 'ca' 'bw' 'all' separated by whitespaces is expected")
      ("its",  po::value<string>()->default_value("all"), "specifies the time sampling (INTEGRATION and/or SUBINTEGRATION)  to be considered on input. A quoted string containing a sequence of 'i' 'si' 'all' separated by whitespaces is expected")  
      ("ocm",  po::value<string>()->default_value("ca"),  "output data for correlation mode AUTO_ONLY (ao) or CROSS_ONLY (co) or CROSS_AND_AUTO (ca)")
      ("compression,c", "produces compressed columns in the resulting measurement set (not set by default). When this option is selected the string '-compressed' is inserted in the pathname of the resulting measurement set.")
      ("asis", po::value<string>(), "creates verbatim copies of the ASDM tables in the output measurement set. The value given to this option must be a quoted string containing a list of table names separated by space characters; the wildcard character '*' is allowed in table names.")
      ("wvr-corrected-data", po::value<string>()->default_value("no"),  "specifies wich values are considered in the ASDM binary data to fill the DATA column in the MAIN table of the MS. Expected values for this option are 'no' for the uncorrected data (this is the default), 'yes' for the corrected data and 'both' for corrected and uncorrected data. In the latter case, two measurement sets are created, one containing the uncorrected data and the other one, whose name is suffixed by '-wvr-corrected', containing the corrected data.")
      ("scans,s", po::value<string>(), "processes only the scans specified in the option's value. This value is a semicolon separated list of scan specifications. A scan specification consists in an exec bock index followed by the character ':' followed by a comma separated list of scan indexes or scan index ranges. A scan index is relative to the exec block it belongs to. Scan indexes are 1-based while exec blocks's are 0-based. \"0:1\" or \"2:2~6\" or \"0:1,1:2~6,8;2:,3:24~30\" \"1,2\" are valid values for the option. \"3:\" alone will be interpreted as 'all the scans of the exec block#3'. An scan index or a scan index range not preceded by an exec block index will be interpreted as 'all the scans with such indexes in all the exec blocks'.  By default all the scans are considered.")
      ("logfile,l", po::value<string>(), "specifies the log filename. If the option is not used then the logged informations are written to the standard error stream.")
      ("verbose,v", "logs numerous informations as the filler is working.")
      ("revision,r", "logs information about the revision of this application.")
      ("dry-run,m", "does not fill the MS MAIN table.")
      ("ignore-time,t", "all the rows of the tables Feed, History, Pointing, Source, SysCal, CalDevice, SysPower and Weather are processed independently of the time range of the selected exec block / scan.")
      ("no-syspower", "the SysPower table will be  ignored.")
      ("no-caldevice", "The CalDevice table will be ignored.")
      ("no-pointing", "The Pointing table will be ignored.")
      ("check-row-uniqueness", "The row uniqueness constraint will be checked in the tables where it's defined")
      ("bdf-slice-size", po::value<uint64_t>(&bdfSliceSizeInMb)->default_value(500),  "The maximum amount of memory expressed as an integer in units of megabytes (1024*1024) allocated for BDF data. The default is 500 (megabytes)") ;

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    po::options_description hidden("Hidden options");
    hidden.add_options()
      ("asdm-directory", po::value< string >(), "asdm directory")
      ;
    hidden.add_options()
      ("ms-directory-prefix", po::value< string >(), "ms directory prefix")
      ;

    po::options_description cmdline_options;
    cmdline_options.add(generic).add(hidden);
    
    po::positional_options_description p;
    p.add("asdm-directory", 1);
    p.add("ms-directory-prefix", 1);
    
    // Parse the command line and retrieve the options and parameters.
    po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);

    po::notify(vm);
    
    // Where do the log messages should go ?
    if (vm.count("logfile")) {
      //LogSinkInterface *theSink;
      ofs.open(vm["logfile"].as<string>().c_str(), ios_base::app);
      LogSinkInterface *theSink = new casa::StreamLogSink(&ofs);
      LogSink::globalSink(theSink);
    }

    // Help ? displays help's content and don't go further.

    if (vm.count("help")) {
      errstream.str("");
      errstream << generic << "\n" ;
      error(errstream.str());
    }

    // Verbose or quiet ?
    verbose = vm.count("verbose") > 0;
   
    // Revision ? displays revision's info and don't go further if there is no dataset to process otherwise proceed....
    string revision = "$Id: asdm2MS.cpp,v 1.84 2011/10/25 14:56:48 mcaillat Exp $\n";
    if (vm.count("revision")) {
      if (!vm.count("asdm-directory")) {
	errstream.str("");
	errstream << revision ;
	error(errstream.str());
      }
      else {
	infostream.str("");
	infostream << revision ;
	info(infostream.str());
      }
    }

    // Selection of correlation mode of data to be considered on input.
    istringstream iss;
    string token;

    string icm_opt = vm["icm"].as< string >();
    iss.clear();
    iss.str(icm_opt);
    
    while (iss >> token) {
      if (token.compare("co") == 0) 
	es_cm.fromString("CROSS_ONLY", false);
      else if (token.compare("ao") == 0)
	es_cm.fromString("AUTO_ONLY", false);
      else if (token.compare("ac") == 0)
	es_cm.fromString("CROSS_AND_AUTO", false);
      else if (token.compare("all") == 0)
	es_cm.fromString("CROSS_ONLY AUTO_ONLY CROSS_AND_AUTO", false);
      else {
	errstream.str("");
	errstream << "Token '" << token << "' invalid for --icm option." << endl;
	errstream << generic << endl;
	error(errstream.str());
      }
    }

    // Selection of spectral resolution type of data to be considered.

    string isrt_opt = vm["isrt"].as< string >();
    iss.clear();
    iss.str(isrt_opt);

    while (iss >> token) {
      if (token.compare("fr") == 0)
	es_srt.fromString("FULL_RESOLUTION", false);
      else if (token.compare("ca") == 0)
	es_srt.fromString("CHANNEL_AVERAGE", false);
      else if (token.compare("bw") == 0)
	es_srt.fromString("BASEBAND_WIDE", false);
      else if (token.compare("all") == 0)
	es_srt.fromString("FULL_RESOLUTION CHANNEL_AVERAGE BASEBAND_WIDE", false);
      else { 
	errstream.str("");
	errstream << "Token '" << token << "' invalid for --isrt option." << endl;
	errstream << generic;
	error(errstream.str());
      }
    }


    // Selection of the time sampling of data to be considered (integration and/or subintegration)

    string its_opt = vm["its"].as < string >();
    iss.clear();
    iss.str(its_opt);

    while ( iss >> token ) {
      if (token.compare("i") == 0)
	es_ts.fromString("INTEGRATION",false);
      else if (token.compare("si") == 0)
	es_ts.fromString("SUBINTEGRATION", false);
      else if (token.compare("all") == 0)
	es_ts.fromString("INTEGRATION SUBINTEGRATION", false);
      else {
	errstream.str("");
	errstream << "Token '" << token << "' invalid for its option." << endl;
	errstream << generic ;
	error(errstream.str());
      }
    }

    // Selection of the correlation mode of data to be produced in the measurement set.

    string ocm_opt = vm["ocm"].as< string >();
    if ( ocm_opt.compare("co") == 0 )
      e_query_cm = CROSS_ONLY;
    else if ( ocm_opt.compare("ao") == 0 )
      e_query_cm = AUTO_ONLY;
    else if ( ocm_opt.compare("ca") == 0 )
      e_query_cm = CROSS_AND_AUTO;
    else {
      errstream.str("");
      errstream << "Token '" << ocm_opt << "' invalid for ocm option." << endl;
      errstream << generic ;
      error(errstream.str());
    }

    if (vm.count("asdm-directory")) {
      string dummy = vm["asdm-directory"].as< string >();
      dsName = lrtrim(dummy) ;
      if (boost::algorithm::ends_with(dsName,"/")) dsName.erase(dsName.size()-1);
    }
    else {
      errstream.str("");
      errstream << generic ;
      error(errstream.str());
    }

    if (vm.count("ms-directory-prefix")) {
      string dummyMSName = vm["ms-directory-prefix"].as< string >();
      dummyMSName = lrtrim(dummyMSName);
      if (boost::algorithm::ends_with(dummyMSName, "/")) dummyMSName.erase(dummyMSName.size()-1);
      boost::filesystem::path msPath(lrtrim(dummyMSName),&boost::filesystem::no_check);
      string msDirectory = msPath.branch_path().string();
      msDirectory = lrtrim(msDirectory);
      if (msDirectory.size() == 0) msDirectory = ".";
      msNamePrefix = msDirectory + "/" + boost::filesystem::basename(msPath);
      msNameExtension = boost::filesystem::extension(msPath);
    }
    else {
      msNamePrefix = dsName;
      msNameExtension = ".ms";
    }
    
    // Does the user want compressed columns in the resulting MS ?
    if ((withCompression = (vm.count("compression") != 0))) {
      infostream.str("");
      infostream << "Compressed columns in the resulting MS(s) : Yes" ;
      info(infostream.str());
    }
    else {
      infostream.str("");
      infostream << "Compressed columns in the resulting MS(s) : No" ;
      info(infostream.str());
    }

    // WVR uncorrected and|or corrected data required ?
    string wvr_corrected_data = vm["wvr-corrected-data"].as<string>();
    if (wvr_corrected_data.compare("no") == 0)
      es_query_apc.fromString("AP_UNCORRECTED");
    else if (wvr_corrected_data.compare("yes") == 0)
      es_query_apc.fromString("AP_CORRECTED");
    else if (wvr_corrected_data.compare("both") == 0)
      es_query_apc.fromString("AP_CORRECTED AP_UNCORRECTED");
    else {
      errstream.str("");
      errstream << "Token '" << wvr_corrected_data << "' invalid for wvr-corrected-data." << endl;
      errstream << generic;
      error(errstream.str());
    }
    
    // Do we want an MS Main table to be filled or not ?
    mute = vm.count("dry-run") != 0;
    if (mute) {
      infostream.str("");
      infostream << "option dry-run is used, the MS Main table will not be filled" << endl;
      info(infostream.str());
    }

    // What is the amount of memory allocated to the BDF slices.
    infostream.str("");
    infostream << "the BDF slice size is set to " << bdfSliceSizeInMb << " megabytes." << endl;
    info(infostream.str());
  }
  catch (std::exception& e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());
  }

  //
  // Try to open an ASDM dataset whose name has been passed as a parameter on the command line
  //
  if ( (dsName.size() > 0) && dsName.at(dsName.size()-1) == '/' ) dsName.erase(dsName.size()-1);


  double cpu_time_parse_xml  = 0.0;
  double real_time_parse_xml = 0.0;
  int mode;
  mode = 0; myTimer(&cpu_time_parse_xml, &real_time_parse_xml, &mode);

  ASDM* ds = new ASDM();
  bool  checkRowUniqueness = vm.count("check-row-uniqueness") != 0;
  infostream.str("");

  if (checkRowUniqueness) 
    infostream << "Row uniqueness constraint will be applied." << endl;
  else
    infostream << "Row uniqueness constraint will be ignored." << endl;

  info(infostream.str());

  try {
    infostream.str("");
    infostream << "Input ASDM dataset : " << dsName << endl;
    info(infostream.str());
    
    ds->setFromFile(dsName, ASDMParseOptions().loadTablesOnDemand(true).checkRowUniqueness(checkRowUniqueness));
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch (std::exception e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());
  }
  catch (...) {
    errstream.str("");
    errstream << "Uncaught exception !" << endl;
    error(errstream.str());
  }
  
  mode = 1; myTimer(&cpu_time_parse_xml, &real_time_parse_xml, &mode);
  infostream.str("");
  infostream << "Time spent parsing the ASDM medata : " << cpu_time_parse_xml << " s.";
  info(infostream.str());

  //
  // What are the apc literals present in the binary data.
  //
  es_apc = apcLiterals(*ds, dsName+"/ASDMBinary");
 
  //
  // Determine what kind of data complex (DATA column) or float (FLOAT_DATA) will be
  // stored in the measurement set by using the method isDataComplex on the first row of 
  // the ASDM Dataset. 
  // This method called on all remaining rows of the ASDM should return the same result
  // otherwise the filling process will stop.
  //
  SDMBinData sdmBinData(ds, dsName);

  // Define the SDM Main table subset of rows to be accepted
  sdmBinData.select( es_cm, es_srt, es_ts);   

  // Define the subset of data to be extracted from the BLOBs
  sdmBinData.selectDataSubset(e_query_cm, es_query_apc);

  //
  // Selection of the scans to consider.
  //
  vector<ScanRow *>	scanRow_v	       = ds->getScan().get();
  map<int, set<int> > all_eb_scan_m;
  for (vector<ScanRow *>::size_type i = 0; i < scanRow_v.size(); i++)
    all_eb_scan_m[scanRow_v[i]->getExecBlockId().getTagValue()].insert(scanRow_v[i]->getScanNumber());

  vector<ScanRow *>	selectedScanRow_v;
  map<int, set<int> >   selected_eb_scan_m;

  string scansOptionInfo;
  if (vm.count("scans")) {
    string scansOptionValue = vm["scans"].as< string >();
    eb_scan_selection ebs;

    int status = parse(scansOptionValue.c_str(), ebs, space_p).full;

    if (status == 0) {
      errstream.str("");
      errstream << "'" << scansOptionValue << "' is an invalid scans selection." << endl;
      error(errstream.str());
    }

    vector<ScanRow *> scanRow_v = ds->getScan().get();
    map<int, set<int> >::iterator iter_m = eb_scan_m.find(-1);

    if (iter_m != eb_scan_m.end())
      for (map<int, set<int> >::iterator iterr_m = all_eb_scan_m.begin(); iterr_m != all_eb_scan_m.end(); iterr_m++)
	if ((iter_m->second).empty())
	  selected_eb_scan_m[iterr_m->first] = iterr_m->second;
	else
	  selected_eb_scan_m[iterr_m->first] = SetAndSet<int>(iter_m->second, iterr_m->second);

    for (map<int, set<int> >::iterator iterr_m = all_eb_scan_m.begin(); iterr_m != all_eb_scan_m.end(); iterr_m++)
      if ((iter_m=eb_scan_m.find(iterr_m->first)) != eb_scan_m.end())
	if ((iter_m->second).empty())
	  selected_eb_scan_m[iterr_m->first].insert((iterr_m->second).begin(), (iterr_m->second).end());
	else {
	  set<int> s = SetAndSet<int>(iter_m->second, iterr_m->second);
	  selected_eb_scan_m[iterr_m->first].insert(s.begin(), s.end());
	}

    ostringstream	oss;
    oss << "The following scans will be processed : " << endl;
    for (map<int, set<int> >::const_iterator iter_m = selected_eb_scan_m.begin(); iter_m != selected_eb_scan_m.end(); iter_m++) {
      oss << "eb#" << iter_m->first << " -> " << displaySet<int>(iter_m->second) << endl;

      Tag		execBlockTag  = Tag(iter_m->first, TagType::ExecBlock);
      for (set<int>::const_iterator iter_s = iter_m->second.begin();
	   iter_s		     != iter_m->second.end();
	   iter_s++)
	selectedScanRow_v.push_back(ds->getScan().getRowByKey(execBlockTag, *iter_s));

    }

    scansOptionInfo = oss.str();
  }
  else {
    selectedScanRow_v = ds->getScan().get();
    selected_eb_scan_m = all_eb_scan_m;
    scansOptionInfo = "All scans of all exec blocks will be processed \n";
  }

  bool	ignoreTime	   = vm.count("ignore-time") != 0;
  bool	processSysPower	   = vm.count("no-syspower") == 0;
  bool	processCalDevice   = vm.count("no-caldevice") == 0;
  bool  processPointing	   = vm.count("no-pointing") == 0;

  //
  // Report the selection's parameters.
  //
  infostream.str("");
  infostream << "Correlation modes requested : " << e_query_cm.str() << endl;
  infostream << "Spectral resolution types requested : " << es_srt.str() << endl;
  infostream << "Time sampling requested : " << es_ts.str() << endl;
  infostream << "WVR uncorrected and|or corrected data requested : " << es_query_apc.str() << endl;
  if (selectedScanRow_v.size()==0) { 
    errstream.str("");
    errstream << "No scan number corresponding to your request. Can't go further.";
    error(errstream.str());
  }

  infostream << scansOptionInfo;

  if (ignoreTime)
    infostream << "All rows of the tables depending on time intervals will be processed independently of the selected exec block / scan.";
  info(infostream.str());

  infostream.str("");
  if (!processSysPower)   infostream << "The SysPower table will not be processed." << endl;
  if (!processCalDevice)  infostream << "The CalDevice table will not be processed." << endl;
  if (!processPointing)   infostream << "The Pointing table will not be processed." << endl;
  info(infostream.str());
  //
  // Shall we have Complex or Float data ?
  //
  bool complexData = true;
  try {
    complexData =  sdmBinData.isComplexData();
  }
  catch (Error & e) {
    errstream.str("");
    errstream << e.getErrorMessage();
    error(errstream.str());
  }

  //
  // Prepare a map AtmPhaseCorrection -> name of measurement set.
  // Three cases are possible :
  // only AP_CORRECTED -> MS name is suffixed with "-wvr-corrected",
  // only AP_UNCORRECTED -> MS name has no particular suffix,
  // AP_CORRECTED and AP_UNCORRECTED -> 2 MSs whith names defined by the two above rules.
  //
  map<AtmPhaseCorrection, string> msNames;
  if (hasCorrectedData(es_apc) && es_query_apc[AP_CORRECTED]) {
    msNames[AP_CORRECTED] = msNamePrefix + "-wvr-corrected";
  }
  if (hasUncorrectedData(es_apc) && es_query_apc[AP_UNCORRECTED]) {
    msNames[AP_UNCORRECTED] = msNamePrefix;
  }
      
  if (msNames.size() == 0) {
    //
    // no MS can be produced due to the selection parameters values.
    // 
    infostream.str("");
    infostream << "No measurement set can be produced with  your selection criteria on '" << dsName << "'" << endl;
    info(infostream.str());
    delete ds;
    exit(1);
  }
  else {
    //
    // OK, we are going to produce at least one MS.
    // If the '--compression' option has been used then append a suffix ".compressed" to
    // the MS name.
    // And eventually always suffix with ".ms".
    //
    for (map<AtmPhaseCorrection, string>::iterator iter=msNames.begin(); iter != msNames.end(); ++iter) {
      if (withCompression)
	iter->second = iter->second + ".compressed";
      iter->second +=  msNameExtension;
    }
  }

  infostream.str("");
  infostream << "The resulting measurement set will contain a '" << ((complexData) ? "DATA" : "FLOAT_DATA") << "' column" << endl; 
  info(infostream.str());

#if DDPRIORITY
  // The data returned by getDataCols will be ordered in priority by Data Descriptions.
  sdmBinData.setPriorityDataDescription();
#endif

  //get numCorr, numChan, telescope name for setupMS
  
  ExecBlockTable& temp_execBlockT = ds->getExecBlock();
  //take first row of the table (assuming telescope name is all the same)
  ExecBlockRow* temp_ebtrow = temp_execBlockT.get()[0];
  string telName  = temp_ebtrow->getTelescopeName();
  //cout<<"telName="<<telName<<endl;

  int maxNumCorr =1;
  PolarizationTable& temp_polT = ds->getPolarization();
  PolarizationRow* temp_poltrow;
  for (int i=0; i<temp_polT.size(); i++) {
    temp_poltrow = temp_polT.get()[i];
    maxNumCorr=max(maxNumCorr, temp_poltrow->getNumCorr());
  }
  //need to add analysis of max NumChan
  int maxNumChan=1;
  SpectralWindowTable& temp_spwT = ds->getSpectralWindow();
  SpectralWindowRow* temp_spwtrow;
  for (int i=0; i<temp_spwT.size(); i++) {
    temp_spwtrow = temp_spwT.get()[i];
    maxNumChan=max(maxNumChan, temp_spwtrow->getNumChan());
  }

  // Create the measurement set(s). 
  if (!false) {
    try {
      for (map<AtmPhaseCorrection, string>::iterator iter = msNames.begin(); iter != msNames.end(); ++iter) {
	msFillers[iter->first] = new ASDM2MSFiller(msNames[iter->first],
						   0.0,
						   false,
						   complexData,
						   withCompression,
                                                   telName,
                                                   maxNumCorr,
                                                   maxNumChan);
	info("About to create a filler for the measurement set '" + msNames[iter->first] + "'");
      }
    }
    catch(AipsError & e) {
      errstream.str("");
      errstream << e.getMesg();
      error(errstream.str());
    }
    catch (std::exception & e) {
      errstream.str("");
      errstream << e.what();
      error(errstream.str());
    }


    msFiller = msFillers.begin()->second;
  }

  //
  // Firstly convert the basic tables.
  //
  // For purpose of simplicity we assume that in all ASDM basic tables having a Tag identifier
  // these Tag values are forming a sequence of integer 0 -> table.size() - 1. If that's not the case, the
  // program aborts.
  //

  //
  // Process the Antenna table.
  // 
  // (This part needs the Station table)
  //
  // At the same time, we populate a map ASDM Station Tag -> MS ANTENNA ID
  // which will be useful when the Weather table will be converted.
  //
  //unsigned int numTrueAntenna
  
  try { 
    AntennaTable& antennaT = ds->getAntenna();
    AntennaRow*   r   = 0;

    int nAntenna = antennaT.size();
    infostream.str("");
    infostream << "The dataset has " << nAntenna << " antenna(s)...";
    info(infostream.str());
    
    for (int i = 0; i < nAntenna; i++) {
      if ((r = antennaT.getRowByKey(Tag(i, TagType::Antenna))) == 0){
	errstream.str("");
	errstream << "Problem while reading the Antenna table, the row with key = Tag(" << i << ") does not exist.Aborting." << endl;
	error(errstream.str());
      }

      // The MS Antenna position is defined as the sum of the ASDM station position and
      // of the ASDM Antenna position after applying to it a coordinate system transformation.
      // Since the ASDM Antenna position is 0,0,0 for now, we only use the ASDM station position.
      vector<Length> position = r->getStationUsingStationId()->getPosition();
      double xPosition = position.at(0).get();
      double yPosition = position.at(1).get();
      double zPosition = position.at(2).get();

      vector<Length> offset = r->getOffset();
      double xOffset = offset.at(0).get();
      double yOffset = offset.at(1).get();
      double zOffset = offset.at(2).get();

      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	int antenna_id = iter->second->addAntenna(r->getName(),
						  r->getStationUsingStationId()->getName(),
						  xPosition,
						  yPosition,
						  zPosition,
						  xOffset,
						  yOffset,
						  zOffset,
						  (float)r->getDishDiameter().get());	
      }
    }

    int numTrueAntenna = msFillers.begin()->second->ms()->antenna().nrow();
    if (numTrueAntenna) {
      infostream.str("");
      infostream << "converted in " << numTrueAntenna << " antenna(s)  in the measurement set(s)." ;
      info(infostream.str());
    }
  }
  catch (IllegalAccessException& e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }
  
  //
  // Process the SpectralWindow table.
  //
  
  try {
    SpectralWindowTable& spwT = ds->getSpectralWindow();      
    vector<Tag> reorderedSwIds = reorderSwIds(*ds); // The vector of Spectral Window Tags in the order they will be inserted in the MS.
    
    for (vector<Tag>::size_type i = 0; i != reorderedSwIds.size() ; i++) swIdx2Idx[reorderedSwIds[i].getTagValue()] = i;

    SpectralWindowRow* r = 0;
    int nSpectralWindow = spwT.size();
    
    infostream.str("");
    infostream << "The dataset has " << nSpectralWindow << " spectral window(s)..."; 
    info(infostream.str());
    
    for (vector<Tag>::size_type i = 0; i < reorderedSwIds.size(); i++) {
      if ((r = spwT.getRowByKey(reorderedSwIds[i])) == 0) {
	errstream.str("");
	(errstream << "Problem while reading the SpectralWindow table, the row with key = Tag(" << i << ") does not exist.Aborting." << endl);
	error(errstream.str());
      }
      
      
      /* Processing the chanFreqXXX
       *
       * Either (chanFreqStart, chanFreqStep) or (chanFreqArray) must be present
       * with a priority to chanFreqArray if both are present. If chanFreqArray
       * is present it *must* have a size equal to numChan otherwiser EXIT.
       */
      bool chanStartAndStep = r->isChanFreqStartExists() && r->isChanFreqStepExists();
      bool chanArray = r->isChanFreqArrayExists();
      if (!chanStartAndStep && !chanArray) {
	errstream.str("");
	errstream << "Did not find (chanFreqStart, chanFreqStep) nor chanFreqArray. Can't go further.";
	error(errstream.str());
      }
      
      //double* chanFreq1D = (double *) 0;
      //DConverter chanFreqConverter;
      vector<double> chanFreq1D;
      vector<Frequency> chanFreqArray;
      Frequency chanFreqStart, chanFreqStep;
      if (chanArray) { // Frequency channels are specified by an array.
	chanFreqArray = r->getChanFreqArray();
	if (chanFreqArray.size() != (unsigned int)r->getNumChan()) {
	  errstream.str("");
	  errstream << "Size of chanFreqArray ('"
		    << chanFreqArray.size()
		    << "') is not equal to numChan ('"
		    << r->getNumChan()
		    << "'). Can't go further.";
	  error(errstream.str());
	}
      }
      else { // Frequency channels are specified by a (start, step) pair.
	chanFreqStart = r->getChanFreqStart();
	chanFreqStep  = r->getChanFreqStep();
	for (int i = 0; i < r->getNumChan(); i++)
	  chanFreqArray.push_back(chanFreqStart + i * chanFreqStep);
      }
      //chanFreq1D = chanFreqConverter.to1DArray(chanFreqArray);
      chanFreq1D = DConverter::toVectorD<Frequency>(chanFreqArray);
      
      /* Processing the chanWidthXXX
       *
       * Either chanWidth or chanWidthArray must be present
       * with a priority to chanWidthArray if both are present. If chanWidthArray
       * is present it *must* have a size equal to numChan otherwiser EXIT.
       */
      if (!r->isChanWidthExists() && !r->isChanWidthArrayExists()) {
	errstream.str("");
	errstream << "Did not find chanWidth nor chanWidthArray. Can't go further.";
	error(errstream.str());
      }
      
      //double* chanWidth1D = (double *) 0;
      //DConverter chanWidthConverter;
      vector<double> chanWidth1D;
      vector<Frequency> chanWidthArray;
      if (r->isChanWidthArrayExists()) { // Frequency channels widths are specified by an array.
	chanWidthArray = r->getChanWidthArray();
	if (chanWidthArray.size() != (unsigned int) r->getNumChan()) {
	  errstream.str("");
	  errstream << "Size of chanWidthArray ('"
		    << chanWidthArray.size()
		    << "') is not equal to numChan ('"
		    << r->getNumChan()
		    << "'). Can't go further.";
	  error(errstream.str());
	}
      }
      else { // Frequency channels widths are specified by a constant value.
	chanWidthArray.resize(r->getNumChan());
	chanWidthArray.assign(chanWidthArray.size(), r->getChanWidth());
      }
      //chanWidth1D = chanWidthConverter.to1DArray(chanWidthArray);
      chanWidth1D = DConverter::toVectorD<Frequency>(chanWidthArray);
      
      /* Processing the effectiveBwXXX
       *
       * Either effectiveBw or effectiveBwArray must be present
       * with a priority to effectiveBwArray if both are present. If effectiveBwArray
       * is present it *must* have a size equal to numChan otherwiser EXIT.
       */
      if (!r->isEffectiveBwExists() && !r->isEffectiveBwArrayExists()) {
	errstream.str("");
	errstream << "Did not find effectiveBw nor effectiveBwArray. Can't go further.";
	error(errstream.str());
      }
      
      //double* effectiveBw1D = (double *) 0;
      vector<double> effectiveBw1D;
      //DConverter effectiveBwConverter;
      vector<Frequency> effectiveBwArray;
      if (r->isEffectiveBwArrayExists()) { // Effective BWs are specified by an array.
	effectiveBwArray = r->getEffectiveBwArray();
	if (effectiveBwArray.size() != (unsigned int) r->getNumChan()) {
	  errstream.str("");
	  errstream << "Size of effectiveBwArray ('"
		    << effectiveBwArray.size()
		    << "') is not equal to numChan ('"
		    << r->getNumChan()
		    << "'). Can't go further." ;
	  error(errstream.str());
	}
      }
      else { // Effective BWs are specified by a constant value.
	effectiveBwArray.resize(r->getNumChan());
	effectiveBwArray.assign(effectiveBwArray.size(), r->getEffectiveBw());
      }
      //effectiveBw1D = effectiveBwConverter.to1DArray(effectiveBwArray);
      effectiveBw1D = DConverter::toVectorD<Frequency>(effectiveBwArray);
      
      
      /* Processing the resolutionXXX
       *
       * Either resolution or resolutionArray must be present
       * with a priority to resolutionArray if both are present. If resolutionArray
       * is present it *must* have a size equal to numChan otherwiser EXIT.
       */
      if (!r->isResolutionExists() && !r->isResolutionArrayExists()) {
	errstream.str("");
	errstream << "Did not find resolution nor resolutionArray. Can't go further";
	error(errstream.str());
      }
      
      //double* resolution1D = (double *) 0;
      vector<double> resolution1D;
      //DConverter resolutionConverter;
      vector<Frequency> resolutionArray;
      if (r->isResolutionArrayExists()) { // Resolutions are specified by an array.
	resolutionArray = r->getResolutionArray();
	if (resolutionArray.size() != (unsigned int) r->getNumChan()) {
	  errstream.str("");
	  errstream << "Size of resolutionArray ('"
		    << resolutionArray.size()
		    << "') is not equal to numChan ('"
		    << r->getNumChan()
		    << "'). Can't go further.";
	  error(errstream.str());
	}
      }
      else { // Resolutions are specified by a constant value.
	resolutionArray.resize(r->getNumChan());
	resolutionArray.assign(resolutionArray.size(), r->getResolution());
      }
      //resolution1D = resolutionConverter.to1DArray(resolutionArray);
      resolution1D = DConverter::toVectorD<Frequency>(resolutionArray);

      /*
       * associated spectral windows and and natures.
       */
      
      unsigned int numAssocValues = 0;
      if (r->isNumAssocValuesExists()) numAssocValues = r->getNumAssocValues();
      
      // Test the simultaneous presence or absence of assocNature and assoSpectralWindowId
      if ((r->isAssocNatureExists() && !r->isAssocSpectralWindowIdExists()) ||
	  (!r->isAssocNatureExists() && r->isAssocSpectralWindowIdExists())) {
	errstream.str("");
	errstream << "Only one of the attributes assocSpectralWindowId and assocNature is present. Can't go further."
		  << endl;
	error(errstream.str());
      }
      
      vector<int> assocSpectralWindowId_;
      vector<string> assocNature_ ;

      if (r->isAssocSpectralWindowIdExists()) { // it exists then the assocNature exists also, given the test
	                                        // which is done before.
	vector<Tag> assocSpectralWindowId = r->getAssocSpectralWindowId();
	if (numAssocValues != assocSpectralWindowId.size()) {
	  infostream.str("");
	  infostream << "The size of assocSpectralWindowId ('"
		     << assocSpectralWindowId.size()
		     << "') is not equal to the value announced in numAssocValues ('"
		     << numAssocValues
		     << "'). Ignoring the difference and sending the full vector assocSpectralWindowId to the filler" 
	    ;
	  info(infostream.str());
	}
	numAssocValues = assocSpectralWindowId.size();
	assocSpectralWindowId_ = IConverter::toVectorI(assocSpectralWindowId);

	// Take into account the re ordering of the spectral window indices.
	for (unsigned int iAssocSw = 0; iAssocSw < numAssocValues; iAssocSw++)
	  assocSpectralWindowId_[iAssocSw] =  swIdx2Idx[assocSpectralWindowId_[iAssocSw]];

	vector<SpectralResolutionType> assocNature = r->getAssocNature();

	if (assocNature.size() != assocSpectralWindowId_.size()) {
	  infostream.str("");
	  infostream << "The size of assocNature ('"
		     << assocNature.size() 
		     << "') is not equal to the size of assocSpectralWindowId ('"
		     << assocSpectralWindowId.size()
		     << "'). Ignoring the difference and sending the full assocNature vector to the filler.";
	  info(infostream.str());
	}
	assocNature_ = SConverter::toVectorS<SpectralResolutionType, CSpectralResolutionType>(r->getAssocNature());
      }
      
      int numChan           = r->getNumChan();
      string name           = r->isNameExists()?r->getName():"";
      double refFreq        = r->getRefFreq().get();
      int measFreqRef       = r->isMeasFreqRefExists()?FrequencyReferenceMapper::value(r->getMeasFreqRef()):MFrequency::TOPO;
      double totalBandwidth = r->getTotBandwidth().get();
      int netSideband       = r->getNetSideband();
      int bbcNo             = r->getBasebandName();
      int ifConvChain       = 0;
      int freqGroup         = r->isFreqGroupExists()?r->getFreqGroup():0;
      string freqGroupName  = r->isFreqGroupNameExists()?r->getFreqGroupName().c_str():"";

      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	iter->second->addSpectralWindow(numChan,
					name,
					refFreq,
					chanFreq1D,  
					chanWidth1D, 
					measFreqRef,
					effectiveBw1D, 
					resolution1D,
					totalBandwidth,
					netSideband,
					bbcNo,
					ifConvChain,
					freqGroup,
					freqGroupName,
					numAssocValues,
					assocSpectralWindowId_,
					assocNature_);      
      }      
    }
    if (nSpectralWindow) {
      infostream.str("");
      infostream << "converted in " << msFillers.begin()->second->ms()->spectralWindow().nrow() << " spectral window(s) in the measurement set(s).";
      info(infostream.str());
    }
  }
  catch (IllegalAccessException& e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }

  //
  // Process the Polarization table
  //
  Stokes::StokesTypes linearCorr[] = { Stokes::XX, Stokes::XY, Stokes::YX, Stokes::YY };
  Stokes::StokesTypes circularCorr[] = { Stokes::RR, Stokes::RL, Stokes::LR, Stokes::LL };
  int corrProduct1[] = { 0, 0 };
  int corrProduct2[] = { 0, 0, 1, 1};
  int corrProduct4[] = { 0, 0, 0, 1, 1, 0, 1, 1 };
			 
  vector<int> polarizationIdx2Idx;
  int pIdx;

  try {
    PolarizationTable& polT = ds->getPolarization();  
    PolarizationRow* r = 0;
    int nPolarization = polT.size();
    infostream.str("");
    infostream << "The dataset has " << nPolarization << " polarization(s)..."; 
    info(infostream.str());

    for (int i = 0; i < nPolarization; i++) {
      if ((r=polT.getRowByKey(Tag(i, TagType::Polarization))) == 0) {
	errstream.str("");
	(errstream << "Problem while reading the Polarization table, the row with key = Tag(" << i << ") does not exist.Aborting." << endl);
	error(errstream.str());
      }
      
      int numCorr = r->getNumCorr();
      if (numCorr < 1 || numCorr > 4) {
	ostringstream oss ;
	oss << "a polarization row cannot be processed due to  'numCorr = " << numCorr << "'.";
	throw ASDM2MSException(oss.str());
      }

      //Stokes::StokesTypes * corrType;
      //vector<Stokes::StokesTypes> corrType;
      vector<int> corrType;
      StokesMapper stokesMapper;
      if (numCorr != 3) {
	corrType = StokesMapper::toVectorI(r->getCorrType());
      }
      else {
	numCorr  = 4;
	StokesParameterMod::StokesParameter sp = r->getCorrType()[0];
	if ((sp == StokesParameterMod::RR) ||
	    (sp == StokesParameterMod::LL) ||
	    (sp == StokesParameterMod::RL) ||
	    (sp == StokesParameterMod::LR)) {
	  corrType.resize(4);
	  copy (circularCorr, circularCorr+4, corrType.begin());
	}
	else if ((sp == StokesParameterMod::XX) ||
		 (sp == StokesParameterMod::XY) ||
		 (sp == StokesParameterMod::YX) ||
		 (sp == StokesParameterMod::YY)) {
	  corrType.resize(4);
	  copy (linearCorr, linearCorr+4, corrType.begin());
	}
	else {
	  errstream.str("");
	  errstream << " I don't know what to do with the given Stokes parameters for autocorrelation data" << endl;
	  error(errstream.str());
	}
	  
      }
      
      
      /*int* corrProduct = 0;*/
      vector<int> corrProduct; 
      switch (numCorr) {
      case 1: corrProduct.resize(2); copy(corrProduct1, corrProduct1+2, corrProduct.begin()); break;
      case 2: corrProduct.resize(4); copy(corrProduct2, corrProduct2+4, corrProduct.begin()); break;
      case 4: corrProduct.resize(8); copy(corrProduct4, corrProduct4+8, corrProduct.begin()); break;
      }


      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	pIdx = iter->second->addUniquePolarization(numCorr,
						   corrType,
						   corrProduct
						   );
      }
      polarizationIdx2Idx.push_back(pIdx);
    }
    if (nPolarization) {
      infostream.str("");
      infostream << "converted in " << msFillers.begin()->second->ms()->polarization().nrow() << " polarization(s)." ;
      info(infostream.str());
    }
  }
  catch (ASDM2MSException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }
   
  //
  // Process the DataDescription table.
  //
  try {
    DataDescriptionTable& ddT = ds->getDataDescription();
    DataDescriptionRow* r = 0;
    int nDataDescription = ddT.size();
    infostream.str("");
    infostream << "The dataset has " << nDataDescription << " data description(s)...";
    info(infostream.str());

    for (int i = 0; i < nDataDescription; i++) {
      if ((r=ddT.getRowByKey(Tag(i, TagType::DataDescription))) == 0) {
	errstream.str("");
	(errstream << "Problem while reading the DataDescription table, the row with key = Tag(" << i << ") does not exist.Aborting." << endl);
	error(errstream.str());
      }
      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	ddIdx = iter->second->addUniqueDataDescription(swIdx2Idx[r->getSpectralWindowId().getTagValue()],
						       polarizationIdx2Idx.at(r->getPolOrHoloId().getTagValue()));
      }
      dataDescriptionIdx2Idx.push_back(ddIdx); 
    }
    if (nDataDescription) {
      infostream.str("");
      infostream << "converted in " << msFillers.begin()->second->ms()->dataDescription().nrow() << " data description(s)  in the measurement set(s)." ;
      info(infostream.str());
    }
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }

  // 
  // Process the ExecBlock table,
  // in order to build the MS Observation table.
  // 
  try {
    const ExecBlockTable& execBlockT = ds->getExecBlock(); 
    ExecBlockRow* r = 0;
    int nExecBlock = execBlockT.size();
    infostream.str("");
    infostream << "The dataset has " << nExecBlock << " execBlock(s) ...";

    vector<ExecBlockRow *> temp_v = execBlockT.get();
    vector<ExecBlockRow *> v;
    for (vector<ExecBlockRow *>::iterator iter_v = temp_v.begin(); iter_v != temp_v.end(); iter_v++)
      if ( selected_eb_scan_m.find((*iter_v)->getExecBlockId().getTagValue()) != selected_eb_scan_m.end() )
	v.push_back(*iter_v);
    
    vector<string> schedule; schedule.resize(2);

    infostream << v.size() << " of them in the selected exec blocks / scans ... ";
    info(infostream.str());

    for (unsigned int i = 0; i < v.size(); i++) {
      r = v.at(i);
      
      string telescopeName  = r->getTelescopeName();
      double startTime      = r->getStartTime().getMJD()*86400;
      double endTime        = r->getEndTime().getMJD()*86400;
      string observerName   = r->getObserverName();

      vector<string> observingLog = r->getObservingLog();

      string scheduleType("ALMA");
      schedule[0] = "SchedulingBlock " + ds->getSBSummary().getRowByKey(r->getSBSummaryId())->getSbSummaryUID().getEntityId().toString();
      schedule[1] = "ExecBlock " + r->getExecBlockUID().getEntityId().toString();
      string project(r->getProjectUID().getEntityId().toString());
      double releaseDate = r->isReleaseDateExists() ? r->getReleaseDate().getMJD():0.0;

      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	iter->second->addObservation(telescopeName,
				     startTime,
				     endTime,
				     observerName,
				     observingLog,
				     scheduleType,
				     schedule,
				     project,
				     releaseDate
				     );
      }
      if (i==0) { // assume same telescope for all execBlocks 
        if (telescopeName.find("EVLA")!=string::npos) {
          isEVLA=true;
        }
        else if (telescopeName.find("OSF")!=string::npos || 
                 telescopeName.find("AOS")!=string::npos || 
                 telescopeName.find("ATF")!=string::npos) {
          isEVLA=false;
        }     
        string telname = (isEVLA ? "EVLA" : "ALMA");
        infostream.str("");
        infostream << "Telescope Name:" <<telescopeName << ", process as "<<telname<<" data." ; 
        info(infostream.str());
      }
    }
    if (nExecBlock) {
      infostream.str("");
      infostream << "converted in " << msFillers.begin()->second->ms()->observation().nrow() << " observation(s) in the measurement set(s)." ;
      info(infostream.str());
    }
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }
  
  //
  // Process the Feed table
  // Issues :
  //    - time (epoch) : at the moment it takes directly the time as it is stored in the ASDM.
  //    - focusLength (in AIPS++) is no defined.
  try {
    const FeedTable& feedT = ds->getFeed();
    FeedRow* r = 0;
    infostream.str("");
    infostream << "The dataset has " << feedT.size() << " feed(s)...";
    rowsInAScanbyTimeIntervalFunctor<FeedRow> selector(selectedScanRow_v);
    
    const vector<FeedRow *>& v = selector(feedT.get(), ignoreTime);
    if (!ignoreTime)
      infostream << v.size() << " of them in the exec blocks / selected scans ... ";
    
    info(infostream.str());
    int nFeed = v.size();
    for (int i = 0; i < nFeed; i++) {
      r = v.at(i);
      // For now we just adapt the types of the time related informations and compute a mid-time.
      //
      double interval = ((double) r->getTimeInterval().getDuration().get()) / ArrayTime::unitsInASecond ;
      double time;
      // if (isEVLA) {
      //   time =  ((double) r->getTimeInterval().getStart().get()) / ArrayTime::unitsInASecond;
      // }
      // else {
      time =  ((double) r->getTimeInterval().getStart().get()) / ArrayTime::unitsInASecond + interval/2.0;
      //}

      vector<double> beam_offset_ =  DConverter::toVectorD(r->getBeamOffset());
      vector<std::string> polarization_type_ = PolTypeMapper().toStringVector(r->getPolarizationTypes());
      vector<complex<float> > polarization_response_ = CConverter::toVectorCF(r->getPolResponse());
      vector<double> xyzPosition (3, 0.0);
      if (r->isPositionExists()) {
	vector<Length> position = r->getPosition();
	if (position.size() != 3) {
	  errstream.str("");
	  errstream << "The size of attribute position ('" 
		    << position.size()
		    << "') is not equal to 3. Can't go further."
		    << endl;
	  error(errstream.str());
	}
	
	xyzPosition = DConverter::toVectorD<Length>(position);
      }
      vector<double> receptor_angle_ = DConverter::toVectorD<Angle>(r->getReceptorAngle());
      
      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	iter->second->addFeed((int) r->getAntennaId().getTagValue(),
			      r->getFeedId(),
			      swIdx2Idx[r->getSpectralWindowId().getTagValue()],
			      time,
			      interval,
			      r->getNumReceptor(), 
			      -1,             // We ignore the beamId array
			      beam_offset_,
			      polarization_type_,
			      polarization_response_,
			      xyzPosition,
			      receptor_angle_);
      } 
    }
    if (nFeed) {
      infostream.str("");
      infostream <<  "converted in " << msFillers.begin()->second->ms()->feed().nrow() << " feed(s) in the measurement set." ;
      info(infostream.str());
    }
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }
  
  // Process the Field table.
  // Issues :
  // - only processes the case with numPoly == 0 at the moment.

  FieldTable& fieldT = ds->getField();
  
  try {
    FieldRow* r = 0;
    int nField = fieldT.size();
    infostream.str("");
    infostream << "The dataset has " << nField << " field(s)...";
    info(infostream.str());

    for (int i = 0; i < nField; i++) {
      if ((r=fieldT.getRowByKey(Tag(i, TagType::Field))) == 0) {
	errstream.str("");
	(errstream << "Problem while reading the Field table, the row with key = Tag(" << i << ") does not exist.Aborting." << endl);
	error(errstream.str());
      }

      string fieldName = r->getFieldName();
      string code = r->getCode();
      DirectionReferenceCodeMod::DirectionReferenceCode dirRefCode = DirectionReferenceCodeMod::J2000;
      if(r->isDirectionCodeExists()){
	dirRefCode = r->getDirectionCode();
	//cout << "found directionCode for field " << fieldName << ": ";
      }
      //       else{
      // 	cout << "No directionCode in input table. Assuming ";
      //       }
      string directionCode = CDirectionReferenceCode::name(dirRefCode);
      //cout << directionCode << endl;

      vector<double> delayDir     = DConverter::toVectorD<Angle>(r->getDelayDir());
      vector<double> phaseDir     = DConverter::toVectorD<Angle>(r->getPhaseDir());
      vector<double> referenceDir = DConverter::toVectorD<Angle>(r->getReferenceDir());

      int sourceId = -1;
      if (r->isSourceIdExists()) sourceId = r->getSourceId();

      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	iter->second->addField( fieldName,
				code,
				r->isTimeExists() ? ((double) r->getTime().get()) / ArrayTime::unitsInASecond : 0.0,
				delayDir,
				phaseDir,
				referenceDir,
				directionCode,
				r->isSourceIdExists()?r->getSourceId():0);
      }
    }
    if (nField) {
      infostream.str("");
      infostream << "converted in " << msFillers.begin()->second->ms()->field().nrow() << "  field(s) in the measurement set(s)." ;
      info(infostream.str());
    }
  }
  catch (IllegalAccessException& e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }

  //
  // Process the FlagCmd table.
  //
  try {
    const FlagCmdTable& flagCmdT  = ds->getFlagCmd();
    FlagCmdRow* r = 0;
    infostream.str("");
    infostream << "The dataset has " << flagCmdT.size() << " FlagCmd(s)...";
    rowsInAScanbyTimeIntervalFunctor<FlagCmdRow> selector(selectedScanRow_v);

    const vector<FlagCmdRow *>& v = selector(flagCmdT.get(), ignoreTime);
    if (!ignoreTime)
      infostream << v.size() << " of them in the exec blocks / selected scans ... ";

    info(infostream.str());
    int nFlagCmd = v.size();
    for (int i = 0; i < nFlagCmd; i++) {
      r = v.at(i);
      // For now we just adapt the types of the time related informations and compute a mid-time.
      //
      double interval = ((double) r->getTimeInterval().getDuration().get()) / ArrayTime::unitsInASecond ;
      double time;
      // if (isEVLA) {
      //   time =  ((double) r->getTimeInterval().getStart().get()) / ArrayTime::unitsInASecond ;
      // }
      // else {
      time =  ((double) r->getTimeInterval().getStart().get()) / ArrayTime::unitsInASecond + interval/2.0;
      //}
      string type = r->getType();
      string reason = r->getReason();
      string command = r->getCommand();
   
      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	iter->second->addFlagCmd(time,
				 interval,
				 type,
				 reason,
				 r->getLevel(),
				 r->getSeverity(),
				 r->getApplied() ? 1 : 0,
				 command);
      }
    }
    if (nFlagCmd) {
      infostream.str("");
      infostream << "converted in " << msFillers.begin()->second->ms()->flagCmd().nrow() << " in the measurement set." ;
      info(infostream.str());
    }
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }
  
  // Process the History table.
  // Issues :
  // - use executeBlockId for observationId ...to be discussed with Francois.
  // - objectId : not taken into account (it's a string while the MS expects an int).
  try {
    const HistoryTable& historyT = ds->getHistory();
    HistoryRow* r = 0;
    int nHistory = historyT.size();
    infostream.str("");
    infostream << "The dataset has " << nHistory << " history(s)...";
    rowsInAScanbyTimeFunctor<HistoryRow> selector(selectedScanRow_v, isEVLA);

    const vector<HistoryRow *>& v = selector(historyT.get(), ignoreTime);;
    if (!ignoreTime) 
      infostream << v.size() << " of them in the selected exec blocks / scans ... ";

    info(infostream.str()); 

    for (int i = 0; i < nHistory; i++) {
      r = v.at(i);
      double time =  ((double) r->getTime().get()) / ArrayTime::unitsInASecond ;
      string message     = r->getMessage();
      string priority    = r->getPriority();
      string origin      = r->getOrigin();
      string application = r->getApplication();
      string cliCommand  = r->getCliCommand();
      string appParams   = r->getAppParms();
 
      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	iter->second->addHistory(time,
				 r->getExecBlockId().getTagValue(),   
				 message,
				 priority,
				 origin,
				 -1,
				 application,
				 cliCommand,
				 appParams);
      }
    }
    if (nHistory) {
      infostream.str("");
      infostream << "converted in " << msFillers.begin()->second->ms()->history().nrow() << " history(s) in the measurement set(s)." ;
      info(infostream.str());
    }
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }
  
  //
  // Process the Pointing table.
  // Issues :
  // - pointingModelId , phaseTracking, sourceOffset and overTheTop not taken into account.

  if (processPointing) 
    try {
      const PointingTable& pointingT = ds->getPointing();
      infostream.str("");
      infostream << "The dataset has " << pointingT.size() << " pointing(s)...";
      rowsInAScanbyTimeIntervalFunctor<PointingRow> selector(selectedScanRow_v);

      const vector<PointingRow *>& v = selector(pointingT.get(), ignoreTime);
      if (!ignoreTime) 
	infostream << v.size() << " of them in the selected exec blocks / scans ... ";

      info(infostream.str());
      int nPointing = v.size();

      if (nPointing > 0) {

	// Check some assertions.
	//
	// All rows of ASDM-Pointing must have their attribute usePolynomials equal to false
	// and their numTerm attribute equal to 1. Use the opportunity of this check
	// to compute the number of rows to be created in the MS-Pointing by summing
	// all the numSample attributes values.
	//
	int numMSPointingRows = 0;
	for (unsigned int i = 0; i < v.size(); i++) {
	  if (v[i]->getUsePolynomials()) {
	    errstream.str("");
	    errstream << "Found usePolynomials equal to true at row #" << i <<". Can't go further.";
	    error(errstream.str());
	  }

	  numMSPointingRows += v[i]->getNumSample();
	}

	//
	// Ok now we have verified the assertions and we know the number of rows
	// to be created into the MS-Pointing, we can proceed.

	PointingRow* r = 0;

	vector<int> antenna_id_(numMSPointingRows, 0);
	vector<double> time_(numMSPointingRows, 0.0);
	vector<double> interval_(numMSPointingRows, 0.0);
	vector<double> direction_(2 * numMSPointingRows, 0.0);
	vector<double> target_(2 * numMSPointingRows, 0.0);
	vector<double> pointing_offset_(2 * numMSPointingRows, 0.0);
	vector<double> encoder_(2 * numMSPointingRows, 0.0);
	vector<bool> tracking_(numMSPointingRows, false);

	//
	// Let's check if the optional attribute overTheTop is present somewhere in the table.
	//
	unsigned int numOverTheTop = count_if(v.begin(), v.end(), overTheTopExists);
	bool overTheTopExists4All = v.size() == numOverTheTop;

	vector<bool> v_overTheTop_ ;

	vector<s_overTheTop> v_s_overTheTop_;
    
	if (overTheTopExists4All) 
	  v_overTheTop_.resize(numMSPointingRows);
	else if (numOverTheTop > 0) 
	  v_overTheTop_.resize(numOverTheTop);

	int iMSPointingRow = 0;
	for (int i = 0; i < nPointing; i++) {     // Each row in the ASDM-Pointing ...
	  r = v.at(i);

	  // Let's prepare some values.
	  int antennaId = r->getAntennaId().getTagValue();
      
	  double time = 0.0, interval = 0.0;
	  if (!r->isSampledTimeIntervalExists()) { // If no sampledTimeInterval then
	    // then compute the first value of MS TIME and INTERVAL.
	    interval   = ((double) r->getTimeInterval().getDuration().get()) / ArrayTime::unitsInASecond / r->getNumSample();
	    // if (isEVLA) {
	    //   time = ((double) r->getTimeInterval().getStart().get()) / ArrayTime::unitsInASecond;
	    // }
	    // else {
	    time = ((double) r->getTimeInterval().getStart().get()) / ArrayTime::unitsInASecond + interval / 2.0;
	    //}
	  }

	  //
	  // The size of each vector below 
	  // should be checked against numSample !!!
	  //
	  int numSample = r->getNumSample();
	  const vector<vector<Angle> > encoder = r->getEncoder();
	  checkVectorSize<vector<Angle> >("encoder", encoder, "numSample", (unsigned int) numSample, "Pointing", (unsigned int)i);

	  const vector<vector<Angle> > pointingDirection = r->getPointingDirection();
	  checkVectorSize<vector<Angle> >("pointingDirection", pointingDirection, "numSample", (unsigned int) numSample, "Pointing", (unsigned int) i);

	  const vector<vector<Angle> > target = r->getTarget();
	  checkVectorSize<vector<Angle> >("target", target, "numSample", (unsigned int) numSample, "Pointing", (unsigned int) i);

	  const vector<vector<Angle> > offset = r->getOffset();
	  checkVectorSize<vector<Angle> >("offset", offset, "numSample", (unsigned int) numSample, "Pointing", (unsigned int) i);

	  bool   pointingTracking = r->getPointingTracking();
 
	  //
	  // Prepare some data structures and values required to compute the
	  // (MS) direction.
	  vector<double> cartesian1(3, 0.0);
	  vector<double> cartesian2(3, 0.0);
	  vector<double> spherical1(2, 0.0);
	  vector<double> spherical2(2, 0.0);
	  vector<vector<double> > matrix3x3;
	  for (unsigned int ii = 0; ii < 3; ii++) {
	    matrix3x3.push_back(cartesian1); // cartesian1 is used here just as a way to get a 1D vector of size 3.
	  }
	  double PSI = M_PI_2;
	  double THETA;
	  double PHI;
      
	  vector<ArrayTimeInterval> timeInterval ;
	  if (r->isSampledTimeIntervalExists()) timeInterval = r->getSampledTimeInterval();

	  // Use 'fill' from algorithm for the cases where values remain constant.
	  // ANTENNA_ID
	  fill(antenna_id_.begin()+iMSPointingRow, antenna_id_.begin()+iMSPointingRow+numSample, antennaId);

	  // TRACKING 
	  fill(tracking_.begin()+iMSPointingRow, tracking_.begin()+iMSPointingRow+numSample, pointingTracking);

	  // OVER_THE_TOP 
	  if (overTheTopExists4All)
	    // it's present everywhere
	    fill(v_overTheTop_.begin()+iMSPointingRow, v_overTheTop_.begin()+iMSPointingRow+numSample,
		 r->getOverTheTop());
	  else if (r->isOverTheTopExists()) {
	    // it's present only in some rows.
	    s_overTheTop saux ;
	    saux.start = iMSPointingRow; saux.len = numSample; saux.value = r->getOverTheTop();
	    v_s_overTheTop_.push_back(saux);
	  }
       
	  // Use an explicit loop for the other values.
	  for (int j = 0 ; j < numSample; j++) { // ... must be expanded in numSample MS-Pointing rows.

	    // TIME and INTERVAL
	    if (r->isSampledTimeIntervalExists()) { //if sampledTimeInterval is present use its values.	           
	      // Here the size of timeInterval will have to be checked against numSample !!
	      interval_[iMSPointingRow] = ((double) timeInterval.at(j).getDuration().get()) / ArrayTime::unitsInASecond ;
	      time_[iMSPointingRow] = ((double) timeInterval.at(j).getStart().get()) / ArrayTime::unitsInASecond
		+ interval_[iMSPointingRow]/2;	  
	    }
	    else {                                     // otherwise compute TIMEs and INTERVALs from the first values.
	      interval_[iMSPointingRow]            = interval;
	      time_[iMSPointingRow]                = time + j*interval;
	    }

	    // DIRECTION
	    THETA = target.at(j).at(1).get();
	    PHI   = -M_PI_2 - target.at(j).at(0).get();
	    spherical1[0] = offset.at(j).at(0).get();
	    spherical1[1] = offset.at(j).at(1).get();
	    rect(spherical1, cartesian1);
	    eulmat(PSI, THETA, PHI, matrix3x3);
	    matvec(matrix3x3, cartesian1, cartesian2);
	    spher(cartesian2, spherical2);
	    direction_[2*iMSPointingRow]  = spherical2[0];
	    direction_[2*iMSPointingRow+1]= spherical2[1];

	    // TARGET
	    target_[2*iMSPointingRow]     = target.at(j).at(0).get();
	    target_[2*iMSPointingRow+1]   = target.at(j).at(1).get();

	    // POINTING_OFFSET
	    pointing_offset_[2*iMSPointingRow]   = offset.at(j).at(0).get();
	    pointing_offset_[2*iMSPointingRow+1] = offset.at(j).at(1).get();

	    // ENCODER
	    encoder_[2*iMSPointingRow]           = encoder.at(j).at(0).get();
	    encoder_[2*iMSPointingRow+1]         = encoder.at(j).at(1).get();

	
	    // increment the row number in MS Pointing.
	    iMSPointingRow++;	
	  }
	}
    

	for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	     iter != msFillers.end();
	     ++iter) {
	  iter->second->addPointingSlice(numMSPointingRows,
					 antenna_id_,
					 time_,
					 interval_,
					 direction_,
					 target_,
					 pointing_offset_,
					 encoder_,
					 tracking_,
					 overTheTopExists4All,
					 v_overTheTop_,
					 v_s_overTheTop_);
	}
    
	if (nPointing) {
	  infostream.str("");
	  infostream << "converted in " << msFillers.begin()->second->ms()->pointing().nrow() << " pointing(s) in the measurement set." ;
	  info(infostream.str()); 
	}
      }
    }
    catch (ConversionException e) {
      errstream.str("");
      errstream << e.getMessage();
      error(errstream.str());
    }
    catch ( std::exception & e) {
      errstream.str("");
      errstream << e.what();
      error(errstream.str());      
    }

    
  // Process the processor table.
  //

  try {
    ProcessorTable& processorT = ds->getProcessor();
    ProcessorRow* r = 0;
    int nProcessor = processorT.size();

    infostream.str("");
    infostream << "The dataset has " << nProcessor << " processor(s)...";
    info(infostream.str());
    
    for (int i = 0; i < nProcessor; i++) {
      if ((r=processorT.getRowByKey(Tag(i, TagType::Processor))) == 0) {
	errstream.str("");
	(errstream << "Problem while reading the Processor table, the row with key = Tag(" << i << ") does not exist.Aborting." << endl);
	error(errstream.str());
      }
      
      string processorType    = CProcessorType::name(r->getProcessorType());
      string processorSubType = CProcessorSubType::name(r->getProcessorSubType());
      
      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	iter->second->addProcessor(processorType,
				   processorSubType,
				   -1,    // Since there is no typeId in the ASDM.
				   r->getModeId().getTagValue());
      }  
    }
    if (nProcessor) {
      infostream.str("");
      infostream << "converted in " << msFillers.begin()->second->ms()->processor().nrow() << " processor(s) in the measurement set." ;
      info(infostream.str());
    } 
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }
  
  // Process the Source table.
  //
  const SourceTable& sourceT = ds->getSource();
  try {
    SourceRow* r = 0;
    infostream.str("");
    infostream << "The dataset has " << sourceT.size() << " sources(s)...";
    rowsInAScanbyTimeIntervalFunctor<SourceRow> selector(selectedScanRow_v);

    const vector<SourceRow *>& v = selector(sourceT.get(), ignoreTime);
    if (!ignoreTime) 
      infostream << v.size() << " of them in the selected scans ... ";

    info(infostream.str());
    int nSource = v.size();

    for (int i = 0; i < nSource; i++) {
      r = v.at(i);
      //
      // Check some assertions. 
      // For each row of the Source table, if any of the optional attributes which is an array and depend on numLines for the size of one
      // of its dimensions then the (optional) attribute numLines must be present and the dimensions depending on on numLines must be 
      // consistent with the value of numLines.
      //
      int numLines = r->isNumLinesExists() ? r->getNumLines() : 0;
      
      if (r->isTransitionExists()) {
	if (!r->isNumLinesExists()) {
	  errstream.str("");
	  errstream << "Source row#" << i << ". The attribute 'transition' exists but the attribute 'numLines' which serves to define its shape is missing. Can't go further.";
	  error(errstream.str());
	}

	int transitionSize = r->getTransition().size();
	if (numLines != transitionSize) {
	  errstream.str("");
	  errstream << "The value of 'numLines' (" << numLines << ") is not compatible with the found size of 'transition' (" << transitionSize << "). Can't go further.";
	  error(errstream.str());
	}
      }

      if (r->isRestFrequencyExists()) {
	if (!r->isNumLinesExists()) {
	  errstream.str("");
	  errstream << "Source row#" << i << ". The attribute 'restFrequency' exists but the attribute 'numLines' which serves to define its shape is missing. Cant' go further.";
	  error(errstream.str());
	}
	
	int restFrequencySize = r->getRestFrequency().size();
	if (numLines != restFrequencySize) {
	  errstream.str("");
	  errstream << "The value of 'numLines' (" << numLines << ") is not compatible with the found size of 'restFrequency' (" << restFrequencySize << "). Can't go further.";
	  error(errstream.str());
	}
      }

      if (r->isSysVelExists()) {
	if (!r->isNumLinesExists()) {
	  errstream.str("");
	  errstream << "Source row#" << i << ". The attribute 'sysVel' exists but the attribute 'numLines' which serves to define its shape is missing. Cant' go further.";
	  error(errstream.str());
	}
	
	int sysVelSize = r->getSysVel().size();
	if (numLines != sysVelSize) {
	  errstream.str("");
	  errstream << "The value of 'numLines' (" << numLines << ") is not compatible with the found size of 'sysVel' (" << sysVelSize << "). Can't go further.";
	  error(errstream.str());
	}
      }          

      int sourceId = r->getSourceId();
      // For now we just adapt the types of the time related informations and compute a mid-time.
      //
      double interval = ((double) r->getTimeInterval().getDuration().get()) / ArrayTime::unitsInASecond ;
      double time;
      // if (isEVLA) {
      //   time =  r->getTimeInterval().getStartInMJD()*86400 ;
      // }
      // else {
      time =  r->getTimeInterval().getStartInMJD()*86400 + interval / 2.0 ;
      //}

      int spectralWindowId = swIdx2Idx[r->getSpectralWindowId().getTagValue()];

      string sourceName = r->getSourceName();

      int calibrationGroup = r->isCalibrationGroupExists() ? r->getCalibrationGroup() : 0;
      
      string code = r->getCode();

      vector<double> direction = DConverter::toVectorD(r->getDirection());
 
      DirectionReferenceCodeMod::DirectionReferenceCode dirRefCode = DirectionReferenceCodeMod::J2000;
      if(r->isDirectionCodeExists()){
	dirRefCode = r->getDirectionCode();
	//cout << "found directionCode for source " << sourceName << ": ";
      }
      //else{
      //  cout << "No directionCode in input table. Assuming ";
      //}
      string directionCode = CDirectionReferenceCode::name(dirRefCode);
      //cout << directionCode << endl;

      vector<double> position ;
      if (r->isPositionExists()){
	position = DConverter::toVectorD<Length>(r->getPosition());
      } 
				
      vector<double> properMotion = DConverter::toVectorD(r->getProperMotion());
  
      vector<string> transition;
      if (r->isTransitionExists()) {
	transition = r->getTransition();
      }

      vector<double> restFrequency;
      if (r->isRestFrequencyExists()) {
	restFrequency = DConverter::toVectorD<Frequency>(r->getRestFrequency());
      }

      vector<double> sysVel;
      if (r->isSysVelExists()) {
	sysVel = DConverter::toVectorD<Speed>(r->getSysVel());
      }
   

      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	iter->second->addSource(sourceId,
				time,
				interval,
				spectralWindowId,
				numLines,
				sourceName,
				calibrationGroup,
				code,
				direction,
				directionCode,
				position,
				properMotion,
				transition,
				restFrequency,
				sysVel);
      }
    }
    
    if (nSource) {
      infostream.str("");
      infostream << "converted in " << msFillers.begin()->second->ms()->source().nrow() <<" source(s) in the measurement set(s)." ;
      info(infostream.str());
    }
  }
  catch (IllegalAccessException& e) {
    errstream.str("");
    error(errstream.str());
  }

  //
  // Process the SysCal table.
  //
  const SysCalTable& sysCalT = ds->getSysCal();
  try {
    SysCalRow* r = 0;
    infostream.str("");
    infostream << "The dataset has " << sysCalT.size() << " sysCal(s)...";
    rowsInAScanbyTimeIntervalFunctor<SysCalRow> selector(selectedScanRow_v);

    const vector<SysCalRow *>& v = selector(sysCalT.get(), ignoreTime);
    if (!ignoreTime) 
      infostream << v.size() << " of them in the selected scans ... ";

    info(infostream.str());
    int nSysCal = v.size();

    for (int i = 0; i < nSysCal; i++) {
      r = v.at(i);
      double interval = ((double) r->getTimeInterval().getDuration().get()) / ArrayTime::unitsInASecond ;
      double time;
      // if (isEVLA) {
      //   time =  ((double) r->getTimeInterval().getStart().get()) / ArrayTime::unitsInASecond ;
      // }
      // else {
      time =  ((double) r->getTimeInterval().getStart().get()) / ArrayTime::unitsInASecond + interval / 2.0 ;
      //}

      pair<bool, bool> tcal_flag_pair;
      tcal_flag_pair.first   = r->isTcalFlagExists();
      tcal_flag_pair.second  = r->isTcalFlagExists() ? r->getTcalFlag() : false;

      pair<bool, vector<float> > tcal_spectrum_pair;
      tcal_spectrum_pair.first  =  r->isTcalSpectrumExists() ;
      if (tcal_spectrum_pair.first)
	tcal_spectrum_pair.second = FConverter::toVectorF<Temperature>(r->getTcalSpectrum(), true);

      pair<bool, bool> trx_flag_pair;
      trx_flag_pair.first   = r->isTrxFlagExists();
      trx_flag_pair.second  = r->isTrxFlagExists() ? r->getTrxFlag() : false;

      pair<bool, vector<float> > trx_spectrum_pair;
      trx_spectrum_pair.first  =  r->isTrxSpectrumExists() ;
      if (trx_spectrum_pair.first)
	trx_spectrum_pair.second = FConverter::toVectorF<Temperature>(r->getTrxSpectrum(), true);

      pair<bool, bool> tsky_flag_pair;
      tsky_flag_pair.first   = r->isTskyFlagExists();
      tsky_flag_pair.second  = r->isTskyFlagExists() ? r->getTskyFlag() : false;

      pair<bool, vector<float> > tsky_spectrum_pair;
      tsky_spectrum_pair.first  =  r->isTskySpectrumExists() ;
      if (tsky_spectrum_pair.first)
	tsky_spectrum_pair.second = FConverter::toVectorF<Temperature>(r->getTskySpectrum(), true);

      pair<bool, bool> tsys_flag_pair;
      tsys_flag_pair.first   = r->isTsysFlagExists();
      tsys_flag_pair.second  = r->isTsysFlagExists() ? r->getTsysFlag() : false;

      pair<bool, vector<float> > tsys_spectrum_pair;
      tsys_spectrum_pair.first  =  r->isTsysSpectrumExists() ;
      if (tsys_spectrum_pair.first)
	tsys_spectrum_pair.second = FConverter::toVectorF<Temperature>(r->getTsysSpectrum(), true);

      pair<bool, bool> tant_flag_pair;
      tant_flag_pair.first   = r->isTantFlagExists();
      tant_flag_pair.second  = r->isTantFlagExists() ? r->getTantFlag() : false;

      pair<bool, vector<float> > tant_spectrum_pair;
      tant_spectrum_pair.first  =  r->isTantSpectrumExists() ;
      if (tant_spectrum_pair.first)
	tant_spectrum_pair.second = FConverter::toVectorF(r->getTantSpectrum(), true);

      pair<bool, bool> tant_tsys_flag_pair;
      tant_tsys_flag_pair.first   = r->isTantTsysFlagExists();
      tant_tsys_flag_pair.second  = r->isTantTsysFlagExists() ? r->getTantTsysFlag() : false;

      pair<bool, vector<float> > tant_tsys_spectrum_pair;
      tant_tsys_spectrum_pair.first  =  r->isTantTsysSpectrumExists() ;
      if (tant_tsys_spectrum_pair.first)
	tant_tsys_spectrum_pair.second = FConverter::toVectorF(r->getTantTsysSpectrum(), true);

      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	iter->second->addSysCal((int) r->getAntennaId().getTagValue(),
				(int) r->getFeedId(),
				(int) swIdx2Idx[r->getSpectralWindowId().getTagValue()],
				time,
				interval,
				r->getNumReceptor(),
				r->getNumChan(),
				tcal_spectrum_pair,
				tcal_flag_pair,
				trx_spectrum_pair,
				trx_flag_pair,
				tsky_spectrum_pair,
				tsky_flag_pair,
				tsys_spectrum_pair,
				tsys_flag_pair,
				tant_spectrum_pair,
				tant_flag_pair,
				tant_tsys_spectrum_pair,
				tant_tsys_flag_pair);				
      }
    }
    if (nSysCal) {
      infostream.str("");
      infostream << "converted in " << msFillers.begin()->second->ms()->sysCal().nrow() <<" sysCal(s) in the measurement set(s)." ;
      info(infostream.str());
    }
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }
  
  //
  // Process the CalDevice table.
  try {
    const CalDeviceTable& calDeviceT = ds->getCalDevice();
    infostream.str("");
    infostream << "The dataset has " << calDeviceT.size() << " calDevice(s)...";

    if (processCalDevice && calDeviceT.size() > 0) {
      rowsInAScanbyTimeIntervalFunctor<CalDeviceRow> selector(selectedScanRow_v);    
      const vector<CalDeviceRow *>& calDevices = selector(calDeviceT.get(), ignoreTime);
      if (!ignoreTime) 
	infostream << calDevices.size() << " of them in the selected exec blocks / scans ... ";
    
      info(infostream.str());

      for (vector<CalDeviceRow*>::const_iterator iter = calDevices.begin(); iter != calDevices.end(); iter++) {
	bool ignoreThisRow = false;
	unsigned int numCalload = 0;
	unsigned int numReceptor = 0;
      
	//
	// Let's make some checks on the attributes.
	errstream.str("");
	infostream.str("");

	//
	// Is numCalload > 0 ?
	if ((numCalload = (*iter)->getNumCalload()) <= 0) { 
	  errstream << "In the table CalDevice, the attribute 'numCalload' in row #"
		    << (unsigned int) (iter - calDevices.begin())
		    << " has an invalid value '("
		    << numCalload << "'), a strictly positive value is expected."
		    << endl; 
	  ignoreThisRow = true;
	}
      
	//
	// Do we have enough elements in calLoadNames ?
	vector<CalibrationDevice> temp = (*iter)->getCalLoadNames();
	vector<string> calLoadNames;
	if (temp.size() < numCalload) { 
	  errstream  << "In the table CalDevice, the size of the attribute 'calLoadNames' in row #"
		     << (unsigned int) (iter - calDevices.begin())
		     << " is too small. It should be greater than or equal to the value of the atttribute 'numCalload' ("
		     << numCalload
		     <<")."
		     << endl;
	  ignoreThisRow = true;
	}
	else {
	  calLoadNames.resize(temp.size());
	  transform(temp.begin(), temp.end(), calLoadNames.begin(), stringValue<CalibrationDevice, CCalibrationDevice>);	  
	}

	//
	// Do we have numReceptor ?
	if ((*iter)->isNumReceptorExists()) {
	  numReceptor = (*iter)->getNumReceptor();
	  if (numReceptor == 0) {
	    errstream << "In the table CalDevice, the value of the attribute 'numReceptor' in row #"
		      << (unsigned int) (iter - calDevices.begin())
		      << " is invalid (" 
		      << numReceptor 
		      << "). It is expected to be strictly positive."
		      << endl;
	    ignoreThisRow = true;
	  }
	}

	//
	// Do we have calEff ?
	vector<vector<float> > calEff;
	if ((*iter)->isCalEffExists()) {
	  //
	  // Do we take it into account ?
	  if (numReceptor == 0) {
	    infostream << "In the table CalDevice, the attribute 'calEff' is present in row #"
		       << (unsigned int) (iter - calDevices.begin())
		       << " but it will be ignored due to the fact that the attribute 'numReceptor' is null."
		       << endl;
	  }
	  else {
	    calEff = (*iter)->getCalEff();
	  
	    //
	    // Let's check the sizes of its content.
	    if (calEff.size() < numReceptor) {
	      errstream << "In the table CalDevice, the size of the attribute 'calEff' in row #"
			<< (unsigned int) (iter - calDevices.begin())
			<< " is too small. It should be greater than or equal to the value of the attribute 'numReceptor' ("
			<< numReceptor
			<<")."
			<< endl;
	      ignoreThisRow = true;
	    }
	    else {
	      if (find_if(calEff.begin(), calEff.end(), size_lt<float>(numCalload)) != calEff.end()) {
		errstream << "In the table CalDevice, the attribute 'calEff' in row #"
			  << (unsigned int) (iter - calDevices.begin())
			  << " has at least one element whose size is too small. All its elements should have their size"
			  << " greater then  or equal to the value of the attribute 'numCalload' ("
			  << numCalload
			  << ")."
			  << endl;
		ignoreThisRow = true;
	      }
	    }
	  }	
	}
      
	//
	// In priority let's see if we have coupledNoiseCal ?
	vector<vector<float> > coupledNoiseCal;
	if ((*iter)->isCoupledNoiseCalExists()) {
	  //
	  // Do we take it into account ?
	  if (numReceptor == 0) {
	    infostream << "In the table CalDevice, the attribute 'coupledNoiseCal' is present in row #"
		       << (unsigned int) (iter - calDevices.begin())
		       << " but it will be ignored due to the fact that the attribute 'numReceptor' is null."
		       << endl;
	  }
	  else {
	    coupledNoiseCal = (*iter)->getCoupledNoiseCal();
	  
	    //
	    // Let's check the sizes of its content.
	    if (coupledNoiseCal.size() < numReceptor) {
	      errstream << "In the table CalDevice, the size of the attribute 'coupledNoiseCal' in row #"
			<< (unsigned int) (iter - calDevices.begin())
			<< " is too small. It should be greater than or equal to the value of the attribute 'numReceptor' ("
			<< numReceptor
			<<")."
			<< endl;
	      ignoreThisRow = true;
	    }
	    else {
	      if (find_if(coupledNoiseCal.begin(), coupledNoiseCal.end(), size_lt<float>(numCalload)) != coupledNoiseCal.end()) {
		errstream << "In the table CalDevice, the attribute 'coupledNoiseCal' in row #"
			  << (unsigned int) (iter - calDevices.begin())
			  << " has at least one element whose size is too small. All its elements should have their size"
			  << " greater than or equal to the value of the attribute 'numCalload' (=="
			  << numCalload
			  << ")."
			  << endl;
		ignoreThisRow = true;
	      }
	    }
	  }	
	}
	// Ok we don't have coupledNoiseCal , but maybe we have noiseCal ?
	else if ((*iter)->isNoiseCalExists()) {
	  //
	  // Do we take it into account ?
	  vector<double> noiseCal = (*iter)->getNoiseCal();
	
	  if (noiseCal.size() < numCalload) {
	    infostream << "In the table CalDevice, the size of the attribute 'noiseCal' in row #"
		       << (unsigned int) (iter - calDevices.begin())
		       << " is too small. It should be greater than or equal to the value of the attribute 'numCalload' ("
		       << numCalload
		       << ")."
		       << endl;
	    ignoreThisRow = true;
	  }
	  else {
	    // So yes we have a noiseCal attribute, then pretend we have coupledNoiseCal. 
	    // Artificially force numReceptor to 2 and fill coupledNoiseCal by replicating what we have in noiseCal :
	    // coupledNoiseCal[0] = noiseCal
	    // coupledNoiseCal[1] = noiseCal
	    //
	    infostream << "In the table CalDevice  there is no attribute 'coupledNoiseCal' but there an attribute 'noiseCal' in row #"
		       << (unsigned int) (iter - calDevices.begin())
		       << " which we are going to use to fill the MS NOISE_CAL by replicating its values."
		       << endl;
	  
	    numReceptor = 2;
	    coupledNoiseCal.resize(numReceptor);
	    for (unsigned int iReceptor = 0; iReceptor < numReceptor; iReceptor++) {
	      coupledNoiseCal[iReceptor].resize(numCalload);
	      transform(noiseCal.begin(), noiseCal.begin()+numCalload, coupledNoiseCal[iReceptor].begin(), d2f);
	    } 
	  }
	}
      
	//
	// Do we have temperatureLoad ?
	vector<double> temperatureLoad;
	if ((*iter)->isTemperatureLoadExists()) {
	  vector<Temperature> temp = (*iter)->getTemperatureLoad();
	  if (temp.size() < numCalload) {
	    errstream  << "In the table CalDevice, the size of the attribute 'temperatureLoad' in row #"
		       << (unsigned int) (iter - calDevices.begin())
		       << " is too small. It should be greater than or equal to the value of the atttribute 'numCalload' ("
		       << numCalload
		       <<")."
		       << endl;
	    ignoreThisRow = true;
	  }
	  else {
	    temperatureLoad.resize(temp.size());
	    transform(temp.begin(), temp.end(), temperatureLoad.begin(), basicTypeValue<Temperature, float>);	  
	  }
	}
      
	if (errstream.str().size() > 0) 
	  error(errstream.str());
      
	if (infostream.str().size() > 0)
	  info(infostream.str());

	if (ignoreThisRow) {
	  infostream.str("");
	  infostream << "This row will be ignored." << endl;
	  info(infostream.str());
	  continue;
	}

	//
	// And finally we can add a new row to the MS CALDEVICE table.
	double interval = ((double) (*iter)->getTimeInterval().getDuration().get()) / ArrayTime::unitsInASecond ;
	double time;
	// if (isEVLA) {
	//   time =  (*iter)->getTimeInterval().getStartInMJD()*86400 ;
	// }
	// else {
	time =  (*iter)->getTimeInterval().getStartInMJD()*86400 + interval / 2.0 ;
	//}
      
	for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator msIter = msFillers.begin();
	     msIter != msFillers.end();
	     ++msIter) {
	  msIter->second->addCalDevice((*iter)->getAntennaId().getTagValue(),
				       (*iter)->getFeedId(),
				       swIdx2Idx[(*iter)->getSpectralWindowId().getTagValue()],
				       time,
				       interval,
				       numCalload,
				       calLoadNames,
				       numReceptor,
				       calEff,
				       coupledNoiseCal,
				       temperatureLoad);
	}      
      }
      unsigned int numMSCalDevices = (const_cast<casa::MeasurementSet*>(msFillers.begin()->second->ms()))->rwKeywordSet().asTable("CALDEVICE").nrow();
      if (numMSCalDevices > 0) {
	infostream.str("");
	infostream << "converted in " << numMSCalDevices << " caldevice(s) in the measurement set.";
	info(infostream.str());
      }
    }
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }
 
  //
  // Process the SysPower table.
#if 1 
  if ( processSysPower )
    fillSysPower(dsName, ds, ignoreTime, selectedScanRow_v);
#else
  if ( processSysPower ) 
    try {
      const SysPowerTable& sysPowerT = ds->getSysPower();
      infostream.str("");
      infostream << "The dataset has " << sysPowerT.size() << " syspower(s)..."; 

      if ( sysPowerT.size() > 0 ) { 
	vector<int>		antennaId;
	vector<int>		spectralWindowId;
	vector<int>		feedId;
	vector<double>	time;
	vector<double>	interval;
	vector<int>		numReceptor;
	vector<float>	switchedPowerDifference;
	vector<float>	switchedPowerSum;
	vector<float>	requantizerGain;
    
	unsigned int        numReceptor0;
	{
	  info(infostream.str()); infostream.str("");
	  rowsInAScanbyTimeIntervalFunctor<SysPowerRow> selector(selectedScanRow_v);
      
	  const vector<SysPowerRow *>& sysPowers = selector(sysPowerT.get(), ignoreTime);
      
	  if (!ignoreTime) 
	    infostream << sysPowers.size() << " of them in the selected exec blocks / scans ... ";	
      
	  info(infostream.str());
      
	  infostream.str("");
	  errstream.str("");
      
	  antennaId.resize(sysPowers.size());
	  spectralWindowId.resize(sysPowers.size());
	  feedId.resize(sysPowers.size());
	  time.resize(sysPowers.size());
	  interval.resize(sysPowers.size());
      
	  /*
	   * Prepare the mandatory attributes.
	   */
	  transform(sysPowers.begin(), sysPowers.end(), antennaId.begin(), sysPowerAntennaId);
	  transform(sysPowers.begin(), sysPowers.end(), spectralWindowId.begin(), sysPowerSpectralWindowId);
	  transform(sysPowers.begin(), sysPowers.end(), feedId.begin(), sysPowerFeedId);
	  transform(sysPowers.begin(), sysPowers.end(), time.begin(), sysPowerMidTimeInSeconds);
	  transform(sysPowers.begin(), sysPowers.end(), interval.begin(), sysPowerIntervalInSeconds);
      
	  /*
	   * Prepare the optional attributes.
	   */
	  numReceptor0 = (unsigned int) sysPowers[0]->getNumReceptor();
	  //
	  // Do we have a constant numReceptor all over the array numReceptor.
	  if (find_if(sysPowers.begin(), sysPowers.end(), sysPowerCheckConstantNumReceptor(numReceptor0)) != sysPowers.end()) {
	    errstream << "In SysPower table, numReceptor is varying. Can't go further." << endl;
	    error(errstream.str());
	  }
	  else 
	    infostream << "In SysPower table, numReceptor is uniformly equal to '" << numReceptor0 << "'." << endl;
            
	  bool switchedPowerDifferenceExists0 = sysPowers[0]->isSwitchedPowerDifferenceExists();
	  if (find_if(sysPowers.begin(), sysPowers.end(), sysPowerCheckSwitchedPowerDifference(numReceptor0, switchedPowerDifferenceExists0)) == sysPowers.end())
	    if (switchedPowerDifferenceExists0) {
	      infostream << "In SysPower table all rows have switchedPowerDifference with " << numReceptor0 << " elements." << endl;
	      switchedPowerDifference.resize(numReceptor0 * sysPowers.size());
	      for_each(sysPowers.begin(), sysPowers.end(), sysPowerSwitchedPowerDifference(switchedPowerDifference.begin()));
	    }
	    else
	      infostream << "In SysPower table no switchedPowerDifference recorded." << endl;
	  else {
	    errstream << "In SysPower table, switchedPowerDifference has a variable shape or is not present everywhere or both. Can't go further." ;
	    error(errstream.str());
	  }
      
	  bool switchedPowerSumExists0 = sysPowers[0]->isSwitchedPowerSumExists();
	  if (find_if(sysPowers.begin(), sysPowers.end(), sysPowerCheckSwitchedPowerSum(numReceptor0, switchedPowerSumExists0)) == sysPowers.end())
	    if (switchedPowerSumExists0) {
	      infostream << "In SysPower table all rows have switchedPowerSum with " << numReceptor0 << " elements." << endl;
	      switchedPowerSum.resize(numReceptor0 * sysPowers.size());
	      for_each(sysPowers.begin(), sysPowers.end(), sysPowerSwitchedPowerSum(switchedPowerSum.begin()));
	    }
	    else
	      infostream << "In SysPower table no switchedPowerSum recorded." << endl;
	  else {
	    errstream << "In SysPower table, switchedPowerSum has a variable shape or is not present everywhere or both. Can't go further." ;
	    error(errstream.str());
	  }
      
	  bool requantizerGainExists0 = sysPowers[0]->isRequantizerGainExists();
	  if (find_if(sysPowers.begin(), sysPowers.end(), sysPowerCheckRequantizerGain(numReceptor0, requantizerGainExists0)) == sysPowers.end())
	    if (requantizerGainExists0) {
	      infostream << "In SysPower table all rows have switchedPowerSum with " << numReceptor0 << " elements." << endl;
	      requantizerGain.resize(numReceptor0 * sysPowers.size());
	      for_each(sysPowers.begin(), sysPowers.end(), sysPowerRequantizerGain(requantizerGain.begin()));  
	    }
	    else
	      infostream << "In SysPower table no switchedPowerSum recorded." << endl;
	  else {
	    errstream << "In SysPower table, requantizerGain has a variable shape or is not present everywhere or both. Can't go further." ;
	    error(errstream.str());
	  }
	}
	info(infostream.str());
        
	for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator msIter = msFillers.begin();
	     msIter != msFillers.end();
	     ++msIter) {
	  msIter->second->addSysPowerSlice(antennaId.size(),
					   antennaId,
					   spectralWindowId,
					   feedId,
					   time,
					   interval,
					   (unsigned int) numReceptor0,
					   switchedPowerDifference,
					   switchedPowerSum,
					   requantizerGain);
	}
     
	unsigned int numMSSysPowers =  (const_cast<casa::MeasurementSet*>(msFillers.begin()->second->ms()))->rwKeywordSet().asTable("SYSPOWER").nrow();
	if (numMSSysPowers > 0) {
	  infostream.str("");
	  infostream << "converted in " << numMSSysPowers << " syspower(s) in the measurement set.";
	  info(infostream.str());
	}
      } // end of filling SysPower by slice.
    }
    catch (ConversionException e) {
      errstream.str("");
      errstream << e.getMessage();
      error(errstream.str());
    }
    catch ( std::exception & e) {
      errstream.str("");
      errstream << e.what();
      error(errstream.str());      
    }
#endif

  //
  // Load the weather table
  const WeatherTable& weatherT = ds->getWeather();

  try {
    WeatherRow* r = 0;
    infostream.str("");
    infostream << "The dataset has " << weatherT.size() << " weather(s)...";
    rowsInAScanbyTimeIntervalFunctor<WeatherRow> selector(selectedScanRow_v);
    
    const vector<WeatherRow *>& v = selector(weatherT.get(), ignoreTime);
    if (!ignoreTime) 
      infostream << v.size() << " of them in the selected scans ... ";

    info(infostream.str());
    int nWeather = v.size();

    infostream.str("");
    infostream << "The dataset has " << nWeather << " weather(s)...";
    info(infostream.str());
    
    pair<bool, float>
      pressureOpt,
      relHumidityOpt,
      temperatureOpt,
      windDirectionOpt,
      windSpeedOpt,
      dewPointOpt;
    
#define OPT_ATTR_PAIR( rowPtr, AttributeName ) rowPtr -> is ## AttributeName ## Exists() ? make_pair ( true, rowPtr -> get ## AttributeName ().get()) : make_pair( false, 0.)        
    
    for (int i = 0; i < nWeather; i++) {
      r			 = v.at(i);      
      double	interval = ((double) r->getTimeInterval().getDuration().get()) / ArrayTime::unitsInASecond ;
      double	time	 = ((double) r->getTimeInterval().getStart().get()) / ArrayTime::unitsInASecond + interval / 2.0;
      
      pressureOpt			   = OPT_ATTR_PAIR(r, Pressure);
      pressureOpt.second		  /= 100. ;	// We consider that ASDM stores Pascals & MS expects hectoPascals
      relHumidityOpt			   = OPT_ATTR_PAIR(r, RelHumidity);
      temperatureOpt			   = OPT_ATTR_PAIR(r, Temperature);
      windDirectionOpt			   = OPT_ATTR_PAIR(r, WindDirection);
      windSpeedOpt			   = OPT_ATTR_PAIR(r, WindSpeed);
      dewPointOpt			   = OPT_ATTR_PAIR(r, DewPoint);
      int		wxStationId        = r->getStationId().getTagValue();
      vector<double>	wxStationPosition  = DConverter::toVectorD(r->getStationUsingStationId()->getPosition());
    
      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	iter->second->addWeather(-1,
				 time,
				 interval,
				 pressureOpt,
				 relHumidityOpt,
				 temperatureOpt,
				 windDirectionOpt,
				 windSpeedOpt,
				 dewPointOpt,
				 wxStationId,
				 wxStationPosition);
      }
    }

    if (nWeather) {
      infostream.str("");
      infostream << "converted in " << msFillers.begin()->second->ms()->weather().nrow() <<" weather(s) in the measurement set." ;
      info(infostream.str());
    }
  }
  catch (ConversionException e) {
    errstream.str("");
    errstream << e.getMessage();
    error(errstream.str());
  }
  catch ( std::exception & e) {
    errstream.str("");
    errstream << e.what();
    error(errstream.str());      
  }


  // And then finally process the state and the main table.
  //
  {
    const MainTable& mainT = ds->getMain();
    const StateTable& stateT = ds->getState();
    
    MainRow* r = 0;
    vector<MainRow*> v;
    vector<int32_t> mainRowIndex; 
    //
    //
    // Consider only the Main rows whose execBlockId and scanNumber attributes correspond to the selection.
    //
    const vector<MainRow *>& temp = mainT.get();
    for ( vector<MainRow *>::const_iterator iter_v = temp.begin(); iter_v != temp.end(); iter_v++) {
      map<int, set<int> >::iterator iter_m = selected_eb_scan_m.find((*iter_v)->getExecBlockId().getTagValue());
      if ( iter_m != selected_eb_scan_m.end() && iter_m->second.find((*iter_v)->getScanNumber()) != iter_m->second.end() ) {
	mainRowIndex.push_back(iter_v - temp.begin());
	v.push_back(*iter_v);
      }
    }
      
    infostream.str("");
    infostream << "The dataset has " << mainT.size() << " main(s)...";
    infostream << v.size() << " of them in the selected exec blocks / scans." << endl;
    info(infostream.str());
    uint32_t nMain = v.size();

    const VMSData *vmsDataPtr = 0;
    // Initialize an UVW coordinates engine.
    UvwCoords uvwCoords(ds);
    
    ostringstream oss;

    // For each selected main row.
    for (int32_t i = 0; i < nMain; i++) {
      try {
	// Populate the State table.
	fillState(v[i]);

	// What's the processor for this Main row ?
	Tag cdId = v[i]->getConfigDescriptionId();
	ConfigDescriptionTable& cT = ds->getConfigDescription();
	ConfigDescriptionRow* cR = cT.getRowByKey(cdId);
	Tag pId = cR->getProcessorId();
	ProcessorTable& pT = ds->getProcessor();
	ProcessorRow* pR = pT.getRowByKey(pId);
	ProcessorType processorType = ds->getProcessor().getRowByKey(pId)->getProcessorType();
	infostream.str("");
	infostream << "ASDM Main row #" << mainRowIndex[i] << " contains data produced by a '" << CProcessorType::name(processorType) << "'." ;
	info(infostream.str());

	infostream.str("");
	infostream << "ASDM Main row #" << mainRowIndex[i] << " - BDF file size is " << v[i]->getDataSize() << " bytes for " << v[i]->getNumIntegration() << " integrations." << endl;
	info(infostream.str());

	if (processorType == RADIOMETER) {
	  if (!sdmBinData.acceptMainRow(v[i])) {
	    infostream.str("");
	    infostream <<"No data retrieved in the Main row #" << mainRowIndex[i] << " (" << sdmBinData.reasonToReject(r) <<")" << endl;
	    info(infostream.str());
	    continue;
	  }
	  vmsDataPtr = sdmBinData.getDataCols();
	  fillMain(i, v[i], sdmBinData, vmsDataPtr, uvwCoords, complexData, mute);
	  infostream.str("");
	  infostream << "ASDM Main row #" << mainRowIndex[i] << " produced a total of " << vmsDataPtr->v_antennaId1.size() << " MS Main rows." << endl;
	  info(infostream.str());
	}
	else {

	  // Open its associate BDF.
	  sdmBinData.openMainRow(v[i]);
	  
	  uint32_t		N			 = v[i]->getNumIntegration();
	  uint64_t		bdfSize			 = v[i]->getDataSize();
	  vector<uint64_t>	actualSizeInMemory(sizeInMemory(bdfSize, bdfSliceSizeInMb*1024*1024));
	  int32_t			numberOfMSMainRows	 = 0;
	  int32_t			numberOfIntegrations	 = 0;
	  int32_t			numberOfReadIntegrations = 0;
	  
	  // For each slice of the BDF with a size approx equal to the required size
	  for (unsigned int j = 0; j < actualSizeInMemory.size(); j++) {
	    numberOfIntegrations = actualSizeInMemory[j] / (bdfSize / N);
	    infostream.str("");
	    infostream << "ASDM Main row #" << mainRowIndex[i] << " - " << numberOfReadIntegrations  << " integrations done so far - the next " << numberOfIntegrations << " integrations produced " ;
	    vmsDataPtr = sdmBinData.getNextMSMainCols(numberOfIntegrations);
	    numberOfReadIntegrations += numberOfIntegrations;
	    numberOfMSMainRows += vmsDataPtr->v_antennaId1.size();
	    
	    fillMain(i, v[i], sdmBinData, vmsDataPtr, uvwCoords, complexData, mute);
	    
	    infostream << vmsDataPtr->v_antennaId1.size()  << " MS Main rows." << endl;
	    info(infostream.str());
	  }
	  
	  uint32_t numberOfRemainingIntegrations = N - numberOfReadIntegrations;
	  if (numberOfRemainingIntegrations) {
	    infostream.str("");
	    infostream << "ASDM Main row #" << mainRowIndex[i] << " - " << numberOfReadIntegrations  << " integrations done so far - the next " << numberOfRemainingIntegrations << " integrations produced " ;
	    vmsDataPtr = sdmBinData.getNextMSMainCols(numberOfRemainingIntegrations);
	    fillMain(i, v[i], sdmBinData, vmsDataPtr, uvwCoords, complexData, mute);
	    
	    infostream << vmsDataPtr->v_antennaId1.size()  << " MS Main rows." << endl;
	    info(infostream.str());
	    numberOfMSMainRows += vmsDataPtr->v_antennaId1.size();
	  }
	  infostream.str("");
	  infostream << "ASDM Main row #" << mainRowIndex[i] << "produced a total of " << numberOfMSMainRows << " MS Main rows." << endl;
	}
      }
      catch ( IllegalAccessException& e) {
	infostream.str("");
	infostream << e.getMessage();
	info(infostream.str());
      }
      catch ( SDMDataObjectStreamReaderException& e ) {
	infostream.str("");
	infostream << e.getMessage();
	info(infostream.str());
      }
      catch ( SDMDataObjectReaderException& e ) {
	infostream.str("");
	infostream << e.getMessage();
	info(infostream.str());
      }
      catch (ConversionException e) {
	infostream.str("");
	infostream << e.getMessage();
	info(infostream.str());
      }
      catch ( std::exception & e) {
	infostream.str("");
	infostream << e.what();
	info(infostream.str());      
      }
      catch (Error & e) {
	infostream.str("");
	infostream << e.getErrorMessage();
	info(infostream.str());
      }
    }

    infostream.str("");
    infostream << "The dataset has "  << stateT.size() << " state(s)..." ;
    info(infostream.str());

    if (stateT.size()) {
      infostream.str("");
      infostream << "converted in " << msFiller->ms()->state().nrow() << " state(s) in the measurement set.";
      info(infostream.str());
    }

    infostream.str("");
    infostream << "The dataset has " << mainT.size() << " main(s)...";
    info(infostream.str());

    if (mainT.size()) {
      for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
	   iter != msFillers.end();
	   ++iter) {
	string kindOfData = (iter->first == AP_UNCORRECTED) ? "wvr uncorrected" : "wvr corrected";
	infostream.str("");
	infostream << "converted in " << iter->second->ms()->nrow() << " main(s) rows in the measurement set containing the " << kindOfData << " data.";
	info(infostream.str());
      }
    }
  }
 
  // Do we also want to store the verbatim copies of some tables of the ASDM dataset ?
  if (vm.count("asis")) {
    istringstream iss;
    iss.str(vm["asis"].as<string>());
    string word;
    vector<string> tablenames;
    while (iss>>word)
      tablenames.push_back(word);
    
    ASDMVerbatimFiller avf(const_cast<casa::MS*>(msFillers.begin()->second->ms()), Name2Table::find(tablenames, verbose));
    avf.fill(*ds);
  }
  
  for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
       iter != msFillers.end();
       ++iter)
    iter->second->end(0.0);
  
  
  for (map<AtmPhaseCorrection, ASDM2MSFiller*>::iterator iter = msFillers.begin();
       iter != msFillers.end();
       ++iter)
    delete iter->second;
  delete ds;
  return 0;
}
