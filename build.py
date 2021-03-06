#!/usr/bin/env python
# -*- coding: utf-8 -*-

from cpt.packager import ConanMultiPackager

if __name__ == "__main__":
    builder = ConanMultiPackager(archs=["x86_64"], build_types=["Debug"], build_policy="missing", docker_run_options='--cap-add SYS_PTRACE')
    builder.add_common_builds(pure_c=False)
    builder.run()
