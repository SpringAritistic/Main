#include "stdafx.h"
#include "OutputFormatTypes.h"
#include "OutputFormatSHP.h"
#include "ogrsf_frmts.h"

void OutputFormatSHP::addPoint(OutputFeaturePoint newPoint) {
	if (!this->lyr) {
		fprintf(stderr, "No output layer is available.");
		return;
	}
	
	if (this->myType != wkbPoint) {
		fprintf(stderr, "Ignoring due to incompatible type: POINT at (%f,%f)\n", newPoint.get_X(), newPoint.get_Y());
		return;
	}

	OGRFeature * newFeat = OGRFeature::CreateFeature(this->lyr->GetLayerDefn());
	newFeat->SetField("id", this->lyr->GetFeatureCount(1));
	OGRPoint point;
	point.setX(newPoint.get_X());
	point.setY(newPoint.get_Y());
	point.setZ(newPoint.get_Z());
	newFeat->SetGeometry(&point);
	this->lyr->CreateFeature(newFeat);
	
	// Clean up our memory
	OGRFeature::DestroyFeature(newFeat);
}

void OutputFormatSHP::addLine(OutputFeatureLine newLine) {
	if (!this->lyr) {
		fprintf(stderr, "No output layer is available.");
		return;
	}

	if (this->myType == wkbPoint) {
		// Add every point in the line instead
		OutputFeaturePoint p1(newLine.get_X1(), newLine.get_Y1(), newLine.get_Z1());
		OutputFeaturePoint p2(newLine.get_X2(), newLine.get_Y2(), newLine.get_Z2());
		this->addPoint(p1);
		this->addPoint(p2);
		return;
	}
	if (this->myType == wkbPolygon) {
		// Add as a closed polygon instead
		OutputFeaturePoint * p1 = new OutputFeaturePoint(newLine.get_X1(), newLine.get_Y1(), newLine.get_Z1());
		OutputFeaturePoint * p2 = new OutputFeaturePoint(newLine.get_X2(), newLine.get_Y2(), newLine.get_Z2());
		std::vector<OutputFeaturePoint *> line;
		line.push_back(p1);
		line.push_back(p2);
		this->addPolygon(line);
		return;
	}
	
	OGRFeature * newFeat = OGRFeature::CreateFeature(this->lyr->GetLayerDefn());
	newFeat->SetField("id", this->lyr->GetFeatureCount(1));
	
	OGRPoint point;
	point.setX(newLine.get_X1());
	point.setY(newLine.get_Y1());
	point.setZ(newLine.get_Z1());
	OGRPoint point2;
	point2.setX(newLine.get_X2());
        point2.setY(newLine.get_Y2());
        point2.setZ(newLine.get_Z2());
	OGRLineString line;
	line.addPoint(&point);
	line.addPoint(&point2);	
	newFeat->SetGeometry(&line);
	this->lyr->CreateFeature(newFeat);

	// Clean up our memory
	OGRFeature::DestroyFeature(newFeat);
}

void OutputFormatSHP::addPolyLine(std::vector<OutputFeaturePoint *> newPoints) {
	if (!this->lyr) {
		fprintf(stderr, "No output layer is available.");
		return;
	}

	if (this->myType == wkbPoint) {
		// Add every point in the line instead
		for (int i = 0; i < newPoints.size(); i++) {
			OutputFeaturePoint point(newPoints[i]->get_X(), newPoints[i]->get_Y(), newPoints[i]->get_Z());
			this->addPoint(point);
		}
		return;
	}
	if (this->myType == wkbPolygon) {
		// Add as a closed polygon instead
		this->addPolygon(newPoints);
		return;
	}
	
	OGRFeature * newFeat = OGRFeature::CreateFeature(this->lyr->GetLayerDefn());
	newFeat->SetField("id", this->lyr->GetFeatureCount(1));
	OGRLineString newLine;

	for (int i = 0; i < newPoints.size(); i++) {
		OGRPoint point;
		point.setX(newPoints[i]->get_X());
		point.setY(newPoints[i]->get_Y());
		point.setZ(newPoints[i]->get_Z());
		newLine.addPoint(&point);
	}
	newFeat->SetGeometry(&newLine);
	this->lyr->CreateFeature(newFeat);

	// Clean up our memory
	OGRFeature::DestroyFeature(newFeat);
}

