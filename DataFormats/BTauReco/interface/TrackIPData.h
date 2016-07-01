#ifndef TrackIPData_h
#define TrackIPData_h

#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"

#include "DataFormats/JetReco/interface/JetTracksAssociation.h"

namespace reco {


struct TrackIPData
{ 
  Measurement1D AbsimpactParameter3D;
  Measurement1D AbsimpactParameter2D;
  Measurement1D AbsimpactParameter1D;

  Measurement1D impactParameter3D;
  Measurement1D impactParameter2D;
  Measurement1D impactParameter1D;
  //float decayLen;
  //float decayLenError;
};

}
#endif

