#!/usr/bin/env python

import scripts.SimpleConfigParser as SimpleConfigParser
import os
import sys
import re
import shutil
import subprocess
from scripts import dependencyGraph
import ConfigParser

try:
	from argparse import ArgumentParser, RawDescriptionHelpFormatter
except ImportError:
	from scripts.argparse import ArgumentParser, RawDescriptionHelpFormatter
import scripts


__rev__ = 737
__descr__ = ("""
   Use this script when working with NA62Analysis. The script takes care of
   operations like preparing the environment, creating, renaming and cleaning 
   analyzers.
   It also takes care of parsing the configuration file and generating all necessary
   information to build the framework with the desired analyzers.
""")

#----- Version handling -----
# Version Check
# Version Update
def getUserVersion(UserPath):
	version = ""
	#Get the user version from the hidden file in the user directory
	if os.path.exists("%s/.version" % UserPath):
		f = open("%s/.version" % UserPath, 'r')
		version = f.read()
		f.close()
	
	rev = 0
	#Try to parse it as integer
	try:
		rev = int(version)
	finally:
		return rev

def writeUserVersion(UserPath):
	global __rev__
	f = open("%s/.version" % UserPath, 'w')
	f.write(str(__rev__))
	f.close()
	
def updateOld(UserPath, FWPath):
	#Verify the Analyzer folders have been renamed Analyzers
	if os.path.exists("%s/Analyzer" % UserPath):
		if os.path.exists("%s/Analyzers" % UserPath):
			print ("""
   \033[33;1m WARNING: You have a "Analyzer" folder in you user directory.
   This folder has been renamed "Analyzers" but it seems to still exist.
   Please move all your analyzers still inside to the correct "Analyzers" folder.
   Warning : When moving your analyzers in the new folder, they may already exist
   from the automatic update of the user folder. Take care of not overwriting
   you previous work.\033[0m
""")
			sys.exit(0)
		else:
			os.rename("%s/Analyzer" % UserPath, "%s/Analyzers" % UserPath)
	
	#Verify that the PhysicsObjects folder has been created
	if not os.path.exists("%s/PhysicsObjects" % UserPath):
		os.mkdir("%s/PhysicsObjects" % UserPath)
	if not os.path.exists("%s/PhysicsObjects/include" % UserPath):
		os.mkdir("%s/PhysicsObjects/include" % UserPath)
	if not os.path.exists("%s/PhysicsObjects/src" % UserPath):
		os.mkdir("%s/PhysicsObjects/src" % UserPath)
	if FWPath != -1:
		if not os.path.exists("%s/PhysicsObjects/CMakeLists.txt" % UserPath):
			shutil.copyfile("%s/Templates/CMakeLists_PO.txt" % FWPath, "%s/PhysicsObjects/CMakeLists.txt" % UserPath)
		if not os.path.exists("%s/CMakeLists.txt" % UserPath):
			shutil.copyfile("%s/Templates/CMakeLists.txt" % FWPath, "%s/CMakeLists.txt" % UserPath)