void OutputFormatSHP::addPolygon(std::vector<OutputFeaturePoint *> newPoints) {
	if (!this->lyr) {
		fprintf(stderr, "No output layer is available.");
		return;
	}

	if (this->myType == wkbPoint) {
		// Add every point instead
		for (int i = 0; i < newPoints.size(); i++) {
			OutputFeaturePoint point(newPoints[i]->get_X(), newPoints[i]->get_Y(), newPoints[i]->get_Z());
			this->addPoint(point);
		}
		return;
	}
	if (this->myType == wkbLineString) {
		// Add as a polyline instead
		this->addPolyLine(newPoints);
		return;
	}

	OGRFeature * newFeat = OGRFeature::CreateFeature(this->lyr->GetLayerDefn());
	newFeat->SetField("id", this->lyr->GetFeatureCount(1));
	OGRPolygon newPoly;
	OGRLinearRing ring1;	
	for (int i = 0; i < newPoints.size(); i++) {
		OGRPoint point;
		point.setX(newPoints[i]->get_X());
		point.setY(newPoints[i]->get_Y());
		point.setZ(newPoints[i]->get_Z());
		ring1.addPoint(&point);
	}
	newPoly.addRing(&ring1);
	newPoly.closeRings(); // Force polygon to close, if last point != first point
	newFeat->SetGeometry(&newPoly);
	this->lyr->CreateFeature(newFeat);

	// Clean up our memory
	OGRFeature::DestroyFeature(newFeat);
}

void OutputFormatSHP::addText(OutputFeaturePoint location, const char * text) {
	if (!this->textlyr) {
		// No text attributes layer - not really an error, user might just not care about text attributes
		return;
	}

	OGRFeature * newFeat = OGRFeature::CreateFeature(this->textlyr->GetLayerDefn());
	newFeat->SetField("id", this->textlyr->GetFeatureCount(1));
	newFeat->SetField("text", text);
	OGRPoint point;
	point.setX(location.get_X());
	point.setY(location.get_Y());
	point.setZ(location.get_Z());
	newFeat->SetGeometry(&point);
	this->textlyr->CreateFeature(newFeat);

	// Clean up our memory
	OGRFeature::DestroyFeature(newFeat);
}

OutputFormatSHP::OutputFormatSHP(const char * filename, const char * textFilename, const char * layerName, OGRwkbGeometryType type) {
	OGRRegisterAll();
	this->drvr = dynamic_cast<OGRSFDriver *>(  OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile"));
	if (this->drvr == NULL) {
		fprintf(stderr, "Could not open Shapefile OGR driver.\n");
		return;
	}	

	this->ds = this->drvr->CreateDataSource(filename, NULL);
	if (this->ds == NULL) {
		fprintf(stderr, "Could not create output file.\n");
		return;
	}

	this->lyr = ds->CreateLayer(layerName, NULL, type, NULL);
	if (this->lyr == NULL) {
		fprintf(stderr, "Could not create layer.\n");
		return;
	}

	// Add an ID field
	OGRFieldDefn newField("id", OFTInteger);
	newField.SetWidth(32);
	this->lyr->CreateField(&newField);
	this->myType = type;

	// Text shapefile, if there are any text attributes:
	if (textFilename != NULL) {
		this->textds = this->drvr->CreateDataSource(textFilename, NULL);
		if (this->textds == NULL) {
			fprintf(stderr, "Could not create output file.\n");
			return;
		}

		this->textlyr = this->textds->CreateLayer(layerName, NULL, wkbPoint, NULL);
		if (this->textlyr == NULL) {
			fprintf(stderr, "Could not create layer.\n");
			return;
		}

		// Add an ID field
		OGRFieldDefn newField("id", OFTInteger);
		this->textlyr->CreateField(&newField);
		this->myType = type;
		// ...and a field to store the text
		OGRFieldDefn textField("text", OFTString);
		textField.SetWidth(32);
		this->textlyr->CreateField(&textField);
	}
}

void OutputFormatSHP::finalizeOutput() {
	if (this->ds) {
		// Will trigger saving the file, and also will
		// clean up any layer references from Create/Get Layer calls
		OGRDataSource::DestroyDataSource(this->ds);
	}

	if (this->textds) {
		OGRDataSource::DestroyDataSource(this->textds);
	}
}
