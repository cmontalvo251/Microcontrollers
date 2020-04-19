Adafruit CircuitPython BusDevice
================================

.. image:: https://readthedocs.org/projects/adafruit-circuitpython-busdevice/badge/?version=latest
    :target: https://circuitpython.readthedocs.io/projects/busdevice/en/latest/
    :alt: Documentation Status

.. image :: https://img.shields.io/discord/327254708534116352.svg
    :target: https://discord.gg/nBQh6qu
    :alt: Discord

.. image:: https://github.com/adafruit/Adafruit_CircuitPython_BusDevice/workflows/Build%20CI/badge.svg
    :target: https://github.com/adafruit/Adafruit_CircuitPython_BusDevice/actions/
    :alt: Build Status

The ``I2CDevice`` and ``SPIDevice`` helper classes make managing transaction state
on a bus easy. For example, they manage locking the bus to prevent other
concurrent access. For SPI devices, it manages the chip select and protocol
changes such as mode. For I2C, it manages the device address.

.. _bus_device_installation:

On supported GNU/Linux systems like the Raspberry Pi, you can install the driver locally `from
PyPI <https://pypi.org/project/adafruit-circuitpython-busdevice/>`_. To install for current user:

.. code-block:: shell
    
    pip3 install adafruit-circuitpython-busdevice
    
To install system-wide (this may be required in some cases):

.. code-block:: shell
    
    sudo pip3 install adafruit-circuitpython-busdevice
   
To install in a virtual environment in your current project:

.. code-block:: shell

    mkdir project-name && cd project-name
    python3 -m venv .env
    source .env/bin/activate
    pip3 install adafruit-circuitpython-busdevice
    
Usage Example
=============

See examples/read_register_i2c.py and examples/read_register_spi.py for examples of the module's usage.

Contributing
============

Contributions are welcome! Please read our `Code of Conduct
<https://github.com/adafruit/Adafruit_CircuitPython_BusDevice/blob/master/CODE_OF_CONDUCT.md>`_
before contributing to help this project stay welcoming.

Documentation
=============

For information on building library documentation, please check out `this guide <https://learn.adafruit.com/creating-and-sharing-a-circuitpython-library/sharing-our-docs-on-readthedocs#sphinx-5-1>`_.
