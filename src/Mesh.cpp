/*
 * Mesh.cpp
 *
 *  Created on: May 6, 2012
 *      Author: samuelshaner
 */

#include "Mesh.h"

Mesh::Mesh(){

}

Mesh::~Mesh(){
	delete [] _cells;
}

void Mesh::makeMeshCells(){

	/* make mesh cells */
	_cells = new MeshCell[_cell_width * _cell_height];

	/* set mesh surface cell id's */
	for (int i = 0; i < _cell_width * _cell_height; i++){
		for (int s = 0; s < 8; s++){
			_cells[i].getMeshSurfaces(s)->setCellId(i);
		}
	}

}

int Mesh::getCellWidth(){
	return _cell_width;
}

int Mesh::getCellHeight(){
	return _cell_height;
}

void Mesh::setCellWidth(int cellWidth){
	_cell_width = cellWidth;
}

void Mesh::setCellHeight(int cellHeight){
	_cell_height = cellHeight;
}

MeshCell* Mesh::getCells(int cell){
	return &_cells[cell];
}


void Mesh::setMultigroup(bool multigroup){
	_multigroup = multigroup;
}

bool Mesh::getMultigroup(){
	return _multigroup;
}

void Mesh::setPrintMatrices(bool printMatrices){
	_print_matrices = printMatrices;
}

bool Mesh::getPrintMatrices(){
	return _print_matrices;
}

/* given an x,y coordinate, find what mesh cell the point is in */
int Mesh::findMeshCell(double pointX, double pointY){

	double left;
	double top = _height / 2.0;
	bool flag = false;
	int cell = 0;

	for (int y = 0; y < _cell_height; y++){
		left = - _width / 2.0;
		cell = y * _cell_width;
		if (pointY <= top && pointY >= top - getCells(cell)->getHeight()){
			for (int x = 0; x < _cell_width; x++){
				cell = y * _cell_width + x;
				if (pointX >= left && pointX <= left + getCells(cell)->getWidth()){
					flag = true;
					break;
				}
				left = left + getCells(cell)->getWidth();
			}
			if (flag == true){
				break;
			}
		}
		top = top - getCells(cell)->getHeight();
	}

	return cell;
}

void Mesh::setWidth(double width){
	_width = width;
}

void Mesh::setHeight(double height){
	_height = height;
}

double Mesh::getWidth(){
	return _width;
}

double Mesh::getHeight(){
	return _height;
}

void Mesh::setCellBounds(){

	double x = -_width / 2.0;
	double y = _height / 2.0;

	/* loop over MeshCells and set bounds */
	for (int i = 0; i < _cell_height; i++){
		x = -_width / 2.0;
		y = y - _cells[i * _cell_width].getHeight();
		for (int j = 0; j < _cell_width; j++){
			_cells[i * _cell_width + j].setBounds(x,y);
			x = x + _cells[i * _cell_width + j].getWidth();
		}
	}

}

