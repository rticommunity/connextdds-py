.. py:currentmodule:: rti.connextdds

Building and Installing
~~~~~~~~~~~~~~~~~~~~~~~

Building connextdds-py
======================

This section describes how to build and install connextdds-py.

Requirements
============

- Windows® (requires Visual Studio® 2015 or newer), macOS®, or Linux®
- Python® 2.7 or Python3.x with pip or pip3 respectively
- *Connext DDS* 5.3.1, 6.0.0, or 6.0.1
- `patchelf <https://github.com/NixOS/patchelf>`_ (Python 2.7.x on Linux)

.. note::

    On certain platforms (such as Ubuntu 20.04 LTS), you must explicitly mention
    what Python version you are using. To do so, just use pip3 or python3 in place of
    pip or python.

Configuration
===================

Before building/installing connextdds-py, it is necessary to run the configuration.py
script.

.. code-block:: shell

    $ python configure.py [options...] platform

Where "platform" specifies the RTI target architecture/platform string and the options are
specified in the following table.

.. list-table::
    :widths: auto
    :header-rows: 1

    * - Short Option
      - Long Option
      - Description
    * - -n NDDSHOME
      - --nddshome NDDSHOME
      - NDDSHOME directory. Defaults to NDDSHOME environment variable.
    * - -j JOBS
      - --jobs JOBS
      - Number of concurrent build jobs/processes
    * - -t
      - --tcp
      - Add the TCP transport plugin
    * - -m
      - --monitoring
      - Add the RTI Monitoring plugin
    * - -s
      - --secure
      - Add the RTI DDS Secure plugin + openssl libraries
    * - -p PLUGIN
      - --plugin PLUGIN
      - Add a user-defined plugin. This option can be specified multiple times
    * - -o OPENSSL
      - --openssl OPENSSL
      - Location of openssl libraries (defaults to platform library location under NDDSHOME)
    * - -r DIR
      - --python-root DIR
      - Root directory of Python (prefers 3.x over 2.x if both are under root)
    * - -c FILE
      - --cmake-toolchain FILE
      - CMake toolchain file to use when cross compiling
    * - -d
      - --debug
      - Use debug libraries and build debug modules for connext-py
    * - -h
      - --help
      - Show help message and exit

Simple Installation
===================

1. Clone the repository and submodules, and enter the directory.

.. code-block:: shell

    $ git clone https://github.com/rticommunity/connextdds-py.git
    $ cd connextdds-py

2. Run configuration.py script

.. code-block:: shell

    $ python configure.py [options...] <platform>

3. Build and install.

.. code-block:: shell

    $ pip install .

Simple Wheel Build
==================

1. Clone the repository and submodules, and enter the directory.

.. code-block:: shell

    $ git clone https://github.com/rticommunity/connextdds-py.git
    $ cd connextdds-py

2. Run configuration.py script

.. code-block:: shell

    $ python configure.py [options...] <platform>

3. Build the wheel.

.. code-block:: shell

    $ pip wheel .

Development Installation
========================
If you want to cache the build files, you can use wheels.

1. Install the required Python modules:

.. code-block:: shell
    :caption: Windows

    $ pip install setuptools wheel cmake pybind11==2.7.1

.. code-block:: shell
    :caption: Linux

    $ pip install setuptools wheel cmake patchelf-wrapper pybind11==2.6.1


.. code-block:: shell
    :caption: macOS

    $ pip install setuptools wheel cmake delocate pybind11==2.6.1

2. Clone the repository and submodules, and enter the directory.

.. code-block:: shell

    $ git clone https://github.com/rticommunity/connextdds-py.git
    $ cd connextdds-py

3. Run configuration.py script

.. code-block:: shell

    $ python configure.py [options...] <platform>

4. Run the setup script.

.. code-block:: shell

    $ python setup.py bdist_wheel

5. Install the package.

.. code-block:: shell

    $ pip install dist/*


Cross Compilation
=================
If you want to build for a different architecture, the configuration and build scripts 
provide some options for doing so. The repository contains an example CMake toolchain
file at resources/cmake/ExampleToolchain.cmake for use on Linux hosts to build a wheel
for an armv7l target (such as a 32-bit Raspberry Pi) with Buildroot. The following
steps assume familiarity with cross-compilation toolchains and that you have a cross
compiled version of Python for the target.

1. Install the required Python modules:

.. code-block:: shell
    :caption: Linux

    $ pip install setuptools wheel cmake patchelf-wrapper pybind11==2.6.1

2. Clone the repository and enter the directory.

.. code-block:: shell

    $ git clone https://github.com/rticommunity/connextdds-py.git
    $ cd connextdds-py

3. Modify the ExampleToolchain.cmake file with the appriate host and target values

4. Run configuration.py script, making sure to point to the cross-compiled Python root 
   directory and the CMake toolchain file

.. code-block:: shell

    $ python configure.py --python-root <cross-compiled Python root> --cmake-toolchain <toolchain file> [options...] <target platform>

4. Run the setup script, providing version and architecture information.

.. code-block:: shell

    $ python setup.py bdist_wheel --py-limited-api cp37 --plat-name linux_armv7l

5. Install the wheel package on the target.


Uninstalling
============
If you wish to install a new version or update the development version,
you must uninstall the Python package. To do so, run:

.. code-block:: shell

    $ pip uninstall rti -y
