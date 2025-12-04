# =====================================================
#   AED-LIBRARY PROJECT MAKEFILE
# =====================================================

# Compiler and settings
CC       := gcc
CFLAGS   := -Wall -Wextra -std=c11 -Iinclude -Iinclude/lib -Iinclude/lib/dlist -Iinclude/lib/cutils
LDFLAGS  :=

# Output directories
BUILDDIR := build
TARGET   := $(BUILDDIR)/main

# Platform helpers for mkdir/rmdir and executable suffix
ifeq ($(OS),Windows_NT)
	EXEEXT := .exe
define MKDIR_P
	@if not exist "$(subst /,\\,$(1))" mkdir "$(subst /,\\,$(1))"
endef
define RM_DIR
	@if exist "$(subst /,\\,$(1))" rmdir /S /Q "$(subst /,\\,$(1))"
endef
else
	EXEEXT :=
define MKDIR_P
	@mkdir -p $(1)
endef
define RM_DIR
	@rm -rf $(1)
endef
endif

# Manually list all source files (avoids Unix 'find')
SRC := \
	src/app/main.c \
	src/app/app.c \
	src/app/menu.c \
	src/app/user_controller.c \
	src/app/book_controller.c \
	src/app/loan_controller.c \
	src/app/suggestion_controller.c \
	src/db/db.c \
	src/fs/books_file.c \
	src/fs/users_file.c \
	src/fs/loans_file.c \
	src/fs/suggestions_file.c \
	src/lib/cutils/cutils.c \
	src/lib/dlist/dlist.c \
	src/lib/dlist/dlist_priority.c \
	src/model/book.c \
	src/model/loan.c \
	src/model/user.c \
	src/model/suggestion.c

# Convert src/.../*.c  build/.../*.o
OBJ := $(patsubst src/%.c,$(BUILDDIR)/%.o,$(SRC))

# =====================================================
#   Default build target
# =====================================================
all: $(TARGET)

# Link final executable
$(TARGET): $(OBJ)
	$(call MKDIR_P,$(BUILDDIR))
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)
	@echo "Build successful."

# Compile object files
$(BUILDDIR)/%.o: src/%.c
	$(call MKDIR_P,$(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

# =====================================================
#   TEST: DLIST
# =====================================================
test-dlist: src/tests/test_dlist.c \
            src/lib/dlist/dlist.c \
            src/lib/dlist/dlist_priority.c \
            src/lib/cutils/cutils.c
	$(call MKDIR_P,$(BUILDDIR))
	$(CC) $(CFLAGS) \
		src/tests/test_dlist.c \
		src/lib/dlist/dlist.c \
		src/lib/dlist/dlist_priority.c \
		src/lib/cutils/cutils.c \
		-o $(BUILDDIR)/test_dlist
	@echo "Running test..."
	$(BUILDDIR)/test_dlist$(EXEEXT)

# =====================================================
#   CLEAN
# =====================================================
clean:
	$(call RM_DIR,$(BUILDDIR))
	@echo "Cleaned."

# =====================================================
#   PHONY
# =====================================================
.PHONY: all clean test-dlist test-fs test-db test-db-edge

# =====================================================
#   TEST: FS LAYER
# =====================================================
test-fs: src/tests/test_fs.c \
	src/fs/books_file.c \
	src/fs/users_file.c \
	src/fs/loans_file.c \
	src/fs/suggestions_file.c \
	src/lib/dlist/dlist.c \
	src/lib/dlist/dlist_priority.c \
	src/lib/cutils/cutils.c \
	src/model/book.c \
	src/model/user.c \
	src/model/loan.c \
	src/model/suggestion.c
	$(call MKDIR_P,$(BUILDDIR))
	$(CC) $(CFLAGS) \
		src/tests/test_fs.c \
		src/fs/books_file.c \
		src/fs/users_file.c \
		src/fs/loans_file.c \
		src/lib/dlist/dlist.c \
		src/lib/dlist/dlist_priority.c \
		src/lib/cutils/cutils.c \
		src/model/book.c \
		src/model/user.c \
		src/model/loan.c \
		-o $(BUILDDIR)/test_fs
	@echo "Running fs layer test..."
	$(BUILDDIR)/test_fs$(EXEEXT)

# =====================================================
#   TEST: DB LAYER (integrated with FS)
# =====================================================
test-db: src/tests/test_db.c \
	src/db/db.c \
	src/fs/books_file.c \
	src/fs/users_file.c \
	src/fs/loans_file.c \
	src/lib/dlist/dlist.c \
	src/lib/dlist/dlist_priority.c \
	src/lib/cutils/cutils.c \
	src/model/book.c \
	src/model/user.c \
	src/model/loan.c
	$(call MKDIR_P,$(BUILDDIR))
	$(CC) $(CFLAGS) \
		src/tests/test_db.c \
		src/db/db.c \
		src/fs/books_file.c \
		src/fs/users_file.c \
		src/fs/loans_file.c \
		src/fs/suggestions_file.c \
		src/lib/dlist/dlist.c \
		src/lib/dlist/dlist_priority.c \
		src/lib/cutils/cutils.c \
		src/model/book.c \
		src/model/user.c \
		src/model/loan.c \
		src/model/suggestion.c \
		-o $(BUILDDIR)/test_db
	@echo "Running db layer integration test..."
	$(BUILDDIR)/test_db$(EXEEXT)

# =====================================================
#   DB USAGE EXAMPLE (standalone demo)
# =====================================================
example-db: src/tests/example_db_usage.c \
	src/db/db.c \
	src/fs/books_file.c \
	src/fs/users_file.c \
	src/fs/loans_file.c \
	src/fs/suggestions_file.c \
	src/lib/dlist/dlist.c \
	src/lib/dlist/dlist_priority.c \
	src/lib/cutils/cutils.c \
	src/model/book.c \
	src/model/user.c \
	src/model/loan.c \
	src/model/suggestion.c
	$(call MKDIR_P,$(BUILDDIR))
	$(CC) $(CFLAGS) \
		src/tests/example_db_usage.c \
		src/db/db.c \
		src/fs/books_file.c \
		src/fs/users_file.c \
		src/fs/loans_file.c \
		src/fs/suggestions_file.c \
		src/lib/dlist/dlist.c \
		src/lib/dlist/dlist_priority.c \
		src/lib/cutils/cutils.c \
		src/model/book.c \
		src/model/user.c \
		src/model/loan.c \
		src/model/suggestion.c \
		-o $(BUILDDIR)/example_db_usage
	@echo "Running DB usage example..."
	$(BUILDDIR)/example_db_usage$(EXEEXT)

# =====================================================
#   RUN MAIN APPLICATION
# =====================================================
run: all
	@echo "Running main application..."
	$(TARGET)$(EXEEXT)
