#ifndef OUTPUTFORMATKML_H
#define OUTPUTFORMATKML_H

#include "OutputFormatTypes.h"
#include "OutputFormat.h"
#include <stdio.h>

class OutputFormatKML: public OutputFormat {
	public:
		void addPoint(OutputFeaturePoint newPoint);
		void addLine(OutputFeatureLine newLine);
		void addPolyLine(std::vector<OutputFeaturePoint *> newPoints);
		void addPolygon(std::vector<OutputFeaturePoint *> newPoints);

		void addText(OutputFeaturePoint location, const char * text);

		OutputFormatKML (const char * filename, const char * layerName);
		void finalizeOutput();
	private:
		FILE * outputFile;

		const char * xmlEscape(const char * srcCharArray);
};

#endif
