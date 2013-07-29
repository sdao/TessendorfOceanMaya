TessendorfOceanNode
===================

Maya node implementing Tessendorf's FFT method for generating ocean waves.
The Xcode project generates an Autodesk Maya plugin for Mac OS X (TessendorfOceanNode.bundle),
but the code is portable and can be compiled on Linux or Windows using the Maya SDKs
for those platforms.

The plugin can be used with Maya by placing it in your plugins directory (`~/Users/Shared/Autodesk/maya/2014/plug-ins` for Maya 2014).
Load it using the command Windows > Settings/Preferences > Plug-in Manager.
The `oceanNode.mel` script can then be run in Maya to setup the required nodes in the dependency graph and
to connect the `time` attribute to the scene's time slider.

For more information on how Tessendorf's equations are used to generate waves, see the `coursenotes2002.pdf` file.

This project incorporates the [Kiss FFT library](http://sourceforge.net/projects/kissfft/) for performing Fast Fourier Transforms. (Code licensed under a BSD-style license.)

This project minimally uses Autodesk sample code.

License
=======
The code in this project, outside Kiss FFT code and Autodesk code, is licensed thusly:

The MIT License (MIT)

Copyright (c) 2013 Steven Dao

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
