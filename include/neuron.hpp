#ifndef _NEURON_HPP_
#define _NEURON_HPP_

#include <iostream>
#include <math.h>
using namespace std;

class Neuron 
{
/*
Neuron class with the essential features required for a 
feedforward neural network: an input value, an activation value,
and the ability to get the derivative of its activation.
*/

    public:
        Neuron(double input);

        void setInput(double input);    // Function to set the input.
        void setBias(double bias);      // Set the bias.
        void activate();                // Function to compute the activation.
        void derive();                  // Function to compute the derivative of the activation.

        double getInput()      { return m_input; }      // Accessor for input value.
        double getActivation() { return m_activation; } // Accessor for activation value.
        double getDerivative() { return m_derivative; } // Accessor for the derrivative of the activation.

    private:
        double m_input;       // Input value
        double m_activation;  // Activation of the neuron.
        double m_derivative;  // Derivative of the neuron's activation.
        double m_bias{1.0};        // neuron's bias
};

#endif