def updateHeaderSignature(UserPath):
	print ("""\033[31m
	The signature of the Process and DefineMCSimple method of the analyzer have changed. This requires
	to change the method signature in every analyzer header in Analyzers/include. 
	Replace both
	\t\033[32mvoid Process(int i, MCSimple &fMCSimple, Event* MCTruthEvent);\033[31m
	\t\033[32mvoid DefineMCSimple(MCSimple *fMCSimple);\033[31m
	with 
	\t\033[32mvoid Process(int i);\033[31m
	\t\033[32mvoid DefineMCSimple();\033[31m
	It also requires to change the source file of every analyzer.
	Replace both
	\t\033[32mvoid clusterNN::Process(int iEvent, MCSimple &fMCSimple, Event* MCTruthEvent){\033[31m
	\t\033[32mvoid clusterNN::DefineMCSimple(MCSimple *fMCSimple){\033[31m
	with 
	\t\033[32mvoid clusterNN::Process(int iEvent){\033[31m
	\t\033[32mvoid clusterNN::DefineMCSimple(){\033[31m
	and if you want access to MCTruthEvent you can now use
	\t\033[32mEvent* MCTruthEvent = GetMCEvent();\033[31m
	You can either allow this script to attempt to automatically change all your
	analyzers, or do it manually. In case you allow this script to automatically
	do the changes, a backup copy of your analyzers will be created in Analyzers/bckp.
\033[0m""")
	
	answer = raw_input("\tDo you want this script to automatically modify your analyzers [Y/N]?")
	if answer.lower() != "y":
		return
	
	if not os.path.exists("%s/Analyzers/bckp" % (UserPath)):
		os.mkdir("%s/Analyzers/bckp" % (UserPath))
	if not os.path.exists("%s/Analyzers/bckp/include" % (UserPath)):
		os.mkdir("%s/Analyzers/bckp/include" % (UserPath))
	if not os.path.exists("%s/Analyzers/bckp/src" % (UserPath)):
		os.mkdir("%s/Analyzers/bckp/src" % (UserPath))
	
	#Read user Analyzers include dir
	anList = os.listdir("%s/Analyzers/include" % UserPath)
	for header in anList:
		if (os.path.isdir(header)):
			continue
		#For each file, search "void Process(int xxx, MCSimple &xxx, Event* xxx);" and replace with "void Process(int iEvent);"
		original = "%s/Analyzers/include/%s" % (UserPath,header)
		backup = "%s/Analyzers/bckp/include/%s.bak" % (UserPath,header)
		
		shutil.copy2(original, backup)
		if not os.path.exists(backup):
			print "\033[33;1m WARNING: Unable to create backup copy of %s in %s\033[0m" % (original,backup)
			continue
		with open(backup, 'r') as f1:
			with open(original, 'w') as f2:
				print "Updating " + original
				for line in f1:
					#search
					m1 = re.findall("(.*)void Process\(int(.*),.*MCSimple.*&.*,.*Event.*\*.*\);", line)
					m2 = re.findall("(.*)void DefineMCSimple\(.*MCSimple.*\*.*\);", line)
					if m1:
						print "\033[31m- "+line.replace("\n","")+"\033[0m"
						print "\033[32m+ "+m1[0][0]+"void Process(int"+m1[0][1]+");\n\033[0m",
						f2.write(m1[0][0]+"void Process(int"+m1[0][1]+");\n")
					elif m2:
						print "\033[31m- "+line.replace("\n","")+"\033[0m"
						print "\033[32m+ "+m2[0]+"void DefineMCSimple();\n\033[0m",
						f2.write(m2[0]+"void DefineMCSimple();\n")
					else:
						f2.write(line)
	
	#Read user Analyzers src dir
	anList = os.listdir("%s/Analyzers/src" % UserPath)
	for source in anList:
		if(os.path.isdir(source)):
			continue
		#For each file, search "void clusterNN::Process(int ${1}, MCSimple &xxx, Event* ${2}){"
		# and replace with
		#	void clusterNN::Process(int ${1}){
		#		Event* ${2}
		#		if(GetWithMC()) ${2}= GetMCEvent()
		original = "%s/Analyzers/src/%s" % (UserPath,source)
		backup = "%s/Analyzers/bckp/src/%s.bak" % (UserPath,source)
		
		shutil.copy2(original, backup)
		if not os.path.exists(backup):
			print "\033[33;1m WARNING: Unable to create backup copy of %s in %s\033[0m" % (original,backup)
			continue
		
		with open(backup, 'r') as f1:
			with open(original, 'w') as f2:
				print "Updating " + original
				for line in f1:
					#search
					m1 = re.findall("(.*)void (.*)::Process\(int(.*),.*MCSimple.*&.*,.*Event.*\*(.*)\)(.*)", line)
					m2 = re.findall("(.*)void (.*)::DefineMCSimple\(.*MCSimple.*\*.*\)(.*)", line)
					m3 = re.findall("(.*)fMCSimple->(.*)", line)
					if m1:
						print "\033[31m- "+line.replace("\n","")+"\033[0m"
						print "\033[32m+ "+m1[0][0]+"void "+m1[0][1]+"::Process(int"+m1[0][2]+")"+m1[0][4]+"\033[0m\n",
						print "\033[32m+ "+m1[0][0]+"\tEvent* "+m1[0][3]+" = NULL;\033[0m\n",
						print "\033[32m+ "+m1[0][0]+"\tif(GetWithMC()) "+m1[0][3]+"= GetMCEvent();\033[0m\n",
						f2.write(m1[0][0]+"void "+m1[0][1]+"::Process(int"+m1[0][2]+")"+m1[0][4]+"\n")
						f2.write(m1[0][0]+"\tEvent* "+m1[0][3]+" = NULL;\n")
						f2.write(m1[0][0]+"\tif(GetWithMC()) "+m1[0][3]+"= GetMCEvent();\n")
					elif m2:
						print "\033[31m- "+line.replace("\n","")+"\033[0m"
						print "\033[32m+ "+m2[0][0]+"void "+m2[0][1]+"::DefineMCSimple()"+m2[0][2]+"\033[0m\n",
						f2.write(m2[0][0]+"void "+m2[0][1]+"::DefineMCSimple()"+m2[0][2]+"\n")
					elif m3:
						print "\033[31m- "+line.replace("\n","")+"\033[0m"
						print "\033[32m+ "+m3[0][0]+"fMCSimple."+m3[0][1]+"\033[0m\n",
						f2.write(m3[0][0]+"fMCSimple."+m3[0][1]+"\n")
					else:
						f2.write(line)
	

def updateSettings(UserPath, FwPath):
	p = ConfigParser.RawConfigParser()
	p.add_section("Global")
	p.read("%s/Templates/settingsna62" % FwPath)
	p.read("%s/.settingsna62" % UserPath)

	with open("%s/.settingsna62" % UserPath, "wb") as configFile:
		p.write(configFile)
		
	with open("%s/.settingsna62" % UserPath, "r") as configFile:
		lines = configFile.readlines()
		for i,line in enumerate(lines):
			if line=="[Global]\n":
				lines.insert(i+1, "; UseColors : If set to true NA62Analysis can use colors in the standard and error output.\n")
				lines.insert(i+2, "; ProcessOutputNewLine : NA62Analysis regularly outputs the number of events already read.\n")
				lines.insert(i+3, ";  If set to true this output is printed on a new line every time.\n") 
				lines.insert(i+4, ";  If set to false, a carriage return is used instead of a new line and the line is replaced.\n")
				break
	
	with open("%s/.settingsna62" % UserPath, "wb") as configFile:
		configFile.writelines(lines)
				
	
	
