
#
# sample command-file for knete                            --- tr, 13.11.95 ---
# for classifying digits 
#



# --- files --------------------------------------------------------------------


network       sa.net
learnpattern  sa_learn.pat
crosspattern   sa_test.pat 

historyFile   sa_hst
netDestName   sa_erg


# --- modules ------------------------------------------------------------------


genpopNepo     # init nepomuk
loadSNNSPat    # load patterns
initPop
optInitPop     # init population

stopErr        # stop if something's wrong with parents/offsprings
stopIt         # stop after maxGenerations 

preferSel

mutLinks       # mutate weights
mutUnits       # and units
mutInputs

learnModul learnSNNS


relearn        # cut weights down
learnSNNS      # train nets
prune          # delete weights too small
cleanup        # remove unneccessary units
learnSNNS      # train again

learnRating    # measure learning fitness
topologyRating #         topological fitness
bestGuessHigh  #         generalization fitness

histSimple     
histFitness  
histWeights  
histCross


fittestSurvive



saveAll



# --- params ------------------------------------------------------------------



maxGenerations 100   # stop after  x  generations

popsize   30
gensize   10

preferfactor 4.0

learnfct   Rprop
maxtss     0.1
maxepochs  50   
learnparam 0.0 0.0 5.0 0.0  0.0

saveNetsCnt 5

threshold 0.7

relearnfactor 0.7

probadd 0.3
probdel 0.1

probMutUnits       0.4
probMutUnitsSplit  0.85

weightRating   80.0
unitRating    100.0
inputRating   100.0
tssRating       5.0
noLearnRating 200.0

missRating    150.0
noneRating    150.0
hitDistance     0.0
hitThreshold    0.0


