#include <getopt.h>
#include <iostream>
#include <signal.h>

#include <TString.h>
#include <TApplication.h>

#include "BaseAnalysis.hh"

$$ANALYZERSINCLUDE$$

NA62Analysis::Core::BaseAnalysis *ban = 0;
TApplication *theApp = 0;

void usage(char* name)
{
	cout << endl;
	cout << "Usage: \t"<< name << " (-i path | -l/--list path) [options]" << endl << endl;
	cout << "Allowed options:" << endl;
	cout << "  -h/--help\t\t: Display this help" << endl;
	cout << "  -v [level]\t\t: Verbosity level." << endl
		 << "\t\t\t  Possible values: kNo, kUser, kNormal, kExtended, kDebug or 0,1,2,3,4;" << endl
		 << "\t\t\t  Default=kNo; If level not specified: kNormal" << endl;
	cout << "  -g\t\t\t: Graphical mode. Starts a ROOT application for display." << endl
		 << "\t\t\t  Do not automatically exit at the end of the processing, Ctrl-C to exit." << endl;
	cout << "  -n/--nevt int\t\t: Maximum number of events to process." << endl;
	cout << "  -o/--output path\t: Path to output ROOT file. Will be overwritten if already exists." << endl;
	cout << "  -p/--params string\t: List of parameters to pass to analyzers." << endl
		 << "\t\t\t  The format of the string is " << endl
		 << "\t\t\t  \"analyzerName:param=val;param=val&analyzerName:param=val&...\"" << endl;
	cout << "  --histo\t\t: Read histograms only and bypass TTree reading." << endl;
	cout << "  --start int\t\t: Index of the first event to process." << endl
		 << "\t\t\t  Event index starts at 0." << endl;
	cout << "  --config path\t\t: Path to a configuration file containing analyzers parameters." << endl;
	cout << "  --reffile path\t: Path to a ROOT file containing reference plots." << endl;
	cout << "  --ignore\t\t: Ignore non-existing trees and continue processing." << endl;
	cout << endl;
	cout << "Mutually exclusive options groups:" << endl;
	cout << " Group1:" << endl;
	cout << "  -i path\t\t: Path to an input ROOT file." << endl;
	cout << " Group2:" << endl;
	cout << "  -l/--list path\t: Path to a text file containing a list of paths to input ROOT files." << endl
		 << "\t\t\t  One file per line." << endl;
	cout << "  -B/-b/--nfiles int\t: Maximum number of files to process from the list. (Default: All)" << endl
		 << "\t\t\t  !Warning. When using -g option, do not use the -b but -B or --nfiles." << endl;
	cout << endl << endl;
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
	using NA62Analysis::Verbosity::VerbosityLevel;

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
	bool ignoreNonExisting = false;
	VerbosityLevel verbosity = VerbosityLevel::kNo;
	bool readPlots = false;

	// Browsing arguments
	int opt;
	int n_options_read = 0;
	int flReadPlots = 0;
	int flIgnoreNonExisting = 0;

	struct option longopts[] = {
			{ "list",	required_argument,	NULL,	'l'},
			{ "nfiles",	required_argument,	NULL,	'B'},
			{ "nevt",	required_argument,	NULL,	'n'},
			{ "output",	required_argument,	NULL,	'o'},
			{ "params",	required_argument,	NULL,	'p'},
			{ "histo",	no_argument,		&flReadPlots,	1},
			{ "start",	required_argument,	NULL,		'0'},
			{ "config",	required_argument,	NULL,		'1'},
			{ "reffile",required_argument,	NULL,		'2'},
			{ "ignore",	no_argument,		&flIgnoreNonExisting,	1},
			{0,0,0,0}
	};

	while ((opt = getopt_long(argc, argv, "hi:v:gl:B:b:n:o:p:0:1:2:", longopts, NULL)) != -1) {
		n_options_read++;
		switch (opt) {
		// Short options only cases
		case 'i': /* Input file */
			inFileName = TString(optarg);
			break;
		case 'v':
			//Verbosity
			if(optarg){
				argTS = TString(optarg);
				if(argTS.IsDec()) verbosity = (VerbosityLevel)argTS.Atoi();
				else{
					if(argTS.CompareTo("kNo", TString::kIgnoreCase)==0) verbosity = VerbosityLevel::kNo;
					if(argTS.CompareTo("kUser", TString::kIgnoreCase)==0) verbosity = VerbosityLevel::kUser;
					if(argTS.CompareTo("kNormal", TString::kIgnoreCase)==0) verbosity = VerbosityLevel::kNormal;
					if(argTS.CompareTo("kExtended", TString::kIgnoreCase)==0) verbosity = VerbosityLevel::kExtended;
					if(argTS.CompareTo("kDebug", TString::kIgnoreCase)==0) verbosity = VerbosityLevel::kDebug;
				}
			}
			else verbosity = VerbosityLevel::kNormal;
			break;
		case 'g':
			graphicMode = true;
			break;

		// Mixed short-long options cases
		case 'l': /* Input files list, long_option: list */
			if(!NFiles) NFiles = -1;
			inFileName = TString(optarg);
			fromList = true;
			break;
		case 'B': /* Number of files to read, long_option: nfiles */
			NFiles = TString(optarg).Atoi();
			break;
		case 'b': /* Number of files to read, long_option: nfiles */
			NFiles = TString(optarg).Atoi();
			break;
		case 'n': /* Maximum number of events to process, long_option: nevt */
			evtNb = TString(optarg).Atoi();
			break;
		case 'o': /* Output file path, long_option: output */
			outFileName = TString(optarg);
			break;
		case 'p': /* Analyzer params, long_options: params */
			params = TString(optarg);
			break;

		// long options only cases
		case '0':	/* First event to process, long_option: start */
			NEvt = TString(optarg).Atoi();
			break;
		case '1': /* Config file to parse, long_option: config */
			configFile = TString(optarg);
			break;
		case '2': /* Reference file path, long_option: reffile */
			refFileName = TString(optarg);
			break;

		case 0: /* getopt_long() set a variable, continue */
			break;

		// Default (includes help)
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

	ignoreNonExisting = flIgnoreNonExisting;
	readPlots = flReadPlots;

	if(graphicMode) theApp = new TApplication("NA62Analysis", &argc, argv);


	ban = new NA62Analysis::Core::BaseAnalysis();
	ban->SetGlobalVerbosity(verbosity);
	ban->SetGraphicMode(graphicMode);
	if(readPlots) ban->SetReadType(NA62Analysis::Core::IOHandlerType::kHISTO);
	else ban->SetReadType(NA62Analysis::Core::IOHandlerType::kTREE);
	//DEF_ANALYZER is the ClassName of the analyzer. Defined by Makefile target
/*$$ANALYZERSNEW$$*/

	ban->Init(inFileName, outFileName, params, configFile, NFiles, refFileName, ignoreNonExisting);
	ban->Process(NEvt, evtNb);

	if(graphicMode) theApp->Run();

/*$$ANALYZERSDELETE$$*/
	delete ban;

	return 0;
}
