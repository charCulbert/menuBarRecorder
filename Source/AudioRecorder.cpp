#include "AudioRecorder.h"

Recorder::Recorder()
{
    formatManager.registerBasicFormats(); // Supports WAV, AIFF, etc.
}

Recorder::~Recorder()
{
    stopRecording();
}

void Recorder::startRecording(const juce::File& file, double sampleRate, int numChannels)
{
    stopRecording(); // Ensure no existing recording

    fileStream = file.createOutputStream();
    if (!fileStream)
        return;


    juce::WavAudioFormat wavFormat;
    writer.reset(wavFormat.createWriterFor(fileStream.get(), sampleRate, numChannels, 16, {}, 0));

    if (writer)
        fileStream.release(); // Hand over ownership to the writer
}

void Recorder::stopRecording()
{
    writer.reset(); // This closes the writer
    fileStream.reset();
}

void Recorder::process(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (writer)
        writer->writeFromAudioSampleBuffer(*bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples);
}
