//# Copyright (C) 2005
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#

#include "ExternalAxisWidgetBottom.h"
#include <QDebug>
#include <QPainter>
#include <qwt_plot_canvas.h>
#include <qwt_scale_div.h>

namespace casa {

ExternalAxisWidgetBottom::ExternalAxisWidgetBottom( QWidget* parent ):
	ExternalAxisWidget( parent ){
	setFixedHeight( AXIS_SMALL_SIDE / 2 );
	setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );

}

void ExternalAxisWidgetBottom::defineAxis( QLine& axisLine ){
	const int MARGIN = 5;
	int left = MARGIN;
	int right = width() - MARGIN;
	int top = 1;
	int bottom = 1;
	QPoint firstPt( left, top );
	QPoint secondPt( right, bottom );
	axisLine.setP1( firstPt );
	axisLine.setP2( secondPt );
}

void ExternalAxisWidgetBottom::drawTick( QPainter* painter, int xPixel, double value,
		int tickLength ){
	int yEnd = 0;
	painter->drawLine( xPixel, yEnd, xPixel, yEnd + tickLength  );
	QFont font = painter->font();
	QString tickLabel( QString::number( value ) );
	QRect fontBoundingRect = QFontMetrics(font).boundingRect( tickLabel );
	int letterWidth = fontBoundingRect.width();
	int letterHeight = fontBoundingRect.height();
	painter->drawText( xPixel - letterWidth / 2, yEnd + tickLength + letterHeight, tickLabel );
}

void ExternalAxisWidgetBottom::drawTicks( QPainter* painter, int tickLength ){

	int MARGIN = 5;

	//First tick
	double lowerBound = scaleDiv->lowerBound();
	int startPixelX = MARGIN;
	drawTick( painter, startPixelX, lowerBound, tickLength );

	//Middle ticks
	const QList<double> axisTicks = scaleDiv->ticks(QwtPlot::yLeft);
	int originalTickCount = axisTicks.size();
	//We don't want to draw too many ticks so adjust the number
	//of ticks we draw accordingly.
	int tickIncrement = getTickIncrement( originalTickCount, true );
	int xIncrement = canvas->width() / (originalTickCount + 1);
	for ( int i = tickIncrement-1; i < originalTickCount; i = i + tickIncrement ){
		int tickPosition = startPixelX + (i+1)*xIncrement;
		drawTick( painter, tickPosition, axisTicks[i], tickLength);
	}

}

void ExternalAxisWidgetBottom::drawAxisLabel( QPainter* painter ){
	 QFont font = painter->font();
	 QRect fontBoundingRect = QFontMetrics(font).boundingRect( axisLabel );
	 int yPosition = height() / 2;
	 int xPosition = canvas->width()/2 - fontBoundingRect.width() / 2;
	 painter->drawText( xPosition, yPosition, fontBoundingRect.width(), height(),
					  Qt::AlignHCenter|Qt::AlignTop, axisLabel);
		 painter->rotate(-90);
}

ExternalAxisWidgetBottom::~ExternalAxisWidgetBottom() {
}

} /* namespace casa */
