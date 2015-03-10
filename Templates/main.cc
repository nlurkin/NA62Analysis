#include <iostream>
#include <signal.h>

#include <TString.h>
#include <TApplication.h>

#include "BaseAnalysis.hh"

$$ANALYZERSINCLUDE$$
using namespace std;

BaseAnalysis *ban = 0;
TApplication *theApp = 0;

void usage(char* name)
{
	cout << "Usage: \t"<< name << " [-hga] < -i InputFile.root | -l InputFilesList.txt [-B #MaxFiles] > [-n #FirstEvent]" << endl;
	cout << "\t\t[-N #Events] [-o OutputFile.root] [-r ReferenceFile.root] [-v verbosity] [-c configFile]" << endl;
	cout << "[-p \"analyzerName:param=val;param=val&analyzerName:param=val&...\"]" << endl;
	cout << "\t -h : Display this help" << endl;
	cout << "\t -g : Graphical mode. Starts a ROOT application for display. Do not automatically exit at the end of the processing, Ctrl-C to exit." << endl;
	cout << "\t -i : Path to an input ROOT file." << endl;
	cout << "\t -l : Path to a text file containing a list of paths to input ROOT files. One file per line." << endl;
	cout << "\t -B : Maximum number of files to process from the list." << endl;
	cout << "\t -n : Index of the first event to process. Event index starts at 0. The n first events will be skipped." << endl;
	cout << "\t -N : Maximum number of events to process." << endl;
	cout << "\t -o : Path to output ROOT file. Will be overwritten if already exists." << endl;
	cout << "\t -r : Path to a file containing reference plots." << endl;
	cout << "\t -v : Verbosity level." << endl;
	cout << "\t -p : List of parameters to pass to analyzers." << endl;
	cout << "\t -c : Path to a configuration file containing analyzers parameters." << endl;
	cout << "\t -a : Allow non-existing trees." << endl;
}

void sighandler(int sig)
{
	cerr << endl << "********************************************************************************" << endl;
	cerr << "Killed with Signal " << sig << endl;
	cerr << endl << "********************************************************************************" << endl;
	cerr << "Bye!" << endl;

	delete ban;

	if(theApp) theApp->Terminate();

	exit(0);
}

int main(int argc, char** argv){
	// Setting signals
	signal(SIGXCPU, sighandler);
	signal(SIGTERM, sighandler);
	signal(SIGINT, sighandler);
	signal(SIGABRT, sighandler);

	TString inFileName;
	TString outFileName = "outFile.root";
	TString refFileName;
	TString params;
	TString configFile;
	TString argTS;

	int NEvt = 0;
	int evtNb = -1;
	Int_t NFiles = 0;
	bool graphicMode = false;
	bool fromList = false;
	bool allowNonExisting = false;
	AnalysisFW::VerbosityLevel verbosity = AnalysisFW::kNo;

	// Browsing arguments
	int opt;
	int n_options_read = 0;
	while ((opt = getopt(argc, argv, "hagB:n:i:l:N:o:v:p:c:r:")) != -1) {
		n_options_read++;
		switch (opt) {
		case 'B':
			NFiles = TString(optarg).Atoi();
			break;
		case 'i':
			//Input file
			inFileName = TString(optarg);
			break;
		case 'l':
			//Input files list
			if(!NFiles) NFiles = 1;
			inFileName = TString(optarg);
			fromList = true;
			break;
		case 'n':
			//First event to process
			NEvt = TString(optarg).Atoi();
			break;
		case 'N':
			//Number of events to process
			evtNb = TString(optarg).Atoi();
			break;
		case 'o':
			//Output file
			outFileName = TString(optarg);
			break;
		case 'v':
			//Verbosity
			argTS = TString(optarg);
			if(argTS.IsDec()) verbosity = (AnalysisFW::VerbosityLevel)argTS.Atoi();
			else{
				if(argTS.CompareTo("kNo", TString::kIgnoreCase)==0) verbosity = AnalysisFW::kNo;
				if(argTS.CompareTo("kUser", TString::kIgnoreCase)==0) verbosity = AnalysisFW::kUser;
				if(argTS.CompareTo("kNormal", TString::kIgnoreCase)==0) verbosity = AnalysisFW::kNormal;
				if(argTS.CompareTo("kDebug", TString::kIgnoreCase)==0) verbosity = AnalysisFW::kDebug;
			}
			break;
		case 'p':
			//Analyzer params
			params = TString(optarg);
			break;
		case 'c':
			//Config file to parse
			configFile = TString(optarg);
			break;
		case 'r':
			refFileName = TString(optarg);
			break;
		case 'g':
			graphicMode = true;
			break;
		case 'a':
			allowNonExisting = true;
			break;
		default: /* '?' */
			usage(argv[0]);
			return 0;
		}
	}

	if (!n_options_read) {
		usage(argv[0]);
		return 0;
	}

	if(!fromList && NFiles>0){
		cerr << "Option -B can only be used with the -l parameter" << endl;
		return 0;
	}

	if(graphicMode) theApp = new TApplication("NA62Analysis", &argc, argv);


	ban = new BaseAnalysis();
	ban->SetVerbosity(verbosity);
	//DEF_ANALYZER is the ClassName of the analyzer. Defined by Makefile target
/*$$ANALYZERSNEW$$*/

	ban->Init(inFileName, outFileName, params, configFile, NFiles, graphicMode, refFileName, allowNonExisting);
	ban->Process(NEvt, evtNb);

	if(graphicMode) theApp->Run();

/*$$ANALYZERSDELETE$$*/
	delete ban;

	return 0;
}
