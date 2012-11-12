/*
 * Solver.h
 *
 *  Created on: Feb 7, 2012
 *      Author: William Boyd
 *				MIT, Course 22
 *              wboyd@mit.edu
 */

#ifndef SOLVER_H_
#define SOLVER_H_

#include <utility>
#include <math.h>
#include <unordered_map>
#include <limits.h>
#include <string>
#include <sstream>
#include <queue>
#include <armadillo>
#include "Geometry.h"
#include "Quadrature.h"
#include "Track.h"
#include "TrackGenerator.h"
#include "FlatSourceRegion.h"
#include "configurations.h"
#include "log.h"
#include "quickplot.h"
#include "Mesh.h"
#include "MeshCell.h"
#include "Material.h"
#include "Cmfd.h"

#if USE_OPENMP == true
	#include <omp.h>
#endif

class Solver {
private:
	Geometry* _geom;
	Quadrature* _quad;
	FlatSourceRegion* _flat_source_regions;
	Track** _tracks;
	int* _num_tracks;
	int _num_azim;
	int _num_FSRs;
	double *_FSRs_to_fluxes[NUM_ENERGY_GROUPS + 1];
	double *_FSRs_to_powers;
	double *_FSRs_to_pin_powers;
	double *_FSRs_to_fission_source;
	double *_FSRs_to_scatter_source;
	double *_FSRs_to_absorption[NUM_ENERGY_GROUPS + 1];
	double *_FSRs_to_pin_absorption[NUM_ENERGY_GROUPS + 1];
	double _k_eff;
	std::queue<double> _old_k_effs;
	Plotter* _plotter;
	float* _pix_map_total_flux;
	Cmfd* _cmfd;
#if !STORE_PREFACTORS
	double* _pre_factor_array;
	int _pre_factor_array_size;
	int _pre_factor_max_index;
	double _pre_factor_spacing;
	bool _update_flux;
#endif
	void precomputeFactors();
	double computePreFactor(segment* seg, int energy, int angle);
	void initializeFSRs();
public:
	Solver(Geometry* geom, TrackGenerator* track_generator, Plotter* plotter, Cmfd* cmfd, bool _update_flux);
	virtual ~Solver();
	void zeroTrackFluxes();
	void oneFSRFluxes();
	void zeroFSRFluxes();
	void zeroMeshCells();
	void computeRatios();
	void updateKeff();
	double** getFSRtoFluxMap();
	void fixedSourceIteration(int max_iterations);
	double computeKeff(int max_iterations);
	void plotFluxes(int iter_num);
	void checkTrackSpacing();
	void computePinPowers();
	void computeDs(Mesh* mesh);
	double computeCMFDFlux(Mesh* mesh, double keff);
	void updateMOCFlux(Mesh* mesh);
 	void checkNeutBal(Mesh* mesh, double keff);
 	void computeXS(Mesh* mesh);
 	void renormCurrents(Mesh* mesh, double keff);
 	double getEps(Mesh* mesh, double keff, double renorm_factor);
 	void initializeSource();
};

#endif /* SOLVER_H_ */
