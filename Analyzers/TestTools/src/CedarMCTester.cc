#include <stdlib.h>
#include <iostream>
#include <TChain.h>
#include "CedarMCTester.hh"
#include "MCSimple.hh"
#include "functions.hh"
#include "Event.hh"
#include "Persistency.hh"

#include "TH3D.h"
#include "TLorentzVector.h"
#include "TFile.h"
#include "TCedarHit.hh"
#include "TEllipse.h"
#include <numeric>


/// \class CedarMCTester
/// \Brief 
/// Describe your Analyzer
/// \EndBrief
///
/// \Detailed
/// Describe your Analyzer
/// \EndDetailed

CedarMCTester::CedarMCTester(BaseAnalysis *ba) : Analyzer(ba)
{
    /// \MemberDescr
    /// \param ba : parent BaseAnalysis
    ///
    ///Specify the trees you want to use and the event class corresponding\n
    ///Don't try to load MCTruth tree (RUN_0 or Event). Use the MCTruthEvent in Process function instead. Problems when opening twice the same tree.\n
    ///Example with RecoEvent\n
    ///	RequestTree("GigaTracker", new TRecoGigaTrackerEvent);\n
    ///Example with MC Event\n
    ///	RequestTree("GigaTracker", new TGigaTrackerEvent);\n
    ///Example with generic tree\n
    ///	RequestTree<MyClass>("MyTree", "BranchName", "MyClass", new MyClass);\n
    ////\n
    ///Initialize DetectorAcceptance if needed\n
    ///use of global instance\n
    ///	fDetectorAcceptanceInstance = GetDetectorAcceptanceInstance();\n
    ///use of local instance\n
    ///	fDetectorAcceptanceInstance = new DetectorAcceptance("./NA62.root");

    fAnalyzerName = "CedarMCTester";
    RequestTree("Cedar",new TCedarEvent);

    fDivider = "";

    pi = std::atan2( 0, -1 );
    hc = 1.239e-03; //nm Ev
    fAngularSeparation = 2 * pi  / 8; //radians
    fKaonRate = 45e6; //Hz

    fZCedarOriginLab   =   69278; //mm
    fZDiaphragmCentre  =    1251; //mm

    fZLightGuideCentre     = 701; //mm
    fLightGuideInnerRadius = 285; //mm

    fZQuartzWindowStart       = 851; //mm
    fZQuartzWindowLength      =  10; //mm
    fQuartzWindowRadialOffset = 103; //mm

    fZSphericalMirrorCapCentreNominal =  701; //mm
    fSphericalMirrorDiameter          =   50; //mm
    fSphericalMirrorCentralAngle      = pi/3; //radians;
    fSphericalMirrorCapRadialOffset   = 106;  //mm
}

void CedarMCTester::InitOutput(){
    /// \MemberDescr
    /// Register the output variables of the analyzer.\n
    /// Call\n
    /// 	RegisterOutput("outputName", &variableName)\n
    /// for each variable that should be in the output of the Analyzer\n
    /// The name of the analyzer will be prepended to the outputName (to avoid collisions with other analyzers)\n
    /// variableName should be the name of a variable declared in the definition of the class\n
    /// \n
    /// Call\n
    /// 	AddParam("paramName", "paramType", &variableName, defaultValue);\n
    /// for each parameter of the analyzer. These parameters can be set when starting the FW from the command line with the -p option.\n
    /// paramName is the name of the parameter in the command line\n
    /// paramType is the type of the parameter (allowed : bool, int, long, float, double, char, string, TString)\n
    /// variableName is the name of the variable that should be declared in the definition of the class\n
    /// defaultValue is the default value if not specified in the command line\n
    /// \n
    /// To create a new TTree in the output file, call\n
    /// 	void OpenNewTree("TTreeName", "TTreeTitle");\n
    /// TTreeName is the name of the TTree (will be used to refer to this TTree later)\n
    /// TTreeTitle is the title of the TTree\n
    /// Call\n
    /// 	void AddBranch<VariableType>("TTreeName", "BranchName", &pointer);\n
    /// VariableType is the type of the variable for this branch (fundamental data type or class)\n
    /// TTreeName is the name of the TTree to add this branch\n
    /// BranchName is the name of the branch\n
    /// pointer is a pointer to the variable (should be declared in the header file)\n
    /// \n
    /// To create a standard TTree containing KineParts (for candidates) use\n
    ///     CreateStandardTree("TTreeName", "TTreeTitle");
    /// \EndMemberDescr
}

