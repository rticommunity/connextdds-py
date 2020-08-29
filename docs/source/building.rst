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

.. note::

    On certain platforms (such as Ubuntu 20.04 LTS), you must explicitly mention
    what Python version you are using. To do so, just use pip3 or python3 in place of
    pip or python.

Simple Installation
===================

1. Clone the repository and submodules, and enter the directory.

.. code-block:: shell

    $ git clone --recurse-submodules https://github.com/rticommunity/connextdds-py.git
    $ cd connextdds-py

2. Set the environment variables.

.. code-block:: shell

    $ export NDDSHOME=<install location of Connext Pro>
    $ export CONNEXTDDS_ARCH=<target arch>
    $ export NJOBS=<number of concurrent make jobs or VS build processes, default 1>

3. Build and install.

.. code-block:: shell

    $ pip install .

Simple Wheel Build
==================

1. Clone the repository and submodules, and enter the directory.

.. code-block:: shell

    $ git clone --recurse-submodules https://github.com/rticommunity/connextdds-py.git
    $ cd connextdds-py

2. Set the environment variables.

.. code-block:: shell

    $ export NDDSHOME=<install location of Connext Pro>
    $ export CONNEXTDDS_ARCH=<target arch>
    $ export NJOBS=<number of concurrent make jobs or VS build processes, default 1>

3. Build the wheel.

.. code-block:: shell

    $ pip wheel .

Development Installation
========================
If you want to cache the build files, you can use wheels.

1. Install the required Python modules:

.. code-block:: shell

    $ pip install setuptools wheel cmake

2. Clone the repository and submodules, and enter the directory.

.. code-block:: shell

    $ git clone --recurse-submodules https://github.com/rticommunity/connextdds-py.git
    $ cd connextdds-py

3. Set the environment variables.

.. code-block:: shell

    $ export NDDSHOME=<install location of Connext Pro>
    $ export CONNEXTDDS_ARCH=<target architecture>
    $ export NJOBS=<number of concurrent make jobs or VS build processes, default 1>
    $ export DEBUG=<1 for debug build, unset or any other value for release build>

4. Run the setup script.

.. code-block:: shell

    $ python wheel .

5. Install the package.

.. code-block:: shell

    $ pip install dist/*


Uninstalling
============
If you wish to install a new version or update the development version,
you must uninstall the Python package. To do so, run:

.. code-block:: shell

    $ pip uninstall rti -y