void Mesh::setFSRBounds(boundaryType left, boundaryType right, boundaryType bottom, boundaryType top){

	_fsr_indices = new int        [2 * _cell_width * _cell_height];
	_cell_bounds = new double     [4 * _cell_width * _cell_height];
	_surfaces =  new MeshSurface *[8 * _cell_width * _cell_height];

	int min;
	int max;
	int fsr;

	for (int i = 0; i < _cell_height * _cell_width; i++){
		min = _cells[i].getFSRs()->front();
		max = _cells[i].getFSRs()->front();

		std::vector<int>::iterator iter;
		for (iter = _cells[i].getFSRs()->begin(); iter != _cells[i].getFSRs()->end(); ++iter) {
			fsr = *iter;
			min = std::min(fsr, min);
			max = std::max(fsr, max);
		}

		_cells[i].setFSRStart(min);
		_cells[i].setFSREnd(max);
		_fsr_indices[2*i] = min;
		_fsr_indices[2*i+1] = max;

		_cell_bounds[i*4  ] = _cells[i].getBounds()[0];
		_cell_bounds[i*4+1] = _cells[i].getBounds()[1];
		_cell_bounds[i*4+2] = _cells[i].getBounds()[2];
		_cell_bounds[i*4+3] = _cells[i].getBounds()[3];

		_surfaces[i*8  ] = _cells[i].getMeshSurfaces(0);
		_surfaces[i*8+1] = _cells[i].getMeshSurfaces(1);
		_surfaces[i*8+2] = _cells[i].getMeshSurfaces(2);
		_surfaces[i*8+3] = _cells[i].getMeshSurfaces(3);
		_surfaces[i*8+4] = _cells[i].getMeshSurfaces(4);
		_surfaces[i*8+5] = _cells[i].getMeshSurfaces(5);
		_surfaces[i*8+6] = _cells[i].getMeshSurfaces(6);
		_surfaces[i*8+7] = _cells[i].getMeshSurfaces(7);

	}

	for (int x = 0; x < _cell_width; x++){
		for (int y = 0; y < _cell_height; y++){

			/* left */
			if (x == 0){
				if (left == VACUUM){
					_cells[y*_cell_width+x].getMeshSurfaces(0)->setBoundary(VACUUM);
				}
				else{
					_cells[y*_cell_width+x].getMeshSurfaces(0)->setBoundary(REFLECTIVE);
				}
			}

			/* right */
			if (x == _cell_width-1){
				if (right == VACUUM){
					_cells[y*_cell_width+x].getMeshSurfaces(2)->setBoundary(VACUUM);
				}
				else{
					_cells[y*_cell_width+x].getMeshSurfaces(2)->setBoundary(REFLECTIVE);
				}
			}

			/* bottom */
			if (y == _cell_height - 1){
				if (bottom == VACUUM){
					_cells[y*_cell_width+x].getMeshSurfaces(1)->setBoundary(VACUUM);
				}
				else{
					_cells[y*_cell_width+x].getMeshSurfaces(1)->setBoundary(REFLECTIVE);
				}
			}

			/* top */
			if (y == 0){
				if (top == VACUUM){
					_cells[y*_cell_width+x].getMeshSurfaces(3)->setBoundary(VACUUM);
				}
				else{
					_cells[y*_cell_width+x].getMeshSurfaces(3)->setBoundary(REFLECTIVE);
				}
			}

		}
	}


}


/* Using an fsr_id and coordinate, find which surface a coordinate is on */
int Mesh::findMeshSurface(int fsr_id, LocalCoords* coord){

	int surface = -1;
	double x = coord->getX();
	double y = coord->getY();

	/* find which MeshCell fsr_id is in -> get meshSuface that coord is on*/
	for (int i = 0; i < _cell_width * _cell_height; i++){
		if (fsr_id >= _fsr_indices[2*i] && fsr_id <= _fsr_indices[2*i+1]){

			/* find which surface coord is on */
			/* left */
			if (fabs(x - _cell_bounds[i*4+0]) < 1e-8){
				if (fabs(y - _cell_bounds[i*4+1]) > 1e-8 && fabs(y - _cell_bounds[i*4+3]) > 1e-8){
					surface = i*8+0;
				}
				else if (fabs(y - _cell_bounds[i*4+3]) < 1e-8){
					surface = i*8+7;
				}
				else{
					surface = i*8+4;
				}
			}
			/* right */
			else if (fabs(x - _cell_bounds[i*4+2]) < 1e-8){
				if (fabs(y - _cell_bounds[i*4+1]) > 1e-8 && fabs(y - _cell_bounds[i*4+3]) > 1e-8){
					surface = i*8+2;
				}
				else if (fabs(y - _cell_bounds[i*4+3]) < 1e-8){
					surface = i*8+6;
				}
				else{
					surface = i*8+5;
				}
			}
			/* top */
			else if (fabs(y - _cell_bounds[i*4+3]) < 1e-8){
				surface = i*8+3;
			}
			/* bottom */
			else if (fabs(y - _cell_bounds[i*4+1]) < 1e-8){
				surface = i*8+1;
			}

			break;
		}
	}

	return surface;
}