void CedarMCTester::InitHist(){
    /// \MemberDescr
    /// Book and Initialize histograms in this function.\n
    /// Same function to Book TH1, TH2, TGraph and TGraphAsymmErrors (anything derived from TH1 or TGraph)\n
    ///  BookHisto(histogram*)\n
    /// If isAutotUpdate is true, this histogram will be drawn and updated regularly during the processing. (default=false)
    /// The refresh interval can be set with (default=10)
    ///  SetUpdateInterval(interval)
    /// Defining plots as AutoUpdate and setting the interval can also be done at runtime with a configuration file
    /// Example\n
    /// 	BookHisto(new TH2I("PartEnergy", "Energy as a function of particle", 0, 0, 0, Bins, MinEnergy, MaxEnergy));\n
    /// \n
    /// Booking of counters and creation of EventFraction can be done here\n
    ///	BookCounter(name)\n
    ///	NewEventFraction(name)\n
    /// Example\n
    /// 	BookCounter("Total");\n
    /// 	BookCounter("PassCuts");\n
    /// 	NewEventFraction("Cuts");\n
    /// \n
    /// Add the counters to the EventFraction\n
    /// 	AddCounterToEventFraction(EventFractionName, CounterName)\n
    /// Example\n
    /// 	AddCounterToEventFraction("Cuts", "Total");\n
    /// 	AddCounterToEventFraction("Cuts", "PassCuts");\n
    /// \n
    /// Then define which counter represents the sample size\n
    /// 	DefineSampleSizeCounter(EventFractionName, CounterName)\n
    /// Example\n
    /// 	DefineSampleSizeCounter("Cuts", "Total");\n
    /// \n
    /// You can retrieve histograms from the input ROOT file (Anything derived from TH1) with\n
    ///  RequestHistogram("TDirectoryName", "HistogramName", appendOnNewFile);\n
    /// appendOnNewFile is a boolean. If set to true, each time a new file is opened the content\n
    /// of the histogram will be appended to the content of the previous one. If set to false, the content \n
    /// of the histogram is replaced each time a new file is opened.
    /// \EndMemberDescr

    bool refresh = false;

    BookCounter("NKaons");

    //XRAY 

    //All end positions
    fhAllPositions = new TH3D( "EndPoints", "All particle end points",
            200, 69000, 95000,
            200, -1000, 1000,
            200, -1000, 1000
            );

    fhAllPositions->GetXaxis()->SetTitle( "Z (mm)" );
    fhAllPositions->GetYaxis()->SetTitle( "X (mm)" );
    fhAllPositions->GetZaxis()->SetTitle( "Y (mm)" );

    BookHisto( "EndPoints", fhAllPositions, refresh, "Geantino" );

    //----------

    //RZ projections
    TH2D * fhRZPositions = new TH2D( "RZEndPoints", "All particle end points",
            200, 69000, 95000, 200, 0, 1000  );

    fhRZPositions->GetXaxis()->SetTitle( "Z (mm)" );
    fhRZPositions->GetYaxis()->SetTitle( "r (mm)" );

    BookHisto( "RZEndPoints", fhRZPositions, refresh, "Geantino" );

    //----------

    //KTAG end positions
    fhKTAGPositions = new TH3D( "KTAGEndPoints", "All particle end points",
            200, 69750, 70150,
            200, -500, 500,
            200, -500, 500
            );

    fhKTAGPositions->GetXaxis()->SetTitle( "Z (mm)" );
    fhKTAGPositions->GetYaxis()->SetTitle( "X (mm)" );
    fhKTAGPositions->GetZaxis()->SetTitle( "Y (mm)" );

    BookHisto( "KTAGEndPoints", fhKTAGPositions, refresh, "Geantino" );

    //------------------------------------------------------------

    //STANDARD

    //pmt rates
    TH1D * hrate = new TH1D ( "Rate", "PMT rate" , 80, 9.5, 89.5 );
    hrate->GetXaxis()->SetTitle( "Position ID" );
    hrate->GetYaxis()->SetTitle( "Rate ( Hz )" );

    BookHisto( "PhotonsRateAverage",
            static_cast<TH1*>( hrate->Clone("RateAverage") ),
            refresh, "Photons/Standard" ) ;

    BookHisto( "PhotoelectronsRateAverage",
            static_cast<TH1*>( hrate->Clone("RateAverage") ),
            refresh, "Photoelectrons/Standard" ) ;

    BookHistoArray( "PhotonsRate", static_cast<TH1*>( hrate->Clone("Rate") ),
            8, refresh, "Photons/Standard" );

    BookHistoArray( "PhotoelectronsRate", static_cast<TH1*>( hrate->Clone("Rate") ),
            8, refresh, "Photoelectrons/Standard" );

    delete hrate;

    BookCounter("NKaons");

    //----------

    //Photon counts
    TH1D * hcount = new TH1D ( "NPhotons", "Number of photons",
            100, -0.5, 99.5 );

    hcount->GetXaxis()->SetTitle( "Number of photons" );
    hcount->GetYaxis()->SetTitle( "Number of events" );

    TH1D * htotalcount = new TH1D( "NPhotonsTotal", "Total number of photons" ,
            500, -0.5, 499.5  );

    htotalcount->GetXaxis()->SetTitle( "Number of photons" );
    htotalcount->GetYaxis()->SetTitle( "Number of events" );

    BookHisto( "PhotonsNPhotonsTotal",
            static_cast<TH1D*>( htotalcount->Clone( "NPhotonsTotal") ),
            refresh, "Photons/Standard");

    BookHisto( "PhotoelectronsNPhotonsTotal",
            static_cast<TH1D*>( htotalcount->Clone( "NPhotonsTotal") ),
            refresh, "Photoelectrons/Standard");

    BookHistoArray( "PhotonsNPhotons", 
            static_cast<TH1D*>(  hcount->Clone("NPhotons") ),
            8, refresh, "Photons/Standard" );

    BookHistoArray( "PhotoelectronsNPhotons", 
            static_cast<TH1D*>(  hcount->Clone("NPhotons") ),
            8, refresh, "Photoelectrons/Standard" );

    delete hcount;
    delete htotalcount;

    //------------------------------------------------------------

    //RAYTRACING

    //ZRProfile
    TH2D * hZRProfile = new TH2D( "ZRProfile" , "Z  and r coordinate of all hits" ,
            1000, 500, 1500, 100, 0, 400 );

    hZRProfile->GetXaxis()->SetTitle( "Z ( mm ) " );
    hZRProfile->GetYaxis()->SetTitle( "r ( mm ) " );

    BookHisto( "PhotonsZRProfile",
            static_cast<TH2D*>( hZRProfile->Clone("ZRProfile") ),
            refresh, "Photons/RayTracing" );

    BookHisto( "PhotoelectronsZRProfile",
            static_cast<TH2D*>( hZRProfile->Clone("ZRProfile") ),
            refresh, "Photoelectrons/RayTracing" );

    delete hZRProfile;

    //Diaphragm
    TH2D * hDiaphragm = new TH2D( "Diaphragm", 
            "Radius and wavelength and diaphragm",
            100, 97, 105, 100, 150, 750);

    hDiaphragm->GetXaxis()->SetTitle( "Radius (mm)" );
    hDiaphragm->GetYaxis()->SetTitle( "Wavelength (nm)" );

    BookHisto( "PhotonsDiaphragm", 
            static_cast<TH2D*>(  hDiaphragm ->Clone( "Diaphragm" ) ),
            refresh, "Photons/RayTracing" );

    BookHisto( "PhotoelectronsDiaphragm", 
            static_cast<TH2D*>(  hDiaphragm ->Clone( "Diaphragm" ) ),
            refresh, "Photoelectrons/RayTracing" );

    delete hDiaphragm;

    //Window
    TH2D * hWindow = new TH2D( "Window", " Photon positions at quartz window",
            100, -25, 25, 100, -25, 25);
    hWindow->GetXaxis()->SetTitle( "X (mm)" );
    hWindow->GetYaxis()->SetTitle( "Y (mm)" );

    BookHistoArray( "PhotonsWindow",
            static_cast<TH2D*>( hWindow ->Clone("Window" )),
            8,  refresh, "Photons/RayTracing")  ;

    BookHistoArray( "PhotoelectronsWindow",
            static_cast<TH2D*>( hWindow ->Clone("Window" )),
            8,  refresh, "Photoelectrons/RayTracing")  ;

    BookHisto( "PhotonsWindowSum", static_cast<TH2*>( hWindow->Clone("WindowSum")) ,
            refresh, "Photons/RayTracing")  ;

    BookHisto( "PhotoelectronsWindowSum", static_cast<TH2*>( hWindow->Clone("WindowSum")) ,
            refresh, "Photoelectrons/RayTracing")  ;

    delete hWindow;

    //Mirror
    TH2D * hMirror = new TH2D( "Mirror",
            " Photon positions at spherical mirror",
            100, -16, 16, 100, -3, 2 );

    hMirror->GetXaxis()->SetTitle( "U (mm)" );
    hMirror->GetYaxis()->SetTitle( "V (mm)" );

    BookHisto( "PhotonsMirrorSum", 
            static_cast<TH2*>( hMirror->Clone("MirrorSum") ),
            refresh, "Photons/RayTracing" );

    BookHisto( "PhotoelectronsMirrorSum", 
            static_cast<TH2*>( hMirror->Clone("MirrorSum") ),
            refresh, "Photoelectrons/RayTracing" );

    BookHistoArray("PhotonsMirror", 
            static_cast<TH2*>( hMirror->Clone("Mirror") ),
            8 , refresh, "Photons/RayTracing" );

    BookHistoArray("PhotoelectronsMirror", 
            static_cast<TH2*>( hMirror->Clone("Mirror") ),
            8 , refresh, "Photoelectrons/RayTracing" );

    delete hMirror;

    //Lightguide
    TH2D * hLightGuide = new TH2D("LightGuide", 
            "Photon positions at the light guide",
            100, -100, 100, 100, -100, 100);

    hLightGuide->GetXaxis()->SetTitle( "Z (mm)" );
    hLightGuide->GetYaxis()->SetTitle( "Y (mm)" );

    BookHisto( "PhotonsLightGuideSum", 
            static_cast<TH2*>( hLightGuide->Clone("LightGuideSum") ),
            refresh, "Photons/RayTracing" ) ;

    BookHisto( "PhotoelectronsLightGuideSum", 
            static_cast<TH2*>( hLightGuide->Clone("LightGuideSum") ),
            refresh, "Photoelectrons/RayTracing" ) ;

    BookHistoArray( "PhotonsLightGuide",
            static_cast<TH2*>( hLightGuide->Clone("LightGuide") ),
            8 , refresh, "Photons/RayTracing" );

    BookHistoArray( "PhotoelectronsLightGuide",
            static_cast<TH2*>( hLightGuide->Clone("LightGuide") ),
            8 , refresh, "Photoelectrons/RayTracing" );
}

