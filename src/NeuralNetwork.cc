#include <iostream>
#include "NeuralNetwork.hh"
#include <TTree.h>
#include <TMultiLayerPerceptron.h>
#include <TMLPAnalyzer.h>
#include <TCanvas.h>
using namespace std;

NeuralNetwork::NeuralNetwork() {
	/// \MemberDescr
	/// Experimental feature. Not completely developed.
	/// Constructor
	/// \EndMemberDescr

	fTrainingTree = new TTree("fTrainingTree", "Tree for NN training");
	fTrainingTree->Branch("NNOutputResult", &fNNOutputResult, "NNOutputResult/I");

	fCreated = false;
	fNumberBckg = 0;
	fNumberSignal = 0;
}

NeuralNetwork::~NeuralNetwork() {
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr

}

void NeuralNetwork::AddLayer(int neurons){
	/// \MemberDescr
	/// \param neurons : Number of neurons in the new layer
	///
	/// Add a layer of neurons at the end.
	/// \EndMemberDescr

	fLayers.push_back(neurons);
}

void NeuralNetwork::AddInput(TString name, void* address, TString otherName){
	/// \MemberDescr
	/// \param name : name of the variable
	/// \param address : pointer to the input variable
	/// \param otherName : name again. Requested by NN, I don't know why
	///
	/// Add new input variable to the Neural Network
	/// \EndMemberDescr

	fInputsName.push_back(name);
	fInputs.push_back(address);

	fTrainingTree->Branch(name, address, otherName);
}

void NeuralNetwork::FillSignal(){
	/// \MemberDescr
	/// Fill the NN with a signal event (for training)
	/// \EndMemberDescr

	fNNOutputResult = 1;
	fTrainingTree->Fill();
	fNumberSignal++;
}

void NeuralNetwork::FillBackground(){
	/// \MemberDescr
	/// Fill the NN with a background event (for training)
	/// \EndMemberDescr

	fNNOutputResult = 0;
	fTrainingTree->Fill();
	fNumberBckg++;
}

void NeuralNetwork::Create(){
	/// \MemberDescr
	/// Create the NN from the info given before
	/// \EndMemberDescr

	TString opt = "";
	TString weights = "";

	if(fCreated) return;

	for(unsigned int i=0; i<fInputsName.size(); i++){
		opt += "@";
		opt += fInputsName[i];
		if(i<(fInputsName.size()-1)) opt += ",";
	}
	for(unsigned int i=0; i<fLayers.size(); i++){
		opt += ":";
		opt += fLayers[i];
	}

	opt += ":NNOutputResult";

	weights = "(NNOutputResult==0)/";
	weights += fNumberBckg;
	weights += " + (NNOutputResult==1)/";
	weights += fNumberSignal;
	fMlp = new TMultiLayerPerceptron(opt, weights, fTrainingTree, "Entry$%2", "(Entry$+1)%2");
	fCreated = true;
}

void NeuralNetwork::Train(int epoch){
	/// \MemberDescr
	/// \param epoch : Number of epoch to train the NN
	///
	/// Train the NN with the previously provided Signal and Background events
	/// \EndMemberDescr

	Create();

	if(fVerbosity>=AnalysisFW::kNormal) cout << "Training NN with " << fNumberSignal << " signal and " << fNumberBckg << " background samples" << endl;
	fMlp->Train(epoch, "text,graph,update=10");

	TCanvas *c1 = new TCanvas("c1", "Network Analysis");
	c1->Divide(2,2);

	TMLPAnalyzer ana(fMlp);

	ana.GatherInformations();
	ana.CheckNetwork();
	c1->cd(1);
	ana.DrawDInputs();
	c1->cd(2);
	fMlp->Draw();
	c1->cd(3);
	ana.DrawNetwork(0, "NNOutputResult==1", "NNOutputResult==0");
}

double NeuralNetwork::Evaluate(){
	/// \MemberDescr
	/// Evaluate the output of the previously trained or loaded NN on the current event
	/// \EndMemberDescr

	double params[fInputs.size()];

	for(unsigned int i=0; i<fInputs.size(); i++){
		params[i] = *(double*)fInputs[i];
	}

	return fMlp->Evaluate(0, params);
}

void NeuralNetwork::Save(TString name){
	/// \MemberDescr
	/// \param name : path to the text file where the NN will be saved
	///
	/// Save the network in a text file
	/// \EndMemberDescr

	fMlp->DumpWeights(name);
}

void NeuralNetwork::Load(TString name){
	/// \MemberDescr
	/// \param name : path to the text file from which the NN will be loaded
	///
	/// Load the network from a text file
	/// \EndMemberDescr

	fMlp->LoadWeights(name);
}
