//
//  tessendorfOceanNode.h
//  TessendorfOceanNode
//
//  Created by Steven Dao on 7/24/13.
//
//
// ==========================================================================
// This file is based on the sample file [animCubeNode.cpp] in the Maya SDK.
// Copyright 1995,2006,2008 Autodesk, Inc. All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================

#include <maya/MGlobal.h>

#include <maya/MTime.h>
#include <maya/MAngle.h>
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

#include "tessendorf.h"

#define MCheckErr(stat,msg)     \
if (MS::kSuccess != stat) {	\
cerr << msg;                    \
return MS::kFailure;            \
}

/**
 * A Maya node that creates an ocean wave simulation using Tessendorf's FFT method.
 */
class tessendorfOcean : public MPxNode
{
public:
    tessendorfOcean() {};
    virtual         ~tessendorfOcean() {};
    virtual MStatus compute(const MPlug& plug, MDataBlock& data);
    static  void*   creator();
    static  MStatus initialize();
    
    static MObject  time;           /** MTime attribute; the time passed in the simulation. */
    static MObject  resolution;     /** int attribute; the number of vertices per row or column. */
    static MObject  planeSize;      /** int attribute; the length or width of the ocean plane. */
    static MObject  waveSizeFilter; /** double attribute; waves smaller than this size are hidden. */
    static MObject  amplitude;      /** double attribute; determines the height of the waves. */
    static MObject  windSpeed;      /** double attribute; the speed of waves. */
    static MObject  windDirection;  /** MVector attribute; the direction of the wave movement. */
    static MObject  choppiness;     /** double attribute; higher value is choppier. */
    static MObject  seed;           /** int attribute; seed for the pseudorandom number generator. */
    static MObject  outputMesh;
    static MTypeId  id;
    
protected:
    /**
     * Generates an output mesh given the specified wave simulation parameters.
     *
     * \param time the time passed in the simulation
     * \param vertexResolution the number of vertices per row or column
     * \param planeSize the length or width of the ocean plane
     * \param waveSizeFilter waves smaller than this size are hidden
     * \param amplitude determines the height of the waves
     * \param windSpeed the speed of waves
     * \param windDirection the direction of the wave movement
     * \param choppiness higher value is choppier
     * \param seed seed for the pseudorandom number generator
     * \param the object reference to the output mesh data
     * \return the output mesh
     */
    MObject createMesh(const MTime& time,
                       const int vertexResolution,
                       const double planeSize,
                       const double waveSizeFilter,
                       const double amplitude,
                       const double windSpeed,
                       const MAngle& windDirection,
                       const double choppiness,
                       const int seed,
                       MObject& outData,
                       MStatus& stat);
};

