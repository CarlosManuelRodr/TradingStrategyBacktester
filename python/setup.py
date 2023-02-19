import sys
from distutils.core import setup
from setuptools.dist import Distribution

if sys.version_info < (3, 0):
    sys.exit('Error: Python < 3.0 is not supported.')


class BinaryDistribution(Distribution):
    """Distribution which always forces a binary package with platform name"""
    @staticmethod
    def has_ext_modules():
        return True


setup(
    name='TradingStrategyBacktester',
    version='0.1',
    package_dir={},
    package_data={
        '': ['TradingStrategyBacktester.*.so']
    },
    distclass=BinaryDistribution
)