void CedarMCTester::DefineMCSimple(MCSimple *fMCSimple){
    /// \MemberDescr
    /// \param fMCSimple : MCSimple
    ///
    /// Setup of fMCSimple. You must specify the generated MC particles you want.\n
    /// Add particles you want to recover from fMCSimple\n
    /// int particleID = fMCSimple->AddParticle(parentID, pdgCode)\n
    /// parentID : 	0=no parent (=beam particle)\n
    /// 	...\n
    /// Example : you want to retrieve the kaon from the beam, the pi0 an pi+ from the beam kaon and the 2 photons coming from the previous pi0 decay :\n
    /// 	int kaonID = fMCSimple->AddParticle(0, 321) //Ask beam kaon (sequence ID=1)\n
    /// 	fMCSimple->AddParticle(kaonID, 211) //Ask pi+ from previous kaon (sequence ID=2)\n
    /// 	int pi0ID = fMCSimple->AddParticle(kaonID, 111) //Ask pi0 from previous kaon (sequence ID=3)\n
    /// 	fMCSimple->AddParticle(pi0ID, 22) //Ask first gamma from previous pi0 (sequence ID=4)\n
    /// 	fMCSimple->AddParticle(pi0ID, 22) //Ask second gamma from previous pi0 (sequence ID=4)
    ///
    /// @see ROOT TDatabasePDG for a list of PDG codes and particle naming convention
    /// \EndMemberDescr
}