MObject tessendorfOcean::time;
MObject tessendorfOcean::resolution;
MObject tessendorfOcean::planeSize;
MObject tessendorfOcean::waveSizeFilter;
MObject tessendorfOcean::amplitude;
MObject tessendorfOcean::windSpeed;
MObject tessendorfOcean::windDirection;
MObject tessendorfOcean::choppiness;
MObject tessendorfOcean::seed;
MObject tessendorfOcean::outputMesh;
MTypeId tessendorfOcean::id(0x12345);

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
    tessendorfOcean::time = unitAttr.create("time", "tm", MFnUnitAttribute::kTime, 0.0);
    addAttribute(tessendorfOcean::time);
    
    // Resolution (powers of 2 between 16 and 2048)
    tessendorfOcean::resolution = numAttr.create("resolution", "res", MFnNumericData::kInt, 8);
    numAttr.setMin(4);
    numAttr.setMax(11);
    addAttribute(tessendorfOcean::resolution);
    
    // Plane size
    tessendorfOcean::planeSize = numAttr.create("planeSize", "psz", MFnNumericData::kDouble, 100.);
    numAttr.setMin(10.);
    numAttr.setMax(500.);
    addAttribute(tessendorfOcean::planeSize);
    
    // Wave size
    tessendorfOcean::waveSizeFilter = numAttr.create("waveSizeFilter", "wsz", MFnNumericData::kDouble, 1.);
    numAttr.setMin(0.);
    numAttr.setMax(50.);
    addAttribute(tessendorfOcean::waveSizeFilter);
    
    // Amplitude <= 1.0
    tessendorfOcean::amplitude = numAttr.create("amplitude", "a", MFnNumericData::kDouble, 0.001);
    numAttr.setMin(0.);
    numAttr.setMax(0.1);
    addAttribute(tessendorfOcean::amplitude);
    
    // Wind speed
    tessendorfOcean::windSpeed = numAttr.create("windSpeed", "ws", MFnNumericData::kDouble, 2.);
    numAttr.setMin(0.);
    numAttr.setMax(20.);
    addAttribute(tessendorfOcean::windSpeed);
    
    // Wind direction (degrees)
    tessendorfOcean::windDirection = unitAttr.create("windDirection", "wd", MAngle(0.));
    numAttr.setSoftMin(0.);
    numAttr.setSoftMax(2 * M_PI);
    addAttribute(tessendorfOcean::windDirection);
    
    // Choppiness
    tessendorfOcean::choppiness = numAttr.create("choppiness", "c", MFnNumericData::kDouble, 0.5);
    numAttr.setMin(0.);
    numAttr.setMax(2.);
    addAttribute(tessendorfOcean::choppiness);
    
    // Seed for PRNG
    tessendorfOcean::seed = numAttr.create("seed", "seed", MFnNumericData::kInt, 1);
    addAttribute(tessendorfOcean::seed);
    
    // Output mesh
    tessendorfOcean::outputMesh = typedAttr.create("outputMesh", "out", MFnData::kMesh);
    typedAttr.setStorable(false);
    addAttribute(tessendorfOcean::outputMesh);
    
    attributeAffects(tessendorfOcean::time, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::resolution, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::planeSize, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::waveSizeFilter, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::amplitude, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::windSpeed, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::windDirection, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::choppiness, tessendorfOcean::outputMesh);
    attributeAffects(tessendorfOcean::seed, tessendorfOcean::outputMesh);
    
    return MS::kSuccess;
}

