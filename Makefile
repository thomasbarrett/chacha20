CC=clang
CFLAGS = -std=c11 -Wall -pedantic -Iinclude -Wall -O3 $(MODULES_INCLUDE)
SRC_FILES = $(wildcard src/*.c)  $(wildcard src/*/*.c)
FILES = $(basename $(SRC_FILES:src/%=%))
OBJ_FILES = $(addprefix obj/,$(FILES:=.o))
TEST_FILES = $(join $(dir $(addprefix bin/tests/,$(FILES))), $(addprefix test_,$(notdir $(FILES))))
VERSION = 0.1.0

RWILDCARD = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

MODULES = git@github.com:thomasbarrett/uint.git@v0.1.0
MODULES_REPOSITORY = $(addprefix git@, $(foreach dep, $(MODULES), $(word 2, $(subst @, ,$(dep)))))
MODULES_VERSION = $(foreach dep, $(MODULES), $(word 3, $(subst @, ,$(dep))))
MODULES_MAJOR_VERSION = $(foreach dep, $(MODULES_VERSION), $(word 1, $(subst ., ,$(dep))))
MODULES_PREFIX = ~/.modules/
MODULES_ROOT = $(foreach dep, $(MODULES_REPOSITORY), $(word 1, $(subst ., ,$(word 2, $(subst :, ,$(dep))))))
MODULES_PATH = $(addprefix $(MODULES_PREFIX), $(join $(MODULES_ROOT), $(addprefix /,$(MODULES_MAJOR_VERSION))))
MODULES_OBJ = $(foreach p, $(MODULES_PATH), $(prefix $p, $(shell $(MAKE) object-files -C $p 2>/dev/null)))
MODULES_INCLUDE = $(foreach p, $(MODULES_PATH), $(p:=/include))

.PHONY: all 
all: $(TEST_FILES)

.PHONY: clean
clean:
	@rm -rf obj
	@rm -rf bin

.PHONY: build
build: $(TEST_FILES) $(OBJ_FILES)

obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) $^ -o $@

bin/tests/%: tests/%.c $(OBJ_FILES) $(MODULES_OBJ)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $^ -o $@

# suppress error for missing test file
bin/tests/%:
	@:

.PHONY: test
test: $(TEST_FILES)
	tests/run.sh $(TEST_FILES)

.PHONY: version
version:
	@echo ${VERSION}

.PHONY: list-deps
list-deps:
	@echo $(MODULES)

.PHONY: install-deps
install-deps:
	@$(foreach i, $(shell seq 1 $(words $(MODULES))), \
		M_PATH=$(word $i, $(MODULES_PATH)); \
		M_ROOT=$(word $i, $(MODULES_ROOT)); \
		M_REPOSITORY=$(word $i, $(MODULES_REPOSITORY)); \
		M_VERSION=$(word $i, $(MODULES_VERSION)); \
		if [ ! -d "$${M_PATH}" ]; then \
			printf "[ %s ] %s\n" $${M_VERSION} $${M_ROOT}; \
 			git clone "$${M_REPOSITORY}" --branch $${M_VERSION} --single-branch $${M_PATH} 2> /dev/null; \
		fi \
	)

# The build-deps target builds all dependency module make 
.PHONY: build-deps
build-deps: install-deps
	@$(foreach i, $(shell seq 1 $(words $(MODULES))), \
		printf "[ %s ] %s\n" $(word $i, $(MODULES_VERSION)) $(word $i, $(MODULES_ROOT)); \
		$(MAKE) -C $(word $i, $(MODULES_PATH)) &> /dev/null; \
	)

.PHONY: example
example: install-deps
	@echo $(MODULES_OBJ)

.PHONY: lint
lint:
	@for file in $(SRC_FILES); do \
		clang-tidy $$file --checks=clang-analyzer-*,performance-* -- $(CFLAGS); \
	done

.PHONY: benchmark
benchmark: bin/benchmark
	./bin/benchmark