#ifndef _NEURON_HPP_
#define _NEURON_HPP_

using namespace std;

class Neuron 
{
    /*
    Neuron class with the essential features required for a 
    feedforward neural network: an input value, an activation value,
    and the ability to get the derivative of its activation.
    */

    public:
        Neuron(const double input);        // Constructor for input layer neurons (with no bias)
        Neuron(double input, double bias); // Constructor for subsequent layer neurons (with random initial bias)

        void setInput(double input);    // Function to set the input.
        void setBias(double bias);      // Set the bias.
        void activate();                // Function to compute the activation.
        void derive();                  // Function to compute the derivative of the activation.

        double getInput() const      { return m_input; }      // Accessor for input value.
        double getActivation() const { return m_activation; } // Accessor for activation value.
        double getDerivative() const { return m_derivative; } // Accessor for the derivative of the activation.
        double getBias() const       { return m_bias; }       // Accessor for neuron's bias.

    private:
        double m_input;       // Input value
        double m_activation;  // Activation of the neuron.
        double m_derivative;  // Derivative of the neuron's input.
        double m_bias{0};   // neuron's bias
};

#endif