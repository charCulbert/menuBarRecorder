#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>

class Recorder
{
public:
    Recorder();
    ~Recorder();

    void startRecording(const juce::File& file, double sampleRate, int numChannels);
    void stopRecording();
    void process(const juce::AudioSourceChannelInfo& bufferToFill);

    bool isRecording() const { return writer != nullptr; }

private:
    std::unique_ptr<juce::FileOutputStream> fileStream;
    std::unique_ptr<juce::AudioFormatWriter> writer;
    juce::AudioFormatManager formatManager;
};
