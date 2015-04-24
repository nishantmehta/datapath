# Introduction #

DataPath is a data processing engine designed from ground up to fully utilize modern hardware for large analytical queries. With DataPath, it is possible to process in minutes 1TB dataset and in minutes to hours 10TB datasets with 20,000-40,000$ hardware investment.

# Design Philosophy #

When designing DataPath, we made it a point to start with a clean slate. We strived to avoid reusing either code or ideas from traditional database systems. This allowed us to reconsider the design of a data processin engine from ground up, to try to make the best use of modern architectures without any attempt to salvage any existing effort.

The major design considerations for DataPath were:

  1. **Truly multi-threaded execution engine**: The engine should fully utilize multi-core systems even if the query involves few operators. Take for example a query like Q1 in TPC-H. It only involves a scan of `lineitem`, a selection on date and an group by with aggregate. Any implementation in a relational engine would use 2 or 3 operators (depending on whether the scan is performed through an index). To take advantage of a system with 16-64 cores, both the selection and the aggregate should be multi-threaded and make use of many cores.
  1. **Data driven**: The engine should focus on the data and data movement rather than the computation. The main reason for this is the realization that memory bandwidth is the most precious resource. A direct consequence of this is the fact that, once the data is accessed, as much computation as possible should be performed. When running multiple queries, the system should perform work on behalf of all queries that could use the data, thus saving memory bandwidth. This is a generalization of the _shared scan_ ideas in databases; not only the data from the disk should be shared but the access of the data in memory.
  1. **No/little tuning**: Database tuning is a truly complicated endeavor and a large industry as a result of this. It is not uncommon for a skilled DBA to speed up query processing by a factor of 10X or more by careful tuning of the database parameters. While possible, such tuning is too expensive for most users that cannot afford the high cost of such a skill. Even if it is possible to get reasonable performance at 1TB with a 20,000$ system, the cost of tuning  and of the software would significantly raise the cost. This is one of the main reasons map-reduce solutions are sometimes preferred. DataPath strives to require very little tuning, most of which can be automated.  For the most part, the system should figure out how to set itself up to run fast.
  1. **High-performance**: Any system strives to achieve high-performance, making this a default goal. In DataPath, we identified a concrete way to achieve this for large queries: on-the-fly code generation. DataPath will generate _custom_ C++ code to execute the query workload, code that is devoid of data-structures and any interpretation. Effectively, all actions are hard-wired and the code is similar to expertly written custom code. With a cost of 2-5s for generation and compilation, the code executed by DataPath can be 10-100X faster than code of other database or data processing systems.
  1. **Asynchronous**: Any linearity in how modern hardware is used leads to inefficient resource utilization. Barrier-like synchronization makes the code potentially simpler but reduces efficiency. To fully utilize the resources, the design of the system has to be asynchronous in all aspects. This consideration significantly influenced the  software engineering aspect.

# Software Engineering and Coding #

From a software engineering point of view, DataPath is atypical. It is written in a combination of C++ and m4, it provides a complete separation between planning and execution, uses the _actor model_ (somewhat similar to Erlang but more pragmatic) and uses large amount of _meta programming_.  To understand these decisions, some discussion is in order.

## The Convenience-Performance Tradeoff ##

When it comes to software engineering, in most situations there is a tradeoff between convenience and performance. To allow high-performance, languages like C allow little convenience, especially in terms high-level features. The programmer has full control but relatively little support from the language. At the other extreme, languages like Erlang and ML, offer powerful abstractions that allow ease of use, simple semantics, protection against silly errors using strict type systems, etc. What they do not provide is control, when needed. Allowing both in a single language turns out to be tricky.

On the surface, C++ offers both convenience and performance. While some of the issues are taken care of such as a powerful type system, the full use almost always leads to complex code and possibly a loss of performance due to abstraction. For the most part, C++ lacks economy of means, is over complicated, has a lot of hidden abstraction penalties and, to a large extent, has insufficient support for high-level programming. If a small subset of C++ with added mechanisms is used, there is hope that both convenience and performance could be achieved. C++ is a better starting point since convenience is easier to add then performance. Once lost, performance is very hard to re-acquire.

When developing DataPath, we realized that C++ is one of the few languages that will allow the control we need to bypass any unsuitable mechanism -- there are a lot of such bypasses in DataPath. At the same time, we recognized that we need to build  number of convenience _features_ to make C++ bearable, especially for a project developed by a small team.

## The evils of explicit memory allocation ##

