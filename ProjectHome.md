## C Network library - netlib ##

View [README](http://qilfau.googlecode.com/svn/trunk/netlib/docs/readme)

### TODO: ###

  * Simulation of Network of queue system.
    * Sequential implementation: in testing progress but get the good result with 3-4 nodes (with one node is source, one node is sink, others are transist).
    * Parallel implementation: in testing phase. Work but speed up is not good (nearly similar the sequential algorithm with topology is a line. May be depend on topology - testing).
    * Routing strategies in each node: in developing (both sequential and parallel implementation)
    * Configuration file: reconstructing the structure of configuration file to support all-in-one used in configuring network of queueing system (In developing state)
  * Meta-heuristics search algorithm: in developing state