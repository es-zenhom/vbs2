#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [[ "$1" == *".gz"* ]]; then
    LHEGZFILE=$1
    gunzip $LHEGZFILE
    LHEFILENAME=${LHEGZFILE/.lhe.gz/.lhe}
else
    LHEFILENAME=$1
fi

ROOTFILENAME=${LHEFILENAME/.lhe/.root}
python3 ${DIR}/../python/lhe2root.py ${LHEFILENAME} ${ROOTFILENAME}
makeclass.sh -f -x $ROOTFILENAME Physics LHE tas lhe
cp $DIR/../macro/process_madgraph.cc_template process.cc
cp $DIR/../python/plot_madgraph_analyzer.py .
