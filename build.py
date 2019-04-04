#!/usr/bin/env python
# -*- coding: utf-8 -*-

from cpt.packager import ConanMultiPackager

if __name__ == "__main__":
    builder = ConanMultiPackager(archs=["x86_64"], build_types=["Debug"])
    builder.add_common_builds(pure_c=False)
    builder.update_build_if(lambda build: build.settings["compiler"] == "clang" and build.settings["compiler.version"] <= "4.0",
                            new_settings={"compiler.libcxx": "libc++"})
    builder.run()