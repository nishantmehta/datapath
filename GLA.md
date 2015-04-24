# GLA : Up Close And Personal #

# Introduction #

With the advent of MapReduce many heads have shifted to the NoSql approach for  large scale data analysis. It has its merits as well as a number of demerits as pointed out in many papers already published. GLA is a computational model that has come out of University of Florida, conceptualized by Dr. Florin Rusu and Dr. Alin Dobra. It can be termed as a NoSql approach similar to MapReduce in concept but the model is so natural and flexible that it can be used to leverage all the advantages of a traditional database system.


# In Detail #

## GLA and its Form ##

Generalized Linear Aggregate can be defined as an Abstract datatype. It has a state and a set of functions to explore and transform the state. The state can be anything ranging from a simple integer to as complex as a hash table that maps a complex set of words to a heap. The main functions defined by the model are the following:

i) _Initialize()_ : is used to setup the state appropriately before any computation begins. It is similar to a constructor in the common high level languages.

ii) _AddItem(Tuple t)_ : Tuple here designates a sequence of fields which can be of any type and can range from one element to a number of elements. _AddItem_ essentially adds the tuple t to the current state that it is maintaining. Tuple can be inserted from a relational database or can be read from a file.

iii) _AddState(GLA &other)_ : combines the state maintained by one GLA of the same type with another. _AddState_ is made associative in nature for computational efficiency, so that there does not have to be any order to be maintained for combining the states.

iv) _Finalize()_ : in its original conceptual form was used to infer the final results from the state maintained by the GLA.

v) _Serialize()/Deserialize()_ : These are optional functions which are necessary in a shared nothing architectural system for communicating the states between nodes.


## A Simple Example ##

Lets now look at a simple example to calculate the average of a large set of numbers. What you essentially need to maintain as the state here is the _sum of all numbers_ and their _count_.
GLA utilizes a form of data level parallelism that necessitates the division of the input to smaller chunks. So given the current scenario in a shared memory subsystem, there will be as many GLA's as there are cores in the system. Each GLA operates in parallel utilizing some form of a parallel threading mechanism. Each of the GLAs are given a chunk to execute and when it finishes execution, it asks for a new chunk. So the state grows in parallel and can be aggregated using an aggregation tree or any form of aggregation as required by the user.
Now towards the Average example :

**State** :

a) Sum
b) Count

**Functions**:

i) _Initialize()_ : initialized Sum and Count to 0.

ii) _AddItem(Tuple t)_ : Tuple here refers to one of the numbers from the set of numbers. _AddItem_ adds the number to the Sum and increments the count.
```
 Sum += t;

 ++Count;

```

iii) _AddState(GLA &other)_ : It combines the Sum and Count of the two GLA's essentially adding up the states. The _AddState_ has become associative as well without any additional costs, which is the case usually even for the most complex GLA's.
```
     Sum += other.Sum;
     Count += other.Count;
```

iv) _Finalize()_ :  Here, Finalize infers the average from the maintained statistics, which can be done by a single division.

```
 Average = Sum / Count;
```

## So how is it different from MapReduce? ##

So you have seen a GLA in concept and also in action in the form of an example. Now, those familiar with MapReduce must be wondering how this is different from the more popular model. There is an AddItem similar to Map and an AddState similar to Reduce.

i) For one, it is highly flexible and natural in comparison with MapReduce. It does not enforces any restrictions on the data that it handles like maintaining the key value pair as done by MapReduce. The key value pairs in some computations are a boon and in some are a curse. Data is most often not available in this format and requires some manipulation to transform it to this form. This ends up in creating numerous unwanted artifacts and also extra computational overhead.

ii) Secondly, key value pairs are also used to manage distribution of data based on the key in Map Reduce. Here the associative nature of GLAs and an efficient chunking mechanism plays a large part in distribution. AddState() enables you to add the states up in any manner facilitating efficient management of data. Chunking enables the division of the input to smaller pieces which can be handled by the GLAs. There is no restriction to the amount of computation that you need to do in a single pass. Therefore you can focus on building a part of the final state serialize it to disk and continue with the rest. This format is particularly useful for distributing data as well for handling large states.

iii) Localization of Aggregation - Each Mapper produces data for every reducer in a MapReduce paradigm. Therefore, there is a lot of traffic on the interconnecting networks. In GLAs as mentioned earlier we can use an aggregation tree to combine GLAs thereby getting better localization on the network.

## GLA and Datapath ##

The mode of execution of a GLA as mentioned earlier during the example is as simple as it gets. GLA essentially operates on a framework called GLADE implemented on top of Datapath. Datapath is a data centric data processing engine. It is push based not pull based and focuses on finding computation for data rather than data for a computation. As introduced earlier data circulates through datapath in the form of chunks. GLADE automates all the process of resource management and task management for GLAs, utilizing all the computational abilities of Datapath.

**More to Come**

## Further Reading ##
Florin Rusu and Alin Dobra. GLADE: [A Scalable Framework for Ecient Analytics](http://faculty.ucmerced.edu/frusu/Papers/Conference/2011-ladis-glade.pdf). Proceedings of the 2011 LADIS Workshop on Large Scale Distributed Systems and Middleware (LADIS 2011) held in
conjunction with the VLDB Conference on Very Large Databases (VLDB 2011), Seattle, Washington, September 2011


The DataPath System: [A Data-Centric Analytic Processing Engine for Large Data Warehouses](http://www.cise.ufl.edu/~cjermain/p519-arumugam.pdf), Subi Arumugam, Alin Dobra, Chris Jermaine, Niketan Pansare, Luis Perez, Proc. SIGMOD 2010