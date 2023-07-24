/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
///
/// This process allows to transform the detector hits mean positions using
/// rotation, translation and specular transformations.
///
/// This process will not modify the detector hits inside the event. It
/// will just create new observables with the resulting transformed mean
/// positions.
///
/// The process allows to include different definitions:
/// - **specular**: It requires a `position` and a `vector` that emplace the plane
/// position and orientation used for the specular transformation.
/// - **rotation**: It requires a `position` that defines the rotation center,
/// a `vector` that defines the rotation axis, and an `angle` that defines the
/// rotation amount.
/// - **translation**: It only requires to define the `vector` field with the
/// translation to be applied.
///
/// The different transformations can be defined inside the RML section as
/// illustrated in the following example:
///
/// \code
///     <TRestDetectorHitmapAnalysisProcess name="hitmap"
///                                             title="A properly oriented hitmap">
///        <specular name="specY" position="(0,0,0)mm" vector="(0,1,0)" />
///        <rotation name="rot25" position="(0,0,0)mm" vector="(0,1,0)" angle="25deg" />
///        <translation name="up10" vector="(0,10,0)mm" />
///        <rotation name="rot10" position="(10,20,0)mm" vector="(0,1,0)" angle="10deg" />
///
///        <parameter name="transformations" value="{specY,rot25,up10,rot10}" />
///    </TRestDetectorHitmapAnalysisProcess>
/// \endcode
///
/// \htmlonly <style>div.image img[src="transformedHitmap.png"]{width:800px;}</style> \endhtmlonly
/// ![The effect of different transfromation processes](transformedHitmap.png)
///
/// The transformations to be applied, and the order in which those transformations will
/// be applied are defined inside the parameter `transformations` that contains a list
/// with the names from the transformation definitions.
///
/// ### Observables added by this process
///
/// * **xMean**: The mean X-position after the transformation
/// * **yMean**: The mean Y-position after the transformation
/// * **zMean**: The mean Z-position after the transformation
///
///______________________________________________________________________________
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2023-July: First implementation
///
/// \class      TRestDetectorHitmapAnalysisProcess
/// \author     Javier Galan
///
///______________________________________________________________________________
///
//////////////////////////////////////////////////////////////////////////

#include "TRestDetectorHitmapAnalysisProcess.h"

using namespace std;

