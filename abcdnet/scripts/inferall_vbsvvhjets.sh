TAG=abcdnet_v3
CONFIG=configs/vbsvvh_semimerged_finalrun.json
MODEL=/home/users/eslam.zenhom/public_html/uf_work/vbs2/abcdnet/vbsvvh_semimerged_finalrun/models/vbsvvh_semimerged_finalrun_modelLeakyNeuralNetwork_nhidden3_hiddensize64_lrConstantLR0.001_discolambda150_epoch300_model.pt

python3 scripts/infer_ucsd.py $CONFIG $MODEL --baby_glob="/home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/output_${TAG}*/Run2"
