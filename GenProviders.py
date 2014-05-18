#!/usr/bin/python

import glob

HEADER = """#ifndef PROVIDERS_H
#define PROVIDERS_H

#include "StreamProvider.h"

"""

FOOTER = """
#endif
"""

OUT_PATH = 'Providers.h'
SUFFIX   = 'StreamProvider.h'

EXCEPTIONS = [SUFFIX,
              'SetOperation' + SUFFIX]

def GenProvidersHeader():
    handle = open(OUT_PATH, 'w')
    handle.write(HEADER)
    paths = glob.glob('*' + SUFFIX)
    for p in paths:
        if p not in EXCEPTIONS:
            handle.write('#include "%s"\n' % p)
    handle.write(FOOTER)

if __name__ == '__main__':
    GenProvidersHeader()