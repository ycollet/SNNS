#!/usr/bin/python

from snns import krui,util
import sys

def headline(txt) :
	print
	print txt
	print "-" * len(txt)
	print

print krui.getVersion()

print "Loadnet:"
try: 
	headline("general kernel information")
	funcnum = krui.getNoOfFunctions()
	print "Available functions:"
	for i in range(funcnum)[1:] :
		params = krui.getFuncInfo(i)
		print "Function:", params[0], 
		print " Type: ", 
		print util.func_types[params[1] & krui.FUNC_TYPE_MASK],
		print " i/o params: ",
		print krui.getFuncParamInfo(params[0],params[1])
		
	headline("some fixed dimension example (encoder.*)")
	net = krui.loadNet("encoder.net")
	print "Net loaded: " + net
	print krui.getMemoryManagerInfo()
	print "Number of input units:", krui.getNoOfTTypeUnits(krui.INPUT)
	print "Number of output units:", krui.getNoOfOutputUnits()
	print "Total number of units:", krui.getNoOfUnits()
	print "Number of special input units:", krui.getNoOfSpecialInputUnits()
	print krui.getNetInfo()
	print krui.getSymbolTable()
	if krui.setFirstFTypeEntry() :
		print "First prototype name:", krui.getFTypeName()
	else :
		print "No prototypes found."
		krui.createFTypeEntry("myftype","Act_Identity","Out_Identity")
		if krui.setFirstFTypeEntry() :
			print "New prototype defined:", krui.getFTypeName()

	succ = krui.getFirstSuccUnit(1)
	if not succ[0] :
		print "Unit 1 has no successor unit"
	else :
		print "Unit 1 has successor", succ[0],
		print "with link strength", succ[1] 
		num = krui.getNextSuccUnit()[0]
		while num :
			print "Next successor:", num
			num = krui.getNextSuccUnit()[0]
		print "Deleting a few links"
		krui.getFirstSuccUnit(9)
		krui.deleteLink()
		krui.getFirstSuccUnit(1)
		krui.getNextSuccUnit()
		krui.deleteAllInputLinks()
		krui.getNextSuccUnit()
		krui.deleteAllOutputLinks()
	
	krui.saveNet('foo.net','Testnet')
	print krui.getLearnFunc()
	#krui.setLearnFunc('RBF-DDA')
	print krui.getInitialisationFunc()
	print krui.getUpdateFunc()
	patset = krui.loadNewPatterns('encoder.pat')
	print "Patternset", patset, "loaded."
	print "Old pattern number:", krui.getPatternNo()
	# fiddle around with some patterns
	krui.setPatternNo(8)
	krui.deletePattern()
	krui.setPatternNo(5)
	krui.modifyPattern()
	krui.setPatternNo(7)
	krui.showPattern(krui.OUTPUT_OUT)
	krui.newPattern()
	krui.shufflePatterns(1)
	krui.shuffleSubPatterns(1)
	krui.saveNewPatterns('foo.pat',patset)
	krui.setPatternNo(1)
	(setinfo, patinfo) = krui.GetPatInfo()
	print "Number of patterns: ", setinfo.number_of_pattern
	print "Input Dimensions:", len(patinfo.input_dim_sizes)
	print "Output Dimensions:", len(patinfo.output_dim_sizes)
	print "Number of subpatterns for each pattern:", krui.DefTrainSubPat((),(),(),())
	print "Total number of subpatterns:", krui.getTotalNoOfSubPatterns()
	krui.setRemapFunc("None",())
	print "Learning all patterns, result:"
	print krui.learnAllPatterns((1.2,0))
	print "Learning pattern 3, result:"
	print krui.learnSinglePattern(3,(1.2,0))
	print "Updating unit 1"
	krui.updateSingleUnit(1)
	print "Updating net"
	krui.updateNet(())
	krui.saveResultParam('foo.res', 1,1,krui.getNoOfPatterns(),1,1,())
	krui.deleteAllPatterns()
	krui.deletePatSet(patset)
	patset = krui.allocNewPatternSet()
	patset2 = krui.allocNewPatternSet()
	krui.setCurrPatSet(patset)
	krui.deleteNet()
	#######################################################
	
	headline("some variable dimension example (watch.*)")
	
	krui.loadNet('watch.net')
	patset = krui.loadNewPatterns('watch.pat')
	(setinfo, patinfo) = krui.GetPatInfo()
	print "Number of patterns: ", setinfo.number_of_pattern
	print "Input Dimensions:", len(patinfo.input_dim_sizes)
	print "Output Dimensions:", len(patinfo.output_dim_sizes)
	scheme = ((200,150),(199,149),(25,10),(5,10))
	print "Number of subpatterns for each pattern using scheme", scheme,":", krui.DefTrainSubPat(scheme[0],scheme[1],scheme[2],scheme[3])
	print "Shapes of first five subpatterns:"
	for i in range(1,5):
		print krui.GetShapeOfSubPattern(i)
	print "Total number of subpatterns:", krui.getTotalNoOfSubPatterns()
	patnum = krui.AlignSubPat((100,50),(50,70))
	print "Closest pattern to (100,50),(50,70) is #", patnum
	print "Shape: ", krui.GetShapeOfSubPattern(patnum)
	krui.deleteNet()
	krui.deleteAllPatterns()
	########################################################
	
	headline("some classes example (letters*)")
	krui.loadNet('letters.net')
	patset = krui.loadNewPatterns('letters_with_classes.pat')
	(setinfo,patinfo) = krui.GetPatInfo()
	print "Names of classes:", setinfo.class_names
	print "Class distribution:", setinfo.class_redistribution
	print "Changing to 1,2"
	krui.setClassDistribution(1,2)
	krui.setPatternNo(5)
	krui.setClassInfo('Foo')
	(setinfo,patinfo) = krui.GetPatInfo()
	print "New class names:", setinfo.class_names
	print "New distribution:", setinfo.class_redistribution
	##########################################################
	
	headline("sites")
	print krui.getSiteTable()

	if krui.setFirstSite() :
		print "Selected site", krui.getSiteName()
	else :
		print "No site for current unit found"

	headline("unit functions")
	print "Unit 1 has the name", krui.getUnitName(1)
	print "Setting the name to foo"
	krui.setUnitName(1,"foo")
	print "Unit with name 'u35' has number", krui.searchUnitName("u35")

	for i in range(1,4) :
		print "Position of unit %d:" % i, krui.getUnitPosition(i)
	print "Swapping units 1 and 3"
	pos = krui.getUnitPosition(3)
	krui.setUnitPosition(3,krui.getUnitPosition(1))
	krui.setUnitPosition(1,pos)
	for i in range(1,4) :
		print "Position of unit %d:" % i, krui.getUnitPosition(i)
	pos = (2,1,0)
	print "Unit at position", pos, "has number", krui.getUnitNoAtPosition(pos,0)
	print "Next unit to the origin is unit",
	print krui.getUnitNoNearPosition((0,0,0),0,10,10)
	print "Setting unit center for unit 1, center 0"
	krui.setUnitCenters(1,0,(1.2,1.3,1.4,1.5))
	print "Unit centers for unit 1 and center 0:", str(krui.getUnitCenters(1,0))
	print "Input type of unit 10:", util.unit_input_types[krui.getUnitInputType(10)]
	print "Unit defaults:", krui.getUnitDefaults()


except:
	headline("There was some error!")
	print "Please copy the following files from the original SNNS examples"
	print "to the current directory and make sure you have write permissions:"
	print "letters.net"
	print "encoder.net"
	print "watch.net"
	print "letters_with_classes.pat"
	print "encoder.pat"
	print "watch.pat"
	raise
