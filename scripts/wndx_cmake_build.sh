#!/bin/sh
## AUTHOR : github.com/WANDEX
## LICENSE: MIT
##
## wndx cmake build script & optionally run tests. (POSIX sh compliant)
##
## Script is meant only for the project developer and as the reference.
## No guarantees that it will work on your OS/platform!
## Use standard cmake commands for building the project.
## Or adapt script steps yourself for your personal preference.
##
## Manually export following environment variables
## or pass -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++
## to build/test under multiple compilers before submitting code!
##
## export CC=gcc   CXX=g++
## export CC=clang CXX=clang++  GENERATOR=Ninja
## export CC=cl    CXX=cl       GENERATOR='Visual Studio 17 2022'

set -e

OLDIFS="$IFS"

CMAKE="${CMAKE:-cmake}" # override cmake executable path via env var
CTEST="${CTEST:-ctest}"

at_path() { hash "$1" >/dev/null 2>&1 ;} # if $1 is found at $PATH -> return 0

## version update should reflect improvement or a functional change in the script logic.
## get_opt() change of the "OPTION DEFAULTS" should not lead to a version update.
## "OPTION DEFAULTS" - project specific, they may be changed between the projects freely.
## In other cases, version should be bumped, then updated script must be propagated
## to older versions of the script and changes must be merged except "OPTION DEFAULTS".
VERSION="1.3.2"
VERSION_DATE="2026-03-17" # update at each VERSION bump

bname="wndx_cmake_build.sh"; at_path basename && bname=$(basename "$0")
USAGE="\
Usage: $bname [OPTION...]
OPTIONS
    -D <var>[:<type>]=<value> create or update a cmake cache entry
    --get_build_dir           print build dir
    --get_project_name        print project name parsed from CMakeLists.txt
    --get_project_name_upper  print project name in uppercase
    -h, --help                print help
    -v, --version             print version
POSITIONAL
    x  , x_cache , --x_cache  rm CMakeCache.txt with cached variables
    c  , clean   , --clean    built-in cmake clean build
    cc , cleaner , --cleaner  built-in cmake clean configure & build
    ccc, cleanest, --cleanest rm build dir & build from scratch
    ctest, ct, ctp, ctr       (optional) arg - filter regex    (default: '.*')
    gtest, gt                 (optional) arg - filter wildcard (default:  '*')
                              special chars (:,*,?,-), ':' separator, '-' negative
EXAMPLES
./scripts/$bname
./scripts/$bname x
./scripts/$bname x c
./scripts/$bname c ct
./scripts/$bname c gt *
# ====== build & run tests:
./scripts/$bname ctest
# ====== make clean build & run tests:
./scripts/$bname clean ctest
# ====== make cleaner build (for edge cases) & run tests:
./scripts/$bname cleaner ctest
# ====== build & run filtered tests:
./scripts/$bname ctest .*regex.*
# ====== or
./scripts/$bname gtest *wildcard*
# ====== override default env vars
CC=clang CXX=clang++ BUILD_TYPE=Release COPTS='-Wno-dev -Wno-error=dev' ./scripts/$bname x
# ====== or export modified env vars
export CC=cl CXX=cl VERBOSE=1 DEPLOY=1 BUILD_TYPE=Debug GENERATOR='Visual Studio 17 2022'
./scripts/$bname c ct
"

printe() { # print to stderr
  at_path printf || exit 63
  if [ "$#" = 1 ]; then
    printf "%s\n" "$*" 1>&2
  else # shellcheck disable=SC2059 # intentional - re-split any amount of arguments.
    printf "$@" 1>&2
  fi
}

