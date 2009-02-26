// -*- mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// --------------------------------------------------------------------------
//                   OpenMS Mass Spectrometry Framework
// --------------------------------------------------------------------------
//  Copyright (C) 2003-2009 -- Oliver Kohlbacher, Knut Reinert
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
// $Maintainer: Johannes Junker $
// --------------------------------------------------------------------------


#ifndef OPENMS_VISUAL_TOPPASWIDGET_H
#define OPENMS_VISUAL_TOPPASWIDGET_H

#include <OpenMS/DATASTRUCTURES/Param.h>

#include <QtGui/QGraphicsView>

namespace OpenMS
{
  /**
  	@brief Widget visualizing and allowing to edit TOPP pipelines.
  */
  class OPENMS_DLLAPI TOPPASWidget
  	: public QGraphicsView
  {
      Q_OBJECT

    public:
      /// Default constructor
      TOPPASWidget(const Param& preferences, QWidget* parent = 0);

      /// Destructor
      virtual ~TOPPASWidget();
      
      /// Widget id used as identifier
			Int window_id;
		
		signals:
			/// Emits a status message that should be displayed for @p time ms. If @p time is 0 the message should be displayed until the next message is emitted.
			void sendStatusMessage(std::string, OpenMS::UInt);
			/// Emitted when the cursor position changes (for displaying e.g. in status bar)
			void sendCursorStatus(double x=0.0, double y=0.0);
			/// Message about the destruction of this widget
		  void aboutToBeDestroyed(int w_id);
  };
}

#endif