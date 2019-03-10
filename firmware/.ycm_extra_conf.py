# based on https://github.com/Valloric/ycmd/blob/master/.ycm_extra_conf.py

import ycm_core
import os

def FlagsForFile(filename, **kwargs):
    return {
            'flags': [
                '--std=c99',
                '-Wall',
                '-Wextra',
                '-Wpedantic',
                '-ffunction-sections',
                '-fdata-sections',
                '-mmcu=attiny85',
                '-DF_CPU=16500000L',
                '-DE2END',
                '-I.',
                '-Iusbdrv',
                '-Ilibs-device',
                '-I/usr/lib/avr/include',
                ],
            'include_paths_relative_to_dir': os.path.dirname(os.path.abspath(__file__)),
            'do_cache': True
            }

