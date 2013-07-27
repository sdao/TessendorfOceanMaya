//-
// ==========================================================================
// Copyright 1995,2006,2008 Autodesk, Inc. All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+

#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnPlugin.h>

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnNumericData.h>

#include <maya/MIOStream.h>

#include "helpers.h"
#include "tessendorf.h"

#define MCheckErr(stat,msg)     \
if ( MS::kSuccess != stat ) {	\
cerr << msg;                    \
return MS::kFailure;            \
}

class tessendorfOcean : public MPxNode
{
public:
    tessendorfOcean() {};
	virtual 		~tessendorfOcean() {};
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void*	creator();
	static  MStatus initialize();
    
	static MObject	time;
    static MObject  resolution;     // int
    static MObject  planeSize;      // double
    static MObject  waveSteepness;  // double
    static MObject  waveAmplitude;  // double
    static MObject  waveLength;     // double
    static MObject  waveSpeed;      // double
    static MObject  waveRotation;   // double
	static MObject	outputMesh;
	static MTypeId	id;
    
protected:
	MObject createMesh(const MTime& time,
                       const int resolution,
                       const double planeSize,
                       const double waveSteepness[3],
                       const double waveAmplitude[3],
                       const double waveLength[3],
                       const double waveSpeed[3],
                       const MVector waveDirection[3], // Vector3 with y == 0.
                       MObject& outData,
                       MStatus& stat);
};

MObject tessendorfOcean::time;
MObject tessendorfOcean::resolution;
MObject tessendorfOcean::planeSize;
MObject tessendorfOcean::waveSteepness;
MObject tessendorfOcean::waveAmplitude;
MObject tessendorfOcean::waveLength;
MObject tessendorfOcean::waveSpeed;
MObject tessendorfOcean::waveRotation;
MObject tessendorfOcean::outputMesh;
MTypeId tessendorfOcean::id( 0x12345 );

void* tessendorfOcean::creator()
{
	return new tessendorfOcean;
}

MStatus tessendorfOcean::initialize()
{
	MFnUnitAttribute unitAttr;
	MFnTypedAttribute typedAttr;
    MFnNumericAttribute numAttr;
    
    // Time
	tessendorfOcean::time = unitAttr.create( "time", "tm",
                                  MFnUnitAttribute::kTime,
                                          0.0 );
	addAttribute(tessendorfOcean::time);
    
    // Resolution
    tessendorfOcean::resolution = numAttr.create( "resolution", "res",
                                               MFnNumericData::kInt, 100 );
    numAttr.setMin(10);
    numAttr.setMax(500);
    addAttribute(tessendorfOcean::resolution);
    
    // Plane size
    tessendorfOcean::planeSize = numAttr.create( "planeSize", "ps",
                                              MFnNumericData::kDouble, 100. );
    numAttr.setMin(10.);
    numAttr.setMax(500.);
    addAttribute(tessendorfOcean::planeSize);
    
    // Steepness 0 <= x <= 1.0
    tessendorfOcean::waveSteepness = numAttr.create( "waveSteepness", "st",
                                                  MFnNumericData::k3Double );
    numAttr.setDefault(.5, .4, .3);
    numAttr.setMin(0., 0., 0.);
    numAttr.setMax(1., 1., 1.);
    addAttribute(tessendorfOcean::waveSteepness);
    
    // Amplitude <= 1.0
    tessendorfOcean::waveAmplitude = numAttr.create( "waveAmplitude", "a",
                                              MFnNumericData::k3Double );
    numAttr.setDefault(.4, .2, .1);
    numAttr.setMin(0., 0., 0.);
    numAttr.setMax(10., 10., 10.);
    addAttribute(tessendorfOcean::waveAmplitude);
    
    // Length >= 2 * M_PI
    tessendorfOcean::waveLength = numAttr.create( "waveLength", "l",
                                              MFnNumericData::k3Double );
    numAttr.setDefault(400., 200., 600.);
    numAttr.setMin(10., 10., 10.);
    numAttr.setMax(1000., 1000., 1000.);
    addAttribute(tessendorfOcean::waveLength);
    
    // Speed
    tessendorfOcean::waveSpeed = numAttr.create( "waveSpeed", "sp",
                                                  MFnNumericData::k3Double );
    numAttr.setDefault(50., 100., 150.);
    numAttr.setMin(0., 0., 0.);
    numAttr.setMax(1000., 1000., 1000.);
    addAttribute(tessendorfOcean::waveSpeed);
    
    // Angle
    tessendorfOcean::waveRotation = numAttr.create( "waveAngle", "ang",
                                                  MFnNumericData::k3Double );
    numAttr.setDefault(0., 20., -20.);
    addAttribute(tessendorfOcean::waveRotation);
    
    // Output mesh
    tessendorfOcean::outputMesh = typedAttr.create( "outputMesh", "out",
                                                 MFnData::kMesh );
	typedAttr.setStorable(false);
	addAttribute(tessendorfOcean::outputMesh);
    
	attributeAffects(tessendorfOcean::time, tessendorfOcean::outputMesh);
	attributeAffects(tessendorfOcean::resolution, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::planeSize, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::waveSteepness, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::waveAmplitude, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::waveLength, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::waveSpeed, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::waveRotation, tessendorfOcean::outputMesh);
    
	return MS::kSuccess;
}

