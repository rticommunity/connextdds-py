.. py:currentmodule:: rti.connextdds

Building and Installing
~~~~~~~~~~~~~~~~~~~~~~~

This section describes how to build and install the Connext Python API.

Requirements
============

- Linux®, macOS®, or Windows® (with Visual Studio® 2015 or newer)
- Python® 3.6 or newer
- A *Connext DDS* 7.0.0 host and target installation (see the `Connext Getting Started Guide <https://community.rti.com/static/documentation/connext-dds/7.0.0/doc/manuals/connext_dds_professional/getting_started_guide/index.html>`_ for instructions on how to install Connext).

.. note::

  Like any other Connext language API, the Connext Python API interoperates
  with other Connext versions. The required version above is **only to build**
  the Python API. Once built, you can use it to communicate with applications
  built on other Connext versions.

Simple Installation
===================

1. Clone the repository and submodules, and enter the directory.

  .. code-block:: shell

      $ git clone https://github.com/rticommunity/connextdds-py.git
      $ cd connextdds-py

2. Run configure.py script

  .. code-block:: shell

      $ python configure.py [options...] <platform>

  For example:

  .. code-block:: shell

      $ python configure.py --nddshome /opt/rti_connext_dds-7.0.0 --jobs 4 x64Linux4gcc7.3.0

  More options are listed later in this section.

3. Build and install.

  .. code-block:: shell

      $ pip install .

  This command will compile C++ code and install the Python package. Depending
  on your machine, it make take a while to complete. You can increase the number
  of parallel jobs for the compilation (``--jobs`` option used above).

4. The package is now installed and ready to use, you can import
   the module ``rti.connextdds`` and others. See :ref:`hello:Hello World`, or
   enter the following on the python interpreter:

   .. code-block:: shell

      $ python
      >>> import rti.connextdds as dds
      >>> participant = dds.DomainParticipant(100)
      >>> participant.domain_id
      100

Redistributable Wheel Build
===========================

In addition to installing the API on your machine, you can build a wheel file
that you can use to install it on other machines. The wheel works on machines
with the same processor and OS, and the same major Python version. For example,
you can build a wheel on a x64 Linux machine with Python 3.10 and then
install it on other x64 Linux machines with Python 3.10.

1. Clone the repository and submodules, and enter the directory.

  .. code-block:: shell

      $ git clone https://github.com/rticommunity/connextdds-py.git
      $ cd connextdds-py

2. Run configure.py script

  .. code-block:: shell

      $ python configure.py [options...] <platform>

3. Build the wheel.

  .. code-block:: shell

      $ pip wheel .

4. Install the wheel on your machine or other machines:

  .. code-block:: shell

    $ pip install rti.connext-<version>-<platform>.whl


Advanced configuration
======================

The configure.py takes additional options.

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


Uninstalling
============
If you wish to install a new version or update the development version,
you must uninstall the Python package. To do so, run:

.. code-block:: shell

    $ pip uninstall rti.connext -y
