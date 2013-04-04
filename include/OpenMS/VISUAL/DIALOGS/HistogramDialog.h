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
// $Maintainer: Timo Sachsenberg $
// $Authors: Marc Sturm $
// --------------------------------------------------------------------------


#ifndef OPENMS_VISUAL_DIALOGS_HISTOGRAMDIALOG_H
#define OPENMS_VISUAL_DIALOGS_HISTOGRAMDIALOG_H

#include <QtGui/QDialog>

#include <OpenMS/MATH/STATISTICS/Histogram.h>
#include <OpenMS/VISUAL/HistogramWidget.h>

namespace OpenMS
{
  /**
      @brief Dialog that show a HistogramWidget.

      @ingroup Dialogs
  */
  class OPENMS_GUI_DLLAPI HistogramDialog :
    public QDialog
  {
    Q_OBJECT

public:
    /// Constructor
    HistogramDialog(const Math::Histogram<> & distribution, QWidget * parent = 0);
    /// Destructor
    ~HistogramDialog();

    /// Returns the value of the left splitter
    Real getLeftSplitter();
    /// Returns the value of the right splitter
    Real getRightSplitter();

    /// Sets the value of the left splitter
    void setLeftSplitter(Real position);
    /// Sets the value of the right splitter
    void setRightSplitter(Real position);

    /// Sets the axis legend
    void setLegend(const String & legend);
    /// Sets log mode
    void setLogMode(bool log_mode);

protected:
    HistogramWidget * mw_;
  };

} //namespace

#endif //OPENMS_VISUAL_DIALOGS_HISTOGRAMDIALOG_H
