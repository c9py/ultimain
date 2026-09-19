#include "avatar/MetaEchoDNA.h"

#include <cmath>

namespace Ultima {
namespace Avatar {

namespace {

std::uint64_t fnv1a(std::uint64_t h, float v) {
    const auto bits = static_cast<std::uint32_t>(std::lround(v * 10000.0f) + 100000);
    h ^= bits;
    h *= 1099511628211ull;
    return h;
}

} // namespace

std::uint64_t MetaEchoCodec::fingerprint(const std::array<float, kDnaDim>& genotype) {
    std::uint64_t h = 14695981039346656037ull;
    for (float g : genotype) {
        h = fnv1a(h, g);
    }
    return h;
}

MetaEchoDNA MetaEchoCodec::encode(const RigPose& rig,
                                  const FACSFrame& facs,
                                  const EchoState& persona,
                                  const std::string& name) const {
    MetaEchoDNA dna;
    dna.name = name;
    dna.persona = persona;
    dna.restFacs = facs;
    dna.restRig = rig;

    // [0:8] Echo persona dimensions
    for (int i = 0; i < kEchoDim; ++i) {
        dna.genotype[i] = saturate(persona.dimensions[i]);
    }
    // [8:26] FACS rest (identity baseline)
    for (int i = 0; i < kFacsCount; ++i) {
        dna.genotype[8 + i] = saturate(facs.au[i]);
    }
    // [26:31] rig / echo meta
    dna.genotype[26] = saturate(persona.coherence);
    dna.genotype[27] = saturate(persona.resonance);
    dna.genotype[28] = saturate(persona.energy);
    dna.genotype[29] = saturate(rig.jawOpen);
    dna.genotype[30] = saturate(rig.smile);
    dna.genotype[31] = saturate(0.35f + persona.cognitiveLoad * 0.3f);

    dna.fingerprint = fingerprint(dna.genotype);
    return dna;
}

FACSFrame MetaEchoCodec::decodeRestFacs(const MetaEchoDNA& dna) const {
    FACSFrame facs;
    for (int i = 0; i < kFacsCount; ++i) {
        facs.au[i] = saturate(dna.genotype[8 + i]);
    }
    return facs;
}

MetaEchoDNA MetaEchoCodec::blend(const MetaEchoDNA& a, const MetaEchoDNA& b, float weight) const {
    const float t = saturate(weight);
    MetaEchoDNA out;
    out.name = t < 0.5f ? a.name : b.name;
    out.persona = t < 0.5f ? a.persona : b.persona;
    for (int i = 0; i < kDnaDim; ++i) {
        out.genotype[i] = lerpf(a.genotype[i], b.genotype[i], t);
    }
    for (int i = 0; i < kEchoDim; ++i) {
        out.persona.dimensions[i] = lerpf(a.persona.dimensions[i], b.persona.dimensions[i], t);
    }
    out.restFacs = decodeRestFacs(out);
    out.restRig = t < 0.5f ? a.restRig : b.restRig;
    out.fingerprint = fingerprint(out.genotype);
    return out;
}

} // namespace Avatar
} // namespace Ultima