def checkUpdate():
	global __rev__
	UserPath = getVar("ANALYSISFW_USERDIR", -1)
	FWPath = getVar("ANALYSISFW_PATH", -1)
	NA62MCSOURCE = getCheckVar("NA62MCSOURCE")
	
	if UserPath != -1:
		if not os.path.exists(UserPath) or not os.path.exists("%s/Analyzers" % UserPath):
			return
		#check version number
		version = getUserVersion(UserPath)
		
		#Old versions with no version number
		updateOld(UserPath,FWPath)
		
		#Newer versions
		if int(version)<__rev__:
			print "\033[94mUpdating user directory from revision %s to revision %s \033[0m\n" % (version,__rev__)
			if(int(version))<=385:
				updateHeaderSignature(UserPath)
			
			updateSettings(UserPath, FWPath)
			#Always replace the CMakeLists.txt in case it changed
			shutil.copyfile("%s/Templates/CMakeLists.txt" % FWPath, "%s/CMakeLists.txt" % UserPath)
			shutil.copyfile("%s/Templates/CMakeLists_PO.txt" % FWPath, "%s/PhysicsObjects/CMakeLists.txt" % UserPath)
			
			readAndReplace("%s/Templates/env.sh" % FWPath, "%s/scripts/env.sh" % UserPath, {"$$ANALYSISFW$$":FWPath, "$$USERDIR$$":UserPath, "$$NA62MCSOURCE$$":NA62MCSOURCE})
			readAndReplace("%s/Templates/env.csh" % FWPath, "%s/scripts/env.csh" % UserPath, {"$$ANALYSISFW$$":FWPath, "$$USERDIR$$":UserPath, "$$NA62MCSOURCE$$":NA62MCSOURCE})
	
			#write the new version file
			writeUserVersion(UserPath)

#----- Utility functions -----
# Directory listing with filtering
def listDirClean(path):
	list = os.listdir(path)
	list = [x for x in list if not ".svn" in x]
	list = [x for x in list if os.path.isdir("%s/%s" %(path,x))]
	return list

def listFileClean(path):
	list = os.listdir(path)
	list = [x for x in list if not ".svn" in x]
	list = [x for x in list if not os.path.isdir("%s/%s" %(path,x))]
	return list

# Execute a command in suitable shell
def bash_command(cmd):
	shell = getVar("FWSHELL", "sh")
	if shell == "sh":
		p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
	else:
		p = subprocess.Popen(cmd, shell=True, executable='/bin/tcsh')
	p.wait()

# Check existence of environment variable an get it 
def getCheckVar(name):
	var = os.getenv(name, -1);
	if var == -1:
		print "\033[33;1m WARNING: Environment variable %s was not found.\033[0m" % name
		sys.exit(0)
	return var

# Get environment variable and set to default if not existing
def getVar(name, default):
	var = os.getenv(name, -1);
	if var == -1:
		var = default
	return var

# Read a template file and copy it to oPath, replacing every 
# matching string found in the searchMap by its value
def readAndReplace(iPath, oPath, searchMap, skipComments=True):
	inComment = False
	
	with open(iPath, 'r') as f1:
		with open(oPath, 'w') as f2:
			for line in f1:
				if skipComments:
					#Skip comment lines (but not doxygen doc)
					if "//" in line and not "///" in line:
						continue
					#Skip comment blocks 
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
# Test if a file is binary or text
def is_binary(filename):
    """Return true if the given filename is binary.
    @raise EnvironmentError: if the file does not exist or cannot be accessed.
    @attention: found @ http://bytes.com/topic/python/answers/21222-determine-file-type-binary-text on 6/08/2010
    @author: Trent Mick <TrentM@ActiveState.com>
    @author: Jorge Orpinel <jorge@orpinel.com>"""
    fin = open(filename, 'rb')
    try:
        CHUNKSIZE = 1024
        while 1:
            chunk = fin.read(CHUNKSIZE)
            if '\0' in chunk: # found null byte
                return True
            if len(chunk) < CHUNKSIZE:
                break # done
    finally:
        fin.close()

    return False
  