ClassImp(TRestDetectorHitmapAnalysisProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDetectorHitmapAnalysisProcess::TRestDetectorHitmapAnalysisProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDetectorHitmapAnalysisProcess::~TRestDetectorHitmapAnalysisProcess() {}

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param configFilename A const char* giving the path to an RML file.
///
TRestDetectorHitmapAnalysisProcess::TRestDetectorHitmapAnalysisProcess(const char* configFilename) {
    Initialize();
    LoadConfigFromFile(configFilename);
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the section name
///
void TRestDetectorHitmapAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fHitsEvent = nullptr;
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestDetectorHitmapAnalysisProcess::ProcessEvent(TRestEvent* inputEvent) {
    fHitsEvent = (TRestDetectorHitsEvent*)inputEvent;

    TVector3 hitMean = fHitsEvent->GetMeanPosition();

    TVector3 transformedMean = hitMean;

    for (const auto& t : fTransformations) {
        HitTransformation tr;
        tr = GetTransformation(t);

        if (tr.type == "specular") transformedMean = Specular(transformedMean, tr);
        if (tr.type == "rotation") transformedMean = Rotation(transformedMean, tr);
        if (tr.type == "translation") transformedMean = Translation(transformedMean, tr);
    }

    SetObservableValue("xMean", transformedMean.X());
    SetObservableValue("yMean", transformedMean.Y());
    SetObservableValue("zMean", transformedMean.Z());

    return fHitsEvent;
}

///////////////////////////////////////////////
/// \brief It performs a specular transformation of the given `pos` in the argument and the transformation
/// properties stored in the HitTransformation.
///
TVector3 TRestDetectorHitmapAnalysisProcess::Specular(const TVector3& pos, const HitTransformation& tr) {
    if (tr.type != "specular") return {0, 0, 0};

    TVector3 V = pos - tr.position;
    Double_t vByN = V.Dot(tr.vector);
    TVector3 reflectionVector = V - 2 * vByN * tr.vector;
    return tr.position + reflectionVector;
}

///////////////////////////////////////////////
/// \brief It performs a rotation of the given `pos` in the argument and the transformation properties
/// stored in the HitTransformation.
///
TVector3 TRestDetectorHitmapAnalysisProcess::Rotation(const TVector3& pos, const HitTransformation& tr) {
    if (tr.type != "rotation") return {0, 0, 0};

    TVector3 position = pos - tr.position;
    position.Rotate(tr.angle, tr.vector);
    position += tr.position;

    return position;
}

///////////////////////////////////////////////
/// \brief It performs a translation of the given `pos` in the argument and the transformation properties
/// stored in the HitTransformation.
///
TVector3 TRestDetectorHitmapAnalysisProcess::Translation(const TVector3& pos, const HitTransformation& tr) {
    if (tr.type != "translation") return {0, 0, 0};

    TVector3 position = pos + tr.vector;
    return position;
}

///////////////////////////////////////////////
/// \brief It returns the transformation structure that matches the name in the argument
///
HitTransformation TRestDetectorHitmapAnalysisProcess::GetTransformation(const std::string& name) {
    for (const auto& t : fTransDefinitions) {
        if (t.name == name) return t;
    }

    HitTransformation Tr;
    return Tr;
}

///////////////////////////////////////////////
/// \brief A custom initalization from a config file
///
void TRestDetectorHitmapAnalysisProcess::InitFromConfigFile() {
    TRestEventProcess::InitFromConfigFile();

    auto specularDef = GetElement("specular");
    while (specularDef) {
        HitTransformation trans;
        trans.type = "specular";

        string name = GetFieldValue("name", specularDef);
        if (name == "Not defined") {
            RESTError << "The `name` field was not found in specular definition!" << RESTendl;
            exit(1);
        }
        trans.name = name;

        TVector3 position = Get3DVectorParameterWithUnits("position", specularDef);
        if (position == TVector3(-1, -1, -1)) {
            RESTError << "The `position` field was not found in specular definition!" << RESTendl;
            exit(1);
        }
        trans.position = position;

        TVector3 vect = Get3DVectorParameterWithUnits("vector", specularDef);
        if (vect == TVector3(-1, -1, -1)) {
            RESTError << "The `vector` field was not found in specular definition!" << RESTendl;
            exit(1);
        }
        trans.vector = vect;

        fTransDefinitions.push_back(trans);
        specularDef = GetNextElement(specularDef);
    }

    auto rotationDef = GetElement("rotation");
    while (rotationDef) {
        HitTransformation trans;
        trans.type = "rotation";

        string name = GetFieldValue("name", rotationDef);
        if (name == "Not defined") {
            RESTError << "The `name` field was not found in rotation definition!" << RESTendl;
            exit(1);
        }
        trans.name = name;

        TVector3 position = Get3DVectorParameterWithUnits("position", rotationDef);
        if (position == TVector3(-1, -1, -1)) {
            RESTError << "The `position` field was not found in rotation definition!" << RESTendl;
            exit(1);
        }
        trans.position = position;

        TVector3 vect = Get3DVectorParameterWithUnits("vector", rotationDef);
        if (vect == TVector3(-1, -1, -1)) {
            RESTError << "The `vector` field was not found in rotation definition!" << RESTendl;
            exit(1);
        }
        trans.vector = vect;

        Double_t angle = GetDblParameterWithUnits("angle", rotationDef);
        if (angle == PARAMETER_NOT_FOUND_DBL) {
            RESTError << "The `angle` field was not found in rotation definition!" << RESTendl;
            exit(1);
        }
        trans.angle = angle;

        fTransDefinitions.push_back(trans);
        rotationDef = GetNextElement(rotationDef);
    }

    auto translationDef = GetElement("translation");
    while (translationDef) {
        HitTransformation trans;
        trans.type = "translation";

        string name = GetFieldValue("name", translationDef);
        if (name == "Not defined") {
            RESTError << "The `name` field was not found in translation definition!" << RESTendl;
            exit(1);
        }
        trans.name = name;

        TVector3 vect = Get3DVectorParameterWithUnits("vector", translationDef);
        if (vect == TVector3(-1, -1, -1)) {
            RESTError << "The `vector` field was not found in translation definition!" << RESTendl;
            exit(1);
        }
        trans.vector = vect;

        fTransDefinitions.push_back(trans);
        translationDef = GetNextElement(translationDef);
    }
}

///////////////////////////////////////////////
/// \brief Prints out the metadata member values
///
void TRestDetectorHitmapAnalysisProcess::PrintMetadata() {
    BeginPrintProcess();

    if (!fTransDefinitions.empty()) {
        RESTMetadata << " List of transformation definitions: " << RESTendl;
        RESTMetadata << " ----------------------------------- " << RESTendl;
        RESTMetadata << " " << RESTendl;
    } else
        RESTMetadata << " No transformations found! " << RESTendl;

    for (const auto& t : fTransDefinitions) {
        if (t.type == "specular") {
            RESTMetadata << " - Specular transformation: " << RESTendl;
            RESTMetadata << "   + Plane position : (" << t.position.X() << ", " << t.position.Y() << ", "
                         << t.position.Z() << ") mm" << RESTendl;
            RESTMetadata << "   + Plane normal : (" << t.vector.X() << ", " << t.vector.Y() << ", "
                         << t.vector.Z() << ")" << RESTendl;
            RESTMetadata << " " << RESTendl;
        }

        if (t.type == "rotation") {
            RESTMetadata << " - Rotation transformation: " << RESTendl;
            RESTMetadata << "   + Rotation center : (" << t.position.X() << ", " << t.position.Y() << ", "
                         << t.position.Z() << ") mm" << RESTendl;
            RESTMetadata << "   + Rotation axis : (" << t.vector.X() << ", " << t.vector.Y() << ", "
                         << t.vector.Z() << ")" << RESTendl;
            RESTMetadata << "   + Rotation angle : " << t.angle * units("degrees") << " degrees" << RESTendl;
            RESTMetadata << " " << RESTendl;
        }

        if (t.type == "translation") {
            RESTMetadata << " - Translation transformation: " << RESTendl;
            RESTMetadata << "   + Translation vector : (" << t.vector.X() << ", " << t.vector.Y() << ", "
                         << t.vector.Z() << ") mm" << RESTendl;
            RESTMetadata << " " << RESTendl;
        }
    }

    RESTMetadata << " Order of transformations: " << RESTendl;
    RESTMetadata << " ------------------------- " << RESTendl;
    RESTMetadata << " " << RESTendl;

    int n = 0;
    for (const auto& t : fTransformations) {
        n++;
        RESTMetadata << n << ". " << t << RESTendl;
    }

    EndPrintProcess();
}
