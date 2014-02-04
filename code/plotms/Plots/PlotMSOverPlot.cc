//# PlotMSOverPlot.cc: Subclass of PlotMSPlot for a single plot/canvas.
//# Copyright (C) 2012
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
//# $Id: $
#include <plotms/Plots/PlotMSOverPlot.h>


#include <plotms/PlotMS/PlotMS.h>
#include <plotms/Plots/PlotMSPlotParameterGroups.h>
#include <plotms/Data/CacheFactory.h>
#include <plotms/Data/PlotMSCacheBase.h>
#include <plotms/Data/MSCache.h>
#include <plotms/Data/CalCache.h>
#include <QDebug>

#include <algorithm>
#include <cmath>

#define THREADLOAD True

namespace casa {

////////////////////////////////
// PlotMSOverPlot Definitions //
////////////////////////////////

// Static

PlotMSPlotParameters PlotMSOverPlot::makeParameters(PlotMSApp *plotms) {
	PlotMSPlotParameters p = PlotMSPlot::makeParameters(plotms);
	makeParameters(p, plotms);
	return p;
}

void PlotMSOverPlot::makeParameters(PlotMSPlotParameters &params,
		PlotMSApp *plotms) {
	PlotMSPlot::makeParameters(params, plotms);

	// Add cache parameters if needed
	if(params.typedGroup<PMS_PP_Cache>() == NULL)
		params.setGroup<PMS_PP_Cache>();

	// Add axes parameters if needed.
	if(params.typedGroup<PMS_PP_Axes>() == NULL)
		params.setGroup<PMS_PP_Axes>();

	// Add canvas parameters if needed.
	if(params.typedGroup<PMS_PP_Canvas>() == NULL)
		params.setGroup<PMS_PP_Canvas>();

	// Add display parameters if needed.
	if(params.typedGroup<PMS_PP_Display>() == NULL)
		params.setGroup<PMS_PP_Display>();

	// Add iteration parameters if needed.
	if(params.typedGroup<PMS_PP_Iteration>() == NULL)
		params.setGroup<PMS_PP_Iteration>();
}

// Constructors/Destructors

PlotMSOverPlot::PlotMSOverPlot(PlotMSApp *parent)
:
    		PlotMSPlot(parent),
    		iter_(0),
    		iterStep_(1) {
	constructorSetup();
}

PlotMSOverPlot::~PlotMSOverPlot() {
}

String PlotMSOverPlot::name() const {
	const PMS_PP_MSData *data = itsParams_.typedGroup<PMS_PP_MSData>();
	const PMS_PP_Cache *cache = itsParams_.typedGroup<PMS_PP_Cache>();
	const PMS_PP_Display *display = itsParams_.typedGroup<PMS_PP_Display>();

	if(data == NULL || cache == NULL || display == NULL || !data->isSet())
		return "Over Plot";
	return display->titleFormat().getLabel(cache->xAxis(), cache->yAxis());
	//return "Over Plot for " + data->filename();
}

vector<MaskedScatterPlotPtr> PlotMSOverPlot::plots() const {
	if((itsPlots_.size() == 0) || (itsPlots_[0].size() == 0))
		return vector<MaskedScatterPlotPtr>();
	uInt index = 0;
	uInt nIter = itsCache_->nIter();
	vector<MaskedScatterPlotPtr> v(nIter);
	for(unsigned int i = 0; i < itsPlots_.size(); i++) {
		for(unsigned int j = 0; j < itsPlots_[i].size(); j++) {
			if(index >= nIter) break;
			v[index] = itsPlots_[i][j];
			++index;
		}
	}
	return v;
}

vector<PlotCanvasPtr> PlotMSOverPlot::canvases() const {

	if(( itsCanvases_.size() == 0) || (itsCanvases_[0].size() == 0)){
		return vector<PlotCanvasPtr>();
	}
	uInt index = 0;
	uInt nIter = itsCache_->nIter();
	int canvasCount = std::min(nIter, uInt(itsCanvases_.size() * itsCanvases_[0].size()));
	vector<PlotCanvasPtr> v( canvasCount );
	for(uInt i = 0; i < itsCanvases_.size(); i++) {
		for(uInt j = 0; j < itsCanvases_[i].size(); j++) {
			if(index >= nIter) break;
			v[index] = itsCanvases_[i][j];
			++index;
		}
	}
	return v;
}



void PlotMSOverPlot::attachToCanvases() {
	Int iter = iter_;
	Int nIter = itsCache_->nIter();
	for(uInt r = 0; (r < itsCanvases_.size()); ++r) {
		for(uInt c = 0; (c < itsCanvases_[r].size()); ++c) {
			if(!itsCanvases_[r][c].null()) {
				if(!itsPlots_[r][c].null() &&
						//We are not iterating OR
						// the iteration index is less than the iteration count
						( nIter == 0 || (iter < nIter))) {
					itsCanvases_[r][c]->plotItem(itsPlots_[r][c]);
					++iter;
				}
				((&*itsCanvases_[r][c]))->show();
				((&*itsCanvases_[r][c]))->setMinimumSize(5,5);
			}
		}
	}
}

void PlotMSOverPlot::detachFromCanvases() {
	for(uInt r = 0; r < itsCanvases_.size(); ++r) {
		for(uInt c = 0; c < itsCanvases_[r].size(); ++c) {
			if(!itsCanvases_[r][c].null()) {
				if(itsCanvases_[r][c]->numPlotItems() > 0) {
					itsCanvases_[r][c]->removePlotItem(itsPlots_[r][c]);
				}
				//This is necessary in scripting mode so that we don't see
				//detached canvases.
				if ( ! itsParent_->guiShown() ){
					((&*itsCanvases_[r][c]))->hide();
				}

			}
		}
	}
}

// Protected members

void PlotMSOverPlot::resize(PlotMSPages &pages, uInt rows, uInt cols) {
	// Resize canvases and plots
	int plotCanvasRowCount = 1;
	int plotCanvasColCount = 1;
	if ( isIteration() ){
		plotCanvasRowCount = rows;
		plotCanvasColCount = cols;

	}

	itsCanvases_.resize( plotCanvasRowCount );
	itsPlots_.resize( plotCanvasRowCount );
	for( int r = 0; r < plotCanvasRowCount; ++r) {
		itsCanvases_[r].resize(plotCanvasColCount);
		itsPlots_[r].resize(plotCanvasColCount);
	}

	// Resize pages
	iterStep_ = plotCanvasRowCount * plotCanvasColCount;
	double nIter = itsCache_->nIter();
	if(nIter == 0) nIter = 1;
	int pageSize = static_cast<int>(ceil(nIter / iterStep_));
	pages.resize( pageSize );
	for(size_t i = 0; i < pages.totalPages(); ++i) {
		pages[i].resize(rows, cols);
	}

}


bool PlotMSOverPlot::assignCanvases(PlotMSPages &pages) {
	if(pages.totalPages() == 0) {
		pages.insertPage();
		pages.firstPage();
	}

	//Resize based on the row and column count
	PlotMSParameters params = itsParent_->getParameters();
	uInt rows = params.getRowCount();
	uInt cols = params.getColCount();
	resize( pages, rows, cols );
	PlotMSPage& page = pages[pages.currentPageNumber()];

	const PMS_PP_Iteration* iterParams = itsParams_.typedGroup<PMS_PP_Iteration>();
	uInt rowIndex = 0;
	uInt colIndex = 0;
	if ( iterParams != NULL ){
		rowIndex = static_cast<uInt>(iterParams->getGridRow());
		colIndex = static_cast<uInt>(iterParams->getGridCol());
	}

	//Find a canvas for this plot.
	for(uInt r = 0; r < rows; ++r) {
		bool assigned = false;
		for(uInt c = 0; c < cols; ++c) {
			if ( isIteration() ){
				if( !page.isOwned(r, c)) {
					page.setOwner(r, c, this);
				}
				itsCanvases_[r][c] = page.canvas(r, c);
			}
			else {
				//If it is not an iteration plot, there is just
				//one canvas for this plot.
				if ( rowIndex == r && colIndex == c){
					page.disown( this );
					page.setOwner(r, c, this);
					itsCanvases_[0][0] = page.canvas(r,c);
					assigned = true;
					break;
				}
			}
		}
		if ( assigned ){
			break;
		}
	}
	page.setupPage();
	return true;
}


void PlotMSOverPlot::getPlotSize( Int& rows, Int& cols ){
	rows = 1;
	cols = 1;
	if ( isIteration() ){
		PlotMSParameters params = itsParent_->getParameters();
		rows = params.getRowCount();
		cols = params.getColCount();
	}
}

bool PlotMSOverPlot::initializePlot() {

	Int rows = 1;
	Int cols = 1;
	getPlotSize( rows, cols );
	uInt iter = 0;
	uInt nIter = itsCache_->nIter();
	for(Int r = 0; r < rows; ++r) {
		for(Int c = 0; c < cols; ++c) {
			PlotMaskedPointDataPtr data(&(itsCache_->indexer0()), false);
			itsPlots_[r][c] = itsFactory_->maskedPlot(data);
			++iter;

			// We want to execute this loop at least once to fill in
			// a single plot scenario; but after that, if there are
			// no iterations, break out
			if(iter >= nIter) break;
		}
		if(iter >= nIter) break;
	}

	setColors();
	return true;
}

bool PlotMSOverPlot::isIteration() const {
	const PMS_PP_Iteration *iter = itsParams_.typedGroup<PMS_PP_Iteration>();
	bool iterationPlot = false;
	if ( iter != NULL ){
		iterationPlot = iter->isIteration();
	}
	return iterationPlot;
}

void PlotMSOverPlot::updateLocation(){

	PlotMSPages &pages = itsParent_->getPlotManager().itsPages_;
	assignCanvases(pages);
	//Put the plot data on the canvas.
	attachToCanvases();
	//Put the plot axis on the canvas.  For scripting mode, we get plots without
	//axes if the call is not preset.  In Gui mode, we get an exception if it is
	//called for the case where the number of plots in the grid is going down.  We
	//seem to need it when the number of plots is going up. However, when reload is
	//checked, it seems to work.
	if ( !itsParent_->guiShown()  ){
		updateCanvas();
	}


}

bool PlotMSOverPlot::parametersHaveChanged_(const PlotMSWatchedParameters &p,
		int updateFlag,
		bool releaseWhenDone) {

	if(&p != &itsParams_) {
		return false;
	}

	const PMS_PP_MSData *data = itsParams_.typedGroup<PMS_PP_MSData>();
	const PMS_PP_Iteration *iter = itsParams_.typedGroup<PMS_PP_Iteration>();
	const PMS_PP_Axes *axes = itsParams_.typedGroup<PMS_PP_Axes>();

	if(data == NULL || iter == NULL || axes == NULL ){
		return true;
	}
	itsTCLParams_.releaseWhenDone = releaseWhenDone;
	itsTCLParams_.updateCanvas = (updateFlag & PMS_PP::UPDATE_AXES) ||
			(updateFlag & PMS_PP::UPDATE_CACHE) ||
			(updateFlag & PMS_PP::UPDATE_CANVAS) ||
			(updateFlag & PMS_PP::UPDATE_ITERATION) ||
			(updateFlag & PMS_PP::UPDATE_MSDATA) || !data->isSet();

	itsTCLParams_.updateDisplay = updateFlag & PMS_PP::UPDATE_DISPLAY;
	itsTCLParams_.endCacheLog = false;


	// Clear selection if axes change
	//if(updateFlag & PMS_PP::UPDATE_AXES) {
	// Apparently UPDATE_AXES is not triggered by anything...
	// UPDATE_CACHE should be close enough for now (I hope)
	if(updateFlag & PMS_PP::UPDATE_CACHE) {
		for(size_t r = 0; r < itsCanvases_.size(); ++r) {
			for(size_t c = 0; c < itsCanvases_[r].size(); ++c) {
				PlotCanvasPtr plotCanvas = itsCanvases_[r][c];
				if ( ! plotCanvas.null() ){
					plotCanvas->standardMouseTools()->selectTool()->clearSelectedRects();
					plotCanvas->clearAnnotations();
				}
			}
		}
	}

	//See if the iteration parameters have changed.
	bool commonAxisX = iter->isCommonAxisX();
	bool commonAxisY = iter->isCommonAxisY();
	Int rows = 0;
	Int cols = 0;
	getPlotSize( rows, cols );
	PlotAxis locationAxisX = axes->xAxis();
	PlotAxis locationAxisY = axes->yAxis();
	int displayRow = iter->getGridRow();
	int displayCol = iter->getGridCol();
	int plotRows = itsPlots_.size();
	int plotCols = 0;
	if ( plotRows > 0 ){
		plotCols = itsPlots_[0].size();
	}
	bool locationChange = false;
	if ( gridRow != displayRow || gridCol != displayCol ){
		locationChange = true;
	}

	bool updateIter = updateFlag & PMS_PP::UPDATE_ITERATION;
	itsTCLParams_.updateIteration = ( updateIter || ((plotRows != rows) || (plotCols != cols)) ||
					(itsParent_->isCommonAxisX() != commonAxisX) ||
					(itsParent_->isCommonAxisY() != commonAxisY) ||
					(itsParent_->getAxisLocationX() != locationAxisX) ||
					(itsParent_->getAxisLocationY() != locationAxisY) ||
					locationChange );
	itsParent_->setCommonAxes( commonAxisX, commonAxisY);
	itsParent_->setAxisLocation( locationAxisX, locationAxisY);
	gridRow = rows;
	gridCol = cols;

	bool dataSet = data->isSet();
	bool updateData = (updateFlag & PMS_PP::UPDATE_MSDATA) || (updateFlag & PMS_PP::UPDATE_CACHE);

	// Update cache if needed
	bool handled = true;
	if( dataSet && updateData ) {
		try {
			handled = !updateCache();
		}
		catch( AipsError& error ){
			cerr << "Could not update cache: "<<error.getMesg().c_str()<<endl;
			cacheLoaded_(false);
		}
	}
	else {
		cacheLoaded_(false);
	}
	return handled;
}

PlotMSRegions PlotMSOverPlot::selectedRegions(
		const vector<PlotCanvasPtr>& canvases) const {
	PlotMSRegions r;
	PMS::Axis x = (PMS::Axis)PMS_PP_RETCALL(itsParams_, PMS_PP_Cache,
			xAxis, 0);
	PMS::Axis y = (PMS::Axis)PMS_PP_RETCALL(itsParams_, PMS_PP_Cache,
			yAxis, 0);

	for(uInt i = 0; i < canvases.size(); ++i) {
		r.addRegions(x, y, canvases[i]);
	}
	return r;
}

void PlotMSOverPlot::constructorSetup() {
	PlotMSPlot::constructorSetup();
	gridRow = -1;
	gridCol = -1;
	makeParameters(itsParams_, itsParent_);
}

// Private Methods //

bool PlotMSOverPlot::updateCache() {
	PMS_PP_MSData* data = itsParams_.typedGroup<PMS_PP_MSData>();
	PMS_PP_Cache* cache = itsParams_.typedGroup<PMS_PP_Cache>();
	PMS_PP_Iteration* iter = itsParams_.typedGroup<PMS_PP_Iteration>();
	if(data == NULL || cache == NULL || iter == NULL){
		return false;
	}

	// Don't load if data isn't set or there was an error during data opening.
	if(!data->isSet()) return false;
	// Trap bad averaging/iteration combo
	if (data->averaging().baseline() &&
			iter->iterationAxis()==PMS::ANTENNA) {
		stringstream ss;
		ss << "Cannot iterate on Antenna if averaging over baseline, "
				<< "so turning off iteration.";
		itsParent_->getLogger()->postMessage(PMS::LOG_ORIGIN,
				PMS::LOG_ORIGIN_PLOT,
				ss.str(),
				PMS::LOG_EVENT_PLOT);
		iter->setIterationAxis(PMS::NONE);
	}

	// Notify the plots that the data will change
	updatePlots();

	// Set up cache loading parameters
	if(cache->numXAxes() != cache->numYAxes()){
		return false;
	}
	vector<PMS::Axis> caxes(cache->numXAxes() + cache->numYAxes());
	vector<PMS::DataColumn> cdata(cache->numXAxes() + cache->numYAxes());
	for(uInt i = 0; i < cache->numXAxes(); ++i) {
		caxes[i] = cache->xAxis(i);
		cdata[i] = cache->xDataColumn(i);
	}
	for(uInt i = cache->numYAxes(); i < caxes.size(); ++i) {
		caxes[i] = cache->yAxis(i - cache->numXAxes());
		cdata[i] = cache->yDataColumn(i - cache->numXAxes());
	}

	itsParent_->getLogger()->markMeasurement(PMS::LOG_ORIGIN,
			PMS::LOG_ORIGIN_LOAD_CACHE,
			PMS::LOG_EVENT_LOAD_CACHE);
	itsTCLParams_.endCacheLog = true;

	// Delete existing cache if it doesn't match
	if (CacheFactory::needNewCache(itsCache_, data->filename())) {
		if(itsCache_) {
			delete itsCache_;
			itsCache_ = NULL;
		}
		itsCache_ = CacheFactory::getCache(data->filename(), itsParent_);
		if(itsCache_ == NULL) {
			throw AipsError("Failed to create a new Cache object!");
		}
	}


	bool result = itsParent_->updateCachePlot( this,
			PlotMSOverPlot::cacheLoaded, true );
	return result;
}

bool PlotMSOverPlot::updateCanvas() {

	bool set = PMS_PP_RETCALL(itsParams_, PMS_PP_MSData, isSet, false);

	PMS_PP_Axes *axes = itsParams_.typedGroup<PMS_PP_Axes>();
	PMS_PP_Cache *cache = itsParams_.typedGroup<PMS_PP_Cache>();
	PMS_PP_Canvas *canv = itsParams_.typedGroup<PMS_PP_Canvas>();
	PMS_PP_Iteration *iter = itsParams_.typedGroup<PMS_PP_Iteration>();
	if(axes == NULL || cache == NULL || canv == NULL || iter == NULL ) {
		return false;
	}

	PMS::Axis x = cache->xAxis();
	PMS::Axis y = cache->yAxis();
	PlotAxis cx = axes->xAxis();
	PlotAxis cy = axes->yAxis();

	uInt nIter = itsCache_->nIter();
	uInt rows = itsCanvases_.size();

	for(uInt r = 0; r < rows; ++r) {
		uInt cols = itsCanvases_[r].size();
		uInt iterationRows = iter_ + r * cols;
		if( iterationRows >= nIter /*&& iterationPlot*/ ){
			break;
		}
		for(uInt c = 0; c < cols; ++c) {
			uInt iteration = iterationRows + c;
			if(iteration >= nIter){
				clearCanvasProperties( r, c );
			}
			else {
				setCanvasProperties( r, c, cx, cy, x, y, set, canv,
						rows, cols, axes, iter, iteration );
			}
		}
	}
	return true;
}

void PlotMSOverPlot::clearCanvasProperties( int row, int col){
	PlotCanvasPtr canvas = itsCanvases_[row][col];
	if(canvas.null()){
		return;
	}
	canvas->showAllAxes( false );
	canvas->setTitle( "" );
	canvas->setCommonAxes( false, false );
}

void PlotMSOverPlot::setCanvasProperties (int row, int col,
		PlotAxis cx, PlotAxis cy, PMS::Axis x, PMS::Axis y,
		bool set, PMS_PP_Canvas *canv, uInt rows, uInt cols,
		PMS_PP_Axes *axes, PMS_PP_Iteration *iter, uInt iteration) {
	PlotCanvasPtr canvas = itsCanvases_[row][col];
	if(canvas.null()){
		return;
	}

	// Set axes scales
	canvas->setAxisScale(cx, PMS::axisScale(x));
	canvas->setAxisScale(cy, PMS::axisScale(y));

	// Set reference values
	bool xref = itsCache_->hasReferenceValue(x);
	bool yref = itsCache_->hasReferenceValue(y);
	double xrefval = itsCache_->referenceValue(x);
	double yrefval = itsCache_->referenceValue(y);
	canvas->setAxisReferenceValue(cx, xref, xrefval);
	canvas->setAxisReferenceValue(cy, yref, yrefval);

	// Set axes labels
	canvas->clearAxesLabels();
	if(set) {
		canvas->setAxisLabel(
				cx, canv->xLabelFormat().getLabel(x, xref, xrefval));
		canvas->setAxisLabel(
				cy, canv->yLabelFormat().getLabel(y, yref, yrefval));
		PlotFontPtr xFont = canvas->axisFont(cx);
		PlotFontPtr yFont = canvas->axisFont(cy);
		xFont->setPointSize(std::max(12. - rows*cols+1., 8.));
		yFont->setPointSize(std::max(12. - rows*cols+1., 8.));
		canvas->setAxisFont(cx, xFont);
		canvas->setAxisFont(cy, yFont);
	}

	// Custom axes ranges
	canvas->setAxesAutoRescale(true);
	if(set && axes->xRangeSet() && axes->yRangeSet()) {
		canvas->setAxesRanges(cx, axes->xRange(),
				cy, axes->yRange());
	} else if(set && axes->xRangeSet()) {
		canvas->setAxisRange(cx, axes->xRange());
	} else if(set && axes->yRangeSet()) {
		canvas->setAxisRange(cy, axes->yRange());
	}

	// Show/hide axes

	canvas->showAllAxes(false);
	//ShowX and showY determine whether axes are visible at
	//all.  For visible axes, there is the option of sharing
	//them (common) or for each plot to manage its own.
	bool commonX = iter->isCommonAxisX();
	bool commonY = iter->isCommonAxisY();
	canvas->setCommonAxes( commonX, commonY );
	bool showX = set && canv->xAxisShown();
	bool showY = set && canv->yAxisShown();
	canvas->showAxis(cx, showX);
	canvas->showAxis(cy, showY);

	// Legend
	canvas->showLegend(set && canv->legendShown(),
			canv->legendPosition());

	// Title font
	PlotFontPtr font = canvas->titleFont();
	font->setPointSize(std::max(16. - rows*cols+1., 8.));
	font->setBold(true);
	canvas->setTitleFont(font);
	// Title
	bool resetTitle = set || (iter->iterationAxis() != PMS::NONE);
	String iterTxt;
	if(iter->iterationAxis() != PMS::NONE &&
			itsCache_->nIter() > 0) {
		iterTxt = itsCache_->indexer(iteration).iterLabel();
	}
	String title = "";
	if(resetTitle) {
		title = canv->titleFormat().getLabel(
				x, y, xref, xrefval, yref, yrefval) + " " + iterTxt;
	}
	canvas->setTitle(title);

	// Grids
	canvas->showGrid(canv->gridMajorShown(),
			canv->gridMinorShown(),
			canv->gridMajorShown(),
			canv->gridMinorShown());

	PlotLinePtr major_line =
			itsFactory_->line(canv->gridMajorLine());
	if(!canv->gridMajorShown()) {
		major_line->setStyle(PlotLine::NOLINE);
	}
	canvas->setGridMajorLine(major_line);

	PlotLinePtr minor_line =
			itsFactory_->line(canv->gridMinorLine());
	if(!canv->gridMinorShown()) {
		minor_line->setStyle(PlotLine::NOLINE);
	}
	canvas->setGridMinorLine(minor_line);
}

bool PlotMSOverPlot::updateDisplay() {
	try {
		PMS_PP_Cache *cache = itsParams_.typedGroup<PMS_PP_Cache>();
		PMS_PP_Axes *axes = itsParams_.typedGroup<PMS_PP_Axes>();
		PMS_PP_Display *display = itsParams_.typedGroup<PMS_PP_Display>();
		if(cache == NULL || axes == NULL || display == NULL) return false;

		MaskedScatterPlotPtr plot;
		uInt nIter = itsCache_->nIter();
		uInt rows = itsPlots_.size();
		for(uInt row = 0; row < rows; ++row) {
			uInt cols = itsPlots_[row].size();
			uInt iter = iter_ + row * cols;
			if(iter >= nIter) break;
			for(uInt col = 0; col < itsPlots_[row].size(); ++col) {
				if(iter >= nIter) break;
				// Set symbols.
				PlotSymbolPtr unflaggedSym = display->unflaggedSymbol();
				PlotSymbolPtr symbolUnmasked = itsParent_->createSymbol ( unflaggedSym );
				uInt dataSize = itsCache_->indexer(iter).sizeUnmasked();
				customizeAutoSymbol( symbolUnmasked, dataSize );

				PlotSymbolPtr flaggedSym = display->flaggedSymbol();
				PlotSymbolPtr symbolMasked = itsParent_->createSymbol ( flaggedSym  );

				dataSize = itsCache_->indexer(iter).sizeMasked();
				customizeAutoSymbol( symbolMasked, dataSize );

				plot = itsPlots_[row][col];
				if(plot.null()) continue;

				plot->setSymbol(symbolUnmasked);
				plot->setMaskedSymbol(symbolMasked);

				// Colorize and set data changed, if redraw is needed
				if(nIter > 0 && itsCache_->indexer(iter).colorize(
						display->colorizeFlag(), display->colorizeAxis())) {
					plot->dataChanged();
				}

				// Set item axes
				plot->setAxes(axes->xAxis(), axes->yAxis());

				// Set plot title
				PMS::Axis x = cache->xAxis();
				PMS::Axis y = cache->yAxis();
				plot->setTitle(display->titleFormat().getLabel(
						x, y,
						itsCache_->hasReferenceValue(x),
						itsCache_->referenceValue(x),
						itsCache_->hasReferenceValue(y),
						itsCache_->referenceValue(y)));

				++iter;
			}
		}
	} catch(AipsError &err) {
		itsParent_->showError("Could not update plot: " + err.getMesg());
		return false;
	} catch(...) {
		itsParent_->showError("Could not update plot, for unknown reasons!");
		return false;
	}
	return true;
}

void PlotMSOverPlot::setColors() {
	uInt nIter = itsCache_->nIter();
	uInt rows = itsPlots_.size();
	itsColoredPlots_.resize(rows);
	for(uInt row = 0; row < rows; ++row) {
		uInt cols = itsPlots_[row].size();
		itsColoredPlots_[row].resize(cols);
		for(uInt col = 0; col < cols; ++col) {
			uInt iteration = iter_ + row * cols + col;
			if(iteration >= nIter) break;
			itsColoredPlots_[row][col] = ColoredPlotPtr(
					dynamic_cast<ColoredPlot*>(&*itsPlots_[row][col]), false);
			//dynamic_cast<QPScatterPlot*>(&*itsPlots_[row][col]);
			if(!itsColoredPlots_[row][col].null()) {
				//if(itsColoredPlots_[row][col] != NULL) {
				const vector<String> &colors = PMS::COLORS_LIST();
				for(uInt i = 0; i < colors.size(); ++i) {
					itsColoredPlots_[row][col]->setColorForBin(
							i ,itsFactory_->color(colors[i]));
				}
			} else {
				std::cout << "Could not convert plot (" << row << ", " << col
						<< ") into a ColoredPlot" << std::endl;
				itsParent_->showError("Could not convert a plot in a ColoredPlot");
			}
		}
	}
}

Int PlotMSOverPlot::nIter() {
	Int iterationCount = 0;
	if ( itsCache_ != NULL ){
		iterationCount = itsCache_->nIter();
	}
	return iterationCount;
}



bool PlotMSOverPlot::firstIter() {
	Int nIter = itsCache_->nIter();
	if((nIter > 1) && (iter_ != 0)) {
		PlotMSPages &pages = itsParent_->getPlotManager().itsPages_;
		pages.firstPage();
		iter_ = 0;
		recalculateIteration();
		return true;
	}
	return false;
}

bool PlotMSOverPlot::prevIter() {
	Int nIter = itsCache_->nIter();
	if((nIter > 1) && ((iter_ - iterStep_) >= 0)) {
		PlotMSPages &pages = itsParent_->getPlotManager().itsPages_;
		pages.previousPage();
		iter_ -= iterStep_;
		recalculateIteration();
		return true;
	}
	return false;
}

bool PlotMSOverPlot::setIter( int index ){
	Int nIter = itsCache_->nIter();
	bool successful = false;
	if( nIter > 1 && index < nIter && index >= 0) {
		PlotMSPages &pages = itsParent_->getPlotManager().itsPages_;
		pages.setCurrentPageNum( index );
		iter_ = index;
		recalculateIteration();
		successful = true;
	}
	return successful;
}

bool PlotMSOverPlot::nextIter() {
	Int nIter = itsCache_->nIter();
	if((nIter > 1) && ((iter_ + iterStep_) < nIter)) {
		PlotMSPages &pages = itsParent_->getPlotManager().itsPages_;
		pages.nextPage();
		iter_ += iterStep_;
		recalculateIteration();
		return true;
	}
	return false;
}

bool PlotMSOverPlot::lastIter() {
	Int nIter = itsCache_->nIter();
	if((nIter > 0) && (iter_ < (nIter - iterStep_))) {
		PlotMSPages &pages = itsParent_->getPlotManager().itsPages_;
		pages.lastPage();
		iter_ = int(double(nIter-1) / iterStep_) * iterStep_;
		if(iterStep_ == 1) iter_ = nIter - 1;
		recalculateIteration();
		return true;
	}
	return false;
}

bool PlotMSOverPlot::resetIter() {
	Int nIter = itsCache_->nIter();
	if(nIter > 0 ) {
		PlotMSPages &pages = itsParent_->getPlotManager().itsPages_;
		pages.firstPage();
		iter_ = 0;
		recalculateIteration();
		return true;
	}
	return false;
}

void PlotMSOverPlot::recalculateIteration( ) {

	detachFromCanvases();
	if(itsTCLParams_.updateIteration ) {
		PlotMSPages &pages = itsParent_->getPlotManager().itsPages_;
		assignCanvases(pages);

	}

	uInt nIter = itsCache_->nIter();
	uInt rows = itsPlots_.size();
	for(uInt r = 0; r < rows; ++r) {
		uInt cols = itsPlots_[r].size();
		uInt iterationRows = iter_ + r * cols;
		if(iterationRows >= nIter) break;
		for(uInt c = 0; c < cols; ++c) {
			uInt iteration = iterationRows + c;
			if(iteration >= nIter) break;
			logIter(iteration, nIter);
			PlotMaskedPointDataPtr data(&(itsCache_->indexer(iteration)),
				false);
			itsPlots_[r][c] = itsFactory_->maskedPlot(data);
		}
	}

	setColors();
	itsTCLParams_.updateDisplay = true;

	attachToCanvases();
	updatePlots();
	updateCanvas();
	updateDisplay();
	releaseDrawing();
	logPoints();
}

void PlotMSOverPlot::updatePlots() {
	for(uInt row = 0; row < itsPlots_.size(); ++row) {
		for(uInt col = 0; col < itsPlots_[row].size(); ++col) {
			bool plottable = itsParent_->getPlotManager().isPlottable( this );
			if(!itsPlots_[row][col].null() && plottable ) {
				itsPlots_[row][col]->dataChanged();
			}
		}
	}
}

bool PlotMSOverPlot::updateIndexing() {
	PMS_PP_Iteration *iter = itsParams_.typedGroup<PMS_PP_Iteration>();
	itsCache_->setUpIndexer(iter->iterationAxis(),
			iter->isGlobalScaleX(),
			iter->isGlobalScaleY());
	return true;
}

void PlotMSOverPlot::logPoints() {
	PMS_PP_Display *display = itsParams_.typedGroup<PMS_PP_Display>();
	bool showUnflagged =
			display->unflaggedSymbol()->symbol() != PlotSymbol::NOSYMBOL;
	bool showFlagged =
			display->flaggedSymbol()->symbol() != PlotSymbol::NOSYMBOL;

	stringstream ss;
	ss << "Plotting ";
	if(showUnflagged) {
		if ( itsCache_->nIter() > iter_ ){
			ss << itsCache_->indexer(iter_).sizeUnmasked() << " unflagged"
					<< (showFlagged ? ", " : "");
		}
		else {
			ss << "0 unflagged" <<(showFlagged ? ", " : "");
		}
	}
	if(showFlagged) {
		if ( itsCache_->nIter() > iter_ ){
			ss << itsCache_->indexer(iter_).sizeMasked() << " flagged";
		}
		else {
			ss << "0 flagged";
		}
	}
	ss << " points.";

	itsParent_->getLogger()->postMessage(PMS::LOG_ORIGIN,
			PMS::LOG_ORIGIN_PLOT,
			ss.str(),
			PMS::LOG_EVENT_PLOT);
}

void PlotMSOverPlot::logIter(Int iter, Int nIter) {
	if(nIter > 1) {
		stringstream ss;
		ss << "Stepping to iteration = " << iter+1
				<< " (of " << nIter << "): "
				<< itsCache_->indexer(iter).iterLabel();
		itsParent_->getLogger()->postMessage(PMS::LOG_ORIGIN,
				PMS::LOG_ORIGIN_PLOT,
				ss.str(),
				PMS::LOG_EVENT_PLOT);
	}
}

void PlotMSOverPlot::cacheLoaded_(bool wasCanceled) {
	// Ensure we fail gracefully if cache loading yielded nothing
	// or was cancelled
	if ( itsCache_ == NULL ){
		return;
	}

	if (!itsCache_->cacheReady() || wasCanceled) {
		detachFromCanvases();
		initializePlot();
		releaseDrawing();
		itsCache_->clear();
		return;
	}

	// Make this more specific than canvas-triggered
	if (itsTCLParams_.updateCanvas){
		updateIndexing();
	}

	// Reset the iterator (if data are new)
	resetIter();

// Let the plot know that the data has been changed as needed, unless the
	// thread was canceled.
	updatePlots();

	// End cache log as needed.
	if(itsTCLParams_.endCacheLog)
		itsParent_->getLogger()->releaseMeasurement();
// Update canvas as needed.
	if(itsTCLParams_.updateCanvas){
		updateCanvas();
	}


	// Update display as needed.
	if(itsTCLParams_.updateDisplay){
		updateDisplay();
	}


	// Release drawing if needed.
	if(itsTCLParams_.releaseWhenDone)
		releaseDrawing();

	// Log number of points as needed.
	if(itsTCLParams_.endCacheLog)
		logPoints();
}

} //namespace casa