void Mesh::printBounds(){

	double* bounds;

	for (int i = 0; i < _cell_width * _cell_height; i++){
		bounds = _cells[i].getBounds();
		log_printf(NORMAL, "cell: %i bounds [%f, %f, %f, %f]", i, bounds[0], bounds[1], bounds[2], bounds[3]);
	}

}


void Mesh::printCurrents(){

	double current;

	for (int i = 0; i < _cell_width * _cell_height; i++){
		for (int surface = 0; surface < 8; surface++){
			for (int group = 0; group < NUM_ENERGY_GROUPS; group++){
				current = _cells[i].getMeshSurfaces(surface)->getCurrent(group);
				log_printf(NORMAL, "cell: %i, surface: %i, group: %i, current: %f", i, surface, group, current);
			}
		}
	}
}


void Mesh::setBoundary(boundaryType boundary, int s){
	_boundary[s] = boundary;
}


boundaryType Mesh::getBoundary(int s){
	return _boundary[s];
}


void Mesh::splitCorners(){

	MeshSurface* surfaceSide;
	MeshSurface* surfaceCorner1;

	int cell_width = getCellWidth();
	int cell_height = getCellHeight();
	MeshCell* meshCell;
	MeshCell* meshCellNext;
	MeshSurface* surfaceSideNext;

	for (int x = 0; x < cell_width; x++){
		for (int y = 0; y < cell_height; y++){

			/* corner 4 */
			if (x > 0 && y < cell_height - 1){
				meshCell = &_cells[y*cell_width + x];
				surfaceSide = meshCell->getMeshSurfaces(1);
				surfaceCorner1 = meshCell->getMeshSurfaces(4);
				meshCellNext = &_cells[(y+1)*cell_width + x];
				surfaceSideNext = meshCellNext->getMeshSurfaces(0);

				for (int group = 0; group < NUM_ENERGY_GROUPS; group++){
					surfaceSide->incrementCurrent(surfaceCorner1->getCurrent(group), group);
					surfaceSideNext->incrementCurrent(surfaceCorner1->getCurrent(group), group);
				}
			}
			else{
				meshCell = &_cells[y*cell_width + x];
				surfaceSide = meshCell->getMeshSurfaces(1);
				surfaceCorner1 = meshCell->getMeshSurfaces(4);
				surfaceSideNext = meshCell->getMeshSurfaces(0);

				for (int group = 0; group < NUM_ENERGY_GROUPS; group++){
					surfaceSide->incrementCurrent(surfaceCorner1->getCurrent(group), group);
					surfaceSideNext->incrementCurrent(surfaceCorner1->getCurrent(group), group);
				}
			}


			/* corner 5 */
			if (x < cell_width - 1 && y < cell_height - 1){
				meshCell = &_cells[y*cell_width + x];
				surfaceSide = meshCell->getMeshSurfaces(1);
				surfaceCorner1 = meshCell->getMeshSurfaces(5);
				meshCellNext = &_cells[(y+1)*cell_width + x];
				surfaceSideNext = meshCellNext->getMeshSurfaces(2);

				for (int group = 0; group < NUM_ENERGY_GROUPS; group++){
					surfaceSide->incrementCurrent(surfaceCorner1->getCurrent(group), group);
					surfaceSideNext->incrementCurrent(surfaceCorner1->getCurrent(group), group);
				}
			}
			else{
				meshCell = &_cells[y*cell_width + x];
				surfaceSide = meshCell->getMeshSurfaces(1);
				surfaceCorner1 = meshCell->getMeshSurfaces(5);
				surfaceSideNext = meshCell->getMeshSurfaces(2);

				for (int group = 0; group < NUM_ENERGY_GROUPS; group++){
					surfaceSide->incrementCurrent(surfaceCorner1->getCurrent(group), group);
					surfaceSideNext->incrementCurrent(surfaceCorner1->getCurrent(group), group);
				}
			}


			/* corner 6 */
			if (x < cell_width - 1 && y > 0){
				meshCell = &_cells[y*cell_width + x];
				surfaceSide = meshCell->getMeshSurfaces(2);
				surfaceCorner1 = meshCell->getMeshSurfaces(6);
				meshCellNext = &_cells[y*cell_width + x + 1];
				surfaceSideNext = meshCellNext->getMeshSurfaces(3);

					for (int group = 0; group < NUM_ENERGY_GROUPS; group++){
						surfaceSide->incrementCurrent(surfaceCorner1->getCurrent(group), group);
						surfaceSideNext->incrementCurrent(surfaceCorner1->getCurrent(group), group);
					}
			}
			else{
				meshCell = &_cells[y*cell_width + x];
				surfaceSide = meshCell->getMeshSurfaces(2);
				surfaceCorner1 = meshCell->getMeshSurfaces(6);
				surfaceSideNext = meshCell->getMeshSurfaces(3);

					for (int group = 0; group < NUM_ENERGY_GROUPS; group++){
						surfaceSide->incrementCurrent(surfaceCorner1->getCurrent(group), group);
						surfaceSideNext->incrementCurrent(surfaceCorner1->getCurrent(group), group);
					}
			}


			/* corner 7 */
			if (x > 0 && y > 0){
				meshCell = &_cells[y*cell_width + x];
				surfaceSide = meshCell->getMeshSurfaces(0);
				surfaceCorner1 = meshCell->getMeshSurfaces(7);
				meshCellNext = &_cells[y*cell_width + x - 1];
				surfaceSideNext = meshCellNext->getMeshSurfaces(3);

				for (int group = 0; group < NUM_ENERGY_GROUPS; group++){
					surfaceSide->incrementCurrent(surfaceCorner1->getCurrent(group), group);
					surfaceSideNext->incrementCurrent(surfaceCorner1->getCurrent(group), group);
				}
			}
			else{
				meshCell = &_cells[y*cell_width + x];
				surfaceSide = meshCell->getMeshSurfaces(0);
				surfaceCorner1 = meshCell->getMeshSurfaces(7);
				surfaceSideNext = meshCell->getMeshSurfaces(3);

				for (int group = 0; group < NUM_ENERGY_GROUPS; group++){
					surfaceSide->incrementCurrent(surfaceCorner1->getCurrent(group), group);
					surfaceSideNext->incrementCurrent(surfaceCorner1->getCurrent(group), group);
				}
			}
		}
	}
}

