DataPath was designed from scratch to allow efficient use of modern architectures for large analytical queries.  DataPath makes full use of mult-cores, large amounts of memory, many disks. One of the more "exotic" features of DataPath is the fact that it can execute multiple-queries very efficiently, often about at the same speed of single query execution if the structure (but not details) are the same.

The initial DataPath system is described in the SIGMOD 2010 paper "The DataPath System: A Data-Centric Analytic Processing Engine" available from http://www.cise.ufl.edu/~sa2/pubs/paper537.pdf


Here are some documents you might be interested in:

  * [DeveloperGuide](DeveloperGuide.md)
  * [UserGuide](UserGuide.md)
  * [Performance](Performance.md)