#include "../include/network.hpp"
#include "../include/math/linearalgebra.h"
#include "../include/math/numerical.h"

Network::Network(vector<int> &layerSizes)
{
    m_layerSizes = layerSizes;
    m_numLayers = layerSizes.size();

    // cout << "CREATING " << numLayers << " LAYERS..." << endl;
    for (int layerNum=0; layerNum<m_numLayers; ++layerNum)
    { 
        m_layers.push_back(Layer(layerSizes.at(layerNum))); // Then add it to this network's vector of layers.
    }
    // cout << m_layers.size() << " LAYERS CREATED." << endl;

    for (int layerNum=0; layerNum<m_numLayers-1; ++layerNum)
    {
        m_weightMatrices.push_back
        (
            linalg::Matrix<double>
            ( // Create a new weight matrix for each pair of adjacent layers.
            layerSizes.at(layerNum+1),  // Rows correspond to neurons in next layer,
            layerSizes.at(layerNum),    // columns to neurons in current layer.
            true                        // Initialize weights to random doubles.
            )
        );

        for (int neuronIndex=0; neuronIndex<m_layers.at(layerNum+1).getSize(); ++neuronIndex)
        { // randomly initialize neuron biases in the hidden and output layers
            m_layers.at(layerNum+1).setBiasAt(neuronIndex, double{numerical::randomDouble()});
        }
    }
}

void Network::setInput(vector<double> &input)
{
    /*
    Sets the inputs of the neurons in the 0th (input) layer.
    */

    m_input = input;

    for (int neuronIndex=0; neuronIndex<input.size(); ++neuronIndex)
    {
        m_layers.at(0).setInputAt(neuronIndex, input.at(neuronIndex));
    }
}

void Network::feedForward()
{
    /*
    Implements the feedforward algorithm.
    */

    for (int layerNum=0; layerNum<(m_layers.size()-1); ++layerNum) // for the input to penultimate layer
    {
        vector<double> currentLayerOutputs = m_layers.at(layerNum).getActivations();
        vector<double> nextLayerInputs { m_weightMatrices.at(layerNum) * currentLayerOutputs };

        for (int neuronNum=0; neuronNum<nextLayerInputs.size(); ++neuronNum)
        {// set the inputs of the next layer, with bias
            m_layers.at(layerNum+1).setInputAt(neuronNum, nextLayerInputs.at(neuronNum));
        }

    }
}

void Network::backPropagate()
{
    /*
    Implements backpropagation using quadratic cost function,
    with derivative (activation - target value).
    */

    // Compute the output error: equal to ( grad the vector of quadratic costs for
    // each neuron, Hadamard product the vector of derivatives of the output neurons ).
    vector<double> gradCost;
    vector<double> output {m_layers.back().getActivations()};
    vector<double> outputDerivatives {m_layers.back().getDerivatives()};
    
    for(int i=0; i<m_layers.back().getSize(); ++i)
    {
        gradCost.push_back(output.at(i) - m_targetOutput.at(i));
    }

    // Calculate output error
    // the index in m_errors goes in reverse order, from the output layer to the input layer
    // (this makes it easier to use of back() and push_back())
    m_errors.push_back(linalg::hadamardProduct(gradCost, outputDerivatives));

    cout << "ERRORS:";
    linalg::print(m_errors.at(0));

    // Backpropagate the error
    for(int i=m_numLayers-2; i>0; --i) // m_numLayers should be equal to m_weightMatrices.size()-1
    {
        linalg::Matrix<double> weights_T { m_weightMatrices.at(i).transpose() };
        vector<double> thisLayerDerivatives { m_layers.at(i).getDerivatives() };

        vector<double> backpropagatedError { weights_T * m_errors.back() };
        vector<double> thisLayerError {linalg::hadamardProduct(backpropagatedError, thisLayerDerivatives)};

        m_errors.push_back(thisLayerError);

        linalg::print(m_errors.back());
        cout << endl;
    };
}

void Network::update()
{
    /*
    Using the most recent error, updates the weight matrices
    and neuron biases.
    */

   for(int l=0; l<m_weightMatrices.size(); ++l)
   {
        linalg::Matrix<double> &currentWeightMatrix = m_weightMatrices.at(l);      // weight of connections from layer l to layer l+1
        vector<double> &currentError = m_errors.at(m_errors.size()-1-l); // layer indices in reverse order for m_errors (perhaps introduce sorting if not computationally costly?)

        // neuron indexing: i in layer l and j in layer l+1
        for(int j=0; j<m_layers.at(l+1).getSize(); ++j)
        {
            for(int i=0; i<m_layers.at(l).getSize(); ++i)
            {
                double newWeight { currentWeightMatrix(j,i)     // connection weight from neuron i to neuron j
                                 - m_LEARNINGRATE 
                                 * m_layers.at(l).getActivations().at(i) // activation of neuron i in layer l
                                 * m_errors.at(m_errors.size()-1-l).at(j) };     // error of neuron j in layer l+1
                
                currentWeightMatrix(j, i) = newWeight;
            }

            // update neuron j's bias
            double newBias { m_layers.at(l+1).getBiasAt(j) 
                           - m_LEARNINGRATE
                           * m_errors.at(m_errors.size()-1-l).at(j)};
            m_layers.at(l+1).setBiasAt(j, newBias);
        }
   }
}

void Network::train(vector<vector<vector<double>>> trainingData)
{
    /*
    Trains the network on a training set, given data in the appropriate format.
    */

    int trainingPass = 0;

    for (vector<vector<double>> trainingSample : trainingData) // for each training sample
    {
        /* Set inputs */
        vector<double> input {trainingSample.at(0)};
        setInput(input);

        /* Current target */
        vector<double> targetOutput {trainingSample.at(1)};
        setTarget(targetOutput);

        /* Feedforward */
        cout << "(PASS : " << trainingPass << ")" << endl;
        feedForward();
        printToConsole();
        for (double output : targetOutput)
        {
            cout << "\t(Target: " << output << ")" << endl << endl;
        }

        /* Backpropagate */
        m_errors.clear();  // Clear the error from any previous training loop
        backPropagate();

        /* Update weights */
        update();

        ++trainingPass;
    }
}

void Network::printToConsole() const
{
    /*
    Prints the input values of the neurons in the input layer,
    then the activations of the neurons in subsequent layers.
    */

    for (int layerIndex=0; layerIndex<m_numLayers; ++layerIndex) {

        if (layerIndex==0)
        {
            cout << "INPUT LAYER:";
            vector<double> layerVector{ m_layers.at(layerIndex).getInputs() };
            linalg::print(layerVector);
        } 
        else if (layerIndex==m_numLayers-1)
        {
            cout << "OUTPUT LAYER:";
            vector<double> layerVector{ m_layers.at(layerIndex).getActivations() };
            linalg::print(layerVector);   
        }
        else
        {
            cout << "LAYER " << layerIndex << ":";
            vector<double> layerVector{ m_layers.at(layerIndex).getActivations() };
            linalg::print(layerVector);
        }
    }
    cout << endl;
}