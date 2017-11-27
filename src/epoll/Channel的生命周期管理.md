## 存在的问题
1. epoll select以后，准备调用Channel::react()前，Channel已经销毁

2. 回调Channel::reactor()以后，异步回调函数，往往需要Channel* 
作为参数，异步回调函数，往往运行在另外的线程，当异步回调函数在别的线程执行时，或则是已经成了异步任务，将Channel*传到别的线程去了等待执行
时，在epoll线程将Channel delete，将会导致任务线程的core dump。

3. 需要在别的线程对epoll进行add/delete操作

先看第一个问题。 这个问题，其实比较好解决，用shared_ptr就可以解决。当Channel添加到Processor
时，就使用shared_ptr，在Processor中删除Channel事件时去解除共享。这样可以保证Processor不会
被动的接受指针被删除这一情况。第二种情况，也是在把回调参数设置为shared_ptr，以保证在回调任务期
间，可以正常执行。

再看第三个问题。 线程间通信，可以使用一个共享的数据结构，在其他线程要进行/add/delete操作的时候
，把操作记录在那个数据结构中，等epoll完成了一个loop以后，再去里面读，然后统一执行。共享的数据
机构就用锁去保护就行。