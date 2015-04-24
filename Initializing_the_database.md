# Initialization Instructions #

We describe here three scenarios.

## Simple installation, 1GB database ##

  * Compile the code
  * Go into directory src/Tool\_DataPath/executable
  * run "./dp" and answer the questions -- this initializes the system
    * What is the page muliplier exponent of the array?
      * 1 -- this is always the best option for spindle disks. Only large SSDs need 2-3
    * The disk array can use multiple stripes. How many stripes should we use?
      * 1 -- use only one disk/stripe
    * Pattern for the stripes fies.
      * stripe -- this creates a file called stripe that has the data
    * At this point the database is created.
  * run "./dp Queries/TPCH-1G/types.dp"
    * this tells the system about the data types
  * run "./dp Queries/TPCH-1G/tpch.dp"
    * this tels the system about the TPC-H schema
  * to generate 