#----- Analyzer handling functions -----
# Check histogram use and booking coherence 
def check_histo(an, iPath):
	with open("%s/src/%s.cc" % (iPath, an), 'r') as f1:
		inComment = False
		
		uses = []
		defs = set()
		error = False
	
		for i,line in enumerate(f1):
			#Skip comment blocks
			if line.strip().startswith("/*"):
				inComment=True
			if inComment and ("*/" in line.strip()[2:]):
				inComment=False
				continue
			if line.strip().startswith("//"):
				continue
			if inComment:
				continue
			
			#List booked histograms in analyzer
			m = re.findall("(fHisto|fHisto2|fGraph)\[\"(.*?)\"\]", line)
			for e in m:
				uses.append((e[1], i))
			
			#List used histograms in analyzer
			m = re.findall("FillHisto\(\"(.*?)\"", line)
			for e in m:
				uses.append((e, i))	
			
			m = re.findall("BookHisto\(\"(.*?)\"", line)
			for e in m:
				defs.add(e)
		
		# Check all used histograms have been booked
		for e in uses:
			if not e[0] in defs:
				print "\033[31;1mError in analyzer %s : Histogram %s not defined at line %s\033[0m" % (an, e[0],e[1])
				error = True
	
	return error

# Check analyzer dependencies with dependencyGraph
def checkDependence(depsGraph, name, prefix):
	inComment = False
	with open("%s/src/%s.cc" % (prefix, name), 'r') as f:
		for line in f:
			# skip comment blocks
			if line.find("/**")>=0 and line.find("//**")<0:
				inComment = True
			if inComment:
				if line.find("*/")>=0:
					inComment = False
				else:
					continue
			#skip comment lines
			if line.strip().startswith("//"):
				continue
			
			# Regex matching analyzer call introducing dependency
			m = re.search("(?:[^/+].*)?GetOutput(?:<.*>)?\(\"(.*)\..*\",.*\);", line)
			if m:
				print m.group(1)
				depsGraph.addDependency(name, m.group(1))

# Check analyzer existence and return to which hierarchy it belongs to
def checkAnalyzerExists(an, FWPath, userPath):
	for dir in listDirClean("%s/Analyzers" % (FWPath)):
		if os.path.exists("%s/Analyzers/%s/include/%s.hh" % (FWPath, dir, an)):
			return [1,dir]
	if os.path.exists("%s/Analyzers/include/%s.hh" % (userPath, an)):
		return [2,""]
	elif os.path.exists("%s/Examples/include/%s.hh" % (FWPath, an)):
		return [3,""]
	else:
		return [0,""]

#----- Building functions -----
def buildFW(FWPath, defines, jobs):
	shell = getVar("FWSHELL", "sh")
	if not os.path.exists("%s/build" % FWPath):
		command = ["cd %s && source ./scripts/env.%s && cmake -H. -Bbuild" % (FWPath,shell)]
		for d in defines or []:
			command.append(" -D%s=1" % d)
		bash_command(''.join(command))
	bash_command("cd %s/build && source ../scripts/env.%s && make -j%s" % (FWPath,shell, jobs))

def buildUser(defines, jobs):
	shell = getVar("FWSHELL", "sh")
	if not os.path.exists("build"):
		command = ["source ./scripts/env.%s && cmake -H. -Bbuild" % (shell)]
		for d in defines or []:
			command.append(" -D%s=1" % d)
		bash_command(''.join(command))
	bash_command("cd build && source ../scripts/env.%s && make -j%s" % (shell, jobs))

# Parse analyzer definition from config file (or command line)
# Matching analyzerName(InputType InputName, ...)
def parseAnalyzerDef(name):
	m = re.findall("(.*?)\((.*)\)", name)
	if not m:
		return [name,0]
	else:
		anName = m[0][0]
		inputs = m[0][1].split(",")
	
	return [anName,inputs]

#-----------------------------
#----- Command functions -----
#-----------------------------
#----- Clean functions -------
def cleanUser(args):
	UserPath = getCheckVar("ANALYSISFW_USERDIR")
	
	os.chdir(UserPath)
	print "rm obj/*.o"
	for f in os.listdir("%s/obj" % UserPath):
		if ".o" in f:
			os.remove("obj/%s" % (f))
	print "rm lib/*.so"
	print "rm lib/*.a"
	for f in os.listdir("%s/lib" % UserPath):
		if (".so" in f) or (".a" in f):
			os.remove("lib/%s" % (f))
	print "rm outfile.root"
	if(os.path.exists("outfile.root")):
		os.remove("outfile.root")
		
	if(os.path.exists("main.cc")):
		os.remove("main.cc")
	if(os.path.exists("Makefile")):
		os.remove("Makefile")
	if(os.path.exists("build")):
		shutil.rmtree("build")

def cleanFW(args):
	FWPath = getVar("ANALYSISFW_PATH", ".")
	
	os.chdir(FWPath)
	print "rm obj"
	print "rm lib"
	shutil.rmtree("obj", True)
	shutil.rmtree("lib", True)
	print "rm *.pyc"
	for f in os.listdir("%s" % FWPath):
		if ".pyc" in f:
			os.remove(f)
	print "rm scripts/*.pyc"
	for f in os.listdir("%s/scripts" % FWPath):
		if ".pyc" in f:
			os.remove("scripts/%s" % (f))

	if(os.path.exists("build")):
		shutil.rmtree("build")

def cleanAll(args):
	cleanUser(args)
	cleanFW(args)