MObject tessendorfOcean::createMesh(const MTime& time,
                                    const int vertexResolution /* Number of vertices per row/col. */,
                                    const double planeSize,
                                    const double waveSizeFilter,
                                    const double amplitude,
                                    const double windSpeed,
                                    const MAngle& windDirection,
                                    const double choppiness,
                                    const int seed,
                                    MObject& outData,
                                    MStatus& stat)
{
    int faceResolution = vertexResolution - 1; /* Number of faces per row/col. */
    
    MFloatPointArray vertices;
    MIntArray faceDegrees;
    MIntArray faceVertices;
    int i, j;
    
    int numFaces = faceResolution * faceResolution;
    
    // Scale using the current time.
    double seconds = time.as(MTime::kSeconds);
    
    // Convert wind direction to a unit vector.
    double dirRadians = windDirection.asRadians();
    MVector dirVector = MVector(cos(dirRadians), 0., sin(dirRadians));
    
    // tessendorf(double amplitude, double speed, MVector direction, double choppiness, double time, int resX, int resZ, double scaleX, double scaleZ, int rngSeed);
    tessendorf simulation(amplitude, windSpeed, dirVector, choppiness, seconds, vertexResolution, vertexResolution, planeSize, planeSize, waveSizeFilter, seed);
    MFloatPointArray simResult = simulation.simulate();
    // Set up an array containing the vertex positions for the plane. The
    // vertices are placed equi-distant on the X-Z plane to form a square
    // grid that has a side length of "planeSize".
    for (i = 0; i < vertexResolution; ++i)
    {
        for (j = 0; j < vertexResolution; ++j)
        {
            MFloatPoint disp = simResult[i * vertexResolution + j];
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
    for (i = 0; i < faceResolution; ++i)
    {
        for (j = 0; j < faceResolution; ++j)
        {
            faceVertices.append((i+1) * vertexResolution + j);
            faceVertices.append((i+1) * vertexResolution + j + 1);
            faceVertices.append(i * vertexResolution + j + 1);
            faceVertices.append(i * vertexResolution + j);
        }
    }
    
    MFnMesh meshFn;
    MObject newMesh = meshFn.create(vertices.length(), numFaces, vertices,
                                    faceDegrees, faceVertices, outData, &stat);
    
    return newMesh;
}

MStatus tessendorfOcean::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus returnStatus;
    
    if (plug == outputMesh) {
        // Get the time attribute.
        MDataHandle timeData = data.inputValue(time, &returnStatus);
        MCheckErr(returnStatus, "ERROR getting time data handle\n");
        MTime time = timeData.asTime();
        
        // Get the resolution attribute.
        MDataHandle resData = data.inputValue(resolution, &returnStatus);
        MCheckErr(returnStatus, "ERROR getting resolution data handle\n");
        int res = pow(2, resData.asInt());
        
        // Get the planeSize attribute.
        MDataHandle sizeData = data.inputValue(planeSize, &returnStatus);
        MCheckErr(returnStatus, "ERROR getting planeSize data handle\n");
        double size = sizeData.asDouble();
        
        // Get the waveSizeFilter attribute.
        MDataHandle wSizeData = data.inputValue(waveSizeFilter, &returnStatus);
        MCheckErr(returnStatus, "ERROR getting waveSizeFilter data handle\n");
        double wSize = wSizeData.asDouble();
        
        // Get the amplitude attribute.
        MDataHandle ampData = data.inputValue(amplitude, &returnStatus);
        MCheckErr(returnStatus, "ERROR getting amplitude data handle\n");
        double amp = ampData.asDouble();
        
        // Get the windSpeed attribute.
        MDataHandle windSpeedData = data.inputValue(windSpeed, &returnStatus);
        MCheckErr(returnStatus, "ERROR getting windSpeed data handle\n");
        double speed = windSpeedData.asDouble();
        
        // Get the windDirection attribute.
        MDataHandle windDirectionData = data.inputValue(windDirection, &returnStatus);
        MCheckErr(returnStatus, "ERROR getting windDirection data handle\n");
        MAngle dir = windDirectionData.asAngle();
        
        // Get the choppiness attribute.
        MDataHandle chopData = data.inputValue(choppiness, &returnStatus);
        MCheckErr(returnStatus, "ERROR getting choppiness data handle\n");
        double chop = chopData.asDouble();
        
        // Get the seed attribute.
        MDataHandle seedData = data.inputValue(seed, &returnStatus);
        MCheckErr(returnStatus, "ERROR getting seed data handle\n");
        int rngSeed = seedData.asInt();
        
        // Get the output object attribute.
        MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
        MCheckErr(returnStatus, "ERROR getting polygon data handle\n");
        
        MFnMeshData dataCreator;
        MObject newOutputData = dataCreator.create(&returnStatus);
        MCheckErr(returnStatus, "ERROR creating outputData");
        
        createMesh(time, res, size, wSize, amp, speed, dir, chop, rngSeed, newOutputData, returnStatus);
        MCheckErr(returnStatus, "ERROR creating new tessendorfOcean");
        
        outputHandle.set(newOutputData);
        data.setClean(plug);
    } else
        return MS::kUnknownParameter;
    
    return MS::kSuccess;
}

MStatus initializePlugin(MObject obj)
{
    MStatus status;
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
    MStatus status;
    MFnPlugin plugin(obj);
    
    status = plugin.deregisterNode(tessendorfOcean::id);
    if (!status) {
        status.perror("deregisterNode");
        return status;
    }
    
    return status;
}
