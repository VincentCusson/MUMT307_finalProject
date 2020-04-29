# MUMT 307 Final Project
This repository contains the code and report of my final project for the class 'Music & Audio Computing II' with Professor Gary P. Scavone.

I designed my first module, a basic synthesizer, for the [VCV Rack](https://vcvrack.com/) Virtual Eurorack DAW.

---
### Report
* [Introduction](#Introduction)
* [Objectives](#Objectives)
* [Methodology](#Methodology)
* [Results](#Results)
* [Issues and future work](#Issues-and-future-work)

#### Introduction
The world of synthesizers, driven by the imagination of engineers and musicians, have been constantly evolving over the years. A specific subset of synthesizers, modular synthesizers called Eurorack, got standardized in the late 90s. Hundreds of different manufacturers produced thousands of modules for the delight of musicians. However, the price of this kind of hardware can be an obstacle for many.

In 2017, Andrew Belt launched the VCV Rack free and open-source software. This project aims to emulate an Eurorack setup allowing developers to create software implementation of old and new modules. Their well-documented C++ SDK and straightforward UI design workflow make it a good platform to experiment and apply knowledge from this class.
The [official forum](https://community.vcvrack.com/), [Facebook group](https://www.facebook.com/groups/vcvrack/), and [git repositories](https://github.com/VCVRack) are the best place to get an insight of its vibrant community of programmers and artist.


#### Objectives
As a user of VCV Rack, one of my main goals for this project was to discover the development process of a module. It was also a great opportunity to tackle C++ programming. Having always loved playing them, I decided to use my newly acquired DSP skills to create a synthesizer, instead of a 'utility' or 'effect' module. Outputting a sinusoidal waveform controlled by a MIDI device was my first objective but I managed to add more features to it.

#### Methodology
At first, I followed the official plugin development tutorial. Except for some small issues with the `export RACK_DIR=<Rack SDK folder` command, I quickly got a working environment.

The first step is to use the `helper.py` script to create a plugin. A plugin is a collection of modules by a developer (e.g. MyPlugin could contain ASynth, AnotherSynth, AnEffect). It also creates a `plugin.json` manifest describing the plugin allowing it to be added to the community plugin library.

I then moved to the Inkscape software to create the SVG panel graphic for my first envisaged module 'mirror'. Again, following the official documentation for size and placeholder conventions was pretty straightforward.

The same `helper.py` script is reused, this time with the `createmodule`  and the 'mirror.SVG' arguments, and create the skeleton of the module for us.

Inspired by the 'hello world' of audio programming in 'Designing Audio Objects for Max/MSP and Pd' book, I wrote my first bit of code in the 'process' function.

```C++
void process(const ProcessArgs& args) override {
  float in = 0.f;
  in = inputs[IN1_INPUT].getVoltage();
  outputs[OUT1_OUTPUT].setVoltage(in);
}
```
This small program takes the voltage input value and output the exact same thing, acting (vaguely) like a mirror.

I run the `make install` command in the plugin directory to build my module and install it automatically in the local Rack library.

Here is a screenshot of the working 'mirror' module in VCV Rack.

![](/report/mirror.png "Working mirror module")

---

We are still far from what we could call a synthesizer so let's get more serious.

I decided to create a module called 'Playground' that would allow me to experiment with the API and test some synthesis ideas. It has 4 inputs, 4 LEDs, 4 widgets (knob, button, slider, etc.), and 4 outputs. It really helped me in the programming of my final synthesizer.

Here is a screenshot of the 'Playground' interface.

![](/report/playground.png "Playground module")


#### Results
Following my experimentation with the 'Playground' module, here is a list of features of my final 'Synth' module:
* Pitch of the waveform controlled by a MIDI note
* Octave change [-2,2] from a knob
* Detune [-100,100] cents from a knob
* Gate input to trigger the ADSR envelope
* A-D-S-R knobs to control those parameters
* Velocity input from a MIDI controller to change the amplitude
* A tremolo effect controlled by a knob
* A 'harmonic' knob to add sine waves for additive synthesis
* A 'shaper' knob to control some distortion effect

The ADSR envelope is based on a tutorial from the book 'Developing virtual synthesizers with VCV Rack' by Leonardo Gabrielli.
The distortion effect is based on a module from lindenbergresearch.

Here is a screenshot of the final synth.

![](/report/synth.png "Synth")

[Here](/report/synthDemo.mp4) is a short video demo of the synth in action.

#### Issues and future work
The two main issues I had during this project are:
* The CV conventions in the VCV Rack software
  * In different parts of the code, we use 10V, 5V, and even normalized values. Expecting or producing the wrong kind of signal can be problematic for our speakers and ears. Mapping the range and interpolation type of some knobs to their audio parameters was also challenging.


* The C++ syntax
  * Beside the homework, this was my first real C++ project. Using mainly Max/Msp and a bit of Python for my various projects, I did not take advantage of the OOP paradigm this new language was offering me. Having to declare the (hopefully best) type of variable was also a nice challenge.

  An anthology of my 'what is this' moments:
  * `template <typename T>`
  * `this->pw = clamp(pw, pwMin, 1.f - pwMin);`
  * `T triggered = ~resetState & on;`
  * `auto* oscillator = &oscillators[c / 4];`

  It was a very good way to learn more about this popular language and I am looking forward having the opportunity to practice it more.


Future work would include:
* Make it polyphonic
  * This involves a big rewrite in classes (which I am not yet used to in C++)


* Set default values for unconnected CV inputs
  * Right now the synth won't play without a velocity input


* Add CV inputs to existing knob parameters
  * To allow more expressive control over the synth


* Check input validity to avoid breaking my speakers
  * Basically clamping input and output signals according to CV conventions


* Some sort of optimization
  * Mainly related to conditions based on cable connection


* Exploring some other synthesis and filtering algorithms


* UI redesign according to aforementioned changes

### References
* [VCV Rack official website](https://vcvrack.com/)
* [VCV Rack tutorial](https://vcvrack.com/manual/PluginDevelopmentTutorial)
* [VCV Rack fundamental modules repository](https://github.com/VCVRack/Fundamental)
* [VCVBook repository](https://github.com/LOGUNIVPM/VCVBook)
* [Bogaudio modules repository](https://github.com/bogaudio/BogaudioModules)
* [Lindenbergresearch modules repository](https://github.com/lindenbergresearch/LRTRack)