void CedarMCTester::StartOfRunUser(){
    /// \MemberDescr
    /// This method is called at the beginning of the processing (corresponding to a start of run in the normal NA62 data taking)\n
    /// Do here your start of run processing if any
    /// \EndMemberDescr
}

void CedarMCTester::StartOfBurstUser(){
    /// \MemberDescr
    /// This method is called when a new file is opened in the ROOT TChain (corresponding to a start/end of burst in the normal NA62 data taking) + at the beginning of the first file\n
    /// Do here your start/end of burst processing if any
    /// \EndMemberDescr
}

void CedarMCTester::EndOfBurstUser(){
    /// \MemberDescr
    /// This method is called when a new file is opened in the ROOT TChain (corresponding to a start/end of burst in the normal NA62 data taking) + at the end of the last file\n
    /// Do here your start/end of burst processing if any
    /// \EndMemberDescr
}

void CedarMCTester::Process(int iEvent){
    /// \MemberDescr
    /// \param iEvent : Event number
    /// \param fMCSimple : MCSimple
    /// \param MCTruthEvent : Complete set of MC particles
    ///
    /// Main process method. Called on each event. Write you analysis here.\n
    /// Ask the fMCSimple to have the complete set of particles we specified\n
    /// You can retrieve MC particles from the fMCSimple Set with (return a vector<KinePart*>\n
    ///  fMCSimple["particleName"]\n
    ///  fMCSimple[pdgID]\n
    /// Example\n
    /// 	fMCSimple["K+"][index] for the kaon\n
    /// 	fMCSimple["pi+"][index] for the positive pion\n
    /// 	fMCSimple["gamma"][index] for the photon\n
    /// The number in the brackets is the index of the particle (if you asked for two photons in the set, you can ask fMCSimple["gamma"][0] for the first one and fMCSimple["gamma"][1] for the second)\n
    /// \n
    /// If you need a property of a particle, you can make a call to fParticleInterface (instance of the ParticleInterface class).\n
    ///	This class has two methods FindParticle that will return a TParticlePDG with the required particle. You can search by pdgID or by name.\n
    ///	This class also provide two methods to switch between particle name and pdgID if necessary.\n
    ///	Example\n
    ///		double kaonMass = fParticleInterface->FindParticle(321).Mass();
    ///		double pi0Lifetime = fParticleInterface->FindParticle("pi0").Lifetime();
    /// You can retrieve the events from the trees with\n
    /// 	(eventClass*)GetEvent("treeName");\n
    /// You can retrieve data from generic TTrees with\n
    /// 	GetObject<MyClass>("treeName");\n
    /// \n
    /// You can retrieve the histograms you booked (for drawing, changing, filling, ...) with\n
    /// 	fHisto.GetTH1("histoName") for TH1\n
    /// 	fHisto.GetTH2("histoName") for TH2\n
    /// 	fHisto.GetGraph("graphName") for TGraph and TGraphAsymmErrors\n
    /// To fill the histograms you can use\n
    /// 	FillHisto("histoName", values)\n
    /// where values are the same parameters as if you call histogram->Fill(values) (x,y,weight,...)\n
    /// If the histogram is not found, an error message is printed\n
    /// \n
    /// Modify a counter with one of the following methods\n
    /// 	IncrementCounter(name)\n
    /// 	IncrementCounter(name, delta)\n
    /// 	DecrementCounter(name)\n
    /// 	DecrementCounter(name, delta)\n
    /// 	SetCounterValue(name, value)\n
    /// \n
    /// For use of fGeom, read DetectorAcceptance class.\n
    ///	WARNING: this class provides "exact" results, there is not tolerance. If the particle\n
    ///	passes in the sensitive volume of a detector it's considered as detected, wether it's close\n
    ///	to the edge or not. But as the class gives you the position of the passage point and the \n
    ///	estimated path length in the sensitive volume, you can apply further cuts from this\n
    ///	information at your convenience.
    /// \n
    /// To use the output of a different analyzer, use\n
    /// 	outputType *var = GetOutput<outputType>("analyzerName.outputName", state);\n
    /// Where outputType is the variable type and state is of type outputState\n
    /// This function returns a void* and you have to explicitly cast it to the real object type\n
    /// State is set with the state of the variable (kOUninit, kOInvalid ,kOValid). The value of the output should only be trusted if state == kOValid\n
    /// example : TLorentzVector vertex = *(TLorentzVector*)GetOutput("simpleVertexAnalyzer.vertex", state);\n
    /// \n
    /// Before starting the processing of an event, the state flag of each output variable is reset to kOUninit\n
    /// When setting the value of an output variable, don't forget to set appropriately the state flag to either kOValid or kOInvalid\n
    /// to indicate if the value can/can't be used in other analyzer\n
    /// 	SetOutputState("outputName", kOValid);\n
    /// \n
    /// If you want to append a candidate in one of your standard output Tree, use\n
    /// 	KinePart *candidate = CreateStandardCandidate("treeName");\n
    /// and fill the properties of your candidate. It will be automatically written in the output tree.\n
    ///	\n
    /// If you want to save this event in your custom and standard TTrees (not the input tree replication), call\n
    /// 	FillTrees();\n
    /// This will call the Fill method of every TTree created in this analyzer.\n
    ///	\n
    /// If you want to replicate this event in the output file, call\n
    /// 	ExportEvent();\n
    /// The structure of all the trees that have been opened (by all Analyzer) will be copied in the output file\n
    /// and the events for which at least one analyzer called ExportEvent() will be replicated in the output trees.
    /// @see ROOT TParticlePDG for the particle properties
    /// @see ROOT TDatabasePDG for a list of PDG codes and particle naming convention
    /// \EndMemberDescr
    //if(fMCSimple.fStatus == MCSimple::kMissing){printIncompleteMCWarning(iEvent);return;}
    //if(fMCSimple.fStatus == MCSimple::kEmpty){printNoMCWarning();return;}

    IncrementCounter( "NKaons");

    //XRAY - GEANTINO

    //Save EndPosition of every MC particle
    Event* MCTruthEvent;
    if ( GetWithMC() )
    {
    	MCTruthEvent = GetMCEvent();
        unsigned int nPart = MCTruthEvent->GetNKineParts();
        TClonesArray*  ParticleArray = MCTruthEvent->GetKineParts();

        for ( unsigned int ihit = 0 ; ihit != nPart ; ++ihit )
        {
            TLorentzVector EndPosition = 
                static_cast<KinePart*>( (*ParticleArray)[ihit] )
                ->GetEndPos();

            fhAllPositions->Fill( EndPosition.Z(),
                    EndPosition.X(), EndPosition.Y() );

            fhKTAGPositions->Fill( EndPosition.Z(),
                    EndPosition.X(), EndPosition.Y() );

            FillHisto( "RZEndPoints", EndPosition.Z(), EndPosition.Perp() );
        }
    }

    //--------------------------------------------------

    //Extract event and hits
    TCedarEvent *CedarEvent = (TCedarEvent*)GetEvent("Cedar");
    int nHits = CedarEvent->GetNHits();
    const TClonesArray&  hits = *( CedarEvent->GetHits() );

    std::vector<int> lightguide_photons( 8, 0 );
    std::vector<int> lightguide_photoelectrons( 8, 0 );

    //Loop over hits
    for ( int iHit = 0 ; iHit != nHits ; ++ iHit )
    {
        //Extract hit
        TCedarHit * hit = static_cast<TCedarHit*>( hits[iHit] );

        //Apply PMT quantum efficiency
        const int PMTTYPE = 2;
        double energy = hit->GetEnergy();
        double wavelength = hc / energy;
        double efficiency = CedarTools::QE(wavelength, PMTTYPE );
        bool photoelectron =  ( gRandom->Rndm() < efficiency ) ;

        //Determine octant
        TVector3 position = hit->GetPosition();
        double phi = position.Phi();
        double clock_angle = phi - pi / 2;

        clock_angle = clock_angle < 0 ? clock_angle + 2 * pi : clock_angle;
        int octant = static_cast<int>
            ( std::floor( clock_angle / fAngularSeparation ) );

        double rotation =  ( 1.5 - octant  )  * fAngularSeparation;

        //STANDARD
        FillHisto("PhotonsRateAverage", hit->GetChannelID()%100 );
        FillHistoArray( "PhotonsRate", octant, hit->GetChannelID()%100 );

        if ( photoelectron )
        {
            FillHisto("PhotoelectronsRateAverage", hit->GetChannelID()%100 );
            FillHistoArray( "PhotoelectronsRate", octant, hit->GetChannelID()%100 );
        }

        //CATEGORIZE HIT

        double radius = position.Perp();
        double zHit = position.Z();
        double zHitCedar = zHit - fZCedarOriginLab;

        FillHisto ( "PhotonsZRProfile", zHitCedar, radius );

        if ( photoelectron )
        {FillHisto ( "PhotoelectronsZRProfile", zHitCedar, radius );}

        //Hit in LightGuide or cones or PMTs

        if ( radius > fLightGuideInnerRadius )
        {
            position.RotateZ( rotation );
            double fZDisplacement = fZCedarOriginLab + fZLightGuideCentre;

            lightguide_photons[octant]++;

            FillHistoArray( "PhotonsLightGuide", octant,
                    position.Y(), position.Z() - fZDisplacement );

            FillHisto("PhotonsLightGuideSum", position.Y(),
                    position.Z() - fZDisplacement );

            if ( photoelectron )
            { 
                lightguide_photoelectrons[octant]++;

                FillHistoArray( "PhotoelectronsLightGuide", octant,
                        position.Y(), position.Z() - fZDisplacement );

                FillHisto("PhotoelectronsLightGuideSum", position.Y(),
                        position.Z() - fZDisplacement );
            }
        }
        ///Hit in Spherical Mirror
        else if ( zHitCedar < fZSphericalMirrorCapCentreNominal + fSphericalMirrorDiameter )
        {

            //Azimuthal rotation
            position.RotateZ( rotation );

            double fZDisplacement = fZCedarOriginLab + fZSphericalMirrorCapCentreNominal;
            position -= TVector3(  -fSphericalMirrorCapRadialOffset, 0, fZDisplacement );

            //Mirror rotation
            position.RotateY( fSphericalMirrorCentralAngle );

            FillHistoArray( "PhotonsMirror", octant, position.Y(), position.Z() );
            FillHisto( "PhotonsMirrorSum", position.Y(), position.Z() );

            if ( photoelectron )
            {
                FillHistoArray( "PhotoelectronsMirror", octant, position.Y(), position.Z() );
                FillHisto( "PhotoelectronsMirrorSum", position.Y(), position.Z() );
            }
        }
        //Hit in quartz window
        else if ( zHitCedar < fZQuartzWindowStart + fZQuartzWindowLength + 20 )
        {
            position.RotateZ( rotation );
            position += TVector3( fQuartzWindowRadialOffset, 0, 0 );
            position.RotateZ( - pi / 2 );

            FillHistoArray( "PhotonsWindow", octant, position.X(), position.Y() );
            FillHisto( "PhotonsWindowSum", position.X(), position.Y() );

            if ( photoelectron )
            {
                FillHistoArray( "PhotoelectronsWindow", octant, position.X(), position.Y() );
                FillHisto( "PhotoelectronsWindowSum", position.X(), position.Y() );
            }
        }

        //Hit in diaphragm
        else if ( zHitCedar < fZDiaphragmCentre + 50 )
        {
            double energy = hit->GetEnergy();
            double wavelength = hc / energy;
            FillHisto( "PhotonsDiaphragm", radius, wavelength );
            if ( photoelectron )
            {
                FillHisto( "PhotoelectronsDiaphragm", radius, wavelength );
            }
        }
    }

    //Fill photon counts
    for ( int octant = 0 ; octant != 8 ; ++octant )
    {
        FillHistoArray("PhotonsNPhotons", octant, 
                lightguide_photons[octant]);

        FillHistoArray("PhotoelectronsNPhotons", octant,
                lightguide_photoelectrons[octant]);
    }

    FillHisto( "PhotonsNPhotonsTotal",
            std::accumulate( lightguide_photons.begin(), 
                lightguide_photons.end(), 0 ) );

    FillHisto( "PhotoelectronsNPhotonsTotal",
            std::accumulate( lightguide_photoelectrons.begin(),
                lightguide_photoelectrons.end(), 0 ) );
}

