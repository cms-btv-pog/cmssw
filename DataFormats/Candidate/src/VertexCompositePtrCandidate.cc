#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "MagneticField/UniformEngine/src/UniformMagneticField.h"
#include "TrackingTools/TrajectoryParametrization/interface/GlobalTrajectoryParameters.h"
#include "TrackingTools/AnalyticalJacobians/interface/JacobianCurvilinearToCartesian.h"
#include "TrackingTools/AnalyticalJacobians/interface/JacobianCartesianToCurvilinear.h"

using namespace reco;

VertexCompositePtrCandidate::VertexCompositePtrCandidate(Charge q, const LorentzVector & p4, const Point & vtx,
						   const CovarianceMatrix & err, double chi2, double ndof,
						   int pdgId, int status, bool integerCharge) :
  CompositePtrCandidate(q, p4, vtx, pdgId, status, integerCharge),
  chi2_(chi2), ndof_(ndof), createdTrack_(false), hasTrack_(false) {
  setCovariance(err);
}

VertexCompositePtrCandidate::~VertexCompositePtrCandidate() { }

VertexCompositePtrCandidate * VertexCompositePtrCandidate::clone() const { 
  return new VertexCompositePtrCandidate(*this); 
}

void VertexCompositePtrCandidate::fillVertexCovariance(CovarianceMatrix& err) const {
  index idx = 0;
  for(index i = 0; i < dimension; ++i) 
    for(index j = 0; j <= i; ++ j)
      err(i, j) = covariance_[idx++];
}

void VertexCompositePtrCandidate::setCovariance(const CovarianceMatrix & err) {
  index idx = 0;
  for(index i = 0; i < dimension; ++i) 
    for(index j = 0; j <= i; ++j)
      covariance_[idx++] = err(i, j);
}

void VertexCompositePtrCandidate::createTrack() const {

  int nTracks = 0;
  const std::shared_ptr<MagneticField> field( new UniformMagneticField(3.8) );

  // total 6x6 Cartesian covariance matrix
  // defined as the sum of individual covariance matrices, i.e.
  // cov(p) = cov(Sum_i p_i) = Sum_i cov(p_i) assuming cov(p_i,p_j) = 0.
  AlgebraicSymMatrix66 totCartError;

  // loop over daughters
  for(index i=0; i<numberOfDaughters(); ++i) {

    // try to get a pointer to the track
    const reco::Track* track = daughterPtr(i)->bestTrack();
    if(track) { // if track exists
      ++nTracks;

      // get 5x5 curvilinear covariance matrix
      AlgebraicSymMatrix55 curvError;
      for(int i=0; i<5; ++i) {
        for(int j=i; j<5; ++j) {
          curvError(i,j) = track->covariance(i,j);
        }
      }

      GlobalPoint  vtx( track->vx(), track->vy(), track->vz() );
      GlobalVector mom( track->px(), track->py(), track->pz() );
      GlobalTrajectoryParameters gtp(vtx, mom, track->charge(), &*field);

      // define Jacobian to tranform 5x5 curvilinear to 6x6 Cartesian covariance matrix
      JacobianCurvilinearToCartesian curv2Cart(gtp);
      const AlgebraicMatrix65& jac = curv2Cart.jacobian();
      // tranform 5x5 curvilinear to 6x6 Cartesian covariance matrix and add it to the total covariance matrix
      totCartError += ROOT::Math::Similarity(jac, curvError);
    }
  }

  if(nTracks) {
    // 6x6 Cartesian covariance matrix for the vertex candidate
    AlgebraicSymMatrix66 cartError;
    // upper 3x3 submatrix taken from the vertex covariance
    for(int i=0; i<3; ++i) {
      for(int j=i; j<3; ++j) {
        cartError(i,j) = vertexCovariance(i,j);
      }
    }
    // lower 3x3 submatrix taken from the momentum covariance
    for(int i=3; i<6; ++i) {
      for(int j=i; j<6; ++j) {
        cartError(i,j) = totCartError(i,j);
      }
    }

    GlobalPoint  vtx( vx(), vy(), vz() );
    GlobalVector mom( px(), py(), pz() );
    GlobalTrajectoryParameters gtp(vtx, mom, 0, &*field);

    // define Jacobian to tranform 6x6 Cartesian to 5x5 curvilinear covariance matrix
    JacobianCartesianToCurvilinear cart2Curv(gtp);
    const AlgebraicMatrix56& jac = cart2Curv.jacobian();
    // tranform 6x6 Cartesian to 5x5 curvilinear covariance matrix and put it in the appropriate format for the track creation
    AlgebraicSymMatrix55 curvError = ROOT::Math::Similarity(jac, cartError);
    reco::TrackBase::CovarianceMatrix m;
    for(int i=0; i<5; ++i) {
      for(int j=i; j<5; ++j) {
        m(i,j) = curvError(i,j);
      }
    }

    // create track
    track_ = reco::Track(vertexChi2(),vertexNdof(),math::XYZPoint(vx(),vy(),vz()),math::XYZVector(px(),py(),pz()),0,m,reco::TrackBase::undefAlgorithm,reco::TrackBase::loose);

    hasTrack_ = true;
  }
  
  createdTrack_ = true;
}

