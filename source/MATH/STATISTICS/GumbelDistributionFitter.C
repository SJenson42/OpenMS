// --------------------------------------------------------------------------
//                   OpenMS -- Open-Source Mass Spectrometry
// --------------------------------------------------------------------------
// Copyright The OpenMS Team -- Eberhard Karls University Tuebingen,
// ETH Zurich, and Freie Universitaet Berlin 2002-2013.
//
// This software is released under a three-clause BSD license:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of any author or any participating institution
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// For a full list of authors, refer to the file AUTHORS.
// --------------------------------------------------------------------------
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: David Wojnar $
// $Authors: David Wojnar $
// --------------------------------------------------------------------------
//

#include <sstream>

#include <cmath>

#include <gsl/gsl_sf_psi.h>
#include <OpenMS/MATH/STATISTICS/GumbelDistributionFitter.h>

using namespace std;

#define GUMBEL_DISTRIBUTION_FITTER_VERBOSE
#undef  GUMBEL_DISTRIBUTION_FITTER_VERBOSE

#ifdef GUMBEL_DISTRIBUTION_FITTER_VERBOSE
#include <iostream>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#endif

namespace OpenMS
{
  namespace Math
  {
    GumbelDistributionFitter::GumbelDistributionFitter()
    {
      init_param_.a = 0.25;
      init_param_.b = 0.1;
    }

    GumbelDistributionFitter::~GumbelDistributionFitter()
    {
    }

    void GumbelDistributionFitter::setInitialParameters(const GumbelDistributionFitResult & param)
    {
      init_param_.a = param.a;
      init_param_.b = param.b;
    }

    const String & GumbelDistributionFitter::getGnuplotFormula() const
    {
      return gnuplot_formula_;
    }

    int GumbelDistributionFitter::gumbelDistributionFitterf_(const gsl_vector * x, void * params, gsl_vector * f)
    {
      vector<DPosition<2> > * data = static_cast<vector<DPosition<2> > *>(params);

      double a = gsl_vector_get(x, 0);
      double b = gsl_vector_get(x, 1);

      UInt i = 0;
      for (vector<DPosition<2> >::iterator it = data->begin(); it != data->end(); ++it)
      {
        double the_x = it->getX();
        double z = exp((a - the_x) / b);
        gsl_vector_set(f, i++, (z * exp(-1 * z)) / b - it->getY());
      }

      return GSL_SUCCESS;
    }

    // compute Jacobian matrix for the different parameters
    int GumbelDistributionFitter::gumbelDistributionFitterdf_(const gsl_vector * x, void * params, gsl_matrix * J)
    {
      vector<DPosition<2> > * data = static_cast<vector<DPosition<2> > *>(params);

      double a = gsl_vector_get(x, 0);
      double b = gsl_vector_get(x, 1);
      UInt i(0);
      for (vector<DPosition<2> >::iterator it = data->begin(); it != data->end(); ++it, ++i)
      {
        double the_x = it->getX();
        double z = exp((a - the_x) / b);
        double f = z * exp(-1 * z);
        double part_dev_a = (f - pow(z, 2) * exp(-1 * z)) / pow(b, 2);
        gsl_matrix_set(J, i, 0, part_dev_a);
        double dev_z =  ((the_x - a) / pow(b, 2));
        double cum = f * dev_z;
        double part_dev_b = ((cum - z * cum) * b - f) / pow(b, 2);
        gsl_matrix_set(J, i, 1, part_dev_b);




      }
      return GSL_SUCCESS;
    }

    int GumbelDistributionFitter::gumbelDistributionFitterfdf_(const gsl_vector * x, void * params, gsl_vector * f, gsl_matrix * J)
    {
      gumbelDistributionFitterf_(x, params, f);
      gumbelDistributionFitterdf_(x, params, J);
      return GSL_SUCCESS;
    }

#ifdef GUMBEL_DISTRIBUTION_FITTER_VERBOSE
    void GumbelDistributionFitter::printState_(size_t iter, gsl_multifit_fdfsolver * s)
    {
      printf("iter: %3u x = % 15.8f % 15.8f "
             "|f(x)| = %g\n",
             (unsigned int)iter,
             gsl_vector_get(s->x, 0),
             gsl_vector_get(s->x, 1),
             gsl_blas_dnrm2(s->f));
    }

#endif

    GumbelDistributionFitter::GumbelDistributionFitResult GumbelDistributionFitter::fit(vector<DPosition<2> > & input)
    {
      const gsl_multifit_fdfsolver_type * T = NULL;
      gsl_multifit_fdfsolver * s = NULL;

      int status = 0;
      size_t iter = 0;

      const size_t p = 2;

      gsl_multifit_function_fdf f;
      double x_init[2] = { init_param_.a, init_param_.b };
      gsl_vector_view x = gsl_vector_view_array(x_init, p);
      const gsl_rng_type * type = NULL;
      gsl_rng * r = NULL;

      gsl_rng_env_setup();

      type = gsl_rng_default;
      r = gsl_rng_alloc(type);

      f.f = &gumbelDistributionFitterf_;
      f.df = &gumbelDistributionFitterdf_;
      f.fdf = &gumbelDistributionFitterfdf_;
      f.n = input.size();
      f.p = p;
      f.params = &input;

      T = gsl_multifit_fdfsolver_lmsder;
      s = gsl_multifit_fdfsolver_alloc(T, input.size(), p);
      gsl_multifit_fdfsolver_set(s, &f, &x.vector);

#ifdef GUMBEL_DISTRIBUTION_FITTER_VERBOSE
      printState_(iter, s);
#endif

      do
      {
        ++iter;
        status = gsl_multifit_fdfsolver_iterate(s);

#ifdef GUMBEL_DISTRIBUTION_FITTER_VERBOSE
        printf("status = %s\n", gsl_strerror(status));
        printState_(iter, s);
#endif
        if (status)
        {
          break;
        }

        status = gsl_multifit_test_delta(s->dx, s->x, 1e-4, 1e-4);
#ifdef GUMBEL_DISTRIBUTION_FITTER_VERBOSE
        printf("Status = '%s'\n", gsl_strerror(status));
#endif
      }
      while (status == GSL_CONTINUE && iter < 1000);

#ifdef GUMBEL_DISTRIBUTION_FITTER_VERBOSE
      printf("Final status = '%s'\n", gsl_strerror(status));
#endif

      if (status != GSL_SUCCESS)
      {
        gsl_rng_free(r);
        gsl_multifit_fdfsolver_free(s);

        throw Exception::UnableToFit(__FILE__, __LINE__, __PRETTY_FUNCTION__, "UnableToFit-GumbelDistributionFitter", "Could not fit the gumbel distribution to the data");
      }

      // write the result in a GumbelDistributionFitResult struct
      GumbelDistributionFitResult result;
      result.a = gsl_vector_get(s->x, 0);
      result.b = gsl_vector_get(s->x, 1);

      // build a formula with the fitted parameters for gnuplot
      stringstream formula;
      formula << "f(x)=" << "(1/" << result.b << ") * " << "exp(( " << result.a << "- x)/" << result.b << ") * exp(-exp((" << result.a << " - x)/" << result.b << "))";
      gnuplot_formula_ = formula.str();

#ifdef GUMBEL_DISTRIBUTION_FITTER_VERBOSE
      cout << gnuplot_formula_ << endl;
#endif

      gsl_rng_free(r);
      gsl_multifit_fdfsolver_free(s);

      return result;
    }

  }   //namespace Math
} // namespace OpenMS
