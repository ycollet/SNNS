
#
# sample command-file for knete                             --- tr, 13.11.95 --
# for xor- problem 
#



# --- files --------------------------------------------------------------------


network       xor.net
learnpattern  xor.pat

historyFile   xor_hst
netDestName   xor_erg


# --- modules -----------------------------------------------------------------

genpopNepo 
loadSNNSPat
initPop
optInitPop

stopErr        # stop if something's wrong with parents/offsprings
stopIt         # stop after maxGenerations 

preferSel

simpleMut  
mutUnits   

nullWeg
relearn
learnSNNS 
prune
learnSNNS 
cleanup
nullWeg

learnRating    
topologyRating 

histSimple   
histFitness  
histWeights  
Xhist

fittestSurvive


saveAll



# --- params ------------------------------------------------------------------

Xgeometry 700 10 400 300
Xcoord 0.0 0.0 10.0 300.0

maxGenerations 10   # stop after  x  generations

popsize   20
gensize    5

learnModul learnSNNS

preferfactor 3.0

learnfct   Rprop
maxtss     0.1
maxepochs  60
learnparam 0.0 0.0 5.0 0.0 0.0

saveNetsCnt 5

probMutUnits 0.4
probMutUnitsSplit 0.8

threshold 0.7

relearnfactor 0.7

probadd 0.2
probdel 0.1

weightRating   80.0
unitRating    100.0
tssRating       5.0
noLearnRating 200.0


# eof
