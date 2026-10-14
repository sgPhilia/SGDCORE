#!/usr/bin/env bash
set -euo pipefail

# Read-only preflight for a local SGDCORE checkout. This script deliberately
# does not initialize Git, stage files, commit, create tags, add remotes, or push.

if ! root="$(git rev-parse --show-toplevel 2>/dev/null)"; then
    cat >&2 <<'EOF'
This directory is not inside a Git repository. No files were changed.

Review the project first, then initialize and publish it manually using the
GitHub instructions at:
https://docs.github.com/en/repositories/creating-and-managing-repositories/creating-a-new-repository
EOF
    exit 1
fi

cd "$root"
printf 'SGDCORE local repository preflight\n'
printf 'Branch: '
git branch --show-current

if git remote | grep -q .; then
    printf 'Git remote: configured (remote address is not displayed)\n'
else
    printf 'Git remote: none configured\n'
fi

printf '\nWorking tree status:\n'
git status --short

printf '\nWhitespace check:\n'
git diff --check
git diff --cached --check
printf 'Passed.\n'

printf '\nThis was a read-only check. No files, commits, tags, remotes, or GitHub state were changed.\n'
