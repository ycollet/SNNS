
#
# sample command-file for knete                            --- tr, 13.11.95 ---
# for spiral problem  like this: 
#
#     #######%	
#     #%%%%%#%
#     #%#####%
#     #%%%%%%%
#      


# --- files --------------------------------------------------------------------


network       spirale1.net
learnpattern  spi_trn.pat
crosspattern   spi_tst.pat

historyFile   hst
netDestName   spi_erg


# --- modules ------------------------------------------------------------------


genpopNepo     # init nepomuk
loadSNNSPat    # load patterns
initPop        # create and initialize Population
optInitPop     # init population

stopErr        # stop if something's wrong with parents/offsprings
stopIt         # stop after maxGenerations 

preferSel

simpleMut      # mutate weights
mutUnits       # and units

relearn        # cut weights down
learnSNNS      # train nets
prune          # delete weights too small
cleanup        # remove unneccessary units
learnSNNS      # train again

learnRating    # measure learning fitness
topologyRating #         topological fitness
tssEval        #       

histSimple     
histFitness  
histWeights  
Xhist

fittestSurvive



saveAll



# --- params ------------------------------------------------------------------

Xcoord 0.0 0.0 30.0 1000.0

maxGenerations 30   # stop after  x  generations

popsize    30
gensize    10

preferfactor 4.0

learnModul learnSNNS    # use standard SNNS-learning
learnfct   Rprop 
maxtss     0.4
maxepochs  500
learnparam 0.0001 0.01 5.0 0.0 0.0

saveNetsCnt 10

threshold      0.4
#thresholdStart 0.0
#pruneEndGen     30

relearnfactor 0.8

probadd 0.2
probdel 0.1

probMutUnits      0.4
probMutUnitsSplit 0.3

weightRating   50.0
unitRating    150.0
tssRating       5.0
noLearnRating 200.0

crossTssRating    20.0


# eof


