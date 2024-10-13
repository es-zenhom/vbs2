#!/bin/bash

VERSION_TAG="output_abcdnet_v11"
# Define your cut values here
CUT_ABCDNET_SCORE=0.86
CUT_ABS_DETA_JJ=6.1
CUT_M_JJ=1400.0

# Base directory for inferences
BASE_DIR="/home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/${VERSION_TAG}/Run2/inferences"

# Set the dump directory for histograms
HISTOGRAMS_DIR="/home/users/eslam.zenhom/public_html/dump"
mkdir -p "$HISTOGRAMS_DIR"

# New variable for the all events flag
ALL_EVENTS_FLAG=${1:-""}  # Use the first argument if provided, otherwise empty string

OUTPUT_CSV="${HISTOGRAMS_DIR}/output_mjj${CUT_M_JJ}_abcdnet${CUT_ABCDNET_SCORE}_deta${CUT_ABS_DETA_JJ}_${VERSION_TAG}_150lamda_final.csv"

# Compile the program
g++ grapher.C -c -o program.o $(root-config --cflags --libs)
g++ program.o -o myProgram $(root-config --ldflags --libs) -lGenVector

# Function to run myProgram with correct paths
run_my_program() {
    local input_file="$1"
    local output_file="$2"
    ./myProgram "$HISTOGRAMS_DIR" "$input_file" "${HISTOGRAMS_DIR}/${output_file}" $CUT_ABCDNET_SCORE $CUT_ABS_DETA_JJ $CUT_M_JJ "$OUTPUT_CSV" $ALL_EVENTS_FLAG
}

# List of background files
BKG_FILES=("QCD.root" "VV_VH.root" "rare_top.root" "wjets.root" "zjets.root" "rare_EWK.root" "ttbar.root")

# Hadd background files
COMBINED_BKG="QCD.root"
if [ -f "${HISTOGRAMS_DIR}/${COMBINED_BKG}" ]; then
    echo "Combined background file already exists. Removing it."
    rm "${HISTOGRAMS_DIR}/${COMBINED_BKG}"
fi
hadd "${HISTOGRAMS_DIR}/${COMBINED_BKG}" ${BKG_FILES[@]/#/$BASE_DIR/}



# Run myProgram for combined background, signal, and data
run_my_program "QCD.root" "background_combined.root"
echo "Script completed successfully."