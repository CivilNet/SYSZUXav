from setuptools import setup
import sys
def readme():
    with open('README.md') as f:
        return f.read()

version="0.1.1"
setup(name='syszuxav',
    version=version,
    description='python bindings for ffmpeg',
    long_description='python bindings for ffmpeg',
    keywords='ffmpeg python',
    url='https://github.com/CivilNet/SYSZUXav',
    download_url="https://github.com/CivilNet/SYSZUXav/releases/download/v0.1/syszuxav-0.1.1.tar.gz",
    author='Gemfield',
    author_email='gemfield@civilnet.cn',
    packages=['syszuxav'],

    install_requires=[
        'opencv_python',
        'numpy'
    ],
    classifiers  = [
        "Programming Language :: Python",
        "Programming Language :: Python :: 2",
        "Programming Language :: Python :: 3.5",
        "Topic :: Software Development :: Libraries"
     ],
    include_package_data=True,
    zip_safe=False)