MObject tessendorfOcean::createMesh(const MTime& time,
                          const int resolution,
                          const double planeSize,
                          const double waveSteepness[3],
                          const double waveAmplitude[3],
                          const double waveLength[3],
                          const double waveSpeed[3],
                          const MVector waveDirection[3], // Vector3 with y == 0.
                          MObject& outData,
                          MStatus& stat)
{
    MFloatPointArray vertices;
	MIntArray faceDegrees;
	MIntArray faceVertices;
	int i, j;
    
	// Scale using the current time.
	double seconds = time.as( MTime::kSeconds );
    
	float planeOffset = planeSize / 2.0f;
    float planeDim = planeSize / resolution;
    
	int numFaces = resolution * resolution;
    
    // tessendorf(double height, double speed, MVector direction, double choppiness, double time, int resX, int resZ, double scaleX, double scaleZ, int rngSeed);
    tessendorf simulation(.005, 2., MVector(.5, 0., .5), 1., seconds, resolution + 1, resolution + 1, 30., 30., 2);
    simulation.simulate();
    // Set up an array containing the vertex positions for the plane. The
	// vertices are placed equi-distant on the X-Z plane to form a square
	// grid that has a side length of "planeSize".
	for (i = 0; i < resolution + 1; ++i)
	{
		for (j = 0; j < resolution + 1; ++j)
		{
			MFloatPoint disp = simulation.vertices[i * (resolution + 1) + j];
			vertices.append(disp);
		}
	}
    
	// Set up an array containing the number of vertices
	// for each of the plane's faces.
	for (i = 0; i < numFaces; ++i)
	{
		faceDegrees.append(4); // Quads, so 4 vertices per face.
	}
    
	// Set up an array to assign the vertices for each face.
	for (i = 0; i < resolution; ++i)
	{
		for (j = 0; j < resolution; ++j)
		{
			faceVertices.append(i*(resolution+1) + j);
			faceVertices.append(i*(resolution+1) + j + 1);
			faceVertices.append((i+1)*(resolution+1) + j + 1);
			faceVertices.append((i+1)*(resolution+1) + j);
		}
	}
    
	MFnMesh	meshFn;
	MObject newMesh = meshFn.create(vertices.length(), numFaces, vertices,
                                    faceDegrees, faceVertices, outData, &stat);
    
	return newMesh;
}

MStatus tessendorfOcean::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;
    
	if (plug == outputMesh) {
		// Get the time attribute.
		MDataHandle timeData = data.inputValue( time, &returnStatus );
		MCheckErr(returnStatus, "ERROR getting time data handle\n");
		MTime time = timeData.asTime();
        
        // Get the resolution attribute.
        MDataHandle resData = data.inputValue( resolution, &returnStatus );
        MCheckErr(returnStatus, "ERROR getting resolution data handle\n");
        int res = resData.asInt();
        
        // Get the planeSize attribute.
        MDataHandle sizeData = data.inputValue( planeSize, &returnStatus );
        MCheckErr(returnStatus, "ERROR getting planeSize data handle\n");
        double size = sizeData.asDouble();
        
        // Get the waveSteepness attribute.
        MDataHandle steepData = data.inputValue( waveSteepness, &returnStatus );
        MCheckErr(returnStatus, "ERROR getting waveSteepness data handle\n");
        double3& steep = steepData.asDouble3();
        
        // Get the waveAmplitude attribute.
        MDataHandle ampData = data.inputValue( waveAmplitude, &returnStatus );
        MCheckErr(returnStatus, "ERROR getting waveAmplitude data handle\n");
        double3& amp = ampData.asDouble3();
        
        // Get the waveLength attribute.
        MDataHandle lengthData = data.inputValue( waveLength, &returnStatus );
        MCheckErr(returnStatus, "ERROR getting waveLength data handle\n");
        double3& length = lengthData.asDouble3();
        
        // Get the waveSpeed attribute.
        MDataHandle speedData = data.inputValue( waveSpeed, &returnStatus );
        MCheckErr(returnStatus, "ERROR getting waveSpeed data handle\n");
        double3& speed = speedData.asDouble3();
        
        // Get the waveRotation attribute.
        MDataHandle angData = data.inputValue( waveRotation, &returnStatus );
        MCheckErr(returnStatus, "ERROR getting waveRotation data handle\n");
        double3& ang = angData.asDouble3();
        
        MVector dir[3] = { MVector(cos_d(ang[0]), 0., sin_d(ang[0])),
            MVector(cos_d(ang[1]), 0., sin_d(ang[1])),
            MVector(cos_d(ang[2]), 0., sin_d(ang[2])) };
        
		// Get the output object attribute.
		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		MCheckErr(returnStatus, "ERROR getting polygon data handle\n");
        
		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		MCheckErr(returnStatus, "ERROR creating outputData");
        
		createMesh(time, 255, size, steep, amp, length, speed, dir, newOutputData, returnStatus);
		MCheckErr(returnStatus, "ERROR creating new tessendorfOcean");
        
		outputHandle.set(newOutputData);
		data.setClean( plug );
	} else
		return MS::kUnknownParameter;
    
	return MS::kSuccess;
}

MStatus initializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj, PLUGIN_COMPANY, "3.0", "Any");
    
	status = plugin.registerNode("tessendorfOcean", tessendorfOcean::id,
                                 tessendorfOcean::creator, tessendorfOcean::initialize);
	if (!status) {
		status.perror("registerNode");
		return status;
	}
    
	return status;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus	  status;
	MFnPlugin plugin(obj);
    
	status = plugin.deregisterNode(tessendorfOcean::id);
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
    
	return status;
}
