#!/usr/bin/env python
import SimpleConfigParser
import os
import sys
import re
import shutil
import subprocess
import dependencyGraph

def checkUpdate():
	UserPath = getVar("ANALYSISFW_USERDIR", -1)
	if UserPath != -1:
		#Verify the Analyzer folders have been renamed Analyzers
		if os.path.exists("%s/Analyzer" % UserPath):
			if os.path.exists("%s/Analyzers" % UserPath):
				print """
You have a "Analyzer" folder in you user directory.
This folder has been renamed "Analyzers" but it seems to still exist.
Please move all your analyzers still inside to the correct "Analyzers" folder.
Warning : When moving your analyzers in the new folder, they may already exist
from the automatic update of the user folder. Take care of not overwriting
you previous work.
				"""
				sys.exit(0)
			else:
				os.rename("%s/Analyzer" % UserPath, "%s/Analyzers" % UserPath)

def check_histo(an, iPath):
	f1 = open("%s/src/%s.cc" % (iPath, an), 'r')
	inComment = False
	
	uses = []
	defs = set()
	i = 0
	error = False

	for line in f1:
		i = i + 1
		if line.strip().startswith("/*"):
			inComment=True
		if inComment and ("*/" in line.strip()[2:]):
			inComment=False
			continue
		if line.strip().startswith("//"):
			continue
		if inComment==True:
			continue
		m = re.findall("(fHisto|fHisto2|fGraph)\[\"(.*?)\"\]", line)
		for e in m:
			uses.append((e[1], i))
		m = re.findall("FillHisto\(\"(.*?)\"", line)
		for e in m:
			uses.append((e, i))	
		
		m = re.findall("BookHisto\(\"(.*?)\"", line)
		for e in m:
			defs.add(e)

	for e in uses:
		if not e[0] in defs:
			print "Error in analyzer %s : Histogram %s not defined at line %s" % (an, e[0],e[1])
			error = True
	f1.close()
	return error
	

def bash_command(cmd):
	shell = getVar("FWSHELL", "sh")
	if shell == "sh":
		p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
	else:
		p = subprocess.Popen(cmd, shell=True, executable='/bin/tcsh')
	p.wait()

def getCheckVar(name):
	var = os.getenv(name, -1);
	if var == -1:
		print "Environment variable %s was not found." % name
		sys.exit(0)
	return var

def getVar(name, default):
	var = os.getenv(name, -1);
	if var == -1:
		var = default
	return var

def cleanUser(UserPath):
	os.chdir(UserPath)
	bash_command("make clean")
	if(os.path.exists("main.cc")):
		os.remove("main.cc")
	if(os.path.exists("Makefile")):
		os.remove("Makefile")

def cleanFW(FWPath):
	os.chdir(FWPath)
	bash_command("make clean")

def available(FWPath, UserPath):
	l = os.listdir("%s/Analyzers/include" % FWPath)
	print "FW Analyzers : "
	for el in l:
		print "\t%s" % el.replace(".hh", "")
	l = os.listdir("%s/Examples/include" % FWPath)
	print "Examples Analyzers : "
	for el in l:
		print "\t%s" % el.replace(".hh", "")
	l = os.listdir("%s/Analyzers/include" % UserPath)
	print "User Analyzers : "
	for el in l:
		print "\t%s" % el.replace(".hh", "")

def getNextAnalyzer(analyzers, deps, ordered):
	for an in analyzers:
		match = True
		if len(deps[an]) == 0:
			return an
		else:
			for d in deps[an]:
				if not d in ordered:
					match = False
			if match == True:
				return an

def checkDependence(depsGraph, name, prefix):
	inComment = False
	f = open("%s/src/%s.cc" % (prefix, name), 'r')
	for line in f:
		if line.find("/**")>=0:
			inComment = True
		
		if inComment == True:
			if line.find("*/")>=0:
				inComment = False
		if inComment == False:
			if line.find("//")<0:
				m = re.search("(?:[^/+].*)?GetOutput(?:<.*>)?\(\"(.*)\..*\",.*\);", line)
				if m:
					depsGraph.addDependency(name, m.group(1))

