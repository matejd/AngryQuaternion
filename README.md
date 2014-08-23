Dual Quaternion Skinning
------------------------

An implementation of dual quaternion blending (as opposed to linear blending) following
papers [1] and [2]. Uses NVIDIA's GameWorks Samples framework.
See it online [here](http://matejd.github.io/AngryQuaternion/AngryDudeApp.html).

[1] Skinning with Dual Quaternions (http://www.seas.upenn.edu/~ladislav/dq/index.html)

[2] Geometric Skinning with Approximate Dual Quaternion Blending


Building the app for the web
----------------------------

Install Emscripten and make sure em++ is visible. Go to
`{SAMPLES_ROOT}/samples/build/html5/`

and run make. AngryDudeApp is built and placed into
`{SAMPLES_ROOT}/samples/bin/html5/`

Check the results with
`python -m SimpleHTTPServer 8080`
or something similar. Tested with Emscripten 1.23.0.


Building for Linux
------------------

Run make in
`{SAMPLES_ROOT}/samples/build/linux64/`


![Screenshot](http://matejd.github.io/AngryQuaternion/screenshot.png)
