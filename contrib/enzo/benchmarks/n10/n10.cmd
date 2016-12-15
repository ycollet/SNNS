
%
% sample command-file for knete                            --- tr, 13.11.95 ---
% for encoder-decoder problem
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


fittestSurvive


saveAll



% --- params ------------------------------------------------------------------



maxGenerations 30   % stop after  x  generations
popsize   40
gensize   15

minNoHidden 2
preferfactor 3.0

saveNetsCnt 5

learnModul learnSNNS

learnfct   Rprop
maxtss     0.05
maxepochs  200
learnparam 0.001 0.99 0.0 7.0 0.0


probMutUnits      0.6
probMutUnitsSplit 0.3


threshold 0.5

relearnfactor 0.7

probadd 0.2
probdel 0.1

weightRating  100.0    % 80
unitRating    150.0     % 90
tssRating       0.0
noLearnRating 200.0



missRating     50.0
noneRating     50.0
hitDistance     0.4
hitThreshold    0.6

% eof