def checkAnalyzerExists(an, FWPath, userPath):
	if os.path.exists("%s/Analyzers/include/%s.hh" % (FWPath, an)):
		return 1
	elif os.path.exists("%s/Analyzers/include/%s.hh" % (userPath, an)):
		return 2
	elif os.path.exists("%s/Examples/include/%s.hh" % (FWPath, an)):
		return 3
	else:
		return 0

def readAndReplace(iPath, oPath, searchMap):
	f1 = open(iPath, 'r')
	f2 = open(oPath, 'w')
	inComment = False
	
	for line in f1:
		if "//" in line and not "///" in line:
			continue
		if "/**" in line:
			inComment=True
		if inComment and ("*/" in line):
			inComment=False
			continue
		if inComment==True:
			continue
		for old in searchMap:
			line = line.replace(old, searchMap[old])
		f2.write(line)
	
	f2.flush()
	f2.close()
	f1.close()

def generateNewAnalyzer(name, FWPath, UserPath, inputs):
	if os.path.exists("%s/Analyzers/include/%s.hh" % (UserPath, name)):
		answer = raw_input("This analyzer already exists. Do you want to overwrite it [Y/N] ? ")
		if answer.lower() == 'y':
			os.remove("%s/Analyzers/include/%s.hh" % (UserPath, name))
			os.remove("%s/Analyzers/src/%s.cc" % (UserPath, name))
		else:
			print "Please choose a different name."
			return
	if os.path.exists("%s/Analyzers/include/%s.hh" % (FWPath, name)):
		print "This analyzer already exists. Please choose a different name."
		return
	if os.path.exists("%s/Examples/include/%s.hh" % (FWPath, name)):
		print "This analyzer already exists. Please choose a different name."
		return
	
	readAndReplace("%s/Templates/templateAnalyzer.hh" % FWPath, "%s/Analyzers/include/%s.hh" % (UserPath, name), {'templateAnalyzer':name, 'TEMPLATEANALYZER':name.upper()})
	readAndReplace("%s/Templates/templateAnalyzer.cc" % FWPath, "%s/Analyzers/src/%s.cc" % (UserPath, name), {'templateAnalyzer':name, '/*$$TREEINCLUDES$$*/':inputs[0], '/*$$TREEREQUEST$$*/':inputs[1], '/*$$GETEVENTS$$*/':inputs[2]})

def parseAnalyzerDef(name):
	m = re.findall("(.*?)\((.*)\)", name)
	if not m:
		return [name,0]
	else:
		anName = m[0][0]
		inputs = m[0][1].split(",")
	
	return [anName,inputs]

def createAnalyzer(name, FWPath, UserPath):
	[anName, inputs] = parseAnalyzerDef(name)
	
	if inputs==0:
		#No analyzer inputs specified. Old style new.
		generateNewAnalyzer(name, FWPath, UserPath, ['','',''])
		return
	
	treeInclude = ""
	treeRequest = ""
	getEvents = ""
	for i in inputs:
		i = i.strip()
		if not " " in i:
			print "Error while creating new analyzer %s: input type not specified for %s. Please add 'MC' or 'Reco' identifier before tree name." % (anName, i)
			return
		[type, tree] = i.split(" ")
		if type=="MC":
			className = "T%sEvent" % (tree)
		else:
			className = "TReco%sEvent" % (tree)
		
		treeInclude = "%s#include \"%s.hh\"\n" % (treeInclude,className)
		treeRequest = "%s\tRequestTree(\"%s\",new %s);\n" % (treeRequest,tree,className)
		getEvents = "%s\t%s *%sEvent = (%s*)GetEvent(\"%s\");\n" % (getEvents,className,tree,className,tree)
	
	generateNewAnalyzer(anName, FWPath, UserPath, [treeInclude,treeRequest,getEvents])
	
