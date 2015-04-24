# Overall Plan #

To add a new waypoint to DataPath, 4 separate tasks need to be accomplished. The parser needs to be changed, the translator needs to know how to include the new waypoint into analysis, the WayPoint code that runs in the execution engine needs to establish the data flow while the operator is running and the patterns for code generation have to specify the code that is executed by the workers.

Throughout this document we use the GLAWaypoint as a running example. This waypoint adds GLA execution capabilities to DataPath. GLAs are a generalization of User Defined Aggregates. Any GLA type is assumed to have the following methods:
  * A constructor, with at most 1 argument: `ConstState&`
  * `AddItem(const T1& v1, const T2& v2, ...)`, a method to add a piece of information to the state of the GLA.
  * `AddState(GLA& other)`, a method to merge content of other into this state
  * Optionally, a mechanism to extract result in the form 1 tuple: through the function `GetResult(T1& t1, T2& t2, ...)`
  * Optionally, a mechanism to extract, 0 or more tuples: `bool GetNext(T1& v1,...)`
  * Optionally, a method to serialize-deserialize the GLA so that it can be transported into user space

**Example of GLA**: Average

```
class AverageGLA {
  long long int count; // keeps the number of tuples aggregated
  double sum; // sum of the values 
public:  
  AverageGLA(){ count=0; sum=0.0; }
  AddItem(const DOUBLE& x){ count++; sum+=x; }
  AddState(AverageGLA& o){ count+=o.count; sum+=o.sum; }
  // we only support one tuple as output
  GetResult(DOUBLE& rez){ rez = (count>0) ? (sum/count) : 0.0;}
};

```

**NOTE**: GLAs are used as syntactic objects. There is no need to inherit from any class since the system uses code generation to produce the functions that are executed. The GLAs should be written as compactly/efficiently as possible.

**WARNING**: Inheritance and other C++ abstractions are **evil** and kill performance.

## Changes to the parser ##

The parser is in `LemonTranslator/parser` directory. We need to decide how to extend the syntax of of the .dp files. A good starting point is the syntax for aggregates, that is close in spirit. We first need to specify connectivity and type. Like the Aggregate, GLA waypoints are non-terminating.

```
new scanner lineitem;
new waypoint ll = Select :-lineitem;
new waypoint gla = GLA :- ll; 
new waypoint pr = Print :- ?gla;
```

The above code specifies that we are  reading `lineitem`, apply selection than send tuples to the GLA. Once done, we print the result. This means that the GLA should produce tuples. If we want to move the GLA state to the users space, we can introduce a new waypoint that allows that

```
new waypoint st = TransmitState :- ?gla;
```

For the specification of the behavior of the GLA waypoint, we can use something like this:

```
new query Q1, Q2;
waypoint gla {
  query Q1 { 
    gla (r:DOUBLE) = AverageGLA( lineitem_l_discount );
  }
  query Q2 {
    gla (a:DOUBLE,b:INT,c:VARCHAR) = MyGla( lineitem_l_discount, lineitem_l_comment); 
  }
}

waypoint pr {
  query Q1 { print r; }
  query Q2 { print a,b,c; }
}
```

The line `gla (r:DOUBLE) = AverageGLA( lineitem_l_discount );` specifies that for `Q1` we have to use GLA `AverageGLA` for which the `AddItem()` function takes as argument expression `lineitem_l_discount`. The GLA will produce a tuple that consists of a single `DOUBLE`, called `r`. A larger set of expressions and a larger set of results can be obtained.

**NOTE** each query can have a different behavior. The system should allow that but it might be useful to have very different behavior. By using different GLAs for different queries, we can evaluate multiple things over the scan of the input. This could significantly improve the performance. Different things should go in different waypoints.

The code above is sufficient to allow the correct code generation as long as the GLAs used are compatible. To perform a type check, we need a GLAManager and type specifier. This is discussed in another place [GLAManager](GLAManager.md).

To change the parser we need to add code in  `LemonTranslator/parser/DataPath.g` to specify the new syntax. Most of the hard work is already done, we can simply just mimic the behavior of aggregate. The code in `DataPath.g` must call the API that we still need to introduce to specify GLA waypoints.

**NOTE**: Expressions, list of attributes, etc, are already supported. We can just reuse the facility. The only slightly different feature is specification of the form `a:DOUBLE` that specify a name and a type.

The only change needed in `LemonTranslator/parser/DPTree.g` is the specification of the content of the GLA waypoint. Again we can mimic the behavior of Aggregate.

## Changing LemonTranslator ##

  1. Add GLA specific API in `LemonTranslator/headers/LemonTranslator.h`
