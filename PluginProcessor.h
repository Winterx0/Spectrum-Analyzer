/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================

template<typename T>
struct Fifo
{
    bool push(const T& itemToAdd)
    {
        auto write = fifo.write(1);
        if(write.blockSize1 <= 1)
        {
            buffer[write.startIndex1] = itemToAdd;
            return true;
        }
        return false;
        
    }
    bool pull(T& itemToUpdate)
    {
        auto read = fifo.read(1);
        if(read.blockSize1 <= 1)
        {
            itemToUpdate = buffer[read.startIndex1];
            return true;
        }
        return false;
        
    }
    
private:
    static constexpr int Capacity = 5;
    std::array<T, Capacity> buffer;
    AbstractFifo fifo{Capacity};
    
    
};
//==============================================================================

struct VariableSizedBuffer
{
    void prepare(int size)
    {
        buffer.setSize(1, size);
        buffer.clear();
        prepared = true;
    }

    void clone(const dsp::AudioBlock<float>& other);
    void clone(const AudioBuffer<float>& other);
    void clone(const VariableSizedBuffer& other);
    
    AudioBuffer<float>& getBuffer() {return buffer;}
    size_t getNumSamples() const {return numSamples;}
    
    
private:
    AudioBuffer<float> buffer;
    size_t numSamples{0};
    bool prepared = false;
    template<typename BufferType>
    void clear(const BufferType& other)
    {
        jassert(prepared);
        jassert(other.getNumSamples() <= buffer.getNumSamples());
        buffer.clear();
    }
    void clear(const VariableSizedBuffer& other)
    {
        jassert(prepared);
        jassert(other.buffer.getNumSamples() <= buffer.getNumSamples());
        buffer.clear();
    }
};

 
//==============================================================================
enum FFTSizes
{
  fftOrder = 11,
  fftSize = 1 << fftOrder,
  numPoints = 512
};
//==============================================================================
struct FFTDataFifo
{
    using BufferType = std::array<float, 2 * FFTSizes::fftSize>;
    bool push(const BufferType& bufferToClone)
     {
         auto write = fifo.write(1);
         if(write.blockSize1 <= 1)
         {
             buffer[write.startIndex1] = bufferToClone;
             return true;
         }
         return false;
     }
     bool pull(BufferType& bufferToFill)
     {
         auto read = fifo.read(1);
         if(read.blockSize1 <= 1)
         {
             auto& buffers = buffer[read.startIndex1];
             return true;
         }
         return false;
     }
    
    
private:
    static constexpr int Capacity = 5;
      std::array<BufferType, Capacity> buffer;
      AbstractFifo fifo{Capacity};
};
//==============================================================================

struct PathFifo
{
    
        bool push(const Path& pathToClone)
         {
             auto write = fifo.write(1);
             if(write.blockSize1 <= 1)
             {
                 buffer[write.startIndex1] = pathToClone;
                 return true;
             }
             return false;
         }
         bool pull(Path& pathToFill)
         {
             auto read = fifo.read(1);
             if(read.blockSize1 <= 1)
             {
                 pathToFill = buffer[read.startIndex1];
                 return true;
             }
             return false;
         }
        
        
    private:
        static constexpr int Capacity = 30;
          std::array<Path, Capacity> buffer;
          AbstractFifo fifo{Capacity};
};
 

//=============================================================================
struct BufferAnalyzer2 : Thread, Timer, Component
{
    BufferAnalyzer2();
    ~BufferAnalyzer2();
    void prepare(double sampleRate, int samplesPerBlock);
    void cloneBuffer(const dsp::AudioBlock<float>& other);
    void run() override;
    void timerCallback() override;
    void paint(Graphics& g) override;
    
private:
    std::array<AudioBuffer<float>, 2> buffer;
    Atomic<bool> firstBuffer {true};
    std::array<Atomic<size_t>, 2> samplesCopied;

//==================

    float fifoBuffer [FFTSizes::fftSize];
    float fftData [2 * FFTSizes::fftSize];
    int fifoIndex = 0;
    
    void pushNextSampleIntoFifo (float sample);
    
    Atomic<bool> nextFFTBlockReady{false};
    float curveData [FFTSizes::numPoints];
    
    dsp::FFT forwardFFT{FFTSizes::fftOrder};
    dsp::WindowingFunction<float> window{FFTSizes::fftSize, dsp::WindowingFunction<float>::hann};
    
    void drawNextFrameOfSpectrum();
    Path fftCurve;

};


//==============================================================================
/**
*/
class Pfmproject0AudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    Pfmproject0AudioProcessor();
    ~Pfmproject0AudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    AudioParameterBool* shouldPlaySound = nullptr; // = false;
    
    AudioParameterFloat* byColor = nullptr;
    
    static void UpdateAutomatableParameter(RangedAudioParameter*, float value);
    

    BufferAnalyzer2 leftBufferAnalyzer, rightBufferAnalyzer;
    
private:
    AudioProcessorValueTreeState apvts;
    Random r;

    


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmproject0AudioProcessor)
};
