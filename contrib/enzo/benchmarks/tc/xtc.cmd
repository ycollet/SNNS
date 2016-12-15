
#
# sample command-file for tc-problem                       --- TR, 22.08.95 ---
# use the seed == 814288352 to receive optimal result
# in about 20 generations !
#


#########################
#        files          #
#########################


network      tc.net         # name of network-file
learnpattern tc.pat         # name of learnpattern-file
testpattern  tc.pat         # name of testpattern-file

historyFile  hst            # append this suffix to history files
netDestName  tc_erg         # save nets with this prefix


#########################
#        modules        #
#########################


# ----- initialize and pre-evolution -----------------------------------------

genpopNepo     # initialize nepomuk
loadSNNSPat    # load SNNS-Pattern
initPop        # create and initialize Population
optInitPop     # optimize initial population

# ----- stop-evolution -------------------------------------------------------

stopErr        # stop if something's wrong with parents/offsprings
stopIt         # stop after maxGenerations 

# ----- selection -------------------------------------------------------------

preferSel      # use random prefer selection

# ----- mutation --------------------------------------------------------------

simpleMut      # do simple mutation
mutUnits       # delete or add hidden units
mutLinks       # delete or add links
mutInputs      # cut off input units

# ----- optimize ------------------------------------------------------------

nullWeg        # delete useless nets
relearn        # scale weights before relearning
learnSNNS      # do learning via SNNS-Function

#learnCupit    # do learning via CuPit / switch with learnSNNS 

prune          # use pruning 
cleanup        # remove dead units
nullWeg        # delete useless nets 
learnSNNS      # do re-learning via SNNS-Function
#learnCupit    # do learning via CuPit / switch with learnSNNS 


# ----- evaluate -------------------------------------------------------------

learnRating    # increase fitness if training patterns were not learned
topologyRating # increase fitness dependent on topology

# ----- history ---------------------------------------------------------------

histSimple    # write standard information
histFitness   # write fitness information
histWeights   # write weight and unit information
Xhist         # show fitness in a X-Window 

# ----- survival --------------------------------------------------------------

fittestSurvive  # sort nets by fitness (the lower the better)

# ----- post-evolution --------------------------------------------------------

saveAll         # save networks


#########################
#     parameters        #
#########################



maxGenerations 30   # stop after  x  generations

popsize   30        # population size
gensize   10        # new members each generation

preferfactor 3.0    # preferfactor for better nets


# ----- optimization --------------------------------------------------------

learnModul learnSNNS    # use standard SNNS-learning
#learnModul learnCupit    # use CuPit - learning

learnfct   Rprop                  # learning function to be set
learnparam 0.00 20.99 5.0 0.0 0.0  # and parameters
maxtss     0.01                   # stop learning if error is smaller 
maxepochs  100                    # stop after x epochs
shuffle 1                         # shuffle patterns
relearnfactor 0.5                 # scale weights before relearning by x

threshold 0.2                     # minimal treshhold for pruning
thresholdStart 0.5                # Start with this treshold
pruneEndGen 30                    # use minimal treshold from this generation  




# ----- mutation -------------------------------------------------------------

probadd 0.2               # probability of adding a link
probdel 0.1               # probability of removing a link

probMutUnits 0.1          # probability of unit mutation 
probMutUnitsSplit 0.9     # probability relation between adding and deleting

probMutInputs      0.6    # probability of input unit mutation 
probMutInputsSplit 0.6    # probability relation between adding and deleting



# ----- evaluation -----------------------------------------------------------

                          # linear sum of:
weightRating   20.0       # number of weights times x
unitRating     20.0       # number of units times x
inputRating   200.0       # number of inputs units times x
tssRating      30.0       # mean error  times x
noLearnRating 200.0       # add x if training patterns were not learned

#testHamThresh   0.2       # Treshold for hamming distance
#testHamRating  10.0       # Add x if over treshold  

# ----- history ---------------------------------------------------------------

Xgeometry 650 10 600 330   # size and position of X-Window
Xcoord 0.0 0.0 30.0 600.0  # size of the coordinate system


# ----- post-evolution --------------------------------------------------------

saveNetsCnt 5             # save x nets

# eof













