/*
 * Cmfd.h
 *
 *  Created on: September 13, 2012
 *      Author: Sam Shaner
 *				MIT, Course 22
 *              shaner@mit.edu
 */

#ifndef CMFD_H_
#define CMFD_H_

#include <utility>
#include <math.h>
#include <unordered_map>
#include <limits.h>
#include <string>
#include <sstream>
#include <queue>
#include <iostream>
#include <fstream>
#include "pairwise_sum.h"
#include "Track.h"
#include "TrackGenerator.h"
#include "Geometry.h"
#include "Quadrature.h"
#include "FlatSourceRegion.h"
#include "configurations.h"
#include "log.h"
#include "quickplot.h"
#include "Mesh.h"
#include "MeshCell.h"
#include "MeshSurface.h"
#include "Material.h"
#include "Surface.h"
#include "petsc.h"
#include <petscmat.h>

#if USE_OPENMP
#include <omp.h>
#endif


/**
 * Solver types
 */
enum solveType {
    DIFFUSION,
    CMFD,
    LOO
};

#include "Plotter.h"

class Cmfd {
private:
    double _ignore;
    static int _surf_index[16];
    static double _conv_val[4];
    Geometry* _geom;
    Quadrature* _quad;
    Mesh* _mesh;
    Plotter* _plotter;
    FlatSourceRegion* _flat_source_regions;
    Track **_tracks;
    boundaryType _bc[4];
    Mat _A;
    Mat _M;
    Vec _phi_new;
    Vec _source_old;
    int _moc_iter;
    int _num_azim;
    int _num_iter_to_conv;
    int _num_loop;
    int _num_track;
    int *_num_tracks; 
    int _num_FSRs;
    int _cw;
    int _ch;
    int _ng;
    int _nq;
    int *_i_array, *_t_array, *_t_arrayb;
    double _damp_factor;
    double _keff;
    double _l2_norm;
    double _l2_norm_conv_thresh;
    double _spacing;
    double *_cell_source;
    double *_fsr_source;
    bool _linear_prolongation;
    bool _exact_prolongation;
    bool _acc_after_MOC_converge;
    bool _use_diffusion_correction;
    bool _run_loo;
    bool _run_loo_psi;
    bool _run_loo_phi;
    bool _run_cmfd;
    bool _plot_prolongation;
    bool _any_reflective;
    bool _update_boundary;
    bool _reflect_outgoing;
    bool _converged;
    bool _first_diffusion;
    int _num_first_diffusion;
    double _max_old;

public:
    Cmfd(Geometry* geom, Plotter* plotter, Mesh* mesh, 
         TrackGenerator *track_generator, Options *opts);
    virtual ~Cmfd();

    void runCmfd();
    void runLoo1();
    void runLoo2();

    void setMOCIter(int iter);
    void incrementMOCIter();
    int getNumIterToConv();
    double getL2Norm();
    double getKeff();

    /* Shared by two methods */
    double* computeCellSourceFromFSR(double moc_iter);    
    void computeXS();
    void computeXS_old();	
    void updateFSRScalarFlux(int moc_iter);
    void plotCmfdFluxUpdate(int moc_iter);
    double *getCellSource();
    void setCellSource(double *source);
    double computeCellSourceNorm();
    double computeCellSourceNormGivenTwoSources_old(double *olds, double *news, 
                                                    int num);
    double computeCellSourceNormGivenTwoSources(double *olds, double *news, 
                                                int num);
    void printCellSource(double moc_iter);
    double getExactProlongationRatio(int fsr_id, int quad_id);

    /* CMFD */
    void computeCurrent();
    void computeDs(int moc_iter);
    double computeDiffCorrect(double d, double h);
    int constructAMPhi(Mat A, Mat B, Vec phi_old, solveType solveMethod);
    double computeCMFDFluxPower(solveType solveMethod, int moc_iter);
    Mat getA();
    Mat getM();
    Vec getPhiNew();
    int createAMPhi(PetscInt size1, PetscInt size2, int cells);
    void setFSRs(FlatSourceRegion *fsrs);
    void setTracks(Track **tracks);
    int getNextCellId(int i, int s);
    int getNextCellSurfaceId(int s);
    int convertOutwardToCoordinate(int s);
    int computeCmfdL2Norm(Vec snew, int moc_iter);
    void updateBoundaryFluxByScalarFlux(int moc_iter);
    void updateBoundaryFlux(int moc_iter);

    /* LOO */
    void generateTrack(int *i_array, int *t_array, int *t_arrayb);
    void checkTrack();
    void storePreMOCMeshSource(FlatSourceRegion* fsrs);
    void computeQuadSrc();
    void computeQuadFlux();
    __inline__ double getSurf(int i, int t, int d){
        int id = -1;
        int j = -1;
        MeshCell *meshCell = _mesh->getCells(i);
        id = _surf_index[d*8+t];
        if ((t < 2) || ((t > 3) && (t < 6)))
            j = 0;
        else
            j = 1;

        MeshSurface *surface = meshCell->getMeshSurfaces(id); 

        // getTotalWt(3) and getTotalWt(4) are the weights used in
        // tallying current in HO. To get reflective BC cases to work, we
        // need these weights instead of the actual physical lengths. 
        return surface->getTotalWt(j);
    }

    double computeLooFluxPower(int moc_iter, double k);
    double computeNormalization(double flag);
    void normalizeFlux(double normalize_factor);
    void updateBoundaryFluxByPartialCurrent(int moc_iter);
    void updateBoundaryFluxBySrc(int moc_iter);
    void updateOldQuadFlux();
    void computeLooL2Norm(int moc_iter);
    bool onAnyBoundary(int i, int surf_id);
    bool cellOnAnyBoundary(int i);
    bool cellOnVacuumBoundary(int i);
    bool cellOnReflectiveBoundary(int i);
    bool surfaceOnVacuumBoundary(int i, int surf);
    bool surfaceOnReflectiveBoundary(int i, int surf);
    bool onBoundary(int track_id, int cell_id, int surf, int dir);
    bool onVacuumBoundary(int track_id, int cell_id, int dir);
    int onReflectiveBoundary(int track_id, int cell_id, int dir);
};

#endif /* CMFD_H_ */