void Mesh::computeTotCurrents(){

	MeshCell* meshCell;
	MeshSurface* surfaceSide;
	double sum_cur;

	int cell_width = getCellWidth();
	int cell_height = getCellHeight();

	/* loop over cells */
	for (int i = 0; i < cell_width*cell_height; i++){

		/* get mesh cell */
		meshCell = &_cells[i];

		/* loop over surfaces*/
		for (int i = 0; i < 4; i++){

			/* get mesh surface */
			surfaceSide = meshCell->getMeshSurfaces(i);

			/* set current tally to 0 */
			sum_cur = 0.0;

			/* loop over energy groups */
			for (int e = 0; e < NUM_ENERGY_GROUPS; e++){
				sum_cur += surfaceSide->getCurrent(e);
			}

			/* set current at group 0 to total current */
			surfaceSide->setCurrent(sum_cur,0);

		}
	}
}

void Mesh::setKeffCMFD(double keff, int iter){
	_keff_cmfd[iter] = keff;
}

double Mesh::getKeffCMFD(int iter){
	return _keff_cmfd[iter];
}

void Mesh::setKeffMOC(double keff, int iter){
	_keff_moc[iter] = keff;
}

double Mesh::getKeffMOC(int iter){
	return _keff_moc[iter];
}

MeshSurface **Mesh::getSurfaces(){
	return _surfaces;
}

double Mesh::getOldTime(){
	return _old_time;
}

void Mesh::setOldTime(double time){
	_old_time = time;
}

