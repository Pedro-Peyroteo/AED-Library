# AED-Library Architecture

**Project**: Library Management System  
**Course**: Algoritmos e Estruturas de Dados (AED)  
**Institution**: Universidade de Aveiro, 2025-2026

This document provides a detailed technical overview of the system architecture, design decisions, and implementation details.

---

## Table of Contents

1. [System Architecture](#system-architecture)
2. [Layer Details](#layer-details)
3. [Data Structures](#data-structures)
4. [Data Flow](#data-flow)
5. [Entity Models](#entity-models)
6. [Design Decisions](#design-decisions)
7. [Memory Management](#memory-management)
8. [File Format Specification](#file-format-specification)
9. [Algorithm Analysis](#algorithm-analysis)
10. [Error Handling](#error-handling)

---

## System Architecture

### Layered Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                   Application Layer                         │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │
│  │  menu.c  │  │  book_   │  │  user_   │  │  loan_   │   │
│  │          │─▶│controller│  │controller│  │controller│   │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │
│       │              │              │              │        │
│       └──────────────┴──────────────┴──────────────┘        │
│                          │                                  │
├──────────────────────────┼──────────────────────────────────┤
│                  Database Layer (db.c)                      │
│  ┌────────────────────────────────────────────────────┐    │
│  │  In-Memory Collections (DList)                     │    │
│  │  • books:       DList<Book*>                       │    │
│  │  • users:       DList<User*>                       │    │
│  │  • loans:       DList<Loan*>                       │    │
│  │  • suggestions: DList<Suggestion*>                 │    │
│  └────────────────────────────────────────────────────┘    │
│       │              │              │              │        │
├───────┼──────────────┼──────────────┼──────────────┼────────┤
│  File System Layer (fs/*.c)                                 │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │
│  │ books_   │  │ users_   │  │ loans_   │  │suggestions│   │
│  │ file.c   │  │ file.c   │  │ file.c   │  │ _file.c  │   │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │
│       │              │              │              │        │
├───────┼──────────────┼──────────────┼──────────────┼────────┤
│  Model Layer (model/*.c)                                    │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │
│  │  book.c  │  │  user.c  │  │  loan.c  │  │suggestion│   │
│  │          │  │          │  │          │  │   .c     │   │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │
├─────────────────────────────────────────────────────────────┤
│  Data Structures Library (lib/dlist)                        │
│  ┌────────────────────────────────────────────────────┐    │
│  │  DList (Generic Doubly-Linked List)                │    │
│  │  • dlist.c          - Core operations              │    │
│  │  • dlist_priority.c - Priority queue extension     │    │
│  └────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
         │              │              │              │
         ▼              ▼              ▼              ▼
    books.txt      users.txt      loans.txt    suggestions.txt
```

### Design Principles

**Separation of Concerns**

- Each layer has a single, well-defined responsibility
- Layers communicate only through defined interfaces
- No layer skipping (e.g., App cannot directly access FS)

**Dependency Direction**

- Dependencies flow downward (App → DB → FS → Model)
- Lower layers never depend on higher layers
- Model layer has no dependencies (pure data + logic)

**Modularity**

- Each module can be tested independently
- Easy to replace implementations (e.g., CSV → SQLite)
- Clear contracts via header files

---

## Layer Details

### 1. Application Layer (`src/app/`)

**Responsibility**: User interaction and business logic orchestration

**Components**:

**`main.c`**

- Entry point
- Calls `app_init()`, `app_run()`, `app_shutdown()`

**`app.c`**

- Application lifecycle management
- Initializes DB layer on startup
- Persists data on graceful shutdown
- Global DB instance management

**`menu.c`**

- Interactive menu system
- Input handling and validation
- Screen clearing and formatting
- Delegates to controllers

**Controllers** (`*_controller.c`)

- `book_controller.c` - Book CRUD operations
- `user_controller.c` - User CRUD operations
- `loan_controller.c` - Loan management and business rules
- `suggestion_controller.c` - Suggestion handling

**Responsibilities**:

- Validate user input
- Enforce business rules (e.g., can't delete user with active loans)
- Format output for display
- Call DB layer for data access

---

### 2. Database Layer (`src/db/`)

**Responsibility**: Unified data access and in-memory storage

**Key Functions**:

**Initialization**:

```c
int db_init(DB *db,
            const char *books_path,
            const char *users_path,
            const char *loans_path,
            const char *suggestions_path);
```

- Loads all CSV files into memory via FS layer
- Creates DList for each entity type
- Returns -1 on any failure (partial loads are cleaned up)

**Persistence**:

```c
int db_save(const DB *db, ...paths);
```

- Writes all in-memory data back to CSV files
- Atomic operation (all or nothing on errors)
- Called on app shutdown

**CRUD Operations**:

- `db_add_*()` - Add entity to collection
- `db_remove_*()` - Remove entity by ID
- `db_find_*_by_id()` - Lookup by ID
- `db_get_*()` - Get entire collection for iteration

**Design Choices**:

- **In-memory storage**: Fast access, acceptable for dataset size (~100s records)
- **Lazy persistence**: Only save on shutdown (simplifies logic, acceptable for single-user)
- **Sorted lists**: Entities sorted by ID via priority queue (enables efficient lookups)

---

### 3. File System Layer (`src/fs/`)

**Responsibility**: CSV file I/O and serialization

**Each file handler provides two functions**:

```c
DList* file_load_books(const char *path);
int    file_save_books(const char *path, const DList *books);
```

**Load Operation**:

1. Open file for reading (`fopen`)
2. Skip or parse header line (starts with "id;")
3. For each line:
   - Parse CSV into temp buffer
   - Call `*_from_csv()` to deserialize
   - Allocate struct, copy data
   - Insert into DList via priority queue (sorted by ID)
4. Return populated DList (or empty list if file missing)

**Save Operation**:

1. Open file for writing (truncates existing)
2. Write header line
3. For each entity in DList:
   - Call `*_to_csv()` to serialize
   - Write line to file
4. Close file

**Error Handling**:

- Missing file → return empty list (first run scenario)
- Malloc failure → clean up and return NULL
- Malformed lines → skip and continue (defensive)

---

### 4. Model Layer (`src/model/`)

**Responsibility**: Entity definitions and serialization logic

**Each model provides**:

**Constructor**:

```c
void book_init(Book *b, unsigned id, const char *title, ...);
```

**CSV Serialization**:

```c
int  book_from_csv(Book *b, const char *line);  // Parse
void book_to_csv(const Book *b, char *out, size_t out_size);  // Serialize
```

**Display**:

```c
void book_print(const Book *b);
```

**No Business Logic**:

- Models are "dumb" data containers
- No knowledge of other entities
- No file I/O or database access
- Pure functions (stateless)

---

### 5. Data Structures Library (`src/lib/dlist/`)

**Responsibility**: Generic doubly-linked list implementation

**Core Operations** (`dlist.c`):

```c
DList* dlist_create(bool priority_mode, int (*cmp)(void*, void*));
void   dlist_destroy(DList *list, void (*free_fn)(void*));

void   dlist_push_front(DList *list, void *data);  // O(1)
void   dlist_push_back(DList *list, void *data);   // O(1)
void*  dlist_pop_front(DList *list);               // O(1)
void*  dlist_pop_back(DList *list);                // O(1)

void   dlist_remove_node(DList *list, DListNode *node, void (*free_fn)(void*));  // O(1)

void*  dlist_peek_front(DList *list);
void*  dlist_peek_back(DList *list);
bool   dlist_is_empty(DList *list);
size_t dlist_size(DList *list);
```

**Priority Queue Extension** (`dlist_priority.c`):

```c
void dlist_insert_priority(DList *list, void *data, int priority);  // O(n)
```

- Inserts element in sorted order by priority value
- Lower priority value = closer to head
- Used for ID-sorted collections

---

## Data Structures

### Doubly-Linked List (DList)

```c
typedef struct DListNode {
    void *data;              // Generic data pointer
    int priority;            // Used in priority mode
    struct DListNode *prev;  // Bidirectional links
    struct DListNode *next;
} DListNode;

typedef struct DList {
    DListNode *head;         // First node
    DListNode *tail;         // Last node
    size_t size;             // Number of nodes
    bool priority_mode;      // Enable sorted insertion
    int (*cmp)(void*, void*); // Custom comparator
} DList;
```

**Advantages**:

- **O(1) insertion/deletion** at both ends
- **O(1) removal** with node pointer (useful after search)
- **Bidirectional traversal** (forward and backward)
- **No reallocation** overhead (unlike arrays)

**Trade-offs**:

- **O(n) search** (linear scan required)
- **Memory overhead**: 24 bytes per node (pointers + data)
- **Cache unfriendly**: Non-contiguous memory
- **Acceptable**: Dataset sizes small (~100s records), CRUD > search frequency

**Why not Arrays?**:

- Reallocation costly for growing collections
- Removal requires shifting elements (O(n))
- Fixed capacity or complex growth logic
- DList simpler and sufficient for use case

---

## Data Flow

### Application Startup

```
1. main()
   └─▶ app_init()
       └─▶ db_init(&db, "data/books.txt", ...)
           ├─▶ file_load_books("data/books.txt")
           │   ├─▶ fopen(), fgets() line-by-line
           │   ├─▶ book_from_csv(&book, line)
           │   └─▶ dlist_insert_priority(books, &book, book.id)
           ├─▶ file_load_users(...)
           ├─▶ file_load_loans(...)
           └─▶ file_load_suggestions(...)
```

**Result**: All data loaded into memory, ready for CRUD operations

---

### CRUD Operation Flow (Example: Create Loan)

```
User Input: "Registar Empréstimo"
   │
   ├─▶ menu.c: menu_run() dispatches to loan_controller
   │
   ├─▶ loan_controller.c: loan_create()
   │   ├─▶ Prompt user for user_id, book_id, date
   │   ├─▶ db_find_user_by_id(db, user_id)  [validate user exists]
   │   ├─▶ db_find_book_by_id(db, book_id)  [validate book exists]
   │   ├─▶ Check book->available == 1       [validate availability]
   │   ├─▶ Generate new_loan_id (scan loans for max ID + 1)
   │   ├─▶ loan_init(&loan, new_loan_id, user_id, book_id, date, "")
   │   ├─▶ db_add_loan(db, &loan)
   │   │   └─▶ malloc(sizeof(Loan))
   │   │       └─▶ dlist_insert_priority(db->loans, loan_ptr, loan.id)
   │   └─▶ Set book->available = 0
   │
   └─▶ Display confirmation message
```

**Result**: Loan added to in-memory list, book marked unavailable

---

### Application Shutdown

```
User Input: "Sair"
   │
   ├─▶ menu.c: breaks loop, returns to main()
   │
   ├─▶ main.c: app_shutdown()
   │   └─▶ db_save(&db, "data/books.txt", ...)
   │       ├─▶ file_save_books("data/books.txt", db->books)
   │       │   ├─▶ fopen(..., "w")  [truncate]
   │       │   ├─▶ fprintf(f, "id;title;author;...\n")
   │       │   ├─▶ DLIST_FOREACH(books, node)
   │       │   │   ├─▶ book_to_csv(book, buffer, size)
   │       │   │   └─▶ fprintf(f, "%s\n", buffer)
   │       │   └─▶ fclose(f)
   │       ├─▶ file_save_users(...)
   │       ├─▶ file_save_loans(...)
   │       └─▶ file_save_suggestions(...)
   │
   └─▶ db_destroy(&db)
       └─▶ dlist_destroy(db->books, free)  [frees all Book structs]
           └─▶ dlist_destroy(db->users, free)
               └─▶ ...
```

**Result**: All changes persisted to disk, memory freed

---

## Entity Models

### Book

```c
typedef struct {
    unsigned id;           // Unique identifier
    char title[128];       // Book title
    char author[128];      // Author name(s)
    int year;              // Publication year
    char isbn[32];         // ISBN code
    char edition[32];      // Edition info
    int available;         // 1 = available, 0 = on loan
} Book;
```

**CSV Format**: `id;title;author;year;isbn;edition;available`

**Constraints**:

- `id` > 0 (generated automatically)
- `title` and `author` required (non-empty)
- `year` any integer (no validation)
- `available` synced with active loans

---

### User

```c
typedef struct {
    unsigned id;       // Unique identifier
    char name[128];    // Full name
    char email[128];   // Email address
} User;
```

**CSV Format**: `id;name;email`

**Constraints**:

- `id` > 0 (generated)
- `name` required
- `email` not validated (any string accepted)

---

### Loan

```c
typedef struct {
    unsigned id;              // Unique identifier
    unsigned user_id;         // FK to User
    unsigned book_id;         // FK to Book
    char date_borrow[32];     // YYYY-MM-DD
    char date_return[32];     // YYYY-MM-DD or empty if active
} Loan;
```

**CSV Format**: `id;user_id;book_id;date_borrow;date_return`

**Constraints**:

- `user_id` must exist in users collection (validated)
- `book_id` must exist in books collection (validated)
- Empty `date_return` indicates active loan
- Dates not validated (string format trusted)

**Business Rules**:

- Cannot create loan if book unavailable
- Cannot delete user/book with active loans
- Returning book sets `date_return` and marks book available

---

### Suggestion

```c
typedef struct {
    unsigned id;         // Unique identifier
    char title[128];     // Suggested book title
    char author[128];    // Author name
    char isbn[32];       // ISBN if known
    char edition[32];    // Preferred edition
} Suggestion;
```

**CSV Format**: `id;title;author;isbn;edition`

**Constraints**:

- `title` and `author` required
- `isbn` and `edition` optional

---

## Design Decisions

### 1. ID Generation Strategy

**Approach**: Scan collection for max ID, return max + 1

```c
unsigned next_book_id(DList *books) {
    unsigned max = 0;
    DLIST_FOREACH(books, node) {
        Book *b = (Book*)node->data;
        if (b->id > max) max = b->id;
    }
    return max + 1;
}
```

**Pros**:

- Simple implementation
- No separate counter to persist
- Survives deletions (IDs never reused)

**Cons**:

- O(n) per ID generation
- Inefficient for large n (acceptable for n < 1000)

**Alternative Considered**: Persist `next_id` in metadata file

- More efficient (O(1) generation)
- Requires additional file I/O
- Complexity not justified for dataset size

---

### 2. CSV Format

**Choice**: Semicolon-delimited (`;`) instead of comma (`,`)

**Rationale**:

- Portuguese text frequently contains commas
- Avoids escaping logic complexity
- Simpler parser implementation

**Trade-offs**:

- **Not RFC 4180 compliant** (no quoted fields)
- **Cannot store `;` in data** (would break parsing)
- **Good enough** for controlled academic project

**Example**:

```
1;Clean Code;Robert C. Martin;2008;9780132350884;1;1
```

---

### 3. In-Memory Storage (No Database)

**Choice**: Load entire dataset into RAM on startup

**Rationale**:

- Fast CRUD operations (no disk I/O per operation)
- Simpler implementation (no SQL, transactions, etc.)
- Dataset size small (~100-1000 records fits easily in RAM)
- Single-user application (no concurrency issues)

**Trade-offs**:

- **Lazy persistence**: Changes lost on crash (acceptable for demo)
- **Scalability**: Not suitable for large datasets or multi-user
- **No queries**: Cannot leverage SQL for complex searches

**When to Migrate**:

- Dataset exceeds ~10K records (memory footprint grows)
- Multi-user access required (need transactions)
- Complex queries needed (SQL benefits)

---

### 4. Priority Queue for Sorted Collections

**Choice**: Use priority queue to maintain ID-sorted order

**Implementation**:

```c
dlist_insert_priority(db->books, book_ptr, book->id);
```

**Rationale**:

- Simpler than explicit sorting after insertions
- Maintains invariant (always sorted)
- Efficient enough for small n (O(n) insertion acceptable)

**Trade-offs**:

- O(n) insertion vs O(1) for unsorted
- Benefits: Cleaner display (sorted lists), easier debugging

---

### 5. Layered Architecture

**Choice**: Strict layer separation (App → DB → FS → Model)

**Rationale**:

- **Testability**: Each layer tested independently
- **Maintainability**: Changes isolated to one layer
- **Clarity**: Clear responsibilities and interfaces
- **Extensibility**: Easy to swap implementations (e.g., CSV → SQLite)

**Example**: Changing from CSV to JSON

- Only modify FS layer (`books_file.c` → `books_json.c`)
- DB and App layers unchanged
- Model layer unchanged

---

## Memory Management

### Ownership Model

**Rule**: **DB layer owns all entity structs**

**Allocation**:

- Controllers create stack-allocated structs (temporary)
- `db_add_*()` allocates heap copy via `malloc()`
- Heap struct stored in DList

**Deallocation**:

- `db_remove_*()` frees struct via `free()` in `dlist_remove_node()`
- `db_destroy()` frees all structs via `dlist_destroy(..., free)`

**Example Flow**:

```c
// Controller
Book temp_book;
book_init(&temp_book, id, title, ...);  // Stack allocation
db_add_book(&db, &temp_book);           // DB copies to heap

// Inside db_add_book():
Book *heap_book = malloc(sizeof(Book)); // Heap allocation
*heap_book = *src;                      // Copy from stack
dlist_insert_priority(db->books, heap_book, id);

// Cleanup (shutdown or removal):
dlist_destroy(db->books, free);  // Frees all heap books
```

---

### Memory Leak Prevention

**Strategies**:

1. **Consistent patterns**: Every `malloc` paired with `free`
2. **Centralized cleanup**: `db_destroy()` frees all entities
3. **RAII-style wrappers**: DList takes ownership, handles cleanup
4. **Testing**: Valgrind on Linux verifies no leaks

**Common Pitfalls Avoided**:

- Forgetting to free on early returns (error paths)
- Double-freeing (careful ownership tracking)
- Freeing stack-allocated data (only heap pointers to DList)

---

## File Format Specification

### CSV Structure

**General Format**:

```
header_line
data_line_1
data_line_2
...
```

**Header Line**:

- Starts with "id;"
- Comma-separated field names
- Ignored on load (field order determined by code)

**Data Lines**:

- Semicolon-delimited values
- No quoting or escaping
- Empty string for optional fields (but field must exist)

---

### books.txt

```
id;title;author;year;isbn;edition;available
1;The C Programming Language;Brian W. Kernighan & Dennis M. Ritchie;1988;0131103628;2;1
2;Clean Code;Robert C. Martin;2008;9780132350884;1;0
```

**Field Count**: 7  
**Optional Fields**: None (all required)

---

### users.txt

```
id;name;email
1;Alice Silva;alice@example.com
2;Bob Martins;bob@example.com
```

**Field Count**: 3  
**Optional Fields**: None

---

### loans.txt

```
id;user_id;book_id;date_borrow;date_return
1;1;2;2025-12-01;2025-12-15
2;2;1;2025-12-04;
```

**Field Count**: 5  
**Optional Fields**: `date_return` (empty = active loan)

---

### suggestions.txt

```
id;title;author;isbn;edition
1;Design Patterns;Erich Gamma;0201633612;1
2;Refactoring;Martin Fowler;;
```

**Field Count**: 5  
**Optional Fields**: `isbn`, `edition` (empty allowed)

---

## Algorithm Analysis

### Time Complexity

| Operation       | DList | Array        | Notes                             |
| --------------- | ----- | ------------ | --------------------------------- |
| Insert at head  | O(1)  | O(n)         | Array requires shift              |
| Insert at tail  | O(1)  | O(1) or O(n) | Array amortized O(1) with realloc |
| Remove at head  | O(1)  | O(n)         | Array requires shift              |
| Remove at tail  | O(1)  | O(1)         | -                                 |
| Remove by node  | O(1)  | O(n)         | DList advantage                   |
| Search by ID    | O(n)  | O(n)         | Both linear (could use hash)      |
| Priority insert | O(n)  | O(n)         | Both need scan for position       |
| Get by index    | O(n)  | O(1)         | Array advantage                   |

**Key Insight**: DList preferred because:

- Removal after search is O(1) (keep node pointer)
- No reallocation overhead
- Sufficient for small datasets

---

### Space Complexity

**Per Node**:

- `DListNode`: 24 bytes (3 pointers × 8 bytes on 64-bit)
- Payload (e.g., `Book`): ~300 bytes
- **Total**: ~324 bytes per book

**Per Collection**:

- 100 books: ~32 KB
- 1000 books: ~320 KB
- **Acceptable** for in-memory storage

**Comparison to Array**:

- Array: Just payload (no pointer overhead)
- DList: +24 bytes overhead per element (~8% for Book struct)
- Trade-off accepted for structural benefits

---

## Error Handling

### Strategy

**Levels**:

1. **Silent Skip** (FS layer)

   - Malformed CSV lines → skip line, continue parsing
   - Rationale: Defensive parsing, don't crash on bad data

2. **Return Codes** (DB/FS layers)

   - Functions return `int` (0 = success, -1 = error)
   - Caller checks and handles
   - Example: `if (db_init(...) != 0) { /* handle error */ }`

3. **User Messages** (App layer)

   - Controllers print Portuguese error messages
   - Example: "Livro não encontrado" (Book not found)

4. **NULL Returns** (Search functions)
   - `db_find_*_by_id()` returns NULL if not found
   - Caller must check before dereferencing

---

### Common Error Scenarios

| Scenario              | Layer | Handling                                       |
| --------------------- | ----- | ---------------------------------------------- |
| File not found        | FS    | Return empty list (first run OK)               |
| Malloc failure        | DB/FS | Return -1 or NULL, cleanup partial allocations |
| Invalid user input    | App   | Re-prompt user, validate before processing     |
| Foreign key violation | App   | Check existence before creating loan           |
| Book unavailable      | App   | Prevent loan creation, show message            |
| Active loans exist    | App   | Prevent user/book deletion, show message       |

---

### Validation

**Input Validation** (App layer):

- Non-empty strings for required fields
- Numeric input within range (if applicable)
- Existence checks for foreign keys

**Data Validation** (Model layer):

- Minimal (trust data from files)
- No email format validation
- No date format validation
- Assumption: Data generated by program is valid

**Future Improvements**:

- Add regex validation for email
- Parse dates into struct tm
- Range check for year field

---

## Performance Considerations

### Current Performance

**Acceptable for**:

- Datasets up to ~1000 records per collection
- Single-user access
- Terminal-based interaction (human speed)

**Bottlenecks**:

- **ID generation**: O(n) scan per insert
- **Search**: O(n) linear scan
- **Startup**: O(n) load per entity type

---

### Optimization Opportunities

**If scaling needed**:

1. **Add Hash Table Index**

   - Index by ID for O(1) lookup
   - Trade-off: Additional memory + complexity

2. **Cache Max ID**

   - Maintain `max_id` variable in DB layer
   - Update on insert/remove
   - O(1) ID generation

3. **Lazy Loading**

   - Load entities on-demand (paginated)
   - Suitable for large datasets

4. **Database Migration**
   - Move to SQLite for complex queries
   - Keep DList for active working set

---

## Testing Strategy

### Unit Tests

**Data Structures** (`test_dlist.c`):

- Test all DList operations independently
- Verify memory management (no leaks)
- Test edge cases (empty list, single element)

**File I/O** (`test_fs.c`):

- Test CSV parsing (valid and malformed input)
- Test round-trip (load → save → load)
- Verify data integrity

---

### Integration Tests

**Database Layer** (`test_db.c`):

- Test multi-entity operations
- Verify foreign key enforcement
- Test cascade checks
- Verify persistence

---

### Manual Testing

**Interactive Demo** (`example_db_usage.c`):

- Walk through complete workflows
- Useful for regression testing
- Demonstrates API usage

---

**Document Version**: 1.0  
**Last Updated**: December 2025  
**Maintained By**: AED-Library Team
