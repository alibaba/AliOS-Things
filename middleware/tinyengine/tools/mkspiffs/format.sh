#!/usr/bin/env bash
# Runs astyle with the full set of formatting options
astyle \
	--style=otbs \
	--indent=spaces=4 \
	--convert-tabs \
	--keep-one-line-statements \
	--pad-header \
	--pad-oper \
	"$@"