# List available analyzers ( in FW and User directories)	
def available(args):
	FWPath = getCheckVar("ANALYSISFW_PATH")
	UserPath = getCheckVar("ANALYSISFW_USERDIR")
	
	print "FW Analyzers : "
	for dir in listDirClean("%s/Analyzers" % FWPath):
		if os.path.exists("%s/Analyzers/%s/include" % (FWPath,dir)):
			for el in listFileClean("%s/Analyzers/%s/include" % (FWPath,dir)):
				print "\t%s" % el.replace(".hh", "")
	print "Examples Analyzers : "
	for el in listFileClean("%s/Examples/include" % FWPath):
		print "\t%s" % el.replace(".hh", "")
	print "User Analyzers : "
	for el in listFileClean("%s/Analyzers/include" % UserPath):
		print "\t%s" % el.replace(".hh", "")

# Create new analyzer
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
	readAndReplace("%s/Templates/templateAnalyzer.cc" % FWPath, "%s/Analyzers/src/%s.cc" % (UserPath, name), {'templateAnalyzer':name, '/*$$TREEREQUEST$$*/':inputs[0], '/*$$GETEVENTS$$*/':inputs[1]})

#def createAnalyzer(name, FWPath, UserPath):
def createAnalyzer(args):
	[name] = args.AnalyzerName
	FWPath = getCheckVar("ANALYSISFW_PATH")
	UserPath = getCheckVar("ANALYSISFW_USERDIR")
	
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
			print "\033[31;1mError while creating new analyzer %s: input type not specified for %s. Please add 'MC' or 'Reco' identifier before tree name.\033[0m" % (anName, i)
			return
		[type, tree] = i.split(" ")
		if type=="MC":
			className = "T%sEvent" % (tree)
		else:
			className = "TReco%sEvent" % (tree)
		
		#treeInclude = "%s#include \"%s.hh\"\n" % (treeInclude,className)
		treeRequest = "%s\tRequestTree(\"%s\",new %s);\n" % (treeRequest,tree,className)
		getEvents = "%s\t%s *%sEvent = (%s*)GetEvent(\"%s\");\n" % (getEvents,className,tree,className,tree)
	
	generateNewAnalyzer(anName, FWPath, UserPath, [treeRequest,getEvents])
	
# Rename a user analyzer
def renameAnalyzer(args):
	FWPath = getCheckVar("ANALYSISFW_PATH")
	UserPath = getCheckVar("ANALYSISFW_USERDIR")
	
	[oldName] = args.oldName
	[newName] = args.newName
	
	if not os.path.exists("%s/Analyzers/include/%s.hh" % (UserPath, oldName)):
		print "\033[31;1mThis analyzer does not exist (%s). It cannot be renamed.\033[0m" % oldName
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
	
	readAndReplace("%s/Analyzers/include/%s.hh" % (UserPath, oldName), "%s/Analyzers/include/%s.hh" % (UserPath, newName), {oldName:newName, oldName.upper():newName.upper()}, skipComments=False)
	readAndReplace("%s/Analyzers/src/%s.cc" % (UserPath, oldName), "%s/Analyzers/src/%s.cc" % (UserPath, newName), {oldName:newName}, skipComments=False)
	
	os.remove("%s/Analyzers/include/%s.hh" % (UserPath, oldName))
	os.remove("%s/Analyzers/src/%s.cc" % (UserPath, oldName))

# Clone a user analyzer
def cloneAnalyzer(args):
	FWPath = getCheckVar("ANALYSISFW_PATH")
	UserPath = getCheckVar("ANALYSISFW_USERDIR")
	
	[origin] = args.origin
	[dest] = args.dest
	
	if not os.path.exists("%s/Analyzers/include/%s.hh" % (UserPath, origin)):
		print "\033[31;1mThis analyzer does not exist (%s). It cannot be cloned.\033[0m" % origin
		return

	if os.path.exists("%s/Analyzers/include/%s.hh" % (UserPath, dest)):
		answer = raw_input("This analyzer (%) already exists. Do you want to overwrite it [Y/N] ? " % dest)
		if answer.lower() == 'y':
			os.remove("%s/Analyzers/include/%s.hh" % (UserPath, dest))
			os.remove("%s/Analyzers/src/%s.cc" % (UserPath, dest))
		else:
			print "Please choose a different name."
			return
	if os.path.exists("%s/Analyzers/include/%s.hh" % (FWPath, dest)):
		print "This analyzer already exists. Please choose a different name."
		return
	if os.path.exists("%s/Examples/include/%s.hh" % (FWPath, dest)):
		print "This analyzer already exists. Please choose a different name."
		return
	
	readAndReplace("%s/Analyzers/include/%s.hh" % (UserPath, origin), "%s/Analyzers/include/%s.hh" % (UserPath, dest), {origin:dest, origin.upper():dest.upper()}, skipComments=False)
	readAndReplace("%s/Analyzers/src/%s.cc" % (UserPath, origin), "%s/Analyzers/src/%s.cc" % (UserPath, dest), {origin:dest}, skipComments=False)
	