void CedarMCTester::PostProcess(){
    /// \MemberDescr
    /// This function is called after an event has been processed by all analyzers. It could be used to free some memory allocated
    /// during the Process.
    /// \EndMemberDescr

}

void CedarMCTester::EndOfRunUser(){
    /// \MemberDescr
    /// This method is called at the end of the processing (corresponding to a end of run in the normal NA62 data taking\n)
    /// Do here your end of run processing if any
    /// \EndMemberDescr

}

void CedarMCTester::ExportPlot(){
    /// \MemberDescr
    /// This method is called at the end of processing to save plots.\n
    /// If you want to save them all, just call\n
    /// 	SaveAllPlots();\n
    /// Or you can just save the ones you want with\n
    /// 	histogram->Write()\n
    /// \EndMemberDescr

    int nKaons = static_cast<double>( GetCounterValue("NKaons") );

    //Normalize rate plots
    TH1 * hPhotonsRateAverage = fHisto.GetTH1("PhotonsRateAverage");
    hPhotonsRateAverage->Scale( fKaonRate / 8.0 / nKaons );

    TH1 * hPhotoelectronsRateAverage = fHisto.GetTH1("PhotoelectronsRateAverage");
    hPhotoelectronsRateAverage->Scale( fKaonRate / 8.0 / nKaons );

    for ( unsigned int i = 0 ; i != 8 ; ++ i )
    {
        TH1 * hPhotonsRateI = 
            fHisto.GetTH1(  Form("PhotonsRate%s%d",fDivider.c_str(), i) );
        hPhotonsRateI->Scale( fKaonRate / nKaons );

        TH1 * hPhotoelectronsRateI = 
            fHisto.GetTH1(  Form("PhotoelectronsRate%s%d",fDivider.c_str(), i) );
        hPhotoelectronsRateI->Scale( fKaonRate / nKaons );
    }

    //----------

    SaveAllPlots();

    //----------

    //LIGHTGUIDE CANVAS
    TCanvas hclg( "LightGuideCanvas", "Light Guide");
    TCanvas hclgpe( "LightGuideCanvas", "Light Guide");

    TH1 * hlg = fHisto.GetTH2( "PhotonsLightGuideSum" );
    hlg->SetContour(100);
    hclg.cd();
    hlg->Draw("colz");

    TH1 * hlgpe = fHisto.GetTH2( "PhotoelectronsLightGuideSum" );
    hlgpe->SetContour(100);
    hclgpe.cd();
    hlgpe->Draw("colz");

    //Create PMT positions
    int fNofRows = 8;
    std::vector<int> fNofCones( fNofRows, 0 );
    fNofCones[0]  = 5;
    fNofCones[1]  = 8;
    fNofCones[2]  = 9;
    fNofCones[3]  = 8;
    fNofCones[4]  = 9;
    fNofCones[5]  = 8;
    fNofCones[6]  = 9;
    fNofCones[7]  = 8;

    double fConesPhiShift    =  3.654*2*pi / 360 ;
    double fConesThetaShift  =  3.224*2*pi / 360 ;

    double rConeCentre = 302.0;
    double rPmt = 4;

    //Loop over PMT positions
    for (int iRow=0; iRow<fNofRows; iRow++) {

        double theta  = (iRow-0.5*(fNofRows-1))*fConesThetaShift;

        for (int iCone=0; iCone<fNofCones[iRow]; iCone++) {

            double phi = -(iCone-0.5*(fNofCones[iRow]-1))*fConesPhiShift;

            double y = rConeCentre*sin(phi);
            double z =  -rConeCentre*sin( theta );

            TEllipse * te = new TEllipse( y, z, rPmt);
            te->SetLineWidth(2);
            te->SetFillStyle( 0 );
            hclg.cd();
            te->Draw("same");
            hclgpe.cd();
            te->Draw("same");
        }
    }
    gFile->cd( GetAnalyzerName() + "/Photons/RayTracing" );
    hclg.Write("LightGuideCanvas");

    gFile->cd( GetAnalyzerName() + "/Photoelectrons/RayTracing" );
    hclgpe.Write("LightGuideCanvas");

}