check_prerequisites() {
  if ! at_path "$CMAKE"; then
    printe "\$CMAKE='$CMAKE'- NOT VALID cmake executable"
    printe "'cmake' - REQUIRED DEPENDENCY NOT FOUND AT \$PATH! EXIT."
    exit 64
  fi
  if ! at_path "$CTEST"; then
    printe "\$CTEST='$CTEST'- NOT VALID ctest executable"
    printe "'ctest' - REQUIRED DEPENDENCY NOT FOUND AT \$PATH! EXIT."
    exit 65
  fi
  if [ ! -r ./CMakeLists.txt ]; then
    printe "%s %s\n" "Current directory does not contain CMakeLists.txt!" "EXIT."
    exit 66
  fi
  IFS=" " # split arguments by space
  deps="basename cut getopt printf sed tr uname"
  for executable in $deps; do
    if ! at_path "$executable"; then
      printe "'$executable' - REQUIRED DEPENDENCY NOT FOUND AT \$PATH! EXIT."
      exit 67
    fi
  done
  IFS="$OLDIFS" # restore
}

check_prerequisites

## do not override, on win10 in MINGW64:/git-bash shell
## $(uname -s)="MINGW64_NT-10.0-19045"
## overriding time() as time() leads to weird error!
## unset -f time # does not work as it is hardcoded built-in!
time_() { "$CMAKE" -E time "$@"           ;}

echo()  { "$CMAKE" -E echo_append "$*"    ;}
echon() { "$CMAKE" -E echo "$*"           ;}
false() { "$CMAKE" -E false               ;}
mkdir() { "$CMAKE" -E make_directory "$1" ;}
rm_f()  { "$CMAKE" -E rm  -f "$1"         ;}
rm_rf() { "$CMAKE" -E rm -rf "$1"         ;}
true()  { "$CMAKE" -E true                ;}

get_prj_name() {
  fn_re='\bproject\b' # function name regex
  pn_re='\b(\w+)\b' # project name regex group
  ws_ol='\s*?' # optional=*, lazy=?, \s=ws including '\n' etc.
  beg_re="${ws_ol}${fn_re}${ws_ol}\(?"
  end_re="${ws_ol}${pn_re}.*?\)?"
  rg_re="${beg_re}${end_re}"
  ## no comments - remove comments & delete empty lines with/without spaces
  no_co=$(sed "s/[[:space:]]*#.*$//g; /^[[:space:]]*$/d" ./CMakeLists.txt)
  if   at_path  rg; then
    # _span=$(echo "$no_co" | rg -U -or '$0' --trim "$rg_re")
    _prj_name=$(echo "$no_co" | rg -U -or '$1' --trim "$rg_re")
  elif at_path sed; then
    _span=$(echo "$no_co" | sed -E -n "/${beg_re}/,/\)? /p; s/^[[:blank:]]*//g") # blank = 'rg --trim'
    _prj_name=$(echo "$_span" | sed -E -e "s/${fn_re}\(//g;  /^[[:space:]]*$/d; s/\)//g")
  elif at_path git; then # not robust - clone dir != project name
    _prj_name=$(basename "$(git rev-parse --show-toplevel)")
  else
    exit 68
  fi
  echo "$_prj_name"
}

notify() {
  EC="$?" ## save error/exit code of the previously executed command
  at_path notify-send || return 0
  tag="string:x-dunst-stack-tag"
  sbg="string:bgcolor:"
  sfg="string:fgcolor:"
  arg="$2"
  ##
  case "$1" in
  error|ERROR)     urg="critical" ;;
  warning|WARNING) urg="normal"   ;;
  *)
    arg="$1"
    urg="low"
    sbg="string:bgcolor:#11FF44"
    sfg="string:fgcolor:#000000"
  ;;
  esac
  notify-send -u "$urg" -h "$tag:$PRJ_NAME" -h "$tag:hi" -h "$sbg" -h "$sfg" "[$PRJ_NAME]" "$arg"
}