# Build the framework against the provided config file
def build(args):
	[filename] = args.configFileName
	FWPath = getCheckVar("ANALYSISFW_PATH")
	UserPath = getCheckVar("ANALYSISFW_USERDIR")
	userStrictOrder = args.strict_order
	
	if not os.path.exists(filename):
		print "The config file %s does not exist" % filename
		return
	
	cp = SimpleConfigParser.SimpleConfigParser()
	if is_binary(filename):
		print "\033[31;1mError reading the configuration file. It seems to be a binary file.\033[0m"
		return
	cp.read(filename)

	noAnalyzer = False
	noExecutable = False
	if not cp.hasoption('analyzers'):
		noAnalyzer = True
	else:
		analyzersList = cp.getoption("analyzers")
		
	if not cp.hasoption('exec'):
		noExecutable = True
	else:
		executable = cp.getoption("exec")

	if len(analyzersList) == 0:
		noAnalyzer = True
	if len(executable) == 0:
		noExecutable = True
	
	if noAnalyzer:
		print "\033[31;1mNo analyzers found in config file\033[0m"
	if noExecutable:
		print "\033[31;1mNo executable name in config file\033[0m"
	
	if noAnalyzer or noExecutable:
		return
	
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
	
	#create list of analyzers
	analyzers = [x.strip() for x in re.findall(" ?(.+?(?:\(.+?\)|[ ]|$)) ?", analyzersList)]
	usrAnList = ""
	fwAnList = ""
	exAnList = ""
	subFolder = ""
	prefixList = {}
	ordered = []
	missing = False
	
	for anDef in analyzers:
		an = parseAnalyzerDef(anDef)[0]
		[anType,subFolder] = checkAnalyzerExists(an, FWPath, UserPath)
		if anType==0:
			answer = raw_input("Analyzer %s does not exists. Do you want to create it [Y/N]" % (an))
			if answer.lower()=="y":
				createAnalyzer(anDef, FWPath, UserPath)
			else:
				missing = True
		else:
			if anType==1:
				prefix = "%s/Analyzers/%s" % (FWPath,subFolder)
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
					print "\033[33;1mWARNING: Missing dependence %s for %s. Adding it to the list of analyzers\033[0m" % (d, an)
					analyzers.append(d)
					#missing = True
	
	if missing == True:
		return
	
	if not userStrictOrder:
		p = depsGraph.getNextPath()
		while len(p)>0:
			if p[0]==-1:
				exit(0)
			for node in p:
				ordered.append(node)
			p = depsGraph.getNextPath()
	else:
		print """\033[33;1mWARNING: Using strict analyzer order. Dependencies between analyzers are not automatically resolved...
Re-run without --strict-order if you want dependencies to be automatically solved.\033[0m"""
		ordered = analyzers[:]
	
	strExtralibs = ""
	strExtralibs_CMake = ""
	for lib in extralibs:
		strExtralibs += "-l%s " % (lib)
		strExtralibs_CMake += "%s " % (lib)
	
	strExtralibsdirs = ""
	strExtralibsdirs_CMake = ""
	for dir in extralibsdirs:
		strExtralibsdirs += "-L%s " % (dir)
		strExtralibsdirs_CMake += "%s " % (dir)

	strExtraIncdirs = ""
	strExtraIncdirs_CMake = ""
	for dir in extraincludedirs:
		strExtraIncdirs += "-I%s " % (dir)
		strExtraIncdirs_CMake += "%s " % (dir)
	
	makefileDict = {"$$FWPath$$":FWPath, "$$USERCODEPATH$$":UserPath, "$$ANALYZERSHH$$":fwAnList, "$$USRHH$$":usrAnList, "$$EXAMPLEHH$$":exAnList, "$$EXEC$$": executable, "$$EXTRALIBS$$": strExtralibs_CMake, "$$EXTRALIBSDIRS$$": strExtralibsdirs_CMake, "$$EXTRAINCLUDEDIRS$$": strExtraIncdirs_CMake}
	readAndReplace("%s/Templates/analyzers.cmake" % FWPath, "%s/analyzers.cmake" % UserPath, makefileDict)
	
	includesList = ""
	instancesAnalyzer = ""
	deleteAnalyzer = ""
	error = False
	for an in ordered:
		includesList += """#include "%s.hh"\n""" % an;
		instancesAnalyzer += "\t%s *an_%s = new %s(ban);\n" % (an,an,an)
		instancesAnalyzer += "\tban->AddAnalyzer(an_%s);\n" % an
		deleteAnalyzer += "\tdelete an_%s;\n" % an
	
	readAndReplace("%s/Templates/main.cc" % FWPath, "%s/main.cc" % UserPath, {"$$ANALYZERSINCLUDE$$":includesList, "/*$$ANALYZERSNEW$$*/":instancesAnalyzer, "/*$$ANALYZERSDELETE$$*/":deleteAnalyzer})
	
	#Check if FW needs to be recompiled
	if not error:
		buildFW(FWPath,args.defines, args.jobs)
		buildUser(args.defines, args.jobs)

