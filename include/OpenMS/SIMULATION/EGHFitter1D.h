// -*- mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// --------------------------------------------------------------------------
//                   OpenMS Mass Spectrometry Framework
// --------------------------------------------------------------------------
//  Copyright (C) 2003-2010 -- Oliver Kohlbacher, Knut Reinert
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// --------------------------------------------------------------------------
// $Maintainer: Stephan Aiche $
// $Authors: Stephan Aiche $
// --------------------------------------------------------------------------

#ifndef OPENMS_SIMULATION_EGHFITTER1D_H
#define OPENMS_SIMULATION_EGHFITTER1D_H

#define DEBUG_FEATUREFINDER

#include <OpenMS/TRANSFORMATIONS/FEATUREFINDER/LevMarqFitter1D.h>

namespace OpenMS
{
    /**
      @brief Exponential-Gaussian hybrid distribution fitter (1-dim.) using Levenberg-Marquardt algorithm (GSL implementation) for parameter optimization.

      @htmlinclude OpenMS_EGHFitter1D.parameters
    */
  class OPENMS_DLLAPI EGHFitter1D
  : public LevMarqFitter1D
  {
    public:
    /// Default constructor
    EGHFitter1D();

    /// copy constructor
    EGHFitter1D(const EGHFitter1D& source);

    /// destructor
    virtual ~EGHFitter1D();

    /// assignment operator
    virtual EGHFitter1D& operator = (const EGHFitter1D& source);

    /// create new EGHFitter1D object (function needed by Factory)
    static Fitter1D* create()
    {
      return new EGHFitter1D();
    }

    /// name of the model (needed by Factory)
    static const String getProductName()
    {
      return "EGHFitter1D";
    }

    /// return interpolation model
    QualityType fit1d(const RawDataArrayType& range, InterpolationModel*& model);

    protected:

    /// Helper struct (contains the size of an area and a raw data container)
    struct Data
    {
      typedef Peak1D PeakType;
      typedef std::vector<PeakType > RawDataArrayType;

      Size n;
      RawDataArrayType set;
    };

    /// Compute start parameter
    virtual void setInitialParameters_(const RawDataArrayType& set);

    /// Evaluation of the target function for nonlinear optimization
    static Int residual_(const gsl_vector* x, void* params, gsl_vector* f);

    /// Compute the Jacobian matrix, where each row of the matrix corresponds to a point in the data
    static Int jacobian_(const gsl_vector* x, void* params, gsl_matrix* J);

    /// Driver function for the evaluation of function and jacobian
    static Int evaluate_(const gsl_vector* x, void* params, gsl_vector* f, gsl_matrix* J);

    /** Diplay the intermediate state of the solution. The solver state contains
    the vector s->x which is the current position, and the vector s->f with
    corresponding function values */
    void printState_(Int iter, gsl_multifit_fdfsolver * s);

    /// Parameter of egh - peak height
    CoordinateType height_;
    /// Parameter of egh - tau
    CoordinateType tau_;
    /// Parameter of egh - sigma-square
    CoordinateType sigma_square_;
    /// Parameter of egh - peak retention time
    CoordinateType retention_;

    void updateMembers_();
};

}

#endif // OPENMS_SIMULATION_EGHFITTER1D_H