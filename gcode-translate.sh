#!/bin/zsh
# Translates all G-Code objects in the X-Y plane to be in the centre of the specified print area
# NOTE: This assumes that all gcode values are absolute
# TODO: Segment gcode file based on absolute and relative values

# Tunable parameters (EDIT THESE!)
_MINX=0
_MINY=0
_MAXX=150
_MAXY=150

_OUTFILE='%s-translated.%s'

# TODO getopts

# No need for further editing
_SIZEX=$(( _MAXX - _MINX ))
_SIZEY=$(( _MAXY - _MINY ))
_CX=$(( _MINX + _SIZEX / 2 ))
_CY=$(( _MINY + _SIZEY / 2 ))

# Message reporting
function echof() { printf "%s: \033[${1};${2}m${3}\033[0m %s\n" "${4}" "${@:5}" >&2 }

function errmsg() { echof 1 31 '[ERROR]' "${1:t}" "${@:2}" >&2 }
function warn() { echof 1 35 '[WARNING]' "${1:t}" "${@:2}" }
function notice() { echof 1 34 '[NOTICE]' "${1:t}" "${@:2}" }
function msg() { echof 1 1 '[MESSAGE]' "${1:t}" "${@:2}" }

# Helper functions
function formatOutfile() { printf "${_OUTFILE}\n" "${1:r}" "${${1:e}:-gcode}"; }

_TMPFILE="$(mktemp -q)"
_TMPFILE2="$(mktemp -q)"
trap 'rm -f ${_TMPFILE} ${_TMPFILE2}' ERR_EXIT EXIT INT HUP

# The main iterator
function translateFile() {
	local _filename="${1}"
	notice "${_filename}" "Processing file name \"${_filename:t}\""
	[[ ! -e "${_filename}" ]] && {errmsg "${_filename}" "Cannot find file." ; return 1}

	local _outfile="$(formatOutfile "${_filename}")" 
	notice "${_filename}" "Output filename: \"${_outfile:t}\""

	# Get G1
	msg "${_filename}" "Finding G1 sequences"
	grep '^G1.\+[XY]' "${_filename}" >| "${_TMPFILE}"

	if grep -q '\s\+X[0-9\.\-]\+\s\+Y[0-9\.\-]' "${_TMPFILE}"; then
		notice "${_filename}" "Normally formatted gcode file. Using tricks"

	else
		notice "${_filename}" ""
	fi

	# Get X values
	msg "${_filename}" "Getting X axis positions"
	grep '^G1' "${_TMPFILE}" >| "${_TMPFILE2}"

	# Get G1 fields of X, Y independently,
	# Check size of each file won't exceed bounds of printbed
	# Calculate delta for X, Y
	# Format g-code for each delta
}

# Process each file passed as an argument
while [[ ! -z "${1}" ]]; do
	translateFile "${1}"
	shift
done
