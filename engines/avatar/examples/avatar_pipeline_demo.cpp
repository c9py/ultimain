#include "avatar/Avatar.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace Ultima::Avatar;

static EchoDrive makeDrive(Emotion emotion, float intensity, const EchoState& persona) {
    EchoDrive drive;
    drive.echo = persona;
    drive.emotion = emotion;
    drive.emotionIntensity = intensity;
    drive.valence = (emotion == Emotion::Happiness) ? 0.7f
                    : (emotion == Emotion::Sadness || emotion == Emotion::Anger) ? -0.5f
                    : 0.0f;
    drive.arousal = (emotion == Emotion::Fear || emotion == Emotion::Surprise) ? 0.8f : 0.45f;
    drive.identityName = "avatar";
    return drive;
}

int main(int argc, char** argv) {
    std::string jsonPath;
    std::string objPath;
    if (argc > 1) {
        jsonPath = argv[1];
    }
    if (argc > 2) {
        objPath = argv[2];
    }

    Pipeline pipe("avatar");
    EchoState persona = pipe.identity().persona;

    std::cout << "Avatar motion mesh pipeline\n";
    std::cout << "  meshy3d( live2d-dtecho[ meta-echo-dna< rig-logic + facs > ] ) /deltecho\n";
    std::cout << "  identity: " << pipe.identity().name
              << " fingerprint=" << pipe.identity().fingerprint << "\n\n";

    const Emotion sequence[] = {
        Emotion::Neutral, Emotion::Happiness, Emotion::Surprise,
        Emotion::Fear, Emotion::Anger, Emotion::Sadness, Emotion::Disgust,
        Emotion::Neutral
    };

    std::ostringstream frames;
    frames << "{\n  \"composition\": \"meshy3d(live2d-dtecho[meta-echo-dna<rig-logic+facs>])/deltecho\",\n";
    frames << "  \"fingerprint\": " << pipe.identity().fingerprint << ",\n";
    frames << "  \"frames\": [\n";

    MotionFrame last;
    for (size_t i = 0; i < sizeof(sequence) / sizeof(sequence[0]); ++i) {
        const EchoDrive drive = makeDrive(sequence[i], sequence[i] == Emotion::Neutral ? 0.0f : 0.9f, persona);
        const PipelineStages s = pipe.evaluateDetailed(drive, 0.08f);
        last = s.echoed;
        const float disp = MotionMesh::vertexDisplacement(s.echoed);
        std::cout << std::setw(10) << emotionName(sequence[i])
                  << "  AU12=" << std::fixed << std::setprecision(3) << s.facs.get(ActionUnit::AU12_LipCornerPuller)
                  << "  jaw=" << s.rig.jawOpen
                  << "  mouthForm=" << s.live2d.get(Live2DParam::MouthForm)
                  << "  mouthOpen=" << s.echoed.mouthOpen
                  << "  verts=" << s.echoed.deformed.size()
                  << "  disp=" << disp
                  << "  residual=" << pipe.deltecho().lastResidualEnergy()
                  << "\n";
        if (i) {
            frames << ",\n";
        }
        frames << "    {\"emotion\":\"" << emotionName(sequence[i]) << "\","
               << "\"mouthOpen\":" << s.echoed.mouthOpen << ","
               << "\"smile\":" << s.echoed.smile << ","
               << "\"blink\":" << s.echoed.blink << ","
               << "\"displacement\":" << disp << ","
               << "\"residual\":" << pipe.deltecho().lastResidualEnergy() << "}";
    }
    frames << "\n  ]\n}\n";

    std::cout << "\nlast mesh triangles=" << (last.rest.indices.size() / 3)
              << " bones=" << last.rest.bones.size() << "\n";

    if (!jsonPath.empty()) {
        std::ofstream out(jsonPath);
        out << MotionMesh::toJson(last, makeDrive(Emotion::Happiness, 0.9f, persona));
        std::cout << "wrote " << jsonPath << "\n";
    }
    if (!objPath.empty()) {
        std::ofstream out(objPath);
        out << MotionMesh::toObj(last);
        std::cout << "wrote " << objPath << "\n";
    } else {
        std::cout << frames.str();
    }
    return 0;
}