# Prepare user folder
def prepareUserFolder(args):
	[path] = args.UserDirectory
	path = path.rstrip("/")
	path = os.path.abspath(path)
	FWPath = getVar("ANALYSISFW_PATH", os.getcwd())
	NA62MCSOURCE = getCheckVar("NA62MCSOURCE")
	
	if not os.path.exists(path):
		os.makedirs(path)	
	else:
		if len(os.listdir(path))>0:
			print (
				"""The destination path is not empty. If you continue, the folder structure will be checked and eventually updated.
				The env.(c)sh and config files will be regenerated."""
			)
			answer = raw_input("Are you sure you want to continue [Y/N]?")
			if answer.lower() != "y":
				return
				
	
	buildFW(FWPath,args.defines, args.jobs)
	
	if not os.path.exists("%s/Analyzers" % path):
		os.mkdir("%s/Analyzers" % path)
	if not os.path.exists("%s/Analyzers/include" % path):
		os.mkdir("%s/Analyzers/include" % path)
	if not os.path.exists("%s/Analyzers/src" % path):
		os.mkdir("%s/Analyzers/src" % path)
	if not os.path.exists("%s/PhysicsObjects" % path):
		os.mkdir("%s/PhysicsObjects" % path)
	if not os.path.exists("%s/PhysicsObjects/include" % path):
		os.mkdir("%s/PhysicsObjects/include" % path)
	if not os.path.exists("%s/PhysicsObjects/src" % path):
		os.mkdir("%s/PhysicsObjects/src" % path)
	if not os.path.exists("%s/obj" % path):
		os.mkdir("%s/obj" % path)
	if not os.path.exists("%s/lib" % path):
		os.mkdir("%s/lib" % path)
	if not os.path.exists("%s/scripts" % path):
		os.mkdir("%s/scripts" % path)
	if not os.path.exists("%s/NA62.root" % path):
		shutil.copyfile("%s/NA62.root" % FWPath, "%s/NA62.root" % path)
	
	readAndReplace("%s/Templates/env.sh" % FWPath, "%s/scripts/env.sh" % path, {"$$ANALYSISFW$$":FWPath, "$$USERDIR$$":path, "$$NA62MCSOURCE$$":NA62MCSOURCE})
	readAndReplace("%s/Templates/env.csh" % FWPath, "%s/scripts/env.csh" % path, {"$$ANALYSISFW$$":FWPath, "$$USERDIR$$":path, "$$NA62MCSOURCE$$":NA62MCSOURCE})
	readAndReplace("%s/Templates/config" % FWPath, "%s/config" % path, {})
	shutil.copyfile("%s/Templates/CMakeLists.txt" % FWPath, "%s/CMakeLists.txt" % path)
	shutil.copyfile("%s/Templates/CMakeLists_PO.txt" % FWPath, "%s/PhysicsObjects/CMakeLists.txt" % path)
	updateSettings(path, FWPath)
	writeUserVersion(path)
	
	print "\nYour new user directory has been created. \nTo continue, go in %s, edit your config file, verify and source env.(c)sh, and run \nNA62AnalysisBuilder.py config" % path

# Build example analyzers and copy config files in user dir
def buildExample(args):
	FWPath = getCheckVar("ANALYSISFW_PATH")
	UserPath = getCheckVar("ANALYSISFW_USERDIR")
	
	shell = getVar("FWSHELL", "sh")
	shutil.copyfile("%s/Examples/examplePi0Config" % FWPath, "%s/examplePi0Config" % UserPath)
	shutil.copyfile("%s/Examples/exampleSkimmingConfig" % FWPath, "%s/exampleSkimmingConfig" % UserPath)
	readAndReplace("%s/Examples/exampleExportTreesConfig" % FWPath, "%s/exampleExportTreesConfig" % UserPath, {"$$FWPATH$$":FWPath})

def printHelp(args):
	args.parserRef.print_help()
	
