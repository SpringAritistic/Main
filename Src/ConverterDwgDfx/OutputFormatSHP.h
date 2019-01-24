#ifndef OUTPUTFORMATSHP_H
#define OUTPUTFORMATSHP_H

#include "OutputFormatTypes.h"
#include "OutputFormat.h"
#include "ogrsf_frmts.h"
#include <stdio.h>

class OutputFormatSHP: public OutputFormat {
	public:
		void addPoint(OutputFeaturePoint newPoint);
		void addLine(OutputFeatureLine newLine);
		void addPolyLine(std::vector<OutputFeaturePoint *> newPoints);
		void addPolygon(std::vector<OutputFeaturePoint *> newPoints);

		void addText(OutputFeaturePoint location, const char * text);

		OutputFormatSHP (const char * filename, const char * textFilename, const char * layerName, OGRwkbGeometryType type);
		void finalizeOutput();
	private:
		OGRSFDriver * drvr;
		OGRLayer * lyr;
		OGRDataSource * ds;
		OGRLayer * textlyr;
		OGRDataSource * textds;
		OGRwkbGeometryType myType;

};

#endif
