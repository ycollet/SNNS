
%
% sample command-file for knete                             --- tr, 13.11.95 ---
% for encoder-decoder problem with input unit mutation
%



% --- files --------------------------------------------------------------------


network       n10.net
learnpattern  n10.pat
crosspattern   n10.pat 

historyFile   n10_hst
netDestName   n10_erg


% --- modules ------------------------------------------------------------------

genpopNepo
loadSNNSPat
initPop

stopErr      % stop if something's wrong with parents/offsprings
stopIt       % stop after maxGenerations 

preferSel

simpleMut
mutUnits
mutInputs

nullWeg
relearn   
learnSNNS 
prune     
cleanup   
nullWeg
learnSNNS 

learnRating    
topologyRating 
bestGuessHigh

histSimple   
histFitness  
histWeights
Xhist

fittestSurvive


saveAll



% --- params ------------------------------------------------------------------

Xcoord 0.0 0.0 30.0 1000.0

maxGenerations 30   % stop after  x  generations
popsize   40
gensize   15

minNoHidden 2
preferfactor 3.0

saveNetsCnt 5

learnModul learnSNNS

learnfct   Rprop
maxtss     0.1
maxepochs  150
learnparam 0.001 0.1 0.0 4.0 0.0


probMutUnits      0.4
probMutUnitsSplit 0.2

probMutInputs      0.1
probMutInputsSplit 0.5


threshold 0.5

relearnfactor 0.7

probadd 0.1
probdel 0.2

weightRating   30.0
unitRating    170.0
inputRating   120.0 
tssRating      10.0
noLearnRating 200.0

missRating     50.0
noneRating     50.0
hitDistance     0.4
hitThreshold    0.6



% eof

