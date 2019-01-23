#include "stdafx.h"
#include "OutputFormatTypes.h"
#include "OutputFormatKML.h"
#include <string>
#include <sstream>

// Preserve a few bytes by not adding newlines and tab indentation

void OutputFormatKML::addPoint(OutputFeaturePoint newPoint) {
	if (!this->outputFile) {
		fprintf(stderr, "No output file is open for writing.");
		return;
	}
	
	fprintf(this->outputFile, "<Placemark><Point><altitudeMode>relativeToGround</altitudeMode><coordinates>%f,%f,%f</coordinates></Point></Placemark>", newPoint.get_X(), newPoint.get_Y(), newPoint.get_Z());
}

void OutputFormatKML::addLine(OutputFeatureLine newLine) {
	if (!this->outputFile) {
		fprintf(stderr, "No output file is open for writing.");
		return;
	}

	fprintf(this->outputFile, "<Placemark><LineString><tessellate>0</tessellate><coordinates>%f,%f,%f %f,%f,%f</coordinates></LineString></Placemark>", newLine.get_X1(), newLine.get_Y1(), newLine.get_Z1(), newLine.get_X2(), newLine.get_Y2(), newLine.get_Z2());
}

void OutputFormatKML::addPolyLine(std::vector<OutputFeaturePoint *> newPoints) {
	if (!this->outputFile) {
		fprintf(stderr, "No output file is open for writing.");
		return;
	}

	fprintf(this->outputFile, "<Placemark><LineString><tessellate>0</tessellate><coordinates>");
	for (int i = 0; i < newPoints.size(); i++) {
		fprintf(this->outputFile, "(%f,%f,%f) ", newPoints[i]->get_X(), newPoints[i]->get_Y(), newPoints[i]->get_Z());
	}
	fprintf(this->outputFile, "</coordinates></LineString></Placemark>");
}

void OutputFormatKML::addPolygon(std::vector<OutputFeaturePoint *> newPoints) {
	if (!this->outputFile) {
		fprintf(stderr, "No output file is open for writing.");
		return;
	}
	
	fprintf(this->outputFile, "<Placemark><Polygon><extrude>1</extrude><altitudeMode>relativeToGround</altitudeMode><outerBoundaryIs><LinearRing><coordinates>");
	
	for (int i = 0; i < newPoints.size(); i++) {
		fprintf(this->outputFile, "%f,%f,%f ", newPoints[i]->get_X(), newPoints[i]->get_Y(), newPoints[i]->get_Z());
	}

	fprintf(this->outputFile, "</coordinates></LinearRing></outerBoundaryIs></Polygon></Placemark>");
}

void OutputFormatKML::addText(OutputFeaturePoint location, const char * text) {
	if (!this->outputFile) {
		fprintf(stderr, "No output file is open for writing.");
		return;
	}

	const char * safeText = this->xmlEscape(text);
	fprintf(this->outputFile, "<Placemark><name>%s</name><description>Text Attribute</description><Point><coordinates>%f,%f,%f</coordinates></Point></Placemark>", safeText, location.get_X(), location.get_Y(), location.get_Z());
	// No need to delete safeText - will be handled by std::string
}

OutputFormatKML::OutputFormatKML(const char * filename, const char * layerName) {
	/*this->outputFile =*/ fopen_s(&this->outputFile, filename, "w");
	const char * safeLayerName = this->xmlEscape(layerName);
	fprintf(this->outputFile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://www.opengis.net/kml/2.2\"><Document><name>%s</name><open>1</open><description>Converted from AutoCAD DXF or DWG</description><Folder><name>%s</name>", safeLayerName, safeLayerName);
	// No need to delete safeLayerName - will be handled by std::string
}

void OutputFormatKML::finalizeOutput() {
	if (this->outputFile) {
		fprintf(this->outputFile, "</Folder></Document></kml>");
		fclose(this->outputFile);
	}
}

const char * OutputFormatKML::xmlEscape(const char * srcCharArray) {
	// Adapted from http://www.mdawson.net/misc/xmlescape.php
	// Quick and easy XML escaping

	std::string srcString(srcCharArray);
	std::ostringstream outStr;

	for (std::string::const_iterator iter = srcString.begin(); iter != srcString.end(); iter++) {
		switch((unsigned char) *iter) {
			case '"':
				outStr << "&quot;";
				break;
			case '&':
				outStr << "&amp;";
				break;
			case '\'':
				outStr << "&apos;";
				break;
			case '<':
				outStr << "&lt;";
				break;
			case '>': 
				outStr << "&gt;";
				break;
			default:
				if ((unsigned char) *iter < 32 || (unsigned char) *iter > 127) {
					outStr << "&#" << (unsigned int) *iter << ";";
				}
				else {
					outStr << (unsigned char) *iter;
				}
				break;
		}
	}

	return outStr.str().c_str();
}
