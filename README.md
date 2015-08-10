# ImageBackgroundRemoval
A project for MU 2015

This is part of my summer internship. The purpose of the application is to take a
sequence of images and generate a ground truth algorithm that separates the foreground
from the background.

Dependencies: Canon SDK, OpenCV, QT5, SDL2.

Tested on: Windows, but may support Mac OS.

Usage:
1. Position your Canon camera so that it points at the monitor where the program is run.
2. Connect it to the computer using a USB cable and make sure that the drivers for it are installed.
3. Place an object between the camera and the screen that you wish to separate from the background.
4. Start up the application and configure any settings you wish. There is a histogram in the live
   preview window to aid in exposure selection.
5. Ensure that at least 5 colours are selected for Ground Truth generation. Without Ground Truth,
   the images are saved without processing.
6. Press GO, and wait for the camera to take a sequence of images.
7. When prompted, remove the object and press enter to take 5 other images.
8. Wait for the generated results.

Note that the debug output provided through the console is highly useful, and is designed to be
the main feedback mechanism. So you should preferably run the program through a terminal!

System requirements:
  At least 1.7GB of free RAM memory. May use as much as 500MB of free disk space.
  These requirements for a seemingly simple application are due to the pure
  amount of data that must be processed (13 images in F32 3-component format at its peak).

System structure:
  Aside from the many helper classes and files, the five main components are:
     1. OpenGLBox - This class is responsible for drawing the live view, and applying the histogram.
        Retrieving a live preview frame is a 2-stage process: First a the image is downloaded to the 
        computer and a thread is spawned to process the image. Then, the image is actually presented
        and drawn.
     2. Window - This is responsible for handling the user interface, and preparing information for
        the other classes.
     3. ActionClass - This is the class responsible for the process of taking and processing the
        sequence of images upon pressing Go. When used, the QT event queue should be disabled to
        avoid it interfering with the SDL event queue.
     4. Ground Truth - This is a separate process spawned by the Action Class to compute the three Ground
        Truth images. If the computation is done in the main process, it crashes - I image that's due to the
        DLL placement in memory of the 32 bit application. If separated, not only can the Ground Truth algorithm
        be compiled into a 64 bit application, but even the 32 bit version does not crash. It receives the
        processing task from the Action Class using a list of arguments and temporary files.
     5. Camera - This class encapsulates handling one or multiple Canon cameras. Currently the program only
        selects the first camera found, but the Camera class is capable of handling multiple cameras.
        
 The typical run scenario:
     1. The system initialises the camera, interface and preview window.
     2. The user presses the Go button, and a sequence of photos is taken in rapid succession.
     3. The images are processed, their raw values interpolated into rgb.
     4. The GroundTruth application is run on temporary files to compute the alpha.
     5. The program cleans up and resumes the user interface.
     
 Warning: There might be a potential memory leak when taking a sequence of images. If so, it is minor, but I
 can't seem to be able to find it.
 
 
