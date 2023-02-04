from distutils.core import setup

import sys

if sys.version_info < (3, 0):
    sys.exit('Error: Python < 3.0 is not supported.')

setup(
    name='TradingStrategyBacktester',
    version='0.1',
    packages=['TradingStrategyBacktester'],
    package_dir={
        '': '${CMAKE_CURRENT_BINARY_DIR}'
    },
    package_data={
        '': ['TradingStrategyBacktester.cpython-310-x86_64-linux-gnu.so']
    }
)
