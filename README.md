# MenuBarRecorder

A simple macOS menu bar app that records system audio. Click the tray icon, hit record, and capture whatever's playing on your Mac.

It temporarily switches your system audio output to a virtual loopback device, records the audio flowing through it, then switches back to your normal output when done.


## Requirements

Requires a virtual audio loopback device:
- Install the included `MenuBarRecorder-1.0.0.pkg` for a named driver
- Or use standard BlackHole 2ch (`brew install blackhole-2ch`, or https://existential.audio/blackhole/)

## Built with
- [JUCE](https://juce.com) - Cross-platform audio framework
- BlackHole - Virtual audio loopback driver