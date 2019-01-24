#include "InputFormatDWG.h"

InputFormatDWG::InputFormatDWG(const char * inputFilename) {
	this->dwg = new Dwg_Data();
	int errno = dwg_read_file((char *)inputFilename, this->dwg);
	if (errno) {
		fprintf(stderr, "Could not open DWG. Returned error code: $d\n", errno);
		delete this->dwg;
	}

	this->currentBlockX = 0;
	this->currentBlockY = 0;

}

void InputFormatDWG::readFeaturesInto(OutputFormat * outputHandler) {
	if (!this->dwg) return;
	
	// Pull in actual data now...
	this->output = outputHandler;
	
	Dwg_Object_BLOCK_CONTROL * ctrl = this->dwg->object[0].tio.object->tio.BLOCK_CONTROL;
	for (int i = 0; i < ctrl->num_entries; i++) {
		this->dumpBlock(ctrl->block_headers[i]);
	}	
	
	this->dumpBlock(ctrl->model_space);
	this->dumpBlock(ctrl->paper_space);	
	
	// Don't delete output format - caller can do that
	this->output = NULL;
	dwg_free(this->dwg);
}

void InputFormatDWG::dumpBlock(Dwg_Object_Ref * block) {
	if (!block) return;
	if (!block->obj) return;
	if (!block->obj->tio.object) return;

	Dwg_Object_BLOCK_HEADER * header = block->obj->tio.object->tio.BLOCK_HEADER;
	Dwg_Object * obj = get_first_owned_object(block->obj, header);
	while (obj) {
		if (obj->type == DWG_TYPE_INSERT) {
			Dwg_Entity_INSERT * insrt = obj->tio.entity->tio.INSERT;
			this->currentBlockX = insrt->ins_pt.x;
			this->currentBlockY = insrt->ins_pt.y;
			// No need to delete
		}
		if (obj->type == DWG_TYPE_LINE) {
			Dwg_Entity_LINE * line = obj->tio.entity->tio.LINE;
			OutputFeatureLine * newLine = new OutputFeatureLine(line->start.x + this->currentBlockX, line->start.y + this->currentBlockY, 0, line->end.x + this->currentBlockX, line->end.y + this->currentBlockY, 0);
			this->output->addLine(*newLine);
			delete newLine;
		}
		if (obj->type == DWG_TYPE_CIRCLE) {
			// Approximate the circle with 360 line segments connecting points along that circle
			register double pi = 3.1415926535897932384626433832795;
			register double increment = ((2 * pi ) / 360);
			register double i = 0;

			Dwg_Entity_CIRCLE * circle = obj->tio.entity->tio.CIRCLE;

			std::vector<OutputFeaturePoint *> points;
			for (; i <= 2 * pi; i += increment) {
				points.push_back(new OutputFeaturePoint(circle->radius * cos(i) + circle->center.x + this->currentBlockX, circle->radius * sin(i) + circle->center.y + this->currentBlockY, 0));
			}

			this->output->addPolyLine(points);

			for (int q = 0; q < 360; q++) delete points[q];
		}
		if (obj->type == DWG_TYPE_TEXT) {
			Dwg_Entity_TEXT * text = obj->tio.entity->tio.TEXT;

			OutputFeaturePoint * newPoint = new OutputFeaturePoint(text->insertion_pt.x + this->currentBlockX, text->insertion_pt.y + this->currentBlockY, 0);
			this->output->addText(*newPoint, (const char *)text->text_value);
			delete newPoint;
		}
		if (obj->type == DWG_TYPE_ARC) {
			Dwg_Entity_ARC * arc = obj->tio.entity->tio.ARC;

			double pi = 3.1415926535897932384626433832795;
			int fromAngle = (int) arc->start_angle + 1;
			int toAngle = (int) arc->end_angle + 1;
	
			// Approximate the arc with 500 points
			const int approximationCount = 500;
			std::vector<OutputFeaturePoint *> points;
			register int i = 0;
			register double radianMeasure;
			register double deg2radFactor = (pi / 180);
			for (i = fromAngle; ; i++) {
				if (i > 360) i = 0;
				if (points.size() > approximationCount) break;

				radianMeasure = i * deg2radFactor;
				points.push_back(new OutputFeaturePoint(arc->radius * cos(radianMeasure) + arc->center.x + this->currentBlockX, arc->radius * sin(radianMeasure) + arc->center.y + this->currentBlockY, 0));
			}

			this->output->addPolyLine(points);

			for (i = 0; i < approximationCount; i++) delete points[i];
		}

		obj = get_next_owned_object(block->obj, obj, header);
	}
}
