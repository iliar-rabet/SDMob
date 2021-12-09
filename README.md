This repository serves as the implementation for the following papers:

[PDF](SDMob.pdf)

To run the SDMob, you will need to install the mobility plugin for Cooja
[Mobility plugin] https://anrg.usc.edu/contiki/index.php/Mobility_of_Nodes_in_Cooja

In the examples you will find, a bunch of *.csc files. For example, 3tlw.csc runs 3 mobile nodes based on Sky platform (broadcast/broadcast-example.c) moving by TLW mobility pattern - 1 slip radio (slip-radio/slip-radio.c) that connects the cooja environment to the external border router and 25 anchor nodes (anchor/anchor-downward-v0.c).

For the border router you can run the example "sock" in native mode (not in cooja but in linux) by the following command:

sudo ./border-router-udp-server.native fd00::1/64 -a 127.0.0.1

You can also run different versions of the filter using the python script named particle-v1.py using python3. Make sure to configure the script to have the same mobility trace file as given to the cooja mobility plugin.

-----------
