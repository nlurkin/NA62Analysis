#!/usr/bin/env python

import sys

req_version = (2,6)
cur_version = sys.version_info

if cur_version < req_version:
	print """
This script is only working with python2 > 2.5. Please use a more recent version of python.
On LXLPLUS you can choose a version from /afs/cern.ch/sw/lcg/external/Python/ and add it in front or your PATH.
"""
	sys.exit()

import scripts.NA62AnalysisBuilder
scripts.NA62AnalysisBuilder.main()

