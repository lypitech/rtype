#!/usr/bin/env bash

COLOR_RESET="\033[0m"
COLOR_RED="\033[31m"
COLOR_BRIGHT_GREEN="\033[92m"
BOLD="\e[1m"
NORMAL="\e[0m"



declare -a DIRS
VERBOSE="false"
FLAG="-n"

for arg in $@; do

  if [[ "$arg" == -* ]]; then
    if [[ "$arg" == "-v" ]]; then
      VERBOSE="true"
    else
      FLAG="$arg"
    fi
  else
    DIRS+=("$arg")
  fi

done

if [[ "$FLAG" == "-h" ]]; then
  echo -e "USAGE: $0 [-h | -v | -n | -i] <directories>";
  echo -e "  <directories> the list of directories to run the script for. Separated by spaces. ${COLOR_BRIGHT_GREEN}Default: .${NORMAL}"
  echo -e "  -h Display this help"
  echo -e "  -v Verbose the error checking"
  echo -e "  -n use the -n (--dry-run) flag on clang-format ${COLOR_BRIGHT_GREEN}Activated by default${NORMAL}"
  echo -e "  -i format the staged files"
  echo -e "You will be able to find a report in the violations.report.log file."
  exit 0;
fi

if [[ ${#DIRS[@]} -eq 0 ]]; then
  DIRS+=(".");
  echo -e "${COLOR_BRIGHT_GREEN}Tip: Specify a directory name or more to run script locally.${COLOR_RESET}"
fi

declare -a FILES
mapfile -t FILES < <(find "${DIRS[@]}" \
    \( -path "*build*" \) -prune -false -o \
    \( -iname "*.cpp" -o -iname "*.hpp" \))

if [ "${#FILES[@]}" -eq 0 ]; then
    echo "No file found."
    exit 0
fi

echo "Files staged for format checking:"
printf " -> %s\n" "${FILES[@]}"

echo "Running format check..."
OUTPUT=$(clang-format $FLAG "${FILES[@]}" 2>&1)
if [[ "$VERBOSE" == "true" ]]; then
  clang-format -n "${FILES[@]}"
else
  clang-format -n "${FILES[@]}" &> violations.report.log
fi
RET=$(echo "$OUTPUT" | grep -c "violations")

NUM_FILES=${#FILES[@]}

if [ "$RET" -eq 0 ]; then
  echo -e "${BOLD}${COLOR_BRIGHT_GREEN}No errors${COLOR_RESET}${NORMAL} found in $NUM_FILES files."
  exit 0
else
  echo -e "${BOLD}${COLOR_RED}$RET errors${COLOR_RESET}${NORMAL} found in $NUM_FILES files."
  exit 1
fi
