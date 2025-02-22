//
// Created by phil on 11/10/21.
//

#ifndef GEANT4_SIM_GDML_GEANT4_SIM_GDML_H
#define GEANT4_SIM_GDML_GEANT4_SIM_GDML_H


#define PARTICLE_NUMBER_OPT 999
#define PARTICLE_NAME_OPT 1000
#define OUTPUT_NAME_OPT 1001
#define THREAD_NUM_OPT 1002
#define STEP_SIZE_OPT 1003
#define ENERGY_LOW_LIMIT_OPT 1004
#define ENERGY_HIGH_LIMIT_OPT 1005
#define ENERGY_CENTER_OPT 1006
#define GAUSSIAN_ENERGY_OPT 1007
#define POWERLAW_SPECTRUM_OPT 1008
#define POWERLAW_ALPHA_OPT 1009
#define MONOENERGETIC_OPT 1010
#define ARBITRARY_SPECTRUM_OPT 1011
#define GAUSSIAN_SIGMA_OPT 1012
#define FTFP_PHYSICS_OPT 1013
#define RANDOM_FILE_ID_OPT 1014
#define INPUT_FILE_NAME_OPT 1015
#define MACRO_FILE_NAME_OPT 1016
#define PRINT_STATISTICS_ON_DETECTORS_OPT 1017
#define QGSP_PHYSICS_OPT 1018
#define QGSP_PHYSICS_HIGH_PRECISION_OPT 1019
#define FTFP_PHYSICS_HIGH_PRECISION_OPT 1020
#define LOAD_IAEA_PHSP_FILE 1021
#define ULTRA_LONG_ALLOWED_DECAY_TIME 1022

#define RUN_CUT_THRESHOLD (INT_MAX)
//#define RUN_CUT_THRESHOLD (100)

void print_usage();

#endif //GEANT4_SIM_GDML_GEANT4_SIM_GDML_H
