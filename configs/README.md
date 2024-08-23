# Clock Configurations

This folder contains various predefined clock configurations. The full spectrum of configuration options along with instructions on generating the configuration header file can be found [here](../README.md).

To use a prebuilt configuation, copy the corresponding file to the root directory containing the source files as `.config` and run `make config`.

Example:

```sh
cp configs/avr-nano-iso-metric-normal.config .config
make config
```
