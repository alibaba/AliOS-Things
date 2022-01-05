#!/bin/bash -e

# from git-sh-setup.sh
require_clean_work_tree () {
    git rev-parse --verify HEAD >/dev/null || exit 1
    git update-index -q --ignore-submodules --refresh
    err=0

    if ! git diff-files --quiet --ignore-submodules
    then
        echo >&2 "Cannot $0: You have unstaged changes."
        err=1
    fi

    if ! git diff-index --cached --quiet --ignore-submodules HEAD --
    then
        if [ $err = 0 ]
        then
            echo >&2 "Cannot $0: Your index contains uncommitted changes."
        else
            echo >&2 "Additionally, your index contains uncommitted changes."
        fi
        err=1
    fi

    if [ $err = 1 ]
    then
        test -n "$2" && echo >&2 "$2"
        exit 1
    fi
}

require_clean_work_tree

if ! which doxygen >/dev/null; then
    echo "Error: doxygen is required"
    exit 1
fi

DOXYGEN_VERSION="$(doxygen --version)"

DOC_BRANCH="gh-pages"
ORIG_BRANCH="$(git rev-parse --abbrev-ref HEAD)"
ORIG_COMMIT="$(git describe --match=NeVeRmAtCh --always --abbrev=40 --dirty)"

TOP="$(pwd)"
export GIT_DIR="$TOP/.git"

TMPDIR="$(mktemp --tmpdir=$TOP -d)"
HTMLDIR="$TMPDIR/_build/html"
INDEX_FILE="$GIT_DIR/index.${DOC_BRANCH}"

rm -f "$INDEX_FILE"

trap "{ cd $TOP; git checkout --force ${ORIG_BRANCH}; rm -f $INDEX_FILE; rm -rf $TMPDIR; }" EXIT

cd "$TMPDIR"
git reset --hard HEAD

./autogen.sh
mkdir _build
cd _build
../configure
make html

if ! git checkout "${DOC_BRANCH}"; then
    git checkout --orphan "${DOC_BRANCH}"
fi

touch "$HTMLDIR/.nojekyll"
echo lib.protobuf-c.io > "$HTMLDIR/CNAME"

GIT_INDEX_FILE="$INDEX_FILE" GIT_WORK_TREE="$HTMLDIR" \
    git add --no-ignore-removal .
GIT_INDEX_FILE="$INDEX_FILE" GIT_WORK_TREE="$HTMLDIR" \
    git commit -m "Rebuild html documentation from commit ${ORIG_COMMIT} using Doxygen ${DOXYGEN_VERSION}"
