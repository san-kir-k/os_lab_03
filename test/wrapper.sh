#! /usr/bin/env bash

function log_info()
{
  log_ "info" "$@"
}

function log_error()
{
  log_ "error" "$@"
}

function log_()
{
  local type=$1
  local message=$2
  local date_str=; date_str=$(date +'%Y-%m-%d %H:%M:%S')
  echo "[${type}] [${date_str}] ${message}"
}

function main()
{
  log_info "Stage #1 Compiling..."
  if ! ( cd ../src && make ) ; then
    log_error "Failed to compile."
    return 1
  fi


  log_info "Stage #2 Test generating..."
  if ! python3 generator.py . ; then
    log_error "Failed to generate tests."
    return 1
  fi

  log_info "Stage #3 Checking (only spent time output)..."
  threads=(1 2 4)
  for i in "${threads[@]}"; do
    for test_file in $( ls *.txt ) ; do
        echo ">>Checking "${test_file}", threads: "${i}"..."
        ../src/lab3 "-"${i} -w < ${test_file}
        ret=$?
        if [ ${ret} -ne 0 ]; then
          $log_error "Failed to run test"  
        return 1
        fi
    done
  done
}

main $@