void CedarMCTester::DrawPlot(){
    /// \MemberDescr
    /// This method is called at the end of processing to draw plots.
    /// If you want to draw all the plots, just call
    /// 	DrawAllPlots();
    /// Or do as usual (TCanvas, Draw, ...)
    /// \EndMemberDescr
}

namespace CedarTools
{

    Double_t QE (Double_t wavelength, Int_t PMType) {
        if (PMType==1) return QE_EMI_9820_QB (wavelength);
        if (PMType==2) return QE_R7400U_03   (wavelength);
        if (PMType==3) return QE_R9880U_110  (wavelength);
        if (PMType==4) return QE_R9880U_210  (wavelength);
        return 0;
    }


    // Hamamatsu R7400U-03 quantum efficiency
    // Parameterized by Evgueni, July 2011
    Double_t QE_R7400U_03 (Double_t wavelength) {
        double x = wavelength;
        Double_t par[10] =
        {-58.41145755814, 1.450540667766, -0.01561331198442,
            9.545010080831e-05, -3.648461145542e-07, 9.047599515597e-10,
            -1.457151808585e-12, 1.471328774241e-15, -8.46121819724e-19,
            2.11384701372e-22};

        if (x<180) return 0;
        if (x>660) return 0;
        Double_t qe = 0;
        for (int i=0; i<10; i++) qe += par[i]*TMath::Power(x,i);
        if (qe<0) qe = 0;
        return qe;
    }


