#!/usr/bin/env python3
"""The modularity note names the five seams the evaluation recorded."""

import os
import unittest

NOTE = os.path.join(
    os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))),
    "docs",
    "u8_map_modularity.md",
)

HEADINGS = (
    "shared seam",
    "engine-specific seam",
    "osm adapter",
    "procedural adapter",
    "pentagram load blockers",
)


class TestModularityNote(unittest.TestCase):
    def test_note_has_five_headings(self):
        text = open(NOTE, encoding="utf-8").read().lower()
        for heading in HEADINGS:
            self.assertIn(heading, text)


if __name__ == "__main__":
    unittest.main()