def renameAnalyzer(oldName, newName, FWPath, UserPath):
	if not os.path.exists("%s/Analyzers/include/%s.hh" % (UserPath, oldName)):
		print "This analyzer does not exist (%s). It cannot be renamed." % oldName
		return

	if os.path.exists("%s/Analyzers/include/%s.hh" % (UserPath, newName)):
		answer = raw_input("This analyzer (%) already exists. Do you want to overwrite it [Y/N] ? " % newName)
		if answer.lower() == 'y':
			os.remove("%s/Analyzers/include/%s.hh" % (UserPath, newName))
			os.remove("%s/Analyzers/src/%s.cc" % (UserPath, newName))
		else:
			print "Please choose a different name."
			return
	if os.path.exists("%s/Analyzers/include/%s.hh" % (FWPath, newName)):
		print "This analyzer already exists. Please choose a different name."
		return
	if os.path.exists("%s/Examples/include/%s.hh" % (FWPath, newName)):
		print "This analyzer already exists. Please choose a different name."
		return
	
	readAndReplace("%s/Analyzers/include/%s.hh" % (UserPath, oldName), "%s/Analyzers/include/%s.hh" % (UserPath, newName), {oldName:newName, oldName.upper():newName.upper()})
	readAndReplace("%s/Analyzers/src/%s.cc" % (UserPath, oldName), "%s/Analyzers/src/%s.cc" % (UserPath, newName), {oldName:newName})
	
	os.remove("%s/Analyzers/include/%s.hh" % (UserPath, oldName))
	os.remove("%s/Analyzers/src/%s.cc" % (UserPath, oldName))

def build(filename, FWPath, UserPath):
	if not os.path.exists(filename):
		print "The config file %s does not exist" % filename
		return
	
	cp = SimpleConfigParser.SimpleConfigParser()
	cp.read(filename)

	if cp.hasoption('analyzers'):
		analyzersList =  cp.getoption("analyzers")
	else:
		print "No analyzers found in config file"

	if cp.hasoption('exec'):
		executable = cp.getoption("exec")
	else:
		print "No executable name in config file"
	
	extralibs = []
	if cp.hasoption("libs"):
		extralibs = cp.getoption("libs").split()
	
	extralibsdirs = []
	if cp.hasoption("libsdirs"):
		extralibsdirs = cp.getoption("libsdirs").split()
	
	extraincludedirs = []
	if cp.hasoption("includedirs"):
		extraincludedirs = cp.getoption("includedirs").split()
	
	depsGraph = dependencyGraph.DependencyGraph()
	
	#analyzers = analyzersList.split()
	analyzers = re.findall(" ?(.+?(?:\(.+?\)|[ ]|$)) ?", analyzersList)
	print analyzers
	usrAnList = ""
	fwAnList = ""
	exAnList = ""
	prefixList = {}
	ordered = []
	missing = False
	
	for anDef in analyzers:
		an = parseAnalyzerDef(anDef)[0]
		anType = checkAnalyzerExists(an, FWPath, UserPath)
		if(anType==0):
			answer = raw_input("Analyzer %s does not exists. Do you want to create it [Y/N]" % (an))
			if answer.lower()=="y":
				createAnalyzer(anDef, FWPath, UserPath)
			else:
				missing = True
		else:
			if anType==1:
				prefix = "%s/Analyzers" % FWPath
				fwAnList += "%s " % an
			elif anType==2:
				prefix = "%s/Analyzers" % UserPath
				usrAnList += "%s " % an
			elif anType==3:
				prefix = "%s/Examples" % FWPath
				exAnList += "%s " % an
			prefixList[an] = prefix
			depsGraph.addNode(an)
			checkDependence(depsGraph, an, prefix)
			for d in depsGraph.getDependencies(an):
				if not d in analyzers:
					print "Missing dependance %s for %s" % (d, an)
					missing = True
	
	if missing == True:
		return
	
	p = depsGraph.getNextPath()
	while len(p)>0:
		if p[0]==-1:
			exit(0)
		for node in p:
			ordered.append(node)
		p = depsGraph.getNextPath()
	
	strExtralibs = ""
	for lib in extralibs:
		strExtralibs += "-l%s " % (lib)
	
	strExtralibsdirs = ""
	for dir in extralibsdirs:
		strExtralibsdirs += "-L%s " % (dir)

	strExtraIncdirs = ""
	for dir in extraincludedirs:
		strExtraIncdirs += "-I%s " % (dir)
	
	makefileDict = {"$$FWPath$$":FWPath, "$$USERCODEPATH$$":UserPath, "$$ANALYZERSHH$$":fwAnList, "$$USRHH$$":usrAnList, "$$EXAMPLEHH$$":exAnList, "$$EXEC$$": executable, "$$EXTRALIBS$$": strExtralibs, "$$EXTRALIBSDIRS$$": strExtralibsdirs, "$$EXTRAINCLUDEDIRS$$": strExtraIncdirs}
	readAndReplace("%s/Templates/Makefile" % FWPath, "%s/Makefile" % UserPath, makefileDict)
	
	includesList = ""
	instancesAnalyzer = ""
	deleteAnalyzer = ""
	error = False
	for an in ordered:
		#check histograms in analyzer
		error = error | check_histo(an, prefixList[an])
		includesList += """#include "%s.hh"\n""" % an;
		instancesAnalyzer += "\t%s *an_%s = new %s(ban);\n" % (an,an,an)
		instancesAnalyzer += "\tban->AddAnalyzer(an_%s);\n" % an
		deleteAnalyzer += "\tdelete an_%s;\n" % an
	
	readAndReplace("%s/Templates/main.cc" % FWPath, "%s/main.cc" % UserPath, {"$$ANALYZERSINCLUDE$$":includesList, "/*$$ANALYZERSNEW$$*/":instancesAnalyzer, "/*$$ANALYZERSDELETE$$*/":deleteAnalyzer})
	
	#Check if FW needs to be recompiled
	if not error:
		shell = getVar("FWSHELL", "sh")
		bash_command("cd %s && source ./scripts/env.%s && make" % (FWPath,shell))
		bash_command("make")