    // Hamamatsu R9880U-110 quantum efficiency
    // Parameterized by Angela, July 2011
    Double_t QE_R9880U_110 (Double_t wavelength) {
        double x = wavelength;
        Double_t par[15] =
        {1489.053765000671, -20.61340505642701, 0.09607362916193821,
            -0.000144918944048782, -1.087924475686453e-07, 3.619104979507752e-10,
            2.742092765095943e-13, -1.067200613381487e-15, 6.333980140159196e-19,
            4.675391577876988, 505.1903283978535, 15.37334879108591,
            -23.08738129086531, 358.7521218115685, 53.63424346389683};

        Double_t x1 = (x<650) ? x : 650;
        Double_t qe = 0;
        for (int i=0; i<9; i++) qe += par[i]*TMath::Power(x1,i);
        qe += par[9]*TMath::Gaus(x1, par[10], par[11]);
        qe += par[12]*TMath::Gaus(x1, par[13], par[14]);
        qe *= 0.01;
        if (x>650) qe *= (1 - (x-650)/(675-650));
        if (qe<0 || x<200) qe = 0;
        return qe;
    }


    // Hamamatsu R9880U-210 quantum efficiency
    // Parameterized by Evgueni, March 2013
    Double_t QE_R9880U_210 (Double_t wavelength) {
        double x = wavelength;
        double par[9] = 
        {277.3385690654, -5.360192324445, 0.04415739632667,
            -0.0002031054657811, 5.721437395991e-07, -1.012602804374e-09,
            1.100802213492e-12, -6.72600529683e-16, 1.769806940956e-19};

        Double_t x1 = (x<680) ? ((x>240) ? x : 240) : 680;
        Double_t qe = 0;
        for (int i=0; i<9; i++) qe += par[i]*TMath::Power(x1,i);
        if (x>680) qe *= (1 - (x-680)/(700-680));
        if (x<240) qe *= (1 - (240-x)/(240-225));
        if (qe<0)  qe = 0;
        return qe;
    }