pre_configure() {
  ## cmake verbosity level
  _verbose=""
  _cmake_log_level=""
  if [ "$VERBOSE" = 1 ]; then
    _verbose="--verbose"
    _cmake_log_level="--log-level=DEBUG"
  fi
  ## cmake clean options
  _fresh=""
  _clean_first=""
  ## x_cache - rm CMakeCache.txt with cached variables.
  if [ "$RM_CMAKECACHE" = 1 ]; then
    [ -f "$BUILD_DIR/CMakeCache.txt" ] && rm_f "$BUILD_DIR/CMakeCache.txt"
  fi
  ## clean - built-in cmake clean build step.
  if [ "$CLEAN" = 1 ]; then
    _clean_first="--clean-first"
  fi
  ## cleaner - build using built-in cmake clean options, for both: configure & build step.
  if [ "$CLEANER" = 1 ]; then
    _fresh="--fresh"
    _clean_first="--clean-first"
  fi
  ## cleanest - wipe whole build dir, build from scratch.
  if [ "$CLEANEST" = 1 ]; then
    [ -d "$BUILD_DIR" ] && rm_rf "$BUILD_DIR"
  fi
  [ -d "$BUILD_DIR" ] || mkdir "$BUILD_DIR"
  ## source and build dirs provided as the relative paths
  _sdir="."
  _bdir="$BUILD_DIR"
  prefix_deploy="$(pwd)/$BUILD_DIR/deploy" # must be an absolute path for Qt deploy etc.
  if [ "$REL" = 1 ]; then
    cd "$BUILD_DIR" || exit 11
    pwd # cd - cmd visual feedback about internal behavior
    _sdir="../../.."
    _bdir="."
  fi
}

trailing_args() {
  targc="${#}" # arguments count
  targv="${*}" # arguments concatenated string
  [ -z "$targv" ] && return
  argn=0
  set -o noglob # disable (wildcard expansion)
  IFS=" " # split arguments by space
  _dbg_trailing_args=0 # debug
  for arg in $targv; do
    argn=$((argn+1))
    if [ $_dbg_trailing_args = 1 ]; then
      printe "\ntargc=%d targv='%s' argv[%d]='%s'\n" "$targc" "$targv" "$argn" "$arg"
    fi
    if [ -n "$arg_type" ]; then # => filter pattern: ctest regex or gtest wildcard
      TESTS_FILTER="$arg"
      arg_type="" # set tmp var type - back to empty
      continue
    fi
    case "$arg" in
    ctest|ctp|ctr|ct|gtest|gt)
      arg_type="$arg" # tmp var type
      RUN_TESTS=1
      RUN_TESTS_TYPE="$arg_type"
      ## test filter pattern will be set ON NEXT LOOP ITERATION! => next $arg is filter (optional).
      case "$arg_type" in gtest|gt) TESTS_FILTER='*' ;; esac # gtest - default wildcard
      continue
    ;;
    cleanest|ccc)
      CLEANEST=1
      continue
    ;;
    cleaner|cc)
      CLEANER=1
      continue
    ;;
    clean|c)
      CLEAN=1
      continue
    ;;
    x_cache|x)
      RM_CMAKECACHE=1
      continue
    ;;
    *)
      targv=$(echo "$targv" | cut -d ' ' -f "${argn}-")
      printe "unparsed trailing args[%d]=[%s]\nunsupported arg: [%s]\n" "$targc" "$targv" "$arg"
      exit 6
    ;;
    esac
  done
  set +o noglob # restore - enable (wildcard expansion)
  IFS="$OLDIFS" # restore
  if [ $_dbg_trailing_args = 1 ]; then
    printe "CLEAN=$CLEAN CLEANER=$CLEANER CLEANEST=$CLEANEST"
    printe "RUN_TESTS_TYPE=$RUN_TESTS_TYPE TESTS_FILTER=$TESTS_FILTER"
  fi
}