def prepareUserFolder(path, FWPath):
	path = path.rstrip("/")
	path = os.path.abspath(path)
	
	if not os.path.exists(path):
		os.makedirs(path)	
	else:
		if len(os.listdir(path))>0:
			print "The destination path is not empty. If you continue, the folder structure will be checked and eventually updated."
			print "The env.(c)sh and config files will be regenerated."
			answer = raw_input("Are you sure you want to continue [Y/N]?")
			if answer.lower() != "y":
				return
				
	
	NA62MCSOURCE = getCheckVar("NA62MCSOURCE")
	os.chdir(FWPath)
	shell = getVar("FWSHELL", "sh")
	bash_command("source ./scripts/env.%s && make" % shell)
	
	if not os.path.exists("%s/Analyzers" % path):
		os.mkdir("%s/Analyzers" % path)
	if not os.path.exists("%s/Analyzers/include" % path):
		os.mkdir("%s/Analyzers/include" % path)
	if not os.path.exists("%s/Analyzers/src" % path):
		os.mkdir("%s/Analyzers/src" % path)
	if not os.path.exists("%s/obj" % path):
		os.mkdir("%s/obj" % path)
	if not os.path.exists("%s/lib" % path):
		os.mkdir("%s/lib" % path)
	if not os.path.exists("%s/scripts" % path):
		os.mkdir("%s/scripts" % path)
	if not os.path.exists("%s/NA62.root" % path):
		shutil.copyfile("%s/NA62.root" % FWPath, "%s/NA62.root" % path)
	
	readAndReplace("Templates/env.sh", "%s/scripts/env.sh" % path, {"$$ANALYSISFW$$":os.getcwd(), "$$USERDIR$$":path, "$$NA62MCSOURCE$$":NA62MCSOURCE})
	readAndReplace("Templates/env.csh", "%s/scripts/env.csh" % path, {"$$ANALYSISFW$$":os.getcwd(), "$$USERDIR$$":path, "$$NA62MCSOURCE$$":NA62MCSOURCE})
	readAndReplace("Templates/config", "%s/config" % path, {})
	
	print "\nYour new user directory has been created. \nTo continue, go in %s, edit your config file, verify and source env.(c)sh, and run \nNA62AnalysisBuilder.py config" % path

