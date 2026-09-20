/**
 * MetaEchoDNA.h - Identity genotype wrapping Rig Logic + FACS.
 *
 *   /meta-echo-dna < /rig-logic + /facs >
 *
 * Encodes a stable Echo identity that later stages (Live2D-DTECHO, Meshy)
 * bind against, instead of treating each frame as a new character.
 */

#ifndef ULTIMA_AVATAR_META_ECHO_DNA_H
#define ULTIMA_AVATAR_META_ECHO_DNA_H

#include "avatar/RigLogic.h"

#include <array>
#include <cstdint>
#include <string>

namespace Ultima {
namespace Avatar {

struct MetaEchoDNA {
    std::string name;
    std::array<float, kDnaDim> genotype{};
    std::uint64_t fingerprint = 0;
    EchoState persona;
    FACSFrame restFacs;
    RigPose restRig;

    MetaEchoDNA() { genotype.fill(0.5f); }

    float get(int index) const {
        if (index < 0 || index >= kDnaDim) {
            return 0.0f;
        }
        return genotype[index];
    }
};

class MetaEchoCodec {
public:
    /**
     * Encode identity from a rest-pose (rig + facs) plus Echo persona.
     */
    MetaEchoDNA encode(const RigPose& rig,
                       const FACSFrame& facs,
                       const EchoState& persona,
                       const std::string& name) const;

    /**
     * Reconstruct rest FACS from the genotype (lossy, identity-stable).
     */
    FACSFrame decodeRestFacs(const MetaEchoDNA& dna) const;

    /**
     * Blend two identities. weight=0 keeps a, weight=1 becomes b.
     */
    MetaEchoDNA blend(const MetaEchoDNA& a, const MetaEchoDNA& b, float weight) const;

    static std::uint64_t fingerprint(const std::array<float, kDnaDim>& genotype);
};

} // namespace Avatar
} // namespace Ultima

#endif // ULTIMA_AVATAR_META_ECHO_DNA_H
