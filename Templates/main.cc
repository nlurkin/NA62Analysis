#include <iostream>
#include <signal.h>

#include <TString.h>
#include <TApplication.h>

#include "BaseAnalysis.hh"

$$ANALYZERSINCLUDE$$
using namespace std;

void usage(char* name)
{
	cout << "Usage: "<< name << " [-hg] < -i InputFile.root | -l InputFilesList.txt > [-B #MaxFiles] [-n #FirstEvent] [-N #Events] [-o OutputFile.root] [-v verbosity] [-p \"analyzerName:param=val;param=val&analyzerName:param=val&...\"]" << endl;
}


//FIXME
void sighandler(int sig)
{
	if (sig == 138)
	{
	}
	else
	{
		cerr << endl << "********************************************************************************" << endl;
		cerr << "Killed with Signal " << sig << endl;
		cerr << endl << "********************************************************************************" << endl;
	}
	cerr << "Bye!" << endl;
	exit(0);
}

int main(int argc, char** argv){
	// Setting signals
	signal(SIGXCPU, sighandler);
	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);
	signal(138, sighandler);
	signal(127,sighandler);

	TString inFileName;
	TString outFileName = "outFile.root";
	TString params;
	TString configFile;
	TString argTS;

	int NEvt = 0;
	int evtNb = -1;
	Int_t NFiles = 0;
	bool graphicMode = false;
	AnalysisFW::VerbosityLevel verbosity = AnalysisFW::kNo;

	// Browsing arguments
	int opt;
	int n_options_read = 0;
	while ((opt = getopt(argc, argv, "hgB:n:i:l:N:o:v:p:c:")) != -1) {
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
		case 'g':
			graphicMode = true;
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

	TApplication *theApp = 0;
	if(graphicMode) theApp = new TApplication("RecoAnalysis", &argc, argv);


	BaseAnalysis *ban = new BaseAnalysis();
	ban->SetVerbosity(verbosity);
	//DEF_ANALYZER is the ClassName of the analyzer. Defined by Makefile target
	$$ANALYZERSNEW$$

	ban->Init(inFileName, outFileName, params, configFile, NFiles, graphicMode);
	ban->Process(NEvt, evtNb);

	if(graphicMode) theApp->Run();

	$$ANALYZERSDELETE$$
	delete ban;

	return 0;
}