def buildExample(fwPath, userPath):
	shell = getVar("FWSHELL", "sh")
	bash_command("cd %s && source ./scripts/env.%s && make && make example" % (fwPath,shell))
	shutil.copyfile("%s/Examples/examplePi0Config" % fwPath, "%s/examplePi0Config" % userPath)
	shutil.copyfile("%s/Examples/exampleSkimmingConfig" % fwPath, "%s/exampleSkimmingConfig" % userPath)
	readAndReplace("%s/Examples/exampleExportTreesConfig" % fwPath, "%s/exampleExportTreesConfig" % userPath, {"$$FWPATH$$":fwPath})

def printUsage():
	print "Usage:"
	print "NA62AnalysisBuilder.py help | cleanUser | cleanFW | available | configFileName | new AnalyzerName | rename oldName newName | prepare UserDirectory | examples"
	print "\t help : will display this message"
	print "\t cleanUser : will remove all files generated by the build in the user directory"
	print "\t cleanFW : will remove all files generated by the build in the FW directory"
	print "\t available : will display the list of available analyzers"
	print "\t configFileName : build the FW using this configuration file"
	print "\t new : create a new analyzer with name AnalyzerName in the user directory"
	print "\t rename : rename a user analyzer"
	print "\t prepare : prepare a new user directory at the specified path"
	print "\t examples : build the libraries for the examples and copy the config files into the user directory"



if __name__ == '__main__':
	if len(sys.argv)<2 or len(sys.argv)>4:
		printUsage()
		sys.exit(0)
	
	if getVar("ANALYSISFW_USERDIR", -1)!=-1:
		checkUpdate();
	
	filename = sys.argv[1]
	if len(sys.argv)==4:
		if filename == "rename":
			oldName = sys.argv[2]
			newName = sys.argv[3]
			renameAnalyzer(oldName, newName, getCheckVar("ANALYSISFW_PATH"), getCheckVar("ANALYSISFW_USERDIR"))
			sys.exit(0)
	elif len(sys.argv)==3:
		if filename == "new":
			anName = sys.argv[2]
			createAnalyzer(anName, getCheckVar("ANALYSISFW_PATH"), getCheckVar("ANALYSISFW_USERDIR"))
			sys.exit(0)
		if filename == "prepare":
			prepareUserFolder(sys.argv[2], getVar("ANALYSISFW_PATH", "."))
			sys.exit(0)
	elif len(sys.argv)==2:
		if filename=="help":
			printUsage()
			sys.exit(0)
		if filename=="cleanUser":
			cleanUser(getCheckVar("ANALYSISFW_USERDIR"))
			sys.exit(0)
		if filename=="cleanFW":
			cleanFW(getVar("ANALYSISFW_PATH", "."));
			sys.exit(0)
		if filename=="cleanAll":
			cleanUser(getCheckVar("ANALYSISFW_USERDIR"))
			cleanFW(getVar("ANALYSISFW_PATH", "."));
			sys.exit(0)
		if filename=="available":
			available(getCheckVar("ANALYSISFW_PATH"), getCheckVar("ANALYSISFW_USERDIR"))
			sys.exit(0)
		if filename=="examples":
			buildExample(getCheckVar("ANALYSISFW_PATH"), getCheckVar("ANALYSISFW_USERDIR"))
			sys.exit(0)
		
		build(filename, getCheckVar("ANALYSISFW_PATH"), getCheckVar("ANALYSISFW_USERDIR"))
	else:
		printUsage()
		sys.exit(0)

