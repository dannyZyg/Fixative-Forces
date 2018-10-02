# Fixative Forces

A program that explores forces through interaction. Uses computer vision (kinect depth image) to isolate a body in the image, facilitating communication
between small particles/shapes and a human player. Different modes of behaviour are triggered after periods of interaction. After a certain number of
large waves or strong motions (currently 10) the program will move to the next mode.

Optical Flow from the video acts as another interactive force.

# Getting Started

Requires Microsoft Kinect


# Acknowledgements

Originally inspired by magnetic forces, although the program strays away from that a fair bit!

Inspiration:

https://www.youtube.com/watch?v=LMFAOLKaYd0
https://www.openprocessing.org/sketch/171308
https://vimeo.com/153944735


Programming references and help:

Optical Flow example from WCC class

Transfering ofPolyline to ofPath = https://forum.openframeworks.cc/t/fill-polyline/10800

Particle System help from the Kadenze course Creative Programming for Audiovisual Art (Memo Akten guest lecture)

Parameter Smoother class - original code from https://www.youtube.com/watch?v=BdJRSqgEqPQ, adapted to accept ofPoint

Daniel Shiffman's Nature of Code - Forces chapter
