.. py:currentmodule:: rti.connextdds

Building and Installing
~~~~~~~~~~~~~~~~~~~~~~~

Building connextdds-py
======================

This section is for instructions on how to build and install connextdds-py.

Requirements
============

- macOS or Linux (Windows support coming soon)
- CMake 3.15+
- Python 2.7, Python3.x with pip or pip3 respectively
- RTI Connext DDS 5.3.1, 6.0.x

**Note: On certain platforms (ie Ubuntu 20.04 LTS), you must explicitly mention
what python version you are using. To do so, just use pip3 or python3 in place of
pip or python**

Simple Installation
===================

1. Clone the repo and submodules and enter the directory

.. code-block:: shell

    $ git clone --recurse-submodules https://github.com/rticommunity/connextdds-py.git
    $ cd connextdds-py

2. Set the environment variables

.. code-block:: shell

    $ export NDDSHOME=<install location of Connext Pro>
    $ export CONNEXTDDS_ARCH=<target arch>
    $ export LD_LIBRARY_PATH=<location of target libs>:$LD_LIBRARY_PATH
    $ export NJOBS=<number of concurrent make jobs, default 1>

3. Build and install

.. code-block:: shell

    $ pip install .

Development Installation
========================
If you want to cache the build files, you can use wheels.

1. Install the wheel package using pip.

.. code-block:: shell

    $ pip install wheel

2. Clone the repo and submodules and enter the directory

.. code-block:: shell

    $ git clone --recurse-submodules https://github.com/rticommunity/connextdds-py.git
    $ cd connextdds-py

3. Set the environment variables

.. code-block:: shell

    $ export NDDSHOME=<install location of Connext Pro>
    $ export CONNEXTDDS_ARCH=<target arch>
    $ export LD_LIBRARY_PATH=<location of target libs>:$LD_LIBRARY_PATH
    $ export NJOBS=<number of concurrent make jobs, default 1>

4. Run the setup script

.. code-block:: shell 

    $ python setup.py bdist_wheel

5. Install the package

.. code-block:: shell

    $ pip install dist/*


Uninstalling
============
If you wish to install a new version or update the development version,
you must uninstall the python package. To do so, run

.. code-block:: shell

    $ pip uninstall rti -y
