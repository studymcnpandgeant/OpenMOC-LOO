/*
 * openmoc.cpp
 *
 *  Created on: Jan 21, 2012
 *      Author: Lulu Li
 *				MIT, Course 22
 *              lululi@mit.edu
 *
 *  This file defines the executable for OpenMOC.
 *
 */

#include "Geometry.h"
#include "TrackGenerator.h"
#include "Parser.h"
#include "log.h"
#include "Options.h"
#include "LocalCoords.h"
#include "Geometry.h"

// FIXME: These should be removed when main() is properly implemented
#pragma GCC diagnostic ignored "-Wunused"
#pragma GCC diagnostic ignored "-Wunused-variable"

// TODO: This is just stubbed out for now
int main(int argc, const char **argv) {
	log_printf(NORMAL, "Starting OpenMOC...");

	/* Create an options class to parse command line options */
	Options opts(argc, argv);

	/* Set the verbosity */
	log_setlevel(opts.getVerbosity());

	/* Create an empty geometry */
	Geometry geometry(opts.getNumSectors(), opts.getNumRings(),
						opts.getSectorOffset());

	// FIXME: PUT THIS INSIDE THE PARSER TO HIDE IT FROM THE MAIN PROGRAM
	/* We only want general functional calls inside the main program
	 * The logic going on here is too specific and should be hidden from
	 * main */
	/* Initialize the parser */
	Parser parser(&opts);
	parser.each_material([&geometry](Material *m) -> void
			  {
				  geometry.addMaterial(m);
				  return;
			  });
	parser.each_surface([&geometry](Surface *s) -> void
			    {
				    geometry.addSurface(s);
				    return;
			    });
	parser.each_cell([&geometry](Cell *c) -> void
			 {
				 geometry.addCell(c);
				 return;
			 });
	

	parser.each_lattice([&geometry](Lattice *l) -> void
			  {
				  geometry.addLattice(l);
				  return;
			  });

//	/* Print out geometry to console if requested at runtime*/
	if (opts.dumpGeometry())
		geometry.printString();

	/* Adjust the indices for each geometry class to use uids */
//	geometry.adjustKeys();

	/* Generate the neighbor cells for each surface in geometry */
//	geometry.buildNeighborsLists();

	/* Initialize the trackgenerator */
	TrackGenerator trackGenerator(&geometry, opts.getNumAzim(),
									opts.getTrackSpacing());
	trackGenerator.generateTracks();
	trackGenerator.makeReflective();

	log_printf(DEBUG, "NUM-AZIM = %d", trackGenerator.getNumAzim());
	/* Print out geometry to console if requested at runtime*/
//	if (opts.dumpGeometry())
//		geometry.printString();

	trackGenerator.segmentize();

	/* Testing findCell method */
//	LocalCoords* test_coords = new LocalCoords(0, 0);
//	test_coords->setUniverse(0);
//	Cell* result = geometry.findCell(test_coords);
//	log_printf(DEBUG, "Found cell for point %s: %s", test_coords->toString().c_str(), result->toString().c_str());
//	result = geometry.findNextCell(test_coords, M_PI/2);
//	log_printf(DEBUG, "Found next cell for point %s: %s", test_coords->toString().c_str(), result->toString().c_str());

	log_printf(INFO, "Program complete");
}
