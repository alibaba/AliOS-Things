#!/bin/bash

# Generates a graphviz dependency graph for :ruy, with details trimmed.
# Suggested rendering: pipe to `neato` (part of graphviz standard distribution)
#   doc/depgraph.sh | dot -Tsvg > depgraph.svg

drop=(
    ':platform'
    ':check_macros'
    ':asm_helpers'
    ':size_util'
    ':system_aligned_alloc'
    ':side_pair'
    ':opt_set'
    ':blocking_counter'
    ':wait'
    ':time'
    ':path'
    ':performance_advisory'
    ':tune'
    ':matrix'
    ':mat'
    ':mul_params'
    ':context_get_ctx'
    ':have_built_path_for'
    ':pack_common'
    ':kernel_common'
    ':trace'
    ':validate'
    'profiler:instrumentation'
    '\bclog\b'
    '\bcpuinfo_impl\b'
    ':apply_multiplier'
    '\blabel='
)

graph="$(bazel query 'kind("cc_library", deps(//ruy))' --output graph --noimplicit_deps 2>/dev/null)"

graph="$(echo "${graph}" | sed 's|//ruy/\?||g')"

for t in "${drop[@]}"; do
  graph="$(echo "${graph}" | grep -v "${t}")"
done

graph="$(echo "${graph}" | sed 's|//:cpuinfo_with_unstripped_include_path||g')"
graph="$(echo "${graph}" | sed 's|//third_party/cpuinfo:[a-z0-9_]*|@cpuinfo|g')"

frontend=(
    ':ruy'
    ':context'
    ':frontend'
    ':prepare_packed_matrices'
    ':create_trmul_params'
)

middleend=(
    ':ctx'
    ':trmul_params'
    ':trmul'
    ':block_map'
    ':cpuinfo'
    ':cpu_cache_params'
    ':allocator'
    ':prepacked_cache'
)

backend=(
    ':kernel.*'
    ':pack.*'
)

threadpool=(
    ':thread_pool'
)

frontend_lines=()
middleend_lines=()
backend_lines=()
threadpool_lines=()
misc_lines=()
arrow_lines=()

while IFS= read -r line; do
  if [[ "${line}" =~ '->' ]]; then
    arrow_lines+=("${line}")
  else
    handled=false
    if [ $handled = false ]; then
        for f in "${frontend[@]}"; do
            if [[ "${line}" =~ ${f} ]]; then
                frontend_lines+=("${line}")
                handled=true
                break
            fi
        done
    fi
    if [ $handled = false ]; then
        for f in "${middleend[@]}"; do
            if [[ "${line}" =~ ${f} ]]; then
                middleend_lines+=("${line}")
                handled=true
                break
            fi
        done
    fi
    if [ $handled = false ]; then
        for f in "${backend[@]}"; do
            if [[ "${line}" =~ ${f} ]]; then
                backend_lines+=("${line}")
                handled=true
                break
            fi
        done
    fi
    if [ $handled = false ]; then
        for f in "${threadpool[@]}"; do
            if [[ "${line}" =~ ${f} ]]; then
                threadpool_lines+=("${line}")
                handled=true
                break
            fi
        done
    fi
    if [ $handled = false ]; then
        if [[ "${line}" =~ ^[[:space:]]+\" ]]; then
            misc_lines+=("${line}")
        fi
    fi
  fi
done <<< "${graph}"

echo "digraph ruy {"
echo "  splines = true"
echo "  node [shape=box]"
for f in "${frontend_lines[@]}"; do
  echo "  $f [style=filled, color=\"#B2EBF2\"];"
done
for m in "${middleend_lines[@]}"; do
  echo "  $m [style=filled, color=\"#C8E6C9\"];"
done
for b in "${backend_lines[@]}"; do
  echo "  $b [style=filled, color=\"#FFCDD2\"];"
done
for b in "${threadpool_lines[@]}"; do
  echo "  $b [style=filled, color=\"#FFF9C4\"];"
done
for m in "${misc_lines[@]}"; do
  echo "$m"
done
for a in "${arrow_lines[@]}"; do
  echo "$a"
done
echo "}"