One of the best features of C/C++ type language is the explicit memory allocation through `malloc/free` or `new/delete` mechanism. This is where most of the headaches come from as well. The problem is exacerbated when multi-threading is used since responsibility for deleting objects is harder to establish. If not carefully controlled, very subtle bugs creep into the system. Using deleted objects leads to hard to track bugs. Not deallocating memory leads to memory leaks. A _garbage collector_ solves the problem but takes away the control. _Smart pointers_ with reference counts might mitigate some of the problems, but not all.

A related problem is the fact that, if explicitly allocated, objects need to be manipulated through pointers rather than local variables. This makes the code less legible and strongly encourages hacks. Converting pointers to `void*` is way too tempting. The question is **can we have our cake and eat it too?**.

## Swapping Paradigm ##

A relatively unknown, undeservedly so, programming paradigm is the use of swapping to avoid aliasing. These ideas are quite old but have been mostly championed by Bruce Weide at Ohio State through the **Resolve/C++**.  The main problem that the swapping paradigm tackles is eliminating _aliasing_. By doing so, a lot of headaches are eliminated. As a bonus, swapping paradigm, as we explain below, simplifies multi-threaded programming.

Aliasing refers to the fact that, in most languages, it is easy to create multiple names/handles for the same object. Due to this disconnect between name and object, is is easy to loose track of who has access to the object. This makes it hard to control when the object can be destroyed or, worse, whether we need synchronization to ensure consistent multi-threaded manipulation of the object. Immutable datatypes_and garbage collectors were introduced to combat these problems. They work well for high-level languages but they do not allow any type of control, when needed. If we could guarantee that objects are not aliased, all these problems would disappear; no need for garbage collectors or immutable datatypes._

Swapping paradigm solves completely the aliasing problem. Each object has single ownership, thus a single handler. When this only handler is destroyed, the object is destroyed. Moreover, whoever has the handler, has exclusive access to the object, thus there is no need for any synchronization.  How then can we allow somebody else to access an object we own; this is strictly needed if we want to delegate work. The answer is by swapping.  Under swapping paradigm, two operations are supported. First, is the creation of _empty/bogus_ objects, second the the `swap`. To see how this works consider the following piece of code:

```
void StoreObject(Object& object){
  Object myObject; // create empty object
  myObject.swap(object); // exchange content
  // object now contains my bogus content, myObject has the real content
  // caller has lost ownership, the real object is mine
}

void GetObject(Object& object){
  myObject.swap(object); // give back the object
}
```

Under swapping paradigm, objects behave more like real objects. If you need to use something I have you can borrow it and give it back latter. By banning _copy constructors_  and requiring an explicit `copy()` that duplicates an object, if needed, we can get rid of explicit memory manipulation for long lived objects. The implementation of `Object` will create the content/real object through constructors. The destructor will delete the real object, if not bogus. In terms of efficient implementation, we can use two classes in the following way:

```
class ObjectImp; // forward declaration of real object type
// frontend class. Makes object look like local variable
class Object{
  private:
    ObjectImp* pObject; // pointer to real object
    // deactivate copy constructor
    Object(const Object&)

  public:
   // construct real object from int 
   Object(int i){ pObject = new ObjectImp(i); }
   // construct bogus/empty object
   Object(){ pObject = NULL; }
   // method call
   void Process(double x){ if (pObject!=NULL) pObject->Process(x); }
   // destructor
   ~Object(){ if (pObject!=NULL) delete pObject; }
};

// backend class, real object
class ObjectImp {
  private:
    int myI; // private state
  public:
    // constructor from int  
    ObjectImp(int i){ myI=i; }
    // no need for default constructor
    
    // method
    Process(double x){ myInt*=x; }

    // no need for destructor
  
};
```

Using this type of implementation, swapping is very efficient. Even though there is a level of indirection, the compiler will do a very good job at optimizing the code and masking this. The only truly annoying thing is the fact that we need to write 2 classes instead of 1.

DataPath uses the above technique extensively.  Alternatives are possible to avoid the 2-class strategy;  copy constructor is deactivated and `swap` method is provided for all objects that are exchanged between modules. By deactivating the copy constructor, any misuse of the object (from the swapping paradigm point of view) is caught by the compiler.

**NOTE**: New developers should resist the temptation to bypass the _limitations_ of the swapping paradigm.  There is almost never any efficiency lost and the code will have far fewer defects. In fact, due to the extensive use of  swapping paradigm, DataPath rarely needs to be _valgrinded_.