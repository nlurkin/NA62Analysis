#ifndef NEURALNETWORK_HH_
#define NEURALNETWORK_HH_

#include <TMultiLayerPerceptron.h>
#include <TMLPAnalyzer.h>
#include "FWEnums.hh"
using namespace std;

/// \class NeuralNetwork
/// \Brief 
/// Experimental feature. Wrapper for a neural network
/// \EndBrief 
///
/// \Detailed
/// Experimental feature. Wrapper for a neural network
/// \EndDetailed

class NeuralNetwork {
public:
	NeuralNetwork();
	~NeuralNetwork();

	//Methods to define the NN
	void AddLayer(int neurons);
	//void ModifyLayer(int id, int neurons);
	void AddInput(TString name, void* address, TString otherName);

	//Methods to fill the NN for training
	void FillSignal();
	void FillBackground();

	//Methods to use the NN
	void Train(int epoch);
	double Evaluate();

	//Save and load NN
	void Save(TString name);
	void Load(TString name);

private:
	void Create();

	//Containers for the components of the NN
	vector<TString> fInputsName; ///< Container for the names of the input variables
	vector<void*> fInputs; ///< Container for the pointers to the input variables
	vector<int> fLayers; ///< Definition of the NN structure
	int fNNOutputResult; ///< Store the output of the NN

	int fNumberBckg; ///< Number of background entries
	int fNumberSignal; ///< Number of signal entries

	TMultiLayerPerceptron *fMlp; ///< NN
	TTree *fTrainingTree; ///< Tree containing the training set

	bool fCreated; ///< Does the network already exists

	AnalysisFW::VerbosityLevel fVerbosity;
};

#endif /* NEURALNETWORK_HH_ */
