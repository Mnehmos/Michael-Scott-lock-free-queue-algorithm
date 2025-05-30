# Phase 2: PSEUDOCODE

## Task 2.1: Core Data Structure Pseudocode

### Node Structure
```
struct Node<T> {
    T data
    Atomic<Node<T>*> next
    uintptr_t aba_counter
}

struct pointer_t {
    Node<T>* ptr
    uintptr_t counter
}
```

### Queue Class Interface
```
class MSQueue<T> {
private:
    Atomic<pointer_t> head
    Atomic<pointer_t> tail
    HazardPointerManager hp_manager
    
public:
    MSQueue()
    void enqueue(T data)
    bool dequeue(T& result)
    bool is_empty()
}
```

## Task 2.2: Queue Operation Pseudocode

### Constructor
```
MSQueue() {
    dummy = new Node<T>()
    head = {dummy, 0}
    tail = {dummy, 0}
}
```

### Enqueue Operation
```
procedure ENQUEUE(data) {
    node = new Node(data)
    while true {
        last = tail.load(relaxed)
        next = last.ptr->next.load(relaxed)
        if last == tail.load(acquire) {
            if next.ptr == null {
                if CAS(&last.ptr->next, next, {node, next.counter+1}, release) {
                    CAS(&tail, last, {node, last.counter+1}, relaxed)
                    return
                }
            } else {
                CAS(&tail, last, {next.ptr, last.counter+1}, relaxed)
            }
        }
    }
}
```

// Additional pseudocode for dequeue and other operations will follow

## Task 2.3: Dequeue Operation Pseudocode

### Dequeue with Hazard Pointers
```
procedure DEQUEUE(result) {
    while true {
        // Use hazard pointers to protect head and next
        hp1 = hp_manager.acquire(0)
        first = head.load(relaxed)
        hp1.store(first.ptr)
        if head.load(acquire) != first { continue } // Double-check
        
        last = tail.load(relaxed)
        next = first.ptr->next.load(acquire)
        
        hp2 = hp_manager.acquire(1)
        hp2.store(next.ptr)
        if head.load(acquire) != first { continue } // Re-check
        
        if first.ptr == last.ptr {
            if next.ptr == null {
                return false // Queue empty
            }
            CAS(&tail, last, {next.ptr, last.counter+1}, relaxed)
        } else {
            result = next.ptr->data
            if CAS(&head, first, {next.ptr, first.counter+1}, release) {
                hp_manager.retire_node(first.ptr)
                return true
            }
        }
    }
}
```

### is_empty() Implementation
```
function IS_EMPTY() {
    first = head.load(acquire)
    last = tail.load(acquire)
    return (first.ptr == last.ptr) && (first.ptr->next.load(relaxed) == null)
}
```

## Task 2.4: Hazard Pointer Pseudocode

### HP Manager Interface
```
class HazardPointerManager {
private:
    static constexpr int K = 2;
    thread_local static HazardPointer hps[K];
    thread_local static std::vector<Node*> retire_list;
    static std::atomic<HazardPointer*> global_list;
    
public:
    void* acquire(int index, void* ptr);
    void release(int index);
    void retire_node(void* ptr);
    void scan_and_reclaim();
}
```

### acquire/release Implementation
```
void* ACQUIRE(int index, void* ptr) {
    hps[index].ptr.store(ptr, std::memory_order_release);
    return ptr;
}

void RELEASE(int index) {
    hps[index].ptr.store(nullptr, std::memory_order_relaxed);
}
```

### retire_node and scan
```
void RETIRE_NODE(void* ptr) {
    retire_list.push_back(ptr);
    if (retire_list.size() >= R) { // R = 2*K
        scan_and_reclaim();
    }
}

void SCAN_AND_RECLAIM() {
    std::unordered_set<void*> hazardous;
    
    // Collect all protected pointers
    HazardPointer* curr = global_list.load(acquire);
    while (curr) {
        for (int i = 0; i < K; i++) {
            void* ptr = curr[i].ptr.load(acquire);
            if (ptr) hazardous.insert(ptr);
        }
        curr = curr->next;
    }
    
    // Reclaim non-hazardous nodes
    auto it = retire_list.begin();
    while (it != retire_list.end()) {
        if (hazardous.find(*it) == hazardous.end()) {
            delete static_cast<Node*>(*it);
            it = retire_list.erase(it);
        } else {
            ++it;
        }
    }
}
```

### Global Registration
```
// During thread initialization
void INIT_THREAD() {
    HazardPointer* new_hps = new HazardPointer[K];
    HazardPointer* old = global_list.load(relaxed);
    do {
        new_hps->next = old;
    } while (!global_list.compare_exchange_weak(old, new_hps, release, relaxed));
}
```

## Task 2.5: Integration Pseudocode

### Queue-HP Initialization
```
MSQueue() {
    dummy = new Node<T>()
    head = {dummy, 0}
    tail = {dummy, 0}
    hp_manager = HazardPointerManager()
}

~MSQueue() {
    // Drain remaining nodes
    T tmp;
    while (dequeue(tmp)) {}
    delete head.load(relaxed).ptr
}
```

### Memory Ordering Integration
```
// Enqueue operation
if CAS(&last.ptr->next, next, {node, next.counter+1}, memory_order_release) 

// Dequeue operation
next = first.ptr->next.load(memory_order_acquire)
if CAS(&head, first, {next.ptr, first.counter+1}, memory_order_release)
```

### Error Handling Integration
```
procedure ENQUEUE(data) {
    node = new (std::nothrow) Node(data)
    if node == null {
        return ERROR_OUT_OF_MEMORY
    }
    // ... rest of enqueue logic ...
}

procedure DEQUEUE(result) {
    // ... 
    if invariant_check_failed {
        return ERROR_QUEUE_CORRUPTED
    }
    // ...
}
```

### Performance Hooks
```
void ENQUEUE(data) {
    start = clock()
    // ... operation ...
    stats.enqueue_time.add(clock() - start)
    stats.enqueue_count++
}
```

## Phase 2 Completion
Pseudocode for all queue components is now complete. Next phase will focus on architecture design.
