cd /home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis
source setup.sh;  
export PYTHONPATH="${PYTHONPATH}:/home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis"; 
source scripts/runall_vbsvvhjets_semimerged.sh 

#copy files from uaf to hipergator for training 
scp -r /home/users/eslam.zenhom/public_html/uf_work/vbs2/analysis/studies/vbsvvhjets_semimerged/output_abcdnet_v6/Run2 eslam.zenhom@hpg.rc.ufl.edu:/blue/p.chang/eslam.zenhom/data/vbsvvh/vbsvvh_semimerged_data/ 
sbatch --nodes=1 --cpus-per-task=16 --gpus=2  batch/full300.script configs/vbsvvh_semimerged_finalrun_correctsigfiles_150lamda.json
sbatch --nodes=1 --cpus-per-task=16 --gpus=2  batch/full300.script configs/vbsvvh_semimerged_finalrun_correctsigfiles_50lamda.json

#after finishing copy the files (you must be in hipergator device):
##for the 50 lambda
scp -r /home/eslam.zenhom/data/vbsvvh/vbsvvh_semimerged_finalrun_correctsigfiles_50lamda eslam.zenhom@uaf-10.t2.ucsd.edu:/home/users/eslam.zenhom/public_html/uf_work/vbs2/abcdnet/
scp /home/eslam.zenhom/uf_work/abcd/vbs/abcdnet/configs/vbsvvh_semimerged_finalrun_correctsigfiles_50lamda.json eslam.zenhom@uaf-10.t2.ucsd.edu:/home/users/eslam.zenhom/public_html/uf_work/vbs2/abcdnet/configs
###MODEL=/home/users/eslam.zenhom/public_html/uf_work/vbs2/abcdnet/vbsvvh_semimerged_finalrun_correctsigfiles_50lamda/models/vbsvvh_semimerged_finalrun_correctsigfiles_50lamda_modelLeakyNeuralNetwork_nhidden3_hiddensize64_lrConstantLR0.001_discolambda50_epoch300_model.pt
### CONFIG=configs/vbsvvh_semimerged_finalrun_correctsigfiles_50lamda.json


##for the 150 lamda
scp -r /home/eslam.zenhom/data/vbsvvh/vbsvvh_semimerged_finalrun_correctsigfiles_150lamda eslam.zenhom@uaf-10.t2.ucsd.edu:/home/users/eslam.zenhom/public_html/uf_work/vbs2/abcdnet/
scp /home/eslam.zenhom/uf_work/abcd/vbs/abcdnet/configs/vbsvvh_semimerged_finalrun_correctsigfiles_150lamda.json eslam.zenhom@uaf-10.t2.ucsd.edu:/home/users/eslam.zenhom/public_html/uf_work/vbs2/abcdnet/configs
###MODEL=/home/users/eslam.zenhom/public_html/uf_work/vbs2/abcdnet/vbsvvh_semimerged_finalrun_correctsigfiles_150lamda/models/vbsvvh_semimerged_finalrun_correctsigfiles_150lamda_modelLeakyNeuralNetwork_nhidden3_hiddensize64_lrConstantLR0.001_discolambda150_epoch300_model.pt
### CONFIG=configs/vbsvvh_semimerged_finalrun_correctsigfiles_150lamda.json


 
