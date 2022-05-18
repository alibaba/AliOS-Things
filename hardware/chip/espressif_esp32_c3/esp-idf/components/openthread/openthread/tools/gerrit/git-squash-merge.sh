#!/bin/bash
#
#  Copyright (c) 2018, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

die()
{
    echo " *** ERROR: " "$*"
    exit 1
}

display_usage()
{
    echo "Squash merge a given branch into the current branch"
    echo ""
    echo "This command squash merges all commits from a given branch into the current branch"
    echo "By default, the changes are committed with a commit message which includes the list of all squashed commits"
    echo "(use --no-commit and --no-list options to change default behavior)."
    echo ""
    echo "Usage: $(basename "$0") [--no-list] [--no-commit] <branch> [<commit msg>]"
    echo ""
    echo "     <branch>       Specifies the name of branch to merge into current branch"
    echo "     <commit msg>   An optional parameter specifying text to add to the commit message"
    echo "Options:"
    echo "     --no-list      The commit message will not include the list of squashed commits"
    echo "     --no-commit    Squash and stage the changes but do not commit "
    echo ""
}

SHOULD_ADD_LIST=true
SHOULD_COMMIT=true

while test $# != 0; do
    case "$1" in
        --help | -h | -[?])
            display_usage
            exit 0
            ;;

        --no-list)
            SHOULD_ADD_LIST=false
            ;;
        --no-commit)
            SHOULD_COMMIT=false
            ;;
        --debug) set -x ;;
        --*) die "Unknown argument $1" ;;
        -*) die "Unknown argument $1" ;;
        *)
            break
            ;;
    esac
    shift
done

if [ "$#" -eq 0 ]; then
    display_usage
    die "No branch name"
fi

if [ "$#" -gt 2 ]; then
    display_usage
    die "Extra argument"
fi

NEWLINE=$'\n'

branch="$1"
cur_branch=$(git rev-parse --abbrev-ref HEAD)

# Get the list of commits (diff between current and new branch)
# Note that the list starts with older commits

if ${SHOULD_ADD_LIST}; then
    commit_list=$(git log HEAD.."$branch" --oneline --decorate=no --reverse)
else
    commit_list=""
fi

commit_msg_header="Squash merge '$branch' into '$cur_branch'"

if [ -z "$2" ]; then
    commit_msg="${commit_msg_header}${NEWLINE}${NEWLINE}${commit_list}"
else
    commit_msg="${commit_msg_header}${NEWLINE}${NEWLINE}$2${NEWLINE}${NEWLINE}${commit_list}"
fi

git merge --squash "$branch" 1>/dev/null 2>/dev/null || die "Failed to perform 'git merge -squash $branch'"

# Check if there is anything staged
if ! git diff --cached --quiet; then
    echo "No changes to commit when squash merging branch '$branch' into '$cur_branch'"
    exit 0
fi

if ${SHOULD_COMMIT}; then
    # Commit the staged changes
    git commit -m "$commit_msg" 1>/dev/null 2>/dev/null || die "git commit failed${NEWLINE}${NEWLINE}$(cat "$TMP")"

    git log -1

    echo "${NEWLINE}Successfully squash merged branch '$branch' into '$cur_branch'"
else
    echo "Successfully prepared squash merge of branch '$branch' into '$cur_branch' - ready to commit"
fi