get_opt() {
  ## Parse and read OPTIONS command-line options
  SHORT=D:hv
  LONG=x_cache,clean,cleaner,cleanest,get_build_dir,get_project_name,get_project_name_upper,help,version
  OPTIONS=$(getopt --options $SHORT --long $LONG --name "$0" -- "$@")
  ## PLACE FOR OPTION DEFAULTS BEG
  BUILD_TYPE="${BUILD_TYPE:-Debug}"
  GENERATOR="${GENERATOR:-Ninja}" # "MSYS Makefiles", "Unix Makefiles", "Ninja Multi-Config", Ninja
  TESTS_DIR="${TESTS_DIR:-tests/units}"
  MEMCHECK="${MEMCHECK:-0}"
  VERBOSE="${VERBOSE:-0}"
  DEPLOY="${DEPLOY:-0}"
  REL="${REL:-0}"
  ##
  RM_CMAKECACHE=0
  CLEAN=0
  CLEANER=0
  CLEANEST=0
  ##
  RUN_TESTS=0
  RUN_TESTS_TYPE=''
  TESTS_FILTER='.*'
  ##
  _compiler="${CC:-_}"
  cmbn=$(basename "$_compiler") # get compiler basename in case declared via full path
  gen_dir_name=$(echo "$GENERATOR" | sed "s/[ |-]/_/g" | tr "[:upper:]" "[:lower:]")
  arch=$(uname -m)
  BUILD_DIR="build/${gen_dir_name}/${arch}-${cmbn}"
  case "$GENERATOR" in # v Multi-Config Generator
  *"Multi-Config"*|"Xcode"|"Visual Studio"*|"Green Hills MULTI") ;;
  *) BUILD_DIR="${BUILD_DIR}-${BUILD_TYPE}" ;; # else append build type
  esac
  BUILD_DIR=$(echo "$BUILD_DIR" | tr "[:upper:]" "[:lower:]") # force lowercase build dir path
  ##
  PRJ_NAME=$(get_prj_name)
  PRJ_NAME_UPPER=$(echo "$PRJ_NAME" | tr "[:lower:]" "[:upper:]")
  PRJ_BUILD_TESTS="${BUILD_TESTS_OPT:-${PRJ_NAME_UPPER}_BUILD_TESTS}"
  PRJ_MEMCHECK_EN="${MEMCHECK_OPT:-${PRJ_NAME_UPPER}_MEMCHECK_ENABLE}"
  ##
  COPTS=\
"-D CMAKE_C_COMPILER='$CC'
 -D CMAKE_CXX_COMPILER='$CXX'
 -D CMAKE_BUILD_TYPE=$BUILD_TYPE
 -D $PRJ_BUILD_TESTS=ON
 -D $PRJ_MEMCHECK_EN=ON
 $COPTS
" ## <- default cmake configure options
  BOPTS=\
"$BOPTS
" ## <- default cmake build options
  ## PLACE FOR OPTION DEFAULTS END
  _dbg=0 # debug
  eval set -- "$OPTIONS"
  while true; do
    case "$1" in
    --x_cache)
      RM_CMAKECACHE=1
    ;;
    --clean)
      CLEAN=1
    ;;
    --cleaner)
      CLEANER=1
    ;;
    --cleanest)
      CLEANEST=1
    ;;
    -D)
      shift
      COPTS="$COPTS -D $1"
    ;;
    --get_build_dir)
      echo "$BUILD_DIR"
      exit 0
    ;;
    --get_project_name)
      echo "$PRJ_NAME"
      exit 0
    ;;
    --get_project_name_upper)
      echo "$PRJ_NAME_UPPER"
      exit 0
    ;;
    -h|--help)
      echon "$USAGE"
      exit 0
    ;;
    -v|--version)
      VERSION_STRING="$bname: WNDX_CMAKE_BUILD [$PRJ_NAME] version $VERSION_DATE@$VERSION"
      echon "$VERSION_STRING"
      exit 0
    ;;
    --)
      shift
      trailing_args "$*"
      break
    ;;
    esac
    shift
  done
  ## join string using only single space delimiter
  COPTS=$(echo "$COPTS" | tr -s '[:space:]' ' ')
  BOPTS=$(echo "$BOPTS" | tr -s '[:space:]' ' ')
  if [ $_dbg = 1 ]; then
    printe "COPTS: $COPTS"
    printe "BOPTS: $BOPTS"
    exit 33
  fi
}

