#!/bin/bash

DIR="."

COLOR_RESET="\033[0m"
COLOR_RED="\033[31m"
COLOR_BRIGHT_GREEN="\033[92m"
BOLD="\e[1m"
NORMAL="\e[0m"

if [[ -z "$1" ]]; then
  echo -e "${COLOR_BRIGHT_GREEN}Tip: Specify a directory name to run script locally.${COLOR_RESET}"
else
  DIR=$1
fi

FIND_FILES_CMD="find \"$DIR\" \
    \( -path \"*test*\" -o -path \"*build*\" \) -prune -false -o \
    \( -iname \"*.cpp\" -o -iname \"*.hpp\" \)"

FILES=$( eval "$FIND_FILES_CMD" | xargs)

echo "Files staged for format checking: "
echo "$(eval $FIND_FILES_CMD)"

echo "Running format check..."
RET=$(clang-format --dry-run $FILES |& grep violations | wc -l)

if [ "$RET" = 0 ]; then # grep failed -> no lines were returned by clang-format
  echo -e "${BOLD}${COLOR_BRIGHT_GREEN}No errors ${COLOR_RESET}${NORMAL}found in $(echo "$FILES" | wc -w) files."
  exit 0;
else
  echo -e "${BOLD}${COLOR_RED}$RET errors ${COLOR_RESET}${NORMAL}found in $(echo "$FILES" | wc -w) files."
  exit 1;
fi