# Command line argument parser
def parseArgs():
	global __rev__, __descr__
	
	program_version_message = "rev %s." % __rev__
	program_short_description = __descr__
	
	# Prepend the build command if the first positional argument is a file path (for compatibility with previous syntax)
	# Plus lowercase the first positional argument (supposed to be the command then)
	if len(sys.argv)>=1:
		try:
			index,positional = next((i,x) for (i,x) in enumerate(sys.argv[1:]) if not x.startswith('-'))
		except StopIteration:
			pass
		else:
			if os.path.exists(positional):
				if os.path.isfile(positional):
					sys.argv.insert(1, 'build')
					index = 0
			sys.argv[index+1] = sys.argv[index+1].lower()
	
	# Setup argument parser
	common_flags = ArgumentParser(add_help=False)
	common_flags.add_argument('-j', '--jobs', default=1, type=int, help="Number of processes to use for building (same as make -j)")
	common_flags.add_argument('--strict-order', default=False, action="store_true", help="""Use the analyzer in the same order as defined  
								in the configuration file (beware of dependencies between analyzers)""")
	clean_group = common_flags.add_argument_group(title="Build options", 
												description=("""The following options require a cleanAll to take effect 
															if the framework was already compiled without the option"""))
	clean_group.add_argument('-d', '--debug', action="append_const", const="NA62_DEBUG", 
							dest="defines", help="Compile the framework and user directories with debugging informations")
	clean_group.add_argument('--no-c++11', action="store_false", default="True", 
							dest="c11", help="""Compile the framework and user directories without c++11 support 
							(automatically disabled if compiler does not support c++11)""")
	clean_group.add_argument('--full-warning', action="append_const", const="FULL_WARNING", 
							dest="defines", help="Compile the framework and user directories with all the warning flags")
	clean_group.add_argument('--shared', action="append_const", const="SHARED_LIB", 
							dest="defines", help="Use shared libraries rather than static libraries")
	
	parser = ArgumentParser(description=program_short_description, formatter_class=RawDescriptionHelpFormatter)
	parser.add_argument('-V', '--version', action='version', version=program_version_message)
	subparsers = parser.add_subparsers(metavar = "commandName", title="Available commands", 
									description="'NA62AnalysisBulder.py commandName -h' for help about specific command")
	
	''' Help command'''
	parser_help = subparsers.add_parser('help', help='Show this help message and exit', description='Show this help message and exit')
	parser_help.set_defaults(func=printHelp, parserRef=parser)

	''' Build command'''
	parser_build = subparsers.add_parser('build', help='Build the FW using the configuration file', parents=[common_flags],
										description="Build the FW using the configuration file configFileName")
	parser_build.set_defaults(func=build)
	parser_build.add_argument('configFileName', type=str, nargs=1, help="Path to the config file")
	
	''' Rename command'''
	parser_rename = subparsers.add_parser('rename', help='Rename a user analyzer',
										description="Rename a user analyzer")
	parser_rename.set_defaults(func=renameAnalyzer)
	parser_rename.add_argument('oldName', type=str, nargs=1, help="Old name of the analyzer")
	parser_rename.add_argument('newName', type=str, nargs=1, help="New name of the analyzer")

	''' Clone command'''
	parser_rename = subparsers.add_parser('clone', help='Clone a user analyzer with a new name',
										description="Clone a user analyzer with a new name")
	parser_rename.set_defaults(func=cloneAnalyzer)
	parser_rename.add_argument('origin', type=str, nargs=1, help="Origin analyzer")
	parser_rename.add_argument('dest', type=str, nargs=1, help="Destination analyzer")
	
	''' new command'''
	parser_new = subparsers.add_parser('new', help='Create a new analyzer in the user directory',
									description="""
   Create a new analyzer with name AnalyzerName in the user directory
   Alternatively AnalyzerName can be replaced by the following syntax: 
      AnalyzerName(InputType TreeName, ...)
   This will already include the correct persistency headers, request the input tree and 
   retrieve the input events.
   Example: Creating an analyzer named toto using as input the GigaTracker MonteCarlo and 
   LKr and Spectrometer RECO
      NA62AnalysisBuilder.py new "toto(MC GigaTracker, Reco LKr, Reco Spectrometer)""",
      								formatter_class=RawDescriptionHelpFormatter)
	parser_new.set_defaults(func=createAnalyzer)
	parser_new.add_argument('AnalyzerName', type=str, nargs=1, help="Name of the analyzer to create")

	''' prepare command'''
	parser_prepare = subparsers.add_parser('prepare', help='Prepare a new user directory at the specified path', parents=[common_flags],
										description="Prepare a user directory at the specified path")
	parser_prepare.set_defaults(func=prepareUserFolder)
	parser_prepare.add_argument('UserDirectory', type=str, nargs=1, help="Path to the user directory")

	''' cleanUser command'''
	parser_cleanUser = subparsers.add_parser('cleanuser', help='Remove all files generated by the build in the user directory',
										description="Remove all files generated by the build in the user directory")
	parser_cleanUser.set_defaults(func=cleanUser)

	''' cleanFW command'''
	parser_cleanFW = subparsers.add_parser('cleanfw', help='Remove all files generated by the build in the FW directory',
										description="Remove all files generated by the build in the FW directory")
	parser_cleanFW.set_defaults(func=cleanFW)

	''' cleanAll command'''
	parser_cleanAll = subparsers.add_parser('cleanall', help='Remove all files generated by the build in the User and FW directory',
										description="Remove all files generated by the build in the FW directory and user directory")
	parser_cleanAll.set_defaults(func=cleanAll)

	''' available command'''
	parser_available = subparsers.add_parser('available', help='Display the list of available analyzers',
										description="Print the list of available Analyzers")
	parser_available.set_defaults(func=available)
	
	''' examples command'''
	parser_examples = subparsers.add_parser('examples', help='Build the libraries for the examples and copy the config files into the user directory',
										description="Build the libraries for examples and import the configuration files in user directory")
	parser_examples.set_defaults(func=buildExample)

	# Process arguments
	args = parser.parse_args()
	
	if "c11" in args and args.c11:
		if args.defines==None:
			args.defines = []
		args.defines.append("C++11_COMPAT")
	
	args.func(args)

def main():
	if getVar("ANALYSISFW_USERDIR", -1)!=-1:
		checkUpdate();
	
	parseArgs()
	sys.exit(0)

if __name__ == '__main__':
	main()