## https://cmake.org/cmake/help/latest/variable/CMAKE_LINKER_TYPE.html
use_linker_mold() {
  case "$COPTS" in # if explicitly provided - do not override
  *"CMAKE_LINKER_TYPE"*) return ;;
  esac
  if ! at_path mold; then
    printe "%b%s%b\n" "${YEL}" "mold linker not found at \$PATH!" "${END}"
    return
  fi
  pl=$(uname)
  if [ "$pl" = Darwin ]; then
    case "$CC" in *"clang"*)
      COPTS="$COPTS -D CMAKE_LINKER_TYPE=MOLD"
    ;;
    esac
  elif [ "$pl" = Linux ]; then
    case "$CC" in *"clang"*|*"gcc"*)
      COPTS="$COPTS -D CMAKE_LINKER_TYPE=MOLD"
    ;;
    esac
  fi
}

## on win10 in MINGW64:/git-bash shell trailing args via '$*' - works; '$@' - NOT works;
## ^ apparently in this environment - this function is still not always finds tests...
# shellcheck disable=SC2048,SC2068,SC2086 # intentional - re-split trailing arguments.
run_ctest() {
  time_ "$CTEST" --build-config "$BUILD_TYPE" --test-dir "$_bdir/$TESTS_DIR" \
--output-on-failure "$@" \
|| { notify ERROR "RUN CTEST ERROR"; exit "$EC" ;}
}

SEP="=============================================================================="
vsep() { printf "\n%b%.78s%b\n\n" "${2}" "[${1}]${SEP}" "${END}" ;}

## MAIN
get_opt "$@"
use_linker_mold
pre_configure

vsep "CONFIGURE" "${BLU}"
# shellcheck disable=SC2086 # intentional - re-split OPTIONS CONFIGURE
time_ "$CMAKE" -S "$_sdir" -B "$_bdir" -G "$GENERATOR" -Wdev -Werror=dev \
${_fresh} ${_cmake_log_level} ${COPTS} \
|| { notify ERROR "CMAKE CONFIGURE ERROR"; exit "$EC" ;}

vsep "BUILD" "${CYN}"
# shellcheck disable=SC2086 # intentional - re-split OPTIONS BUILD
time_ "$CMAKE" --build "$_bdir" --config "$BUILD_TYPE" ${_clean_first} ${_verbose} ${BOPTS} \
|| { notify ERROR "CMAKE BUILD ERROR"; exit "$EC" ;}

if [ "$RUN_TESTS" = 1 ]; then
  vsep "TESTS" "${RED}"
  case "$RUN_TESTS_TYPE" in
  ct|ctest) # optionally filter by regex
    run_ctest -R "$TESTS_FILTER"
  ;;
  ctp) # shortcut for terse progress output
    run_ctest --progress -R "$TESTS_FILTER"
  ;;
  ctr) # suppress stdout/stderr, EC -> always success (to not immediately exit because of 'set -e')
    run_ctest -R "$TESTS_FILTER" >/dev/null 2>&1 || true
    run_ctest --rerun-failed # rerun previously failed - (filtered) results, with only failed tests.
  ;;
  gt|gtest)
    gtest_binary="./$_bdir/bin/tests_units"
    if [ ! -x "$gtest_binary" ]; then
      printe "%s\n^ %s\n" "$gtest_binary" "File not found or not executable, exit."
      exit 5
    fi
    "$gtest_binary" --gtest_filter="$TESTS_FILTER"
  ;;
  esac
fi

has_debug_info=0
if [ "$BUILD_TYPE" = Debug ] || [ "$BUILD_TYPE" = RelWithDebInfo ]; then
  has_debug_info=1
fi

if [ "$MEMCHECK" = 1 ] && [ "$has_debug_info" = 1 ]; then
  vsep "MEMCHECK" "${YEL}"
  time_ "$CMAKE" --build "$_bdir" --config "$BUILD_TYPE" --target memcheck
fi

if [ "$DEPLOY" = 1 ]; then
  vsep "DEPLOY" "${MAG}"
  printe "%s %s\n" "--prefix" "$prefix_deploy"
  time_ "$CMAKE" --install "$_bdir" --config "$BUILD_TYPE" --prefix "$prefix_deploy"
fi

vsep   "COMPLETED" "${GRN}"
notify "COMPLETED"
