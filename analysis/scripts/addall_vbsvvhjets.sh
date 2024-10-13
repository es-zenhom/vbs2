SR_X="abcdnet_score > 0.87"
SR_Y="abs_deta_jj > 6 and M_jj > 1300"
SR_GLOBAL="hbbfatjet_xbb > 0.8 and ld_vqqfatjet_xwqq > 0.6"
TAG=abcdnet_v12
BASEDIR=studies


for BABYDIR in $BASEDIR/vbsvvhjets_semimerged/output_${TAG}*; do
    BABYTAG=${BABYDIR##*output_}
    # Add ABCD regions to cutflows
    python3 scripts/add_abcd_regions.py vbsvvhjets_semimerged --basedir=$BASEDIR --tag=$BABYTAG --SR_x="$SR_X" --SR_y="$SR_Y" --SR_global="$SR_GLOBAL"
    echo "Copying from $BABYDIR to $PWD/studies/vbsvvhjets_semimerged"
    # cp -R $BABYDIR studies/vbsvvhjets_semimerged/
done

# cp /data/userdata/jguiang/bfscan_results/* studies/vbsvvhjets/output_$TAG/