    /** Original Cedar PMT quantum efficiency **/

    // 1) Lau's parameterization
    Double_t QE_EMI_9820_QB_Lau (Double_t wavelength) {
        double wl = wavelength;
        Double_t qe = 0.25 - TMath::Power((wl-400)/500., 2);
        if (qe<0) qe = 0;
        return qe;
    }

    // 2) Francis's parameterization following the data sheet (March 2013)
    Double_t QE_EMI_9820_QB (Double_t wavelength) {
        double wlraw = wavelength;

        Double_t wl = (wlraw < 141.0 ) ? 141.0 : (wlraw > 649.0) ? 649.0 : wlraw;

        Double_t wls[24] =
        {140, 160, 180, 200, 220, 240, 260, 280, 300, 320,
            340, 360, 380, 400, 420, 440, 460, 480, 500, 520,
            540, 560, 600, 650};
        Double_t qes[24] =
        {0, 20, 21.6, 21.6, 21.4, 21.4, 22, 24, 25, 25.6,
            26, 26, 26.4, 26, 24.8, 23.2, 20.8, 18, 15.2, 12,
            8, 5.6, 2, 0};

        unsigned int i = 0;
        while (true) {
            if (wl > wls[i] && wl < wls[i+1])
            { break; }
            else
            { ++i; }
        }
        return 0.01 * (qes[i] + ( wl - wls[i] )
                / ( wls[i+1] - wls[i] ) * ( qes[i+1] - qes[i] ) );
    }

}
