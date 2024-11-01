#!/bin/sh

../build/geant4-sim-gdml -i ../gdml/test.gdml -x ../macro/run.mac -p proton --threads=2 --powerlaw --energylow=1 --energyhigh=10 --alpha=0 -n 20000