`bool AddGLAWP(WayPointID glaWP)` that specifies the new waypoint type, and `bool AddGLA(WayPointID wp, QueryID query, ..)` that specifies the behavior of the GLA. The arguments we pass should specify the list of outputs and the list of expressions for inputs. The Aggregate allows only 1 input and 1 output. After some thought, we realize we can use the following definition:
```
bool AddGLA(WayPointID wp, QueryID query, SlotContainer& resultAtts, 
            string glaType, SlotContainer& atts, string expr, string initializer = "");
```
`resultAtts` will be the list of attributes produced as the result, empty if none; `glaType` will be the name of the GLA; `atts` the list of attributes in `expr`; `expr` the comma separated list of expressions forming the input.

The meaning of `atts`, `expr`, `initializer` is the same for GLAs as it is for either Print or Aggregate waypoints.


  1. Add code in `LemonTranslator/source/LemonTranslator.cc` (mimic Aggregate)

  1. Add a new class in `LemonTranslator/headers`: `LT_GLA` in file `LT_GLA.h`. This class is responsible for storing the information about the GLA waypoint (on a per/query basis), to participate in the analysis that establishes the use of attributes throughout the system and to generate the `.m4` file that _programs_ the code generator. Again, mimicking either the Aggregate or the Print saves a lot of frustration.
The implementation should be placed in `LemonTranslator/source/LT_GLA.cc`
This is the file where the configuration object used by the GLA waypoint is constructed as well. Implementation depends on the discussion we will have about this waypoint next.

## Add code for GLA Waypoint ##

The code for GLA waypoint goes in `WayPoints` module. Because of the _swapping paradigm_ the waypoints are written in ..Imp files. In particular, we need to write `WayPoints/headers/GLAWayPointImp.h` and 'WayPoints/source/GLAWayPointImp.cc`. The interface should be identical to the other waypoints (copy the `AggWayPointImp.h` content). The members of the `GLAWayPointImp` class can be anything we need to use in this waypoint.

The crucial thing we have to decide at this point is the type of functions the GLAWayPoint will posses. These are the functions called by the Worker threads to do work. The code in the GLAWayPointImp class only deals with decision making/scheduling.

**NOTE**: DataPath enforces a complete separation between planing and execution. Furthermore, to ensure performance, the functions should be generated using the code generator. This means that that the code in the GLAWayPointImp does not understand any spepcifics about the GLAs used at runtime. Since we do not want to use virtual methods to manipulate GLAs, we have to ensure that any manipulation happens exclusively in the functions, not in the `GLAWayPointImp` class. The way we will deal with that is to introduce a `GLAPtr` class and the companion `GLAPtrContainer` that holds a pointer to a GLA but it cannot do anything with the pionter.

A good set of functions might be:

  1. `int ProcessChunk(WorkDescription &workDescription, ExecEngineData &result)`

This function will scan a chunk, unpack the tuples and call AddItem for the corresponding GLA. As a result it produces nothing. It returns 1 (we use this in the implementation of the waypoint to know that this function was processed).

All work specification to a work function in DataPath is specified by building a WorkDescription object. The way to implement a new one is to add such a type in `ExecutionEngine/m4/WorkDescription.h.m4`. Such files (see [data](data.md) ) contain specification of Data objects that are exchanged in the system. We an call the work type for this function `GLAProcChunk` and write:
```
M4_CREATE_DATA_TYPE(GLAProcChunk, WorkDescription,
<//>,
</(whichQueryExits, QueryExitContainer), (glaPtr, GLAPtr), (chunkToProcess, Chunk)/>)
```

The work function will get a `glaPtr`, that contains the state of an existing GLA. If the `glaPtr` is empty, we can produce a GLA. To be able to return this new glaPtr, we need a derived type from `ExecEngineData` to add the capability. We can do so by adding to file `ExecutionEngine/m4/ExecEngineData.h.m4`:
```
M4_CREATE_DATA_TYPE(GLAPtrRez, ExecEngineData,
<//>,
</(glaPtr, GLAPtr)/>)
```
If we need to send more info to the GLAWayPoint, we can add it here.

  1. `MergeStates(...)` function will take as an argument a set of states and return a single, merged, state. We need to introduce a new type of work description, we can produce a GLAPtrRez as above.

  1. `Finalize(...)` will take as argument a glaPtr and extract tuples from it. The resulting chunk is sent to the system as a ChunkContainter.

**NOTE**: It is tempting to destroy the glaPtr at this point since a chunk was produced from it. One type of behavior that any new WayPoint needs to enforce is dealing correctly with chunk drops. If the chunk is droped, there is no way to produce it again. We need a separate function to destroy GLAs

  1. `DestroyGLA(...)`. Get a GLAPtr and delete it. Once the chunk produced is acknowledged, the GLAWayPoint can call this function.

All the work functions we need are specified in the file `ExecutionEngine/m4/WorkFuncs.h.m4`. What we need is:
```
// GLA
M4_CREATE_DATA_TYPE(GLAProcessChunkWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GLAFinalizeWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GLAMergeStatesWorkFunc, WorkFuncWrapper, <//>, <//>)
M4_CREATE_DATA_TYPE(GLADestroyWorkFunc, WorkFuncWrapper, <//>, <//>)
```







