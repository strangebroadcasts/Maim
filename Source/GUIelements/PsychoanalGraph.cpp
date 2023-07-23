/*
  ==============================================================================

    PsychoanalGraph.cpp
    Created: 12 Apr 2023 3:55:31pm
    Author:  Arden Butterfield

  ==============================================================================
*/

#include "PsychoanalGraph.h"
#include "MaimLookAndFeel.h"

//==============================================================================
PsychoanalGraph::PsychoanalGraph(juce::AudioProcessorValueTreeState& p):
    parameters(p),
    threshold(0, 1, 22, MaimLookAndFeel().BEVEL_DARK),
    energy(0, 1, 22, MaimLookAndFeel().SPLASH_COLOR_DARK)
{
    parameters.state.addListener(this);
    
    juce::Component* c = this;
    while (c->getParentComponent()) {
        c = c->getParentComponent();
    }
    
    addAndMakeVisible(energy);
    addAndMakeVisible(threshold);
    
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

PsychoanalGraph::~PsychoanalGraph()
{
    parameters.state.removeListener(this);
}

void PsychoanalGraph::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged,
                                               const juce::Identifier &property)
{
    if (property == juce::Identifier("energy")) {
        juce::Array<juce::var>* e = treeWhosePropertyHasChanged[property].getArray();
        for (int i = 0; i < 22; ++i) {
            energyVals[i] = (*e)[i].operator double();
        }
    } else if (property == juce::Identifier("threshold")) {
        juce::Array<juce::var>* t = treeWhosePropertyHasChanged[property].getArray();
        for (int i = 0; i < 22; ++i) {
            thresholdVals[i] = (*t)[i].operator double();
        }
    }
    
    energy.loadData(energyVals.data());
    threshold.loadData(thresholdVals.data());
}

void PsychoanalGraph::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (MaimLookAndFeel().BEVEL_LIGHT);   // clear the background

}

void PsychoanalGraph::resized()
{
    energy.setBounds(getLocalBounds());
    threshold.setBounds(getLocalBounds());
}
