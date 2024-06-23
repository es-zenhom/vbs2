TAG=abcdnet_v6
N_WORKERS=48

BASEDIR=/home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies
SKIMDIR=/home/users/eslam.zenhom/public_html/skimdata
SKIMTAG=0lep_2ak4_2ak8_ttH

mkdir -p $BASEDIR

# Run main analysis code
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=$TAG
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=$TAG

# Run without ParticleNet corrections
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_nofix --var=nofix
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_nofix

# Run JEC variations (OLD)
# ./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_up --var=jec_up --no_make
# ./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_dn --var=jec_dn --no_make
# ./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_up
# ./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_dn

# Run JER variations
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jer_up --var=jer_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jer_dn --var=jer_dn --no_make
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jer_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jer_dn

# Run JMR AND JMS (leaonardo additon)

##JMR
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jmr_up --var=jmr_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jmr_dn --var=jmr_dn --no_make

for VARDIR in $BASEDIR/vbsvvhjets_semimerged/output_${TAG}_jmr*; do
    cp -R -n $BASEDIR/vbsvvhjets_semimerged/output_${TAG}/* $VARDIR/
done

./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jmr_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jmr_dn


##JMS
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jms_up --var=jms_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jms_dn --var=jms_dn --no_make

for VARDIR in $BASEDIR/vbsvvhjets_semimerged/output_${TAG}_jms*; do
    cp -R -n $BASEDIR/vbsvvhjets_semimerged/output_${TAG}/* $VARDIR/
done

./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jms_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jms_dn


# Run all 22 JEC variations (11 sources x 2 for up/down)...
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_1_up --var=jec_1_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_1_dn --var=jec_1_dn --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_2_up --var=jec_2_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_2_dn --var=jec_2_dn --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_3_up --var=jec_3_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_3_dn --var=jec_3_dn --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_4_up --var=jec_4_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_4_dn --var=jec_4_dn --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_5_up --var=jec_5_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_5_dn --var=jec_5_dn --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_6_up --var=jec_6_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_6_dn --var=jec_6_dn --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_7_up --var=jec_7_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_7_dn --var=jec_7_dn --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_8_up --var=jec_8_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_8_dn --var=jec_8_dn --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_9_up --var=jec_9_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_9_dn --var=jec_9_dn --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_10_up --var=jec_10_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_10_dn --var=jec_10_dn --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_11_up --var=jec_11_up --no_make
./bin/run vbsvvhjets_semimerged --n_workers=$N_WORKERS --basedir=$BASEDIR --skimdir=$SKIMDIR --skimtag=$SKIMTAG --data --tag=${TAG}_jec_11_dn --var=jec_11_dn --no_make
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_1_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_1_dn
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_2_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_2_dn
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_3_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_3_dn
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_4_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_4_dn
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_5_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_5_dn
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_6_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_6_dn
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_7_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_7_dn
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_8_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_8_dn
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_9_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_9_dn
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_10_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_10_dn
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_11_up
./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_jec_11_dn

# Make extra copies for JECs that have to be separated by year (annoying, I know)
JECS="
jec_2
jec_3
jec_5
jec_8
jec_11
"
YEARS="
2016postVFP
2016preVFP
2017
2018
"
VARS="
up
dn
"
for JEC in $JECS; do
    for VAR in $VARS; do
        for YEAR in $YEARS; do
            NOMDIR=$BASEDIR/vbsvvhjets_semimerged/output_${TAG}                       # nominal
            VARDIR=$BASEDIR/vbsvvhjets_semimerged/output_${TAG}_${JEC}_${VAR}         # variation
            NEWDIR=$BASEDIR/vbsvvhjets_semimerged/output_${TAG}_${JEC}_${YEAR}_${VAR} # variation just for 2016, 2017, or 2018
            echo Creating $NEWDIR
            rm -rf $NEWDIR
            cp -R  $NOMDIR $NEWDIR
            rm -rf $NEWDIR/${YEAR}*
            cp -R  $VARDIR/${YEAR}* $NEWDIR
            ./bin/merge_vbsvvhjets_semimerged vbsvvhjets_semimerged --basedir=$BASEDIR --tag=${TAG}_${JEC}_${YEAR}_${VAR}
        done
    done
done
